cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(mac_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(mac
    mac.c
)

pico_add_extra_outputs(mac)
target_link_libraries(mac pico_stdlib)

pico_enable_stdio_usb(mac 1)
pico_enable_stdio_uart(mac 0)
