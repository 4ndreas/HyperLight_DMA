# Hyperlight LED Driver
STM32F407 based WS2811/12 Led driver with DMA output

## variant 
* added custom variant with DMA interrrupt function

## patched libraies are
* Artnet
* Ethernet
	* Changed SPI speed
	* Port so 2 with increases the buffer in the W5500 chip per port to 8k
* Adafruit_SSD1306
	* Display size to 128x64
	


