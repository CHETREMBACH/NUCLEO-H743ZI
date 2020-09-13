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
#include "stm32h7xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define T1_GPIO_CLK_ENABLE	 __HAL_RCC_GPIOG_CLK_ENABLE()
#define T1_PIN               GPIO_PIN_0
#define T1_PORT              GPIOG		
#define T1_HI                T1_PORT->BSRR = T1_PIN	
#define T1_LO                T1_PORT->BSRR = (uint32_t)T1_PIN << 16		
#define T1_TG                if ((T1_PORT->ODR & T1_PIN) == T1_PIN) T1_LO; else T1_HI
	
#define T2_GPIO_CLK_ENABLE	 __HAL_RCC_GPIOG_CLK_ENABLE()
#define T2_PIN               GPIO_PIN_1
#define T2_PORT              GPIOG
#define T2_HI                T2_PORT->BSRR = T2_PIN	
#define T2_LO                T2_PORT->BSRR = (uint32_t)T2_PIN << 16	
#define T2_TG                if ((T2_PORT->ODR & T2_PIN) == T2_PIN) T2_LO; else T2_HI
	
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
