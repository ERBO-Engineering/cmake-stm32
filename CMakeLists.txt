cmake_minimum_required(VERSION 3.16)

# Setup toolchian
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/build-tool/cmake/stm32_gcc.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# set project
project(cmake-stm32 C ASM)

# Set build options
set(USE_HAL 1)

# Setup cubemx directory
set(STM32_CUBE_F4_PATH ${CMAKE_CURRENT_SOURCE_DIR}/BSP)

# Get CMSIS package needed for compilation
find_package(CMSIS COMPONENTS STM32F411VE REQUIRED)

# Set project sources
set(PROJECT_SOURCES
    # ST sources
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c
    ${STM32_CUBE_F4_PATH}/Core/Src/stm32f4xx_hal_msp.c

    # project sources
    src/stm32f4xx_it.c
    src/main.c
    src/led_controller.c
    src/led_pattern.c
    src/led_game.c
)

# get the unity include to allow autocomplete during testing
# Run the 'gem env gemdir' command and capture its output which leads to the ceedling path
execute_process(
    COMMAND gem which ceedling 
    OUTPUT_VARIABLE CEEDLING_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GEM_COMMAND_RESULT
)
# strip away the ceedling.rb from the path so we can use it in a include path
string(REGEX REPLACE "/ceedling\\.rb$" "" CEEDLING_PATH "${CEEDLING_PATH}")

# Setup unity path from the vendor directory of ceedling
set(UNITY_PATH  ${CEEDLING_PATH}/../vendor/unity/src)

# Set project includes
include_directories(
    # ST includes
    ${STM32_CUBE_F4_PATH}/USB_HOST/App

    # project includes
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    #test includes
    ${CMAKE_CURRENT_SOURCE_DIR}/build/test/mocks
    ${UNITY_PATH}


)

# Include HAL if required else only use CMSIS
if (${USE_HAL})
    message("Building with HAL")
    add_definitions(-DUSE_HAL)
    find_package(HAL COMPONENTS STM32F4 REQUIRED)
    set(
        LIBRARIES
        CMSIS::STM32::F411VE
        HAL::STM32::F4::CORTEX
        HAL::STM32::F4::RCC
        HAL::STM32::F4::GPIO
        HAL::STM32::F4::ADC
        HAL::STM32::F4::DMA
    )
else()
    message("Building without HAL")
    set(LIBRARIES CMSIS::STM32::F411VE)
endif()

add_definitions(
    -DLED_COUNT=4
)

# Set the project executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Link libraries to executable based on HAL or CMSIS
target_link_libraries(${PROJECT_NAME}
 ${LIBRARIES}
 STM32::NoSys
)

# Generated wanted files and printout debug information
stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})
stm32_generate_hex_file(${PROJECT_NAME})