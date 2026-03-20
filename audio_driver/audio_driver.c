#include "audio_driver.h"
#include "audio_preinit.h"
#include <stdint.h>
#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "fsl_common.h"
#include "fsl_sai.h"
#include "fsl_clock.h"
#include "fsl_iomuxc.h"
#include "MIMXRT1062.h"
#include "core_cm7.h"

#define DMA_CHANNEL 20
static void dma_callback_manual(void);
void enable_system_clocks(void);
void sai_init(void);
void dma_init(void);


void start_audio(void) {
    enable_system_clocks();
    sai_init();
    dma_init();
    return;
}


void enable_system_clocks(void) {
    const clock_audio_pll_config_t audioPllConfig = {
        .loopDivider = 32,
        .postDivider = 1,
        .numerator = 96,      
        .denominator = 125, 
        .src = kCLOCK_PllClkSrc24M
    };
    
    CLOCK_InitAudioPll(&audioPllConfig);
    
    CLOCK_SetMux(kCLOCK_Sai1Mux, 2);
    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, 0);
    CLOCK_SetDiv(kCLOCK_Sai1Div, 63);
    
    CLOCK_EnableClock(kCLOCK_Sai1);
    CLOCK_EnableClock(kCLOCK_Iomuxc);
    CLOCK_EnableClock(kCLOCK_IomuxcSnvs);
    CLOCK_EnableClock(kCLOCK_Dma);
    
    for (volatile int i = 0; i < 1000; i++);
}



void sai_init(void) {
    if (zero_block == NULL) {
        return;
    }

    uint32_t sai1_mclk = CLOCK_GetClockRootFreq(kCLOCK_Sai1ClkRoot);

    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_14_SAI1_TX_BCLK, 0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_15_SAI1_TX_SYNC, 0);
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_01_SAI1_TX_DATA00, 0);

    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_14_SAI1_TX_BCLK, 0x10B0U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_01_SAI1_TX_DATA00, 0x10B0U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_15_SAI1_TX_SYNC, 0x10B0U);
    
    SAI_Init(SAI1);
    SAI_TxReset(SAI1);

    sai_transceiver_t tx_config;
    SAI_GetClassicI2SConfig(&tx_config, kSAI_WordWidth16bits, kSAI_Stereo, 1U);
    tx_config.masterSlave = kSAI_Master;
    tx_config.syncMode = kSAI_ModeAsync;
    

    
    SAI_TxSetConfig(SAI1, &tx_config);

    sai_fifo_t fifo_config = {
    .fifoWatermark = 8
};

    SAI1->TCR4 &= ~(0x3UL << 24); 
    SAI1->TCR4 |= (0x3UL << 24);

    SAI_TxSetFifoConfig(SAI1, &fifo_config);
    SAI_TxSetBitClockRate(SAI1, sai1_mclk, sample_rate, 16, 2);

    
    SAI_TxEnableDMA(SAI1, kSAI_FIFORequestDMAEnable, true);
    SAI1->TCSR |= I2S_TCSR_FRDE_MASK;  //force frde
}

void dma_init(void) {
    
    DMAMUX_Init(DMAMUX);
    DMAMUX->CHCFG[20] = 0x00000000; //disable dma
    for (volatile int i = 0; i < 100; i++);

    DMAMUX->CHCFG[20] = 0x00000014;  // set source 20 but keep disabled
    for (volatile int i = 0; i < 100; i++);

    DMAMUX->CHCFG[20] = 0x80000014;  // enable channel 20 and keep source 20
    for (volatile int i = 0; i < 100; i++);

    edma_config_t edma_config;
    EDMA_GetDefaultConfig(&edma_config);
    EDMA_Init(DMA0, &edma_config);

    NVIC_SetPriority(DMA4_DMA20_IRQn, 2);
    NVIC_EnableIRQ(DMA4_DMA20_IRQn);

    uint32_t bytes_per_request = sizeof(int32_t);
    uint32_t iterations = (block_length * sizeof(int16_t)) / sizeof(int32_t);;
    
    
    // tcd config
    DMA0->TCD[DMA_CHANNEL].SADDR = (uint32_t)zero_block;
    DMA0->TCD[DMA_CHANNEL].ATTR = DMA_ATTR_SSIZE(2) | DMA_ATTR_DSIZE(2);
    DMA0->TCD[DMA_CHANNEL].NBYTES_MLNO = bytes_per_request;
    DMA0->TCD[DMA_CHANNEL].SLAST = 0;
    DMA0->TCD[DMA_CHANNEL].DADDR = (uint32_t)&SAI1->TDR[0];
    DMA0->TCD[DMA_CHANNEL].DOFF = 0; 
    DMA0->TCD[DMA_CHANNEL].CITER_ELINKNO = iterations;
    DMA0->TCD[DMA_CHANNEL].DLAST_SGA = 0;
    DMA0->TCD[DMA_CHANNEL].CSR = DMA_CSR_INTMAJOR_MASK;
    DMA0->TCD[DMA_CHANNEL].BITER_ELINKNO = iterations;
    DMA0->TCD[DMA_CHANNEL].SOFF = 4;

    
    SAI_TxEnable(SAI1, true);
    
    //prefill fifo with zeros
    for (volatile int i = 0; i < 16; i++) {
        SAI1->TDR[0] = 0x00000000;  
    }
    
    
    SAI1->TCR3 |= I2S_TCR3_TCE(1);

    SAI1->TCSR |= 0x001C0000;

    
    
    // enable channel
    DMA0->SERQ = DMA_SERQ_SERQ(DMA_CHANNEL);


    for (volatile int i = 0; i < 100; i++); //let dma run 


    SAI1->TCSR |= I2S_TCSR_FEF_MASK | I2S_TCSR_SEF_MASK | I2S_TCSR_WSF_MASK;  // clear all flags


}


void DMA4_DMA20_IRQHandler(void) { //interrupt handler
    if (!audio_running) {
        DMA0->CINT = DMA_CHANNEL; //clear interrupt
        dma_callback_manual();
        return;
    }
}


static void dma_callback_manual(void) {
    uint16_t read_ptr, write_ptr;
    get_buffer_ptrs(&read_ptr, &write_ptr);
    
    int16_t available; //calculate available space in buffer.
    if (write_ptr >= read_ptr) {
        available = write_ptr - read_ptr;
    } else {
        available = buffer_size - read_ptr + write_ptr;
    
    }

    const int16_t *src_ptr; //pick source for next transfer
    if (available >= block_length) {
        src_ptr = &ring_buffer[read_ptr];
        set_read_ptr((read_ptr + block_length) % buffer_size);
    } else {
        src_ptr = zero_block;
    }



    //reload tcd
    uint32_t iterations = block_length / 2;  // 512 samples

    DMA0->TCD[DMA_CHANNEL].CSR &= ~DMA_CSR_DONE_MASK;
    
    DMA0->TCD[DMA_CHANNEL].SADDR = (uint32_t)src_ptr;
    DMA0->TCD[DMA_CHANNEL].CITER_ELINKNO = iterations;
    DMA0->TCD[DMA_CHANNEL].BITER_ELINKNO = iterations;
    DMA0->TCD[DMA_CHANNEL].CSR = DMA_CSR_INTMAJOR_MASK;  // reenable interrupt


}



