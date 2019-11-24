/*
 * hyperlight.cpp
 *
 *  Created on: 19.11.2019
 *      Author: ahoel
 */

#include "hyperlight.h"

/* HAL Functions */
TIM_HandleTypeDef htim8;
DMA_HandleTypeDef hdma_tim8_ch1;

void HAL_MspInit(void) ;
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base) ;
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim) ;
static void MX_DMA_Init(void) ;
static void MX_TIM8_Init(void) ;
static void MX_GPIO_Init(void) ;
HAL_StatusTypeDef HAL_TIM_PWM_Set(TIM_HandleTypeDef *htim, uint32_t Channel) ;
static void Data_GPIO_Config(void) ;

static void TransferComplete(DMA_HandleTypeDef *DmaHandle);
static void TransferError(DMA_HandleTypeDef *DmaHandle);

static uint16_t frame_buffer[FULL_FRAME_SIZE];

static uint8_t gamma8[] = {
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

int TransferLock = 0;
uint32_t TransferErrorCounter = 0;


hyperlight::hyperlight() {
	// TODO Auto-generated constructor stub
	_useGamma = 1;
}



void hyperlight::begin(void){

	// init HAL Layer
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM8_Init();
	Data_GPIO_Config();



	setAll(0,0,0);
	Start_DMA();
}
/**
  * @brief  Starts DMA output
  */
void hyperlight::show(void)
{
	Wait_DMA();
	Restart_DMA();
}

void hyperlight::setAll(uint8_t red, uint8_t green , uint8_t blue)
{
  for (int j = 0; j < LED_LINES; j++)
    {
      for(int i = 0; i< LED_LENGHT; i++)
      {
        setLED(j,i,blue,green,red);
      }
    }
}

/**
  * @brief  sets a single led
  * @param  strip: led channel
  *         led_number: led position
  *         red: red color channel
  *         green: green color channel
  *         blue: blue color channel
  */
void hyperlight::setLED(int strip, int led_number, uint8_t red, uint8_t green ,uint8_t blue)
{
  int offset = (led_number )* LED_COLORS * 8 + DMA_DUMMY;

  uint16_t stripClrMask = ~(1<< strip);
  uint16_t stripSetMask = (1<< strip);
  uint32_t color;

  if(_useGamma)
	  {
	  color = (gamma8[red] << 16) + (gamma8[green] << 8) + (gamma8[blue]);
	  }
  else
	  {
	   color = (red << 16) + (green << 8) + (blue);
	  }
/*check buffer limit */
if((offset+24) < FULL_FRAME_SIZE )
  {
	  int i;
	  /* set Color */
	  for(i = 0; i < 24; i++)
	  {
		  /* clear bit */
		  frame_buffer[offset+23-i] &= stripClrMask;

		  /* set bit */
		  if(color & (1<<i))
			{
			 frame_buffer[offset+23-i] |= stripSetMask;
			}
	  }
  }
}

void hyperlight::setLED(int strip, int led_number, uint8_t red, uint8_t green ,uint8_t blue, uint8_t white)
{
  int offset = (led_number )* 4 * 8 + DMA_DUMMY;

  uint16_t stripClrMask = ~(1<< strip);
  uint16_t stripSetMask = (1<< strip);
  uint32_t color;

  if(_useGamma)
	  {
	  color = (gamma8[green] << 24) + (gamma8[red] << 16) + (gamma8[blue] << 8) + (gamma8[white]);
	  }
  else
	  {
	   color = (green << 24) +(red << 16) + (blue << 8) + (white);
	  }
/*check buffer limit */
if((offset+32) < FULL_FRAME_SIZE )
  {
	  int i;
	  /* set Color */
	  for(i = 0; i < 32; i++)
	  {
		  /* clear bit */
		  frame_buffer[offset+31-i] &= stripClrMask;

		  /* set bit */
		  if(color & (1<<i))
			{
			 frame_buffer[offset+31-i] |= stripSetMask;
			}
	  }
  }
}

void hyperlight::setOffset(int strip, int _offset)
{
	statusLedOffsets[strip] = _offset;
}

void hyperlight::setOffsetColor(int strip, uint8_t red, uint8_t green ,uint8_t blue)
{
	for(int i = 0; i< statusLedOffsets[strip]; i++)
	{
		setLED(strip, i,red, green, blue);
	}
}

/**
  * @brief  sets leds from a buffer
  * @param  strip: led channel
  *         *data: pointer to the buffer
  *         data_length: buffer size
  *         start: leds offset
  */
void hyperlight::setStripLED(int strip, uint8_t * data, int data_length, int start, colorMode color)
{
  updatetime[strip] =  millis();
  int offset = (start )* LED_COLORS * 8 + DMA_DUMMY;

  uint16_t stripClrMask = ~(1<< strip);
  uint16_t stripSetMask = (1<< strip);

  int i;
  int j;

  if(_useGamma)
	{
	  uint8_t col;
	  if(color == RGB){
	  for(j = 0; j< data_length; j++){
		  /* set Color */
		  col = gamma8[data[j]];
		  for(i = 0; i < 8; i++)
		  {
			  /* clear bit */
			  frame_buffer[offset+7-i] &= stripClrMask;

			  /* set bit */
			  if(col & (1<<i))
				{
				 frame_buffer[offset+7-i] |= stripSetMask;
				}
		  }
		  offset+=8;
	  	  }
	  }
	  else if(color == RBG)
	  {
		  for(j = 0; j< data_length; j+=3){
			  /* set Color */
			  col = gamma8[data[j+1]];

			  for(i = 0; i < 8; i++)
			  {	  /* clear bit */
				  frame_buffer[offset+7-i] &= stripClrMask;
				  /* set bit */
				  if(col & (1<<i))
					{ frame_buffer[offset+7-i] |= stripSetMask;	}
			  }
			  offset+=8;
			  col = gamma8[data[j]];
			  for(i = 0; i < 8; i++)
			  {	  /* clear bit */
				  frame_buffer[offset+7-i] &= stripClrMask;
				  /* set bit */
				  if(col & (1<<i))
					{ frame_buffer[offset+7-i] |= stripSetMask;	}
			  }
			  offset+=8;
			  col = gamma8[data[j+2]];
			  for(i = 0; i < 8; i++)
			  {	  /* clear bit */
				  frame_buffer[offset+7-i] &= stripClrMask;
				  /* set bit */
				  if(col & (1<<i))
					{ frame_buffer[offset+7-i] |= stripSetMask;	}
			  }
			  offset+=8;
		  }
	  }
	}
  else
  {
    for(j = 0; j< data_length; j++){
	  /* set Color */
	  for(i = 0; i < 8; i++)
	  {
		  /* clear bit */
		  frame_buffer[offset+7-i] &= stripClrMask;

		  /* set bit */
		  if(data[j] & (1<<i))
			{
			 frame_buffer[offset+7-i] |= stripSetMask;
			}
	  }
	  offset+=8;
   }
  }
}

void  hyperlight::setSnakeLED(int strip, uint8_t * data, int data_length, int ledOffset, uint16_t snakeLength)
{
  updatetime[strip] =  millis();

  for(int i = 0; i < data_length/3; i++)
  {
	  setLED(strip, DMXIdToPixelId(ledOffset, i, snakeLength), data[i*3+1],data[i*3],data[i*3 +2]);	//GRB
  }
}


uint16_t hyperlight::DMXIdToPixelId(uint16_t u, uint16_t dmxId, uint16_t snakeLength) {
  uint16_t absDMXid = u + dmxId;
  uint16_t row = absDMXid / snakeLength;
  uint16_t col_raw = absDMXid % snakeLength;
  uint16_t col = (row % 2) ? (snakeLength - col_raw -1) : col_raw;

  return row * snakeLength + col;
}


uint32_t hyperlight::getUpdateTime( int strip)
{
	if(strip < LED_LINES){

	return updatetime[strip];
	}

	return 0;
}

void hyperlight::Wait_DMA(void)
{
	while(TransferLock != 0 )
	{

	}
}

void hyperlight::Start_DMA(void)
{
  HAL_DMA_RegisterCallback(&TIMX_DMA_HANDLE, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&TIMX_DMA_HANDLE, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  if (HAL_DMA_Start_IT(&TIMX_DMA_HANDLE, (uint32_t)&frame_buffer, (uint32_t)(GPIOE_ODR), FULL_FRAME_SIZE) != HAL_OK)
  {
  /* Transfer Error */
  Error_Handler();
  }

  __HAL_DMA_DISABLE_IT(&TIMX_DMA_HANDLE, DMA_IT_HT);
  __HAL_TIM_ENABLE_DMA(&TIMX_HANDLE, TIM_DMA_CC1);

  if (HAL_TIM_PWM_Set(&TIMX_HANDLE, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
  if (HAL_TIM_PWM_Set(&TIMX_HANDLE, TIM_CHANNEL_2) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
  if (HAL_TIM_PWM_Set(&TIMX_HANDLE, TIM_CHANNEL_3) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }

  __HAL_TIM_ENABLE(&TIMX_HANDLE);

  TransferCounter++;
  TransferLock = 1;
}

void hyperlight::Restart_DMA(void)
{
  if(TransferLock == 0)
	{
	  TransferCounter++;
	  GPIOC->MODER = 0x28000;

	  if (HAL_DMA_Start_IT(&TIMX_DMA_HANDLE, (uint32_t)&frame_buffer, (uint32_t)(GPIOE_ODR), FULL_FRAME_SIZE-1) != HAL_OK)
	  {
		/* Transfer Error */
		Error_Handler();
	  }

	  // restart Timer
	  TIM8->CCR2 = TIM_T1H;
	  TIM8->CCR3 = TIM_T0H;
	  TIM8->CR1 |= (TIM_CR1_CEN);

	  TransferLock = 1;
	}
}


/******************************************************************************/
/* HAL Functions                                                              */
/* don't change anything down here                                            */
/* Low Level init functions                                                   */
/******************************************************************************/

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base)
{
  enableTimerClock(htim_base);

  //  configure Update interrupt
  HAL_NVIC_SetPriority(getTimerUpIrq(htim_base->Instance), TIM_IRQ_PRIO, TIM_IRQ_SUBPRIO);
  HAL_NVIC_EnableIRQ(getTimerUpIrq(htim_base->Instance));

  if (getTimerCCIrq(htim_base->Instance) != getTimerUpIrq(htim_base->Instance)) {
    // configure Capture Compare interrupt
    HAL_NVIC_SetPriority(getTimerCCIrq(htim_base->Instance), TIM_IRQ_PRIO, TIM_IRQ_SUBPRIO);
    HAL_NVIC_EnableIRQ(getTimerCCIrq(htim_base->Instance));
  }


  if(htim_base->Instance==TIM8)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM8_CLK_ENABLE();

    /* TIM8 DMA Init */
    /* TIM8_CH1 Init */
    hdma_tim8_ch1.Instance = DMA2_Stream2;
    hdma_tim8_ch1.Init.Channel = DMA_CHANNEL_7;
    hdma_tim8_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim8_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim8_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim8_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim8_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim8_ch1.Init.Mode = DMA_NORMAL;
    hdma_tim8_ch1.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_tim8_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_tim8_ch1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(htim_base,hdma[TIM_DMA_ID_CC1],hdma_tim8_ch1);
  }
}


void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM8)
  {
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }

}


static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA2_CLK_ENABLE();

  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}

static void MX_TIM8_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};


  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = TIM_CLK;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = TIM_TOO;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = TIM_T1H; //50 52
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = TIM_T0H; //125 136
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim8);
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE3 PE4 PE5
                           PE6 PE7 PE8 PE9
                           PE10 PE11 PE12 PE13
                           PE14 PE15 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}



HAL_StatusTypeDef HAL_TIM_PWM_Set(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCX_INSTANCE(htim->Instance, Channel));

  /* Enable the Capture compare channel */
  TIM_CCxChannelCmd(htim->Instance, Channel, TIM_CCx_ENABLE);

  if (IS_TIM_BREAK_INSTANCE(htim->Instance) != RESET)
  {
    /* Enable the main output */
    __HAL_TIM_MOE_ENABLE(htim);
  }

  /* Return function status */
  return HAL_OK;
}

static void Data_GPIO_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;

  /* Enable GPIO Clocks */
  DATA_GPIO_CLK_ENABLE();

  /* Configure output of GPIO as output  */
  GPIO_InitStruct.Pin = DATA_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  HAL_GPIO_Init(DATA_GPIO_PORT, &GPIO_InitStruct);
}



/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void TransferComplete(DMA_HandleTypeDef *DmaHandle)
{

  TIM8->CR1 &= ~(TIM_CR1_CEN);
  TIM8->CCR2 = 0;
  TIM8->CCR3 = 0;
  TIM8->EGR |= (1 << 0); // Bit 0: UG

  HAL_TIM_DMADelayPulseCplt(DmaHandle);
  TransferLock = 0;
}


/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void TransferError(DMA_HandleTypeDef *DmaHandle)
{
  /* end of transaction */
  TIM8->CR1 &= ~(TIM_CR1_CEN);
  TIM8->CCR2 = 0;
  TIM8->CCR3 = 0;
  TIM8->EGR |= (1 << 0); // Bit 0: UG

  HAL_TIM_DMADelayPulseCplt(DmaHandle);

  TransferErrorCounter++;
  TransferLock = 0;
}

