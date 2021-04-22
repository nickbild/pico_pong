#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "blink.pio.h"
#include "hardware/dma.h"

#define DMA_CHANNEL 0

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq);
void horizontal_forever(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side);
void trigger_pio_interrupt(PIO pio, uint sm, uint offset);
void dma_handler();

int main() {
    stdio_init_all();

    // set_sys_clock_khz(40000, true);
    // 400-1600 MHz, 1-7, 1-7
    // set_sys_clock_pll(1440000000, 6, 6); // 40 MHz
    set_sys_clock_pll(1500000000, 6, 2); // 125 MHz, 8 ns/clock, 5 clocks/pixel

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blinky_program);
    uint offset2 = pio_add_program(pio, &interrupt_program);
    
    uint offset4 = pio_add_program(pio1, &horizontal_program);
    // printf("Loaded program at %d\n", offset);


    uint sm = 0;
    // hw_clear_bits(&pio->inte0, 1u << sm);
    // hw_clear_bits(&pio->inte1, 1u << sm);



    blink_pin_forever(pio, 0, offset, 0, 100000000);
    horizontal_forever(pio1, 1, offset4, 1, 100000000, 3);
    
    // blink_pin_forever(pio1, 0, offset, 1, 100000000);
    // clock_pin_forever(pio, 0, offset3, 0, 100000000);

    
    
    dma_channel_config channel_config = dma_channel_get_default_config(DMA_CHANNEL);
    channel_config_set_transfer_data_size(&channel_config, DMA_SIZE_32);
    channel_config_set_read_increment(&channel_config, true);
    channel_config_set_write_increment(&channel_config, false);
    channel_config_set_dreq(&channel_config, pio_get_dreq(pio, sm, true));
    // channel_config_set_ring(&channel_config, false, 1);
    dma_channel_configure(DMA_CHANNEL,
                          &channel_config,
                          &pio->txf[sm],
                          NULL,
                          2,
                          false);
    
    dma_channel_set_irq0_enabled(DMA_CHANNEL, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    dma_handler();


    trigger_pio_interrupt(pio, 3, offset2);
    // pio->irq = 1u << sm;

    while(true) {
        printf("test\n");
        printf("%d\n", clock_get_hz(clk_sys) / 2 * 5);
        sleep_ms(1000);
    }
}

void dma_handler() {
    static uint32_t src[] = {7998, 1959993};
    dma_hw->ints0 = 1u << DMA_CHANNEL;
    dma_channel_set_read_addr(DMA_CHANNEL, &src[0], true);
}

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq) {
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    // printf("Blinking pin %d at %d Hz\n", pin, freq);
    // pio->txf[sm] = clock_get_hz(clk_sys) / 2 * freq; // 312,500,000
}

void horizontal_forever(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side) {
    horizontal_program_init(pio, sm, offset, pin, pin_side);
    pio_sm_set_enabled(pio, sm, true);
}

void trigger_pio_interrupt(PIO pio, uint sm, uint offset) {
    trigger_pio_interrupt_init(pio, sm, offset);
    pio_sm_set_enabled(pio, sm, true);
    sleep_ms(10);
    pio_sm_set_enabled(pio, sm, false);
}
