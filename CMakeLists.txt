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
    ${STM32_CUBE_F4_PATH}/USB_HOST/App/usb_host.c
    ${STM32_CUBE_F4_PATH}/USB_HOST/Target/usbh_conf.c
    ${STM32_CUBE_F4_PATH}/PDM2PCM/App/pdm2pcm.c
    ${STM32_CUBE_F4_PATH}/USB_HOST/Target/usbh_platform.c

    # ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_conf_template.c this is fake
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_core.c
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_pipes.c
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Src/usbh_cdc.c
   
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c

    ${STM32_CUBE_F4_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c

    # src/system_stm32f4xx.c
    src/stm32f4xx_it.c
    src/main.c
    # ${STM32_CUBE_F4_PATH}/Core/Src/main.c

)

# Set project includes
include_directories(
    ${STM32_CUBE_F4_PATH}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${STM32_CUBE_F4_PATH}/USB_HOST/App
    ${STM32_CUBE_F4_PATH}/USB_HOST/Target
    ${STM32_CUBE_F4_PATH}/PDM2PCM/App
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_Audio/Addons/PDM/Inc
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Host_Library/Core/Inc
    ${STM32_CUBE_F4_PATH}/Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc
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
        HAL::STM32::F4::I2S)
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