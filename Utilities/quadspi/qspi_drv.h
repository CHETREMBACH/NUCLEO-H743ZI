/**
  ******************************************************************************
  * File Name          : qspi_drv.h
  * Description        : This file provides code for the driver QUADSPI Flash.
  * 
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __QSPI_DRV_H
#define __QSPI_DRV_H

#include "main.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
extern QSPI_HandleTypeDef hqspi;
void QUADSPI_Init(void);	 
	 
/**
  * @brief  QSPI_Init - инициализация интерфейса
  * @param  None
  * @retval None
  */
void QSPI_Init(QSPI_HandleTypeDef* qspiHandle);
	
/**
  * QUADSPI init function после старта задачи
  * @param  None
  * @retval None
  */
void QUADSPI_Task_Init(void);	
	

/**
  * @brief  The application entry point.
  * @retval None
  */
void test_cmd(void);	 
	 
#ifdef __cplusplus
}
#endif

#endif /* __QSPI_FLASH_H */