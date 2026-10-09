#include "main.h"

void cd74_init(void);
void cd74_poll_all(void);


typedef struct {
    int8_t s0_port;
    int8_t s0_pin;

    int8_t s1_port;
    int8_t s1_pin;

    int8_t s2_port;
    int8_t s2_pin;

    int8_t s3_port;
    int8_t s3_pin;


    int8_t cmn1_port;
    int8_t cmn1_pin;

    int8_t cmn2_port;
    int8_t cmn2_pin;

    int8_t cmn3_port;
    int8_t cmn3_pin;

    int8_t cmn4_port;
    int8_t cmn4_pin;

    uint8_t parameter_array[63];

} cd74_t;


