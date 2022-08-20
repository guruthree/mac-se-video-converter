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

// the SE's video output:
// 15.6672 MHz pixel clock
// 0.045 ms (vs 0.0449438202 ms) per horizontal line? (22.22 KHz vs 22.25 KHz?)
// 22250/15.6672 = 1420 samples across?
// need to translate 512x342 resolution

//#define SE_CLOCK_DIV 12 //(188.0/15.6672)
#define SE_CLOCK_DIV 2

// pin assignments
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

uint8_t se_dma_channel;
PIO se_pio;
int se_pio_sm;

// the pio program waits for vsync, after which it expects the number of lines and pixels
// so use the GPIO interrupt to send that data gain
// at that point the PIO is about to start writing data so set the DMA channel ready to copy to the buffer
// this is also called for hsync to keep the sub clock divisions aligned with what the mac is actually doing
void __isr __time_critical_func(gpio_callback)(uint gpio, uint32_t events) {
    pio_sm_clkdiv_restart(se_pio, se_pio_sm); // so that we sample the pixels at the same time in each row
    if (gpio == VSYNC_PIN) {
        if (dma_channel_is_busy(se_dma_channel)) {
            // if this is flashing something has gone wrong
            gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);
            // we need to be ready to copy the new data
            dma_channel_abort(se_dma_channel);
            // clear fifos because if the channel is interrupted there will be old pixel data left
            pio_sm_clear_fifos(se_pio, se_pio_sm);
        }

        // send how many pixels and lines to read to the PIO program
        // (this appears to be the only way to set a value of more than 5 bits?)
        pio_sm_put(se_pio, se_pio_sm, (LINEBUFFER_LEN_8*8 - 1) << 16 | (MAX_LINES - 1));
        dma_channel_set_write_addr(se_dma_channel, sebuffer, true);
    }
}

void se_init() {
    gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);
    sleep_ms(1000);

    gpio_put(PICO_DEFAULT_LED_PIN, led_status = !led_status);

    // setup and init the PIO
    se_pio = pio1;
    se_pio_sm = 0;
    int pio_offset = pio_add_program(se_pio, &videoinput_program);
    videoinput_program_init(se_pio, se_pio_sm, pio_offset, VIDEO_PIN, SE_CLOCK_DIV);

    // setup DMA
//    se_dma_channel = dma_claim_unused_channel(true);
    se_dma_channel = 6;
    dma_channel_claim(se_dma_channel);
    dma_channel_config channel_config = dma_channel_get_default_config(se_dma_channel);

    channel_config_set_transfer_data_size(&channel_config, DMA_SIZE_32); // read 32 bits at a time
    channel_config_set_read_increment(&channel_config, false); // always read from the same place
    channel_config_set_write_increment(&channel_config, true); // go down the buffer writing
    channel_config_set_dreq(&channel_config, pio_get_dreq(se_pio, se_pio_sm, false));
    channel_config_set_ring(&channel_config, true, 0); // disable wrapping

    dma_channel_configure(se_dma_channel,
                          &channel_config,
                          &sebuffer, // write address
                          &se_pio->rxf[se_pio_sm], // read address
                          BUFFER_LEN_32, // number of data transfers to do
                          false // start immediately
    );

    // initialise the buffer
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
    // we dont actually need to do anything regularly
    // everything interesting happens in the interrupt
    while (1) {
        sleep_ms(1); // need some sleep or lines turn out crooked...
    }
}
