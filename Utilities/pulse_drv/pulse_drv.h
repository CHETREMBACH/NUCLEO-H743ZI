/**
  ******************************************************************************
  * @file    pulse_drv.h
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    27-02-2021
  * @brief   Файл содержит функции настройки и контроля формирования импульсов таймером
  ******************************************************************************
  * @attention
  * 
  * 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PULSE_DRV_H
#define __PULSE_DRV_H

  /* Includes ------------------------------------------------------------------*/
#include "main.h"
  
#define LED_PIN     GPIO_PIN_11
#define LED_PORT    GPIOA
#define LED_CLK_EN 	__HAL_RCC_GPIOA_CLK_ENABLE()
#define LED_ON      LED_PORT->BSRR = LED_PIN
#define LED_OFF     LED_PORT->BSRR = (uint32_t)LED_PIN << (16U)

#define ENA_PIN     GPIO_PIN_12
#define ENA_PORT    GPIOB
#define ENA_CLK_EN 	__HAL_RCC_GPIOB_CLK_ENABLE()
#define ENA_ON      ENA_PORT->BSRR = ENA_PIN
#define ENA_OFF     ENA_PORT->BSRR = (uint32_t)ENA_PIN << (16U)

#define ENB_PIN     GPIO_PIN_13
#define ENB_PORT    GPIOB
#define ENB_CLK_EN 	__HAL_RCC_GPIOB_CLK_ENABLE()
#define ENB_ON      ENB_PORT->BSRR = ENB_PIN
#define ENB_OFF     ENB_PORT->BSRR = (uint32_t)ENB_PIN << (16U)



/**
  * @brief  Configures GPIO / Timer.
  * @param  None 
  * @retval None
  */
void Pulse_Init(void);

#endif /* __PULSE_DRV_H */
/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/

