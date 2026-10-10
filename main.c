
#include <stdint.h>
#include "gpio/gpio.h"

int main(void)
{

  gpio_enable_port(GPIOA);
  gpio_set_output(GPIOA, 5);

  while (1)
  {
    gpio_write(GPIOA, 5, 1);
    for (volatile int i = 0; i < 100000; i++);
    gpio_write(GPIOA, 5, 0);
    for (volatile int i = 0; i < 100000; i++);
  }
}
