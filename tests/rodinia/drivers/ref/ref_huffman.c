// Reference implementation for Huffman VLC encoding matching Rodinia 3.1
#include <stdint.h>

static inline int min_val(int a, int b) {
    return (a < b) ? a : b;
}

void ref_huffman(const uint32_t indata[], uint32_t num_elements,
                 uint32_t outdata[], uint32_t outsize[],
                 const uint32_t codebook[][2]) {
    outsize[0] = 0;
    outsize[1] = 0;
    outsize[2] = 0;
    outdata[0] = 0x00000000U;

    for (uint32_t k = 0; k < num_elements; k++) {
        uint32_t val32 = indata[k];

        for (uint32_t i = 0; i < 4; i++) {
            uint32_t symbol = (val32 >> (8 * (3 - i))) & 0xFFU;
            uint32_t cw32 = codebook[symbol][0];
            uint32_t numbits = codebook[symbol][1];

            while (numbits > 0) {
                uint32_t sb = outsize[1];
                int writebits = min_val(32 - sb, numbits);
                uint32_t mask32;
                if (numbits == (uint32_t)writebits)
                    mask32 = (cw32 & ((1U << numbits) - 1)) << (32 - sb - numbits);
                else
                    mask32 = cw32 >> (numbits - writebits);

                uint32_t widx = outsize[2];
                outdata[widx] = outdata[widx] | mask32;
                numbits = numbits - writebits;
                sb = (sb + writebits) % 32;
                outsize[1] = sb;
                if (sb == 0) {
                    widx++;
                    outsize[2] = widx;
                    outdata[widx] = 0x00000000U;
                    outsize[0] += 4;
                }
            }
        }
    }
    outsize[0] += (outsize[1] / 8) + ((outsize[1] % 8 == 0) ? 0 : 1);
}
