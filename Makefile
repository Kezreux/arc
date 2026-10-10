CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

SRC_DIRS = src drivers startup
BUILD    = build
TARGET   = $(BUILD)/firmware

SRCS = $(shell find $(SRC_DIRS) -name '*.c')
INC_DIRS = $(shell find $(SRC_DIRS) -type d)
OBJS = $(SRCS:%.c=$(BUILD)/%.o)
CFLAGS  += $(addprefix -I,$(INC_DIRS))

CFLAGS  = -mcpu=cortex-m4 -mthumb -Wall -Wextra -O0 -g -ffreestanding
CFLAGS += $(addprefix -I,$(SRC_DIRS))
CFLAGS += -MMD -MP
LDFLAGS = -T linker/stm32f446re.ld -nostdlib

all: $(TARGET).bin

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

flash: $(TARGET).bin
	st-flash --reset write $< 0x08000000

clean:
	rm -rf $(BUILD)

-include $(OBJS:.o=.d)

.PHONY: all flash clean
