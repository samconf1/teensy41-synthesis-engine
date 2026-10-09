#include <stdint.h>

void uart_init();
void parse_midi_message(uint8_t status_nibble);
void midi_read();