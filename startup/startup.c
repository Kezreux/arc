
#include <stdint.h>

// Defined in the linker script
extern uint32_t _sdata, _edata, _sidata, _sbss, _ebss, _estack;

int main(void);

void SysTick_Handler(void);

void Reset_Handler(void) {
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;

    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;

    main();

    while (1);
}

void Default_Handler(void) { while (1); }

__attribute__((section(".isr_vector")))
uint32_t *vector_table[16] = {
    [0]        = (uint32_t *)&_estack,
    [1]        = (uint32_t *)Reset_Handler,
    [2 ... 14] = (uint32_t *)Default_Handler,
    [15]       = (uint32_t *)SysTick_Handler,
};
