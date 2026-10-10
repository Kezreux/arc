
#include <stdint.h>
#include "gpio/gpio.h"

// Delay function for testing purposes - delete me at some point
static void delay(uint32_t loops)
{
    for (volatile uint32_t i = 0; i < loops; i++);
}

int main(void)
{

  gpio_enable_port(GPIOA);
  gpio_set_output(GPIOA, 5);
  gpio_set_output(GPIOA, 6);
  gpio_set_output(GPIOA, 7);


  while (1)
  {
    gpio_write(GPIOA, 5, 1);
    gpio_write(GPIOA, 6, 1);
    gpio_write(GPIOA, 7, 1);
    delay(1000000);
    gpio_write(GPIOA, 5, 0);
    gpio_write(GPIOA, 6, 0);
    gpio_write(GPIOA, 7, 0);
    delay(1000000);
  }
}
