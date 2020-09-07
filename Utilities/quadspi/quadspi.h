/**
  ******************************************************************************
  * File Name          : QUADSPI.h
  * Description        : This file provides code for the configuration
  *                      of the QUADSPI instances.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __quadspi_H
#define __quadspi_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern QSPI_HandleTypeDef hqspi;
void MX_QUADSPI_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int test_flash_mem(void);	 
	 
#ifdef __cplusplus
}
#endif
#endif /*__ quadspi_H */

/**
  * @}
  */

/**
  * @}
  */
