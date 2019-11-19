#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2019-11-19 23:35:35

#include "Arduino.h"
#include "Arduino.h"
#include "variant.h"
#include <SPI.h>
#include <Artnet.h>

void setup() ;
void loop() ;
void SystemClock_Config(void) ;
void HAL_MspInit(void) ;
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base) ;
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim) ;
static void MX_DMA_Init(void) ;
static void MX_TIM8_Init(void) ;
static void MX_GPIO_Init(void) ;
void setAll(uint8_t red,uint8_t green ,uint8_t blue) ;
void setLED(int strip, int led_number, uint8_t red, uint8_t green ,uint8_t blue) ;
void setStripLED(int strip, uint8_t * data, int data_length, int start) ;
static void Wait_DMA(void) ;
static void Start_DMA(void) ;
static void Restart_DMA(void) ;
HAL_StatusTypeDef HAL_TIM_PWM_Set(TIM_HandleTypeDef *htim, uint32_t Channel) ;
static void Data_GPIO_Config(void) ;
static void TransferComplete(DMA_HandleTypeDef *DmaHandle) ;
static void TransferError(DMA_HandleTypeDef *DmaHandle) ;
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) ;

#include "hyperlight_dma_sloeber.ino"


#endif
