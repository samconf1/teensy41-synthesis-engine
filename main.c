#include <stdint.h>
#include <main.h>
#include "audio_driver.h"
#include "audio_preinit.h"
#include "cd74hc4067.h"
#include "pca9555.h"
#include "midi_in.h"


static float audio_block[256];
static float sine_phase = 0.0f;

static uint16_t block_length = 256;

void poll_all(void) {
    cd74_poll_all();
    pca_poll_all();
    midi_read();
    
}

int main(void) {
    start_audio();
    for (volatile uint32_t i = 0; i < 10000000; i++);

    while (1) {
       for (volatile uint32_t i = 0; i < 2000000; i++);
    }

    while (1) {
        uint16_t read_ptr, write_ptr;
        get_buffer_ptrs(&read_ptr, &write_ptr);

        // calculate free space in ring buffer
        uint16_t free_space;
        if (write_ptr >= read_ptr) {
            free_space = buffer_size - (write_ptr - read_ptr) - 1;
    }  else {
            free_space = read_ptr - write_ptr - 1;
    }

        // only generate if theres room for a full block
        if (free_space >= block_length) {
            generate_sine_block(audio_block, &sine_phase, 440.0f);
            send_to_buffer(audio_block);
    }
        if (free_space < block_length * 4) //half full buffer - poll all 
            poll_all():
}

return 0;

}
