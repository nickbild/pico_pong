# set(PICO_DEFAULT_BINARY_TYPE copy_to_ram)

add_executable(pico_pong)

pico_generate_pio_header(pico_pong ${CMAKE_CURRENT_LIST_DIR}/pong.pio)

target_sources(pico_pong PRIVATE pong.c)

# Enable USB output, disable UART output.
pico_enable_stdio_usb(pico_pong 1)
pico_enable_stdio_uart(pico_pong 0)

target_link_libraries(pico_pong PRIVATE pico_stdlib hardware_pio hardware_dma)
pico_add_extra_outputs(pico_pong)

# add url via pico_set_program_url
example_auto_set_url(pico_pong)