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
	


## ToDo

* Improvement for
    * better bit sorting
    * setLED function
    * setSnakeLED

* Webserver Config for
    * Color Mode
    * offset 
    * Snake Wireing
    * FPS Counter

* USB test
* SD-Card 
* better Webserver with SD card or the website.h methode
* DMX out



## How to Install


```
https://github.com/stm32duino/BoardManagerFiles/raw/master/STM32/package_stm_index.json
```

![alt text][preferences1]
![alt text][preferences2]



### Adding a new board variant 

Path to the boards:

coppy the variants\Hyperlight folder in the package dir:

```
\Sloeber\arduinoPlugin\packages\STM32\hardware\stm32\1.7.0\variants\Hyperlight
```
or if the plain arduino IDE is used in  
```
.user\AppData\Local\Arduino15\packages\STM32\hardware\stm32\1.7.0\variants
```

Setting for the board:
![alt text][properties]

### How to Debug
See the wiki form the STM32 Core
https://github.com/stm32duino/wiki/wiki/How-to-debug

Download sloeber-IDE form 
http://eclipse.baeyens.it/index.shtml


Change Target to:
```
-s "${openocd_path}/../scripts" -f interface/stlink-v2-1.cfg -f target/stm32f4x.cfg
```
![alt text][debug1]
![alt text][debug2]

[preferences1]: img/preferences_1.JPG "Settings URL for STM32Core"
[preferences2]: img/preferences_2.JPG "Settings URL for STM32Core"
[properties]: img/properties.JPG "properties of the arduino ide"
[debug1]: img/debug_1.JPG "Settings URL for OCD Debugging"
[debug2]: img/debug_2.JPG "Settings URL for OCD Debugging"
