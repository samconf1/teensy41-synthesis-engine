CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU_FLAGS = \
    -mcpu=cortex-m7 \
    -mthumb \
    -mfpu=fpv5-d16 \
    -mfloat-abi=hard

CFLAGS = $(CPU_FLAGS) \
    -O3 \
    -ffunction-sections \
    -fdata-sections \
    -fno-builtin \
    -DARDUINO_TEENSY41 \
    -Wall \
    -I./lib \
    -I./periph \
    -I./lib/sdk \
    -I./audio_driver \
    -DCPU_MIMXRT1062DVL6A \
    -D__FPU_PRESENT=1



LDFLAGS = $(CPU_FLAGS) \
    -T imxrt1062_t41.ld \
    -Wl,--gc-sections \
    -nostartfiles \
    -lc \
    -lnosys \
    -lgcc \
    -lm

SRCS = $(wildcard *.c) $(wildcard lib/*.c) $(wildcard periph/*.c) $(wildcard lib/sdk/*.c) $(wildcard audio_driver/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = firmware

all: $(TARGET).hex

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@
	$(SIZE) $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

flash: $(TARGET).hex
	teensy_loader_cli --mcu=TEENSY41 -w -v $(TARGET).hex

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex

.PHONY: all flash clean
