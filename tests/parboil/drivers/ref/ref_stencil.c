// Reference for the Parboil stencil kernel with iterations.

#ifdef __cplusplus
extern "C" {
#endif

#define REF_IDX3D(_nx, _ny, _i, _j, _k) ((_i) + (_nx) * ((_j) + (_ny) * (_k)))

#define REF_SWEEP(src, dst) do { \
  for (int k = 1; k < nz - 1; k++) { \
    for (int j = 1; j < ny - 1; j++) { \
      for (int i = 1; i < nx - 1; i++) { \
        dst[REF_IDX3D(nx, ny, i, j, k)] = \
            (src[REF_IDX3D(nx, ny, i, j, k + 1)] + \
             src[REF_IDX3D(nx, ny, i, j, k - 1)] + \
             src[REF_IDX3D(nx, ny, i, j + 1, k)] + \
             src[REF_IDX3D(nx, ny, i, j - 1, k)] + \
             src[REF_IDX3D(nx, ny, i + 1, j, k)] + \
             src[REF_IDX3D(nx, ny, i - 1, j, k)]) * c1 \
            - src[REF_IDX3D(nx, ny, i, j, k)] * c0; \
      } \
    } \
  } \
} while (0)

void ref_stencil(int nx, int ny, int nz, int iterations,
                 float *A0, float *Anext,
                 float c0, float c1) {
  for (int t = 0; t < iterations / 2; t++) {
    REF_SWEEP(A0, Anext);
    REF_SWEEP(Anext, A0);
  }
  if (iterations % 2 == 1) {
    REF_SWEEP(A0, Anext);
  }
}

#ifdef __cplusplus
}
#endif
