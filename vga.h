/*
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 guruthree
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

// code for displaying on VGA, loosely based on
// https://github.com/raspberrypi/pico-playground/blob/master/scanvideo/test_pattern/test_pattern.c

// VGA output using VGA demo board
// https://shop.pimoroni.com/products/pimoroni-pico-vga-demo-base?variant=32369520672851
// with scanvideo_dpi library
// https://github.com/raspberrypi/pico-extras/tree/master/src/common/pico_scanvideo
// useful forum thread about using said library
// https://forums.raspberrypi.com/viewtopic.php?t=305712

// VGA mode 1024x768@70 has a nominal pixel clock of 75 MHz, 188/75 = 2.50666 clock divs
// VGA 1024x768 range for (most?) monitors is 60-75 Hz
// so if we adjust the pixel clock to an even divisor of 188, we get 75.2 MHz
// this works out to 1024x768@70.2
// this falls into the range that at least my NEC LCD1450NX can sync to
// BUT, scanvideo can't do a half divsor, so halve the pixel clock
// gives a resolution of 512x768, which we can make 512x384 - PERFECT

#define WHITE 0x7FFF // warm white
// #define WHITE 0xFFDF // cold white
// #define WHITE 0x05DF // amber
// #define WHITE 0x07C0 // green
#define BLACK 0x0000
#include "lookuptable.h"

// number of VGA lines to skip to adjust vertical position of mac video picture
#define VERTICAL_OFFSET 21

// VGA timings
// http://tinyvga.com/vga-timing/1024x768@70Hz
// based on pico_scanvideo/vga_modes.c
const scanvideo_timing_t vga_timing_1024x768_70_default = {
//    .clock_freq = 75000000, // 150 MHz Sysclock
    .clock_freq = 37600000, // 188 MHz with pixel doubling

    .h_active = 1024/2, // the divisions by two are for the pixel doubling
    .v_active = 768,

    .h_front_porch = 24/2,
    .h_pulse = 136/2,
    .h_total = 1328/2,
    .h_sync_polarity = 0,

    .v_front_porch = 3,
    .v_pulse = 6,
    .v_total = 806,
    .v_sync_polarity = 1,

    .enable_clock = 0,
    .clock_polarity = 0,

    .enable_den = 0
};

const scanvideo_mode_t vga_mode_1024x768_70 = {
    .default_timing = &vga_timing_1024x768_70_default,
    .pio_program = &video_24mhz_composable,
    .width = 1024/2,
    .height = 768/2,
    .xscale = 1,
    .yscale = 2, // keeps in scale for the doubling
};

#define vga_mode vga_mode_1024x768_70

void draw_from_sebuffer(scanvideo_scanline_buffer_t *buffer) {
    int16_t line_num = scanvideo_scanline_number(buffer->scanline_id);
    line_num -= VERTICAL_OFFSET;

    uint16_t *p = (uint16_t*)buffer->data;
    if (line_num < 0 || line_num >= MAX_LINES) {
        // outside where the mac has data, just draw a black line
        *p++ = COMPOSABLE_COLOR_RUN;
        *p++ = BLACK;
        *p++ = vga_mode.width - 2;
        *p++ = COMPOSABLE_RAW_1P;
        *p++ = BLACK; // the required last black pixel
        // composable + black + num + 1p + black = 5
        // end of line with alignment padding
        *p++ = COMPOSABLE_EOL_ALIGN; // after odd # of tokens
//        *p++ = COMPOSABLE_EOL_SKIP_ALIGN; // after even # of tokens
    }
    else {
        // copy the data read from the mac to the out buffer for vga
        *p++ = COMPOSABLE_RAW_RUN;

        // first bit is special
#if INVERTED_SIGNAL == 0
        if (sebuffer[line_num][0] & 1)
#elif INVERTED_SIGNAL == 1
        if (!(sebuffer[line_num][0] & 1))
#endif
            *p++ = BLACK;
        else
            *p++ = WHITE;



        *p++ = vga_mode.width - 2;

        // the next 7 bits
        for (uint16_t d = 1; d < 8; d++) {
#if INVERTED_SIGNAL == 0
            *p++ = lookuptable[sebuffer[line_num][0]][d];
#elif INVERTED_SIGNAL == 1
            *p++ = lookuptable[(uint8_t)(~sebuffer[line_num][0])][d];
#endif
        }

        // the last 504 bits are all done the same
        for (uint16_t c = 1; c < MIN(vga_mode.width/8, LINEBUFFER_LEN_8); c++) {
            uint8_t idx = sebuffer[line_num][c];
#if INVERTED_SIGNAL == 1
            idx = ~idx;
#endif
            for (uint16_t d = 0; d < 8; d++) {
                *p++ = lookuptable[idx][d];
            }
        }

        *p++ = BLACK; // off the screen? the required last black pixel
        // composible + 1 pixel + num across + 7 pixels + 63*8 pixels + 1 black = 515
        // end of line with alignment padding
        *p++ = COMPOSABLE_EOL_ALIGN; // after odd # of tokens
//        *p++ = COMPOSABLE_EOL_SKIP_ALIGN; // after even # of tokens
    }

    buffer->data_used = ((uint32_t *) p) - buffer->data;
    assert(buffer->data_used < buffer->data_max);
    buffer->status = SCANLINE_OK;
}

void vga_init() {
    // initialize video and interrupts on which ever core launches this
    // this also uses pio0!!!
    // this also uses DMA channels 0, 1, and 2!!!
    scanvideo_setup(&vga_mode);
    scanvideo_timing_enable(true);
}

float vgalastprint = 0;
void vga_main() {
    while (true) {
        scanvideo_scanline_buffer_t *scanline_buffer = scanvideo_begin_scanline_generation(true);
        draw_from_sebuffer(scanline_buffer);
        scanvideo_end_scanline_generation(scanline_buffer);
    }
}
