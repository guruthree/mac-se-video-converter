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
#include "dmacpy.h"


// trying to capture video output from a Macintosh SE Logic Board and convert to VGA
// http://www.waveguide.se/?article=compact-mac-video-adapter
// https://tinkerdifferent.com/threads/warpse-25-mhz-68hc000-based-accelerator-for-mac-se.253/page-11#post-10080
// http://www.mac.linux-m68k.org/devel/plushw.php

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

    // setup everything for video capture
    se_init();

    // setup dma for dmacopy (faster than memcpy)
    dmacpy_init();

    // launch VGA on core1
    multicore_launch_core1(core1_entry);
    sleep_ms(1000);

    // VGA is up, go go go!
    gpio_set_irq_enabled_with_callback(VSYNC_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);

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
