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

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#include "videoinput.pio.h"

// fast UNSAFE copies of uint8_t arrays, array length needs to be multiple of 4
int dma_chan32;
void dmacpy(uint8_t *dst, uint8_t *src, uint16_t size) { // inline?
    dma_channel_abort(dma_chan32); // WARNING THIS IS AN UNSAFE COPY!!!
    dma_channel_set_trans_count(dma_chan32, size / 4, false); // size in 8-bit!!!
    dma_channel_set_read_addr(dma_chan32, src, false);
    dma_channel_set_write_addr(dma_chan32, dst, true);
}

// trying to capture video output from a Macintosh SE Logic Board and convert to VGA
// http://www.waveguide.se/?article=compact-mac-video-adapter
// https://tinkerdifferent.com/threads/warpse-25-mhz-68hc000-based-accelerator-for-mac-se.253/page-11#post-10080

// the SE's video output:
// 15.6672 MHz pixel clock
// 0.045 ms (vs 0.0449438202 ms) per horizontal line? (22.22 KHz vs 22.25 KHz?)
// 22250/15.6672 = 1420 samples across?
// need to translate 512x342 resolution

// VGA output using
// https://shop.pimoroni.com/products/pimoroni-pico-vga-demo-base?variant=32369520672851
// with scanvideo_dpi library
// https://github.com/raspberrypi/pico-extras/tree/master/src/common/pico_scanvideo
// useful forum thread about using said library
// https://forums.raspberrypi.com/viewtopic.php?t=305712
// VGA timings
// http://tinyvga.com/vga-timing

// VGA mode 1024x768@70 has a nominal pixel clock of 75 MHz, 188/75 = 2.50666 clock divs
// VGA 1024x768 range for (most?) monitors is 60-75 Hz
// so if we adjust the pixel clock to an even divisor of 188, we get 75.2 MHz
// this works out to 1024x768@70.2
// this falls into the range that at least my NEC LCD1450NX can sync to
// BUT, scanvideo can't do a half divsor, so halve the pixel clock
// gives a resolution of 512x768, which we can make 512x384 - PERFECT


// need an even divisor of the pixel clock for the system clock
// for both the SE and VGA
// 188/15.6672 = 12 for the SE
// 188/37.6 = 5 for VGA



#define CLOCK_SPEED 188e6
#define CLOCK_DIV 12 //(188.0/15.6672)

#define HSYNC_PIN 20 // pin 26
#define VSYNC_PIN 19 // pin 25
#define VIDEO_PIN 18 // pin 24

// about 340 lines? 345 covers some extra to see edges
#define MAX_LINES 345
#define LINE_OFFSET 26

// buffers need to be multiple of 4 for 32 bit copies
#define LINEBUFFER_LEN_32 22  // X*4*8 = # of bits
#define LINEBUFFER_LEN_8 LINEBUFFER_LEN_32*4
#define BUFFER_LEN_32 MAX_LINES*LINEBUFFER_LEN_32
#define BUFFER_LEN_8 BUFFER_LEN_32*4
uint8_t buffer[MAX_LINES][LINEBUFFER_LEN_8];
uint8_t linebuffer0[LINEBUFFER_LEN_8];
uint8_t linebuffer1[LINEBUFFER_LEN_8];
bool dataready = false, datasent = true;
// datasent true so we enter at the start of a new frame?
int16_t currentline = -LINE_OFFSET;

uint8_t dma_channel;
PIO pio;
int pio_sm;

// on vsync check if the data has been shown, if so, collect new data
// if not, setup to collect new data again
void __isr __time_critical_func(gpio_callback)(uint gpio, uint32_t events) {
    if (gpio == HSYNC_PIN) {
        pio_sm_clkdiv_restart(pio, pio_sm);

        dma_channel_abort(dma_channel);
        pio_sm_clear_fifos(pio, pio_sm);

        if (currentline & 1) { // odd
            dma_channel_set_write_addr(dma_channel, linebuffer1, true);
        }
        else { // even
            dma_channel_set_write_addr(dma_channel, linebuffer0, true);
        }

        if (currentline > 0) { // copy the previous line into the buffer
            if (currentline & 1) { // odd
                dmacpy(buffer[currentline], linebuffer0, LINEBUFFER_LEN_8);
            }
            else { // even
                dmacpy(buffer[currentline], linebuffer1, LINEBUFFER_LEN_8);
            }
        }

        currentline++;
    }
    else if (gpio == VSYNC_PIN) {
        if (datasent == true) {
            currentline = -LINE_OFFSET;
            dataready = false;
            datasent = false;
            gpio_set_irq_enabled(HSYNC_PIN, GPIO_IRQ_EDGE_FALL, true);
        }
        else {
            gpio_put(PICO_DEFAULT_LED_PIN, true);
            dataready = true;
            datasent = false;
            gpio_set_irq_enabled(HSYNC_PIN, GPIO_IRQ_EDGE_FALL, false);
//            gpio_set_irq_enabled(VSYNC_PIN, GPIO_IRQ_EDGE_FALL, false);
        }
    }
}

void core1_entry();

int main() {
    set_sys_clock_khz(CLOCK_SPEED/1e3, true);
    stdio_init_all(); // need to get VGA output working so that there's some isolation from the PC?

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, true);

    sleep_ms(5000);
    printf("hello world\n");

    // setup and init the PIO
    pio = pio0;
    pio_sm = 0;
    int pio_offset = pio_add_program(pio, &videoinput_program);
    videoinput_program_init(pio, pio_sm, pio_offset, VIDEO_PIN, CLOCK_DIV);

    // setup DMA
    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config channel_config = dma_channel_get_default_config(dma_channel);

    channel_config_set_transfer_data_size(&channel_config, DMA_SIZE_32); // read 32 bits at a time
    channel_config_set_read_increment(&channel_config, false); // always read from the same place
    channel_config_set_write_increment(&channel_config, true); // go down the buffer writing
    channel_config_set_dreq(&channel_config, pio_get_dreq(pio, pio_sm, false));
    channel_config_set_ring(&channel_config, true, 0); // disable wrapping

    dma_channel_configure(dma_channel,
                          &channel_config,
                          &buffer, // write address
                          &pio->rxf[pio_sm], // read address
                          LINEBUFFER_LEN_32, // number of data transfers to do
                          false // start immediately
    );

    // setup dma for dmacopy (faster than memcpy)
    dma_chan32 = dma_claim_unused_channel(true);
    dma_channel_config channel_config32 = dma_channel_get_default_config(dma_chan32);

    channel_config_set_transfer_data_size(&channel_config32, DMA_SIZE_32); // transfer 32 bits at a time
    channel_config_set_read_increment(&channel_config32, true); 
    channel_config_set_write_increment(&channel_config32, true);

    dma_channel_configure(dma_chan32,
                          &channel_config32,
                          NULL, // write address
                          NULL, // read address
                          0, // number of data transfers to 
                          false // start immediately
    );

    // init buffers
    memset(buffer, 0, BUFFER_LEN_8);
    memset(linebuffer0, 0, LINEBUFFER_LEN_8);
    memset(linebuffer1, 0, LINEBUFFER_LEN_8);

    // setup interrupt on hsync
    gpio_init(HSYNC_PIN);
    gpio_set_dir(HSYNC_PIN, GPIO_IN);
    gpio_pull_down(HSYNC_PIN); // mac defaults to pulling up
//    gpio_set_irq_enabled_with_callback(HSYNC_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // setup interrupt on vsync
    gpio_init(VSYNC_PIN);
    gpio_set_dir(VSYNC_PIN, GPIO_IN);
    gpio_pull_down(VSYNC_PIN); // mac defaults to pulling up

    // launch VGA
    multicore_launch_core1(core1_entry);
    sleep_ms(5000);

    // VGA is up, go go go!
//    gpio_set_irq_enabled(VSYNC_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled_with_callback(VSYNC_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_put(PICO_DEFAULT_LED_PIN, false); // LED off we're in normal operation
    printf("normal operation\n");

    while (1) {
        sleep_ms(1); // need some sleep or lines turn out crooked...

        if (dataready && !datasent) {
            printf("data:\n");
            // print out - there may be a problem trying to print lines longer than 4096 characters
            for (uint16_t k = 0; k < MAX_LINES; k++) { // line
                for (uint16_t i = 0; i < LINEBUFFER_LEN_8; i++) { // byte
                    for (uint8_t j = 0; j < 8; j ++) { // bit

                        if (((buffer[k][i] & (1 << j)) >> j) == 0) {
                            printf("0,");
                        }
                        else {
                            printf("1,");
                        }

                    }
                }
                printf("\n");
            }

            datasent = true;
            dataready = false;
            memset(buffer, 0, BUFFER_LEN_8);
            gpio_put(PICO_DEFAULT_LED_PIN, false);
        }

        //printf("now: %f\n", to_us_since_boot(get_absolute_time())/1.0e6);
        sleep_ms(1000); // need some sleep or lines turn out crooked...
    }
}

// all things VGA
void core1_entry() {
    while (1) {
        sleep_ms(1);
    }
}
