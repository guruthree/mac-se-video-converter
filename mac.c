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
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "videoinput.pio.h"

// fast copies of uint8_t arrays, array length needs to be multiple of 4
int dma_chan32;
void dmacpy(uint8_t *dst, uint8_t *src, uint16_t size) { // inline?
    dma_channel_set_trans_count(dma_chan32, size / 4, false);
    dma_channel_set_read_addr(dma_chan32, src, false);
    dma_channel_set_write_addr(dma_chan32, dst, true);
    dma_channel_wait_for_finish_blocking(dma_chan32);
    // if the write addr isn't NULL, it causes some sort of memory contention slowing down the rest of the loop
    dma_channel_set_write_addr(dma_chan32, NULL, false);
}

// trying to capture video output from a Macintosh SE Logic Board and convert to VGA
// http://www.waveguide.se/?article=compact-mac-video-adapter
// https://tinkerdifferent.com/threads/warpse-25-mhz-68hc000-based-accelerator-for-mac-se.253/page-11#post-10080

// at each hsync record a line, in the blank copy it into big buffer
// after # of lines print out that buffer
// vsync to reset position

// someone doing something similar with the gameboy
// https://old.reddit.com/r/raspberrypipico/comments/lux5k6/pio_issue_attempting_to_capture_gameboy_screen/

// VGA output?
// https://shop.pimoroni.com/products/pimoroni-pico-vga-demo-base?variant=32369520672851
// scanvideo or scanvideo_dpi for doing VGA
// seems to let you write out a line at a time of video?
// some sort of draw n pixels at n colour array of commands?
// https://github.com/raspberrypi/pico-playground
// scanvideo_minimal and test_pattern seem to be the easiest to interpret resources?
// https://blog-boochow-com.translate.goog/article/raspberry-pi-pico-vga.html?_x_tr_sl=ja&_x_tr_tl=en&_x_tr_hl=en&_x_tr_pto=sc


// 0.045 ms (vs 0.0449438202 ms) per horizontal line? (22.22 KHz vs 22.25 KHz?)

// (CLOCK/DV)^-1*SAMPLES*1000 = 1/(horizontal sync in microseconds)
// 1407 samples at 22.25
// 1/((1/22.25)/1000/1407) = 31,305,750 MHz
// clock_div = 125e6 /  31305750

#define CLOCK_SPEED 125e6
#define CLOCK_DIV 3.992876708

#define HSYNC_PIN 20 // pin 26
#define VSYNC_PIN 19 // pin 25
#define VIDEO_PIN 18 // pin 24

// about 370 lines? YES
// usable data = 1024ish, so total size = 1024*370 to start?

// buffers need to be multiple of 4 for 32 bit copies
#define BUFFER_LEN_32 16268
#define BUFFER_LEN_8 BUFFER_LEN_32*4
uint8_t buffer[BUFFER_LEN_8];
bool dataready = false;
uint16_t currentline = 0;

uint8_t dma_channel;

// this should wait for an h-sync
// mark data to be sent
// then disable itself

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == HSYNC_PIN) {
        gpio_set_irq_enabled(HSYNC_PIN, GPIO_IRQ_EDGE_RISE, false);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        dma_channel_set_write_addr(dma_channel, buffer, true);
        dataready = true;
    }
    else if (gpio == VSYNC_PIN) {
        currentline = 0;
    }
}

int main() {
    set_sys_clock_khz(CLOCK_SPEED/1e3, true);
    stdio_init_all(); // need to get VGA output working so that there's some isolation from the PC?

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, true);

    sleep_ms(5000);
    printf("hello world\n");
    gpio_put(PICO_DEFAULT_LED_PIN, false);

    // setup and init the PIO
    PIO pio = pio0;
    int pio_sm = 0;
    int pio_offset = pio_add_program(pio, &videoinput_program);
    videoinput_program_init(pio, pio_sm, pio_offset, VIDEO_PIN, CLOCK_DIV);

    // setup DMA
    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config channel_config = dma_channel_get_default_config(dma_channel);

    channel_config_set_transfer_data_size(&channel_config, DMA_SIZE_32); // read 32 bits at a time
    channel_config_set_read_increment(&channel_config, false); // always read from the same place
    channel_config_set_write_increment(&channel_config, true); // go down the buffer writing
    channel_config_set_dreq(&channel_config, pio_get_dreq(pio, pio_sm, false));

    dma_channel_configure(dma_channel,
                          &channel_config,
                          &buffer, // write address
                          &pio->rxf[pio_sm], // read address
                          BUFFER_LEN_32, // number of data transfers to do
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


    // setup interrupt on hsync
    gpio_init(HSYNC_PIN);
    gpio_set_dir(HSYNC_PIN, GPIO_IN);
    gpio_pull_down(HSYNC_PIN); // mac defaults to pulling up
    gpio_set_irq_enabled_with_callback(HSYNC_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    // setup interrupt on vsync
    gpio_init(VSYNC_PIN);
    gpio_set_dir(VSYNC_PIN, GPIO_IN);
    gpio_pull_down(VSYNC_PIN); // mac defaults to pulling up
    gpio_set_irq_enabled(VSYNC_PIN, GPIO_IRQ_EDGE_RISE, true);
//    gpio_set_irq_enabled_with_callback(VSYNC_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);


    while (1) {
        sleep_us(1);
//        tight_loop_contents();
        if (dataready) {
            printf("waiting\n");
            dma_channel_wait_for_finish_blocking(dma_channel);

            uint32_t at = 0;
            for (uint32_t i = 0; i < BUFFER_LEN_8; i++) {
                for (uint8_t j = 0; j < 8; j ++) {

                    if (((buffer[i] & (1 << j)) >> j) == 0) {
                        printf("0");
                    }
                    else {
                        printf("1");
                    }

                    at++;
                    if (at > 1407-1) {
                        at = 0;
                        printf("\n");
                    }

                }
            }

            dataready = false;
            printf("sleeping\n");
            sleep_ms(5000);
            printf("interrupt enable\n");
            gpio_set_irq_enabled(VSYNC_PIN, GPIO_IRQ_EDGE_RISE, true);
            gpio_put(PICO_DEFAULT_LED_PIN, false);
        }

//        printf("now: %f\n", to_us_since_boot(get_absolute_time())/1.0e6);
    }
}
