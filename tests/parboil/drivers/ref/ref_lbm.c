// Flat-pointer reference for the Parboil lbm stream-collide kernel.


#ifdef __cplusplus
extern "C" {
#endif

#define REF_NCE 20 /* N_CELL_ENTRIES */

#define REF_OMEGA 1.95
#define REF_DFL1 (1.0 / 3.0)
#define REF_DFL2 (1.0 / 18.0)
#define REF_DFL3 (1.0 / 36.0)

#define REF_OBSTACLE (1 << 0)
#define REF_ACCEL (1 << 1)

/* CELL_ENTRIES enum from lbm_1d_array.h */
enum {
  REF_C = 0,
  REF_N,
  REF_S,
  REF_E,
  REF_W,
  REF_T,
  REF_B,
  REF_NE,
  REF_NW,
  REF_SE,
  REF_SW,
  REF_NT,
  REF_NB,
  REF_ST,
  REF_SB,
  REF_ET,
  REF_EB,
  REF_WT,
  REF_WB,
  REF_FLAGS
};

/* nzt is the total number of z planes INCLUDING the 2+2 ghost planes; the
 * logical volume is nz = nzt - 4 and src/dst point at the start of the whole
 * allocation, so the sweep starts two planes in. */
static void ref_sweep(int nzt, int ny, int nx, const double *src, double *dst,
                      const int *flag) {
  const long plane = (long)ny * nx * REF_NCE; /* CALC_INDEX z stride */
  const long row = (long)nx * REF_NCE;        /* CALC_INDEX y stride */
  const long cell = REF_NCE;                  /* CALC_INDEX x stride */

  /* CALC_INDEX(dx,dy,dz,0) for each lattice direction */
  const long d_C = 0;
  const long d_N = +row;
  const long d_S = -row;
  const long d_E = +cell;
  const long d_W = -cell;
  const long d_T = +plane;
  const long d_B = -plane;
  const long d_NE = +cell + row;
  const long d_NW = -cell + row;
  const long d_SE = +cell - row;
  const long d_SW = -cell - row;
  const long d_NT = +row + plane;
  const long d_NB = +row - plane;
  const long d_ST = -row + plane;
  const long d_SB = -row - plane;
  const long d_ET = +cell + plane;
  const long d_EB = +cell - plane;
  const long d_WT = -cell + plane;
  const long d_WB = -cell - plane;

  const long first = 2 * plane;                   /* skip the lower margin */
  const long last = (long)(nzt - 2) * plane;      /* stop before the upper one */

  long i;
  for (i = first; i < last; i += REF_NCE) {
    const long c = i / REF_NCE; /* cell number, for the split flag array */

    const double sC = src[i + REF_C];
    const double sN = src[i + REF_N];
    const double sS = src[i + REF_S];
    const double sE = src[i + REF_E];
    const double sW = src[i + REF_W];
    const double sT = src[i + REF_T];
    const double sB = src[i + REF_B];
    const double sNE = src[i + REF_NE];
    const double sNW = src[i + REF_NW];
    const double sSE = src[i + REF_SE];
    const double sSW = src[i + REF_SW];
    const double sNT = src[i + REF_NT];
    const double sNB = src[i + REF_NB];
    const double sST = src[i + REF_ST];
    const double sSB = src[i + REF_SB];
    const double sET = src[i + REF_ET];
    const double sEB = src[i + REF_EB];
    const double sWT = src[i + REF_WT];
    const double sWB = src[i + REF_WB];

    const int f = flag[c];

    if (f & REF_OBSTACLE) {
      dst[i + d_C + REF_C] = sC;
      dst[i + d_S + REF_S] = sN;
      dst[i + d_N + REF_N] = sS;
      dst[i + d_W + REF_W] = sE;
      dst[i + d_E + REF_E] = sW;
      dst[i + d_B + REF_B] = sT;
      dst[i + d_T + REF_T] = sB;
      dst[i + d_SW + REF_SW] = sNE;
      dst[i + d_SE + REF_SE] = sNW;
      dst[i + d_NW + REF_NW] = sSE;
      dst[i + d_NE + REF_NE] = sSW;
      dst[i + d_SB + REF_SB] = sNT;
      dst[i + d_ST + REF_ST] = sNB;
      dst[i + d_NB + REF_NB] = sST;
      dst[i + d_NT + REF_NT] = sSB;
      dst[i + d_WB + REF_WB] = sET;
      dst[i + d_WT + REF_WT] = sEB;
      dst[i + d_EB + REF_EB] = sWT;
      dst[i + d_ET + REF_ET] = sWB;
      continue;
    }

    double rho = +sC + sN + sS + sE + sW + sT + sB + sNE + sNW + sSE + sSW +
                 sNT + sNB + sST + sSB + sET + sEB + sWT + sWB;

    double ux = +sE - sW + sNE - sNW + sSE - sSW + sET + sEB - sWT - sWB;
    double uy = +sN - sS + sNE + sNW - sSE - sSW + sNT + sNB - sST - sSB;
    double uz = +sT - sB + sNT - sNB + sST - sSB + sET - sEB + sWT - sWB;

    ux /= rho;
    uy /= rho;
    uz /= rho;

    if (f & REF_ACCEL) {
      ux = 0.005;
      uy = 0.002;
      uz = 0.000;
    }

    const double u2 = 1.5 * (ux * ux + uy * uy + uz * uz);

    dst[i + d_C + REF_C] = (1.0 - REF_OMEGA) * sC + REF_DFL1 * REF_OMEGA * rho * (1.0 - u2);

    dst[i + d_N + REF_N] = (1.0 - REF_OMEGA) * sN + REF_DFL2 * REF_OMEGA * rho * (1.0 + uy * (4.5 * uy + 3.0) - u2);
    dst[i + d_S + REF_S] = (1.0 - REF_OMEGA) * sS + REF_DFL2 * REF_OMEGA * rho * (1.0 + uy * (4.5 * uy - 3.0) - u2);
    dst[i + d_E + REF_E] = (1.0 - REF_OMEGA) * sE + REF_DFL2 * REF_OMEGA * rho * (1.0 + ux * (4.5 * ux + 3.0) - u2);
    dst[i + d_W + REF_W] = (1.0 - REF_OMEGA) * sW + REF_DFL2 * REF_OMEGA * rho * (1.0 + ux * (4.5 * ux - 3.0) - u2);
    dst[i + d_T + REF_T] = (1.0 - REF_OMEGA) * sT + REF_DFL2 * REF_OMEGA * rho * (1.0 + uz * (4.5 * uz + 3.0) - u2);
    dst[i + d_B + REF_B] = (1.0 - REF_OMEGA) * sB + REF_DFL2 * REF_OMEGA * rho * (1.0 + uz * (4.5 * uz - 3.0) - u2);

    dst[i + d_NE + REF_NE] = (1.0 - REF_OMEGA) * sNE + REF_DFL3 * REF_OMEGA * rho * (1.0 + (+ux + uy) * (4.5 * (+ux + uy) + 3.0) - u2);
    dst[i + d_NW + REF_NW] = (1.0 - REF_OMEGA) * sNW + REF_DFL3 * REF_OMEGA * rho * (1.0 + (-ux + uy) * (4.5 * (-ux + uy) + 3.0) - u2);
    dst[i + d_SE + REF_SE] = (1.0 - REF_OMEGA) * sSE + REF_DFL3 * REF_OMEGA * rho * (1.0 + (+ux - uy) * (4.5 * (+ux - uy) + 3.0) - u2);
    dst[i + d_SW + REF_SW] = (1.0 - REF_OMEGA) * sSW + REF_DFL3 * REF_OMEGA * rho * (1.0 + (-ux - uy) * (4.5 * (-ux - uy) + 3.0) - u2);
    dst[i + d_NT + REF_NT] = (1.0 - REF_OMEGA) * sNT + REF_DFL3 * REF_OMEGA * rho * (1.0 + (+uy + uz) * (4.5 * (+uy + uz) + 3.0) - u2);
    dst[i + d_NB + REF_NB] = (1.0 - REF_OMEGA) * sNB + REF_DFL3 * REF_OMEGA * rho * (1.0 + (+uy - uz) * (4.5 * (+uy - uz) + 3.0) - u2);
    dst[i + d_ST + REF_ST] = (1.0 - REF_OMEGA) * sST + REF_DFL3 * REF_OMEGA * rho * (1.0 + (-uy + uz) * (4.5 * (-uy + uz) + 3.0) - u2);
    dst[i + d_SB + REF_SB] = (1.0 - REF_OMEGA) * sSB + REF_DFL3 * REF_OMEGA * rho * (1.0 + (-uy - uz) * (4.5 * (-uy - uz) + 3.0) - u2);
    dst[i + d_ET + REF_ET] = (1.0 - REF_OMEGA) * sET + REF_DFL3 * REF_OMEGA * rho * (1.0 + (+ux + uz) * (4.5 * (+ux + uz) + 3.0) - u2);
    dst[i + d_EB + REF_EB] = (1.0 - REF_OMEGA) * sEB + REF_DFL3 * REF_OMEGA * rho * (1.0 + (+ux - uz) * (4.5 * (+ux - uz) + 3.0) - u2);
    dst[i + d_WT + REF_WT] = (1.0 - REF_OMEGA) * sWT + REF_DFL3 * REF_OMEGA * rho * (1.0 + (-ux + uz) * (4.5 * (-ux + uz) + 3.0) - u2);
    dst[i + d_WB + REF_WB] = (1.0 - REF_OMEGA) * sWB + REF_DFL3 * REF_OMEGA * rho * (1.0 + (-ux - uz) * (4.5 * (-ux - uz) + 3.0) - u2);
  }
}

void ref_lbm(int nzt, int ny, int nx, int steps, double *src, double *dst,
             const int *flag) {
  for (int t = 0; t < steps / 2; t++) {
    ref_sweep(nzt, ny, nx, src, dst, flag);
    ref_sweep(nzt, ny, nx, dst, src, flag);
  }
  if (steps % 2 == 1) {
    ref_sweep(nzt, ny, nx, src, dst, flag);
  }
}

#ifdef __cplusplus
}
#endif
