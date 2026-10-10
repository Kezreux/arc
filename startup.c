
#include <stdint.h>

// Defined in the linker script
extern uint32_t _sdata, _edata, _sidata, _sbss, _ebss, _estack;

int main(void);

void Reset_Handler(void) {
    // Copy initialized data from flash to RAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;

    // Zero the .bss section
    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;

    main();

    while (1);  // main should never return, but just in case
}

void Default_Handler(void) { while (1); }

// Minimal vector table: initial stack pointer + reset handler.
// The real chip has ~100 entries; we only need these two to blink.
__attribute__((section(".isr_vector")))
uint32_t *vector_table[] = {
    (uint32_t *)&_estack,          // 0x00: initial stack pointer
    (uint32_t *)Reset_Handler,     // 0x04: reset handler
};
