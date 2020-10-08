/**
  ******************************************************************************
  * @file    httpd_main.h
  * @version V1.0.0
  * @date    08-10-2020
  * @brief   Инициализация функций инициализации и поддержки httpd
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HTTPD_MAIN_H
#define __HTTPD_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

	
/**
  * @brief  Инициализация httpd.
  * @param  None
  * @retval None
  */
void httpd_main_setup(void);

#ifdef __cplusplus
}
#endif

#endif /* __HTTPD_MAIN_H */

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/
