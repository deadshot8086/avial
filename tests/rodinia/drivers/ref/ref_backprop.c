#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

void ref_backprop(int in, int hid, int out,
                  float l1[in + 1],
                  float l2[hid + 1],
                  float l3[out + 1],
                  const float target[out + 1],
                  float output_delta[out + 1],
                  float hidden_delta[hid + 1],
                  float w_in[in + 1][hid + 1],
                  float oldw_in[in + 1][hid + 1],
                  float w_hid[hid + 1][out + 1],
                  float oldw_hid[hid + 1][out + 1]) {

    const float ETA = 0.3f;
    const float MOMENTUM = 0.3f;

    // Step 1: Forward layer input -> hidden
    l1[0] = 1.0f;
    for (int j = 1; j <= hid; j++) {
        float sum = 0.0f;
        for (int k = 0; k <= in; k++) {
            sum += w_in[k][j] * l1[k];
        }
        l2[j] = 1.0f / (1.0f + expf(-sum));
    }

    // Step 2: Forward layer hidden -> output
    l2[0] = 1.0f;
    for (int j = 1; j <= out; j++) {
        float sum = 0.0f;
        for (int k = 0; k <= hid; k++) {
            sum += w_hid[k][j] * l2[k];
        }
        l3[j] = 1.0f / (1.0f + expf(-sum));
    }

    // Step 3: Output error
    for (int i = 1; i <= out; i++) {
        float o = l3[i];
        float t = target[i];
        output_delta[i] = o * (1.0f - o) * (t - o);
    }

    // Step 4: Hidden error
    for (int i = 1; i <= hid; i++) {
        float h = l2[i];
        float sum = 0.0f;
        for (int j = 1; j <= out; j++) {
            sum += output_delta[j] * w_hid[i][j];
        }
        hidden_delta[i] = h * (1.0f - h) * sum;
    }

    // Step 5: Adjust hidden weights
    l2[0] = 1.0f;
    for (int k = 0; k <= hid; k++) {
        for (int j = 1; j <= out; j++) {
            float new_dw = (ETA * output_delta[j] * l2[k]) + (MOMENTUM * oldw_hid[k][j]);
            w_hid[k][j] += new_dw;
            oldw_hid[k][j] = new_dw;
        }
    }

    // Step 6: Adjust input weights
    l1[0] = 1.0f;
    for (int k = 0; k <= in; k++) {
        for (int j = 1; j <= hid; j++) {
            float new_dw = (ETA * hidden_delta[j] * l1[k]) + (MOMENTUM * oldw_in[k][j]);
            w_in[k][j] += new_dw;
            oldw_in[k][j] = new_dw;
        }
    }
}

#ifdef __cplusplus
}
#endif
