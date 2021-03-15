/**
  ******************************************************************************
  * @file    fsm_control.h
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    15-03-2021
  * @brief   Файл содержит функции основного автомата состояний 
  *
  *
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2021 OneTiOne < / center>< / h2>
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FSM_CONTROL_H
#define __FSM_CONTROL_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

///**
//  * @brief  FSM MAIN ENUMERATE.
//  */
//typedef enum {
//  START_DISP = 0,         /* */ 
//  RX_DISP,                /* */ 
//  RX_TX_STEP_A,           /* */ 
//  RX_TX_STEP_B,           /* */ 
//  RX_TX_STEP_C,           /* */ 
//  TX_DISP,                /* */ 
//  TX_RX_STEP_A,           /* */ 
//  TX_RX_STEP_B,           /* */    
//  ENABLE_CONFIG_DISP,     /* */
//  CONFIG_DISP,            /* */
//  EDIT_VAR,               /* */
//  STORE_VAR_DISP,         /* */
//  ALARM_DISP              /* */
//}FSM_MAIN_TypeDef;  

/* структура контроля кнопки */
typedef struct
{
	union {
		uint32_t  stored_param; /* переменная для сохранения параметров  */
		struct
		{ 
			uint16_t          mode_state;   /* режим работы          */
			int16_t           encode_state; /* состояние encodera       */  
		};
	};
}param_cntrl_t;	

/**
  * @brief  Инициализация и запуск задачи основного автомата состояний.
  * @param  None 
  * @retval None
  */
void InitFsmTask(void);

#endif /* __FSM_CONTROL_H */
/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/
