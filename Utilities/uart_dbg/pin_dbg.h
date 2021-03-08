/**
  ******************************************************************************
  * @file    pin_dbg.h
  * @version V1.0.0
  * @date    07-09-2020
  * @brief   Инициализация драйвера GPIO для отладки
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PIN_DBG_H
#define __PIN_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

#if  (DBG_PIN_ENABLE == 1)

/* Includes ------------------------------------------------------------------*/
#include "main.h"	
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define T1_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define T1_PIN            GPIO_PIN_3
#define T1_PORT           GPIOD	
#define T1_HI             T1_PORT->BSRR = T1_PIN
#define T1_LO             T1_PORT->BSRR = (uint32_t)T1_PIN << (16U);
#define T1_TG             if ((T1_PORT->ODR & T1_PIN) == T1_PIN) {T1_HI;}else{T1_LO;};

#define T2_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define T2_PIN            GPIO_PIN_4
#define T2_PORT           GPIOD	
#define T2_HI             T2_PORT->BSRR = T2_PIN
#define T2_LO             T2_PORT->BSRR = (uint32_t)T2_PIN << (16U);
#define T2_TG             if ((T2_PORT->ODR & T2_PIN) == T2_PIN) {T2_HI;}else{T2_LO;};	

#define T3_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define T3_PIN            GPIO_PIN_5
#define T3_PORT           GPIOD	
#define T3_HI             T3_PORT->BSRR = T3_PIN
#define T3_LO             T3_PORT->BSRR = (uint32_t)T3_PIN << (16U);
#define T3_TG             if ((T3_PORT->ODR & T3_PIN) == T3_PIN) {T3_HI;}else{T3_LO;};
	
#define T4_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define T4_PIN            GPIO_PIN_6
#define T4_PORT           GPIOD	
#define T4_HI             T4_PORT->BSRR = T4_PIN
#define T4_LO             T4_PORT->BSRR = (uint32_t)T4_PIN << (16U);
#define T4_TG             if ((T4_PORT->ODR & T4_PIN) == T4_PIN) {T4_HI;}else{T4_LO;};	
	
	
/**
  * @brief  Инициализация GPIO для отладки.
  * @param  None
  * @retval None
  */
void DBG_PIN_Setup(void);

#endif /*  (DBG_PIN_ENABLE == 1) */

#ifdef __cplusplus
}
#endif

#endif /* __PIN_DBG_H */

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/
