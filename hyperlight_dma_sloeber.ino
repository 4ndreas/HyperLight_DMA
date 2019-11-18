#include "Arduino.h"
#include "variant.h"
//#include <IWatchdog.h>

//#include "stm32f4xx_hal.h"
//#include "stm32f4xx_hal_tim.h"
//#include "stm32f4xx_hal_gpio.h"

TIM_HandleTypeDef htim8;
DMA_HandleTypeDef hdma_tim8_ch1;

#define TIMx_DMA_ID  TIM_DMA_ID_CC1
#define TIMX_HANDLE  htim8
#define TIMX_DMA_HANDLE hdma_tim8_ch1

#define GPIOE_ODR               (GPIOE_BASE + 0x14)
#define DATA_GPIO_CLK_ENABLE     __HAL_RCC_GPIOE_CLK_ENABLE
#define DATA_GPIO_PORT           GPIOE
#define DATA_GPIO_PIN            GPIO_PIN_All


#define TIM_T1H 0x85
#define TIM_T0H 0x32

#define LED_LINES 16
#define LED_LENGHT 2
#define LED_COLORS 3
#define DMA_DUMMY 2

#define FULL_FRAME_SIZE ( LED_LENGHT * LED_COLORS * 8 )+ DMA_DUMMY    /* max = 65530 (65535 - 5 dummy bits) due to dma limitation  */

static uint16_t frame_buffer[FULL_FRAME_SIZE];


void setAll(uint8_t red,uint8_t green ,uint8_t blue);
void setLED(int strip, int led_number, uint8_t red,uint8_t green ,uint8_t blue);
static void Start_DMA(void);
static void Restart_DMA(void);
static void TransferComplete(DMA_HandleTypeDef *DmaHandle);
static void TransferError(DMA_HandleTypeDef *DmaHandle);
HAL_StatusTypeDef HAL_TIM_PWM_Set(TIM_HandleTypeDef *htim, uint32_t Channel);
static void Data_GPIO_Config(void);
void SystemClock_Config_new(void);



void setup() {

  //HAL_Init();
  //SystemClock_Config_new();
//	IWatchdog.begin(4000000);

  //HAL_DMA_Init(&hdma_tim8_ch1);

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM8_Init();
  Data_GPIO_Config();

 //

  // set all leds to single color;
  setAll(255,0,0);


  Start_DMA();
  //delay(1000);


  setAll(0,255,0);
 // Restart_DMA();
  //delay(1);

}

void loop() {
  // put your main code here, to run repeatedly:
    setAll(255,0,0);
    Restart_DMA();

    delay(1000);

    setAll(0,255,0);
    Restart_DMA();

    delay(1000);

    setAll(0,0,255);
    Restart_DMA();

    delay(1000);

    setAll(0,0,0);
    Restart_DMA();

    delay(1000);
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}


void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}


/**
  * @brief  TIMER Initialization - clock init and nvic init
  * @param  htim_base: TIM handle
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base)
{
  enableTimerClock(htim_base);

//  // configure Update interrupt
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
  /* USER CODE BEGIN TIM8_MspPostInit 0 */

  /* USER CODE END TIM8_MspPostInit 0 */

    __HAL_RCC_GPIOC_CLK_ENABLE();

    /**TIM8 GPIO Configuration
    PC7     ------> TIM8_CH2
    PC8     ------> TIM8_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM8_MspPostInit 1 */

  /* USER CODE END TIM8_MspPostInit 1 */
  }

}


/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);


}




/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 209;
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
  sConfigOC.Pulse = 40;
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
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
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


void setAll(uint8_t red,uint8_t green ,uint8_t blue)
{
  for (int j = 0; j < LED_LINES; j++)
    {
      for(int i = 0; i< LED_LENGHT; i++)
      {
        setLED(j,i,red,green,blue);
      }
    }
}


void setLED(int strip, int led_number, uint8_t red,uint8_t green ,uint8_t blue)
{
  int offset = (led_number )* LED_COLORS * 8 + DMA_DUMMY;

  uint16_t stripClrMask = ~(1<< strip);
  uint16_t stripSetMask = (1<< strip);

  uint32_t color = (red << 16) + (green << 8) + (blue);

  int i;
  /* set Color */
  for(i = 0; i < 24; i++)
  {
    /*check buffer limit */
    if((offset-i) < FULL_FRAME_SIZE )
      {
      /* clear bit */
      frame_buffer[offset+i] &= stripClrMask;

      /* set bit */
      if(color & (1<<i))
        {
         frame_buffer[offset+i] |= stripSetMask;
        }
      }
  }
}


uint32_t TIMX_CCMR1 = 0;
uint32_t TIMX_CCMR2 = 0;
uint32_t TransferCounter = 1;

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
//  TransferCounter++;
//  GPIOC->MODER = 0x28000;
//
//  TIMX_DMA_HANDLE.XferCpltCallback = TransferComplete;
//  TIMX_DMA_HANDLE.XferErrorCallback = TransferError ;
//
//  if (HAL_DMA_Start_IT(&TIMX_DMA_HANDLE, (uint32_t)&frame_buffer, (uint32_t)(GPIOE_ODR), FULL_FRAME_SIZE-1) != HAL_OK)
//  {
//    /* Transfer Error */
//    Error_Handler();
//  }
//  __HAL_DMA_DISABLE_IT(&TIMX_DMA_HANDLE, DMA_IT_HT);
//
//
//  HAL_DMA_Start_IT(&TIMX_DMA_HANDLE, (uint32_t)&frame_buffer, (uint32_t)(GPIOE_ODR), FULL_FRAME_SIZE);
//
//
//  if (HAL_TIM_PWM_Set(&TIMX_HANDLE, TIM_CHANNEL_1) != HAL_OK)
//  {
//    /* PWM Generation Error */
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_Set(&TIMX_HANDLE, TIM_CHANNEL_2) != HAL_OK)
//  {
//    /* PWM Generation Error */
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_Set(&TIMX_HANDLE, TIM_CHANNEL_3) != HAL_OK)
//  {
//    /* PWM Generation Error */
//    Error_Handler();
//  }
//
//  // restart Timer
//  TIM8->CCR2 = TIM_T1H;
//  TIM8->CCR3 = TIM_T0H;
//
//  TIM8->CR1 |= (TIM_CR1_CEN);
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





/**
  * @brief  Configure the Data GPIO in output mode
  *
  * @note   None
  * @param  None
  * @retval None
  */
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


//
///******************************************************************************/
///* STM32F4xx Peripheral Interrupt Handlers                                    */
///* Add here the Interrupt Handlers for the used peripherals.                  */
///* For the available peripheral interrupt handler names,                      */
///* please refer to the startup file (startup_stm32f4xx.s).                    */
///******************************************************************************/
//
///**
//  * @brief This function handles DMA2 stream2 global interrupt.
//  */
//void DMA2_Stream2_IRQHandler(void)
//{
//	while(1)
//	{
//
//	}
//
//  HAL_DMA_IRQHandler(&hdma_tim8_ch1);
//}
//
//



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
  // stop timer
//  TIM8->CR1 &= ~(TIM_CR1_CEN);
  TransferErrorCounter++;
}
