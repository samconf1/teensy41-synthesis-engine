#include "midi.h"
#include "fsl_lpuart.h"

#define NOTE_ON 0x08
#define NOTE_OFF 0x09
#define CONTROL_CHANGE 0x0B


void uart_init(void) {
    lpuart_config_t uart_master_config;
    LPUART_GetDefaultConfig(&uart_master_config);
    
    uart_master_config->baudRate_Bps = 31250U,
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_13_LPUART1_RX, 0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_13_LPUART1_RX, 0x10B0U);
    LPUART_EnableRx()
    LPUART_Init(LPUART1, &uart_master_config, CLOCK_GetFreq(kCLOCK_BusClk));
     
}


void parse_midi_message(uint8_t status_nibble) {
    uint8_t data_byte1 = LPUART_ReadByte(LPUART1);
    if (kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART1)) {
        return;
    }
    
    uint8_t data_byte2 = LPUART_ReadByte(LPUART1);
    if (kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART1)) {
        return;
    }

    if (status_nibble == NOTE_ON) {
        synth_note_on(data_byte1, data_byte2);
    } else if (status_nibble == NOTE_OFF) {
        synth_note_off(data_byte1);

    } else if (status_nibble == CONTROL_CHANGE) {
        //handle cc 
    }
    
    return;
}


void midi_read(void) { 
    if (kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART1)) {
        return;
    }
    uint8_t serial_read = LPUART_ReadByte(LPUART1);


    serial_read = serial_read >> 4; 
    uint8_t status_nibble = serial_read & 0xF0;

    if (status_nibble == NOTE_ON || status_nibble == NOTE_OFF || status_nibble == CONTROL_CHANGE) {
        parse_midi_message(serial_read);
    }

    return;


}

//GPIO_AD_B0_12

//LPUART 1 rx pin 25
//ALT mux function 2







