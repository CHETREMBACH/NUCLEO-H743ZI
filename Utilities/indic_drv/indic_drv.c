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
#include "led_7seg.h"
#include "encoder_drv.h"

SevenSeg IndicStr;
TimerHandle_t SoftTimer;  /*  Програмный таймер  */
TimerHandle_t IndicTimer; /*  Програмный таймер  */
TaskHandle_t  HandleTask; /*   */
static uint32_t NotifiedValue; /*                  */

char buf_string[20] = { 0 };

#define  TIMER_NOTE           (1l<<0)  /* Периодическое уведомление по таймеру      */
#define  INDIC_NOTE           (1l<<1)  /* Получение из пакета данных                */

extern Encoder_t henc;


/**
  * @brief  indic task
  * @param  pvParameters not used
  * @retval None
  */
void indic_task(void * pvParameters)
{
	/* initialize the structure for the display */
	sevenseg_init(&IndicStr);
	
	//sevenseg_unlock(&IndicStr);
	//sevenseg_print(&IndicStr, "-067");
	//sevenseg_setBlink(&IndicStr, true, 0);
	//sevenseg_lock(&IndicStr);	

	for (;;)
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
			if (henc.EN_Count >= 0)
			{
				sprintf(buf_string, "%.5ld", henc.EN_Count * 5);
				buf_string[1] = ' ';
			}
			else
			{
				sprintf(buf_string, "%.5ld", 0 - (henc.EN_Count * 5));
				buf_string[1] = '-';
			}
			
			sevenseg_unlock(&IndicStr);
			sevenseg_print(&IndicStr, &(buf_string[1]));
			sevenseg_lock(&IndicStr);
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
			/* Таймер контроля внешнего сторожевого таймера */
			xTaskNotify( HandleTask,
			/* Указатель на уведомлюемую задачу                         */
			TIMER_NOTE,
			/* Значения уведомления                                     */
			eSetBits); /* Текущее уведомление добавляются к уже прописанному       */
		}
		
		if (IndicTimer == pxTimer)
		{
			/* Таймер контроля внешнего сторожевого таймера */
			xTaskNotify( HandleTask,
			/* Указатель на уведомлюемую задачу                         */
			INDIC_NOTE,
			/* Значения уведомления                                     */
			eSetBits); /* Текущее уведомление добавляются к уже прописанному       */
		}		
	}  
}

/**
  * @brief  Configures Indic.
  * @param  None 
  * @retval None
  */
void Indic_Init(void)
{
	/* Открытие таймер контроля  */
	SoftTimer = xTimerCreate( "TmSf",
		/* Текстовое имя, не используется в RTOS kernel. */
		200,
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
	xTaskCreate(indic_task, "INDIC", configMINIMAL_STACK_SIZE*5, NULL, TreadPrioNormal, &HandleTask);	
}
/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/