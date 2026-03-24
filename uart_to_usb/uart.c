#include "uart.h"
#include <stdint.h>

#define CCM_CCGR3       (*(volatile uint32_t *)0x400FC074) //Clock control module gate register
#define CCM_CSCDR1      (*(volatile uint32_t *)0x400FC024)

#define LPUART6_BASE    0x40198000
#define LPUART6_BAUD    (*(volatile uint32_t *)(LPUART6_BASE + 0x10))
#define LPUART6_STAT    (*(volatile uint32_t *)(LPUART6_BASE + 0x14))
#define LPUART6_CTRL    (*(volatile uint32_t *)(LPUART6_BASE + 0x18))
#define LPUART6_DATA    (*(volatile uint32_t *)(LPUART6_BASE + 0x1C))

#define IOMUXC_SW_MUX_GPIO_AD_B0_03 (*(volatile uint32_t *)0x401F80A0)
#define IOMUXC_SW_PAD_GPIO_AD_B0_03 (*(volatile uint32_t *)0x401F8290)
#define IOMUXC_LPUART6_TX_SELECT    (*(volatile uint32_t *)0x401F8530)

static void uart_putchar(char c)
{
    while (!(LPUART6_STAT & (1 << 23)));
    LPUART6_DATA = c;
}

void uart_init(void)
{
    CCM_CCGR3 |= (3 << 4);
    CCM_CSCDR1 &= ~(0x7 << 24);
    IOMUXC_SW_MUX_GPIO_AD_B0_03 = 2;
    IOMUXC_SW_PAD_GPIO_AD_B0_03 = 0x10B0;
    IOMUXC_LPUART6_TX_SELECT = 1;
    LPUART6_CTRL = 0;
    LPUART6_BAUD = (15 << 24) | 43;
    LPUART6_CTRL = (1 << 19);
}

void println(const char *s)
{
    while (*s) {
        if (*s == '\n') uart_putchar('\r');
        uart_putchar(*s++);
    }
    uart_putchar('\r');
    uart_putchar('\n');
}