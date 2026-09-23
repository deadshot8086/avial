# tests/rodinia

MLIR kernels extracted from **Rodinia 3.1**, packaged as self-verifying MPI benchmarks.

## Layout

```
tests/rodinia/
├── <kernel>.mlir            # one file per Rodinia kernel (hotspot, nw, lud, ...)
├── drivers/
│   ├── <kernel>_main.cc     # per-kernel test driver (input gen, invocation, checking)
│   ├── dhir_bench.h         # shared harness: timing, RESULT line, comparison helpers
│   └── ref/
│       ├── ref_<kernel>.c   # ground-truth C implementation of the kernel
│       └── *.c/h            # extra helper sources some references need (e.g. avilib)
├── test.sh                  # baseline: lower kernels with mlir-opt
├── run_dhir.sh              # DHIR: lower kernels with build/bin/dhir-opt
├── run_dhir_pipeline.sh     # compile-only pipeline diagnostics for all *.mlir
└── test_output/             # output/error logs from run_dhir_pipeline.sh
```

## What the files are

- **`.mlir`** — the computationally intensive kernel of a Rodinia benchmark, hand-extracted
  into MLIR (`func.func` + `affine.for/affine.load/affine.store`). It is *just the kernel* —
  no `main`, no I/O. Example: `nw.mlir` defines `@needleman_wunsch`.
- **`drivers/<kernel>_main.cc`** — the actual `main()`. It allocates/initializes data
  (sometimes reading a dataset file), calls the MLIR-derived kernel through the extern "C"
  signature, calls the reference implementation, and compares the results.
- **`drivers/ref/`** — the ground truth. `ref_<kernel>.c` is the original Rodinia C kernel,
  compiled with `gcc -O3` and linked against the MLIR object so outputs can be diffed.

## Scripts

### `test.sh` — baseline (mlir-opt path)

Lowers `<kernel>.mlir` with **stock `mlir-opt`** (never `dhir-opt`) through
`mlir-opt → mlir-translate → llc`, compiles `ref_<kernel>.c`, links
`driver + kernel.o + ref.o` with `mpicxx`, and runs with `mpirun`.

```bash
./tests/rodinia/test.sh nw            # one kernel, 1 rank
./tests/rodinia/test.sh all 4         # all kernels, 4 MPI ranks
./tests/rodinia/test.sh hotspot 2 --keep --verbose   # keep intermediates (/tmp dir)
```

Pass = driver prints a `RESULT ... status=PASS` line (kernel output matches the reference).

### `run_dhir.sh` — the DHIR compiler path

Same harness, but lowers via **`build/bin/dhir-opt`** with the DHIR pipeline
(`--affine-to-dhir --std-to-dhir --lower-replicate --lower-converge --dhir-to-mpi
--lower-to-llvm`) using `tests/configs/system_config_4_cpu.json`, then links the DHIR
runtime (`build/libdhir_runtime.a`). This is the path that actually exercises the DHIR
compiler — MPI calls are emitted by `--dhir-to-mpi`, and the DHIR runtime/OpenMP executes them.

```bash
./tests/rodinia/run_dhir.sh nw 2
./tests/rodinia/run_dhir.sh all 4
# overrides: DHIR_OPT=... CONFIG=... RUNTIME=... MLIR_TRANSLATE=... LLC=... DHIR_MPIRUN_FLAGS=...
# DHIR_OMP_LIBDIR=... env vars; same --keep / --verbose / --workdir options as test.sh
```

### `run_dhir_pipeline.sh` — compile-only diagnostics

Does **not** build or run binaries. For every `*.mlir` in the directory it reports, in a
table: (1) whether plain `mlir-opt` parses it, (2) whether stage 1
(`--affine-to-dhir`) succeeds, (3) whether the full DHIR pipeline succeeds, and on failure
which pass broke plus the first error line. Logs land in `test_output/`.

```bash
./tests/rodinia/run_dhir_pipeline.sh
```

## Requirements — run from the repo root

All three scripts must be invoked **from the repository root** (the drivers open dataset
files via repo-root-relative paths like `vendor/rodinia/rodinia_3.1/data/hotspot/temp_1024`).
The parent directory must contain the extracted Rodinia 3.1 datasets:

```
vendor/rodinia/rodinia_3.1/data/     # per-benchmark input files (power_1024, mil.txt, ...)
```

Kernels that read datasets include hotspot, hotspot3D, srad, pathfinder, lud, gaussian,
dwt2d, nw, bfs, b+tree, cfd, lavaMD, myocyte, particlefilter, kmeans, backprop, nn,
heartwall, leukocyte, streamcluster, hybridsort, huffman, mummergpu.

You also need the toolchain on PATH or at the defaults the scripts expect:
`mlir-opt` / `mlir-translate` / `llc` under `~/.local/bin`, `mpicxx`/`mpirun`, and
(for `run_dhir*.sh`) a built `build/bin/dhir-opt` + `build/libdhir_runtime.a`.
