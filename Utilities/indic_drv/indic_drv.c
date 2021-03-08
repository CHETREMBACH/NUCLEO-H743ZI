/**
  ******************************************************************************
  * @file    indic_drv.c
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    28-02-2021
  * @brief   Файл содержит функции задачи динамической индикации 
  ******************************************************************************
  * @attention
  * 
  * 
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "indic_drv.h"

/**
  * @brief  indic task
  * @param  pvParameters not used
  * @retval None
  */
void indic_task(void * pvParameters)
{
	// Initialise the xLastWakeTime variable with the current time.
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 5;  	
	
	
	
	xLastWakeTime = xTaskGetTickCount();  
	for (;;)
	{
		// Wait for the next cycle.
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

/**
  * @brief  Configures Indic.
  * @param  None 
  * @retval None
  */
void Indic_Init(void)
{
	//GPIO_InitTypeDef GPIO_InitStructure;
  
	//RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
	//GPIO_InitStructure.GPIO_Pin = LED_PIN;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);	
	/* Запуск задачи контроля платы */
	xTaskCreate(indic_task, "INDIC", configMINIMAL_STACK_SIZE, NULL, TreadPrioNormal, NULL);	
}
/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/