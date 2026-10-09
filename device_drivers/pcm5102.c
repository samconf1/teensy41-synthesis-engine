#include "pcm5102.h"
#include "fsl_dmamux.h"
#include "fsl_iomuxc.h"


void pcm5102_init(void) {
    DMAMUX_Init(DMAMUX);
    DMAMUX->CHCFG[20] = 0x00000000; //disable dma
    for (volatile int i = 0; i < 100; i++);

    DMAMUX->CHCFG[20] = 0x00000014;  // set source 20 but keep disabled
    for (volatile int i = 0; i < 100; i++);

    DMAMUX->CHCFG[20] = 0x80000014;  // enable channel 20 and keep source 20
    for (volatile int i = 0; i < 100; i++);
}

