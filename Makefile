CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m4 -mthumb -Wall -O0 -g -ffreestanding -nostdlib
LDFLAGS = -T linker.ld -nostdlib

SRCS = startup.c main.c gpio/gpio.c

all: firmware.bin

firmware.elf: $(SRCS) linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o $@

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@

flash: firmware.bin
	st-flash write firmware.bin 0x08000000

clean:
	rm -f *.elf *.bin

.PHONY: all flash clean
