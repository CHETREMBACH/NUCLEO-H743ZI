/**
  ******************************************************************************
  * File Name          : task_qspi_flash.c
  * Description        : This file provides code for the driver QUADSPI Flash.
  * 
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "drv_qspi_w25q128.h"
#include "task_qspi_flash.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "main.h"
#include "pin_dbg.h"
#include "test_qspi_flash.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint32_t    qFlashNotifiedValue;     
TaskHandle_t       qFlashHandleTask;        
TimerHandle_t      qFlashxSoftTimer;            /*  Програмный таймер периодического контроля     */
TimerHandle_t      qFlashxUpdateTimer;          /*  Програмный таймер контроля ожидания ресурсов  */


uint8_t data_buf[256];

/**
  * @brief  QSPI_FLASH control - вызывается с частотой 10 Гц
  * @param  None
  * @retval None
  */
void qFlashCntrl(void)
{
	uint8_t id_code[5];
	T2_HI;
	BSP_QSPI_ReadID(id_code);	
	T2_LO;
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

	
	/* Открытие таймера периодического контроля  */
	qFlashxSoftTimer = xTimerCreate( "QTmPrd",		/* Текстовое имя, не используется в RTOS kernel. */
                                  1000,		/* Период таймера в тиках. */
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
			qFlashCntrl();
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
void qFlashInit(void)
{
	uint8_t id_code[5];
	/*Initialize the QSPI in memory mapped mode*/
	T1_HI;
	BSP_QSPI_Init();
	BSP_QSPI_ReadID(id_code);
	BSP_QSPI_ConfigFlash(W25Q128FV_SPI_MODE);
	BSP_QSPI_ReadID(id_code);	
	
	for (uint16_t cntic = 0; cntic < 256; cntic++)
	{
		data_buf[cntic] = cntic;
	}
	data_buf[255] = 55;
	
	BlocDampPrint(data_buf,256);
	
	//BSP_QSPI_EraseBlock(4096, W25Q128FV_ERASE_4K);
		
	BSP_QSPI_Write(data_buf, 4096, 256);
	
	for (uint16_t cntic = 0; cntic < 256; cntic++)
	{
		data_buf[cntic] = 0;
	}
	
	T1_LO;
	
	BSP_QSPI_Read(data_buf, 4096, 256);
	BlocDampPrint(data_buf, 256);
	
	
	/* Инициализация задачи */ 
	xTaskCreate(qFlash_Task, (const char*)"QS_FLSH_TSK", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioBelowNormal, &qFlashHandleTask);	
}



