#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "avilib.c"

#ifdef __cplusplus
extern "C" {
#endif

#define TOP 110
#define BOTTOM 328
#define NCIRCLES 7
#define NPOINTS 150
#define STREL_M 25
#define STREL_N 25

int load_avi_gradients(const char *filename, int frame_num,
                       int *out_width, int *out_height,
                       double **out_grad_x, double **out_grad_y) {
    avi_t *cell_file = AVI_open_input_file((char *)filename, 1);
    if (!cell_file) {
        fprintf(stderr, "Error opening AVI file: %s\n", filename);
        return -1;
    }

    int full_width = AVI_video_width(cell_file);
    int full_height = AVI_video_height(cell_file);

    int top = TOP;
    int bottom = BOTTOM;
    if (bottom >= full_height) {
        top = 0;
        bottom = full_height - 1;
    }

    int H = bottom - top + 1;
    int W = full_width;

    *out_width = W;
    *out_height = H;

    unsigned char *image_buf = (unsigned char *)malloc(full_width * full_height);
    if (!image_buf) {
        AVI_close(cell_file);
        return -2;
    }

    AVI_set_video_position(cell_file, frame_num);
    int dummy = 0;
    if (AVI_read_frame(cell_file, (char *)image_buf, &dummy) == -1) {
        fprintf(stderr, "Error reading frame from AVI\n");
        free(image_buf);
        AVI_close(cell_file);
        return -3;
    }

    AVI_close(cell_file);

    // Crop and flip image (matching Rodinia chop_flip_image with scaled=0)
    double *image = (double *)malloc(H * W * sizeof(double));
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            image[i * W + j] = (double)image_buf[((full_height - 1 - (i + top)) * full_width) + j];
        }
    }
    free(image_buf);

    // Compute gradient_x and gradient_y (matching Rodinia misc_math.c in double precision)
    double *gx = (double *)malloc(H * W * sizeof(double));
    double *gy = (double *)malloc(H * W * sizeof(double));

    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (j == 0)
                gx[i * W + j] = image[i * W + (j + 1)] - image[i * W + j];
            else if (j == W - 1)
                gx[i * W + j] = image[i * W + j] - image[i * W + (j - 1)];
            else
                gx[i * W + j] = (image[i * W + (j + 1)] - image[i * W + (j - 1)]) / 2.0;
        }
    }

    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (i == 0)
                gy[i * W + j] = image[(i + 1) * W + j] - image[i * W + j];
            else if (i == H - 1)
                gy[i * W + j] = image[i * W + j] - image[(i - 1) * W + j];
            else
                gy[i * W + j] = (image[(i + 1) * W + j] - image[(i - 1) * W + j]) / 2.0;
        }
    }

    free(image);

    *out_grad_x = gx;
    *out_grad_y = gy;
    return 0;
}

void ref_leukocyte(int width, int height, int MaxR,
                   const double *grad_x,
                   const double *grad_y,
                   const int *tX,
                   const int *tY,
                   const double *cos_angle,
                   const double *sin_angle,
                   const int *strel,
                   double *Grad,
                   double *gicov,
                   double *dilated) {
    // 1. GICOV Stencil Reduction
    for (int i = MaxR; i < width - MaxR; i++) {
        for (int j = MaxR; j < height - MaxR; j++) {
            double max_GICOV = 0.0;
            for (int k = 0; k < NCIRCLES; k++) {
                double sum = 0.0;
                for (int n = 0; n < NPOINTS; n++) {
                    int x = i + tX[k * NPOINTS + n];
                    int y = j + tY[k * NPOINTS + n];
                    double val = grad_x[y * width + x] * cos_angle[n] + grad_y[y * width + x] * sin_angle[n];
                    Grad[n] = val;
                    sum += val;
                }
                double mean = sum / (double)NPOINTS;
                double var = 0.0;
                for (int n = 0; n < NPOINTS; n++) {
                    double diff = Grad[n] - mean;
                    var += diff * diff;
                }
                var = var / (double)(NPOINTS - 1);
                double GICOV = 0.0;
                if (var > 1e-12) {
                    GICOV = (mean * mean) / var;
                }
                if (GICOV > max_GICOV) max_GICOV = GICOV;
            }
            gicov[j * width + i] = max_GICOV;
        }
    }

    // 2. Morphological Dilation using circular structuring element (R=12)
    int el_center_i = STREL_M / 2;
    int el_center_j = STREL_N / 2;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double max_val = 0.0;
            for (int el_i = 0; el_i < STREL_M; el_i++) {
                int y = i - el_center_i + el_i;
                if (y >= 0 && y < height) {
                    for (int el_j = 0; el_j < STREL_N; el_j++) {
                        int x = j - el_center_j + el_j;
                        if (x >= 0 && x < width && strel[el_i * STREL_N + el_j] != 0) {
                            double temp = gicov[y * width + x];
                            if (temp > max_val) {
                                max_val = temp;
                            }
                        }
                    }
                }
            }
            dilated[i * width + j] = max_val;
        }
    }
}

#ifdef __cplusplus
}
#endif
