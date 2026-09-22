# tests/parboil

MLIR kernels extracted from **Parboil 2.5**, packaged as self-verifying MPI benchmarks.
Same structure and workflow as `tests/rodinia` (see that README for details) — the
differences are the kernel set, the dataset location, and dataset-driven input sizes.

## Layout

```
tests/parboil/
├── <kernel>.mlir            # one file per Parboil kernel (sgemm, spmv, stencil, ...)
├── drivers/
│   ├── <kernel>_main.cc     # per-kernel test driver (input load, invocation, checking)
│   ├── dhir_bench.h         # shared harness: timing, RESULT line, comparison helpers
│   └── ref/
│       └── ref_<kernel>.c   # ground-truth C implementation of the kernel
├── test.sh                  # baseline: lower kernels with mlir-opt
├── run_dhir.sh              # DHIR: lower kernels with build/bin/dhir-opt
├── run_dhir_pipeline.sh     # compile-only pipeline diagnostics for all *.mlir
└── test_output/             # output/error logs from run_dhir_pipeline.sh
```

Kernels: `sgemm spmv stencil histo cutcp mri_q mri_gridding sad tpacf bfs lbm`.

## What the files are

- **`.mlir`** — the extracted kernel (e.g. `sgemm.mlir` defines the matmul), in
  `func.func` + `affine.*` form. No `main`, no I/O — just the compute.
- **`drivers/<kernel>_main.cc`** — the `main()`: loads the Parboil input dataset files
  from `vendor/parboil/datasets/...`, calls the MLIR kernel and the reference, compares.
  Some drivers read env vars (e.g. problem size) so you can pick larger datasets.
- **`drivers/ref/`** — `ref_<kernel>.c` ground-truth implementations from Parboil,
  compiled with `gcc -O3` and linked into the benchmark for output verification.

## Scripts

### `test.sh` — baseline (mlir-opt path)

Lowers with **stock `mlir-opt`**, then `mlir-translate → llc`, compiles the reference,
links `driver + kernel.o + ref.o` with `mpicxx`, runs under `mpirun`.

```bash
./tests/parboil/test.sh sgemm          # one kernel, 1 rank
./tests/parboil/test.sh all 4          # all kernels, 4 MPI ranks
./tests/parboil/test.sh stencil 2 --keep --verbose
```

Pass = the driver prints `RESULT ... status=PASS` (MLIR kernel output matches reference).

### `run_dhir.sh` — the DHIR compiler path

Same harness, but lowering goes through **`build/bin/dhir-opt`** with the DHIR pipeline
(`--affine-to-dhir --std-to-dhir --lower-replicate --lower-converge --dhir-to-mpi
--lower-to-llvm`) using `tests/configs/system_config_4_cpu.json`, linked against
`build/libdhir_runtime.a`. This is the path that exercises the DHIR compiler itself.

```bash
./tests/parboil/run_dhir.sh sgemm 2
./tests/parboil/run_dhir.sh all 4
# overrides: DHIR_OPT=... CONFIG=... RUNTIME=... MLIR_OPT=... DHIR_MPIRUN_FLAGS=...
# DHIR_OMP_LIBDIR=... env vars; same --keep / --verbose / --workdir options as test.sh
```

### `run_dhir_pipeline.sh` — compile-only diagnostics

Does **not** build or run binaries. For every `*.mlir` it prints a table:
does plain `mlir-opt` accept it, does stage 1 (`--affine-to-dhir`) succeed, does the full
DHIR pipeline succeed — and on failure, which pass broke plus the first error line.
Logs are written to `test_output/`.

```bash
./tests/parboil/run_dhir_pipeline.sh
```

## Requirements — run from the repo root

All three scripts must be invoked **from the repository root** — the drivers open dataset
files via repo-root-relative paths like `vendor/parboil/datasets/mri-q/small/input/...`.
The parent directory must contain the extracted Parboil datasets:

```
vendor/parboil/datasets/    # per-benchmark inputs (small/, default/, UT/, short/ splits)
```

Each kernel expects its own dataset split (e.g. `spmv` → `small`, `bfs` → `UT`,
`lbm` → `short`, `histo`/`sad`/`tpacf` → `default`); the driver falls back to synthetic
data if files are missing, but real inputs are what the PASS check is designed for.

Toolchain: `mlir-opt` / `mlir-translate` / `llc` under `~/.local/bin`, `mpicxx`/`mpirun`,
and (for `run_dhir*.sh`) a built `build/bin/dhir-opt` + `build/libdhir_runtime.a`.
