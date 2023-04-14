/*************************************************************************
* Title		: SPI Unit Test
* Author	: Dimitri Dening
* Created	: 13.02.2022 19:30:22
* Software	: Microchip Studio V7
* Hardware	: Atmega1284P
* Usage		: see Doxygen manual
*
* This is a modified version of Microchip Studios' SPI Unit Test.
* Following the license from Microchip used in the original file.
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products.
* It is your responsibility to comply with third party license terms applicable
* to your use of third party software (including open source software) that
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
*************************************************************************/

/**
@file main.c  
@author Dimitri Dening
@date 13.02.2022
@brief SPI Unit Test
@note Connect the flash memory (AT45DB041B) on the STK600 to the SPI PORT.
@note Connect two STK600s to run the entire unit test.       
*/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "test_spi.h"
#include "suite.h"
#include "spi.h"
#include "uart.h"
#include "led_lib.h"

/* Define CPU frequency in Hz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 10000000UL
#endif
	
static device_t* spi_device;

static uint8_t data_buffer_write[]	= { 0x84, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
static uint8_t data_flash_write[]	= { 0x83, 0x00, 0x00, 0x00 };
static uint8_t data_flash_read[]	= { 0xd2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t data_sent[]			= { 0x01, 0x02, 0x03, 0x04, 0x05 };
static uint8_t dummy[]              = { 0x00, 0x00, 0x00, 0x00, 0x00 };

/* CALLBACK FLAGS */
bool memory_return_success = 0;

/* CALLBACK FUNCTIONS */
void callback_memory_leak(void) { memory_return_success = 1; };
      
static int flash_read_data(device_t* device, uint8_t* container) {

	bool ret = 0;
			   
    /* Send the flash read command */
    payload_t* payload1 = payload_create_spi(PRIORITY_LOW, device, data_flash_read, ARRAY_LEN(data_flash_read), NULL);
    
    /* Get the data from flash */  
    payload_t* payload2 = payload_create_spi(PRIORITY_LOW, device, dummy, ARRAY_LEN(dummy), NULL);
    
    ret = spi_read_write(payload1, payload2, container);
    
    if (ret != 0) return TEST_ERROR;
    
	for (volatile uint16_t i = 0; i < 30000; i++) {}
			
	return 0;
}

static int run_spi_flash_read_test(const struct test_case* test) {
	
	bool ret = false;
	
	uint8_t* spi_receive = (uint8_t*)malloc(sizeof(uint8_t) * ARRAY_LEN(dummy));

	if (spi_receive == NULL) { return TEST_ERROR; }
	
	/* Read the data from Page 0 of flash. This is to avoid write of same
	 * data again and to reduce the flash write during testing */
	ret = flash_read_data(spi_device, spi_receive);
	if (ret != 0) { 
		free(spi_receive); 
		return ret; 
	}
	
	/* Check the read data */
	for (uint8_t i = 0; i < ARRAY_LEN(dummy); i++) {
		uart_put("%s %d", "[device 1]: read spi data", spi_receive[i]);
		/* Clear receive data buffer */
		spi_receive[i] = 0;
	}
	
	free(spi_receive);
	
	return TEST_PASS;
}

static int run_spi_transfer_test(const struct test_case* test) {
	
	bool write_enable = false;
	bool ret = false;
    
    payload_t* payload;
	
	uint8_t* spi_receive = (uint8_t*)malloc(sizeof(uint8_t) * ARRAY_LEN(dummy));

	if (spi_receive == NULL) { return TEST_ERROR; }
	
	/* Read the data from Page 0 of flash. This is to avoid write of same
	 * data again and to reduce the flash write during testing */
	ret = flash_read_data(spi_device, spi_receive);
	if (ret != 0) {
		free(spi_receive);
		return ret;
	}
	
	/* Check the read data */
	for (uint8_t i = 0; i < ARRAY_LEN(dummy); i++) {
		if (spi_receive[i] != data_sent[i]) {
			write_enable = true;
		}

		/* Clear receive data buffer */
		spi_receive[i] = 0;
	}
	
	/* If first time flash is getting used data will be 0xff. So write known data */
	if (write_enable) {

		uart_put("%s", "[device 1]: write mode");
		
		/* Send the buffer write command followed by data to be written */     
        payload = payload_create_spi(PRIORITY_LOW, spi_device, data_buffer_write, ARRAY_LEN(data_buffer_write), NULL);
        
        ret = spi_write(payload);
        
		if (ret != 0) {
			free(spi_receive);
			return TEST_ERROR;
		}

		/* Flash delay */
		for (volatile uint16_t i = 0; i < 30000; i++) {}

		/* Send the flash write command to write the previously sent data to flash */
        payload = payload_create_spi(PRIORITY_LOW, spi_device, data_flash_write, ARRAY_LEN(data_flash_write), NULL);
        
		ret = spi_write(payload);
         
		if (ret != 0) { 
			free(spi_receive);
			return TEST_ERROR;
		}

		/* Flash delay */
		for (volatile uint16_t i = 0; i < 30000; i++) {}
	}
	
	/* Read the data from flash. */
	ret = flash_read_data(spi_device, spi_receive);
	if (ret != 0) {
		free(spi_receive);
		return ret;
	}

	/* Check the read data */
	for (uint8_t i = 0; i < ARRAY_LEN(dummy); i++) {
		
		uart_put("%s %d %s %d", "[device 1]: read spi data", spi_receive[i], "expected", data_sent[i]);
		
		if (spi_receive[i] != data_sent[i]) {
			free(spi_receive);
			return TEST_FAIL;
		}
	}
	
	free(spi_receive);
	
	return TEST_PASS;
}
   
static int run_spi_memory_leak_test(const struct test_case* test) {
    
    bool ret = 0;
    
    int number_of_tasks = 30000; // <-- increase value to provoke possible memory leak
    
    uint8_t* container = (uint8_t*)malloc(sizeof(uint8_t) * ARRAY_LEN(dummy));
    
    if (container == NULL) {
        uart_put("%s", "Failed creating container!");
        return TEST_ERROR;
    }
    
    payload_t* payload;
    
    for (int i = 0; i < number_of_tasks; i++) {
              
        payload = payload_create_spi(PRIORITY_LOW, spi_device, dummy, ARRAY_LEN(dummy), &callback_memory_leak);
            
        if (payload == NULL) {
            free(container);
            return TEST_ERROR;
        }    
             
        ret = spi_read(payload, container);
        
        if (ret != 0) {
            uart_put("%s %i", "Failed at task: ", i);
            free(container);
            free(payload);
            return TEST_ERROR;
        }
        
        while(memory_return_success != 1);
        
        memory_return_success = 0;
        
    }
    
    free(container);
    
    return TEST_PASS;
}
     
int main(void) {
    
	cli();		
	
	led_init();
	
	uart_init();
	
	spi_init(&spi_config);
	
	sei();
	
	spi_device = spi_create_device(SPI_TEST_PORT, SPI_TEST_PORT, SPI_TEST_PORT);
    	
	DEFINE_TEST_CASE(data_flash_read_test, NULL, run_spi_flash_read_test, NULL, "SPI data flash read test");
	DEFINE_TEST_CASE(data_transfer_test, NULL, run_spi_transfer_test, NULL, "SPI data transfer test");
    DEFINE_TEST_CASE(memory_leak_test, NULL, run_spi_memory_leak_test, NULL, "SPI memory leak test");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(spi_tests) = {
		&data_flash_read_test,
		&data_transfer_test,
        &memory_leak_test
	};
    	
	/* Define the test suite */
	DEFINE_TEST_SUITE(spi_suite, spi_tests, "SPI driver test suite");
    
	/* Run all tests in the test suite */
	test_suite_run(&spi_suite);
    
	while (1) { /* Busy-wait forever. */ }
}