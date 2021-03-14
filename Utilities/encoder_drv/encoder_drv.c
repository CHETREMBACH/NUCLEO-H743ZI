/**
  ******************************************************************************
  * @file    encoder_drv.c
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

/* Includes ------------------------------------------------------------------*/
#include "encoder_drv.h"
#include "pin_dbg.h"

Encoder_t henc;

/* Return with status macro */
#define RETURN_WITH_STATUS(p, s)    (p)->Rotation = s; return s

Encoder_Rotate_t En_Get(Encoder_t* data) {
	/* Calculate everything */
	data->Diff = data->EN_Count - data->Absolute;
	data->Absolute = data->EN_Count;
	
	/* Check */
	if (data->EN_Count < 0) {
		RETURN_WITH_STATUS(data, En_Rotate_Decrement);
	}
	else if (data->EN_Count > 0) {
		RETURN_WITH_STATUS(data, En_Rotate_Increment);
	}
	RETURN_WITH_STATUS(data, En_Rotate_Nothing);
}

int32_t En_GetData(void) {
	return henc.EN_Count;
}

void En_SetMode(Encoder_t* data, Encoder_Mode_t mode) {
	/* Set mode */
	data->Mode = mode;
}

void En_Process(Encoder_t* data) {
	uint8_t now_a;
	uint8_t now_b;
	
	/* Read inputs */
	now_a = HAL_GPIO_ReadPin(data->GPIO_A, data->GPIO_PIN_A);
	now_b = HAL_GPIO_ReadPin(data->GPIO_B, data->GPIO_PIN_B);
	
	/* Check difference */
	if (now_a != data->LastA) {
		data->LastA = now_a;
		
		if (data->LastA == 0) {
			/* Check mode */
			if (data->Mode == En_Mode_Zero) {
				if (now_b == 1) {
					if (data->EN_Count > -100)
					{
						data->EN_Count--;
					}
				}
				else {
					if (data->EN_Count < 100)
					{
						data->EN_Count++;
					}
				}
			}
			else {
				if (now_b == 1) {
					if (data->EN_Count < 100)
					{
						data->EN_Count++;
					}
				}
				else {
					if (data->EN_Count > -100)
					{
						data->EN_Count--;
					}
				}
			}
		}
	}
}

/**
  * @brief  This function handles external lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(henc.GPIO_PIN_A);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == henc.GPIO_PIN_A)
	{
		En_Process(&henc);
	}
}

/**
  * @brief  Configures Encoder_Init.
  * @param  None 
  * @retval None
  */
void Encoder_Init(void)
{
		
	GPIO_InitTypeDef   GPIO_InitStructure;

	/*  PC11  CKL */
	/*  PC10  DI  */
	/*  PC9  SW  */
	
	/* Enable GPIO clock */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	/* Configure PC.11 pin as input floating */
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Enable and set EXTI lines */
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 5);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	/* Save parameters */
	henc.GPIO_A = GPIOC;
	henc.GPIO_PIN_A = GPIO_PIN_11;
	henc.GPIO_B = GPIOC;
	henc.GPIO_PIN_B = GPIO_PIN_10;
	
	/* Set default mode */
	henc.Mode = En_Mode_Zero;
	/* Set default */
	henc.EN_Count = 0;
	henc.Diff = 0;
	henc.Absolute = 0;
	henc.LastA = 1;
}
/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/