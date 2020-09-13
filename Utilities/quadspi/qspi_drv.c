/**
  ******************************************************************************
  * File Name          : qspi_drv.c
  * Description        : This file provides code for the driver QUADSPI Flash.
  * 
  ******************************************************************************
  * @attention
  *
    // Для включения тактирования необходимо добавить данный фрагмент в void SystemClock_Config(void) 
   
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_QSPI;
	PeriphClkInitStruct.PLL2.PLL2M = 1;
	PeriphClkInitStruct.PLL2.PLL2N = 30;
	PeriphClkInitStruct.PLL2.PLL2P = 2;
	PeriphClkInitStruct.PLL2.PLL2Q = 2;
	PeriphClkInitStruct.PLL2.PLL2R = 2;
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
	PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;
	PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler((const char*)"Initializes the UART1 and QSPI clocks");
	}	
  *
  *  
  *
  *
  *
  *
  *
  *
  *
  *
  *
  *
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "qspi_drv.h"
#include "w25q128fv.h"
#include "stm32746g_qspi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "main.h"
#include "pin_dbg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

QSPI_HandleTypeDef hqspi;
uint16_t ret;

/* QUADSPI init function */
void QUADSPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	__HAL_RCC_QSPI_CLK_ENABLE();	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	/* Инициализация основной структуры */

	hqspi.Instance = QUADSPI;
	hqspi.Init.ClockPrescaler = 2;
	hqspi.Init.FifoThreshold = 32;
	hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	hqspi.Init.FlashSize = 23;
	hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE;
	hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
	hqspi.Init.FlashID = QSPI_FLASH_ID_1;
	hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
	if (HAL_QSPI_Init(&hqspi) != HAL_OK)
	{
		Error_Handler((const char*)"QUADSPI init function");
	}
    
	/* Инициализация GPIO  */
	if (hqspi.Instance == QUADSPI)
	{
		/**QUADSPI GPIO Configuration    
		PE2      ------> QUADSPI_BK1_IO2
		PB2      ------> QUADSPI_CLK
		PD11     ------> QUADSPI_BK1_IO0
		PD12     ------> QUADSPI_BK1_IO1
		PD13     ------> QUADSPI_BK1_IO3
		PB6      ------> QUADSPI_BK1_NCS 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}


/* QUADSPI init function после старта задачи*/
void QUADSPI_Task_Init(void)
{
	/* Brief  Initializes the QSPI interface.*/
	BSP_QSPI_Init();
}

void QSPI_DeInit(QSPI_HandleTypeDef* qspiHandle)
{

	if (qspiHandle->Instance == QUADSPI)
	{
		/* Peripheral clock disable */
		__HAL_RCC_QSPI_CLK_DISABLE();
  
		/**QUADSPI GPIO Configuration    
		PE2      ------> QUADSPI_BK1_IO2
		PB2      ------> QUADSPI_CLK
		PD11     ------> QUADSPI_BK1_IO0
		PD12     ------> QUADSPI_BK1_IO1
		PD13     ------> QUADSPI_BK1_IO3
		PB6      ------> QUADSPI_BK1_NCS 
		*/
		HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2 | GPIO_PIN_6);
		HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);
	}
} 

uint8_t wData[0x100];
uint8_t rData[0x100];



/**
  * @brief  The application entry point.
  * @retval int
  */
void test_cmd(void)
{
	/* USER CODE BEGIN 1 */
	QSPI_CommandTypeDef s_command;
	uint8_t pData[3];
	uint32_t i;
	
	T1_HI;		
	
	/* Initialize the read command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 0x100;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
	/* Configure the command */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Manufacture/Read data Quad I/O command");
	}
  
	/* Reception of the data */
	if (HAL_QSPI_Receive(&hqspi, rData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Manufacture/Read data Quad I/O data");
	}	
			
	/* Read Manufacture/Device ID Quad I/O*/
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = QUAD_OUT_FAST_READ_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytes    = 0x00;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 4;
	s_command.NbData            = 0x100;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
	/* Configure the command */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Manufacture/Read data Quad I/O command");
	}
  
	/* Reception of the data */
	if (HAL_QSPI_Receive(&hqspi, rData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Manufacture/Read data Quad I/O data");
	}	
	
	T1_LO;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int test_flash_mem(void)
{
	/* USER CODE BEGIN 1 */
	QSPI_CommandTypeDef s_command;
	uint8_t pData[3];
	uint32_t i;

	ret = 100;

	printf("***************QuadSPi Example*******************************\r\n");
	BSP_QSPI_Init();
	
	/*##-2-Read Device ID Test    ###########################################*/
	/* Read Manufacture/Device ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_ID_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 2;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Device ID Test command");
	}
	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Device ID Test data");
	}
	printf("SPI  I/0 Read Device ID : 0x%2X 0x%2X\r\n", pData[0], pData[1]);
    

	/* Read Manufacture/Device ID Dual I/O*/
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = DUAL_READ_ID_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_2_LINES;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_2_LINES;
	s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytes    = 0;
	s_command.DataMode          = QSPI_DATA_2_LINES;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 2;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Manufacture/Device ID Dual I/O command");
	}
	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Manufacture/Device ID Dual I/O data");
	}
	printf("Dual I/O Read Device ID : 0x%2X 0x%2X\r\n", pData[0], pData[1]);

	/* Read Manufacture/Device ID Quad I/O*/
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = QUAD_READ_ID_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
	s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateBytes    = 0x00;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 4;
	s_command.NbData            = 2;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Manufacture/Device ID Quad I/O command");
	}
	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read Manufacture/Device ID Quad I/O data");
	}
	printf("Quad I/O Read Device ID : 0x%2X 0x%2X\r\n", pData[0], pData[1]);

	/* Read JEDEC ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_JEDEC_ID_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 3;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read JEDEC ID command");
	}
	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler((const char*)"Read JEDEC ID data");
	}
	printf("Read JEDEC ID :  0x%2X 0x%2X 0x%2X\r\n\r\n", pData[0], pData[1], pData[2]);

	/*##-3-QSPI Erase/Write/Read Test    ###########################################*/
	/* fill buffer */
	for (i = 0; i < 0x100; i++)
	{
		wData[i] = i;
		rData[i] = 0;
	}
//
//	if (BSP_QSPI_Erase_Block(0) == QSPI_OK)
//		printf(" QSPI Erase Block ok\r\n");
//	else
//		Error_Handler((const char*)"QSPI Erase Block");
//
//	if (BSP_QSPI_Write(wData, 0x00, 0x100) == QSPI_OK)
//		printf(" QSPI Write ok\r\n");
//	else
//		Error_Handler((const char*)"QSPI Write");
//
//	if (BSP_QSPI_Read(rData, 0x00, 0x100) == QSPI_OK)
//		printf(" QSPI Read ok\r\n\r\n");
//	else
//		Error_Handler((const char*)"QSPI Read");
//
//	printf("QSPI Read Data : \r\n");
//	for (i = 0; i < 0x100; i++)
//	{
//		printf("0x%02X  ", rData[i]);
//		//HAL_Delay(1);
//	}
//	printf("\r\n\r\n");
//
//	for (i = 0; i < 0x100; i++) 
//		if (rData[i] != wData[i])printf("0x%02X 0x%02X ", wData[i], rData[i]);
//	printf("\r\n\r\n");
//	/* check date */
//	//if (memcmp(wData, rData, 0x100) == 0)
//		printf(" W25Q128FV QuadSPI Test OK\r\n");
//	//else
//		printf(" W25Q128FV QuadSPI Test False\r\n");


	ret = 500;
}

