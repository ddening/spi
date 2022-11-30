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
	
	const uint8_t flash_send[] = { 0x04, 0x01, 0x02 };

    int main(void){
		
		sei();
		
		spi_init();
    
		device_t* spi_device = spi_create_device(PINB4, PORTB4, DDB4);
    
		uint8_t* container = (uint8_t*)malloc(sizeof(uint8_t) * ARRAY_LEN(flash_send)); 
    
		payload_t* payload1 = payload_create_spi(PRIORITY_LOW, spi_device, flash_send, ARRAY_LEN(flash_send), NULL);
		payload_t* payload2 = payload_create_spi(PRIORITY_LOW, spi_device, flash_send, ARRAY_LEN(flash_send), NULL);
      
		spi_error_t err = spi_read_write(payload1, payload2, container);
 
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
#include "spi_config.h"
#include "spi_buffer.h"
#include "spi_error_handler.h"
#include "../queue/queue.h"

/* Describes a spi device */
typedef struct device_t {
    uint8_t pin;
    uint8_t port;
    uint8_t ddr;
} device_t;

/**
 * @brief   Initializes the SPI as master.
 * @param   Check the definitions above for the entire valid argument list.
 * @return  Returns an SPI error code if an operation fails.
 *          Otherwise SPI_NO_ERROR (0) is returned.
 */
spi_error_t spi_init();

device_t* spi_create_device(uint8_t pin, uint8_t port, uint8_t ddr);

spi_error_t spi_free_device(device_t*);

spi_error_t spi_write(payload_t*);

spi_error_t spi_read(payload_t*, uint8_t*);

spi_error_t spi_read_write(payload_t*, payload_t*, uint8_t*);

spi_error_t spi_flush(queue_t*);

#endif /* SPI_H_ */