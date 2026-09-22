#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int mirror(int d, int sizeD) {
    if (d >= sizeD) {
        return 2 * sizeD - 2 - d;
    } else if (d < 0) {
        return -d;
    }
    return d;
}

void ref_dwt2d(int width, int height, int levels, int components,
               const int *in, int *vert, int *out) {
    long num_pixels = (long)width * height;
    for (int c = 0; c < components; c++) {
        const int *in_c = in + c * num_pixels;
        int *out_c = out + c * num_pixels;

        // Initialize out_c with in_c
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                out_c[y * width + x] = in_c[y * width + x];
            }
        }

        int cur_w = width;
        int cur_h = height;

        for (int lvl = 0; lvl < levels; lvl++) {
            int hx = cur_w / 2;
            int hy = cur_h / 2;

            // Copy current LL subband from out_c to vert
            for (int y = 0; y < cur_h; y++) {
                for (int x = 0; x < cur_w; x++) {
                    vert[y * width + x] = out_c[y * width + x];
                }
            }

            // 1. Vertical Transform (along columns)
            for (int x = 0; x < cur_w; x++) {
                // Vertical Predict (odd rows)
                for (int y = 1; y < cur_h; y += 2) {
                    int p = vert[(y - 1) * width + x];
                    int n = vert[mirror(y + 1, cur_h) * width + x];
                    vert[y * width + x] -= (p + n) / 2;
                }
                // Vertical Update (even rows)
                for (int y = 0; y < cur_h; y += 2) {
                    int p = vert[mirror(y - 1, cur_h) * width + x];
                    int n = vert[mirror(y + 1, cur_h) * width + x];
                    vert[y * width + x] += (p + n + 2) / 4;
                }
            }

            // 2. Horizontal Transform (along rows)
            for (int y = 0; y < cur_h; y++) {
                // Horizontal Predict (odd cols)
                for (int x = 1; x < cur_w; x += 2) {
                    int p = vert[y * width + (x - 1)];
                    int n = vert[y * width + mirror(x + 1, cur_w)];
                    vert[y * width + x] -= (p + n) / 2;
                }
                // Horizontal Update (even cols)
                for (int x = 0; x < cur_w; x += 2) {
                    int p = vert[y * width + mirror(x - 1, cur_w)];
                    int n = vert[y * width + mirror(x + 1, cur_w)];
                    vert[y * width + x] += (p + n + 2) / 4;
                }
            }

            // 3. Store into visual quadtree layout within [0..cur_h-1][0..cur_w-1]
            for (int r = 0; r < hy; r++) {
                for (int col = 0; col < hx; col++) {
                    out_c[r * width + col] = vert[(2 * r) * width + (2 * col)];
                    out_c[r * width + (hx + col)] = vert[(2 * r) * width + (2 * col + 1)];
                    out_c[(hy + r) * width + col] = vert[(2 * r + 1) * width + (2 * col)];
                    out_c[(hy + r) * width + (hx + col)] = vert[(2 * r + 1) * width + (2 * col + 1)];
                }
            }

            cur_w = hx;
            cur_h = hy;
        }
    }
}

#ifdef __cplusplus
}
#endif
