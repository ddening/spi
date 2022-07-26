# atmega-spi
 
 This SPI library is compatible with the ATmega1284P. It should be compatible with other microcontrollers from AVR by defining the ports in ```spi_io.h```  according to the datasheet. The implemenation is interrupt based and uses a buffered transmission/receiving state.
 
 ### Hardware
 The library was designed and tested using the ATmega1284P to transmit and receive data via an RFM95 LoRa Module at 8MHz and 10MHz. 
 
 #### Using other microcontrollers
 To use other microcontrollers the ```spi_io.h``` has to be extended according to the datasheet of the microcontroller.
 In this example the ports are defined for the ATmega1284P and ATmega16. Both share the same port layout.
 
 ```
 /* SPI Port Declaration */
#if defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega16__)
#	define SPI_SCK		PORTB7
#	define SPI_MOSI		PORTB5
#	define SPI_MISO		PORTB6
#	define SPI_SS		PORTB4
#	define SPI_PORT		PORTB
#	define SPI_DDR		DDRB
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "ATmega1284P or ATmega16 not found"
#  endif
#endif
```
