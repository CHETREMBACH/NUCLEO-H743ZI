/**
  ******************************************************************************
  * File Name          : QUADSPI.c
  * Description        : This file provides code for the configuration
  *                      of the QUADSPI instances.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "quadspi.h"
#include "w25q128fv.h"
#include "stm32746g_qspi.h"

QSPI_HandleTypeDef hqspi;
uint16_t ret;

uint8_t wData[0x100];
uint8_t rData[0x100];

/* QUADSPI init function */
void MX_QUADSPI_Init(void)
{
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 8;
  hqspi.Init.FifoThreshold = 4;
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
}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{
	
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(qspiHandle->Instance==QUADSPI)
  {

	  __HAL_RCC_QSPI_CLK_ENABLE();
  
	  __HAL_RCC_GPIOE_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  /**QUADSPI GPIO Configuration    
	  PE2     ------> QUADSPI_BK1_IO2
	  PB2     ------> QUADSPI_CLK
	  PD11     ------> QUADSPI_BK1_IO0
	  PD12     ------> QUADSPI_BK1_IO1
	  PD13     ------> QUADSPI_BK1_IO3
	  PB6     ------> QUADSPI_BK1_NCS 
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

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();
  
	  /**QUADSPI GPIO Configuration    
	  PE2     ------> QUADSPI_BK1_IO2
	  PB2     ------> QUADSPI_CLK
	  PD11     ------> QUADSPI_BK1_IO0
	  PD12     ------> QUADSPI_BK1_IO1
	  PD13     ------> QUADSPI_BK1_IO3
	  PB6     ------> QUADSPI_BK1_NCS 
	  */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2|GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13);
  }
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

	if (BSP_QSPI_Erase_Block(0) == QSPI_OK)
		printf(" QSPI Erase Block ok\r\n");
	else
		Error_Handler((const char*)"QSPI Erase Block");

	if (BSP_QSPI_Write(wData, 0x00, 0x100) == QSPI_OK)
		printf(" QSPI Write ok\r\n");
	else
		Error_Handler((const char*)"QSPI Write");

	if (BSP_QSPI_Read(rData, 0x00, 0x100) == QSPI_OK)
		printf(" QSPI Read ok\r\n\r\n");
	else
		Error_Handler((const char*)"QSPI Read");

	printf("QSPI Read Data : \r\n");
	for (i = 0; i < 0x100; i++)
	{
		printf("0x%02X  ", rData[i]);
		//HAL_Delay(1);
	}
	printf("\r\n\r\n");

	for (i = 0; i < 0x100; i++)
		if (rData[i] != wData[i])printf("0x%02X 0x%02X ", wData[i], rData[i]);
	printf("\r\n\r\n");
	/* check date */
	//if (memcmp(wData, rData, 0x100) == 0)
		printf(" W25Q128FV QuadSPI Test OK\r\n");
	//else
		printf(" W25Q128FV QuadSPI Test False\r\n");


	ret = 500;
}