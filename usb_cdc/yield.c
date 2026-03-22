#include <stdint.h>
volatile uint8_t yield_active_check_flags = 0;
volatile uint32_t systick_millis_count = 0;

// RAM vector table — must be 1024 byte aligned for VTOR register
__attribute__((aligned(1024)))
volatile uint32_t _VectorsRam[256];


char *ultoa(unsigned long val, char *buf, int radix)
{
    char tmp[33];
    int i = 0;
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }
    while (val > 0) {
        int digit = val % radix;
        tmp[i++] = digit < 10 ? '0' + digit : 'A' + digit - 10;
        val /= radix;
    }
    int j = 0;
    while (i--) buf[j++] = tmp[i+1];
    buf[j] = '\0';
    return buf;
}
