
#ifndef GPIO_H
#define GPIO_H

#include "stm32f446re.h"

void gpio_enable_port(GPIO_t *port);              // which RCC bit, depends on port
void gpio_set_output(GPIO_t *port, int pin);      // MODER = 01
void gpio_set_input(GPIO_t *port, int pin);       // MODER = 00
void gpio_write(GPIO_t *port, int pin, int val);  // set or clear ODR bit
int  gpio_read(GPIO_t *port, int pin);            // read IDR bit, return 0 or 1
void gpio_toggle(GPIO_t *port, int pin);          // flip ODR bit

#endif
