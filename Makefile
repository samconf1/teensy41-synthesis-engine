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
    -D__FPU_PRESENT=1 \
    -mlong-calls

USB_CFLAGS = $(CPU_FLAGS) \
    -O3 \
    -ffunction-sections \
    -fdata-sections \
    -fno-builtin \
    -DARDUINO_TEENSY41 \
    -Wall \
    -I./usb_cdc \
    -I./usb_cdc/avr \
    -DCPU_MIMXRT1062DVL6A \
    -D__FPU_PRESENT=1 \
    -DLAYOUT_US_ENGLISH \
    -DUSB_SERIAL \
    -mlong-calls

LDFLAGS = $(CPU_FLAGS) \
    -T imxrt1062_t41.ld \
    -Wl,--gc-sections \
    -nostartfiles \
    -specs=nosys.specs

LIBS = -lc -lm -lgcc

SRCS     = $(wildcard *.c) $(wildcard lib/*.c) $(wildcard periph/*.c) $(wildcard lib/sdk/*.c) $(wildcard audio_driver/*.c)
USB_SRCS = $(wildcard usb_cdc/*.c)
ALL_SRCS = $(SRCS) $(USB_SRCS)

OBJS     = $(SRCS:.c=.o)
USB_OBJS = $(USB_SRCS:.c=.o)
ALL_OBJS = $(OBJS) $(USB_OBJS)

TARGET = firmware

all: $(TARGET).hex

$(TARGET).elf: $(ALL_OBJS)
	$(CC) $(LDFLAGS) $(ALL_OBJS) $(LIBS) -o $@
	$(SIZE) $@

$(USB_OBJS): %.o: %.c
	$(CC) $(USB_CFLAGS) -c $< -o $@

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

flash: $(TARGET).hex
	teensy_loader_cli --mcu=TEENSY41 -w -v $(TARGET).hex

clean:
	rm -f $(ALL_OBJS) $(TARGET).elf $(TARGET).hex

.PHONY: all flash clean