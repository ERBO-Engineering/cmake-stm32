cmake_minimum_required(VERSION 3.16)


# xpack arm-none-eabi requires this
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/build-tool/cmake/stm32_gcc.cmake)

project(cmake-stm32f4 C ASM)

set(STM32_CUBE_F4_PATH ${CMAKE_CURRENT_SOURCE_DIR}/BSP)

find_package(CMSIS COMPONENTS STM32F411VE REQUIRED)

set(PROJECT_SOURCES
    src/main.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} CMSIS::STM32::F411VE STM32::NoSys)

stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})
stm32_generate_hex_file(${PROJECT_NAME})