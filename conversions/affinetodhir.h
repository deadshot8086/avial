#include "mlir/Pass/PassManager.h"
#include "mlir/IR/PatternMatch.h"

#include "includes/dhirDialect.h"
#include "includes/dhirTypes.h"

#include "mlir/Transforms/DialectConversion.h"

#include "mlir/Conversion/LLVMCommon/ConversionTarget.h"
#include "mlir/Conversion/Passes.h"

#include "llvm/Support/Casting.h"

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/Arith/IR/Arith.h"

#include "includes/dhirDialect.h"
#include "includes/dhirOps.h"
#include "includes/dhirTypes.h"
#include "includes/utils.h"

#include "mlir/Dialect/DLTI/DLTI.h"
#include "mlir/IR/Builders.h"

#include "mlir/Conversion/Passes.h"

#include "analysis/polyhedralAnalysis.h"
#include "analysis/arrayPartitionAnalysis.h"

#include <string>
#include <functional>

using namespace mlir;
using namespace dhir;

namespace mlir
{
    namespace dhir
    {

#define GEN_PASS_DEF_CONVERTAFFINETODHIRPASS

#include "dialect/Passes.h.inc"

        struct ConvertAffineToDhirPass : public mlir::dhir::impl::ConvertAffineToDhirPassBase<ConvertAffineToDhirPass>
        {
            using ConvertAffineToDhirPassBase::ConvertAffineToDhirPassBase;

            bool isUnitStep(mlir::scf::ForOp loop)
            {
                auto step = loop.getStep().getDefiningOp<mlir::arith::ConstantIndexOp>();
                return step && step.value() == 1;
            }

            bool sameIndices(ValueRange lhs, ValueRange rhs)
            {
                return lhs.size() == rhs.size() &&
                       std::equal(lhs.begin(), lhs.end(), rhs.begin());
            }

            bool isAdditiveReadModifyWrite(mlir::memref::StoreOp store)
            {
                Operation *def = store.getValue().getDefiningOp();
                if (!def || !isa<mlir::arith::AddFOp, mlir::arith::AddIOp>(def))
                    return false;

                for (Value operand : def->getOperands())
                {
                    auto load = operand.getDefiningOp<mlir::memref::LoadOp>();
                    if (load && load.getMemRef() == store.getMemRef() &&
                        sameIndices(load.getIndices(), store.getIndices()))
                        return true;
                }
                return false;
            }

            // A loop whose IV is unused repeats one computation every
            // iteration (e.g. an `iters` convergence loop).  Its iterations
            // address no disjoint data, so partitioning it would make every
            // shard recompute the whole output and the combine would sum the
            // duplicates.  Never select it; let an inner loop carry the
            // parallelism.
            bool isIVInvariantBody(mlir::scf::ForOp loop)
            {
                return loop.getInductionVar().use_empty();
            }

            // SCF lacks the affine dependence test below.  Accept only the
            // common kernel shape: overwrites advance with the IV; other
            // writes must be additive read-modify-write reductions.
            bool isScfLoopIndependent(mlir::scf::ForOp loop)
            {
                if (!loop.getInitArgs().empty() || !isUnitStep(loop))
                    return false;

                if (isIVInvariantBody(loop))
                {
                    llvm::errs() << "SCF loop body is invariant in its IV "
                                    "(repeat loop); not partitioning it\n";
                    return false;
                }

                Value iv = loop.getInductionVar();
                mlir::dhir::ArrayPartitioningAnalysis analysis(loop, iv);
                llvm::SmallVector<mlir::memref::LoadOp> loads;
                llvm::SmallVector<mlir::memref::StoreOp> stores;
                loop.walk([&](mlir::memref::LoadOp load) { loads.push_back(load); });
                loop.walk([&](mlir::memref::StoreOp store) { stores.push_back(store); });

                if (stores.empty())
                    return false;

                for (mlir::memref::StoreOp store : stores)
                {
                    auto storeAccess = analysis.getUnitStrideDimensionAndOffset(store, iv);
                    if (!storeAccess)
                    {
                        if (!isAdditiveReadModifyWrite(store))
                            return false;
                        continue;
                    }
                    if (storeAccess->second != 0)
                        return false;

                    // An in-place load from another iteration's slice is a real
                    // loop-carried dependence, not a stencil input halo.
                    for (mlir::memref::LoadOp load : loads)
                    {
                        if (load.getMemRef() != store.getMemRef())
                            continue;
                        auto loadAccess =
                            analysis.getUnitStrideDimensionAndOffset(load, iv);
                        if (!loadAccess || loadAccess->first != storeAccess->first ||
                            loadAccess->second != storeAccess->second)
                            return false;
                    }
                }

                llvm::errs() << "SCF loop is conservatively independent\n";
                return true;
            }

            bool isStencilLoop(Operation *loop, Value iv,
                               const llvm::SmallVector<llvm::SmallVector<Value>> &insouts)
            {
                mlir::dhir::ArrayPartitioningAnalysis analysis(loop, iv);
                for (Value in : insouts[0])
                {
                    auto info = analysis.analyzeArray(in);
                    if (info.haloLeft > 0 || info.haloRight > 0)
                        return true;
                }
                for (Value out : insouts[1])
                {
                    auto info = analysis.analyzeArray(out);
                    if (info.haloLeft > 0 || info.haloRight > 0)
                        return true;
                }
                return false;
            }

            void wrapScfLoop(mlir::scf::ForOp loop, mlir::OpBuilder &builder,
                             int &repId)
            {
                auto insouts = InsOutsAnalysis::getInsandOut(loop);
                bool isStencil =
                    isStencilLoop(loop, loop.getInductionVar(), insouts);

                // A shard nested in serial control flow may feed the next
                // iteration; force a broadcast so its writes stay ordered.
                bool forceBroadcast =
                    loop->getParentOfType<mlir::scf::ForOp>() ||
                    loop->getParentOfType<mlir::scf::WhileOp>();

                builder.setInsertionPoint(loop);
                auto replicateOp = builder.create<mlir::dhir::ReplicateOp>(
                    loop.getLoc(), insouts[0], insouts[1]);
                replicateOp->setAttr("replicateID",
                                     builder.getI64IntegerAttr(repId));
                replicateOp->setAttr(
                    "pattern",
                    builder.getStringAttr(isStencil ? "stencil" : "default"));
                if (forceBroadcast)
                    replicateOp->setAttr("forceBroadcast", builder.getUnitAttr());

                mlir::Block *newBlock =
                    builder.createBlock(&replicateOp.getBodyRegion());
                loop->moveBefore(newBlock, newBlock->end());
                builder.setInsertionPointToEnd(newBlock);
                builder.create<mlir::dhir::YieldOp>(builder.getUnknownLoc());
                llvm::errs() << "Wrapped SCF loop with ReplicateOp (replicateID="
                             << repId << ")\n";
                ++repId;
            }

            // Helper function to check if a loop is independent (considering only its own iterations)
            // This checks the loop in isolation, not in the context of parent loops
            bool isLoopIndependent(mlir::affine::AffineForOp loop)
            {
                llvm::SmallVector<mlir::Operation *, 4> memOpVector;
                bool hasInvariantStore = false;
                Value iv = loop.getInductionVar();
                mlir::dhir::ArrayPartitioningAnalysis analysis(loop.getOperation(), loop.getInductionVar());

                // Collect only memory operations directly within this loop
                // Do NOT walk into nested loops - we only care about this loop's own dependencies
                loop.getBody()->walk([&](mlir::Operation *op)
                                     {
                    if (mlir::isa<mlir::affine::AffineStoreOp>(op)) {
                        memOpVector.push_back(op);
                        // A memory operation is owned by the loop body block,
                        // not directly by the AffineForOp.  Compare the
                        // nearest enclosing loop so invariant stores in this
                        // loop are actually rejected while nested-loop stores
                        // remain part of the nested loop's analysis.
                        if (op->getParentOfType<mlir::affine::AffineForOp>() == loop) {
                            if (analysis.getDimensionForIV(op, iv) == -1) {
                                hasInvariantStore = true;
                            }
                        }
                    }
                    else if (mlir::isa<mlir::affine::AffineLoadOp>(op)) {
                        memOpVector.push_back(op);
                    } });

                if (hasInvariantStore) {
                    llvm::errs() << "Loop contains invariant store (reduction) - cannot parallelize\n";
                    return false;
                }

                // Check for loop-carried dependencies at this loop's level only
                llvm::SmallVector<mlir::Operation *, 4> forLoopOpVector;
                forLoopOpVector.push_back(loop.getOperation());

                affine::FlatAffineValueConstraints constraints;
                affine::getIndexSet(forLoopOpVector, &constraints);

                llvm::errs() << "---- Checking Inner Loop Independence (Isolated) ----\n";

                for (int i = 0; i < memOpVector.size(); ++i)
                {
                    for (int j = 0; j < memOpVector.size(); ++j)
                    {
                        if (i == j)
                            continue;

                        mlir::affine::MemRefAccess src(memOpVector[i]);
                        mlir::affine::MemRefAccess dst(memOpVector[j]);
                        SmallVector<mlir::affine::DependenceComponent, 2> comps;

                        mlir::affine::DependenceResult res =
                            mlir::affine::checkMemrefAccessDependence(src, dst, 1, &constraints, &comps);

                        if (res.value == mlir::affine::DependenceResult::HasDependence)
                        {
                            if (comps.size() > 0)
                            {
                                auto &comp = comps[0];
                                if (!(comp.lb == 0 && comp.ub == 0))
                                {
                                    llvm::errs() << "Loop carries dependence (isolated check)\n";
                                    llvm::errs() << "---- End Checking Inner Loop Independence ----\n";
                                    return false; // Has loop-carried dependence
                                }
                            }
                        }
                        else if (res.value == mlir::affine::DependenceResult::Failure)
                        {
                            llvm::errs() << "Dependence check failed - assuming dependent\n";
                            llvm::errs() << "---- End Checking Inner Loop Independence ----\n";
                            return false; // Conservative: assume dependent
                        }
                    }
                }

                llvm::errs() << "No loop-carried dependence found - loop is independent!\n";
                llvm::errs() << "---- End Checking Inner Loop Independence ----\n";
                return true; // No loop-carried dependence
            }

            // Helper function to wrap independent loops with ReplicateOp
            void wrapIndependentLoopsInConverge(mlir::affine::AffineForOp outerLoop,
                                                mlir::OpBuilder &builder,
                                                int &repId)
            {
                llvm::SmallVector<mlir::affine::AffineForOp> allInnerLoops;

                // Collect all direct child loops of the outer loop
                // We need to check them at the same nesting level
                for (auto &op : outerLoop.getBody()->getOperations())
                {
                    if (auto innerLoop = mlir::dyn_cast<mlir::affine::AffineForOp>(op))
                    {
                        allInnerLoops.push_back(innerLoop);
                    }
                }

                // Now check each inner loop for independence (in isolation)
                llvm::SmallVector<mlir::affine::AffineForOp> independentLoops;
                for (auto innerLoop : allInnerLoops)
                {
                    // iter_args are invisible to the memory-access dependence
                    // check but are a real loop-carried value dependence: each
                    // iteration feeds the next.  Partitioning such a loop would
                    // need the previous shard's state, so leave it alone.
                    if (innerLoop.getNumRegionIterArgs() != 0)
                    {
                        llvm::errs() << "Inner loop carries iter_args; not wrapping in ReplicateOp\n";
                        continue;
                    }
                    if (isLoopIndependent(innerLoop))
                    {
                        independentLoops.push_back(innerLoop);
                    }
                }

                llvm::errs() << "Found " << independentLoops.size()
                             << " independent inner loops to wrap\n";

                // Wrap each independent loop with ReplicateOp
                for (auto forOp : independentLoops)
                {
                    auto insouts = InsOutsAnalysis::getInsandOut(forOp);

                    bool isStencil = false;

                    mlir::dhir::ArrayPartitioningAnalysis analysis(forOp.getOperation(), forOp.getInductionVar());
                    for (Value in : insouts[0])
                    {
                        auto info = analysis.analyzeArray(in);
                        if (info.haloLeft > 0 || info.haloRight > 0)
                            isStencil = true;
                    }

                    for (Value out : insouts[1])
                    {
                        auto info = analysis.analyzeArray(out);
                        if (info.haloLeft > 0 || info.haloRight > 0)
                            isStencil = true;
                    }

                    builder.setInsertionPoint(forOp);
                    auto replicateOp = builder.create<mlir::dhir::ReplicateOp>(forOp.getLoc(), insouts[0], insouts[1]);
                    replicateOp->setAttr("replicateID", builder.getI64IntegerAttr(repId));

                    if (isStencil)
                        replicateOp->setAttr("pattern", builder.getStringAttr("stencil"));
                    else
                        replicateOp->setAttr("pattern", builder.getStringAttr("default"));

                    mlir::Region &replicateRegion = replicateOp.getBodyRegion();
                    mlir::Block *newBlock = builder.createBlock(&replicateRegion);

                    forOp->moveBefore(newBlock, newBlock->end());
                    builder.setInsertionPointToEnd(newBlock);
                    builder.create<mlir::dhir::YieldOp>(builder.getUnknownLoc());

                    llvm::errs() << "Wrapped loop with ReplicateOp (replicateID=" << repId << ")\n";
                    ++repId;
                }
            }

            void runOnOperation() override
            {
                mlir::MLIRContext *context = &getContext();
                auto *module = getOperation();
                mlir::OpBuilder builder(context);

                llvm::SmallVector<mlir::Operation *, 4> toReplicateVector;
                llvm::SmallVector<mlir::Operation *, 4> toConvergeVector;
                llvm::SmallVector<mlir::Operation *, 4> toTaskVector;

                module->walk<mlir::WalkOrder::PreOrder>([&](mlir::Operation *op)
                                                        {
                    if (mlir::isa<func::FuncOp>(op))
                    {
                        auto funcOp = mlir::dyn_cast<func::FuncOp>(op);
                        Block &blck = funcOp.getBody().front();

                        for (auto &op : blck.getOperations())
                        {
                            if (mlir::isa<affine::AffineForOp>(op))
                            {
                                mlir::affine::AffineForOp forOp = mlir::dyn_cast<mlir::affine::AffineForOp>(op);
                                
                                // Check dependence at depth 1 (outer loop)
                                int outerDep = checkLoopDependence(forOp, 1);
                                
                                // An unused-IV loop repeats one computation (an
                                // `iters` loop); the dependence test reports no
                                // conflict, but partitioning it is unsound — every
                                // shard would recompute the whole output and the
                                // combine would sum the duplicates (spmv/histo
                                // came out scaled by the shard count).
                                //
                                // Distributing an inner loop instead still breaks:
                                // the partialReduce combine mishandles scatter
                                // outputs (spmv's disjoint overwrite gets summed
                                // onto shard 0's old data; histo's i8 saturating
                                // scatter-add under-counts).  Until the combine is
                                // reworked per scatter kind, keep the whole nest
                                // unpartitioned so every rank redundantly computes
                                // the correct result.
                                bool ivRepeatLoop = forOp.getInductionVar().use_empty();

                                if (outerDep == 1) // Outer loop has dependence
                                {
                                    // Collect all inner loops
                                    llvm::SmallVector<mlir::affine::AffineForOp> innerLoops;
                                    forOp.walk<mlir::WalkOrder::PreOrder>([&](mlir::affine::AffineForOp innerOp)
                                    {
                                        if (innerOp != forOp) // Skip the outer loop itself
                                        {
                                            innerLoops.push_back(innerOp);
                                        }
                                    });

                                    // Check if any inner loop is parallelizable
                                    bool hasParallelizableInner = false;
                                    for (auto innerLoop : innerLoops)
                                    {
                                        if (isLoopIndependent(innerLoop))
                                        {
                                            hasParallelizableInner = true;
                                            break;
                                        }
                                    }
                                    
                                    // Wrap with ConvergeOp if there are parallelizable inner loops
                                    if(hasParallelizableInner)
                                        toConvergeVector.push_back(forOp);
                                    else
                                    {
                                        // If it contains dependence at both level, 
                                        // Wrap the whole loop nest with taskOp.
                                        toTaskVector.push_back(forOp); 
                                        llvm::errs() << "---- Wrapping with TaskOp ----\n";
                                    }
                                        
                                }
                                else if (outerDep == 0) // No dependence in outer loop
                                {
                                    // Fully parallelizable - wrap with ReplicateOp.
                                    // iter_args are not visible to the memory-access
                                    // dependence check, yet they carry a value from
                                    // one iteration to the next, so a loop that has
                                    // them is not parallelizable: it is left
                                    // unpartitioned and then runs, unchanged, on
                                    // every rank.
                                    if (forOp.getNumRegionIterArgs() != 0)
                                    {
                                        llvm::errs() << "Loop carries iter_args; leaving it unpartitioned\n";
                                    }
                                    else if (ivRepeatLoop)
                                    {
                                        llvm::errs() << "Loop body is invariant in its IV "
                                                        "(repeat loop); leaving it unpartitioned\n";
                                    }
                                    else
                                        toReplicateVector.push_back(forOp);
                                }
                                else // outerDep == 2, dependence check failed
                                {
                                    // [TODO] Consider it as an Individual task and wrap everything 
                                    // with task Op. 
                                    llvm::errs() << "Dependence Analysis Failed!\n";
                                    exit(0);    
                                }
                            }
                        }
                    } });



                // Generate Individual Tasks
                for(auto task: toTaskVector)
                {
                    
                }
                
                // Create ReplicateOp for fully parallelizable loops
                int repId = 1;
                for (auto op : toReplicateVector)
                {
                    affine::AffineForOp forOp = mlir::dyn_cast<affine::AffineForOp>(op);
                    auto insouts = InsOutsAnalysis::getInsandOut(forOp);

                    bool isStencil = false;
                    mlir::dhir::ArrayPartitioningAnalysis analysis(
                        forOp.getOperation(), forOp.getInductionVar());
                    for (Value in : insouts[0])
                    {
                        auto info = analysis.analyzeArray(in);
                        isStencil |= info.haloLeft > 0 || info.haloRight > 0;
                    }
                    for (Value out : insouts[1])
                    {
                        auto info = analysis.analyzeArray(out);
                        isStencil |= info.haloLeft > 0 || info.haloRight > 0;
                    }

                    builder.setInsertionPoint(forOp);
                    auto replicateOp = builder.create<mlir::dhir::ReplicateOp>(forOp.getLoc(), insouts[0], insouts[1]);
                    replicateOp->setAttr("replicateID", builder.getI64IntegerAttr(repId));
                    replicateOp->setAttr(
                        "pattern",
                        builder.getStringAttr(isStencil ? "stencil" : "default"));

                    mlir::Region &replicateRegion = replicateOp.getBodyRegion();
                    mlir::Block *newBlock = builder.createBlock(&replicateRegion);

                    forOp->moveBefore(newBlock, newBlock->end());
                    builder.setInsertionPointToEnd(newBlock);
                    builder.create<mlir::dhir::YieldOp>(builder.getUnknownLoc());
                    ++repId;
                }

                // Select the outermost conservatively independent loops,
                // including loops nested in serial SCF control flow, without
                // nesting a replicate under an already-selected loop.  Tasks
                // under a loop-carried scf.for stay serial: DHIR has no
                // contract for cloning the carried iteration state across
                // shards.
                llvm::SmallVector<mlir::scf::ForOp> scfCandidates;
                module->walk<mlir::WalkOrder::PreOrder>([&](mlir::scf::ForOp loop) {
                    for (Operation *parent = loop->getParentOp(); parent;
                         parent = parent->getParentOp())
                    {
                        if (auto carried = dyn_cast<mlir::scf::ForOp>(parent);
                            carried && !carried.getInitArgs().empty())
                        {
                            llvm::errs()
                                << "Leaving SCF loop under loop-carried ancestor "
                                << "unpartitioned\n";
                            return;
                        }
                        if (llvm::is_contained(toReplicateVector, parent) ||
                            llvm::is_contained(toConvergeVector, parent))
                            return;
                    }

                    for (mlir::scf::ForOp selected : scfCandidates)
                        if (selected->isProperAncestor(loop))
                            return;

                    if (isScfLoopIndependent(loop))
                        scfCandidates.push_back(loop);
                });

                for (mlir::scf::ForOp loop : scfCandidates)
                    wrapScfLoop(loop, builder, repId);

                // Create ConvergeOp for loops with dependencies
                int taskId = 1;
                for (auto op : toConvergeVector)
                {
                    affine::AffineForOp forOp = mlir::dyn_cast<affine::AffineForOp>(op);

                    // FIRST: Wrap independent inner loops with ReplicateOp
                    // This must be done BEFORE creating ConvergeOp
                    wrapIndependentLoopsInConverge(forOp, builder, repId);

                    // NOW: Create the ConvergeOp and move the outer loop into it
                    auto insouts = InsOutsAnalysis::getInsandOut(forOp);

                    builder.setInsertionPoint(forOp);
                    auto convergeOp = builder.create<mlir::dhir::ConvergeOp>(forOp.getLoc(), insouts[0], insouts[1]);
                    convergeOp->setAttr("ConvergeID", builder.getI64IntegerAttr(taskId));

                    mlir::Region &ConvergeRegion = convergeOp.getBodyRegion();
                    mlir::Block *newBlock = builder.createBlock(&ConvergeRegion);

                    forOp->moveBefore(newBlock, newBlock->end());
                    builder.setInsertionPointToEnd(newBlock);
                    builder.create<mlir::dhir::YieldOp>(builder.getUnknownLoc());

                    ++taskId;
                }

            }
        };
    }
}
