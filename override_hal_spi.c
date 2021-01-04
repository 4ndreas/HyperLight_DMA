#include "stm32f4xx_hal.h"

#define SPI_DEFAULT_TIMEOUT 100U
#define SPI_BSY_FLAG_WORKAROUND_TIMEOUT 1000U /*!< Timeout 1000 µs             */

/**
  * @brief  Handle SPI Communication Timeout.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *              the configuration information for SPI module.
  * @param  Flag SPI flag to check
  * @param  State flag state to check
  * @param  Timeout Timeout duration
  * @param  Tickstart tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SPI_WaitFlagStateUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus State,
                                                       uint32_t Timeout, uint32_t Tickstart)
{
  while ((__HAL_SPI_GET_FLAG(hspi, Flag) ? SET : RESET) != State)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) >= Timeout) || (Timeout == 0U))
      {
        /* Disable the SPI and reset the CRC: the CRC value should be cleared
        on both master and slave sides in order to resynchronize the master
        and slave for their respective CRC calculation */

        /* Disable TXE, RXNE and ERR interrupts for the interrupt process */
        __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

        if ((hspi->Init.Mode == SPI_MODE_MASTER) && ((hspi->Init.Direction == SPI_DIRECTION_1LINE)
                                                     || (hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
        {
          /* Disable SPI peripheral */
          __HAL_SPI_DISABLE(hspi);
        }

        /* Reset CRC Calculation */
        if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
        {
          SPI_RESET_CRC(hspi);
        }

        hspi->State = HAL_SPI_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hspi);

        return HAL_TIMEOUT;
      }
    }
  }

  return HAL_OK;
}

/**
  * @brief  Handle the check of the RXTX or TX transaction complete.
  * @param  hspi SPI handle
  * @param  Timeout Timeout duration
  * @param  Tickstart tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SPI_EndRxTxTransaction(SPI_HandleTypeDef *hspi, uint32_t Timeout, uint32_t Tickstart)
{
  /* Timeout in µs */
  __IO uint32_t count = SPI_BSY_FLAG_WORKAROUND_TIMEOUT * (SystemCoreClock / 24U / 1000000U);
  /* Erratasheet: BSY bit may stay high at the end of a data transfer in Slave mode */
  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Control the BSY flag */
    if (SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_BSY, RESET, Timeout, Tickstart) != HAL_OK)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
      return HAL_TIMEOUT;
    }
  }
  else
  {
    /* Wait BSY flag during 1 Byte time transfer in case of Full-Duplex and Tx transfer
    * If Timeout is reached, the transfer is considered as finish.
    * User have to calculate the timeout value to fit with the time of 1 byte transfer.
    * This time is directly link with the SPI clock from Master device.
    */
    do
    {
      if (count == 0U)
      {
        break;
      }
      count--;
    } while (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_BSY) != RESET);
  }

  return HAL_OK;
}

static unsigned transmit_loop_time = 0;
static uint8_t 			data_dump;

HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size,
                                          uint32_t Timeout)
{
  uint16_t             initial_TxXferCount;
  uint32_t             tmp_mode;
  HAL_SPI_StateTypeDef tmp_state;
  uint32_t             tickstart;

  /* Variable used to alternate Rx and Tx during transfer */
  uint32_t             txallowed = 1U;
  HAL_StatusTypeDef    errorcode = HAL_OK;

  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

  /* Process Locked */
  __HAL_LOCK(hspi);

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  /* Init temporary variables */
  tmp_state           = hspi->State;
  tmp_mode            = hspi->Init.Mode;
  initial_TxXferCount = Size;

  if (!((tmp_state == HAL_SPI_STATE_READY) || \
        ((tmp_mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES) && (tmp_state == HAL_SPI_STATE_BUSY_RX))))
  {
    errorcode = HAL_BUSY;
    goto error;
  }

  if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
  {
    errorcode = HAL_ERROR;
    goto error;
  }

  /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
  if (hspi->State != HAL_SPI_STATE_BUSY_RX)
  {
    hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
  }

  /* Set the transaction information */
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pRxBuffPtr  = (uint8_t *)pRxData;
  hspi->RxXferCount = Size;
  hspi->RxXferSize  = Size;
  hspi->pTxBuffPtr  = (uint8_t *)pTxData;
  hspi->TxXferCount = Size;
  hspi->TxXferSize  = Size;

  /*Init field not used in handle to zero */
  hspi->RxISR       = NULL;
  hspi->TxISR       = NULL;

#if (USE_SPI_CRC != 0U)
  /* Reset CRC Calculation */
  if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    SPI_RESET_CRC(hspi);
  }
#endif /* USE_SPI_CRC */

  /* Check if the SPI is already enabled */
  if ((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

  /* Transmit and Receive data in 16 Bit mode */
  if (hspi->Init.DataSize == SPI_DATASIZE_16BIT)
  {
    if ((hspi->Init.Mode == SPI_MODE_SLAVE) || (initial_TxXferCount == 0x01U))
    {
      hspi->Instance->DR = *((uint16_t *)hspi->pTxBuffPtr);
      hspi->pTxBuffPtr += sizeof(uint16_t);
      hspi->TxXferCount--;
    }
    while ((hspi->TxXferCount > 0U) || (hspi->RxXferCount > 0U))
    {
      /* Check TXE flag */
      if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE)) && (hspi->TxXferCount > 0U) && (txallowed == 1U))
      {
        hspi->Instance->DR = *((uint16_t *)hspi->pTxBuffPtr);
        hspi->pTxBuffPtr += sizeof(uint16_t);
        hspi->TxXferCount--;
        /* Next Data is a reception (Rx). Tx not allowed */
        txallowed = 0U;

#if (USE_SPI_CRC != 0U)
        /* Enable CRC Transmission */
        if ((hspi->TxXferCount == 0U) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE))
        {
          SET_BIT(hspi->Instance->CR1, SPI_CR1_CRCNEXT);
        }
#endif /* USE_SPI_CRC */
      }

      /* Check RXNE flag */
      if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE)) && (hspi->RxXferCount > 0U))
      {
        *((uint16_t *)hspi->pRxBuffPtr) = (uint16_t)hspi->Instance->DR;
        hspi->pRxBuffPtr += sizeof(uint16_t);
        hspi->RxXferCount--;
        /* Next Data is a Transmission (Tx). Tx is allowed */
        txallowed = 1U;
      }
      if (((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY))
      {
        errorcode = HAL_TIMEOUT;
        goto error;
      }
    }
  }
  /* Transmit and Receive data in 8 Bit mode */
  else
  {
    /*if ((hspi->Init.Mode == SPI_MODE_SLAVE) || (initial_TxXferCount == 0x01U))
    {
      *((__IO uint8_t *)&hspi->Instance->DR) = (*hspi->pTxBuffPtr);
      hspi->pTxBuffPtr += sizeof(uint8_t);
      hspi->TxXferCount--;
    }*/

    unsigned transmit_loop_start = micros();
    unsigned count = hspi->TxXferCount;
    uint8_t* tx_buf = hspi->pTxBuffPtr;
    uint8_t* rx_buf = hspi->pRxBuffPtr;

    // not sure if this can happen
    while(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_BSY));
    // not sure if this can happpen, but if we have data stuck in the dr, the loop waiting for the last rx bate will hang
    if((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE))) {
    	data_dump = *(__IO uint8_t *)&hspi->Instance->DR;
    }
    // make sure we are ready to send
    while(!__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE));
    // kick off transfer, this byte should immedeatly go to the shift register, leaving the send buffer free
    *(__IO uint8_t *)&hspi->Instance->DR = (*(tx_buf++));
    count--;

    while (count > 0U)
    {
      __disable_irq(); // once we put the next byte in, we have to read the dr within 7 spi clocks
      // wait for free send buffer, this should typically happen after one spi clock
      while(!__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE));
      // but byte into the send buffer
      *(__IO uint8_t *)&hspi->Instance->DR = (*(tx_buf++));
      // wait for data available in the receive buffer, this should happen shortly before the send buffer becomes free
      while(!(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE)));
      // read and clear receive buffer
      (*(rx_buf++)) = *(__IO uint8_t *)&hspi->Instance->DR;
      count--;
      __enable_irq(); // hopfully this is sufficient to still service interrupts while the loop is running
    }
    // since we started sending one byte before the loop, we also need to read one after
    while(!(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE)));
    // read and clear receive buffer
    (*(rx_buf++)) = *(__IO uint8_t *)&hspi->Instance->DR;
    // we should do this before the spi is disabled according to the st reference manual
    while(!__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE));
    while(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_BSY));

    hspi->TxXferCount = 0;
    hspi->RxXferCount = 0;
    hspi->pTxBuffPtr = tx_buf;
    hspi->pRxBuffPtr = rx_buf;
    transmit_loop_time = micros() - transmit_loop_start;
    /* OLD CODE:
     *     unsigned transmit_loop_start = micros();
  unsigned tx_count = hspi->TxXferCount;
  unsigned rx_count = hspi->RxXferCount;
  uint8_t* tx_buf = hspi->pTxBuffPtr;
  uint8_t* rx_buf = hspi->pRxBuffPtr;

  while ((tx_count > 0U) || (rx_count > 0U))
  {
    /* Check TXE flag *//*
    if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE)) && (tx_count > 0U) && (txallowed == 1U))
    {
      *(__IO uint8_t *)&hspi->Instance->DR = (*tx_buf);
      tx_buf++;
      tx_count--;
      /* Next Data is a reception (Rx). Tx not allowed *//*
      txallowed = 0U;

#if (USE_SPI_CRC != 0U)
      /* Enable CRC Transmission *//*
      if ((hspi->TxXferCount == 0U) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE))
      {
        SET_BIT(hspi->Instance->CR1, SPI_CR1_CRCNEXT);
      }
#endif /* USE_SPI_CRC */
    /*}

    /* Wait until RXNE flag is reset *//*
    if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE)) && (rx_count > 0U))
    {
      (*(uint8_t *)rx_buf) = hspi->Instance->DR;
      rx_buf++;
      rx_count--;
      /* Next Data is a Transmission (Tx). Tx is allowed *//*
      txallowed = 1U;
    }
    /*if ((((HAL_GetTick() - tickstart) >=  Timeout) && ((Timeout != HAL_MAX_DELAY))) || (Timeout == 0U))
    {
      errorcode = HAL_TIMEOUT;
      goto error;
    }*/
  }

#if (USE_SPI_CRC != 0U)
  /* Read CRC from DR to close CRC calculation process */
  if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    /* Wait until TXE flag */
    if (SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SET, Timeout, tickstart) != HAL_OK)
    {
      /* Error on the CRC reception */
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_CRC);
      errorcode = HAL_TIMEOUT;
      goto error;
    }
    /* Read CRC */
    READ_REG(hspi->Instance->DR);
  }

  /* Check if CRC error occurred */
  if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR))
  {
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_CRC);
    /* Clear CRC Flag */
    __HAL_SPI_CLEAR_CRCERRFLAG(hspi);

    errorcode = HAL_ERROR;
  }
#endif /* USE_SPI_CRC */

  /* Check the end of the transaction */
  if (SPI_EndRxTxTransaction(hspi, Timeout, tickstart) != HAL_OK)
  {
    errorcode = HAL_ERROR;
    hspi->ErrorCode = HAL_SPI_ERROR_FLAG;
    goto error;
  }

  /* Clear overrun flag in 2 Lines communication mode because received is not read */
  if (hspi->Init.Direction == SPI_DIRECTION_2LINES)
  {
    __HAL_SPI_CLEAR_OVRFLAG(hspi);
  }

error :
  hspi->State = HAL_SPI_STATE_READY;
  __HAL_UNLOCK(hspi);
  return errorcode;
}
