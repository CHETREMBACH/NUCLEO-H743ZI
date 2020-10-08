/**
  ******************************************************************************
  * @file    main.c 
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    23-08-2020
  * @brief   
  *
  *
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Kernel includes. */
#include "printf_dbg.h"
#include "pin_dbg.h"
#include "cmd_process.h"
#include "app_ethernet.h"
#include "task_qspi_flash.h"

volatile const char __version__[] = "NUCLEO-H743ZI";    
volatile const char __date__[] = __DATE__;
volatile const char __time__[] = __TIME__;

//Структура отсчета времени работы
TimeWorkTypeDef  SysWorkTime;	

/**
  * Initializes the Global MSP.
  */
void InitClock(void);

/**
 * @brief  Функция подсчета времени(секунд) работы системы.
 * @param  None
 * @retval None
 */
void update_time_work(void)
{
	/* подсчет общего числа секунд */
	SysWorkTime.all_second++;
  
	/* подсчет секунд в течении минуты */
	if (SysWorkTime.second > 58)
	{
		SysWorkTime.second = 0;
		/* подсчет минут в течении часа */
		if (SysWorkTime.minute > 58)
		{
			SysWorkTime.minute = 0;
			/* подсчет часов в течении дня */
			if (SysWorkTime.hour > 22)
			{
				SysWorkTime.hour = 0;
				/* подсчет дней */
				SysWorkTime.day++;
			}
			else SysWorkTime.hour++;
		}
		else SysWorkTime.minute++;
	}
	else SysWorkTime.second++;
}


/**
 * @brief  Функция вызываемая основной задачей системы 1 раз в секунду.
 * @param  None
 * @retval None
 */
void system_thread_periodic_func(void) 
{
	/* Подсчет времени работы */
	update_time_work();
}

/**
 * @brief  Start Thread 
 * @param  None
 * @retval None
 */
void system_thread(void *arg)
{ 
	/* Переменная формирования периода вызова задачи */
	TickType_t xLastWakeTime;
	
	//Подключение интерфейса отладки
	DBG_Hardware_Setup();

	//Подключения для отладки GPIO 
	DBG_PIN_Setup();
	
	/* init code for LWIP */
	InitLwIP();
		
	//Инициализация задачи диагностического терминала 
	xTaskCreate(terminal_task, (const char*)"CmdTrmnl", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioNormal, NULL);
	
	/* Инициализация задачи QSPI FLASH */
	qFlashInit();		
		
	// Информационная шапка программы
	printf("______________________________________________\r\n");
	printf("\r\n");
	printf("   %s \r\n", __version__);
	printf("   DATA: %s \r\n", __date__);
	printf("   TIME: %s \r\n", __time__);
	printf("   CPU FREQ = %.9lu Hz \r\n", SystemCoreClock);  
	printf("______________________________________________\r\n"); 

	/* Инициализации структуры отсчета времени работы */
	SysWorkTime.all_second = 0;
	SysWorkTime.second = 0;
	SysWorkTime.minute = 0;
	SysWorkTime.hour = 0;
	SysWorkTime.day = 0;
	/* Система запушена */
  
	/* Инициализация переменной xLastWakeTime на текущее время. */
	xLastWakeTime = xTaskGetTickCount();

	/* Тело стартовой задачи - для вспомогательных вычислений.  */
	for (;;)
	{
		/* Функция вызываемая основной задачей системы 1 раз в секунду */
		system_thread_periodic_func();
		/* Ожидание до следующего цикла. */
		vTaskDelayUntil(&xLastWakeTime, 1000);
	}  
}

/**
  * @brief  Configure the MPU attributes 
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct;
  
	/* Disable the MPU */
	HAL_MPU_Disable();

	/* Configure the MPU attributes as Device not cacheable 
	   for ETH DMA descriptors */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x30040000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
  
	/* Configure the MPU attributes as Normal Non Cacheable
	   for LwIP RAM heap which contains the Tx buffers */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x30044000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER1;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Enable the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
	/* Enable I-Cache */
	SCB_EnableICache();

	/* Enable D-Cache */
	SCB_EnableDCache();
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* Configure the MPU attributes as Device memory for ETH DMA descriptors */
	MPU_Config();
  
	/* Enable the CPU Cache */
	CPU_CACHE_Enable();	
	
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	
	InitClock();
	
	/* Init thread */
	xTaskCreate(system_thread, (const char*)"SysTask", configMINIMAL_STACK_SIZE * 2, NULL, TreadPrioNormal, NULL);
	
	/* Start scheduler */
	vTaskStartScheduler();
  
	/* We should never get here as control is now taken by the scheduler */
	NVIC_SystemReset();
	return 0;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(const char * const Error_Note)
{
	/* User can add his own implementation to report the HAL error return state */
	printf("Error_Handler: %s \r\n",Error_Note);
	while (1) vTaskDelay(1); 	
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	   tex:  */
	printf("Wrong parameters value: file %s on line %lu\r\n", file, line);
	while (1) vTaskDelay(1); 	
}
#endif /* USE_FULL_ASSERT */
/************************ COPYRIGHT 2020 OneTiOne *****END OF FILE****/
