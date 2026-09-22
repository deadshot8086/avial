#!/bin/bash
DHIR_OPT="$PWD/build/bin/dhir-opt"
MLIR_OPT="$HOME/.local/bin/mlir-opt"
CONFIG="$PWD/tests/configs/system_config_4_cpu.json"

PIPELINE=(
    "--affine-to-dhir"
    "--std-to-dhir"
    "--lower-replicate"
    "--lower-converge"
    "--dhir-to-mpi"
    "--lower-to-llvm"
)

OUT_DIR="$PWD/tests/parboil/test_output"
mkdir -p "$OUT_DIR"

echo "=========================================================================================="
echo "Running DHIR Compiler Pipeline (from tests/run_tests.sh) on Parboil MLIR files"
echo "Pipeline: ${PIPELINE[*]}"
echo "System Config: ${CONFIG}"
echo "=========================================================================================="
printf "%-18s %-12s %-15s %-20s %-30s\n" "Benchmark" "mlir-opt" "Stage 1" "Full Pipeline" "Failure / Diagnostic Note"
echo "------------------------------------------------------------------------------------------"

for f in $PWD/tests/parboil/*.mlir; do
    name=$(basename "$f" .mlir)
    out_file="$OUT_DIR/${name}_out.mlir"
    err_file="$OUT_DIR/${name}_err.log"

    # 1. mlir-opt check
    mlir_res="PASS"
    if ! "$MLIR_OPT" "$f" >/dev/null 2>&1; then
        mlir_res="FAIL"
    fi

    # 2. Stage 1 check (--affine-to-dhir)
    stage1_res="PASS"
    if ! "$DHIR_OPT" --affine-to-dhir "$f" "$CONFIG" >/dev/null 2>"$err_file"; then
        stage1_res="FAIL"
    fi

    # 3. Full pipeline test
    full_cmd="$DHIR_OPT ${PIPELINE[*]} $f $CONFIG"
    if eval "$full_cmd > $out_file 2> $err_file"; then
        printf "%-18s \033[0;32m%-12s\033[0m \033[0;32m%-15s\033[0m \033[0;32m%-20s\033[0m %-30s\n" "$name" "[$mlir_res]" "[$stage1_res]" "[PASS FULL]" "Lowered to LLVM + MPI"
    else
        # Determine which stage failed
        accumulated_passes=""
        failed_stage=""
        for pass in "${PIPELINE[@]}"; do
            accumulated_passes="$accumulated_passes $pass"
            test_cmd="$DHIR_OPT $accumulated_passes $f $CONFIG"
            if ! eval "$test_cmd > $out_file 2> $err_file"; then
                failed_stage="$pass"
                break
            fi
        done
        err_snippet=$(grep -m 1 -E "error:|Error|failed|fault|core" "$err_file" | head -c 40)
        if [ -z "$err_snippet" ]; then
            err_snippet=$(tail -n 1 "$err_file" | head -c 40)
        fi
        printf "%-18s \033[0;32m%-12s\033[0m %-15s \033[0;31m%-20s\033[0m %-30s\n" "$name" "[$mlir_res]" "[$stage1_res]" "[FAIL]" "$failed_stage ($err_snippet)"
    fi
done
echo "=========================================================================================="
