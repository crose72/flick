cmake_minimum_required(VERSION 3.20)
project(flick C ASM)

# Toolchain
set(TOOLCHAIN_PATH "${CMAKE_SOURCE_DIR}/../downloads/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# === Toolchain Extraction ===
set(TOOLCHAIN_ARCHIVE "${CMAKE_SOURCE_DIR}/../downloads/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz")
set(TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../toolchain/")

if(NOT EXISTS "${TOOLCHAIN_DIR}")
    message(STATUS "Extracting toolchain...")
    file(MAKE_DIRECTORY "${TOOLCHAIN_DIR}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xvf ${TOOLCHAIN_ARCHIVE}
        WORKING_DIRECTORY ${TOOLCHAIN_DIR}
    )
endif()

set(TOOLCHAIN_BIN_DIR "${TOOLCHAIN_DIR}/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin")

set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-gcc)
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-size)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -nostdlib -ffreestanding -fno-builtin -O2 -g3 -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16")

add_compile_definitions(STM32H743xx)

# Linker script
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/platform/STM32H7/linker/stm32_flash_h743_2m.ld)

# Sources
set(SRC
    src/main.c
    platform/STM32H7/system_stm32h7xx.c
    platform/STM32H7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c
    platform/STM32H7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c
    platform/STM32H7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c
    platform/STM32H7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c
)

# Includes
include_directories(
    include
    platform
    platform/STM32H7/
    platform/STM32H7/Drivers/STM32H7xx_HAL_Driver/Inc
    platform/STM32H7/startup
    modules/config/configs/MATEKH743
    platform/STM32H7/Drivers/CMSIS/Include
)

# Executable and linking
add_executable(${PROJECT_NAME}.elf ${SRC} platform/STM32H7/startup/startup_stm32h743xx.s)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -nostartfiles
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--gc-sections
)

# Convert to binary and hex after build
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
)
