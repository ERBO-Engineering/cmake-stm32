cmake_minimum_required(VERSION 3.15.3)

project(cmake-stm32f4)

enable_language(C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(STM32CUBEMX_GENERATED_BSP

    # We might not need everything :D
    BSP/Core/Src/stm32f4xx_it.c 
    BSP/Core/Src/stm32f4xx_hal_msp.c 

    BSP/USB_HOST/Target/usbh_conf.c 
    BSP/USB_HOST/Target/usbh_platform.c 
    BSP/USB_HOST/App/usb_host.c 

    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c 
    BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c 

    
    BSP/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_core.c 
    BSP/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c 
    BSP/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c 
    BSP/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_pipes.c 
    BSP/Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Src/usbh_cdc.c  

    BSP/Core/Src/system_stm32f4xx.c 

    BSP/startup_stm32f411xe.s
)

set(SOURCE_FILES 
    src/main.c
)

set(EXECUTABLE ${PROJECT_NAME}.out)

add_executable(${EXECUTABLE} ${STM32CUBEMX_GENERATED_BSP} ${SOURCE_FILES})

target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F411xE
)

target_include_directories(${EXECUTABLE} PRIVATE
    BSP/USB_HOST/App 
    BSP/USB_HOST/Target 
    BSP/Core/Inc 
    BSP/Drivers/STM32F4xx_HAL_Driver/Inc 
    BSP/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy 
    BSP/Middlewares/ST/STM32_USB_Host_Library/Core/Inc 
    BSP/Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc 
    BSP/Drivers/CMSIS/Device/ST/STM32F4xx/Include 
    BSP/Drivers/CMSIS/Include
)

target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard

        -fdata-sections
        -ffunction-sections

        -Wall
        # If CONFIG is Debug, then add option -Og, otherwise add nothing. Noice!
        $<$<CONFIG:Debug>:-Og>
)

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/BSP/STM32F411VETx_FLASH.ld
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -specs=nano.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
)

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE}
)

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
)