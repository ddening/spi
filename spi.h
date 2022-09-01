/*************************************************************************
* Title		: SPI Master Implementation
* Author	: Dimitri Dening
* Created	: 25.10.2021 17:56:35
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
*************************************************************************/

/**
@file spi.h
@author Dimitri Dening
@date 25.10.2021
@copyright (C) 2021 Dimitri Dening, MIT License
@brief Single Master implementation to communicate with spi devices using a ringbuffer.
@note Modify the <spi_io.h> port declaration if using another MCU.
@note Connect the error led (see <led_lib.h>) on the STK600 to monitor possible error codes.
      Occuring errors are described in <spi_error_handler.h>
@usage The following code shows typical usage of this library.

@code
    #include "spi.h"

    void callback_function (void){ // Do stuff in callback function }

    int main(void){

        uint8_t tx_data[] = { 0x83, 0x00, 0x00, 0x00 };

        uint8_t* rx_data = (uint8_t*)malloc(sizeof(uint8_t) * 4);

        slave_info slave_device = spi_create_slave(PINB4, PORTB4, DDB4);

        spi_init(SPI_MSB, SPI_MODE0, SPI_CLOCK_DIV4);

        spi_write(&slave_device, tx_data, 4, NORMAL, DEFAULT, &callback_function);

        spi_read(&slave_device, rx_data, 4, 0x00, NORMAL, &callback_function);

        free(rx_data);

        for(;;);
    }
@endcode
*/
#ifndef SPI_H_
#define SPI_H_

/* General libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* User defined libraries */
#include "spi_io.h"
#include "spi_buffer.h"
#include "spi_error_handler.h"
#include "../queue/queue.h"

/* Describes a spi device */
typedef struct device_t {
    uint8_t pin;
    uint8_t port;
    uint8_t ddr;
} device_t;

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

/**
 * @brief   Initializes the SPI as master.
 * @param   Check the definitions above for the entire valid argument list.
 * @return  Returns an SPI error code if an operation fails.
 *          Otherwise SPI_NO_ERROR (0) is returned.
 */
spi_error_t spi_init(data_order_t data_order, mode_t mode, clock_rate_t clock_rate);

device_t* spi_create_device(uint8_t pin, uint8_t port, uint8_t ddr);

spi_error_t spi_free_device(device_t*);

spi_error_t spi_write(payload_t*);

spi_error_t spi_read(payload_t*, uint8_t*);

spi_error_t spi_read_write(payload_t*, payload_t*, uint8_t*);

spi_error_t spi_flush(queue_t*);

#endif /* SPI_H_ */