#pragma once
#include <stdint.h>

// delay stub
static inline void delay(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 100000; i++);
}

// yield stub — no RTOS, nothing to yield to
static inline void yield(void) {}

// yield flags — Arduino event system, not needed
extern volatile uint8_t yield_active_check_flags;
#define YIELD_CHECK_USB_SERIAL  0x01