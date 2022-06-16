/*
 * hyperlight.h
 *
 *  Created on: 19.11.2019
 *      Author: ahoel
 */

#ifndef HYPERLIGHT_H_
#define HYPERLIGHT_H_

#include "Arduino.h"
#include "variant.h"



#define TIMx_DMA_ID  TIM_DMA_ID_CC1
#define TIMX_HANDLE  htim8
#define TIMX_DMA_HANDLE hdma_tim8_ch1

#define GPIOE_ODR               (GPIOE_BASE + 0x14)
#define DATA_GPIO_CLK_ENABLE     __HAL_RCC_GPIOE_CLK_ENABLE
#define DATA_GPIO_PORT           GPIOE
#define DATA_GPIO_PIN            GPIO_PIN_All

// WS2812 timings
//#define TIM_CLK 209		// counter period, time per bit
//#define TIM_TOO 38
//#define TIM_T1H 133
//#define TIM_T0H 50


// current version !
/*
#define TIM_CLK 209		// counter period, time per bit
#define TIM_TOO 39
#define TIM_T1H 132
#define TIM_T0H 55
*/

#define TIM_CLK 209		// counter period, time per bit
#define TIM_TOO 39
#define TIM_T1H 132
#define TIM_T0H 55


#define LED_LINES  16	// outputs
#define LED_LENGHT 680	// leds per output
#define LED_COLORS 3	// colors per led
#define DMA_DUMMY  2	// dummy due hardware limitations

#define FULL_FRAME_SIZE ( LED_LENGHT * LED_COLORS * 8 )+ DMA_DUMMY    /* max = 65530 (65535 - 5 dummy bits) due to dma limitation  */


enum colorMode
	{RGB,
	 RBG,
	 BRG,
	 BGR,
	 GRB,
	 GBR,
	 RGBW,
	 GRBW
	};


struct led_config{
	int offset = 0;
	colorMode color = GRB;
	int useGamma = 1;
};

struct sConfig{
	led_config ledConfig[LED_LINES];
	int status = 0;
	int valid = 0;
	int enOffsetLEDs = 1;
};


class hyperlight {
public:
	hyperlight();

	void begin(void);
	void show(void);
	bool isDMAIdle(void);

	void setAll(uint8_t red,uint8_t green ,uint8_t blue) ;
	void setLED(int strip, int led_number, uint8_t red, uint8_t green ,uint8_t blue) ;
	void setLED(int strip, int led_number, uint8_t red, uint8_t green ,uint8_t blue, uint8_t white);
	void setStripLED(int strip, uint8_t * data, int data_length, int start, colorMode color=RGB) ;

	void setSnakeLED(int strip, uint8_t * data, int data_length, int ledOffset, uint16_t snakeLength) ;
	void useGamma(int mode);

	uint32_t getUpdateTime( int strip);

	void setOffset(int strip, int offset);
	void setOffsetColor(int strip, uint8_t red, uint8_t green ,uint8_t blue);
private:


	void Wait_DMA(void);
	void Start_DMA(void);
	void Restart_DMA(void);

	uint16_t DMXIdToPixelId(uint16_t u, uint16_t dmxId, uint16_t snakeLength);
	uint32_t updatetime[16];
	uint32_t statusLedOffsets[16];
	void setbyteToBuffer(uint8_t val);

	int _useGamma;
	uint32_t TransferCounter = 0;

};


/* To Do for better Ethernet performance change SPI Settings in
 * C:\Program Files (x86)\Arduino\libraries
 *     ../Ethernet/src/utility/w5100.h
 *
 * to
 *
 * #define SPI_ETHERNET_SETTINGS SPISettings(42000000, MSBFIRST, SPI_MODE0)
 *
 */


#endif /* HYPERLIGHT_H_ */
