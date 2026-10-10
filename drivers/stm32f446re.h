
#ifndef STM32F446RE
#define STM32F446RE

#include <stdint.h>

typedef struct {
  volatile uint32_t MODER;    // Offset 0x00
  volatile uint32_t OTYPER;   // Offset 0x04
  volatile uint32_t OSPEEDR;  // Offset 0x08
  volatile uint32_t PUPDR;    // Offset 0x0C
  volatile uint32_t IDR;      // Offset 0x10
  volatile uint32_t ODR;      // Offset 0x14
  volatile uint32_t BSSR;     // Offset 0x18
} GPIO_t;

#define GPIOA ((GPIO_t *)0x40020000)
#define GPIOB ((GPIO_t *)0x40020400)
#define GPIOC ((GPIO_t *)0x40020800)

#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)

#endif
