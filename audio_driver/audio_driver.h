#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include "fsl_edma.h"
#include <stdint.h>
#include <stdbool.h>


extern void set_read_ptr(uint16_t read_ptr);


void DMA4_DMA20_IRQHandler(void);
void dma_callback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);


#endif
