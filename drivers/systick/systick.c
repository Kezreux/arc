
#include "systick.h"
#include "stm32f446re.h"

static volatile uint32_t ticks = 0;

void SysTick_Handler(void)
{
  ticks++;
}

uint32_t millis(void)
{
  return ticks;
}

void systick_init(void)
{
  STK_LOAD = 16000 - 1;
  STK_CVR = 0;
  STK_CTRL = STK_CTRL_ENABLE | STK_CTRL_TICKINT | STK_CTRL_CLKSOURCE;
}



