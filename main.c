
#include <stdint.h>

// Enables clock
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)
// Sets pin mode
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)
// Output pin on/off toggle
#define GPIOA_ODR (*(volatile uint32_t *)0x40020014)

int main(void)
{

  RCC_AHB1ENR |= (1 << 0);
  GPIOA_MODER &= ~(3 << 11);
  GPIOA_MODER |= (1 << 10);

  while (1)
  {
    GPIOA_ODR |= (1 << 5);
    for (volatile int i = 0; i < 8000000; i++);
    GPIOA_ODR &= ~(1 << 5);
    for (volatile int i = 0; i < 8000000; i++);
  }
}
