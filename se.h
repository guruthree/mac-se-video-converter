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

//#define HSYNC_PIN 20 // pin 26
//#define VSYNC_PIN 19 // pin 25
//#define VIDEO_PIN 18 // pin 24
#define HSYNC_PIN 20 // pin 26, UART1_TX
#define VSYNC_PIN 21 // pin 27, UART1_RX
#define VIDEO_PIN 22 // pin 29, SD_DAT3

// about 340 lines? 345 covers some extra to see edges
#define MAX_LINES 345
#define LINE_OFFSET 26

// buffers need to be multiple of 4 for 32 bit copies
#define LINEBUFFER_LEN_32 22  // X*4*8 = # of bits
#define LINEBUFFER_LEN_8 LINEBUFFER_LEN_32*4
#define BUFFER_LEN_32 MAX_LINES*LINEBUFFER_LEN_32
#define BUFFER_LEN_8 BUFFER_LEN_32*4
uint8_t sebuffer[MAX_LINES][LINEBUFFER_LEN_8];
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
                dmacpy(sebuffer[currentline], linebuffer0, LINEBUFFER_LEN_8);
            }
            else { // even
                dmacpy(sebuffer[currentline], linebuffer1, LINEBUFFER_LEN_8);
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
        }
    }
}

void se_init() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, true);

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
                          LINEBUFFER_LEN_32, // number of data transfers to do
                          false // start immediately
    );

    // init buffers
    memset(sebuffer, 0, BUFFER_LEN_8);
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
}

float selastprint = 0;
void se_main() {
    while (1) {
        sleep_ms(1); // need some sleep or lines turn out crooked...

        if (dataready && !datasent) {
            printf("a frame's ready\n");
            // print out - there may be a problem trying to print lines longer than 4096 characters
/*            for (uint16_t k = 0; k < MAX_LINES; k++) { // line
                for (uint16_t i = 0; i < LINEBUFFER_LEN_8; i++) { // byte
                    for (uint8_t j = 0; j < 8; j ++) { // bit

                        if (((sebuffer[k][i] & (1 << j)) >> j) == 0) {
                            printf("0,");
                        }
                        else {
                            printf("1,");
                        }

                    }
                }
                printf("\n");
            }*/
            sleep_ms(30);

            datasent = true;
            dataready = false;
            memset(sebuffer, 0, BUFFER_LEN_8);
            gpio_put(PICO_DEFAULT_LED_PIN, false);
        }

        //printf("now: %f\n", to_us_since_boot(get_absolute_time())/1.0e6);
        sleep_ms(1000); // need some sleep or lines turn out crooked...

        float senewprint = to_us_since_boot(get_absolute_time())/1.0e6;
        if (senewprint - selastprint > 10.0f/1000.0f) {
            selastprint = senewprint;
            printf("se now: %f\n", senewprint);
        }
    }
}
