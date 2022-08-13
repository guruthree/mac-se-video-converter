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

// code having to do with input from the mac

//#define CLOCK_DIV 12 //(188.0/15.6672)
#define CLOCK_DIV 2

//#define HSYNC_PIN 20 // pin 26
//#define VSYNC_PIN 19 // pin 25
//#define VIDEO_PIN 18 // pin 24
#define VIDEO_PIN 20 // pin 26, UART1_TX
#define HSYNC_PIN 21 // pin 27, UART1_RX
#define VSYNC_PIN 22 // pin 29, SD_DAT3

// about 340 lines? 345 covers some extra to see edges
#define MAX_LINES 342
//#define MAX_LINES 256
#define LINE_OFFSET 26 // ignore the first X lines

// buffers need to be multiple of 4 for 32 bit copies
#define LINEBUFFER_LEN_32 16  // X*4*8 = # of bits (22?) (needs to be long enough to be beyond end of hsync pulse?)
#define LINEBUFFER_LEN_8 LINEBUFFER_LEN_32*4
#define BUFFER_LEN_32 MAX_LINES*LINEBUFFER_LEN_32
#define BUFFER_LEN_8 BUFFER_LEN_32*4
uint8_t sebuffer[MAX_LINES][LINEBUFFER_LEN_8];

uint8_t dma_channel;
PIO pio;
int pio_sm;

// on vsync check if the data has been shown, if so, collect new data
// if not, setup to collect new data again
void __isr __time_critical_func(gpio_callback)(uint gpio, uint32_t events) {
    pio_sm_clkdiv_restart(pio, pio_sm); // so that we sample the pixels at the same time in each row
    if (gpio == VSYNC_PIN) {
        if (dma_channel_is_busy(dma_channel))
            gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);


        dma_channel_abort(dma_channel);
        pio_sm_clear_fifos(pio, pio_sm);
//        pio_sm_put(pio, pio_sm, LINEBUFFER_LEN_8*8 << 16 | MAX_LINES);
        pio_sm_put(pio, pio_sm, (LINEBUFFER_LEN_8*8 - 1) << 16 | (MAX_LINES - 1));
//        pio_sm_put(pio, pio_sm, 128 << 16 | MAX_LINES);
        dma_channel_set_write_addr(dma_channel, sebuffer, true);

//        gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);
//        gpio_set_irq_enabled(VSYNC_PIN, GPIO_IRQ_EDGE_FALL, false);
//        dma_channel_wait_for_finish_blocking(dma_channel);
//        for (int c = 0; c < MAX_LINES; c++) {
//            sebuffer[c][0] = 0b1000000;
//        }
//        sleep_ms(1000);
//        gpio_set_irq_enabled(VSYNC_PIN, GPIO_IRQ_EDGE_FALL, true);
    }
}

void se_init() {
    gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);
    sleep_ms(1000);

    gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);

    // setup and init the PIO
    pio = pio1;
    pio_sm = 0;
    int pio_offset = pio_add_program(pio, &videoinput_program);
    videoinput_program_init(pio, pio_sm, pio_offset, VIDEO_PIN, CLOCK_DIV);

    // setup DMA
//    dma_channel = dma_claim_unused_channel(true);
    dma_channel = 6;
    dma_channel_claim(dma_channel);
    dma_channel_config channel_config = dma_channel_get_default_config(dma_channel);

    channel_config_set_transfer_data_size(&channel_config, DMA_SIZE_32); // read 32 bits at a time
    channel_config_set_read_increment(&channel_config, false); // always read from the same place
    channel_config_set_write_increment(&channel_config, true); // go down the buffer writing
    channel_config_set_dreq(&channel_config, pio_get_dreq(pio, pio_sm, false));
    channel_config_set_ring(&channel_config, true, 0); // disable wrapping

    dma_channel_configure(dma_channel,
                          &channel_config,
                          &sebuffer, // write address
                          &pio->rxf[pio_sm], // read address
                          BUFFER_LEN_32, // number of data transfers to do
                          false // start immediately
    );

    // init buffers
    memset(sebuffer, 0, BUFFER_LEN_8);

    // setup hsync pin (this was also setup by setup by pio init?)
    gpio_init(HSYNC_PIN);
    gpio_set_dir(HSYNC_PIN, GPIO_IN);
    gpio_pull_down(HSYNC_PIN); // mac defaults to pulling up
    gpio_set_irq_enabled_with_callback(HSYNC_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // setup interrupt on vsync
    gpio_init(VSYNC_PIN);
    gpio_set_dir(VSYNC_PIN, GPIO_IN);
    gpio_pull_down(VSYNC_PIN); // mac defaults to pulling up
    gpio_set_irq_enabled_with_callback(VSYNC_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

void se_main() {
    while (1) {
        sleep_ms(1); // need some sleep or lines turn out crooked...
        // sleep longer???
        sleep_ms(1000);
    }
}
