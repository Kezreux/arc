
#include <stdint.h>
#include "gpio/gpio.h"
#include "systick/systick.h"

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

  systick_init();

  uint32_t last5 = 0, last6 = 0, last7 = 0;
  
  while (1)
  {

    uint32_t now = millis();

    if (now - last5 >= 250) { last5 += 250; gpio_toggle(GPIOA, 5); }
    if (now - last6 >= 400) { last6 += 400; gpio_toggle(GPIOA, 6); }
    if (now - last7 >= 650) { last7 += 650; gpio_toggle(GPIOA, 7); }
  }
}
