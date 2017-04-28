# prog89
 A FT232x programmer for the atmel AT89S and LP series of microcontrollers. 
 NOTE: This programmer requires your FTDI module to support MPSSE. It will
 not work with older or "low end" ftdi modules. Modules known to work:
 
 * FT232H
 * FT2232
 * FT4232

## Usage:
```bash

prog89 -[eDUk] opts

	-e           Run chip erase before programming
	-D           Disable auto-erase when writing
	-k [bytes]   Set size of chip memory (default 2048)
	
    -U memory:op:file:type
                 memory: flash|fuse|lock|signature
				 op: 
					 r: read
					 w: write
					 v: verify
				 file: filename or (when in immediate mode) raw data
				 mode: 
					 i: iHex
					 r: raw binary
					 m: immediate mode
					 h: hex (output only)
				 
				 Performs <op> on <memory> type using <file>'s 
				 data in <mode> mode
				 
				 Example:
				 ./prog89 -U flash:w:code.hex:i 
					 # Writes the data in iHex file "code.hex" to flash
				 ./prog89 -U flash:r:out.bin:r
					 # Reads data from flash into binary file "out.bin"
```

## Dependencies: 
 * libftdi
 * libmpsse (included)
 
## Building:
```
make
make install
```

## Wiring:
Connect your FTDI dongle's pins to the ISP pins on the AT89. This example 
uses an AT89LP213 and an FT232H.

| FTDI Pin | Name    | AT89 Pin | Name       |
| -------- | ------- | -------- | ---------- |
|       13 | SCK     |        2 | GPI7/SCK   |
|       14 | MOSI    |        1 | GPI5/MOSI  |
|       15 | MISO    |       14 | GPI6/MISO  |
|       16 | SS_n    |       13 | GPI4/SS_n  |
|       17 | RESET_N |        3 | GPI5/RST_n |
|      GND | GND     |      GND | GND        |

