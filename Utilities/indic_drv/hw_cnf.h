/**
  ******************************************************************************
  * @file    hw_cnf.h
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    28-02-2021
  * @brief   Файл содержит функции аппаратных ресурсов индикации 
  ******************************************************************************
  * @attention
  * 
  * 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CNF_H
#define __HW_CNF_H

  /* Includes ------------------------------------------------------------------*/
#include "main.h"

#define I1_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()              	
#define I1_PIN            GPIO_PIN_15
#define I1_PORT           GPIOA	
#define I1_HI             I1_PORT->BSRR = I1_PIN
#define I1_LO             I1_PORT->BSRR = (uint32_t)I1_PIN << (16U);

#define I2_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()              	
#define I2_PIN            GPIO_PIN_10
#define I2_PORT           GPIOC	
#define I2_HI             I2_PORT->BSRR = I2_PIN
#define I2_LO             I2_PORT->BSRR = (uint32_t)I2_PIN << (16U);

#define I3_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()              	
#define I3_PIN            GPIO_PIN_11
#define I3_PORT           GPIOC	
#define I3_HI             I3_PORT->BSRR = I3_PIN
#define I3_LO             I3_PORT->BSRR = (uint32_t)I3_PIN << (16U);

#define I4_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define I4_PIN            GPIO_PIN_2
#define I4_PORT           GPIOD	
#define I4_HI             I4_PORT->BSRR = I4_PIN
#define I4_LO             I4_PORT->BSRR = (uint32_t)I4_PIN << (16U);

#define A_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define A_PIN            GPIO_PIN_1
#define A_PORT           GPIOD	
#define A_HI             A_PORT->BSRR = A_PIN
#define A_LO             A_PORT->BSRR = (uint32_t)A_PIN << (16U);

#define B_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()              	
#define B_PIN            GPIO_PIN_12
#define B_PORT           GPIOC	
#define B_HI             B_PORT->BSRR = B_PIN
#define B_LO             B_PORT->BSRR = (uint32_t)B_PIN << (16U);

#define C_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define C_PIN            GPIO_PIN_4
#define C_PORT           GPIOD	
#define C_HI             C_PORT->BSRR = C_PIN
#define C_LO             C_PORT->BSRR = (uint32_t)C_PIN << (16U);

#define D_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define D_PIN            GPIO_PIN_6
#define D_PORT           GPIOD	
#define D_HI             D_PORT->BSRR = D_PIN
#define D_LO             D_PORT->BSRR = (uint32_t)D_PIN << (16U);

#define E_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define E_PIN            GPIO_PIN_7
#define E_PORT           GPIOD	
#define E_HI             E_PORT->BSRR = E_PIN
#define E_LO             E_PORT->BSRR = (uint32_t)E_PIN << (16U);

#define F_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define F_PIN            GPIO_PIN_0
#define F_PORT           GPIOD	
#define F_HI             F_PORT->BSRR = F_PIN
#define F_LO             F_PORT->BSRR = (uint32_t)F_PIN << (16U);

#define G_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define G_PIN            GPIO_PIN_3
#define G_PORT           GPIOD	
#define G_HI             G_PORT->BSRR = G_PIN
#define G_LO             G_PORT->BSRR = (uint32_t)G_PIN << (16U);

#define DP_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define DP_PIN            GPIO_PIN_5
#define DP_PORT           GPIOD	
#define DP_HI             DP_PORT->BSRR = DP_PIN
#define DP_LO             DP_PORT->BSRR = (uint32_t)DP_PIN << (16U);

/**
  * @brief  Write the charater Indic.
  * @param  dgt the character that has to be displayed 
  * @retval None
  */
void Hw_Write_Sym(uint8_t dgt);

/**
  * @brief  Configures hardware Indic.
  * @param  None 
  * @retval None
  */
void Hw_Indic_Init(void);

#endif /* __HW_CNF_H */
/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/

