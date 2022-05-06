#include "F4DMXSerial.h"
#include <cassert>

static void MX_DMA1_Init(void);
static void MX_USART2_UART_Init(void);


UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

F4DMXSerial::F4DMXSerial()
{
	dma_buffer_a[0] = 0;
	dma_buffer_b[0] = 0;
	dma_front_buffer = dma_buffer_a;
	dma_back_buffer = dma_buffer_b;
}

void F4DMXSerial::begin()
{
	MX_USART2_UART_Init();
	MX_DMA1_Init();

	pinMode(UART2_RE_PIN, OUTPUT);
	pinMode(UART2_DE_PIN, OUTPUT);
	setDataDir(0);
}


void F4DMXSerial::write(unsigned channel, uint8_t value)
{
  // channel starts at 1 first byte in to send always 0
  // adjust parameters
  if (channel < 1) channel = 1;
  if (channel > DMXSERIAL_MAX) channel = DMXSERIAL_MAX;

  dma_back_buffer[channel] = value;
}

void F4DMXSerial::write_block(unsigned first_channel, uint8_t* values, size_t len)
{
  // channel starts at 1 first byte in to send always 0
  // adjust parameters
  if (first_channel < 1)
	  return;
  if ((first_channel + len - 1) > DMXSERIAL_MAX)
	  return;

  memcpy(dma_back_buffer + first_channel, values, len);
}


// set Data direction of the RS485 transiver
// dir 0 -> TX
// dir 1 -> RX
void F4DMXSerial::setDataDir(uint8_t dir)
{
	// TO MAX481 DE - data direction
	// TO MAX481 RE - receive nEnable
	if (dir == 1)
	{	// set to RX mode
		digitalWrite(UART2_DE_PIN, LOW);
		digitalWrite(UART2_RE_PIN, LOW);
	}
	else
	{
		// set to TX mode
		digitalWrite(UART2_DE_PIN, HIGH);
		digitalWrite(UART2_RE_PIN, HIGH);
	}
}

void F4DMXSerial::Send_Packet()
{
	// see
	// http://www.dmx512-online.com/dmx512_packet.html

	// set pin 5 to IO
	// https://stackoverflow.com/questions/66011702/how-to-define-bits-in-gpio-moder-register-of-stm32
	uint32_t reg = DMX_GPIO_Port -> MODER;
	reg &= ~(0b11 << (5 * 2));
	reg |= 0b01 << (5 * 2);
	DMX_GPIO_Port -> MODER = reg;

    HAL_GPIO_WritePin(DMX_GPIO_Port, DMX_TX_GPIO_Pin, GPIO_PIN_RESET);	// HAL should be faster then digitalWrite()
    delayMicroseconds(DMXBREAK);
    HAL_GPIO_WritePin(DMX_GPIO_Port, DMX_TX_GPIO_Pin, GPIO_PIN_SET);
    delayMicroseconds(DMXMAB);

    // set pin 5 back to alternate mode
    reg &= ~(0b11 << (5 * 2));
    reg |= 0b10 << (5 * 2);
    DMX_GPIO_Port -> MODER = reg;
	
	uint8_t* tmp = dma_front_buffer;
	dma_front_buffer = dma_back_buffer;
	dma_back_buffer = tmp;

    // dmx buffer
    // START CODE |   Data0   | ... |  Data511
    // 0000 0000  | xxxx xxxx | ... | xxxx xxxx
    HAL_UART_Transmit_DMA(&huart2, dma_front_buffer, DMX_BUFFER_SIZE);
}



static void MX_DMA1_Init(void){
	  /* DMA controller clock enable */
	  __DMA1_CLK_ENABLE();

	  /* Peripheral DMA init*/
	  hdma_usart2_tx.Instance = DMA1_Stream6;
	  hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
	  hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	  hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	  hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
	  hdma_usart2_tx.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
	  hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	  hdma_usart2_tx.Init.Mode = DMA_NORMAL;
	  hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
	  hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	  HAL_DMA_Init(&hdma_usart2_tx);

	  __HAL_LINKDMA(&huart2,hdmatx,hdma_usart2_tx);

	  /* DMA interrupt init */
	  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}


static void MX_USART2_UART_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  __GPIOD_CLK_ENABLE();


  /**USART2 GPIO Configuration
  PD5     ------> USART2_TX
  PD6     ------> USART2_RX
  */
  GPIO_InitStruct.Pin = DMX_TX_GPIO_Pin|DMX_RX_GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(DMX_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(DMX_GPIO_Port, DMX_TX_GPIO_Pin, GPIO_PIN_SET);

  __USART2_CLK_ENABLE();

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 250000;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_2;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

/*  UART interrupt not used for this */
//  HAL_NVIC_SetPriority(USART2_IRQn,  5, 1);
//  HAL_NVIC_EnableIRQ(USART2_IRQn);

}


/* add this to the variant.cpp file
 * core/variant/variant.cpp+
 */

/* This function handles DMA1 stream6 global interrupt. */
/*
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart2;
void DMA1_Stream6_IRQHandler(void);

void DMA1_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  huart2.gState = HAL_UART_STATE_READY;
}

*******************************************/
