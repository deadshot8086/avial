#ifndef SYNC_HOISTING_H
#define SYNC_HOISTING_H

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/BuiltinTypes.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>

#include "arrayPartitionAnalysis.h"
#include "includes/dhirOps.h"
#include "includes/dhirDialect.h"

namespace mlir
{
    namespace dhir
    {
        // ====================================================================
        // DEFERRED SYNCHRONIZATION
        //
        // A replicate nested in a serial loop is synced after every iteration
        // because its shard may feed the next one.  When the shard is
        // *slab-local* -- every access addresses the partitioned dimension
        // with the shard's own IV at offset zero -- rank r only touches its
        // own slab, whose value depends only on its own history, so one
        // gather after the loop is enough.  Indices on the *other* dimensions
        // are unconstrained (they may be indirect, e.g. contcenters writes
        // coord[assign[i]][d]); only the partitioned dimension decides
        // locality.  A halo access fails the offset-zero test, which is what
        // keeps stencils on their per-iteration exchange.
        // ====================================================================

        // The nearest serial loop enclosing `op`, or null.  This is the loop a
        // deferred sync would be emitted after.
        inline mlir::Operation *nearestEnclosingSerialLoop(mlir::Operation *op)
        {
            for (mlir::Operation *p = op->getParentOp(); p; p = p->getParentOp())
            {
                if (mlir::isa<mlir::scf::ForOp, mlir::scf::WhileOp,
                              mlir::affine::AffineForOp>(p))
                    return p;
                if (mlir::isa<mlir::dhir::ScheduleOp>(p))
                    return nullptr;
            }
            return nullptr;
        }

        // The IV the partition analysis would shard on: the first loop in a
        // pre-order walk of the region, matching analyzeArrayForPartitioning.
        inline mlir::Value shardInductionVar(mlir::Operation *root)
        {
            mlir::Value iv;
            root->walk<mlir::WalkOrder::PreOrder>([&](mlir::Operation *op) {
                if (auto af = mlir::dyn_cast<mlir::affine::AffineForOp>(op))
                {
                    iv = af.getInductionVar();
                    return mlir::WalkResult::interrupt();
                }
                if (auto sf = mlir::dyn_cast<mlir::scf::ForOp>(op))
                {
                    iv = sf.getInductionVar();
                    return mlir::WalkResult::interrupt();
                }
                if (auto pf = mlir::dyn_cast<mlir::scf::ParallelOp>(op))
                {
                    if (!pf.getInductionVars().empty())
                        iv = pf.getInductionVars()[0];
                    return mlir::WalkResult::interrupt();
                }
                return mlir::WalkResult::advance();
            });
            return iv;
        }

        // Does every access to `buffer` inside `replicateOp` stay within the
        // rank's own slab of `partitionDim`?
        inline bool accessesStayInOwnSlab(mlir::Operation *replicateOp,
                                          mlir::Value buffer, mlir::Value shardIV,
                                          int partitionDim)
        {
            if (!shardIV || partitionDim < 0)
                return false;

            ArrayPartitioningAnalysis analysis(replicateOp, shardIV);
            bool local = true;
            bool sawAccess = false;

            auto inspect = [&](mlir::Operation *access, mlir::Value memref) {
                if (memref != buffer || !local)
                    return;
                sawAccess = true;
                auto exact = analysis.getUnitStrideDimensionAndOffset(access, shardIV);
                // Not expressible as IV(+/-const) on a single dimension, a
                // different dimension, or a non-zero offset (a neighbour's
                // slab) all mean this rank reaches outside its own data.
                if (!exact || exact->first != partitionDim || exact->second != 0)
                    local = false;
            };

            replicateOp->walk([&](mlir::Operation *op) {
                if (auto ld = mlir::dyn_cast<mlir::memref::LoadOp>(op))
                    inspect(op, ld.getMemRef());
                else if (auto st = mlir::dyn_cast<mlir::memref::StoreOp>(op))
                    inspect(op, st.getMemRef());
                else if (auto ld = mlir::dyn_cast<mlir::affine::AffineLoadOp>(op))
                    inspect(op, ld.getMemRef());
                else if (auto st = mlir::dyn_cast<mlir::affine::AffineStoreOp>(op))
                    inspect(op, st.getMemRef());
            });

            return local && sawAccess;
        }

        // Is `buffer` accessed inside `loop` anywhere other than inside
        // `replicateOp`?  Such an access runs on every rank and would observe
        // a slab this rank never wrote, so the sync cannot be deferred past
        // it.  Only real memory accesses count: a subview or memref.dim only
        // forms an alias or inspects shape, and the partition lowering emits
        // both next to the replicate, so counting those rejects every
        // candidate.
        inline bool touchedElsewhereInLoop(mlir::Operation *loop,
                                           mlir::Operation *replicateOp,
                                           mlir::Value buffer)
        {
            const bool trace = ::getenv("DHIR_TRACE_HOIST") != nullptr;

            // `buffer` plus every view transitively derived from it.
            llvm::SmallVector<mlir::Value> aliases;
            aliases.push_back(buffer);
            for (unsigned i = 0; i < aliases.size(); ++i)
                for (mlir::Operation *user : aliases[i].getUsers())
                    if (mlir::isa<mlir::memref::SubViewOp, mlir::memref::CastOp,
                                  mlir::memref::ReinterpretCastOp,
                                  mlir::memref::ExpandShapeOp,
                                  mlir::memref::CollapseShapeOp,
                                  mlir::memref::TransposeOp,
                                  mlir::memref::ViewOp>(user))
                        for (mlir::Value res : user->getResults())
                            if (mlir::isa<mlir::MemRefType>(res.getType()) &&
                                !llvm::is_contained(aliases, res))
                                aliases.push_back(res);

            auto isAlias = [&](mlir::Value v) {
                return llvm::is_contained(aliases, v);
            };

            bool touched = false;
            loop->walk([&](mlir::Operation *op) {
                if (touched)
                    return mlir::WalkResult::interrupt();
                if (op == replicateOp || replicateOp->isProperAncestor(op))
                    return mlir::WalkResult::advance();

                bool accesses = false;
                if (auto ld = mlir::dyn_cast<mlir::memref::LoadOp>(op))
                    accesses = isAlias(ld.getMemRef());
                else if (auto st = mlir::dyn_cast<mlir::memref::StoreOp>(op))
                    accesses = isAlias(st.getMemRef());
                else if (auto ld = mlir::dyn_cast<mlir::affine::AffineLoadOp>(op))
                    accesses = isAlias(ld.getMemRef());
                else if (auto st = mlir::dyn_cast<mlir::affine::AffineStoreOp>(op))
                    accesses = isAlias(st.getMemRef());
                else if (auto cp = mlir::dyn_cast<mlir::memref::CopyOp>(op))
                    accesses = isAlias(cp.getSource()) || isAlias(cp.getTarget());
                else if (mlir::isa<mlir::memref::SubViewOp, mlir::memref::CastOp,
                                   mlir::memref::ReinterpretCastOp,
                                   mlir::memref::ExpandShapeOp,
                                   mlir::memref::CollapseShapeOp,
                                   mlir::memref::TransposeOp,
                                   mlir::memref::ViewOp,
                                   mlir::memref::DimOp, mlir::memref::RankOp,
                                   mlir::memref::AllocOp, mlir::memref::AllocaOp,
                                   mlir::memref::DeallocOp,
                                   mlir::dhir::ReplicateOp>(op))
                    accesses = false; // view, metadata, or an operand declaration
                else
                    for (mlir::Value operand : op->getOperands())
                        if (isAlias(operand))
                        {
                            accesses = true; // unknown op holding the buffer
                            break;
                        }

                if (!accesses)
                    return mlir::WalkResult::advance();
                if (trace)
                    llvm::errs() << "  [hoist]   accessed by: " << op->getName()
                                 << "\n";
                touched = true;
                return mlir::WalkResult::interrupt();
            });
            return touched;
        }

        // May this replicate's synchronization be deferred until after the
        // serial loop it sits in?  Every written buffer must be slab-local and
        // untouched elsewhere in that loop; otherwise the per-iteration sync
        // stands.  `loopOut` receives the loop to emit the sync after.
        inline bool canDeferSyncOutOfLoop(mlir::Operation *replicateOp,
                                          mlir::Operation *&loopOut)
        {
            loopOut = nullptr;
            auto rep = mlir::dyn_cast<mlir::dhir::ReplicateOp>(replicateOp);
            if (!rep)
                return false;

            const bool trace = ::getenv("DHIR_TRACE_HOIST") != nullptr;
            auto reject = [&](const char *why) {
                if (trace)
                    llvm::errs() << "  [hoist] reject: " << why << "\n";
                return false;
            };

            mlir::Operation *loop = nearestEnclosingSerialLoop(replicateOp);
            if (!loop)
                return reject("no enclosing serial loop");

            mlir::Value shardIV = shardInductionVar(replicateOp);
            if (!shardIV)
                return reject("no shard induction variable");

            llvm::SmallVector<mlir::Value> buffers(rep.getWrites());
            if (buffers.empty())
                return reject("no write buffers");

            for (mlir::Value buffer : buffers)
            {
                auto memTy = mlir::dyn_cast<mlir::MemRefType>(buffer.getType());
                if (!memTy)
                    return reject("write is not a memref");

                ArrayPartitioningInfo info =
                    analyzeArrayForPartitioning(replicateOp, buffer);
                if (trace)
                    llvm::errs() << "  [hoist] write buffer: strategy="
                                 << (int)info.strategy << " dim="
                                 << info.partitionDimension << " halo="
                                 << info.haloLeft << "/" << info.haloRight
                                 << " (" << info.partitionReason << ")\n";
                if (info.strategy == ArrayPartitioningInfo::NO_PARTITION)
                    return reject("write output is NO_PARTITION");
                if (info.partitionDimension < 0)
                    return reject("write output has no partition dimension");
                if (info.haloLeft || info.haloRight)
                    return reject("write output needs a halo");

                if (!accessesStayInOwnSlab(replicateOp, buffer, shardIV,
                                           info.partitionDimension))
                    return reject("write accesses leave this rank's slab");

                if (touchedElsewhereInLoop(loop, replicateOp, buffer))
                    return reject("buffer touched elsewhere inside the loop");
            }

            // Reads must be slab-local too, or a rank would consume a slab it
            // never received.  A read left whole (NO_PARTITION) is replicated
            // on every rank and stays valid, so only partitioned reads are
            // constrained.
            for (mlir::Value buffer : rep.getReads())
            {
                if (!mlir::isa<mlir::MemRefType>(buffer.getType()))
                    continue;
                ArrayPartitioningInfo info =
                    analyzeArrayForPartitioning(replicateOp, buffer);
                if (trace)
                    llvm::errs() << "  [hoist] read buffer: strategy="
                                 << (int)info.strategy << " dim="
                                 << info.partitionDimension << " halo="
                                 << info.haloLeft << "/" << info.haloRight
                                 << "\n";
                if (info.strategy == ArrayPartitioningInfo::NO_PARTITION)
                    continue;
                if (info.haloLeft || info.haloRight)
                    return reject("read input needs a halo");
                if (!accessesStayInOwnSlab(replicateOp, buffer, shardIV,
                                           info.partitionDimension))
                    return reject("read accesses leave this rank's slab");
            }

            loopOut = loop;
            return true;
        }

    } // namespace dhir
} // namespace mlir

#endif // SYNC_HOISTING_H
