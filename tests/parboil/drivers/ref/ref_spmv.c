// Reference for spmv, independently derived

#ifdef __cplusplus
extern "C" {
#endif

void ref_spmv(int dim, int iters,
              const int *h_nzcnt,
              const int *h_ptr,
              const int *h_indices,
              const float *h_data,
              const float *h_x_vector,
              const int *h_perm,
              float *h_Ax_vector) {
  for (int p = 0; p < iters; p++) {
    for (int i = 0; i < dim; i++) {
      float sum = 0.0f;
      int bound = h_nzcnt[i];
      for (int k = 0; k < bound; k++) {
        int j = h_ptr[k] + i;
        int in = h_indices[j];

        float d = h_data[j];
        float t = h_x_vector[in];

        sum += d * t;
      }
      h_Ax_vector[h_perm[i]] = sum;
    }
  }
}

#ifdef __cplusplus
}
#endif
