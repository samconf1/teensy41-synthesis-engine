#include <stdint.h>
#include <stdbool.h>
#include "MIMXRT1062.h"
#include "core_cm7.h"
#include "audio_preinit.h"

uint16_t block_length = 512;
uint16_t buffer_size = 256*8*2;
uint16_t sample_rate = 48000;
volatile bool audio_running = false;


//STORED IN DTCM SO RING BUFFER IS NEVER CACHED.
__attribute__((section(".dtcm_bss"), aligned(32)))
static int16_t ring_buffer_storage[256 * 8 * 2];
int16_t *ring_buffer = ring_buffer_storage;


//zero block stored in read only
__attribute__((section(".rodata"), aligned(32)))
static const int16_t zero_block_storage[256 * 8 * 2] = {0};
const int16_t *zero_block = zero_block_storage;


volatile uint32_t buffer_ptrs = 0; //lower 16 bits = write_ptr, upper 16 bits = read_ptr

void get_buffer_ptrs(uint16_t *read_ptr, uint16_t *write_ptr) { //read buffer ptrs into 2 seperate variables
    uint32_t ptrs = buffer_ptrs;
    __DMB();
    
    *read_ptr = (uint16_t)(ptrs >> 16);
    *write_ptr = (uint16_t)(ptrs & 0xFFFF);
}


void set_read_ptr(uint16_t read) { //atomic set to prevent race conditions
    uint32_t old_val;
    uint32_t new_val;
    
    do {
        old_val = __LDREXW(&buffer_ptrs); // load exclusive
        new_val = (old_val & 0x0000FFFF) | ((uint32_t)read << 16);

    } while (__STREXW(new_val, &buffer_ptrs)); // store exclusive
    __DMB();
}

void set_write_ptr(uint16_t write) { //atomic write to prevent race conditions
    uint32_t old_val;
    uint32_t new_val;
    
    do {
        old_val = __LDREXW(&buffer_ptrs);  
        new_val = (old_val & 0xFFFF0000) | (uint32_t)write;

    } while (__STREXW(new_val, &buffer_ptrs)); 
    __DMB();
}
