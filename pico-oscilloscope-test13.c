#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "parallel_reader.pio.h"  // PIO assembly will be compiled into this header
#include <stdio.h>  // Include stdio.h for printf

#define SAMPLE_RATE 2000
#define NUM_SAMPLES 1000

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
    stdio_init_all();
    sleep_ms(5000);  // Wait 5 seconds before starting
    
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &parallel_reader_program);
    init_pio(pio, sm, offset);
    uint dma_chan = dma_claim_unused_channel(true);
    init_dma(dma_chan, pio, sm);
    
    while (dma_channel_is_busy(dma_chan));
    pio_sm_set_enabled(pio, sm, false);
    
    // Send back collected data via USB
    printf("done");
    for (int i = 0; i < NUM_SAMPLES; i++) {
        printf("%d\n", buffer[i]);
    }
    
    return 0;
}