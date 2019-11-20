#include "Arduino.h"
#include "variant.h"
#include <SPI.h>
#include <Artnet.h>
#include "hyperlight.h"


// W5500 Ethernet
// HW-SPI-3 or HW_SPI-1
#define ETH_MOSI_PIN  PB5
#define ETH_MISO_PIN  PB4
#define ETH_SCK_PIN   PB3
#define ETH_SCS_PIN   PA15
#define ETH_INT_PIN   PB8
#define ETH_RST_PIN   PD3

Artnet artnet;
hyperlight leds;

byte ip[] = {192, 168, 2, 220};
byte mac[] = {0x04, 0xE9, 0xE5, 0x13, 0x69, 0xEC};

IPAddress remoteArd;

void setup() {

  leds.begin();

  delay(100);

  SPI.setMISO(ETH_MISO_PIN);
  SPI.setMOSI(ETH_MOSI_PIN);
  SPI.setSCLK(ETH_SCK_PIN);
  Ethernet.init(ETH_SCS_PIN);

  artnet.begin(mac, ip);
  artnet.setArtDmxCallback(onDmxFrame);

}

uint32_t last_update = 0;

void loop() {
	uint32_t currentTime = millis();

	artnet.read();

	if (currentTime - last_update > 30)
	{
		last_update = currentTime;
		leds.show();
	}
}


//void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
//{
//  remoteArd = artnet.remote;
//
//  if(universe < 16)
//  {
//	  int led = 0;
//	  int i;
//	  for(i = 0; i< length; i+=3){
//		  if(led < LED_LENGHT){
//			  setLED(universe, led,  gamma8[data[i]], gamma8[data[i+1]],gamma8[data[i+2]]);
//		  }
//		  led++;
//	  }
//  }
//}


void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  remoteArd = artnet.remote;

  if(universe < 16)
  {
	  leds.setStripLED(universe, data, length, 0);
  }
}





