/**
  ******************************************************************************
  * @file    uart_dbg.c
  * @version V1.6.0
  * @date    02-08-2020
  * @brief   Инициализация драйвера для запуска UART в режиме отладки
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "uart_dbg.h"

#if  (DBG_UART_ENABLE == 1)

/* Буфер для диагностического сообщения */
volatile uint8_t buf_dbg[DBG_UART_MAX_SIZE_BUFF];
/* Индекс записи сообщения */
uint16_t index_wr_buf_mes = 0;
/* Индекс чтения сообщения */
volatile uint16_t index_rd_buf_mes = 0;

/* Буфер для приема команды */
volatile uint8_t buf_cmd[DBG_UART_MAX_SIZE_CMD_BUFF];
/* Индекс записи команды */
volatile uint16_t index_wr_buf_cmd = 0;
/* Индекс чтения команды */
uint16_t index_rd_buf_cmd = 0;

/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/**
  * @brief  Инициализация аппаратной части отладки по uart.
  * @param  None
  * @retval None
  */
void DBG_UART_Setup(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	DBG_UART_TX_GPIO_CLK_ENABLE();
	DBG_UART_RX_GPIO_CLK_ENABLE();
	/* Enable USART2 clock */
	DBG_UART_CLK_ENABLE(); 
  
	/*##-2- Configure peripheral GPIO ##########################################*/  
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = DBG_UART_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = DBG_UART_TX_AF;
	
	HAL_GPIO_Init(DBG_UART_TX_GPIO_PORT, &GPIO_InitStruct);
    
	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = DBG_UART_RX_PIN;
	GPIO_InitStruct.Alternate = DBG_UART_RX_AF;
    
	HAL_GPIO_Init(DBG_UART_RX_GPIO_PORT, &GPIO_InitStruct);
    
	/*##-3- Configure the NVIC for UART ########################################*/
	/* NVIC for USART1 */
	HAL_NVIC_SetPriority(DBG_UART_IRQn, 0, 5);
	HAL_NVIC_EnableIRQ(DBG_UART_IRQn);

		
	/*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART configured as follow:
    - Word Length = 8 Bits
    - Stop Bit = One Stop bit
    - Parity = None
    - BaudRate = 9600 baud
    - Hardware flow control disabled (RTS and CTS signals) */
	UartHandle.Instance          = DBG_UART_PORT;
  
	UartHandle.Init.BaudRate     = DBG_BaudRate;
	UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits     = UART_STOPBITS_1;
	UartHandle.Init.Parity       = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode         = UART_MODE_TX_RX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    
	if (HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		Error_Handler((const char*)"Initializes Debug USART");
	}
	
	/* Enable the UART Data Register not empty Interrupt */
	__HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);		
}

/**
  * @brief  Разрешение прерывания передачика UART.
  * @param  None
  * @retval None
  */
void uart_irq_txe_en(void)
{
	__HAL_UART_ENABLE_IT(&UartHandle, UART_IT_TXE); 
}

/**
  * @brief  Прерывание DBG_UART.
  * @param  None
  * @retval None
  */
void DBG_UART_IRQHandler(void)
{
	uint32_t isrflags   = READ_REG(UartHandle.Instance->ISR);
	uint32_t cr1its     = READ_REG(UartHandle.Instance->CR1);
	uint32_t cr3its     = READ_REG(UartHandle.Instance->CR3);
	
	/* UART in mode Receiver -------------------------------------------------*/
	if (((isrflags & USART_ISR_RXNE_RXFNE) != 0U)
	    && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U)
	        || ((cr3its & USART_CR3_RXFTIE) != 0U)))
	{
		/* принимаем данные        */
		buf_cmd[index_wr_buf_cmd] =  (uint8_t)(UartHandle.Instance->RDR);
		/* нкремент индекса */ 
		index_wr_buf_cmd++;
		/* Проверка на переполнение  */
		if (index_wr_buf_cmd >= DBG_UART_MAX_SIZE_CMD_BUFF)
		{
			/* Организация циклического буфера */  
			index_wr_buf_cmd = 0;    
		}		
	}
	
	/* UART TX Fifo Empty occurred ----------------------------------------------*/
	//if (((isrflags & USART_ISR_TXFE) != 0U) && ((cr1its & USART_CR1_TXFEIE) != 0U))
  if(((isrflags & USART_ISR_TXE_TXFNF) != 0U)
      && (((cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U)
          || ((cr3its & USART_CR3_TXFTIE) != 0U)))
	{
		/* TXE flag автоматически очищается когда записываются новые данные в TDR register */
		if (index_rd_buf_mes == index_wr_buf_mes)
		{
			/* Данных в буфере нет - отключение передачи */
		    /* отключаем прерывание по передаче */
			__HAL_UART_DISABLE_IT(&UartHandle, UART_IT_TXFNF);     
		}
		else
		{
			/* Есть данные передаем и инкрементируем индекс */
		    /* Передача байта сообщения */
			UartHandle.Instance->TDR = (uint16_t)(buf_dbg[index_rd_buf_mes]);      
			/* Увеличение индекса */
			index_rd_buf_mes++;
			/* Проверка на переполнение  */
			if (index_rd_buf_mes >= DBG_UART_MAX_SIZE_BUFF)
			{
				/* Организация циклического буфера */  
				index_rd_buf_mes = 0;    
			}  
		} 
	}
}
#endif  /* (DBG_UART_ENABLE == 1) */

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/
