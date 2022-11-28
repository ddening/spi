/*
 * spi_config.h
 *
 * Created: 28.11.2022 17:44:06
 *  Author: workstation-uni
 */ 
#ifndef SPI_CONFIG_H_
#define SPI_CONFIG_H_

typedef enum {
    SPI_MSB = 0x00,
    SPI_LSB = 0x01
} data_order_t;

typedef enum mode_t {
    SPI_MODE0 = 0x00, // CPOL = 0, CPHA = 0
    SPI_MODE1 = 0x01, // CPOL = 0, CPHA = 1
    SPI_MODE2 = 0x02, // CPOL = 1, CPHA = 0
    SPI_MODE3 = 0x03  // CPOL = 1, CPHA = 1
} mode_t;

typedef enum {
    SPI_CLOCK_DIV4 = 0x00,
    SPI_CLOCK_DIV16 = 0x01,
    SPI_CLOCK_DIV64 = 0x02,
    SPI_CLOCK_DIV128 = 0x03,
    // The following clock settings will set the SPI2X bit in the SPSR.
    // The SCK frequency will be doubled when the SPI is in master mode.
    SPI_CLOCK_DIV2 = 0x04,
    SPI_CLOCK_DIV8 = 0x05,
    SPI_CLOCK_DIV32 = 0x06,
    SPI_CLOCK_DIV64X = 0x07
} clock_rate_t;

typedef struct spi_config_t {
    mode_t mode;
    data_order_t data_order;   
    clock_rate_t clockrate; 
} spi_config_t;

spi_config_t test = {
    .mode       = SPI_MODE0,
    .data_order = SPI_MSB,
    .clockrate  = SPI_CLOCK_DIV2
};

#endif /* SPI_CONFIG_H_ */