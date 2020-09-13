/**
  ******************************************************************************
  * File Name          : qspi_flash.c
  * Description        : This file provides code for the driver QUADSPI Flash.
  * 
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "qspi_flash.h"
#include "qspi_drv.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "main.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint32_t    qFlashNotifiedValue;     
TaskHandle_t       qFlashHandleTask;        
TimerHandle_t      qFlashxSoftTimer;            /*  Програмный таймер периодического контроля     */
TimerHandle_t      qFlashxUpdateTimer;          /*  Програмный таймер контроля ожидания ресурсов  */

/**
  * @brief  QSPI_FLASH control - вызывается с частотой 10 Гц
  * @param  None
  * @retval None
  */
void QSPI_FLASH_Cntrl(void)
{
	test_cmd();
}

/**
  * @brief  Функция отработки програмного таймера 
  * @param  TimerHandle_t pxTimer - указатель на таймер вызвавщий функцию
  * @retval None
  */
void qFlashTimCallback(TimerHandle_t pxTimer)
{
	if ((qFlashHandleTask) != NULL)
	{
		if (qFlashxSoftTimer == pxTimer)
		{/* Програмный таймер периодического контроля */
			xTaskNotify( qFlashHandleTask,				/* Указатель на уведомлюемую задачу                         */
					  QFLASH_NOTE_SOFT_TIM,				/* Значения уведомления                                     */
					  eSetBits );                       /* Текущее уведомление добавляются к уже прописанному       */
		}
    
		if (qFlashxUpdateTimer == pxTimer)
		{/* Таймер контроля ожидания ресурсов */
			xTaskNotify( qFlashHandleTask,				/* Указатель на уведомлюемую задачу                         */
					  QFLASH_NOTE_DELAY_TIM,            /* Значения уведомления                                     */
					  eSetBits );                       /* Текущее уведомление добавляются к уже прописанному       */
		}  
	}  
}

/**
  * @brief  задача сторожевого таймера
  * @param  pvParameters not used
  * @retval None
  */
void qFlash_Task(void * pvParameters)
{  
	/* QUADSPI init function после старта задачи*/
	QUADSPI_Task_Init();

	
	/* Открытие таймера периодического контроля  */
	qFlashxSoftTimer = xTimerCreate( "QTmPrd",		/* Текстовое имя, не используется в RTOS kernel. */
                                  100,		/* Период таймера в тиках. */
                                  pdTRUE,		/* Время будет автоматически перезагружать себя, когда оно истечет. */
                                  NULL,		/* В функцию параметры не передаем */
                                  qFlashTimCallback); /* Указатель на функцию , которую вызывает таймер. */  
  
    /* Открытие таймера контроля ожидания ресурсов */
	qFlashxUpdateTimer = xTimerCreate( "QTmDly",		/* Текстовое имя, не используется в RTOS kernel. */
                                    10000,		/* Период таймера в тиках. */
                                    pdTRUE,		/* Время будет автоматически перезагружать себя, когда оно истечет. */
                                    NULL,		/* В функцию параметры не передаем */
                                    qFlashTimCallback); /* Указатель на функцию , которую вызывает таймер. */  

	/*  */
	xTimerStart(qFlashxSoftTimer, 0);   

  
	for (;;)
	{
		/* Обнуляем сообщение */
		qFlashNotifiedValue = 0;
		/*================================== Проверка наличия сообщений ========================================*/
		xTaskNotifyWait(0x00000000,			/* Не очищайте биты уведомлений при входе               */
		                0xFFFFFFFF,			/* Сбросить значение уведомления до 0 при выходе        */
		                &(qFlashNotifiedValue),			/* Значение уведомленное передается в  NotifiedValue    */
		                portMAX_DELAY); /* Блокировка задачи до появления уведомления           */
    
    
        /* Получено уведомление. Проверяем, какие биты были установлены. */
		/*=========== Уведомление от таймер периодического контроля  ==============*/
		if (((qFlashNotifiedValue) & QFLASH_NOTE_SOFT_TIM) != 0)
		{  
			QSPI_FLASH_Cntrl();
		}    
		/*======== Уведомление от таймера контроля ожидания ресурсов  =============*/
		if (((qFlashNotifiedValue) & QFLASH_NOTE_DELAY_TIM) != 0)
		{  


		}      
	} 
}

/**
  * @brief  инициализация QSPI FLASH
  * @param  None
  * @retval None
  */
void QSPI_FLASH_Init(void)
{
	/* Инициализация аппаратной части интерфейса QUADSPI */
	QUADSPI_Init();
	/* Инициализация задачи */ 
	xTaskCreate(qFlash_Task, (const char*)"QS_FLSH_TSK", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioBelowNormal, &qFlashHandleTask);	
}



