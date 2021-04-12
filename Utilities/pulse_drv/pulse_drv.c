/**
  ******************************************************************************
  * @file    pulse_drv.c
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

/* Includes ------------------------------------------------------------------*/
#include "pulse_drv.h"
#include "pin_dbg.h"

/* Timer handler declaration */
TIM_HandleTypeDef    TimHandle;
/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;
/* Timer Break Configuration Structure declaration */
TIM_BreakDeadTimeConfigTypeDef sBreakConfig;

int32_t time_offset = 0;
uint32_t start_time = 400;	
uint32_t time_pulse	= 750;
uint32_t time_period = 2250;
int32_t offset_control = 0;

extern int32_t GetDeltaTime(void);
extern uint8_t GetStateModePWM(void);

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{   
	int32_t temp_data = GetDeltaTime();
	/* TIM Update event */
	if (__HAL_TIM_GET_IT_SOURCE(&TimHandle, TIM_IT_UPDATE) != RESET)
	{
		__HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_UPDATE);

			if (GetStateModePWM() > 0)
			{
				TIM2->CCR1 = (start_time * 200) - 1;
				TIM2->CCR2 = ((start_time + time_pulse) * 200) - 1;
				TIM2->CCR3 = (((start_time + time_pulse) * 200) - 1) + temp_data;
				TIM2->CCR4 = (((start_time + time_pulse + time_pulse) * 200)) - 1 + temp_data;
				GPIOG->BSRR = GPIO_PIN_11;
			}
			else
			{	
				TIM2->CCR1 = (time_period * 200) - 1;
				TIM2->CCR2 = (time_period * 200) - 1;
				TIM2->CCR3 = (time_period * 200) - 1;
				TIM2->CCR4 = (time_period * 200) - 1;
				GPIOG->BSRR = (uint32_t)GPIO_PIN_11 << (16U);				
			}
	}
}


/**
  * @brief  Configures GPIO / Timer.
  * @param  None 
  * @retval None
  */
void Pulse_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	/* Enable Timer Clocks */
	__HAL_RCC_TIM2_CLK_ENABLE();

	/* Common configuration for all channels */
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	/* PA0 TM2_CH1 */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* PB3 TM2_CH2 */
	//__HAL_RCC_GPIOB_CLK_ENABLE();
	//GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	//GPIO_InitStruct.Pin = GPIO_PIN_3;
	//HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* PB10 TM2_CH3 */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* PB11 TM2_CH4 */
	//__HAL_RCC_GPIOB_CLK_ENABLE();
	//GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	//GPIO_InitStruct.Pin = GPIO_PIN_11;
	//HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*##-2- Configure the NVIC for TIMx #########################################*/
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 1);
	/* Enable the TIMx global Interrupt */
	HAL_NVIC_EnableIRQ(TIM2_IRQn);	
	
	/* MODE LED */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
    GPIOG->BSRR = GPIO_PIN_11;

	/*##-1- Configure the TIM peripheral #######################################*/
	/* TIM1 Configuration: generate 4 PWM signals with 4 different duty cycles. */
    /* Initialize TIMx peripheral as follows:
	        + Prescaler = (SystemCoreClock / (2*20000000)) - 1
	        + Period = (1000 - 1)
	        + ClockDivision = 0
	        + Counter direction = Up  */
	TimHandle.Instance = TIM2;
	TimHandle.Init.Prescaler         = 0;
	TimHandle.Init.Period            = (time_period * 200) - 1;
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimHandle.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}
	
	/*##-2- Configure the PWM channels #########################################*/
	/* Common configuration for all channels */
	sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

	/* Set the pulse value for channel 1 */	
	sConfig.OCMode       = TIM_OCMODE_COMBINED_PWM2;
	sConfig.Pulse = (time_period * 200) - 1; //(start_time * 200) - 1;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}

	/* Set the pulse value for channel 2 */
	sConfig.OCMode       = TIM_OCMODE_ASSYMETRIC_PWM1;
	sConfig.Pulse = (time_period * 200) - 1; //((start_time + time_pulse) * 200) - 1;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}

	/* Set the pulse value for channel 3 */
	sConfig.OCMode       = TIM_OCMODE_COMBINED_PWM2;
	sConfig.Pulse = (time_period * 200) - 1; //((start_time + time_pulse) * 200) - 1;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}

	/* Set the pulse value for channel 4 */
	sConfig.OCMode       = TIM_OCMODE_ASSYMETRIC_PWM1;
	sConfig.Pulse = (time_period * 200) - 1;//((start_time + time_pulse + time_pulse) * 200) - 1;
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}	
		
	/*##-3- Start PWM signals generation #######################################*/
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
	/* Start channel 2 */
	if (HAL_TIM_PWM_Stop(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
	/* Start channel 3 */
	if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
	{
		/* PWM generation Error */
		Error_Handler();
	}
	/* Start channel 4 */
	if (HAL_TIM_PWM_Stop(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
	{
		/* PWM generation Error */
		Error_Handler();
	}

	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
    if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
	{
		/* Starting Error */
		Error_Handler();
	}		
	
}
/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/