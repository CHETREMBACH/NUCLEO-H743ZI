/**
  ******************************************************************************
  * @file    pin_dbg.c
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

/* Includes ------------------------------------------------------------------*/
#include "pin_dbg.h"

#if  (DBG_PIN_ENABLE == 1)
/**
  * @brief  Инициализация GPIO для отладки.
  * @param  None
  * @retval None
  */
void DBG_PIN_Setup(void)
{

	GPIO_InitTypeDef  gpio_init_structure;
	
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

	/* Configure the GPIO T1 pin   */
	T1_GPIO_CLK_ENABLE;	
	gpio_init_structure.Pin = T1_PIN;
	HAL_GPIO_Init(T1_PORT, &gpio_init_structure);
	HAL_GPIO_WritePin(T1_PORT, T1_PIN, GPIO_PIN_RESET);
	
	/* Configure the GPIO T1 pin   */
	T2_GPIO_CLK_ENABLE;	
	gpio_init_structure.Pin = T2_PIN;
	HAL_GPIO_Init(T2_PORT, &gpio_init_structure);
	HAL_GPIO_WritePin(T2_PORT, T2_PIN, GPIO_PIN_RESET);	
}

#endif  /* (DBG_PIN_ENABLE == 1) */
/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/
