/**
  ******************************************************************************
  * @file    fsm_control.c
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

/* Includes ------------------------------------------------------------------*/
#include "fsm_control.h"
#include "indic_drv.h"
#include "led_7seg.h"
#include "encoder_drv.h"
#include "key_drv.h"
#include "flash_interface.h"

SevenSeg IndicStr;
TimerHandle_t SoftTimer; /*  Програмный таймер  */
TimerHandle_t IndicTimer; /*  Програмный таймер  */
TaskHandle_t  HandleTask; /*   */
static uint32_t NotifiedValue; /*                  */

char buf_string[20] = { 0 };

#define  TIMER_NOTE           (1l<<0)  /* Периодическое уведомление по таймеру      */
#define  INDIC_NOTE           (1l<<1)  /* Получение из пакета данных                */

extern Encoder_t henc;

static param_cntrl_t  state_new;
static param_cntrl_t  state_work;
static param_cntrl_t  state_store;


uint16_t Flag_mode = 0;



uint8_t GetStateModePWM(void)
{
	if (state_work.mode_state > 0) return 1;
	else return 0; 
}

/**
  * @brief  Задача основного автомата состояний.
  * @param  pvParameters not used
  * @retval None
  */
void FsmTask(void * pvParameters)
{
	for(;  ;)
	{
		/* Обнуляем сообщение */
		NotifiedValue = 0;
		/*  Проверка наличия сообщений  */
		xTaskNotifyWait(0x00000000,
			/* Не очищайте биты уведомлений при входе               */
			0xFFFFFFFF,
			/* Сбросить значение уведомления до 0 при выходе        */
			&(NotifiedValue),
			/* Значение уведомленное передается в  NotifiedValue    */
			portMAX_DELAY); /* Блокировка задачи до появления уведомления           */
    
    
        /* Получено уведомление. Проверяем, какие биты были установлены. */
		if (((NotifiedValue) & TIMER_NOTE) != 0)
		{  
			/*========================== обновление состояния кнопок ==============================*/				
			KeyControl(&KeyMode , 0, 0); /* Контроль кнопок MODE   */              
			KeyControl(&KeyStore, 0, 0);  /* Контроль кнопок STORE  */               
			KeyControl(&KeyEncode, 0, 0); /* Контроль кнопок ENCODE */ 
			state_new.encode_state = henc.EN_Count;
			/*========================== отработка автомата состояний =========================*/				
			
			/* Обработка кнопки включения / отключения генерации */
			if (KeyMode.press_moment == true)
			{
				if (state_new.mode_state > 0) 	state_new.mode_state = 0;
				else  state_new.mode_state = 1;
				state_work.mode_state   = state_new.mode_state;
			}
			
			/* Проверка на сохранение параметров в EEPROM */
			if ( ( KeyStore.press_moment  == true) || ( KeyEncode.release_moment  == true ))
			{
				//обновление данных режима генератора
				state_work.encode_state = state_new.encode_state;
				state_work.mode_state   = state_new.mode_state;
	
				state_store.encode_state = state_new.encode_state;
				state_store.mode_state   = state_new.mode_state;	
				
				StoreEmuleEEPROM(5234, state_store.stored_param);	
				state_store.stored_param = LoadEmuleEEPROM(5234); 	
			}
				
			/* Контроль режима реал тайм */
			if (KeyEncode.current_state == key_on)
			{
				/* Включен режим установки данных с енкодера в реальном времени */
				state_work.encode_state = state_new.encode_state;
				state_work.mode_state   = state_new.mode_state;
			}
									
			/*========================== обновления состояния индикации =========================*/	
			
			if (henc.EN_Count >= 0)
			{
				sprintf(buf_string, "%.5d", (state_new.encode_state) * 5);
				buf_string[1] = ' ';
			}
			else
			{
				sprintf(buf_string, "%.5d", 0 - ((state_new.encode_state) * 5));
				buf_string[1] = '-';
			}
			
			if ((state_store.mode_state != state_new.mode_state) || (state_store.encode_state != state_new.encode_state)) 
			{
				buf_string[5] = '.';
			}
			else
			{
				buf_string[5] = 0;
			}
			
			if ((state_store.mode_state != state_new.mode_state) || (state_store.encode_state != state_new.encode_state)) 
			{
				buf_string[5] = '.';
			}
			else
			{
				buf_string[5] = 0;
			}
			
			sevenseg_unlock(&IndicStr);
			
			if (state_work.encode_state != state_new.encode_state)
			{
				sevenseg_setBlink(&IndicStr, true, 0);
			}
			else
			{
				sevenseg_setBlink(&IndicStr, false, 0);
			}

			sevenseg_print(&IndicStr, &(buf_string[1]));
			sevenseg_lock(&IndicStr);
			/*===============================================================================*/			
		}    
		
		/* Получено уведомление. Проверяем, какие биты были установлены. */
		if (((NotifiedValue) & INDIC_NOTE) != 0)
		{  
			sseg_refresh(&IndicStr); 
		}   		
	} 	
}

/**
  * @brief  Функция отработки програмного таймера 
  * @param  TimerHandle_t pxTimer - указатель на таймер вызвавщий функцию
  * @retval None
  */
void SoftTimerCallback(TimerHandle_t pxTimer)
{
	/* Функция обработки програмного таймера.*/
	if ((HandleTask) != NULL)
	{
		if (SoftTimer == pxTimer)
		{
			/* Таймер контроля обновления данных */
			xTaskNotify( HandleTask,
				/* Указатель на уведомлюемую задачу                         */
				TIMER_NOTE,
				/* Значения уведомления                                     */
				eSetBits); /* Текущее уведомление добавляются к уже прописанному       */
		}
		
		if (IndicTimer == pxTimer)
		{
			/* Таймер контроля динамической индикации */
			xTaskNotify( HandleTask,
				/* Указатель на уведомлюемую задачу                         */
				INDIC_NOTE,
				/* Значения уведомления                                     */
				eSetBits); /* Текущее уведомление добавляются к уже прописанному       */
		}		
	}  
}

int32_t GetDeltaTime(void)
{
	return (int32_t)state_work.encode_state;
}

/**
  * @brief  Инициализация и запуск задачи основного автомата состояний.
  * @param  None 
  * @retval None
  */
void InitFsmTask(void)
{
	
	Init_Emul_EEPROM();
	
    /* Инициализация переменных */
	state_store.stored_param = LoadEmuleEEPROM(5234); 
	
	/* initialize the structure for the display */
	sevenseg_init(&IndicStr);
	/* Функция инициализация кнопок */
	Key_config();
	/* init encoder */
	Encoder_Init();		
	
	if ((state_store.encode_state > 100) || (state_store.encode_state < -100))
	{
		state_store.encode_state = 0;
		state_store.mode_state   = 0;	
		StoreEmuleEEPROM(5234, state_store.stored_param);	
		state_store.stored_param = LoadEmuleEEPROM(5234); 		
	}
		
	state_new.encode_state = state_store.encode_state;
	state_new.mode_state   = state_store.mode_state;
	
	state_work.encode_state = state_store.encode_state;
	state_work.mode_state   = state_store.mode_state;	
	
	/* Предустановка Encodera */
	henc.EN_Count = state_new.encode_state;
	
		
	/* Открытие таймер контроля  */
	SoftTimer = xTimerCreate( "TmSf",
		/* Текстовое имя, не используется в RTOS kernel. */
		50,
		/* Период таймера в тиках. */
		pdTRUE,
		/* Время будет автоматически перезагружать себя, когда оно истечет. */
		NULL,
		/* В функцию параметры не передаем */
		SoftTimerCallback); /* Указатель на функцию , которую вызывает таймер. */  
	
	xTimerStart(SoftTimer, 0);   	
	
	
	/* Открытие таймер контроля  */
	IndicTimer = xTimerCreate( "TmInd",
		/* Текстовое имя, не используется в RTOS kernel. */
		CONFIG_LED_7SEG_RTIME,
		/* Период таймера в тиках. */
		pdTRUE,
		/* Время будет автоматически перезагружать себя, когда оно истечет. */
		NULL,
		/* В функцию параметры не передаем */
		SoftTimerCallback); /* Указатель на функцию , которую вызывает таймер. */  
	
	xTimerStart(IndicTimer, 0);	
	
	/* Запуск задачи контроля платы */
	xTaskCreate(FsmTask, "FSM", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioNormal, &HandleTask);	
}
/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/