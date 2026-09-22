// Flat-pointer reference for the Parboil cutcp kernel.
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_cutcp(int natoms, const float *atoms,
               int nx, int ny, int nz, float *lattice,
               float xlo, float ylo, float zlo,
               float gridspacing, float inv_gridspacing, int radius,
               float a2, float inv_a2)
{
  int n;
  int i, j, k;
  int ia, ib, ic;
  int ja, jb, jc;
  int ka, kb, kc;
  int koff, jkoff;
  float x, y, z, q;
  float dx, dy, dz;
  float dz2, dydz2, r2;
  float s, e;
  float xstart, ystart;

  for (n = 0; n < natoms; n++) {
    q = atoms[n * 4 + 3];
    if (0 == q) continue;   /* non-contributing atom: never binned in cutcpu.c */

    x = atoms[n * 4 + 0] - xlo;
    y = atoms[n * 4 + 1] - ylo;
    z = atoms[n * 4 + 2] - zlo;

    /* find closest grid point with position less than or equal to atom */
    ic = (int) (x * inv_gridspacing);
    jc = (int) (y * inv_gridspacing);
    kc = (int) (z * inv_gridspacing);

    /* find extent of surrounding box of grid points */
    ia = ic - radius;
    ib = ic + radius + 1;
    ja = jc - radius;
    jb = jc + radius + 1;
    ka = kc - radius;
    kb = kc + radius + 1;

    /* trim box edges so that they are within grid point lattice */
    if (ia < 0)   ia = 0;
    if (ib >= nx) ib = nx - 1;
    if (ja < 0)   ja = 0;
    if (jb >= ny) jb = ny - 1;
    if (ka < 0)   ka = 0;
    if (kb >= nz) kb = nz - 1;

    /* loop over surrounding grid points */
    xstart = ia * gridspacing - x;
    ystart = ja * gridspacing - y;
    dz = ka * gridspacing - z;
    for (k = ka; k <= kb; k++, dz += gridspacing) {
      koff = k * ny;
      dz2 = dz * dz;
      dy = ystart;
      for (j = ja; j <= jb; j++, dy += gridspacing) {
        jkoff = (koff + j) * nx;
        dydz2 = dy * dy + dz2;
        if (dydz2 >= a2) continue;   /* CHECK_CYLINDER_CPU */

        dx = xstart;
        for (i = ia; i <= ib; i++, dx += gridspacing) {
          r2 = dx * dx + dydz2;
          if (r2 >= a2) continue;
          s = (1.f - r2 * inv_a2);
          e = q * (1 / sqrtf(r2)) * s * s;
          lattice[jkoff + i] += e;
        }
      }
    } /* end loop over surrounding grid points */
  } /* end loop over atoms */
}

#ifdef __cplusplus
}
#endif
