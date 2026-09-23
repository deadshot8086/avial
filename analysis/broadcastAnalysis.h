#ifndef BROADCAST_ANALYSIS_H
#define BROADCAST_ANALYSIS_H

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>
#include <algorithm>
#include "arrayPartitionAnalysis.h"

#include "includes/dhirOps.h"
#include "includes/dhirDialect.h"


namespace mlir
{
    namespace dhir
    {
    
        // ========================================================================
        // BROADCAST ANALYSIS
        // ========================================================================

        /// Structure to hold broadcast analysis results
        struct BroadcastInfo
        {
            Value memref;                    // The memref being analyzed
            bool needsBroadcast;             // Whether broadcast is needed
            std::string reason;              // Reason for the decision
            ArrayPartitioningInfo partInfo;  // Partitioning info from subsequent usage
            
        };

        /// Helper class for broadcast analysis
        class BroadcastAnalysis
        {
        public:
            BroadcastAnalysis(mlir::Operation *replicateOp) : replicateOp(replicateOp) {}

            /// Perform the broadcast analysis
            bool analyze(std::vector<BroadcastInfo> &results)
            {
                results.clear();

                // Get all write arguments from this replicate operation

                llvm::SmallVector<Value> writeArgs = mlir::dyn_cast<mlir::dhir::ReplicateOp>(replicateOp).getWrites();

                if (writeArgs.empty()) {
                    llvm::errs() << "No write arguments found in replicate operation\n";
                    return true; // Not an error, just nothing to analyze
                }

                llvm::errs() << "\n=== Broadcast Analysis ===\n";
                llvm::errs() << "Analyzing " << writeArgs.size() << " write arguments\n";

                // For each write argument, check if it needs broadcasting
                for (Value writeArg : writeArgs) {
                    BroadcastInfo info;
                    info.memref = writeArg;
                    info.needsBroadcast = false;
                    info.reason = "No subsequent usage found";

                    llvm::errs() << "\n--- Analyzing write argument ---\n";

                    // A reader outside every replicate runs on all ranks and
                    // reads the whole buffer, so the output must be
                    // materialized.  Checked first: overrides shard contracts.
                    if (hasLaterUndistributedReader(writeArg)) {
                        info.needsBroadcast = true;
                        info.reason = "read by undistributed code after this "
                                      "replicate; materialize output";
                        llvm::errs() << "  ✓ BROADCAST NEEDED: " << info.reason
                                     << "\n";
                        results.push_back(info);
                        continue;
                    }

                    // Find subsequent replicate operations that read this output
                    llvm::SmallVector<mlir::Operation *> subsequentReplicates =
                        findSubsequentReplicates(writeArg);

                    if (subsequentReplicates.empty()) {
                        llvm::errs() << "  No subsequent replicate operations found\n";
                        info.reason = "No subsequent replicate operations use this output";
                        results.push_back(info);
                        continue;
                    }

                    llvm::errs() << "  Found " << subsequentReplicates.size() 
                                << " subsequent replicate(s)\n";

                    // Consumer partitioning alone doesn't prove the producer's
                    // shard ranges/axis/bounds match, so the default is to
                    // materialize the producer's output.  Only skip the transfer
                    // when every consumer reads exactly the slab the producer
                    // wrote (shardContractMatches); anything unknown falls back
                    // to broadcast.
                    ArrayPartitioningInfo producerInfo =
                        analyzeArrayForPartitioning(replicateOp, writeArg);
                    LoopBounds producerBounds = partitionedLoopBounds(replicateOp);

                    bool needsAnyBroadcast = !subsequentReplicates.empty();
                    bool allContractsMatch = !subsequentReplicates.empty();
                    ArrayPartitioningInfo firstPartInfo{};
                    bool havePartInfo = false;
                    for (mlir::Operation *nextReplicate : subsequentReplicates)
                    {
                        ArrayPartitioningInfo partInfo =
                            analyzeArrayForPartitioning(nextReplicate, writeArg);
                        if (!havePartInfo)
                        {
                            firstPartInfo = partInfo;
                            havePartInfo = true;
                        }
                        needsAnyBroadcast |= needsBroadcast(partInfo);
                        allContractsMatch &= shardContractMatches(
                            producerInfo, partInfo, producerBounds,
                            partitionedLoopBounds(nextReplicate));
                    }

                    info.partInfo = firstPartInfo;

                    if (needsAnyBroadcast && allContractsMatch) {
                        info.needsBroadcast = false;
                        info.reason =
                            "every consumer reads exactly this producer's shard "
                            "(same axis, no halo, identical bounds)";
                        llvm::errs() << "  ✗ NO BROADCAST: " << info.reason << "\n";
                    } else if (needsAnyBroadcast) {
                        info.needsBroadcast = true;
                        info.reason = "producer/consumer shard contract is not proven; materialize output";
                        llvm::errs() << "  ✓ BROADCAST NEEDED: " << info.reason << "\n";
                    } else {
                        info.needsBroadcast = false;
                        info.reason = "No subsequent consumer";
                        llvm::errs() << "  ✗ NO BROADCAST: " << info.reason << "\n";
                    }

                    results.push_back(info);
                }

                llvm::errs() << "\n=== Analysis Complete ===\n";
                llvm::errs() << "Total decisions: " << results.size() << "\n";
                int broadcastCount = 0;
                for (const auto &info : results) {
                    if (info.needsBroadcast) broadcastCount++;
                }
                llvm::errs() << "Broadcast needed for: " << broadcastCount << " outputs\n\n";

                return true;
            }

        private:
            mlir::Operation *replicateOp;

            // Undistributed readers.
            //
            // affine-to-dhir leaves loop-carried and unanalyzable loops
            // undistributed, so they never become a ReplicateOp yet run on
            // every rank and read the whole output.  A missed reader consumes
            // another rank's uninitialized shard, and a value derived from it
            // (a trip count, a branch condition) can then differ per rank and
            // make ranks disagree on collective counts -- an MPI deadlock.
            // So any reader outside a replicate that may run after this one
            // forces the output to be materialized, and unknowns count as
            // readers: over-materializing costs one redundant transfer,
            // under-materializing costs correctness.

            // The op whose region encloses all code that could observe this
            // output: the schedule when there is one, else the outermost
            // non-module ancestor.  findSubsequentReplicates only searches the
            // immediate parent, which loses every sibling of an enclosing loop.
            static mlir::Operation *analysisScope(mlir::Operation *op)
            {
                mlir::Operation *best = op->getParentOp();
                for (mlir::Operation *p = best; p; p = p->getParentOp())
                {
                    if (mlir::isa<mlir::dhir::ScheduleOp>(p))
                        return p;
                    if (!mlir::isa<mlir::ModuleOp>(p))
                        best = p;
                }
                return best;
            }

            static bool isViewLike(mlir::Operation *op)
            {
                return mlir::isa<mlir::memref::SubViewOp, mlir::memref::CastOp,
                                 mlir::memref::ReinterpretCastOp,
                                 mlir::memref::ExpandShapeOp,
                                 mlir::memref::CollapseShapeOp,
                                 mlir::memref::TransposeOp,
                                 mlir::memref::ViewOp>(op);
            }

            // `root` plus every view derived from it: a load through a subview
            // reads the underlying buffer just as much as a direct load does.
            static void collectAliases(mlir::Value root,
                                       llvm::SmallVectorImpl<mlir::Value> &aliases)
            {
                aliases.push_back(root);
                for (unsigned i = 0; i < aliases.size(); ++i)
                    for (mlir::Operation *user : aliases[i].getUsers())
                        if (isViewLike(user))
                            for (mlir::Value res : user->getResults())
                                if (mlir::isa<mlir::MemRefType>(res.getType()) &&
                                    !llvm::is_contained(aliases, res))
                                    aliases.push_back(res);
            }

            // Does `op` read the *contents* of `v`?  Stores and the ops that
            // only inspect metadata do not; a view does not (its own users are
            // visited separately as aliases); anything else holding the buffer
            // is assumed to read it.
            static bool readsBuffer(mlir::Operation *op, mlir::Value v)
            {
                if (auto load = mlir::dyn_cast<mlir::memref::LoadOp>(op))
                    return load.getMemRef() == v;
                if (auto load = mlir::dyn_cast<mlir::affine::AffineLoadOp>(op))
                    return load.getMemRef() == v;
                if (auto copy = mlir::dyn_cast<mlir::memref::CopyOp>(op))
                    return copy.getSource() == v;
                if (mlir::isa<mlir::memref::StoreOp, mlir::affine::AffineStoreOp>(op))
                    return false;
                if (isViewLike(op))
                    return false;
                if (mlir::isa<mlir::memref::DimOp, mlir::memref::DeallocOp,
                              mlir::memref::AllocOp, mlir::memref::AllocaOp>(op))
                    return false;
                // A replicate's own operand list is a declaration, not a read;
                // its body is visited on its own and handled by the shard
                // contract below.
                if (mlir::isa<mlir::dhir::ReplicateOp>(op))
                    return false;
                for (mlir::Value operand : op->getOperands())
                    if (operand == v)
                        return true;
                return false;
            }

            static bool insideReplicate(mlir::Operation *op)
            {
                for (mlir::Operation *p = op->getParentOp(); p; p = p->getParentOp())
                    if (mlir::isa<mlir::dhir::ReplicateOp>(p))
                        return true;
                return false;
            }

            // The ancestor of `op` that sits directly in `block`, else null.
            static mlir::Operation *ancestorInBlock(mlir::Operation *op,
                                                    mlir::Block *block)
            {
                while (op && op->getBlock() != block)
                    op = op->getParentOp();
                return op;
            }

            // May `other` execute after `anchor`?  Textual order is not enough:
            // a loop enclosing both means an op *before* `anchor` still runs
            // again after it, and two ops in different regions of one parent
            // (scf.while before/after) are ordered by that parent, not by
            // position.  Unknown relationships answer yes.
            static bool mayExecuteAfter(mlir::Operation *anchor,
                                        mlir::Operation *other)
            {
                if (anchor == other || anchor->isProperAncestor(other))
                    return false;

                for (mlir::Operation *p = anchor->getParentOp(); p;
                     p = p->getParentOp())
                    if (mlir::isa<mlir::scf::ForOp, mlir::scf::WhileOp,
                                  mlir::scf::ParallelOp,
                                  mlir::affine::AffineForOp>(p) &&
                        p->isProperAncestor(other))
                        return true;

                for (mlir::Operation *a = anchor; a; a = a->getParentOp())
                {
                    mlir::Block *b = a->getBlock();
                    if (!b)
                        break;
                    mlir::Operation *o = ancestorInBlock(other, b);
                    if (!o)
                        continue;
                    if (o == a)
                        return true; // sibling region of a shared parent
                    return a->isBeforeInBlock(o);
                }
                return true;
            }

            // Is there a reader of this output outside every replicate that may
            // run after this one?
            bool hasLaterUndistributedReader(mlir::Value memref)
            {
                llvm::SmallVector<mlir::Value> aliases;
                collectAliases(memref, aliases);

                mlir::Operation *scope = analysisScope(replicateOp);
                if (!scope)
                    return true;

                bool found = false;
                scope->walk([&](mlir::Operation *op) {
                    if (found)
                        return mlir::WalkResult::interrupt();
                    if (insideReplicate(op))
                        return mlir::WalkResult::advance();
                    bool reads = false;
                    for (mlir::Value alias : aliases)
                        if (readsBuffer(op, alias))
                        {
                            reads = true;
                            break;
                        }
                    if (!reads)
                        return mlir::WalkResult::advance();
                    if (!mayExecuteAfter(replicateOp, op))
                        return mlir::WalkResult::advance();
                    found = true;
                    return mlir::WalkResult::interrupt();
                });
                return found;
            }

            // Find subsequent replicate operations that use the given memref
            llvm::SmallVector<mlir::Operation *> findSubsequentReplicates(Value memref)
            {
                llvm::SmallVector<mlir::Operation *> result;

                // Get the parent operation (e.g., function, module)
                mlir::Operation *parentOp = replicateOp->getParentOp();
                if (!parentOp)
                    return result;

                bool foundCurrentReplicate = false;

                // Walk through all operations in the parent
                
                parentOp->walk([&](mlir::dhir::ReplicateOp op) {
                    // Skip until we find the current replicate operation
                    if (op == replicateOp) {
                        foundCurrentReplicate = true;
                        return;
                    }

                    // Only look at operations after the current replicate
                    if (!foundCurrentReplicate)
                        return;

                    if (replicateOp->isProperAncestor(op.getOperation()))
                        return;

                    // Check if this operation is a replicate that consumes our memref
                    if (isConsumedByReplicate(op, memref)) {
                        result.push_back(op);
                    }
                });

                return result;
            }

            // Does a later replicate consume `memref` — as a read OR a write?
            //
            // An in-place reduction records its accumulator as a write with
            // zero reads (read-modify-write, e.g. sad's second stage), yet it
            // still needs the producer's complete output.  Treating writes as
            // consumption is conservative: it only costs a redundant transfer,
            // never correctness.
            bool isConsumedByReplicate(mlir::Operation *replicate, Value memref)
            {
                auto rep = mlir::dyn_cast<mlir::dhir::ReplicateOp>(replicate);
                if (!rep)
                    return false;
                for (Value readArg : rep.getReads()) {
                    if (readArg == memref)
                        return true;
                }
                for (Value writeArg : rep.getWrites()) {
                    if (writeArg == memref)
                        return true;
                }
                return false;
            }



            // Determine if broadcast is needed based on partitioning strategy
            bool needsBroadcast(const ArrayPartitioningInfo &partInfo)
            {
                // If the subsequent replicate needs NO_PARTITION (replicate strategy),
                // then we need to broadcast the data so all nodes get the processed data
                return partInfo.strategy == ArrayPartitioningInfo::NO_PARTITION;
            }

            // Bounds of the loop a replicate partitions.  Shard slabs come from
            // deterministically splitting this [lb, ub), so equal bounds imply
            // equal shard ranges on every rank.
            struct LoopBounds
            {
                bool known = false;
                Value lb, ub, step;
            };

            LoopBounds partitionedLoopBounds(mlir::Operation *replicate)
            {
                LoopBounds bounds;
                // The partitioned loop is the first loop in the replicate body,
                // matching how analyzeArrayForPartitioning picks the IV.
                replicate->walk<mlir::WalkOrder::PreOrder>([&](mlir::Operation *child) {
                    if (bounds.known)
                        return mlir::WalkResult::interrupt();
                    if (auto scfFor = mlir::dyn_cast<mlir::scf::ForOp>(child)) {
                        bounds.known = true;
                        bounds.lb = scfFor.getLowerBound();
                        bounds.ub = scfFor.getUpperBound();
                        bounds.step = scfFor.getStep();
                        return mlir::WalkResult::interrupt();
                    }
                    return mlir::WalkResult::advance();
                });
                return bounds;
            }

            // Two values are provably the same extent: identical SSA value, or
            // two constants of equal value.  Anything else is treated as unknown.
            static bool sameExtent(Value a, Value b)
            {
                if (!a || !b)
                    return false;
                if (a == b)
                    return true;
                mlir::IntegerAttr ca, cb;
                if (mlir::matchPattern(a, mlir::m_Constant(&ca)) &&
                    mlir::matchPattern(b, mlir::m_Constant(&cb)))
                    return ca.getValue() == cb.getValue();
                return false;
            }

            // True when the producer's shard is exactly the slab each consumer
            // reads: both partition the buffer, same axis, no halo, provably
            // identical loop bounds.  Anything unknown returns false (broadcast).
            bool shardContractMatches(const ArrayPartitioningInfo &producerInfo,
                                      const ArrayPartitioningInfo &consumerInfo,
                                      const LoopBounds &producerBounds,
                                      const LoopBounds &consumerBounds)
            {
                if (producerInfo.strategy == ArrayPartitioningInfo::NO_PARTITION ||
                    consumerInfo.strategy == ArrayPartitioningInfo::NO_PARTITION)
                    return false;
                if (producerInfo.strategy != consumerInfo.strategy)
                    return false;
                if (producerInfo.partitionDimension < 0 ||
                    producerInfo.partitionDimension != consumerInfo.partitionDimension)
                    return false;
                if (producerInfo.haloLeft || producerInfo.haloRight ||
                    consumerInfo.haloLeft || consumerInfo.haloRight)
                    return false;
                if (!producerBounds.known || !consumerBounds.known)
                    return false;
                return sameExtent(producerBounds.lb, consumerBounds.lb) &&
                       sameExtent(producerBounds.ub, consumerBounds.ub) &&
                       sameExtent(producerBounds.step, consumerBounds.step);
            }
        };

        /// Analyzes whether outputs of a replicate operation need broadcasting
        /// based on how they are used in subsequent replicate operations.
        ///
        /// @param replicateOp The replicate operation to analyze
        /// @param broadcastDecisions Output vector of broadcast decisions for each write argument
        /// @return true if analysis was successful, false otherwise
        inline bool analyzeBroadcastRequirements(mlir::Operation *replicateOp,
                                                std::vector<BroadcastInfo> &broadcastDecisions)
        {
            BroadcastAnalysis analysis(replicateOp);
            return analysis.analyze(broadcastDecisions);
        }

        /// Helper function: Check if a specific output needs broadcasting
        inline bool doesOutputNeedBroadcast(mlir::Operation *replicateOp, Value outputMemref)
        {
            std::vector<BroadcastInfo> broadcastDecisions;
            
            // Both fallbacks below default to *broadcast*: answering "no" when
            // the analysis failed or never saw this buffer leaves the output
            // uninitialized on the ranks that read it (and can deadlock
            // collectives); answering "yes" only adds a redundant transfer.
            if (!analyzeBroadcastRequirements(replicateOp, broadcastDecisions)) {
                llvm::errs() << "Broadcast analysis failed; materializing output "
                                "conservatively\n";
                return true;
            }

            // Find the decision for this specific memref
            for (const BroadcastInfo &info : broadcastDecisions) {
                if (info.memref == outputMemref) {
                    return info.needsBroadcast;
                }
            }

            llvm::errs() << "Broadcast analysis has no decision for this output; "
                            "materializing conservatively\n";
            return true;
        }

    } // namespace dhir
} // namespace mlir

/*
 * =============================================================================
 * USAGE EXAMPLE
 * =============================================================================
 *
 * #include "BroadcastAnalysis.h"
 *
 * void processReplicateOp(mlir::Operation *replicateOp) {
 *     std::vector<mlir::dhir::BroadcastInfo> broadcastDecisions;
 *     
 *     // Analyze the replicate operation
 *     if (!mlir::dhir::analyzeBroadcastRequirements(replicateOp, broadcastDecisions)) {
 *         llvm::errs() << "Broadcast analysis failed!\n";
 *         return;
 *     }
 *     
 *     // Process each output
 *     for (const auto &info : broadcastDecisions) {
 *         if (info.needsBroadcast) {
 *             llvm::errs() << "Output needs broadcast: " << info.reason << "\n";
 *             // Insert broadcast operation
 *             insertBroadcast(info.memref);
 *         } else {
 *             llvm::errs() << "No broadcast needed: " << info.reason << "\n";
 *             // Setup partitioned transfer
 *             setupPartitionedTransfer(info.memref, info.partInfo);
 *         }
 *     }
 * }
 *
 *
 * =============================================================================
 * HOW IT WORKS
 * =============================================================================
 *
 * 1. Extracts all write arguments (outputs) from the replicate operation
 * 2. For each output:
 *    a. Finds subsequent replicate operations that read this output
 *    b. Uses analyzeArrayForPartitioning() to check partitioning requirement
 *    c. If NO_PARTITION → broadcast needed (all nodes need full data)
 *    d. If ROW_PARTITION or COL_PARTITION → no broadcast (data distributed)
 * 3. Returns vector of BroadcastInfo with decisions and reasons
 *
 * =============================================================================
 */

#endif // BROADCAST_ANALYSIS_H
