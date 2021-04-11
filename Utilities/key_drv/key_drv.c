/**
  ******************************************************************************
  * @file    key_drv.с
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
#include "key_drv.h"


/* Определение кнопок */
key_control_t KeyMode;
key_control_t KeyStore;
key_control_t KeyEncode;

/* переменная взаимной блокировки  */
static uint8_t blok_id = 0x00;

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
void KeyInit(key_control_t* key_st, GPIO_TypeDef* GPIO_Port, uint32_t GPIO_Pin, uint16_t cnt_state_max, type_quick_tap_t  quick_tap, uint8_t key_id)
{
  key_st->GPIO_Port      = GPIO_Port;                     
  key_st->GPIO_Pin       = GPIO_Pin;               
  key_st->moment_state   = key_off;           
  key_st->old_state      = key_off;                 
  key_st->current_state  = key_off;         
  key_st->press_moment   = 0;           
  key_st->release_moment = 0;       
  key_st->cnt_state      = cnt_state_max;                 
  key_st->cnt_state_max  = cnt_state_max; 
  key_st->quick_tap      = quick_tap;
  key_st->key_id         = key_id;  
}  

/**
  * @brief  Функция контроля состояния кнопки / ввода
  * @param  key_control_t* указатель на структуру кнопки
  * @param  uint8_t remote_en включение удаленного контроля
  * @param  uint8_t remote_control состояние удаленного контроля
  * @retval None
  */
void KeyControl(key_control_t* key_st , uint8_t remote_en , uint8_t remote_control )
{
  /*  */
  if ( ( blok_id == 0x00 ) || ( key_st->key_id == blok_id ) || ( key_st->key_id == 0x00 ) )
  {
    /* Проверка включения удаленного управления */
    if (remote_en > 0)
    {
      /* Включено удаленное управление */
      if ( remote_control == 0 )  
      {
        /* Bit is HIGH */
        key_st->moment_state = key_off;
      }
      else
      {
        /* Bit is LOW */
        key_st->moment_state = key_on;  
      }      
    }
    else
    {
      /* Читаем мгновенное состояние ввода*/
      if ( HAL_GPIO_ReadPin(key_st->GPIO_Port, key_st->GPIO_Pin) != RESET )  
      {
        /* Bit is HIGH */
        key_st->moment_state = key_off;
      }
      else
      {
        /* Bit is LOW */
        key_st->moment_state = key_on;  
      }      
    }  
    
    /* сброс флагов изменения */
    key_st->press_moment   = 0;           
    key_st->release_moment = 0;    
    
    /* обновление времени с последнего изменения */
    if (key_st->time_state < 0xFFFF)
    { /* счетчик не заполнен - увеличиваем */ 
      key_st->time_state++;
    }
    
    /* сравниваем с текущим состоянием */
    if (key_st->moment_state == key_st->current_state)
    {
      /* если совпадает установить счетчик */
      key_st->cnt_state = key_st->cnt_state_max;
      
      /* отрабатываем режим быстрое нажатие */
      if ((key_st->quick_tap == on_tap)&&(key_st->current_state == key_on))
      { 
        if (key_st->time_state < 2000)
        {
          /* 2сек - пусто */
          key_st->cnt_quick_tap = 500;      
        }
        else
        {
          if (key_st->time_state < 5000)
          {/* 2 сек -  5 сек    D = 500 */
            if (key_st->cnt_quick_tap > 0)
            {
              (key_st->cnt_quick_tap)--;          
            }
            else
            {
              key_st->cnt_quick_tap = 500; 
              key_st->press_moment = 1; 
            }  
          }
          else
          {
            if (key_st->time_state < 8000)
            {/* 5 сек -  8 сек    D = 100 */
              if (key_st->cnt_quick_tap > 0)
              {
                (key_st->cnt_quick_tap)--;          
              }
              else
              {
                key_st->cnt_quick_tap = 100; 
                key_st->press_moment = 1;    
              }  
            }
            else
            {
              if (key_st->time_state < 11000)
              {/* 8 сек - 11 сек    D = 20  */
                if (key_st->cnt_quick_tap > 0)
                {
                  (key_st->cnt_quick_tap)--;          
                }
                else
                {
                  key_st->cnt_quick_tap = 20; 
                  key_st->press_moment = 1;    
                }  
              }
              else
              {/* 11 сек  и более   D = 4   */
                if (key_st->cnt_quick_tap > 0)
                {
                  (key_st->cnt_quick_tap)--;          
                }
                else
                {
                  key_st->cnt_quick_tap = 4; 
                  key_st->press_moment = 1;    
                }
              }
            }
          }
        }
      }  
    }
    else
    {
      /* если не совпадает уменьшение счетчика */
      if (key_st->cnt_state) 
      {/* уменьшение счетчика */
        key_st->cnt_state--;
      }
      else
      {/* счетчик исчерпан */
        
        /* обнуление времени с последнего изменения */
        key_st->time_state = 0;
        /* принимаем мгновенное значение как основное */
        key_st->current_state = key_st->moment_state;
        /* установка флагов изменения */
        if (key_st->current_state == key_on)
        {
          key_st->press_moment   = 1; 
          /* если кнопка участвует в взаимной блокировке - включаем блокировку */
          if (key_st->key_id != 0)
          {
            blok_id = key_st->key_id;           
          }
        }
        else
        {
          key_st->release_moment = 1;  
          /* если кнопка участвует в взаимной блокировке - оключаем блокировку */
          if (key_st->key_id != 0)
          {
            blok_id = 0x00;           
          }
        }  
        /* установить счетчик */
        key_st->cnt_state = key_st->cnt_state_max; 					
      }	
    } 
  }
  else
  {
    /* установить счетчик */
    key_st->cnt_state = key_st->cnt_state_max; 
  }  
}

/**
  * @brief  Функция инициализация кнопок  
  * @param  None
  * @retval None
  */
void Key_config(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	/* Инициализация кнопки KEY_MODE */
	KEY_MODE_CLK_ENABLE();
	
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = KEY_MODE_PIN;
	HAL_GPIO_Init(KEY_MODE_PORT, &GPIO_InitStructure);

	KeyInit(&KeyMode,	   /* указатель на структуру кнопки */
          KEY_MODE_PORT,   /* Указатель на порт подключения */   
          KEY_MODE_PIN,	   /* Ввод подключения              */   
          2,   		       /* максимально значение дредезга */ 
          off_tap,   	   /* состояния режима быстрого переключения */ 
          0x00);           /* идентификатор кнопки */

	/* Инициализация кнопки KEY_STORE */
	KEY_STORE_CLK_ENABLE();
	
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = KEY_STORE_PIN;
	HAL_GPIO_Init(KEY_STORE_PORT, &GPIO_InitStructure);	

	KeyInit(&KeyStore, 	   /* указатель на структуру кнопки */
          KEY_STORE_PORT,  /* Указатель на порт подключения */   
          KEY_STORE_PIN,   /* Ввод подключения              */   
          2,		       /* максимально значение дредезга */ 
          off_tap,		   /* состояния режима быстрого переключения */ 
          0x01);           /* идентификатор кнопки */ 

	/* Инициализация кнопки KEY_ENCODE */
	KEY_ENCODE_CLK_ENABLE();
	
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = KEY_ENCODE_PIN;
	HAL_GPIO_Init(KEY_ENCODE_PORT, &GPIO_InitStructure);	

	KeyInit(&KeyEncode,    /* указатель на структуру кнопки */
          KEY_ENCODE_PORT, /* Указатель на порт подключения */    
          KEY_ENCODE_PIN,  /* Ввод подключения              */    
          2,             /* максимально значение дредезга */ 
          off_tap,         /* состояния режима быстрого переключения */ 
          0x02);           /* идентификатор кнопки */
} 
/************************ COPYRIGHT 2020 OneTiOne *****END OF FILE****/