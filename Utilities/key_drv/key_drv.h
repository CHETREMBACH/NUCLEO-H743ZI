/**
  ******************************************************************************
  * @file    key_drv.h
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    15-03-2021
  * @brief   Файл содержит функции для инициализации и контроля клавиш/кнопок   
  *
  *
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2021 OneTiOne < / center>< / h2>
  ******************************************************************************
  */

#ifndef _KEY_DRV_H
#define _KEY_DRV_H 

#include "main.h"

/* описание вводов */
#define KEY_STORE_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define KEY_STORE_PIN                GPIO_PIN_0
#define KEY_STORE_PORT               GPIOD

#define KEY_MODE_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()              	
#define KEY_MODE_PIN                  GPIO_PIN_1
#define KEY_MODE_PORT                 GPIOD

#define KEY_ENCODE_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()              	
#define KEY_ENCODE_PIN                GPIO_PIN_9
#define KEY_ENCODE_PORT               GPIOC

/* перечисление состояния кнопки */
typedef enum 
{
  key_on = 0, /* генерируется программой */
  key_off
}type_key_state_t;		

/* перечисление состояния режима быстрого переключения  */
typedef enum 
{
  off_tap = 0, /*  */
  on_tap
}type_quick_tap_t;	


/* структура контроля кнопки */
typedef struct
{
  GPIO_TypeDef*     GPIO_Port;          /* Указатель на порт подключения        */
  uint32_t          GPIO_Pin;           /* Ввод подключения                     */
  type_key_state_t  moment_state;       /* мгновенное состояние                 */
  type_key_state_t  old_state;          /* предидущее состояние                 */
  type_key_state_t  current_state;      /* текущее состояние                    */
  uint8_t           press_moment;       /* нажатие                              */
  uint8_t           release_moment;     /* отпускание                           */  
  uint16_t          time_state;         /* время с изменения состояния          */
  uint16_t          cnt_state;          /* счетчик переключения состояния       */  
  uint16_t          cnt_state_max;      /* максимально значение дредезга        */
  type_quick_tap_t  quick_tap;          /* флаг включения режима быстрого переключения */
  uint8_t           key_id;             /* идентификатор кнопки                 */
  uint16_t          cnt_quick_tap;      /* счетчик режима быстрого переключения */  
}key_control_t;	

/* Объявление вводов */

extern key_control_t KeyMode;
extern key_control_t KeyStore;
extern key_control_t KeyEncode;

/**
  * @brief  Функция иницмализации кнопки / ввода
  * @param  key_control_t*    key_st              указатель на структуру кнопки
  * @param  GPIO_TypeDef*     GPIO_Port           Указатель на порт подключения        
  * @param  uint32_t          GPIO_Pin            Ввод подключения                     
  * @param  uint16_t          cnt_state_max       максимально значение дредезга
  * @param  type_quick_tap_t  quick_tap           флаг включения режима быстрого переключения 
  * @param  uint8_t           key_id              идентификатор кнопки 
  * @retval None
  */
void KeyInit(key_control_t* key_st, GPIO_TypeDef* GPIO_Port, uint32_t GPIO_Pin, uint16_t cnt_state_max, type_quick_tap_t  quick_tap, uint8_t key_id);

/**
  * @brief  Функция контроля состояния кнопки / ввода
  * @param  key_control_t* указатель на структуру кнопки
  * @param  uint8_t remote_en включение удаленного контроля
  * @param  uint8_t remote_control состояние удаленного контроля
  * @retval None
  */
void KeyControl(key_control_t* key_st , uint8_t remote_en , uint8_t remote_control );

/**
  * @brief  Функция инициализация кнопок  
  * @param  None
  * @retval None
  */
void Key_config(void);

#endif /* _KEY_DRV_H */
/************************ COPYRIGHT 2020 OneTiOne *****END OF FILE****/