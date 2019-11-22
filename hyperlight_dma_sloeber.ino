#include "Arduino.h"
#include "variant.h"
#include <SPI.h>
#include <Artnet.h>
#include "hyperlight.h"

#define USE_WEBSERVER
#define USE_OLED

#ifdef USE_OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBold9pt7b.h>

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

#endif

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

byte ip[] = {192, 168, 2, 80};
byte mac[] = {0x04, 0xE9, 0xE5, 0x80, 0x69, 0xEC};


// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
#ifdef USE_WEBSERVER
EthernetServer server(80);
#endif

IPAddress remoteArd;

void setup() {

  leds.begin();
  leds.setAll(0,0,50);

  pinMode(ETH_RST_PIN, OUTPUT);
  digitalWrite(ETH_RST_PIN, HIGH);

  delay(100);

  SPI.setMISO(ETH_MISO_PIN);
  SPI.setMOSI(ETH_MOSI_PIN);
  SPI.setSCLK(ETH_SCK_PIN);
  Ethernet.init(ETH_SCS_PIN);

  artnet.begin(mac, ip);
  artnet.setArtDmxCallback(onDmxFrame);

#ifdef USE_WEBSERVER
  server.begin();
#endif

#ifdef USE_OLED
  Wire.setSDA(PB7);
  Wire.setSCL(PB6);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  display.clearDisplay();
  updateDisplay();
#endif
}

uint32_t last_update = 0;
uint32_t currentTime = 0;

void loop() {
	currentTime = millis();

	artnet.read();


	if (currentTime - last_update > 20)	// 50 fps
	{
		last_update = currentTime;

		// set status led
		for (int strip = 0; strip < 16; strip++) {
			int lastUpdate = currentTime - leds.getUpdateTime(strip);

			if (lastUpdate < 1000)
			{
				leds.setLED(strip,0,0,128,0);
			}
			else
			{
				leds.setLED(strip,0,128,0,0);
			}
		}
		//update leds leds
		leds.show();
	}

#ifdef USE_WEBSERVER
	webserverLoop();
#endif
#ifdef USE_OLED
    updateDisplay();
#endif
}


//EthernetClient client;
#ifdef USE_WEBSERVER
void webserverLoop(void)
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
	// an http request ends with a blank line
	boolean currentLineIsBlank = true;
	while (client.connected()) {
	  if (client.available()) {
		char c = client.read();
		// if you've gotten to the end of the line (received a newline
		// character) and the line is blank, the http request has ended,
		// so you can send a reply
		if (c == '\n' && currentLineIsBlank) {
		  // send a standard http response header
		  client.println("HTTP/1.1 200 OK");
		  client.println("Content-Type: text/html");
		  client.println("Connection: close");  // the connection will be closed after completion of the response
		  client.println("Refresh: 10");  // refresh the page automatically every 5 sec
		  client.println();
		  client.println("<!DOCTYPE HTML>");
		  client.println("<html>");

		  // output the value of each analog input pin
		  client.print("<h1>HyperLight Info </h1>");

		  client.print("My IP: ");
		  client.print(ip[0], DEC);client.print(".");
		  client.print(ip[1], DEC);client.print(".");
		  client.print(ip[2], DEC);client.print(".");
		  client.print(ip[3], DEC);

		  client.println("<br />");
		  client.print("Host: ");
		  client.print(remoteArd[0]);client.print(".");
		  client.print(remoteArd[1]);client.print(".");
		  client.print(remoteArd[2]);client.print(".");
		  client.print(remoteArd[3]);
		  client.println("<br />");
		  client.println("<br />");
		  client.println("<table border=\"1\">");

		  for (int strip = 0; strip < 16; strip++)
		  {
			client.println("<tr>");
			int lastUpdate = currentTime - leds.getUpdateTime(strip);
			client.println("<td>");
			client.print("Universe ");
			client.print(strip);
			client.println("</td>");
			client.println("<td ALIGN=\"CENTER\">");
			if (lastUpdate < 1000)
			{
				client.print("<font color=\"green\"> OK </font>");
			}
			else
			{
				client.print("<font color=\"red\"> error </font>");
			}
			client.println("</td>");
			client.println("<td>");

			client.print("updated ");
			client.print(lastUpdate);
			client.print(" ms ago ");

			client.println("</li>");
			client.println("</td>");
			client.println("</tr>");
		  }


		  client.println("</table>");
		  client.println("</html>");
		  break;
		}

		if (c == '\n') {
		  // you're starting a new line
		  currentLineIsBlank = true;
		} else if (c != '\r') {
		  // you've gotten a character on the current line
		  currentLineIsBlank = false;
		}
	  }
	}
	// give the web browser time to receive the data
	// close the connection:
	client.stop();
  }
}
#endif

#ifdef USE_OLED


void updateDisplay()
{
  static uint32_t lastDisplayUpdate = 0;
  static uint32_t lastDisplayStatus = 0;
  uint32_t displayStatus = 0;

  if(( currentTime - lastDisplayUpdate) > 1000){
	  lastDisplayUpdate = currentTime;

	  display.clearDisplay();

	  display.setCursor(0,6);
	  display.setTextColor(WHITE);
	  display.setFont(&FreeSansBold9pt7b);

	  display.print(ip[0], DEC);display.print(".");
	  display.print(ip[1], DEC);display.print(".");
	  display.print(ip[2], DEC);display.print(".");
	  display.print(ip[3], DEC);display.println("");

	  display.setFont();

	  display.setCursor(0,32);
	  display.println("|-E1-|-E2-|-E3-|-E4-|");
	  display.setCursor(0,48);

	  for (int strip = 0; strip < 16; strip++) {
		if((strip%4) == 0)
		{
			display.print("|");
		}
		int lastUpdate = currentTime - leds.getUpdateTime(strip);
		if (lastUpdate < 1000)
		{
			display.print("O");
			displayStatus |= (1<<strip);
		}
		else
		{
			display.print("X");
		}
	  }
	  display.print("|");

	  display.setCursor(0,16);
	  display.print("Host:");

	  if(displayStatus == 0)
	  {
		  display.print("*.*.*.*");
	  }
	  else
	  {
		  display.print(remoteArd[0]);display.print(".");
		  display.print(remoteArd[1]);display.print(".");
		  display.print(remoteArd[2]);display.print(".");
		  display.print(remoteArd[3]);
	  }

	  if((displayStatus != lastDisplayStatus) || (displayStatus == 0))
	  {
		  // only update if something changed
		  // update causes a flicker
		  lastDisplayStatus = displayStatus;
		  display.display();
	  }
  }
}
#endif


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
	  leds.setStripLED(universe, data, length, 1);
  }
}





