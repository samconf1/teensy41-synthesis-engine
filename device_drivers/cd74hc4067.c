#include "cd74hc4067.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include <stdlib.h>

cd74_t* cd74_init(void) {
    cd74_t* cd74_config_ptr = malloc(sizeof(cd74_t));
    
    if (!cd74_config_ptr) return NULL;

    cd74_config_ptr->s0_port = 0;
    cd74_config_ptr->s0_pin = 0;

    cd74_config_ptr->s1_port = 0;
    cd74_config_ptr->s1_pin =0;

    cd74_config_ptr->s2_port = 0;
    cd74_config_ptr->s2_pin = 0;

    cd74_config_ptr->s3_port = 0;
    cd74_config_ptr->s3_pin = 0;

    cd74_config_ptr->cmn1_port = 0;
    cd74_config_ptr->cmn1_pin = 0;

    cd74_config_ptr->cmn2_port = 0;
    cd74_config_ptr->cmn2_pin = 0;

    cd74_config_ptr->cmn3_port = 0;
    cd74_config_ptr->cmn3_pin = 0;

    cd74_config_ptr->cmn4_port = 0;
    cd74_config_ptr->cmn4_pin = 0;

    return cd74_config_ptr; //return pointer to function. When called in run, assign this pointer to a variable 
    //cd74_t *pointer = cd74_init();



    //set each pin to gpio 
    //duplicate for each pin and after fabrication, find each macro for each pin 
    IOMUXC_SetPinMux(macro, 0); //pad and mux mode 

    //set each pin to digital output
    //each pin can use the same config struct, so parse in same struct in pininit


    gpio_pin_config_t out_config = {
    .direction     = kGPIO_DigitalOutput,
    .outputLogic   = 0,
    .interruptMode = kGPIO_NoIntmode
    };

    //pininit


    GPIO_PinInit(port, pin_number, &out_config);

}



void cd74_poll_all(cd74_t *cd74_config) {
    uint8_t chip_number = 0;

    for (int8_t channel = 0; channel < 16; i++) {
        GPIO_PinWrite(cd74_config->s0_port, cd74_config->s0_pin, (channel >> 0) & 1);
        GPIO_PinWrite(cd74_config->s1_port, cd74_config->s1_pin, (channel >> 1) & 1);
        GPIO_PinWrite(cd74_config->s2_port, cd74_config->s2_pin, (channel >> 2) & 1);
        GPIO_PinWrite(cd74_config->s3_port, cd74_config->s3_pin, (channel >> 3) & 1);

        uint8_t pinread1 = GPIO_PinRead(cd74_config->cmn1_port, cd74_config->cmn1_pin);
        uint8_t pinread2 = GPIO_PinRead(cd74_config->cmn2_port, cd74_config->cmn2_pin);
        uint8_t pinread3 = GPIO_PinRead(cd74_config->cmn3_port, cd74_config->cmn3_pin);
        uint8_t pinread4 = GPIO_PinRead(cd74_config->cmn4_port, cd74_config->cmn4_pin);

        //store logic for values needs writing

    chip_number = chip_number + 1;

    }
}

