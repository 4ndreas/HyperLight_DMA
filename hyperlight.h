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

TIM_HandleTypeDef htim8;
DMA_HandleTypeDef hdma_tim8_ch1;

#define TIMx_DMA_ID  TIM_DMA_ID_CC1
#define TIMX_HANDLE  htim8
#define TIMX_DMA_HANDLE hdma_tim8_ch1

#define GPIOE_ODR               (GPIOE_BASE + 0x14)
#define DATA_GPIO_CLK_ENABLE     __HAL_RCC_GPIOE_CLK_ENABLE
#define DATA_GPIO_PORT           GPIOE
#define DATA_GPIO_PIN            GPIO_PIN_All

#define TIM_CLK 209
#define TIM_TOO 35
#define TIM_T1H 0x85
#define TIM_T0H 0x32

#define LED_LINES 16
#define LED_LENGHT 240
#define LED_COLORS 3
#define DMA_DUMMY 2

#define FULL_FRAME_SIZE ( LED_LENGHT * LED_COLORS * 8 )+ DMA_DUMMY    /* max = 65530 (65535 - 5 dummy bits) due to dma limitation  */

static uint16_t frame_buffer[FULL_FRAME_SIZE];

uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


uint32_t TIMX_CCMR1 = 0;
uint32_t TIMX_CCMR2 = 0;
uint32_t TransferCounter = 1;
int TransferLock = 0;



class hyperlight {
public:
	hyperlight();

	void begin(void);
	void show(void);
	void setAll(uint8_t red,uint8_t green ,uint8_t blue) ;
	void setLED(int strip, int led_number, uint8_t red, uint8_t green ,uint8_t blue) ;
	void setStripLED(int strip, uint8_t * data, int data_length, int start) ;

private:

	void Wait_DMA(void);
	void Start_DMA(void);
	void Restart_DMA(void);

};


static void TransferComplete(DMA_HandleTypeDef *DmaHandle);
static void TransferError(DMA_HandleTypeDef *DmaHandle);


#endif /* HYPERLIGHT_H_ */
