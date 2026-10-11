
#ifndef STM32F446RE_H
#define STM32F446RE_H

#include <stdint.h>

/*
 * ==================================================
 * STM32F446RE register map
 * RM0390 - ST peripherals
 * PM0214 - Cortex M4 Core
 * ==================================================
 */

/*
 * --------------------------------------------------
 * Base addresses
 * --------------------------------------------------
 */

#define GPIOA_BASE 0x40020000UL
#define GPIOB_BASE 0x40020400UL
#define GPIOC_BASE 0x40020400UL
#define RCC_BASE 0x40023800UL
#define SYSTICK_BASE 0xE000E000UL

/*
 * --------------------------------------------------
 * RCC - Reset and clock control 
 * --------------------------------------------------
 */

#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x30))

#define RCC_AHB1ENR_GPIOAEN (1U << 0)
#define RCC_AHB1ENR_GPIOBEN (1U << 1)
#define RCC_AHB1ENR_GPIOCEN (1U << 2)

/*
 * --------------------------------------------------
 * GPIO
 * --------------------------------------------------
 */

typedef struct {
  volatile uint32_t MODER;    // Offset 0x00
  volatile uint32_t OTYPER;   // Offset 0x04
  volatile uint32_t OSPEEDR;  // Offset 0x08
  volatile uint32_t PUPDR;    // Offset 0x0C
  volatile uint32_t IDR;      // Offset 0x10
  volatile uint32_t ODR;      // Offset 0x14
  volatile uint32_t BSSR;     // Offset 0x18
} GPIO_t;

#define GPIOA ((GPIO_t *)GPIOA_BASE)
#define GPIOB ((GPIO_t *)GPIOB_BASE)
#define GPIOC ((GPIO_t *)GPIOC_BASE)

/*
 * --------------------------------------------------
 * Systick
 * --------------------------------------------------
 */

#define STK_LOAD (*(volatile uint32_t *)(SYSTICK_BASE + 0x14))
#define STK_CVR (*(volatile uint32_t *)(SYSTICK_BASE + 0x18))
#define STK_CTRL (*(volatile uint32_t *)(SYSTICK_BASE + 0x10))

#define STK_CTRL_ENABLE (1U << 0)
#define STK_CTRL_TICKINT (1U << 1)
#define STK_CTRL_CLKSOURCE (1U << 2)


#endif
