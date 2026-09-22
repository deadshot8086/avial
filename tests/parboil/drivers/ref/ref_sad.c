// Independent flat-pointer reference for Parboil sad.

#include <stdlib.h>

#define SEARCH_RANGE 16
#define SEARCH_DIMENSION (2 * SEARCH_RANGE + 1)
#define MAX_POS 1089
#define MAX_POS_PADDED 1092

#define SAD_TYPE_1_IX(image_size) 0
#define SAD_TYPE_2_IX(image_size) ((image_size) * MAX_POS_PADDED)
#define SAD_TYPE_3_IX(image_size) ((image_size) * (3 * MAX_POS_PADDED))
#define SAD_TYPE_4_IX(image_size) ((image_size) * (5 * MAX_POS_PADDED))
#define SAD_TYPE_5_IX(image_size) ((image_size) * (9 * MAX_POS_PADDED))
#define SAD_TYPE_6_IX(image_size) ((image_size) * (17 * MAX_POS_PADDED))
#define SAD_TYPE_7_IX(image_size) ((image_size) * (25 * MAX_POS_PADDED))

#define SAD_TYPE_2_CT 2
#define SAD_TYPE_3_CT 2
#define SAD_TYPE_4_CT 4
#define SAD_TYPE_5_CT 8
#define SAD_TYPE_6_CT 8
#define SAD_TYPE_7_CT 16

#ifdef __cplusplus
extern "C" {
#endif

/* sad_cpu.c's sad4_one_macroblock, flat. `macroblock_sad` already points at this
 * macroblock's type-7 region; `frame` already points at its top-left pixel. */
static void ref_sad_one_macroblock(unsigned short *macroblock_sad,
                                   const unsigned short *frame,
                                   const unsigned short *ref,
                                   int frame_y, int frame_x,
                                   int width, int height) {
  int pos_x, pos_y, pos;

  pos = 0;
  for (pos_y = -SEARCH_RANGE; pos_y <= SEARCH_RANGE; pos_y++) {
    for (pos_x = -SEARCH_RANGE; pos_x <= SEARCH_RANGE; pos_x++, pos++) {
      int blky, blkx;

      for (blky = 0; blky < 4; blky++) {
        for (blkx = 0; blkx < 4; blkx++) {
          int y, x;
          unsigned short sad = 0;

          for (y = 0; y < 4; y++) {
            for (x = 0; x < 4; x++) {
              int ref_x, ref_y, d;
              unsigned int a, b;

              ref_x = frame_x + pos_x + (blkx * 4) + x;
              if (ref_x < 0) ref_x = 0;
              if (ref_x >= width) ref_x = width - 1;

              ref_y = frame_y + pos_y + (blky * 4) + y;
              if (ref_y < 0) ref_y = 0;
              if (ref_y >= height) ref_y = height - 1;

              b = ref[ref_y * width + ref_x];
              a = frame[(blky * 4 + y) * width + (blkx * 4 + x)];

              d = (int)a - (int)b;
              if (d < 0) d = -d;
              sad += (unsigned short)d;
            }
          }

          macroblock_sad[MAX_POS_PADDED * (4 * blky + blkx) + pos] = sad;
        }
      }
    }
  }
}

void ref_sad(int mb_width, int mb_height, int height, int width,
             const unsigned short *cur, const unsigned short *ref,
             unsigned short *sads) {
  const int mbs = mb_width * mb_height;
  int mb_x, mb_y, macroblock;

  /* Stage 1: sad4_cpu */
  for (mb_y = 0; mb_y < mb_height; mb_y++) {
    for (mb_x = 0; mb_x < mb_width; mb_x++) {
      ref_sad_one_macroblock(
          sads + SAD_TYPE_7_IX(mbs) +
              (mb_y * mb_width + mb_x) * (SAD_TYPE_7_CT * MAX_POS_PADDED),
          cur + (mb_y * 16) * width + mb_x * 16, ref, mb_y * 16, mb_x * 16,
          width, height);
    }
  }

  /* Stage 2: larger_sads */
  for (macroblock = 0; macroblock < mbs; macroblock++) {
    int block_x, block_y, count;
    const unsigned short *x;
    const unsigned short *y;
    unsigned short *z;

    /* Block type 6 */
    for (block_y = 0; block_y < 2; block_y++) {
      for (block_x = 0; block_x < 4; block_x++) {
        x = sads + SAD_TYPE_7_IX(mbs) +
            macroblock * SAD_TYPE_7_CT * MAX_POS_PADDED +
            (8 * block_y + block_x) * MAX_POS_PADDED;
        y = x + 4 * MAX_POS_PADDED;
        z = sads + SAD_TYPE_6_IX(mbs) +
            macroblock * SAD_TYPE_6_CT * MAX_POS_PADDED +
            (4 * block_y + block_x) * MAX_POS_PADDED;
        for (count = 0; count < MAX_POS; count++) *z++ = *x++ + *y++;
      }
    }

    /* Block type 5 */
    for (block_y = 0; block_y < 4; block_y++) {
      for (block_x = 0; block_x < 2; block_x++) {
        x = sads + SAD_TYPE_7_IX(mbs) +
            macroblock * SAD_TYPE_7_CT * MAX_POS_PADDED +
            (4 * block_y + 2 * block_x) * MAX_POS_PADDED;
        y = x + MAX_POS_PADDED;
        z = sads + SAD_TYPE_5_IX(mbs) +
            macroblock * SAD_TYPE_6_CT * MAX_POS_PADDED +
            (2 * block_y + block_x) * MAX_POS_PADDED;
        for (count = 0; count < MAX_POS; count++) *z++ = *x++ + *y++;
      }
    }

    /* Block type 4 */
    for (block_y = 0; block_y < 2; block_y++) {
      for (block_x = 0; block_x < 2; block_x++) {
        x = sads + SAD_TYPE_5_IX(mbs) +
            macroblock * SAD_TYPE_5_CT * MAX_POS_PADDED +
            (4 * block_y + block_x) * MAX_POS_PADDED;
        y = x + 2 * MAX_POS_PADDED;
        z = sads + SAD_TYPE_4_IX(mbs) +
            macroblock * SAD_TYPE_4_CT * MAX_POS_PADDED +
            (2 * block_y + block_x) * MAX_POS_PADDED;
        for (count = 0; count < MAX_POS; count++) *z++ = *x++ + *y++;
      }
    }

    /* Block type 3 */
    x = sads + SAD_TYPE_4_IX(mbs) + macroblock * SAD_TYPE_4_CT * MAX_POS_PADDED;
    y = x + 2 * MAX_POS_PADDED;
    z = sads + SAD_TYPE_3_IX(mbs) + macroblock * SAD_TYPE_3_CT * MAX_POS_PADDED;
    for (count = 0; count < MAX_POS; count++) *z++ = *x++ + *y++;

    x = sads + SAD_TYPE_4_IX(mbs) + macroblock * SAD_TYPE_4_CT * MAX_POS_PADDED +
        MAX_POS_PADDED;
    y = x + 2 * MAX_POS_PADDED;
    z = sads + SAD_TYPE_3_IX(mbs) + macroblock * SAD_TYPE_3_CT * MAX_POS_PADDED +
        MAX_POS_PADDED;
    for (count = 0; count < MAX_POS; count++) *z++ = *x++ + *y++;

    /* Block type 2 */
    x = sads + SAD_TYPE_4_IX(mbs) + macroblock * SAD_TYPE_4_CT * MAX_POS_PADDED;
    y = x + MAX_POS_PADDED;
    z = sads + SAD_TYPE_2_IX(mbs) + macroblock * SAD_TYPE_2_CT * MAX_POS_PADDED;
    for (count = 0; count < MAX_POS; count++) *z++ = *x++ + *y++;

    x = sads + SAD_TYPE_4_IX(mbs) + macroblock * SAD_TYPE_4_CT * MAX_POS_PADDED +
        2 * MAX_POS_PADDED;
    y = x + MAX_POS_PADDED;
    z = sads + SAD_TYPE_2_IX(mbs) + macroblock * SAD_TYPE_2_CT * MAX_POS_PADDED +
        MAX_POS_PADDED;
    for (count = 0; count < MAX_POS; count++) *z++ = *x++ + *y++;

    /* Block type 1 */
    x = sads + SAD_TYPE_2_IX(mbs) + macroblock * SAD_TYPE_2_CT * MAX_POS_PADDED;
    y = x + MAX_POS_PADDED;
    z = sads + SAD_TYPE_1_IX(mbs) + macroblock * MAX_POS_PADDED;
    for (count = 0; count < MAX_POS; count++) *z++ = *x++ + *y++;
  }
}

#ifdef __cplusplus
}
#endif
