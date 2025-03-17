# SPI Library for AVR Microcontrollers

*A high-performance SPI (Serial Peripheral Interface) driver for AVR microcontrollers.*

## Overview
This library provides an efficient SPI communication interface for AVR microcontrollers, supporting master configuration. It is designed for fast and reliable data transmission with minimal CPU overhead.

## Features
- Supports Master mode
- Configurable clock speed, polarity, and phase
- Interrupt-driven operation
- Multi-device support using Chip Select (CS)
- Compatible with various AVR microcontrollers

## Dependencies
This library may require additional AVR utilities. Check out the related repository:
[libAVR - Required Dependencies](https://github.com/ddening/libAVR)

## Installation
Clone this repository and include the necessary files in your project:
```sh
$ git clone https://github.com/ddening/spi.git
```
Include the header in your code:
```c
#include "spi.h"
```

 ## Usage with other microcontrollers
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

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
