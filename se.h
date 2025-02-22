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
bi_decl(bi_1pin_with_name(VIDEO_PIN, "Video Input"));
bi_decl(bi_1pin_with_name(HSYNC_PIN, "HSync Input"));
bi_decl(bi_1pin_with_name(VSYNC_PIN, "VSync Input"));

// about 340 lines? 345 covers some extra to see edges
#define MAX_LINES 342
//#define MAX_LINES 256
#define LINE_OFFSET 26 // ignore the first X lines

// buffers need to be multiple of 4 for 32 bit copies
#define WIDTH_PIXELS 512
#define LINEBUFFER_LEN_32 WIDTH_PIXELS/32  // X*4*8 = # of bits (22?) (needs to be long enough to be beyond end of hsync pulse?)
#define LINEBUFFER_LEN_8 LINEBUFFER_LEN_32*4
#define BUFFER_LEN_32 MAX_LINES*LINEBUFFER_LEN_32
#define BUFFER_LEN_8 BUFFER_LEN_32*4
uint8_t sebuffer[MAX_LINES][LINEBUFFER_LEN_8];

// keep track of the last vsync to be able to display lost sync message
uint32_t last_vsync_time;
#define FONT_HEIGHT 7
#define SIGNAL_LOST1_LEN 18
uint8_t signal_lost1[FONT_HEIGHT][SIGNAL_LOST1_LEN] = {
    // message line 1
    {0xC0,0xE6,0x38,0x80,0xE7,0x01,0xD1,0xF7,0x78,0x0E,0xE0,0x38,0x51,0xE4,0x3D,0x9F,0xF7,0x00},
    {0x40,0x15,0x45,0x40,0x10,0x00,0x11,0x11,0x05,0x11,0x10,0x45,0x53,0x14,0x44,0x44,0x10,0x01},
    {0x40,0x15,0x05,0x40,0x10,0x00,0x11,0x11,0x05,0x11,0x10,0x44,0x53,0x14,0x44,0x44,0x10,0x01},
    {0x40,0x14,0x05,0x9F,0xF3,0x7D,0x11,0x11,0x7D,0xD1,0x17,0x44,0x55,0xF4,0x3D,0xC4,0xF7,0x00},
    {0x40,0xF4,0x05,0x00,0x14,0x00,0x0A,0x11,0x05,0x11,0x10,0x44,0x99,0x12,0x24,0x44,0x90,0x00},
    {0x40,0x14,0x45,0x00,0x14,0x00,0x0A,0x11,0x05,0x11,0x10,0x45,0x99,0x12,0x44,0x44,0x10,0x01},
    {0x40,0x14,0x39,0xC0,0xE3,0x01,0xC4,0xF7,0x78,0x0E,0xE0,0x38,0x11,0xE1,0x45,0x84,0x17,0x01},
};
#define SIGNAL_LOST2_LEN 8
uint8_t signal_lost2[FONT_HEIGHT][SIGNAL_LOST2_LEN] = {
    // message line 2
    {0x40,0xE4,0x00,0xDE,0xE7,0x44,0x4E,0x00},
    {0xC0,0x14,0x01,0x01,0x11,0x4D,0x51,0x00},
    {0xC0,0x14,0x01,0x01,0x11,0x4C,0x51,0x00},
    {0x40,0x15,0x01,0x0E,0x11,0x54,0x51,0x00},
    {0x40,0x16,0x01,0x10,0xD1,0x65,0x5F,0x00},
    {0x40,0x16,0x01,0x10,0x11,0x65,0x51,0x00},
    {0x40,0xE4,0x00,0xCF,0xE7,0x44,0xD1,0x07},
};

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

        // mark when the last vsync was
        last_vsync_time = to_ms_since_boot(get_absolute_time());
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

    // initialise the buffer with a black screen
    memset(sebuffer, 0xFF, BUFFER_LEN_8);

    // initialise vsync lost timeout
    last_vsync_time = to_ms_since_boot(get_absolute_time());

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
    // we dont actually _need_ to do anything regularly
    // everything interesting happens in the interrupt
    while (1) {
        // but might as well every now and then check if we have sync
        if (to_ms_since_boot(get_absolute_time()) - last_vsync_time > 50) {
            // if it's been more than one second, signal is probably lost
            // write into sebuffer a message to that effect
            for (uint8_t c = 0; c < FONT_HEIGHT; c++) {
                memcpy(sebuffer[MAX_LINES/2 - FONT_HEIGHT*3 + c] + LINEBUFFER_LEN_8/2 - SIGNAL_LOST1_LEN/2, signal_lost1[c], SIGNAL_LOST1_LEN*sizeof(uint8_t));
                memcpy(sebuffer[MAX_LINES/2 + FONT_HEIGHT*2 + c] + LINEBUFFER_LEN_8/2 - SIGNAL_LOST2_LEN/2, signal_lost2[c], SIGNAL_LOST2_LEN*sizeof(uint8_t));
            }
        }
        sleep_ms(10); // need some sleep or lines turn out crooked...
    }
}
