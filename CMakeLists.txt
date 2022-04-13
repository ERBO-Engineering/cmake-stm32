cmake_minimum_required(VERSION 3.16)


# xpack arm-none-eabi requires this
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/build-tool/cmake/stm32_gcc.cmake)

project(blinky C ASM)

# Retrieve HAL and CMSIS from the ST git repo
stm32_fetch_cmsis(F4)
stm32_fetch_hal(F4)

find_package(CMSIS COMPONENTS STM32F411VE REQUIRED)

set(PROJECT_SOURCES
    src/main.c
)

add_executable(blinky ${PROJECT_SOURCES})
target_link_libraries(blinky CMSIS::STM32::F411VE STM32::NoSys)
stm32_print_size_of_target(blinky)