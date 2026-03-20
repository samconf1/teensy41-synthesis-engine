#include <stdint.h>
#include <stdbool.h>


extern uint16_t block_length;
extern uint16_t buffer_size;
extern volatile uint32_t buffer_ptrs;
extern uint16_t sample_rate;
extern volatile bool audio_running;

extern const int16_t *zero_block;
extern int16_t* ring_buffer;

void set_read_ptr(uint16_t read);
void set_write_ptr(uint16_t write);
void get_buffer_ptrs(uint16_t *read_ptr, uint16_t *write_ptr);