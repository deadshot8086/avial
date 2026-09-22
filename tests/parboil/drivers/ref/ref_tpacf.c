#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

static void ref_do_pair(const float *p1, const float *p2, long long *hist, int nbins, const float *binb) {
  float dot = p1[0] * p2[0] + p1[1] * p2[1] + p1[2] * p2[2];
  int min = 0;
  int max = nbins;
  int k;
  while (max > min + 1) {
    k = (min + max) / 2;
    if (dot >= binb[k]) max = k;
    else min = k;
  }
  if (dot >= binb[min]) {
    hist[min] += 1;
  } else if (dot < binb[max]) {
    hist[max + 1] += 1;
  } else {
    hist[max] += 1;
  }
}

void ref_tpacf(int npoints, int random_count, int nbins,
               const float *data, const float *randoms, const float *binb,
               long long *DD, long long *RRS, long long *DRS) {
  /* 1. Zero out histograms */
  for (int k = 0; k < nbins + 2; k++) {
    DD[k] = 0;
    RRS[k] = 0;
    DRS[k] = 0;
  }

  /* 2. DD autocorrelation */
  for (int i = 0; i < npoints - 1; i++) {
    for (int j = i + 1; j < npoints; j++) {
      ref_do_pair(data + i * 3, data + j * 3, DD, nbins, binb);
    }
  }

  /* 3. Random sets sweep */
  for (int rf = 0; rf < random_count; rf++) {
    const float *r = randoms + (size_t)rf * npoints * 3;
    /* RR */
    for (int i = 0; i < npoints - 1; i++) {
      for (int j = i + 1; j < npoints; j++) {
        ref_do_pair(r + i * 3, r + j * 3, RRS, nbins, binb);
      }
    }
    /* DR */
    for (int i = 0; i < npoints; i++) {
      for (int j = 0; j < npoints; j++) {
        ref_do_pair(data + i * 3, r + j * 3, DRS, nbins, binb);
      }
    }
  }
}

#ifdef __cplusplus
}
#endif
