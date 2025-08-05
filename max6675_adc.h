
#pragma once

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

    /* Function prototypes */

    uint16_t read_max6675(void);
    void init_max6675(spi_inst_t *spi_used, unsigned char number);

#ifdef __cplusplus
}
#endif

