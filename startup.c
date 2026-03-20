#include <stdint.h>

// Linker script symbols
extern unsigned long _estack;
extern unsigned long _stext;
extern unsigned long _etext;
extern unsigned long _stextload;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sdataload;
extern unsigned long _sbss;
extern unsigned long _ebss;
extern unsigned long _flexram_bank_config;


void ResetHandler(void);
static void fault_isr(void);
static void unused_isr(void);
extern void DMA4_DMA20_IRQHandler(void);
extern int main(void);

// Vector table — goes into .startup, which stays in FLASH
__attribute__((section(".startup"), used))
void (*const vector_table[256])(void) = {
    (void(*)(void))&_estack,
    ResetHandler,
    fault_isr,
    fault_isr,
    fault_isr,
    fault_isr,
    fault_isr,
    unused_isr,
    unused_isr,
    unused_isr,
    unused_isr,
    unused_isr,
    unused_isr,
    unused_isr,
    unused_isr,
    unused_isr,
    [16 ... 255] = unused_isr,
    [36] = DMA4_DMA20_IRQHandler,
};

// fault_isr and unused_isr go in .flashmem so they're in FLASH
// (they're referenced by the vector table which is also in FLASH)
__attribute__((section(".flashmem")))
static void fault_isr(void)
{
    while (1) __asm volatile("wfi");
}

__attribute__((section(".flashmem")))
static void unused_isr(void)
{
    while (1) __asm volatile("wfi");
}

// ResetHandler MUST be in FLASH — it runs before ITCM is copied
__attribute__((section(".flashmem")))
void ResetHandler(void)
{
    // Configure FlexRAM banks (ITCM/DTCM/OCRAM split)
    #define IOMUXC_GPR_GPR17 (*(volatile uint32_t *)0x400AC044)
    #define IOMUXC_GPR_GPR16 (*(volatile uint32_t *)0x400AC040)
    #define IOMUXC_GPR_GPR14 (*(volatile uint32_t *)0x400AC038)
    IOMUXC_GPR_GPR17 = (uint32_t)&_flexram_bank_config;
    IOMUXC_GPR_GPR16 = 0x00200007;
    IOMUXC_GPR_GPR14 = 0x00AA0000;

    // Enable FPU (CP10/CP11)
    #define SCB_CPACR (*(volatile uint32_t *)0xE000ED88)
    SCB_CPACR |= (0xF << 20);
    __asm volatile("dsb");
    __asm volatile("isb");

    // Copy ITCM code from flash to ITCM
    // All .text* lives here — must be done before calling any normal functions
    uint32_t *src = (uint32_t *)&_stextload;
    uint32_t *dst = (uint32_t *)&_stext;
    while (dst < (uint32_t *)&_etext)
        *dst++ = *src++;

    // Copy .data from flash to DTCM
    src = (uint32_t *)&_sdataload;
    dst = (uint32_t *)&_sdata;
    while (dst < (uint32_t *)&_edata)
        *dst++ = *src++;

    // Zero .bss
    dst = (uint32_t *)&_sbss;
    while (dst < (uint32_t *)&_ebss)
        *dst++ = 0;

    // Enable I-cache
    #define SCB_ICIALLU (*(volatile uint32_t *)0xE000EF50)
    #define SCB_CCR     (*(volatile uint32_t *)0xE000ED14)
    __asm volatile("dsb");
    __asm volatile("isb");
    SCB_ICIALLU = 0;
    __asm volatile("dsb");
    __asm volatile("isb");
    SCB_CCR |= (1 << 17);

    // Enable D-cache
    #define SCB_CSSELR (*(volatile uint32_t *)0xE000ED84)
    #define SCB_CCSIDR (*(volatile uint32_t *)0xE000ED80)
    #define SCB_DCISW  (*(volatile uint32_t *)0xE000EF60)
    SCB_CSSELR = 0;
    __asm volatile("dsb");
    uint32_t ccsidr = SCB_CCSIDR;
    uint32_t sets = (ccsidr >> 13) & 0x7FFF;
    do {
        uint32_t ways = (ccsidr >> 3) & 0x3FF;
        do {
            SCB_DCISW = ((sets << 5) | (ways << 30));
        } while (ways-- != 0);
    } while (sets-- != 0);
    __asm volatile("dsb");
    SCB_CCR |= (1 << 16);
    __asm volatile("dsb");
    __asm volatile("isb");

    main();

    while (1) __asm volatile("wfi");
}
