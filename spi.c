/*************************************************************************
* Title     : SPI Master Implementation
* Author    : Dimitri Dening
* Created   : 25.10.2021 17:56:24
* Software  : Microchip Studio V7
* Hardware  : Atmega1284P
        
DESCRIPTION:
    Single master implementation to communicate with spi devices using a ringbuffer.  
USAGE:
    see <spi.h>
NOTES:
                       
*************************************************************************/

/* General libraries */
#include <avr/interrupt.h>

/* User defined libraries */
#include "spi.h"

#define SPI_ENABLE() (SPCR = (1 << SPE))
#define SPI_DISABLE() (SPCR &= ~(1 << SPE))
#define SPI_ISR_ENABLE() (SPCR = (1 << SPIE))
#define SPI_ISR_DISABLE() (SPCR &= ~(1 << SPIE))

typedef enum {
    SPI_ACTIVE,
    SPI_INACTIVE
} SPI_STATE_T;

static SPI_STATE_T SPI_STATE;

static queue_t q;

static queue_t* queue = NULL;

static payload_t* payload = NULL;

static device_t* device = NULL;

static uint8_t dump;
  
spi_error_t spi_init(spi_config_t* config){
        
    /* Set MOSI and SCK output, all others input */
    SPI_DDR = (1 << SPI_SCK) | (1 << SPI_MOSI);
    
    /* Make sure the MISO pin is input */
    SPI_DDR &= ~(1 << SPI_MISO);
       
    /* Enable SPI Interrupt Flag, SPI, Data Order, Master Mode, SPI Mode */	
    SPCR = (1 << SPIE) | (1 << SPE) | (config->data_order << DORD) | (1 << MSTR) | (config->mode << CPHA);
    
    /* Set Clock Rate */
    switch (config->clockrate){
        case SPI_CLOCK_DIV4:
        case SPI_CLOCK_DIV16:
        case SPI_CLOCK_DIV64:
        case SPI_CLOCK_DIV128:
            SPCR |= (config->clockrate << SPR0);
            SPSR &= ~(1 << SPI2X);
            break;
        case SPI_CLOCK_DIV2:
        case SPI_CLOCK_DIV8:
        case SPI_CLOCK_DIV32:
        case SPI_CLOCK_DIV64X:
            SPCR |= ((config->clockrate - 0x04) << SPR0);
            SPSR |= (1 << SPI2X);
            break;
     }
    
    SPI_STATE = SPI_INACTIVE;
    
    queue = queue_init(&q);

    // sei(); // global interrupt enable
    
    return SPI_NO_ERROR;
}

static spi_error_t spi_enable_device(device_t* _device){
    
    if (_device->port == device->port) return SPI_NO_ERROR;
    
    device = _device;
    
    /* Pin configuration for the new device */
    SPI_DDR  |= (1 << device->ddr);  // @Output
    SPI_PORT |= (1 << device->port); // Pull up := inactive
    
    /* Re-enable Master Mode again if it got reset by setting a device pin as input by accident. */
    if (!(SPCR & (1 << MSTR))) SPCR |= (1 << MSTR); 

    return SPI_NO_ERROR;
}

device_t* spi_create_device(uint8_t pin, uint8_t port, uint8_t ddr){
    
    if (port == SPI_SCK || port == SPI_MOSI || port == SPI_MISO) {
        return NULL;
    }
    
    device_t* device = (device_t*) malloc(sizeof(device_t));
    
    device->pin = pin;
    device->port = port;
    device->ddr = ddr;
    
    SPI_PORT |= (1 << port); // Pull up := inactive
    SPI_DDR  |= (1 << ddr);  // @Output
        
    return device;
}

spi_error_t spi_free_device(device_t* _device){
    
    free(_device);
    
    device = NULL;
    
    return SPI_NO_ERROR;
}

static spi_error_t _spi(void) {
       
    /* If the SPI is not active right now, it is save to transmit the next dataword from the queue. */
    if (SPI_STATE == SPI_INACTIVE) {
              
        payload = queue_dequeue(queue);
        
        if (payload->spi.device == NULL) {
            payload_free_spi(payload);
            return SPI_ERR_INVALID_PORT;
        }
        
        spi_enable_device(payload->spi.device);
        
        payload->spi.number_of_bytes--;
        
        SPI_STATE = SPI_ACTIVE;
              
        SPI_PORT &= ~(1 << device->port);  /* Pull down := active */
        
        SPDR = *(payload->spi.data);
    }
    
    return SPI_NO_ERROR;
}

spi_error_t spi_write(payload_t* _payload){
        
    spi_error_t err;
       
    _payload->spi.mode = WRITE;
    
    err = queue_enqueue(queue, _payload);
       
    if (err != SPI_NO_ERROR) return error_handler(SPI_ERR_BUFFER_OVERFLOW);
    
    err = _spi();
    
    if (err != SPI_NO_ERROR) return error_handler(err);
    
    return SPI_NO_ERROR;   
}

spi_error_t spi_read(payload_t* _payload, uint8_t* container){
    
    spi_error_t err;
    
    _payload->spi.mode = READ;
    _payload->spi.container = container;
    
    err = queue_enqueue(queue, _payload);
    
    if (err != SPI_NO_ERROR) return error_handler(SPI_ERR_BUFFER_OVERFLOW);
    
    err = _spi();
    
    if (err != SPI_NO_ERROR) return error_handler(err);
    
    return SPI_NO_ERROR;
}

spi_error_t spi_read_write(payload_t* payload_write, payload_t* payload_read, uint8_t* container) {
    
    spi_error_t err;
    
    payload_write->spi.mode = READ_WRITE;
    payload_read->spi.mode  = READ;
    payload_read->spi.container = container;
       
    err = queue_enqueue(queue, payload_write);
    err = queue_enqueue(queue, payload_read);
    
    if (err != SPI_NO_ERROR) return error_handler(SPI_ERR_BUFFER_OVERFLOW);
    
    err = _spi();
    
    if (err != SPI_NO_ERROR) return error_handler(err);
    
    return SPI_NO_ERROR;
}

spi_error_t spi_flush(queue_t* _queue){
    
    if (SPI_STATE == SPI_ACTIVE) return error_handler(SPI_ERR_FLUSH_FAILED);
        
    queue_flush(_queue);
    
    return SPI_NO_ERROR;
}

ISR(SPI_STC_vect){
                         
    if (payload->spi.container != NULL && payload->spi.mode == READ) {
        *(payload->spi.container) = SPDR;   
        (payload->spi.container)++;   
    } 
    else {
        dump = SPDR;
    }

    (payload->spi.data)++;       
 
    if (payload->spi.number_of_bytes != 0){
               
        payload->spi.number_of_bytes--;
        
        SPDR = *(payload->spi.data); 
    } 
    else {
        
        // Task finished
        
        if (payload->spi.callback != NULL) {
            payload->spi.callback(NULL);
            payload->spi.callback = NULL;
        }
              
        if (queue_empty(queue)) {   
            payload_free_spi(payload);                    
            SPI_PORT |= (1 << device->port); // Pull up := inactive   
            SPI_STATE = SPI_INACTIVE;      
        } 
        else {
            
            // Load next task
            
            if (payload->spi.mode == READ_WRITE) {
                // Do nothing.
            } 
            else {
                SPI_PORT |= (1 << device->port); // Pull up := inactive
            }
            
            payload_free_spi(payload);
                     
            payload = queue_dequeue(queue);
            
            spi_enable_device(payload->spi.device);
                       
            payload->spi.number_of_bytes--;           
            
            SPI_PORT &= ~(1 << device->port);  /* Pull down := active */
            
            SPDR = *(payload->spi.data);
        }
    }
}