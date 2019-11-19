/*
 * hyperlight.cpp
 *
 *  Created on: 19.11.2019
 *      Author: ahoel
 */

#include "hyperlight.h"

/* HAL Functions */
void HAL_MspInit(void) ;
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base) ;
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim) ;
static void MX_DMA_Init(void) ;
static void MX_TIM8_Init(void) ;
static void MX_GPIO_Init(void) ;
HAL_StatusTypeDef HAL_TIM_PWM_Set(TIM_HandleTypeDef *htim, uint32_t Channel) ;
static void Data_GPIO_Config(void) ;



hyperlight::hyperlight() {
	// TODO Auto-generated constructor stub

}



void hyperlight::begin(void){

// init HAL Layer

MX_GPIO_Init();
MX_DMA_Init();
MX_TIM8_Init();
Data_GPIO_Config();


}

void hyperlight::setAll(uint8_t red,uint8_t green ,uint8_t blue)
{
  for (int j = 0; j < LED_LINES; j++)
    {
      for(int i = 0; i< LED_LENGHT; i++)
      {
        setLED(j,i,red,green,blue);
      }
    }
}

void hyperlight::setLED(int strip, int led_number, uint8_t red, uint8_t green ,uint8_t blue)
{
  int offset = (led_number )* LED_COLORS * 8 + DMA_DUMMY;

  uint16_t stripClrMask = ~(1<< strip);
  uint16_t stripSetMask = (1<< strip);

  uint32_t color = (red << 16) + (green << 8) + (blue);

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

void hyperlight::setStripLED(int strip, uint8_t * data, int data_length, int start)
{
  int offset = (start )* LED_COLORS * 8 + DMA_DUMMY;

  uint16_t stripClrMask = ~(1<< strip);
  uint16_t stripSetMask = (1<< strip);

  int i;
  int j;

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


/* HAL Functions */


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
  /* USER CODE BEGIN TIM8_MspInit 0 */

  /* USER CODE END TIM8_MspInit 0 */
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





static void Wait_DMA(void)
{
	while(TransferLock != 0 )
	{

	}
}

static void Start_DMA(void)
{
//  TIMX_DMA_HANDLE.XferCpltCallback = TransferComplete;
//  TIMX_DMA_HANDLE.XferErrorCallback = TransferError ;

//  HAL_DMA_Init(&TIMX_DMA_HANDLE);
  HAL_DMA_RegisterCallback(&TIMX_DMA_HANDLE, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&TIMX_DMA_HANDLE, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  if (HAL_DMA_Start_IT(&TIMX_DMA_HANDLE, (uint32_t)&frame_buffer, (uint32_t)(GPIOE_ODR), FULL_FRAME_SIZE) != HAL_OK)
  {
  /* Transfer Error */
  Error_Handler();
  }

//  __HAL_DMA_DISABLE_IT(&TIMX_DMA_HANDLE, DMA_IT_HT);
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

  //HAL_DMA_Init(&TIMX_DMA_HANDLE);
  __HAL_TIM_ENABLE(&TIMX_HANDLE);
}

static void Restart_DMA(void)
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
uint32_t TransferErrorCounter = 0;

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

