#include <stdint.h>

void pca_init(void);
void pca_poll(void);

typedef struct {
    uint8_t sda_pin;
    uint8_t scl_pin;

    uint8_t pca1_adr;
    uint8_t pca2_adr;
    uint8_t pca3_adr;
    uint8_t pca4_adr;
} pca_t;

