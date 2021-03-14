/**
  ******************************************************************************
  * @file    pulse_drv.h
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    8-03-2021
  * @brief   Файл содержит функции драйвера encoder 
  ******************************************************************************
  * @attention
  * 
  * 
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODER_DRV_H
#define __ENCODER_DRV_H

  /* Includes ------------------------------------------------------------------*/
#include "main.h"
  

/**
 * @brief  Rotary encoder rotation status
 */
typedef enum {
	En_Rotate_Increment,
	/*!< Encoder was incremented */
	En_Rotate_Decrement,
	/*!< Encoder was decremented */
	En_Rotate_Nothing    /*!< Encoder stop at it was before */
}Encoder_Rotate_t;

/**
 * @brief  Rotary encoder mode selection for rotation
 */
typedef enum {
	En_Mode_Zero,
	/*!< Rotary encoder mode zero. It is used for direction when it will be increment od decrement, default used */
	En_Mode_One   /*!< Rotary encoder mode one. It is used for direction when it will be increment od decrement */
} Encoder_Mode_t;

/**
 * @brief  Rotary main working structure 
 */
typedef struct {
	int32_t Absolute; /*!< Absolute rotation from beginning, for public use */
	int32_t Diff; /*!< Rotary difference from last check, for public use */
	Encoder_Rotate_t Rotation; /*!< Increment, Decrement or nothing, for public use */
	Encoder_Mode_t Mode; /*!< Rotary encoder mode selected */
	uint8_t LastA; /*!< Last status of A pin when checking. Meant for private use */
	int32_t EN_Count; /*!< Temporary variable to store data between rotation and user check */
	GPIO_TypeDef* GPIO_A; /*!< Pointer to GPIOx for Rotary encode A pin. Meant for private use */
	GPIO_TypeDef* GPIO_B; /*!< Pointer to GPIOx for Rotary encode B pin. Meant for private use */
	uint32_t  GPIO_PIN_A; /*!< GPIO pin for rotary encoder A pin. This pin is also set for interrupt */
	uint32_t  GPIO_PIN_B; /*!< GPIO pin for rotary encoder B pin. */
}Encoder_t;


int32_t En_GetData(void);

/**
  * @brief  Configures Encoder_Init.
  * @param  None 
  * @retval None
  */
void Encoder_Init(void);

#endif /* __ENCODER_DRV_H */
/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/

