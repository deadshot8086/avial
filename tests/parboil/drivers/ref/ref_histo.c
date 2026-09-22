// Independent flat-pointer reference for Parboil histo.

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_histo(int img_width, int img_height, int histo_width, int histo_height,
               int iters,
               const unsigned int *img, unsigned char *histo) {
  const unsigned int nbins = (unsigned int)histo_width * (unsigned int)histo_height;
  const long npixels = (long)img_width * (long)img_height;

  for (int it = 0; it < iters; ++it) {
    for (unsigned int b = 0; b < nbins; ++b) {
      histo[b] = 0;
    }
    for (long i = 0; i < npixels; ++i) {
      const unsigned int value = img[i];
      if (value < nbins) {
        if (histo[value] < 255) {
          ++histo[value];
        }
      }
    }
  }
}

#ifdef __cplusplus
}
#endif
