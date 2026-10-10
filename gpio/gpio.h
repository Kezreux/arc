
#ifndef GPIO_H
#define GPIO_H

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

void gpio_enable_port(GPIO_t *port);              // which RCC bit, depends on port
void gpio_set_output(GPIO_t *port, int pin);      // MODER = 01
void gpio_set_input(GPIO_t *port, int pin);       // MODER = 00
void gpio_write(GPIO_t *port, int pin, int val);  // set or clear ODR bit
int  gpio_read(GPIO_t *port, int pin);            // read IDR bit, return 0 or 1
void gpio_toggle(GPIO_t *port, int pin);          // flip ODR bit

#endif
