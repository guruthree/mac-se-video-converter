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
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "videoinput.pio.h"

// someone doing something similar with the gameboy
// https://old.reddit.com/r/raspberrypipico/comments/lux5k6/pio_issue_attempting_to_capture_gameboy_screen/

// https://shop.pimoroni.com/products/pimoroni-pico-vga-demo-base?variant=32369520672851

// http://www.waveguide.se/?article=compact-mac-video-adapter
// need to read 704 bit between 22.25 kHz pulses?

// scanvideo or scanvideo_dpi for doing VGA
// seems to let you write out a line at a time of video?
// some sort of draw n pixels at n colour array of commands?
// https://github.com/raspberrypi/pico-playground
// scanvideo_minimal and test_pattern seem to be the easiest to interpret resources?
// https://blog-boochow-com.translate.goog/article/raspberry-pi-pico-vga.html?_x_tr_sl=ja&_x_tr_tl=en&_x_tr_hl=en&_x_tr_pto=sc

// 60 Hz * 22.25 KHz = 1.335 MHz

#define CLOCK_SPEED 120e6
#define CLOCK_DIV 90 // at 120 MHz, every 90 samples would be 1.333 MHz

uint64_t a = 0, b = 0;

#define SYNC_PIN 2 // pin 4


// this should wait for an h-sync
// mark data to be sent
// then disable itself
void gpio_callback(uint gpio, uint32_t events) {
    a = to_us_since_boot(get_absolute_time());
    printf("on time: %lld\n", a - b);
    b = a;
}

int main() {
    stdio_init_all(); // need to get VGA output working so that there's some isolation from the PC?

    // setup the DAC
    PIO pio = pio0;
    int pio_sm = 0;
    int pio_offset;

    // init PIO
    pio_offset = pio_add_program(pio, &videoinput_program);
    videoinput_program_init(pio, pio_sm, pio_offset, 0, CLOCK_DIV);

    // setup DMA
    uint8_t dma_channel;

    sleep_ms(5000);
    printf("hello world\n");

    gpio_init(SYNC_PIN);
    gpio_set_dir(SYNC_PIN, GPIO_IN);
    gpio_pull_up(SYNC_PIN);
    gpio_set_irq_enabled_with_callback(SYNC_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (1) {
        sleep_ms(100);
    }
}
