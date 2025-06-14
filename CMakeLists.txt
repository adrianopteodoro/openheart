cmake_minimum_required(VERSION 3.13)

# Include pico-sdk as a submodule
include(pico-sdk/pico_sdk_init.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# VS Code Pico Extension integration (do not edit)
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Board configuration
set(PICO_BOARD pico CACHE STRING "Board type")

project(openheart C CXX ASM)
pico_sdk_init()

# Source files
set(SOURCES
    src/main.c
    src/common.c
    src/clock_control.c
    src/display.c
    #src/region_switch.c
    #src/reset_button.c
    #src/config_store.c
)

add_executable(openheart ${SOURCES})


# this ends up being overridden in the code but I think it's required
# for setting the REFDIV of the system pll
target_compile_definitions(openheart PRIVATE
        PLL_SYS_REFDIV=2
        PLL_SYS_VCO_FREQ_HZ=1075000000
        PLL_SYS_POSTDIV1=5
        PLL_SYS_POSTDIV2=2
        SYS_CLK_HZ=107500000
)

# Add the pico-ssd1306 library
add_subdirectory(pico-ssd1306)

# Include directories
target_include_directories(openheart PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(openheart
    pico_stdlib
    hardware_pwm
    hardware_flash
    hardware_sync
    hardware_clocks
    hardware_watchdog
    pico_multicore
    pico_ssd1306
    hardware_i2c
)

# Generate additional output files (e.g., UF2, bin)
pico_add_extra_outputs(openheart)
