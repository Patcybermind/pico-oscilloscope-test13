# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project())
include(pico_sdk_import.cmake)

# Define the project
project(pico-oscilloscope-test13 C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()




# Add executable
add_executable(pico-oscilloscope-test13 pico-oscilloscope-test13.c)

pico_set_program_name(pico-oscilloscope-test13 "pico-oscilloscope-test13")
pico_set_program_version(pico-oscilloscope-test13 "0.1")

# Process PIO assembly file
pico_generate_pio_header(pico-oscilloscope-test13 ${CMAKE_CURRENT_LIST_DIR}/parallel_reader.pio)

# Enable/disable USB and UART output
pico_enable_stdio_uart(pico-oscilloscope-test13 0)
pico_enable_stdio_usb(pico-oscilloscope-test13 1)

# Link standard libraries
target_link_libraries(pico-oscilloscope-test13
        pico_stdlib
        hardware_pio
        hardware_dma)

# Add standard include directories
target_include_directories(pico-oscilloscope-test13 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR})

# Generate necessary output formats (UF2, ELF, etc.)
pico_add_extra_outputs(pico-oscilloscope-test13)
