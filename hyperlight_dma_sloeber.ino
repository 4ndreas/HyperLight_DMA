#define ARCH_STM32

#include "Arduino.h"
#include "variant.h"
#include "pin_config.h"
#include <SPI.h>
#include <SPIMemory.h>
#include <IWatchdog.h>
#include <FlashStorage_STM32.h>
#include <Math.h>
#include "BufferedArtnet.h"
#include "hyperlight.h"
#include "F4DMXSerial.h"
#include "helper.h"


//#define USE_WEBSERVER
#define USE_OLED
#define USE_BUTTONS
#define USE_DMX
//#define USE_LCD
//#define USE_FLASH
#define USE_EEPROM
//#define USE_WATCHDOG


#define IP_ENDING 22


#ifdef USE_DMX
// To do
#define DMX_UNI 65
F4DMXSerial dmxOUT;
#endif

#ifdef USE_BUTTONS
// Button1
// Button2
#endif

#ifdef USE_OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBold9pt7b.h>
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);
#endif


#ifdef USE_LCD
#include <wire.h>
//#include <liquidcrystal_i2c.h>
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
#endif


#ifdef USE_WEBSERVER
#include "website.h"

// Initialize the Ethernet server library
EthernetServer server(80);
#endif

struct sConfig sysConfig;

#ifdef USE_FLASH
	SPIFlash flash(SPI_FLASH_CS);
#endif




#define CONFIG_UNI 66
BufferedArtnet<67> artnet;
//BufferedArtnet<33> artnet;
hyperlight leds;

#define LED_OFFSET 1


byte ip[] = {10, 39, 0, IP_ENDING};
#define CONCAT(m, n) m ## n
#define HEXIFY(x) CONCAT(0x, x)
byte mac[] = {0x04, 0xE9, 0xE5, HEXIFY(IP_ENDING), 0x69, 0xEC};

// sets artnet timeout in ms (only for show)
#define INACTIVITY_TIMEOUT 1000
IPAddress remoteArd;

void setup() {

#ifdef USE_WATCHDOGUSE_WATCHDOG
  if (IWatchdog.isReset(true)) {
		  // to do monitor resets by watchdog
  }

  IWatchdog.begin(4000000);
#endif

  // Hardware Serial1 for debug out
  Serial.begin(115200);
  Serial.println("init");

  Serial.println("My IP:");
  Serial.print(ip[0], DEC);Serial.print(".");
  Serial.print(ip[1], DEC);Serial.print(".");
  Serial.print(ip[2], DEC);Serial.print(".");
  Serial.print(ip[3], DEC);Serial.println("");

  leds.begin();



#ifdef USE_WEBSERVER
  server.begin();
#endif

#ifdef USE_OLED

  Wire.setSDA(I2C1_SDA_PIN);
  Wire.setSCL(I2C1_SCL_PIN);


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  display.clearDisplay();
  updateDisplay();
#endif


#ifdef USE_LCD

  Wire.setSDA(I2C1_SDA_PIN);
  Wire.setSCL(I2C1_SCL_PIN);
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  lcd.clear();


  lcd.setCursor(0, 0);
  lcd.print("Hyperlight");
  lcd.setCursor(0, 1);
  lcd.print("init");
  lcd.display();

#endif


#ifdef USE_DMX
  dmxOUT.begin();
  delay(1);
  dmxOUT.Send_Packet();
#endif

#ifdef USE_FLASH
  flash.begin();
  readConfig();
#endif

#ifdef USE_EEPROM
  //Serial.print(F("\nStart FlashStoreAndRetrieve on ")); Serial.println(BOARD_NAME);
  Serial.println(FLASH_STORAGE_STM32_VERSION);

  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());

  readConfig();

#else
  defaultConfig();
#endif


  pinMode(ETH_RST_PIN, OUTPUT);

  digitalWrite(ETH_RST_PIN, HIGH);
  delay(100);
  digitalWrite(ETH_RST_PIN, LOW);
  delay(100);
  digitalWrite(ETH_RST_PIN, HIGH);
  delay(100);

  SPI.setMISO(ETH_MISO_PIN);
  SPI.setMOSI(ETH_MOSI_PIN);
  SPI.setSCLK(ETH_SCK_PIN);
  Ethernet.init(ETH_SCS_PIN);
  Ethernet.begin(mac, ip);

  artnet.begin();



}

uint32_t last_update = 0;
uint32_t last_artnet_update = 0;
uint32_t last_dmx_update = 0;
uint32_t currentTime = 0;
unsigned long update_time = 0;

bool new_data = false;
bool new_dmx_data = false;
unsigned frames = 0;

void loop() {
	currentTime = millis();

#ifdef USE_DMX
	if(((currentTime - last_dmx_update) > (1000 / 30)) && new_dmx_data) // 30 fps dmx update
	{
		last_dmx_update = currentTime;
		dmxOUT.Send_Packet();
		new_dmx_data = false;
	}
#endif

	while(!(artnet.syncMode() && artnet.syncPending()) && artnet.read()) {
		new_data = true;
		last_artnet_update = currentTime;
	}

	if ((!artnet.syncMode() && new_data && ((currentTime - last_artnet_update) > 5 )) // heuristic update
	 || (artnet.syncMode() && artnet.syncPending())	)	// synchronized update
	{

		// set status led
		//if (sysConfig.enOffsetLEDs > 0 ){
		if (1 > 0 ){
			for (int strip = 0; strip < 16; strip++) {
				uint32_t lastUpdate = currentTime - leds.getUpdateTime(strip);

				if (lastUpdate < INACTIVITY_TIMEOUT)
				{
					leds.setOffsetColor(strip,0,16,0);
				}
				else
				{
					leds.setOffsetColor(strip,16,0,0);
				}
			}
		}

		if(leds.isDMAIdle()){

			unsigned long conv_start = micros();
			for(unsigned c = 0; c < 16; c++) {
				//leds.setStripLED(c, artnet.getUniverseData(c*2), 510, 0, GRB);
				//leds.setStripLED(c, artnet.getUniverseData((c*2) + 1), 510, 170, GRB);

				// sysConfig.ledConfig[c].color

				if ( artnet.newUniverseData(c) > 0 )
					leds.setStripLED(c, artnet.getUniverseData(c), 510, 0, sysConfig.ledConfig[c].color);
				if ( artnet.newUniverseData(c + 16) > 0 )
					leds.setStripLED(c, artnet.getUniverseData((c) + 16), 510, 170, sysConfig.ledConfig[c].color);
				if ( artnet.newUniverseData(c + 32) > 0 )
					leds.setStripLED(c, artnet.getUniverseData((c) + 32), 510, 340, sysConfig.ledConfig[c].color);
				if ( artnet.newUniverseData(c + 48) > 0 )
					leds.setStripLED(c, artnet.getUniverseData((c) + 48), 510, 510, sysConfig.ledConfig[c].color);

#ifdef USE_DMX
				// To do
				if ( artnet.newUniverseData(DMX_UNI) > 0 )
				{
					uint8_t * data = artnet.getUniverseData(DMX_UNI);
					dmxOUT.write_block(1, data, 512);
					new_dmx_data = true;
				}
#endif
				if ( artnet.newUniverseData(CONFIG_UNI) > 0 )
				{
					// config universe
					uint8_t * data = artnet.getUniverseData(CONFIG_UNI);

					uint8_t magic = *data;
					uint8_t cmd = *(data +1);
					uint8_t channel = *(data +2);
					uint8_t offset = *(data +3);
					uint8_t color = *(data +4);

					//Serial.print(ip[3], DEC);Serial.println("");
					Serial.println("\r\ncondig packet");
					Serial.print("magic:");Serial.println(magic);
					Serial.print("cmd:");Serial.println(cmd);
					Serial.print("channel:");Serial.println(channel);
					Serial.print("offset:");Serial.println(offset);
					Serial.print("color:");Serial.println(color);

					if(magic == 42){
						if (cmd == 1)
						{
							// set offset and color
							if( channel < 16)
							{
								sysConfig.ledConfig[channel].offset = offset;
								sysConfig.ledConfig[channel].color = (colorMode)color;

								// set offset to hardware
								leds.setOffset(channel,offset);
							}
						}
						else if(cmd == 2)
						{
							// save config
							saveConfig();
						}
						else if(cmd == 3)
						{
							// default config
							defaultConfig();
						}
						else if(cmd == 4)
						{
							// read config config
							readConfig();
						}
						else if(cmd == 5)
						{
							// enable status LEDs
							Serial.println("set status LEDs\r\n");
							sysConfig.enOffsetLEDs = channel;
						}
						else if(cmd == 66)
						{
							// reset device
							Serial.println("reset Device\r\n");
							IWatchdog.begin(1000000);
							while(1)
							{
								;
							}
						}
					}
				}
			}

			long unsigned conv_time = micros() - conv_start;
			if(conv_time > update_time) {
				update_time = conv_time;
				conv_start = micros();
			}

			leds.show();

			new_data = false;

			frames++;

			if(frames % 10 == 0)
				update_time = 0;


			artnet.clearSyncPending();
		}

/*		if (currentTime - last_update > 30)	// 50 fps
		{
			last_update = currentTime;
		}*/
	}


#ifdef USE_WEBSERVER
	webserverLoop();
#endif
#ifdef USE_OLED
    updateDisplay();
#endif

#ifdef USE_LCD
  updateDisplay();
#endif

#ifdef USE_WATCHDOGUSE_WATCHDOG
  IWatchdog.reload();
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
	bool currentLineIsBlank = true;
	while (client.connected()) {
	  if (client.available()) {
		char c = client.read();

		if (c == '\n' && currentLineIsBlank) {

//			 send a standard http response header
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
			if (lastUpdate < INACTIVITY_TIMEOUT)
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

//            req_index = 0;
//            StrClear(HTTP_req, REQ_BUF_SZ);
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
		if (lastUpdate < INACTIVITY_TIMEOUT)
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
		  display.print(artnet.remote[0]);display.print(".");
		  display.print(artnet.remote[1]);display.print(".");
		  display.print(artnet.remote[2]);display.print(".");
		  display.print(artnet.remote[3]);
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


#ifdef USE_LCD
void updateDisplay()
{
  static uint32_t lastDisplayUpdate = 0;
  //static uint32_t lastDisplayStatus = 0;
  //uint32_t displayStatus = 0;

  if(( currentTime - lastDisplayUpdate) > 2000){
	  lastDisplayUpdate = currentTime;

	  lcd.setCursor(0, 0);
	  lcd.print(ip[0], DEC);lcd.print(".");
	  lcd.print(ip[1], DEC);lcd.print(".");
	  lcd.print(ip[2], DEC);lcd.print(".");
	  lcd.print(ip[3], DEC);lcd.print("");
	  lcd.print("  ");

	  lcd.setCursor(0, 1);
	  for (int strip = 0; strip < 16; strip++)
	  {
		int lastUpdate = currentTime - leds.getUpdateTime(strip);

		if (lastUpdate < INACTIVITY_TIMEOUT)
		{
			lcd.print("O");
		}
		else
		{
			lcd.print("X");
		}
	  }

	  lcd.display();
  }
}

#endif


#ifdef USE_FLASH

#define FLASH_ADDR 1

//void readFlash()
//{
//	flash.readAnything(FLASH_ADDR, sysConfig);
//
//	if(sysConfig.valid != 42)
//	{
//		Serial.println("\r\nno valid flash config!");
//		defaultConfig();
//	}
//
//	for ( int i = 0; i < LED_LINES; i++)
//	{
//	    leds.setOffset(i,sysConfig.ledConfig[i].offset);
//	}
//}
#endif






uint16_t configAddress = 0;

void readConfig()
{
	//flash.readAnything(FLASH_ADDR, sysConfig);
#ifdef USE_EEPROM
	EEPROM.get(configAddress, sysConfig);
#endif
#ifdef USE_FLASH
	flash.readAnything(FLASH_ADDR, sysConfig);
#endif

	if(sysConfig.valid != 42)
	{
		Serial.println("\r\nno valid flash config!");
		defaultConfig();
	}
	else
	{
		Serial.println("valid flash config");
	}

	for ( int i = 0; i < LED_LINES; i++)
	{
	    leds.setOffset(i,sysConfig.ledConfig[i].offset);
	}
}


void saveConfig()
{
#ifdef USE_EEPROM
	//flash.eraseSection(FLASH_ADDR, sizeof(sysConfig));
	//flash.writeAnything(FLASH_ADDR, sysConfig);
	Serial.println("save config!");
	EEPROM.put(configAddress, sysConfig);
	EEPROM.commit();
#endif
#ifdef USE_FLASH
	flash.eraseSection(FLASH_ADDR, sizeof(sysConfig));
	flash.writeAnything(FLASH_ADDR, sysConfig);
#endif
}


void defaultConfig()
{
	Serial.println("setup default config!");

	struct sConfig dConfig;
	sysConfig = dConfig;
	sysConfig.valid = 42;
	saveConfig();
}



/*void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  remoteArd = artnet.remote;
  // 8x - One ArtNet universe per Output 170 LEDs
  if (length > 510)
  {
	  length = 510;
  }
  if(universe < 16)
  {
	  leds.setStripLED(universe, data, length, 0, GRB);
	  //leds.setStripLED(universe, data, length, 1);
  }
  else if(universe < 32)
  {
	  leds.setStripLED(universe-16, data, length, 170, GRB);
  }
}*/




/*
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  remoteArd = artnet.remote;
  // 8x - One ArtNet universe per Output 170 LEDs
  if (length > 510)
  {
	  length = 510;
  }
  if(universe < 16)
  {
	  leds.setStripLED(universe, data, length, 0, RBG);
	  //leds.setStripLED(universe, data, length, 1);
  }
  else if(universe < 32)
  {
	  leds.setStripLED(universe-16, data, length, 170, RBG);
  }
}

*/

/*
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  remoteArd = artnet.remote;
  // 8x - One ArtNet universe per Output (Maiskolben)
  if(universe < 8)
  {
	  leds.setStripLED(universe, data, length, 1);
  }
  // Ping Pong Panel 1 16x16 RGB leds
  else if(universe == 8)
  {
	  leds.setSnakeLED(8, data, length, 0, 16);	// 170 leds
  }
  else if(universe == 9)
  {
	  leds.setSnakeLED(8, data, length, 170, 16);	// 86 leds
  }
  // Ping Pong Panel 2 16x16 RGB leds
  else if(universe == 10)
  {
	  leds.setSnakeLED(9, data, length, 0, 16);	// 170 leds
  }
  else if(universe == 11)
  {
	  leds.setSnakeLED(9, data, length, 170, 16);	// 86 leds
  }
  // RGBW panel 17x17 RGBW leds
  else if(universe == 12)
  {
	  for(int i = 0; i < length/3; i++)
	  {
		  leds.setLED(10, i, data[i*3], data[i*3+1], data[i*3+2] , 0);
	  }
  }
  else if(universe == 13)
  {
	  for(int i = 0; i < length/3; i++)
	  {
		  leds.setLED(10, 170 + i, data[i*3], data[i*3+1], data[i*3+2] , 0);
	  }
  }
  // Rotating Wheel
  else if(universe == 14)
  {
	  leds.setStripLED(11, data, length, 1,RBG);
  }
  else if(universe == 15)
  {
	  leds.setStripLED(11, data, length, 170,RBG);
  }

  else if(universe == 256+0)
  {
	  leds.setStripLED(12, data, length, 1,RBG);
  }
  else if(universe == 256+1)
  {
	  leds.setStripLED(13, data, length, 1,RBG);
  }
  else if(universe == 256+2)
  {
	  leds.setStripLED(14, data, length, 1,RBG);
  }
  else if(universe == 256+3)
  {
	  leds.setStripLED(15, data, length, 1,RBG);
  }
}
*/





