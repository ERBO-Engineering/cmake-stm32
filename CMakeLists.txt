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
# USB device sources
    ${STM32_CUBE_F4_PATH}/USB_DEVICE/Target/usbd_conf.c

    ${STM32_CUBE_F4_PATH}/USB_DEVICE/App/usb_device.c
    ${STM32_CUBE_F4_PATH}/USB_DEVICE/App/usbd_cdc_if.c
    ${STM32_CUBE_F4_PATH}/USB_DEVICE/App/usbd_desc.c

    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c


    ${STM32_CUBE_F4_PATH}//Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c

    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
#for the mic
    ${STM32_CUBE_F4_PATH}/PDM2PCM/App/pdm2pcm.c

#pherips
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c

    ${STM32_CUBE_F4_PATH}/Core/Src/stm32f4xx_hal_msp.c
    # src/system_stm32f4xx.c
    src/stm32f4xx_it.c
    src/main.c
    # ${STM32_CUBE_F4_PATH}/Core/Src/main.c

)

# Set project includes
include_directories(
    ${STM32_CUBE_F4_PATH}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Inc

    ${STM32_CUBE_F4_PATH}/PDM2PCM/App
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_Audio/Addons/PDM/Inc

    ${STM32_CUBE_F4_PATH}/USB_DEVICE/App
    ${STM32_CUBE_F4_PATH}/USB_DEVICE/Target
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    )

# Include HAL if required else only use CMSIS
if (${USE_HAL})
    message("Building with HAL")
    add_definitions(-DUSE_HAL)
    find_package(HAL COMPONENTS STM32F4 REQUIRED)
    set(LIBRARIES 
        CMSIS::STM32::F411VE
        HAL::STM32::F4::GPIO
        HAL::STM32::F4::CRC
        HAL::STM32::F4::LL_USB
        HAL::STM32::F4::CORTEX
        HAL::STM32::F4::RCC
        HAL::STM32::F4::I2S
        HAL::STM32::F4::PCD
        HAL::STM32::F4::DMA)
else()
    message("Building without HAL")
    set(LIBRARIES CMSIS::STM32::F411VE)
endif()

# Set the project executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

link_directories(
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_Audio/Addons/PDM/Lib
)

# add_link_options(
#     -L${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_Audio/Addons/PDM/Lib
#     )

# Link libraries to executable based on HAL or CMSIS
target_link_libraries(${PROJECT_NAME}
 ${LIBRARIES}
 STM32::NoSys
 ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_Audio/Addons/PDM/Lib/libPDMFilter_CM4_GCC_wc32.a
 m
)

link_libraries(
    PDMFilter_CM4_GCC_wc32
)


# Generated wanted files and printout debug information
stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})
stm32_generate_hex_file(${PROJECT_NAME})