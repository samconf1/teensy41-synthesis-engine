#pragma once
#include <stdint.h>

// Forward declarations of USB CDC functions
// Avoids including PJRC headers in non-USB files

void usb_init(void);
int usb_serial_write(const void *buffer, uint32_t size);
int usb_serial_putchar(uint8_t c);
int usb_serial_available(void);
int usb_serial_getchar(void);
int usb_serial_dtr(void);
