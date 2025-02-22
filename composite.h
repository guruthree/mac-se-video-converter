/*
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2025 guruthree
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

#include <stdlib.h>
#include <math.h>

// code for displaying as composite video
// this will probably be a bit blurry
// based on https://github.com/guruthree/pico-composite-PAL-colour

// CHOOSE CONVERSION TYPE (uncomment to select)
//#define CONVERSION_MODE 0 // interlaced (causes flickering)
#define CONVERSION_MODE 1 // skip every 5th line

// this should give around 558 'pixels' across with a 188 MHz system clock, 542 visible
#define CV_CLOCK_DIV 9
#define DAC_FREQ (CLOCK_SPEED / (2*CV_CLOCK_DIV)) // 2* because dac PIO program is 2 cycles

// timings information http://www.batsocks.co.uk/readme/video_timing.htm
// field 1, even lines, field 2, odd lines
typedef struct {
    float scanline;
    float hsync;
    float backporch;
    float frontporch;
    float gap;
    float shortpulse;
    uint16_t numlines;
} TimingsStruct;

const TimingsStruct PalTimings = {
    .scanline = 64,
    .hsync = 4.7,
    .backporch = 5.7,
    .frontporch = 1.65,
    .gap = 4.7,
    .shortpulse = 2.35,
    .numlines = 625
};

const TimingsStruct NTSCTimings = {
    .scanline = 63.5,
    .hsync = 4.7,
    .backporch = 4.5,
    .frontporch = 1.5,
    .gap = 4.7,
    .shortpulse = 2.3, // changed
    .numlines = 525 // changed
};

// set to NTSCTimings for NTSC
// but line numbers are currently  hard coded for PAL...
// SO THIS WON'T WORK RIGHT NOW!!!
#define Timings PalTimings

// voltages
#define SYNC_VOLTS 0
#define BLANK_VOLTS 0.3
#define WHITE_VOLTS 0.7
// converted to DAC values
const uint8_t LEVEL_SYNC = SYNC_VOLTS * DIVISIONS_PER_VOLT + 0.5;
const uint8_t LEVEL_BLANK = BLANK_VOLTS * DIVISIONS_PER_VOLT + 0.5;
const uint8_t LEVEL_WHITE = WHITE_VOLTS * DIVISIONS_PER_VOLT + 0.5;

// start pin of each DAC on the VGA board
#define RED_CHANNEL 0
#define GREEN_CHANNEL 6
#define BLUE_CHANNEL  11
#define OUPUT_CHANNEL RED_CHANNEL
bi_decl(bi_1pin_with_name(OUPUT_CHANNEL, "Composite out"));

// timings in numbers of samples (bytes)
const uint16_t SAMPLES_PER_LINE = Timings.scanline * DAC_FREQ / 1e6; // 752 for PAL
const uint16_t SAMPLES_HSYNC = Timings.hsync * DAC_FREQ / 1e6; // 55
const uint16_t SAMPLES_BACK_PORCH = Timings.backporch * DAC_FREQ / 1e6; //  67
const uint16_t SAMPLES_FRONT_PORCH = Timings.frontporch * DAC_FREQ / 1e6; // 19
const uint16_t SAMPLES_GAP = Timings.gap * DAC_FREQ / 1e6; // 55
const uint16_t SAMPLES_SHORT_PULSE = Timings.shortpulse * DAC_FREQ / 1e6; // 28
const uint16_t SAMPLES_HALFLINE = SAMPLES_PER_LINE / 2; // 376

// buffers, we work on one while the dma writes the other to the DAC
uint8_t* linebuffer1;
uint8_t* linebuffer2;

uint16_t currentline;

uint8_t cv_dma_channel;
PIO cv_pio;
int cv_pio_sm;

void composite_init() {
    // setup and init the PIO
    cv_pio = pio0;
    cv_pio_sm = 0;
    int pio_offset = pio_add_program(cv_pio, &dac_program);
    dac_program_init(cv_pio, cv_pio_sm, pio_offset, OUPUT_CHANNEL, CV_CLOCK_DIV);

    // setup DMA
    cv_dma_channel = dma_claim_unused_channel(true);
    dma_channel_config channel_config = dma_channel_get_default_config(cv_dma_channel);

    channel_config_set_transfer_data_size(&channel_config, DMA_SIZE_32); // transfer 8 bits at a time
    channel_config_set_read_increment(&channel_config, true); // go down the buffer as it's read
    channel_config_set_write_increment(&channel_config, false); // always write the data to the same place
    channel_config_set_dreq(&channel_config, pio_get_dreq(cv_pio, cv_pio_sm, true));

    dma_channel_configure(cv_dma_channel,
                          &channel_config,
                          &cv_pio->txf[cv_pio_sm], // write address
                          NULL, // read address
                          SAMPLES_PER_LINE / 4, // number of data transfers to ( / 4 because 32-bit copies are faster)
                          false // start immediately
    );

    // initialise buffers (malloc because we can't construct an varname[varname] sized array)
    linebuffer1 = malloc(SAMPLES_PER_LINE);
    linebuffer2 = malloc(SAMPLES_PER_LINE);
    memset(linebuffer1, LEVEL_BLANK, SAMPLES_PER_LINE);
    memset(linebuffer2, LEVEL_BLANK, SAMPLES_PER_LINE);

    currentline = 1;
}

inline void hsync(uint8_t* currentbuffer) {
    memset(currentbuffer, LEVEL_SYNC, SAMPLES_HSYNC);
}

inline void gap(uint8_t* currentbuffer) {
    memset(currentbuffer, LEVEL_SYNC, SAMPLES_HALFLINE-SAMPLES_GAP);
}

inline void shortpulse(uint8_t* currentbuffer) {
    memset(currentbuffer, LEVEL_SYNC, SAMPLES_SHORT_PULSE);
}

inline void copyfromsebuffer(int16_t myline, uint8_t* currentbuffer) {
    uint8_t* p = currentbuffer + SAMPLES_HSYNC + SAMPLES_BACK_PORCH + (uint16_t)(1.8 * DAC_FREQ / 1e6);
    for (uint16_t c = 0; c < LINEBUFFER_LEN_8; c++) {
        for (uint8_t d = 0; d < 8; d++) {
#if INVERTED_SIGNAL == 0
            if (sebuffer[myline][c] & (1 << d))
#elif INVERTED_SIGNAL == 1
            if (!(sebuffer[myline][c] & (1 << d)))
#endif
                *p++ = LEVEL_BLANK; // black
            else
                *p++ = LEVEL_WHITE;
        }
    }
}

void composite_main() {
    uint8_t* currentbuffer;
    int16_t myline;

    while (true) {
        if (currentline & 1) {
            dma_channel_set_read_addr(cv_dma_channel, linebuffer1, true);
            currentbuffer = linebuffer2;
        }
        else {
            dma_channel_set_read_addr(cv_dma_channel, linebuffer2, true);
            currentbuffer = linebuffer1;
        }
        memset(currentbuffer, LEVEL_BLANK, SAMPLES_PER_LINE);

        // TODO: somehow make this work for NTSC too
        // video line specs:
        // http://www.batsocks.co.uk/readme/video_timing.htm
        // https://pe2bz.philpem.me.uk/Misc/-%20Video/NTSC-PAL-etc-Formats/pal_ntsc.html
        switch (currentline) {
            // double gap lines
            case 1 ... 2:
            case 314 ... 315:
                gap(currentbuffer);
                gap(currentbuffer+SAMPLES_HALFLINE);
                break;

            // half gap, half short pulse
            case 3:
                gap(currentbuffer);
                shortpulse(currentbuffer+SAMPLES_HALFLINE);
                break;

            // double short pulse
            case 4 ... 5:
            case 311 ... 312:
            case 316 ... 317:
            case 624 ... 625:
                shortpulse(currentbuffer);
                shortpulse(currentbuffer+SAMPLES_HALFLINE);
                break;

            // blank video lines
            case 6 ... 23:
            case 319 ... 335:
                hsync(currentbuffer);
                break;

            // first half of interlacing
            case 24 ... 310:
                hsync(currentbuffer);
                myline = currentline - 24;
#if CONVERSION_MODE == 0
                myline -= 50;
                if (myline >= 0 && myline < MAX_LINES/2) {
                    copyfromsebuffer(myline*2, currentbuffer);
                }
#elif CONVERSION_MODE == 1
                myline -= 8;
                if (myline >= 0) {
                    myline = myline + floor((4*myline-1)/16);
                    if (myline < MAX_LINES) {
                        copyfromsebuffer(myline, currentbuffer);
                    }
                }
#endif
                break;

            // half short pulse, half gap
            case 313:
                shortpulse(currentbuffer);
                gap(currentbuffer+SAMPLES_HALFLINE);
                break;

            // single short pulse
            case 318:
                shortpulse(currentbuffer);
                break;

            // second half of interlacing
            case 336 ... 622:
                hsync(currentbuffer);
                myline = currentline - 336;
#if CONVERSION_MODE == 0
                myline -= 51;
                if (myline >= 0 && myline < MAX_LINES/2) {
                    copyfromsebuffer(myline*2+1, currentbuffer);
                }
#elif CONVERSION_MODE == 1 // shouldn't get here?
                myline -= 9;
                if (myline >= 0) {
                    myline = myline + floor((4*myline-1)/16);
                    if (myline < MAX_LINES) {
                        copyfromsebuffer(myline, currentbuffer);
                    }
                }
#endif
                break;

            // hsync and half short pulse
            case 623:
                hsync(currentbuffer);
                shortpulse(currentbuffer+SAMPLES_HALFLINE);
                break;

            default:
                // should never get here
                break;
        }

        currentline++;
#if CONVERSION_MODE == 0
        if (currentline > Timings.numlines) {
#elif CONVERSION_MODE == 1
        if (currentline > Timings.numlines/2) {
#endif
            currentline = 1;
            gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);
        }
        dma_channel_wait_for_finish_blocking(cv_dma_channel);
    }
}
