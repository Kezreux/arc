
#include "gpio.h"

void gpio_enable_port(GPIO_t *port)
{
  if (port == GPIOA)
      RCC_AHB1ENR |= (1 << 0);
  else if (port == GPIOB)
      RCC_AHB1ENR |= (1 << 1);
  else if (port == GPIOC)
      RCC_AHB1ENR |= (1 << 2);
}


void gpio_set_output(GPIO_t *port, int pin)
{
  // Clears the MODER memory address first, then sets the output mode bit
  port->MODER &= ~(3 << (pin * 2));
  port->MODER |= (1 << (pin * 2));
}


void gpio_set_input(GPIO_t *port, int pin)
{
  // Clears the MODER memory address to set the bits in input mode
  port->MODER &= ~(3 << (pin * 2));
}


void gpio_write(GPIO_t *port, int pin, int state)
{
  if (state)
    port->ODR |= (1 << pin);
  else 
    port->ODR &= ~(1 << pin);
}


int gpio_read(GPIO_t *port, int pin)
{
  // Shifts the pin to 0 and masks of everything else, leaving just one bit to read as high or low
  return (port->IDR >> pin) & 1;
}


void gpio_toggle(GPIO_t *port, int pin)
{
  port->ODR ^= (1 << pin);
}


void gpio_set_pull(GPIO_t *port, int pin, int pull)
{
    port->PUPDR &= ~(3U << (pin * 2));
    port->PUPDR |=  ((uint32_t)pull << (pin * 2));
}
