#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// https://shop.pimoroni.com/products/pimoroni-pico-vga-demo-base?variant=32369520672851

// http://www.waveguide.se/?article=compact-mac-video-adapter
// need to read 704 bit between 22.25 kHz pulses?

// scanvideo or scanvideo_dpi for doing VGA
// seems to let you write out a line at a time of video?
// some sort of draw n pixels at n colour array of commands?
// https://github.com/raspberrypi/pico-playground
// scanvideo_minimal and test_pattern seem to be the easiest to interpret resources?
// https://blog-boochow-com.translate.goog/article/raspberry-pi-pico-vga.html?_x_tr_sl=ja&_x_tr_tl=en&_x_tr_hl=en&_x_tr_pto=sc

uint64_t a = 0, b = 0;

void gpio_callback(uint gpio, uint32_t events) {
    a = to_us_since_boot(get_absolute_time());
    printf("on time: %lld\n", a - b);
    b = a;
}

#define SYNC_PIN 2 // pin 4

int main() {
    stdio_init_all(); // need to get VGA output working so that there's some isolation from the PC?

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
