#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "parallel_reader.pio.h"  // PIO assembly will be compiled into this header
#include <stdio.h>  // Include stdio.h for printf

#define SAMPLE_RATE 18000000 // this will be divided in 2 because the pio program has 2 instructions per sample
#define NUM_SAMPLES 10

uint8_t buffer[NUM_SAMPLES];

void init_pio(PIO pio, uint sm, uint offset) {
    pio_sm_config c = parallel_reader_program_get_default_config(offset);
    sm_config_set_in_pins(&c, 0);
    sm_config_set_clkdiv(&c, clock_get_hz(clk_sys) / SAMPLE_RATE);
    sm_config_set_in_shift(&c, false, false, 8);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

void init_dma(uint dma_chan, PIO pio, uint sm) {
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));
    dma_channel_configure(dma_chan, &c,
                          buffer,       // Destination
                          &pio->rxf[sm], // Source
                          NUM_SAMPLES,   // Number of transfers
                          true);         // Start immediately
}

int main() {
    set_sys_clock_khz(200000, true);
    stdio_init_all();
    
    // blink led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    for (int i = 0; i < 5; i++) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(250);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(250);
    }
    sleep_ms(5000);  // Wait 5 seconds before starting
    for (int i = 6; i > 0; i--) {
        printf("Starting in %d seconds...\n", i);
        sleep_ms(1000);
    }

    // set clock to 200mhz
    

    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &parallel_reader_program);
    init_pio(pio, sm, offset);
    uint dma_chan = dma_claim_unused_channel(true);
    init_dma(dma_chan, pio, sm);
    
    while (dma_channel_is_busy(dma_chan));
    pio_sm_set_enabled(pio, sm, false);
    
    // Send back collected data via USB
    // countdown s
    for (int i = 5; i > 0; i--) {
        printf("Sending in %d seconds...\n", i);
        sleep_ms(1000);
    }
    printf("Sending data...\n");
    for (int i = 0; i < NUM_SAMPLES; i++) {
        printf("%d\n", buffer[i]);
    }
    
    return 0;
}