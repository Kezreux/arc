
#include <stdint.h>
#include "gpio/gpio.h"
#include "systick/systick.h"

typedef enum { MODE_OFF, MODE_ON, MODE_BLINK } led_mode_t;

static void led_on(int state)
{
   gpio_write(GPIOA, 5, state);
   gpio_write(GPIOA, 6, state);
   gpio_write(GPIOA, 7, state);
}

static void led_blink(void)
{
  gpio_toggle(GPIOA, 5);
  gpio_toggle(GPIOA, 6);
  gpio_toggle(GPIOA, 7);
}


int main(void)
{

  gpio_enable_port(GPIOA);
  gpio_set_output(GPIOA, 5);
  gpio_set_output(GPIOA, 6);
  gpio_set_output(GPIOA, 7);
 
  gpio_set_input(GPIOA, 0);
  gpio_set_input(GPIOA, 1);
  gpio_set_pull(GPIOA, 0, GPIO_PULL_UP);
  gpio_set_pull(GPIOA, 1, GPIO_PULL_UP);

  systick_init();

  led_mode_t mode = MODE_OFF;
  uint32_t was_pressed_PA0 = 0;
  uint32_t was_pressed_PA1 = 0;
  uint32_t led_last_blink = 0;

  while (1)
  {

    uint8_t pressed_PA0 = (gpio_read(GPIOA, 0) == 0);
    uint8_t pressed_PA1 = (gpio_read(GPIOA, 1) == 0);

    if (pressed_PA0 && !was_pressed_PA0)
    {
      if (mode == MODE_ON)
        mode = MODE_OFF;
      else 
        mode = MODE_ON;
    }

    if (pressed_PA1 && !was_pressed_PA1)
    {
      if (mode == MODE_ON)
        mode = MODE_OFF;
      else
        mode = MODE_BLINK;
    }

    was_pressed_PA0 = pressed_PA0;
    was_pressed_PA1 = pressed_PA1;

    switch (mode)
    {
      case MODE_OFF:
      led_on(0);
      break;

      case MODE_ON:
      led_on(1);
      break;

      case MODE_BLINK:
      if (millis() - led_last_blink >= 2000)
      {
        led_last_blink = millis();
        led_blink();
      }
      break;
    }

  }
}
