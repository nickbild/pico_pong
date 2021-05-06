#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "blink.pio.h"
#include "hardware/dma.h"

#define DMA_CHANNEL 0
#define DMA_CHANNEL_H 1

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side);
void horizontal_forever(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side);
void hsync_forever(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side);
void hsync_forever_initial(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side);
void dma_handler();
void dma_handler_h();

uint8_t src_h[224000] = {};

int main() {
    stdio_init_all();

    for (int i=0; i<224000; i++) {
        src_h[i] = 0;
    }

    for (int i=0; i<224000; i=i+640) {
        src_h[i] = 1;
    }

    for (int i=639; i<224000; i=i+640) {
        src_h[i] = 1;
    }

    for (int i=0; i<640; i++) {
        src_h[i] = 1;
    }

    for (int i=222719; i<223360; i++) {
        src_h[i] = 1;
    }

    

    // 400-1600 MHz, 1-7, 1-7
    set_sys_clock_pll(1550000000, 6, 1); // 250 MHz, 4 ns/clock, 10 clocks/pixel

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blinky_program);
    uint offset2 = pio_add_program(pio1, &hsync_program);
    
    uint offset4 = pio_add_program(pio1, &horizontal_program);
    // printf("Loaded program at %d\n", offset);


    uint sm = 0;
    // hw_clear_bits(&pio->inte0, 1u << sm);
    // hw_clear_bits(&pio->inte1, 1u << sm);



    blink_pin_forever(pio, 0, offset, 0, 100000000, 4);
    hsync_forever(pio1, 1, offset2, 4, 100000000, 3);
    hsync_forever_initial(pio1, 2, offset2, 5, 100000000, 3); // Initial

    horizontal_forever(pio1, 0, offset4, 1, 100000000, 3);
    
    
    
    dma_channel_config channel_config = dma_channel_get_default_config(DMA_CHANNEL);
    channel_config_set_transfer_data_size(&channel_config, DMA_SIZE_32);
    channel_config_set_read_increment(&channel_config, true);
    channel_config_set_write_increment(&channel_config, false);
    channel_config_set_dreq(&channel_config, pio_get_dreq(pio, sm, true));
    dma_channel_configure(DMA_CHANNEL,
                          &channel_config,
                          &pio->txf[sm],
                          NULL,
                          5,
                          false);
    
    dma_channel_set_irq0_enabled(DMA_CHANNEL, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    dma_handler();




    
    dma_channel_config channel_config_h = dma_channel_get_default_config(DMA_CHANNEL_H);
    channel_config_set_transfer_data_size(&channel_config_h, DMA_SIZE_8);
    channel_config_set_read_increment(&channel_config_h, true);
    channel_config_set_write_increment(&channel_config_h, false);
    channel_config_set_dreq(&channel_config_h, pio_get_dreq(pio1, sm, true));
    dma_channel_configure(DMA_CHANNEL_H,
                          &channel_config_h,
                          &pio1->txf[sm],
                          NULL,
                          224000,
                          false);
    
    dma_channel_set_irq1_enabled(DMA_CHANNEL_H, true);
    irq_set_exclusive_handler(DMA_IRQ_1, dma_handler_h);
    irq_set_enabled(DMA_IRQ_1, true);
    dma_handler_h();


    // trigger_pio_interrupt(pio, 3, offset2);
    // pio->irq = 1u << sm;

    while(true) {
        for (int i=3000; i<4000; i++) {
            src_h[i-1] = 0;
            src_h[i] = 1;
            sleep_ms(5);
        }
    }
}

void dma_handler() {
    static uint32_t src[] = {349, 7750, 1295473, 15999, 80750};
    dma_hw->ints0 = 1u << DMA_CHANNEL;
    dma_channel_set_read_addr(DMA_CHANNEL, &src[0], true);
} // 1039220

void dma_handler_h() {
    // Not showing: 10 on left, 10 on right.
    // 25.6 - 25.42 = 0.18 microseconds too long = 180 ns = 4.5 pixels
    // static uint8_t src_h[] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
    // static uint8_t src_h[] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};    

    dma_hw->ints0 = 1u << DMA_CHANNEL_H;
    dma_channel_set_read_addr(DMA_CHANNEL_H, &src_h[0], true);
}

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side) {
    blink_program_init(pio, sm, offset, pin, pin_side);
    pio_sm_set_enabled(pio, sm, true);
    pio->txf[sm] = 7994;

    // printf("Blinking pin %d at %d Hz\n", pin, freq);
    // pio->txf[sm] = clock_get_hz(clk_sys) / 2 * freq; // 312,500,000
}

void horizontal_forever(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side) {
    horizontal_program_init(pio, sm, offset, pin, pin_side);
    
    pio->sm[sm].shiftctrl = (8u << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB);
    
    pio_sm_set_enabled(pio, sm, true);
    pio->txf[sm] = 639;
}

void hsync_forever(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side) {
    hsync_program_init(pio, sm, offset, pin, pin_side);
    pio_sm_set_enabled(pio, sm, true);
    pio->txf[sm] = 6809;
}

void hsync_forever_initial(PIO pio, uint sm, uint offset, uint pin, uint freq, uint pin_side) {
    hsync_program_init(pio, sm, offset, pin, pin_side);
    pio_sm_set_enabled(pio, sm, true);
    pio->txf[sm] = 6554;
}
