#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2019-11-22 23:32:19

#include "Arduino.h"
#include "Arduino.h"
#include "variant.h"
#include <SPI.h>
#include <Artnet.h>
#include "hyperlight.h"
#define USE_WEBSERVER
#define USE_OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBold9pt7b.h>

void setup() ;
void loop() ;
void webserverLoop(void) ;
void updateDisplay() ;
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) ;

#include "hyperlight_dma_sloeber.ino"


#endif
