/*************************************************************************
* Title		: spi_config.h
* Author	: Dimitri Dening
* Created	: 28.11.2022 17:44:06
* Software	: Microchip Studio V7
* Hardware	: Atmega1284P
* License	: MIT License
* Usage		: see Doxygen manual
*
*       Copyright (C) 2021 Dimitri Dening
*
*       Permission is hereby granted, free of charge, to any person obtaining a copy
*       of this software and associated documentation files (the "Software"), to deal
*       in the Software without restriction, including without limitation the rights
*       to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*       copies of the Software, and to permit persons to whom the Software is
*       furnished to do so, subject to the following conditions:
*
*       The above copyright notice and this permission notice shall be included in all
*       copies or substantial portions of the Software.
*
*       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*       IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*       FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*       AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*       LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*       OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*       SOFTWARE.
*
* NOTES:
*	This file should only be included from <spi.h>, never directly.
*************************************************************************/
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
    SPI_CLOCK_DIV2 = 0x04,
    SPI_CLOCK_DIV4 = 0x00,
    SPI_CLOCK_DIV8 = 0x05,
    SPI_CLOCK_DIV16 = 0x01,
    SPI_CLOCK_DIV32 = 0x06,
    SPI_CLOCK_DIV64 = 0x02,
    SPI_CLOCK_DIV64X = 0x07,
    SPI_CLOCK_DIV128 = 0x03
} clock_rate_t;

typedef struct spi_config_t {
    data_order_t data_order;
    mode_t mode;
    clock_rate_t clockrate;
} spi_config_t;

static spi_config_t spi_config = {
    .data_order = SPI_MSB,
    .mode = SPI_MODE0,
    .clockrate = SPI_CLOCK_DIV2
};

#endif /* SPI_CONFIG_H_ */