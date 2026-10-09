#include <stdint.h>
#include "pca9555.h"
#include <stdlib.h>
#include <fsl_lpi2c.h>



pca_t* pca_init(void) {

    lpi2c_master_config_t pca_master_config ;
L   PI2C_MasterGetDefaultConfig(&pca_master_config);
    pca_transfer_config.baudRate_Hz = 400000U;

    lpi2c_master_transfer_t pca_transfer_config;
    pca_transfer_config.direction = kLPI2C_Write;
    pca_transfer_config.slaveAddress = 0x0;
    pca_transfer_config.subaddress = 0x0;
    pca_transfer_config.subaddressSize = 0;


    pca_t *pca_config_ptr = malloc(sizeof(pca_t));

    if (!pca_config_ptr) return NULL;

    pca_config_ptr->sda_pin = 0;
    pca_config_ptr->scl_pin = 0;
    pca_config_ptr->pca1_address = 0x0;
    pca_config_ptr->pca2_address = 0x0;
    pca_config_ptr->pca3_address = 0x0;
    pca_config_ptr->pca4_address = 0x0;

    return pca_config_ptr;
}


void pca_poll(void) {
    

}