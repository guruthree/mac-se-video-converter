cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD "vgaboard" CACHE STRING "" FORCE)

include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(mac_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(mac
    mac.c
)

target_compile_definitions(mac PRIVATE
    PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=270
)

pico_generate_pio_header(mac ${CMAKE_CURRENT_LIST_DIR}/videoinput.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_link_libraries(mac
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_dma
    pico_malloc
	pico_mem_ops
    pico_float
    pico_scanvideo_dpi
)
pico_add_extra_outputs(mac)

pico_enable_stdio_usb(mac 1)
pico_enable_stdio_uart(mac 0)
