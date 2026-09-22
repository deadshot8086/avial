#!/usr/bin/env bash
#
# Build and run Parboil kernel driver at given rank
#
#   tests/parboil/test.sh <kernel|all> [ranks] [options]
#
# Pipeline (STRICTLY mlir-opt, NEVER dhir-opt):
#   mlir-opt       -> LLVM dialect
#   mlir-translate -> .ll
#   llc            -> kernel.o
#   gcc/g++        -> ref_<kernel>.o (compiled reference object)
#   mpicxx         -> link driver + kernel.o + ref.o + MPI
#   mpirun -np <ranks>
#
# Prints the driver's RESULT line and exits nonzero on error or verification failure.
set -eo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

MLIR_OPT="${MLIR_OPT:-$HOME/.local/bin/mlir-opt}"
MLIR_TRANSLATE="${MLIR_TRANSLATE:-$HOME/.local/bin/mlir-translate}"
LLC="${LLC:-$HOME/.local/bin/llc}"

EXTRACTED="$SCRIPT_DIR"
DRIVERS="$EXTRACTED/drivers"
REFS="$DRIVERS/ref"

ALL_KERNELS=(
    sgemm
    spmv
    stencil
    histo
    cutcp
    mri_q
    mri_gridding
    sad
    tpacf
    bfs
    lbm
)

usage() {
    cat >&2 <<EOF
usage: $(basename "$0") <kernel|all> [ranks] [--keep] [--verbose] [--workdir DIR]
EOF
    exit 2
}

[[ $# -ge 1 ]] || usage
TARGET="$1"; shift
RANKS="${1:-1}"
[[ $# -ge 1 && "$1" =~ ^[0-9]+$ ]] && shift

WORKDIR=""; KEEP=0; VERBOSE=0
while [[ $# -gt 0 ]]; do
    case "$1" in
        --workdir) WORKDIR="$2"; shift 2 ;;
        --keep)    KEEP=1; shift ;;
        --verbose) VERBOSE=1; shift ;;
        *) echo "unknown option: $1" >&2; usage ;;
    esac
done

say() { [[ $VERBOSE -eq 1 ]] && echo "  [$1] ${*:2}" >&2 || true; }

run_kernel() {
    local KERNEL="$1"
    local P="$RANKS"
    local K_WORKDIR="$WORKDIR"
    local CLEANUP=0

    if [[ -z "$K_WORKDIR" ]]; then
        K_WORKDIR="$(mktemp -d /tmp/parboil_bench_${KERNEL}_XXXXXX)"
        CLEANUP=1
    fi
    mkdir -p "$K_WORKDIR"

    local SRC="$EXTRACTED/${KERNEL}.mlir"
    local DRIVER="$DRIVERS/${KERNEL}_main.cc"
    local REF_C="$REFS/ref_${KERNEL}.c"
    local REF_CPP="$REFS/ref_${KERNEL}.cpp"

    [[ -f "$SRC" ]] || { echo "FAIL $KERNEL: missing MLIR source: $SRC" >&2; return 1; }
    [[ -f "$DRIVER" ]] || { echo "FAIL $KERNEL: missing driver: $DRIVER" >&2; return 1; }

    # 1. Compile reference object file from Parboil base reference source
    say "$KERNEL" "compiling reference object"
    if [[ -f "$REF_C" ]]; then
        gcc -O3 -c "$REF_C" -o "$K_WORKDIR/ref_${KERNEL}.o" 2>"$K_WORKDIR/ref_comp.log" || {
            echo "FAIL $KERNEL: reference C compilation failed" >&2
            cat "$K_WORKDIR/ref_comp.log" >&2
            return 1
        }
    elif [[ -f "$REF_CPP" ]]; then
        g++ -O3 -c "$REF_CPP" -o "$K_WORKDIR/ref_${KERNEL}.o" 2>"$K_WORKDIR/ref_comp.log" || {
            echo "FAIL $KERNEL: reference C++ compilation failed" >&2
            cat "$K_WORKDIR/ref_comp.log" >&2
            return 1
        }
    else
        echo "FAIL $KERNEL: missing reference source in $REFS (ref_${KERNEL}.c or .cpp)" >&2
        return 1
    fi

    # 2. Lower MLIR using STRICTLY mlir-opt (NEVER dhir-opt)
    say "$KERNEL" "mlir-opt lowering"
    "$MLIR_OPT" "$SRC" \
        --lower-affine \
        --convert-scf-to-cf \
        --convert-math-to-llvm \
        --expand-strided-metadata \
        --finalize-memref-to-llvm \
        --convert-arith-to-llvm \
        --convert-index-to-llvm \
        --convert-cf-to-llvm \
        --convert-func-to-llvm \
        --reconcile-unrealized-casts \
        -o "$K_WORKDIR/llvm.mlir" 2>"$K_WORKDIR/opt.log" || {
        echo "FAIL $KERNEL: mlir-opt failed" >&2
        cat "$K_WORKDIR/opt.log" >&2
        return 1
    }

    say "$KERNEL" "mlir-translate"
    "$MLIR_TRANSLATE" --mlir-to-llvmir "$K_WORKDIR/llvm.mlir" > "$K_WORKDIR/kernel.ll" 2>"$K_WORKDIR/translate.log" || {
        echo "FAIL $KERNEL: mlir-translate failed" >&2
        cat "$K_WORKDIR/translate.log" >&2
        return 1
    }

    say "$KERNEL" "llc"
    "$LLC" -O3 -relocation-model=pic -filetype=obj "$K_WORKDIR/kernel.ll" -o "$K_WORKDIR/kernel.o" 2>"$K_WORKDIR/llc.log" || {
        echo "FAIL $KERNEL: llc failed" >&2
        cat "$K_WORKDIR/llc.log" >&2
        return 1
    }

    # 3. Link driver + kernel.o + ref_<kernel>.o
    say "$KERNEL" "link"
    mpicxx -O2 -fopenmp -I"$K_WORKDIR" -I"$DRIVERS" \
        "$DRIVER" "$K_WORKDIR/kernel.o" "$K_WORKDIR/ref_${KERNEL}.o" \
        -lm -o "$K_WORKDIR/bench" 2>"$K_WORKDIR/link.log" || {
        echo "FAIL $KERNEL: link failed" >&2
        cat "$K_WORKDIR/link.log" >&2
        return 1
    }

    # 4. Run through mpirun
    say "$KERNEL" "mpirun -np $P"
    local RC=0
    set +e
    mpirun --oversubscribe --bind-to none -np "$P" "$K_WORKDIR/bench" \
        > "$K_WORKDIR/run.out" 2> "$K_WORKDIR/run.err"
    RC=$?
    set -e

    grep '^RESULT' "$K_WORKDIR/run.out" || true

    if [[ $RC -ne 0 ]] || grep -q 'status=FAIL' "$K_WORKDIR/run.out"; then
        echo "FAIL $KERNEL np=$P: exit $RC" >&2
        grep -v '^RESULT' "$K_WORKDIR/run.out" | head -20 >&2
        cat "$K_WORKDIR/run.err" >&2
        [[ $KEEP -eq 1 ]] && echo "  intermediates: $K_WORKDIR" >&2
        return 1
    fi

    [[ $KEEP -eq 1 ]] && echo "  intermediates: $K_WORKDIR" >&2
    if [[ $CLEANUP -eq 1 && $KEEP -eq 0 ]]; then
        rm -rf "$K_WORKDIR"
    fi
    return 0
}

if [[ "$TARGET" == "all" ]]; then
    FAILED=0
    PASSED=0
    echo "================================================================================"
    echo "Running Parboil Benchmark Verification Suite (${#ALL_KERNELS[@]} kernels, ranks=$RANKS)"
    echo "================================================================================"
    for k in "${ALL_KERNELS[@]}"; do
        if run_kernel "$k"; then
            PASSED=$((PASSED + 1))
        else
            FAILED=$((FAILED + 1))
        fi
    done
    echo "================================================================================"
    echo "SUMMARY: Passed: $PASSED / Total: ${#ALL_KERNELS[@]} (Failed: $FAILED)"
    echo "================================================================================"
    [[ $FAILED -eq 0 ]] || exit 1
else
    run_kernel "$TARGET"
fi
