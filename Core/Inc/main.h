/**
  ******************************************************************************
  * @file    main.h 
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"	
#include "printf_dbg.h"
	
/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* For configMAX_PRIORITIES =7 */   
typedef enum {
	TreadPrioIdle = configMAX_PRIORITIES - 7,         /*< priority: idle (lowest)      */           
	TreadPrioLow = configMAX_PRIORITIES - 6,          /*< priority: low                */   
	TreadPrioBelowNormal = configMAX_PRIORITIES - 5,  /*< priority: below normal       */  
	TreadPrioNormal = configMAX_PRIORITIES - 4,       /*< priority: normal (default)   */
	TreadPrioAboveNormal = configMAX_PRIORITIES - 3,  /*< priority: above normal       */
	TreadPrioHigh = configMAX_PRIORITIES - 2,         /*< priority: high               */
	TreadPrioRealtime = configMAX_PRIORITIES - 1,     /*< priority: realtime (highest) */
} thread_prio_t; 
	

	/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   10
   
	/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

	/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1	
	
/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT 2020 OneTiOne *****END OF FILE****/
