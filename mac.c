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

// trying to capture video output from a Macintosh SE Logic Board and convert to VGA
// or convert to monochrome composite video

// some related information
// http://www.waveguide.se/?article=compact-mac-video-adapter
// https://tinkerdifferent.com/threads/warpse-25-mhz-68hc000-based-accelerator-for-mac-se.253/page-11#post-10080
// http://www.mac.linux-m68k.org/devel/plushw.php
// https://nerdhut.de/2016/06/26/macintosh-classic-crt-1/

//#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "pico/sync.h"

#include "videoinput.pio.h"


// need an even divisor of the pixel clock for the system clock
// for both the SE and VGA
// 188/15.6672 = 12 for the SE
// 188/37.6 = 5 for VGA

#define CLOCK_SPEED 188e6

bool led_status = false;
#include "se.h"
#include "vga.h"

void core1_entry();

int main() {
    // set clock speed
    set_sys_clock_khz(CLOCK_SPEED/1e3, true);
    stdio_init_all(); // need to get VGA output working so that there's some isolation from the PC?

    // initialise onboard LED for debug flashing
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);

    sleep_ms(1000);

    // launch VGA on core1
    multicore_launch_core1(core1_entry);
    sleep_ms(1000);

    gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);

    // setup everything for video capture
    se_init();
    se_main();

    // should never reach here
    while (1) {
        sleep_ms(1);
    }
}

// all things VGA
void core1_entry() {
    vga_init();
    vga_main();

    // should never reach here
    while (1) {
        sleep_ms(1);
    }
}
