#include "config.h"
#include "stm32h743xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_cortex.h"


void SystemClock_Config(void);
void persistentObjectInit(void);

void SystemClock_Config(void) {}
void persistentObjectInit(void) {}

int main(void) {
    // 1. Enable clock to GPIOE (AHB4 bus)
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;

    // 2. Configure PE3 as general-purpose output
    GPIOE->MODER &= ~(3U << (3 * 2));  // Clear mode bits for pin 3
    GPIOE->MODER |=  (1U << (3 * 2));  // Set as output mode (01)
    GPIOE->OTYPER &= ~(1U << 3);       // Push-pull
    GPIOE->OSPEEDR |= (2U << (3 * 2)); // Medium speed
    GPIOE->PUPDR &= ~(3U << (3 * 2));  // No pull-up/pull-down

    // 3. Blink loop
    while (1) {
        GPIOE->ODR ^= (1U << 3);       // Toggle PE3
        for (volatile int i = 0; i < 1000000; ++i); // Crude delay
    }
}
