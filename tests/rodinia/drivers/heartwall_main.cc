#define DHIR_BENCH_NAME "heartwall"
#include "dhir_bench.h"

extern "C" {
void heartwall(
    int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t,
    int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t,
    int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t,
    int32_t, int32_t, int32_t, float,
    // frames (3D): 9 params
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t,
    // baseRow (1D): 5 params
    int32_t *, int32_t *, int64_t, int64_t, int64_t,
    // baseCol (1D): 5 params
    int32_t *, int32_t *, int64_t, int64_t, int64_t,
    // d_T (3D): 9 params
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t,
    // tRowLoc (1D): 5 params
    int32_t *, int32_t *, int64_t, int64_t, int64_t,
    // tColLoc (1D): 5 params
    int32_t *, int32_t *, int64_t, int64_t, int64_t,
    // 10 2D memrefs (ws_in2 ... ws_mask_conv): 7 params each
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t,
    float *, float *, int64_t, int64_t, int64_t, int64_t, int64_t
);

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
);

int hw_load_avi_frame(const char *avi_path, int frame_no, int *out_rows, int *out_cols, float **out_frame);
int hw_get_total_frames(const char *avi_path);
void hw_free_avi_frame(float *frame);
void hw_close_avi(void);
}

// Parameters and dimensions dynamically loaded from input.txt and test.avi
static int TSIZE = 0;
static int SSIZE = 0;
static int MAXMOVE = 0;
static float ALPHA = 0.0f;

static int in_mod_rows = 0;
static int in_mod_cols = 0;
static int in_mod_elem = 0;

static int in2_rows = 0;
static int in2_cols = 0;
static int in2_elem = 0;

static int conv_rows = 0;
static int conv_cols = 0;
static int conv_elem = 0;

static int in2_pad_add_rows = 0;
static int in2_pad_add_cols = 0;
static int in2_pad_rows = 0;
static int in2_pad_cols = 0;
static int in2_pad_elem = 0;

static int in2_pad_cumv_sel_rowlow = 0;
static int in2_pad_cumv_sel2_rowlow = 0;
static int in2_sub_rows = 0;
static int in2_sub_cols = 0;
static int in2_sub_elem = 0;

static int in2_sub_cumh_sel_collow = 0;
static int in2_sub_cumh_sel2_collow = 0;
static int in2_sub2_sqr_rows = 0;
static int in2_sub2_sqr_cols = 0;
static int in2_sub2_sqr_elem = 0;

static int mask_rows = 0;
static int mask_cols = 0;
static int mask_elem = 0;

static int mask_conv_ioffset = 0;
static int mask_conv_joffset = 0;

// Authentic Points from input.txt
static int endoPoints = 0;
static int *endoRow = nullptr;
static int *endoCol = nullptr;
static int epiPoints = 0;
static int *epiRow = nullptr;
static int *epiCol = nullptr;
static int allPoints = 0;

// Trajectory Matrices & Templates
static int *baseRow = nullptr;
static int *baseCol = nullptr;
static int *ref_baseRow = nullptr;
static int *ref_baseCol = nullptr;

static float *d_T = nullptr;
static float *ref_d_T = nullptr;

static int *tRowLoc = nullptr;
static int *tColLoc = nullptr;
static int *ref_tRowLoc = nullptr;
static int *ref_tColLoc = nullptr;

// Pre-loaded video frames
static float *dhir_frames = nullptr;
static int frame_rows = 0;
static int frame_cols = 0;
static int total_avi_frames = 0;
static int frames_processed = 20;

static const char *g_avi_path = nullptr;

// Workspaces
static float *ws_in2 = nullptr;
static float *ws_in2_sqr = nullptr;
static float *ws_in_mod = nullptr;
static float *ws_in_sqr = nullptr;
static float *ws_conv = nullptr;
static float *ws_in2_pad = nullptr;
static float *ws_in2_sub = nullptr;
static float *ws_in2_sub2_sqr = nullptr;
static float *ws_tMask = nullptr;
static float *ws_mask_conv = nullptr;

static void bench_alloc(void) {
    // 1. Locate and parse input.txt
    const char *candidates_txt[] = {
        "vendor/rodinia/rodinia_3.1/data/heartwall/input.txt",
        nullptr
    };

    const char *txt_path = nullptr;
    for (int i = 0; candidates_txt[i]; i++) {
        FILE *f = fopen(candidates_txt[i], "r");
        if (f) {
            fclose(f);
            txt_path = candidates_txt[i];
            break;
        }
    }
    if (!txt_path) {
        fprintf(stderr, "heartwall: failed to locate input.txt\n");
        exit(1);
    }

    FILE *fp = fopen(txt_path, "r");
    if (!fp) {
        fprintf(stderr, "heartwall: failed to open %s\n", txt_path);
        exit(1);
    }

    if (fscanf(fp, "%d %d %d %f\n", &TSIZE, &SSIZE, &MAXMOVE, &ALPHA) != 4) {
        fprintf(stderr, "heartwall: failed to read parameters from %s\n", txt_path);
        exit(1);
    }

    if (fscanf(fp, "%d %d\n", &endoPoints, &epiPoints) != 2) {
        fprintf(stderr, "heartwall: failed to read point counts from %s\n", txt_path);
        exit(1);
    }

    endoRow = (int *)malloc(sizeof(int) * endoPoints);
    endoCol = (int *)malloc(sizeof(int) * endoPoints);
    for (int i = 0; i < endoPoints; i++) fscanf(fp, "%d", &endoRow[i]);
    for (int i = 0; i < endoPoints; i++) fscanf(fp, "%d", &endoCol[i]);

    epiRow = (int *)malloc(sizeof(int) * epiPoints);
    epiCol = (int *)malloc(sizeof(int) * epiPoints);
    for (int i = 0; i < epiPoints; i++) fscanf(fp, "%d", &epiRow[i]);
    for (int i = 0; i < epiPoints; i++) fscanf(fp, "%d", &epiCol[i]);
    fclose(fp);

    allPoints = endoPoints + epiPoints;

    // 2. Compute authentic dimensions
    in_mod_rows = 2 * TSIZE + 1;
    in_mod_cols = 2 * TSIZE + 1;
    in_mod_elem = in_mod_rows * in_mod_cols;

    in2_rows = 2 * SSIZE + 1;
    in2_cols = 2 * SSIZE + 1;
    in2_elem = in2_rows * in2_cols;

    conv_rows = in_mod_rows + in2_rows - 1;
    conv_cols = in_mod_cols + in2_cols - 1;
    conv_elem = conv_rows * conv_cols;

    in2_pad_add_rows = in_mod_rows;
    in2_pad_add_cols = in_mod_cols;
    in2_pad_rows = in2_rows + 2 * in2_pad_add_rows;
    in2_pad_cols = in2_cols + 2 * in2_pad_add_cols;
    in2_pad_elem = in2_pad_rows * in2_pad_cols;

    in2_pad_cumv_sel_rowlow = 1 + in_mod_rows;
    in2_pad_cumv_sel2_rowlow = 1;
    in2_sub_rows = in2_pad_rows - in_mod_rows - 1;
    in2_sub_cols = in2_pad_cols;
    in2_sub_elem = in2_sub_rows * in2_sub_cols;

    in2_sub_cumh_sel_collow = 1 + in_mod_cols;
    in2_sub_cumh_sel2_collow = 1;
    in2_sub2_sqr_rows = in2_sub_rows;
    in2_sub2_sqr_cols = in2_sub_cols - in_mod_cols - 1;
    in2_sub2_sqr_elem = in2_sub2_sqr_rows * in2_sub2_sqr_cols;

    mask_rows = MAXMOVE;
    mask_cols = MAXMOVE;
    mask_elem = mask_rows * mask_cols;

    mask_conv_ioffset = (mask_rows - 1) / 2;
    if ((mask_rows - 1) % 2 != 0) mask_conv_ioffset++;
    mask_conv_joffset = (mask_cols - 1) / 2;
    if ((mask_cols - 1) % 2 != 0) mask_conv_joffset++;

    // 3. Locate test.avi
    const char *candidates_avi[] = {
        "vendor/rodinia/rodinia_3.1/data/heartwall/test.avi",
        nullptr
    };

    for (int i = 0; candidates_avi[i]; i++) {
        const char *path = candidates_avi[i];
        if (FILE *f = fopen(path, "r")) {
            fclose(f);
            g_avi_path = path;
            break;
        }
    }
    if (!g_avi_path) {
        fprintf(stderr, "heartwall: failed to locate test.avi\n");
        exit(1);
    }

    float *test_f0 = nullptr;
    if (hw_load_avi_frame(g_avi_path, 0, &frame_rows, &frame_cols, &test_f0) != 0) {
        fprintf(stderr, "heartwall: failed to load frame 0 from %s\n", g_avi_path);
        exit(1);
    }
    hw_free_avi_frame(test_f0);

    total_avi_frames = hw_get_total_frames(g_avi_path);
    if (total_avi_frames <= 0) total_avi_frames = 104;

    const char *frames_env = getenv("HEARTWALL_FRAMES");
    if (frames_env) {
        frames_processed = atoi(frames_env);
        if (frames_processed <= 0) frames_processed = 20;
        if (frames_processed > total_avi_frames) frames_processed = total_avi_frames;
    }

    // 4. Preload all frames_processed video frames into contiguous memory
    long frame_pixels = (long)frame_cols * frame_rows;
    long all_frames_size = (long)frames_processed * frame_pixels;
    dhir_frames = dhir_alloc(all_frames_size);

    for (int f = 0; f < frames_processed; f++) {
        float *cur_frame = nullptr;
        int fr = 0, fc = 0;
        hw_load_avi_frame(g_avi_path, f, &fr, &fc, &cur_frame);
        memcpy(dhir_frames + (long)f * frame_pixels, cur_frame, sizeof(float) * frame_pixels);
        hw_free_avi_frame(cur_frame);
    }

    // 5. Allocate trajectory matrices & templates
    long traj_size = (long)allPoints * total_avi_frames;
    tRowLoc = (int *)malloc(sizeof(int) * traj_size);
    tColLoc = (int *)malloc(sizeof(int) * traj_size);
    ref_tRowLoc = (int *)malloc(sizeof(int) * traj_size);
    ref_tColLoc = (int *)malloc(sizeof(int) * traj_size);

    memset(tRowLoc, 0, sizeof(int) * traj_size);
    memset(tColLoc, 0, sizeof(int) * traj_size);
    memset(ref_tRowLoc, 0, sizeof(int) * traj_size);
    memset(ref_tColLoc, 0, sizeof(int) * traj_size);

    baseRow = (int *)malloc(sizeof(int) * allPoints);
    baseCol = (int *)malloc(sizeof(int) * allPoints);
    ref_baseRow = (int *)malloc(sizeof(int) * allPoints);
    ref_baseCol = (int *)malloc(sizeof(int) * allPoints);

    long t_total_size = (long)allPoints * in_mod_elem;
    d_T = dhir_alloc(t_total_size);
    ref_d_T = dhir_alloc(t_total_size);

    const float *frame0 = dhir_frames; // frame 0 is at offset 0

    // Initialize Frame 0 positions and crop initial templates
    for (int p = 0; p < allPoints; p++) {
        int r = (p < endoPoints) ? endoRow[p] : epiRow[p - endoPoints];
        int c = (p < endoPoints) ? endoCol[p] : epiCol[p - endoPoints];

        baseRow[p] = ref_baseRow[p] = r;
        baseCol[p] = ref_baseCol[p] = c;

        tRowLoc[0 + p * total_avi_frames] = ref_tRowLoc[0 + p * total_avi_frames] = r;
        tColLoc[0 + p * total_avi_frames] = ref_tColLoc[0 + p * total_avi_frames] = c;

        float *T_p = d_T + (long)p * in_mod_elem;
        float *ref_T_p = ref_d_T + (long)p * in_mod_elem;
        for (int col = 0; col < in_mod_cols; col++) {
            for (int row = 0; row < in_mod_rows; row++) {
                int ori_r = r - TSIZE + row - 1;
                int ori_c = c - TSIZE + col - 1;
                float val = frame0[ori_c * frame_rows + ori_r];
                T_p[col * in_mod_rows + row] = val;
                ref_T_p[col * in_mod_rows + row] = val;
            }
        }
    }

    // 6. Pre-allocate single-point workspace buffers
    ws_in2 = dhir_alloc(in2_elem);
    ws_in2_sqr = dhir_alloc(in2_elem);
    ws_in_mod = dhir_alloc(in_mod_elem);
    ws_in_sqr = dhir_alloc(in_mod_elem);
    ws_conv = dhir_alloc(conv_elem);
    ws_in2_pad = dhir_alloc(in2_pad_elem);
    ws_in2_sub = dhir_alloc(in2_sub_elem);
    ws_in2_sub2_sqr = dhir_alloc(in2_sub2_sqr_elem);
    ws_tMask = dhir_alloc(conv_elem);
    ws_mask_conv = dhir_alloc(conv_elem);
}

static void bench_call(void) {
    long traj_size = (long)allPoints * total_avi_frames;
    heartwall(
        frames_processed, allPoints, total_avi_frames,
        frame_rows, frame_cols,
        in_mod_rows, in_mod_cols,
        in2_rows, in2_cols,
        conv_rows, conv_cols,
        in2_pad_rows, in2_pad_cols,
        in2_sub_rows, in2_sub_cols,
        in2_sub2_sqr_rows, in2_sub2_sqr_cols,
        mask_rows, mask_cols,
        mask_conv_ioffset, mask_conv_joffset,
        in2_pad_cumv_sel_rowlow, in2_pad_cumv_sel2_rowlow,
        in2_sub_cumh_sel_collow, in2_sub_cumh_sel2_collow,
        SSIZE, TSIZE, ALPHA,
        MR3(dhir_frames, frames_processed, frame_cols, frame_rows),
        MR1(baseRow, allPoints),
        MR1(baseCol, allPoints),
        MR3(d_T, allPoints, in_mod_cols, in_mod_rows),
        MR1(tRowLoc, traj_size),
        MR1(tColLoc, traj_size),
        MR2(ws_in2, in2_cols, in2_rows),
        MR2(ws_in2_sqr, in2_cols, in2_rows),
        MR2(ws_in_mod, in_mod_cols, in_mod_rows),
        MR2(ws_in_sqr, in_mod_cols, in_mod_rows),
        MR2(ws_conv, conv_cols, conv_rows),
        MR2(ws_in2_pad, in2_pad_cols, in2_pad_rows),
        MR2(ws_in2_sub, in2_sub_cols, in2_sub_rows),
        MR2(ws_in2_sub2_sqr, in2_sub2_sqr_cols, in2_sub2_sqr_rows),
        MR2(ws_tMask, conv_cols, conv_rows),
        MR2(ws_mask_conv, conv_cols, conv_rows)
    );
}

static void bench_reference(void) {
    ref_heartwall(
        frames_processed, allPoints, total_avi_frames,
        frame_rows, frame_cols,
        in_mod_rows, in_mod_cols,
        in2_rows, in2_cols,
        conv_rows, conv_cols,
        in2_pad_rows, in2_pad_cols,
        in2_sub_rows, in2_sub_cols,
        in2_sub2_sqr_rows, in2_sub2_sqr_cols,
        mask_rows, mask_cols,
        mask_conv_ioffset, mask_conv_joffset,
        in2_pad_cumv_sel_rowlow, in2_pad_cumv_sel2_rowlow,
        in2_sub_cumh_sel_collow, in2_sub_cumh_sel2_collow,
        SSIZE, TSIZE, ALPHA,
        dhir_frames,
        ref_baseRow,
        ref_baseCol,
        ref_d_T,
        ref_tRowLoc,
        ref_tColLoc,
        ws_in2,
        ws_in2_sqr,
        ws_in_mod,
        ws_in_sqr,
        ws_conv,
        ws_in2_pad,
        ws_in2_sub,
        ws_in2_sub2_sqr,
        ws_tMask,
        ws_mask_conv
    );
}

static int bench_check(void) {
    int errors = 0;
    for (int p = 0; p < allPoints; p++) {
        for (int f = 0; f < frames_processed; f++) {
            int got_r = tRowLoc[f + p * total_avi_frames];
            int want_r = ref_tRowLoc[f + p * total_avi_frames];
            if (got_r != want_r) {
                if (errors < 5)
                    printf("  mismatch tRowLoc[p=%d, f=%d]: got %d want %d\n", p, f, got_r, want_r);
                errors++;
            }
            int got_c = tColLoc[f + p * total_avi_frames];
            int want_c = ref_tColLoc[f + p * total_avi_frames];
            if (got_c != want_c) {
                if (errors < 5)
                    printf("  mismatch tColLoc[p=%d, f=%d]: got %d want %d\n", p, f, got_c, want_c);
                errors++;
            }
        }
    }
    return errors;
}

static void bench_free(void) {
    if (tRowLoc) free(tRowLoc);
    if (tColLoc) free(tColLoc);
    if (ref_tRowLoc) free(ref_tRowLoc);
    if (ref_tColLoc) free(ref_tColLoc);

    if (baseRow) free(baseRow);
    if (baseCol) free(baseCol);
    if (ref_baseRow) free(ref_baseRow);
    if (ref_baseCol) free(ref_baseCol);

    if (d_T) free(d_T);
    if (ref_d_T) free(ref_d_T);

    if (dhir_frames) free(dhir_frames);

    if (endoRow) free(endoRow);
    if (endoCol) free(endoCol);
    if (epiRow) free(epiRow);
    if (epiCol) free(epiCol);

    if (ws_in2) free(ws_in2);
    if (ws_in2_sqr) free(ws_in2_sqr);
    if (ws_in_mod) free(ws_in_mod);
    if (ws_in_sqr) free(ws_in_sqr);
    if (ws_conv) free(ws_conv);
    if (ws_in2_pad) free(ws_in2_pad);
    if (ws_in2_sub) free(ws_in2_sub);
    if (ws_in2_sub2_sqr) free(ws_in2_sub2_sqr);
    if (ws_tMask) free(ws_tMask);
    if (ws_mask_conv) free(ws_mask_conv);

    hw_close_avi();
}
