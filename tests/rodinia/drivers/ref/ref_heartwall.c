#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "avilib.c"
#include "avimod.c"

#ifdef __cplusplus
extern "C" {
#endif

static avi_t *g_avi_file = NULL;

int hw_load_avi_frame(const char *avi_path, int frame_no, int *out_rows, int *out_cols, float **out_frame) {
    if (!g_avi_file) {
        g_avi_file = (avi_t *)AVI_open_input_file((char *)avi_path, 1);
        if (!g_avi_file) {
            fprintf(stderr, "Error opening AVI file: %s\n", avi_path);
            return -1;
        }
    }
    int total_frames = AVI_video_frames(g_avi_file);
    if (frame_no < 0 || frame_no >= total_frames) {
        fprintf(stderr, "Invalid frame number: %d (total: %d)\n", frame_no, total_frames);
        return -1;
    }
    *out_rows = AVI_video_height(g_avi_file);
    *out_cols = AVI_video_width(g_avi_file);
    *out_frame = get_frame(g_avi_file, frame_no, 0, 0, 1);
    return 0;
}

void hw_free_avi_frame(float *frame) {
    if (frame) free(frame);
}

int hw_get_total_frames(const char *avi_path) {
    if (!g_avi_file) {
        g_avi_file = (avi_t *)AVI_open_input_file((char *)avi_path, 1);
        if (!g_avi_file) return -1;
    }
    return AVI_video_frames(g_avi_file);
}

void hw_close_avi(void) {
    if (g_avi_file) {
        AVI_close(g_avi_file);
        g_avi_file = NULL;
    }
}

void ref_heartwall(
    int frames_processed, int allPoints, int total_avi_frames,
    int frame_rows, int frame_cols,
    int in_mod_rows, int in_mod_cols,
    int in2_rows, int in2_cols,
    int conv_rows, int conv_cols,
    int in2_pad_rows, int in2_pad_cols,
    int in2_sub_rows, int in2_sub_cols,
    int in2_sub2_sqr_rows, int in2_sub2_sqr_cols,
    int mask_rows, int mask_cols,
    int mask_conv_ioffset, int mask_conv_joffset,
    int in2_pad_cumv_sel_rowlow, int in2_pad_cumv_sel2_rowlow,
    int in2_sub_cumh_sel_collow, int in2_sub_cumh_sel2_collow,
    int sSize, int tSize, float alpha,
    const float *frames,
    int *baseRow, int *baseCol,
    float *d_T,
    int *tRowLoc,
    int *tColLoc,
    float *d_in2,
    float *d_in2_sqr,
    float *d_in_mod,
    float *d_in_sqr,
    float *d_conv,
    float *d_in2_pad,
    float *d_in2_sub,
    float *d_in2_sub2_sqr,
    float *d_tMask,
    float *d_mask_conv
) {
    int in_mod_elem = in_mod_rows * in_mod_cols;
    int in2_pad_add_rows = in_mod_rows;
    int in2_pad_add_cols = in_mod_cols;
    long frame_pixels = (long)frame_cols * frame_rows;

    for (int f = 1; f < frames_processed; f++) {
        const float *cur_frame = frames + (long)f * frame_pixels;

        for (int p = 0; p < allPoints; p++) {
            int orig_row = baseRow[p];
            int orig_col = baseCol[p];
            int prev_row = tRowLoc[(f - 1) + p * total_avi_frames];
            int prev_col = tColLoc[(f - 1) + p * total_avi_frames];
            float *T_p = d_T + (long)p * in_mod_elem;

            int in2_rowlow = orig_row - sSize;
            int in2_collow = orig_col - sSize;

            // 1. Crop in2 and in2_sqr
            for (int col = 0; col < in2_cols; col++) {
                for (int row = 0; row < in2_rows; row++) {
                    int ori_r = row + in2_rowlow - 1;
                    int ori_c = col + in2_collow - 1;
                    float val = cur_frame[ori_c * frame_rows + ori_r];
                    d_in2[col * in2_rows + row] = val;
                    d_in2_sqr[col * in2_rows + row] = val * val;
                }
            }

            // 2. Rotate template into in_mod and in_sqr
            for (int col = 0; col < in_mod_cols; col++) {
                for (int row = 0; row < in_mod_rows; row++) {
                    int rot_r = (in_mod_rows - 1) - row;
                    int rot_c = (in_mod_cols - 1) - col;
                    float val = T_p[rot_c * in_mod_rows + rot_r];
                    d_in_mod[col * in_mod_rows + row] = val;
                    d_in_sqr[rot_c * in_mod_rows + rot_r] = val * val;
                }
            }

            // 3. Template stats
            float in_final_sum = 0.0f;
            float in_sqr_final_sum = 0.0f;
            for (int col = 0; col < in_mod_cols; col++) {
                for (int row = 0; row < in_mod_rows; row++) {
                    in_final_sum += T_p[col * in_mod_rows + row];
                    in_sqr_final_sum += d_in_sqr[col * in_mod_rows + row];
                }
            }
            float mean = in_final_sum / (float)in_mod_elem;
            float variance = (in_sqr_final_sum / (float)in_mod_elem) - (mean * mean);
            float deviation = sqrtf(variance);
            float denomT = sqrtf((float)(in_mod_elem - 1)) * deviation;

            // 4. 2D cross correlation
            for (int col = 0; col < conv_cols; col++) {
                int j = col + 1;
                int jp1 = j + 1;
                int ja1 = (in2_cols < jp1) ? jp1 - in2_cols : 1;
                int ja2 = (in_mod_cols < j) ? in_mod_cols : j;

                for (int row = 0; row < conv_rows; row++) {
                    int i = row + 1;
                    int ip1 = i + 1;
                    int ia1 = (in2_rows < ip1) ? ip1 - in2_rows : 1;
                    int ia2 = (in_mod_rows < i) ? in_mod_rows : i;

                    float s = 0.0f;
                    for (int ja = ja1; ja <= ja2; ja++) {
                        int jb = jp1 - ja;
                        for (int ia = ia1; ia <= ia2; ia++) {
                            int ib = ip1 - ia;
                            s += d_in_mod[(ja - 1) * in_mod_rows + (ia - 1)] *
                                 d_in2[(jb - 1) * in2_rows + (ib - 1)];
                        }
                    }
                    d_conv[col * conv_rows + row] = s;
                }
            }

            // 5. Local Sum 1
            for (int col = 0; col < in2_pad_cols; col++) {
                for (int row = 0; row < in2_pad_rows; row++) {
                    if (row >= in2_pad_add_rows && row < in2_pad_add_rows + in2_rows &&
                        col >= in2_pad_add_cols && col < in2_pad_add_cols + in2_cols) {
                        d_in2_pad[col * in2_pad_rows + row] =
                            d_in2[(col - in2_pad_add_cols) * in2_rows + (row - in2_pad_add_rows)];
                    } else {
                        d_in2_pad[col * in2_pad_rows + row] = 0.0f;
                    }
                }
            }

            for (int col = 0; col < in2_pad_cols; col++) {
                float sum = 0.0f;
                for (int row = 0; row < in2_pad_rows; row++) {
                    d_in2_pad[col * in2_pad_rows + row] += sum;
                    sum = d_in2_pad[col * in2_pad_rows + row];
                }
            }

            for (int col = 0; col < in2_sub_cols; col++) {
                for (int row = 0; row < in2_sub_rows; row++) {
                    int r1 = row + in2_pad_cumv_sel_rowlow - 1;
                    int r2 = row + in2_pad_cumv_sel2_rowlow - 1;
                    d_in2_sub[col * in2_sub_rows + row] =
                        d_in2_pad[col * in2_pad_rows + r1] - d_in2_pad[col * in2_pad_rows + r2];
                }
            }

            for (int row = 0; row < in2_sub_rows; row++) {
                float sum = 0.0f;
                for (int col = 0; col < in2_sub_cols; col++) {
                    d_in2_sub[col * in2_sub_rows + row] += sum;
                    sum = d_in2_sub[col * in2_sub_rows + row];
                }
            }

            for (int col = 0; col < in2_sub2_sqr_cols; col++) {
                for (int row = 0; row < in2_sub2_sqr_rows; row++) {
                    int c1 = col + in2_sub_cumh_sel_collow - 1;
                    int c2 = col + in2_sub_cumh_sel2_collow - 1;
                    float temp2 = d_in2_sub[c1 * in2_sub_rows + row] - d_in2_sub[c2 * in2_sub_rows + row];
                    d_in2_sub2_sqr[col * in2_sub2_sqr_rows + row] = temp2 * temp2;
                    d_conv[col * conv_rows + row] -= temp2 * in_final_sum / (float)in_mod_elem;
                }
            }

            // 6. Local Sum 2
            for (int col = 0; col < in2_pad_cols; col++) {
                for (int row = 0; row < in2_pad_rows; row++) {
                    if (row >= in2_pad_add_rows && row < in2_pad_add_rows + in2_rows &&
                        col >= in2_pad_add_cols && col < in2_pad_add_cols + in2_cols) {
                        d_in2_pad[col * in2_pad_rows + row] =
                            d_in2_sqr[(col - in2_pad_add_cols) * in2_rows + (row - in2_pad_add_rows)];
                    } else {
                        d_in2_pad[col * in2_pad_rows + row] = 0.0f;
                    }
                }
            }

            for (int col = 0; col < in2_pad_cols; col++) {
                float sum = 0.0f;
                for (int row = 0; row < in2_pad_rows; row++) {
                    d_in2_pad[col * in2_pad_rows + row] += sum;
                    sum = d_in2_pad[col * in2_pad_rows + row];
                }
            }

            for (int col = 0; col < in2_sub_cols; col++) {
                for (int row = 0; row < in2_sub_rows; row++) {
                    int r1 = row + in2_pad_cumv_sel_rowlow - 1;
                    int r2 = row + in2_pad_cumv_sel2_rowlow - 1;
                    d_in2_sub[col * in2_sub_rows + row] =
                        d_in2_pad[col * in2_pad_rows + r1] - d_in2_pad[col * in2_pad_rows + r2];
                }
            }

            for (int row = 0; row < in2_sub_rows; row++) {
                float sum = 0.0f;
                for (int col = 0; col < in2_sub_cols; col++) {
                    d_in2_sub[col * in2_sub_rows + row] += sum;
                    sum = d_in2_sub[col * in2_sub_rows + row];
                }
            }

            for (int col = 0; col < conv_cols; col++) {
                for (int row = 0; row < conv_rows; row++) {
                    int c1 = col + in2_sub_cumh_sel_collow - 1;
                    int c2 = col + in2_sub_cumh_sel2_collow - 1;
                    float temp2 = d_in2_sub[c1 * in2_sub_rows + row] - d_in2_sub[c2 * in2_sub_rows + row];
                    temp2 -= (d_in2_sub2_sqr[col * in2_sub2_sqr_rows + row] / (float)in_mod_elem);
                    if (temp2 < 0.0f) temp2 = 0.0f;
                    temp2 = sqrtf(temp2);
                    temp2 = denomT * temp2;
                    d_conv[col * conv_rows + row] /= temp2;
                }
            }

            // 7. Template mask setup
            for (int col = 0; col < conv_cols; col++) {
                for (int row = 0; row < conv_rows; row++) {
                    d_tMask[col * conv_rows + row] = 0.0f;
                }
            }
            int cent = sSize + tSize + 1;
            int tMask_row = cent + prev_row - orig_row - 1;
            int tMask_col = cent + prev_col - orig_col - 1;
            if (tMask_col >= 0 && tMask_col < conv_cols && tMask_row >= 0 && tMask_row < conv_rows) {
                d_tMask[tMask_col * conv_rows + tMask_row] = 1.0f;
            }

            // Motion mask convolution
            for (int col = 0; col < conv_cols; col++) {
                int j = col + 1 + mask_conv_joffset;
                int jp1 = j + 1;
                int ja1 = (mask_cols < jp1) ? jp1 - mask_cols : 1;
                int ja2 = (conv_cols < j) ? conv_cols : j;

                for (int row = 0; row < conv_rows; row++) {
                    int i = row + 1 + mask_conv_ioffset;
                    int ip1 = i + 1;
                    int ia1 = (mask_rows < ip1) ? ip1 - mask_rows : 1;
                    int ia2 = (conv_rows < i) ? conv_rows : i;

                    float s = 0.0f;
                    for (int ja = ja1; ja <= ja2; ja++) {
                        int jb = jp1 - ja;
                        for (int ia = ia1; ia <= ia2; ia++) {
                            int ib = ip1 - ia;
                            s += d_tMask[(ja - 1) * conv_rows + (ia - 1)];
                        }
                    }
                    d_mask_conv[col * conv_rows + row] = d_conv[col * conv_rows + row] * s;
                }
            }

            // 8. ArgMax Search
            float fin_max_val = 0.0f;
            int fin_max_coo = 0;
            for (int col = 0; col < conv_cols; col++) {
                for (int row = 0; row < conv_rows; row++) {
                    int idx = col * conv_rows + row;
                    float v = d_mask_conv[idx];
                    if (v > fin_max_val) {
                        fin_max_val = v;
                        fin_max_coo = idx;
                    }
                }
            }

            // Convert coordinate to offset
            int largest_row = (fin_max_coo + 1) % conv_rows - 1;
            int largest_col = (fin_max_coo + 1) / conv_rows;
            if ((fin_max_coo + 1) % conv_rows == 0) {
                largest_row = conv_rows - 1;
                largest_col = largest_col - 1;
            }
            largest_row++;
            largest_col++;

            int offset_row = largest_row - in_mod_rows - (sSize - tSize);
            int offset_col = largest_col - in_mod_cols - (sSize - tSize);

            int out_row = orig_row + offset_row;
            int out_col = orig_col + offset_col;

            tRowLoc[f + p * total_avi_frames] = out_row;
            tColLoc[f + p * total_avi_frames] = out_col;

            // 9. Coordinate & Template update every 10 frames
            if (f != 0 && (f % 10) == 0) {
                baseRow[p] = out_row;
                baseCol[p] = out_col;
                for (int col = 0; col < in_mod_cols; col++) {
                    for (int row = 0; row < in_mod_rows; row++) {
                        int ori_r = out_row - tSize + row - 1;
                        int ori_c = out_col - tSize + col - 1;
                        T_p[col * in_mod_rows + row] =
                            alpha * T_p[col * in_mod_rows + row] +
                            (1.0f - alpha) * cur_frame[ori_c * frame_rows + ori_r];
                    }
                }
            }
        }
    }
}

#ifdef __cplusplus
}
#endif
