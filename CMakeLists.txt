add_executable(pico_vga)

pico_generate_pio_header(pico_vga ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

target_sources(pico_vga PRIVATE blink.c)

# Enable USB output, disable UART output.
pico_enable_stdio_usb(pico_vga 1)
pico_enable_stdio_uart(pico_vga 0)

target_link_libraries(pico_vga PRIVATE pico_stdlib hardware_pio hardware_dma)
pico_add_extra_outputs(pico_vga)

# add url via pico_set_program_url
example_auto_set_url(pico_vga)