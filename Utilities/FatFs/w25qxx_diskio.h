/**
  ******************************************************************************
  * @file    w25qxx_diskio.h
  * @author  
  * @brief   Header for w25qxx_diskio.c module.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _W25QXX_DISKIO
#define _W25QXX_DISKIO

/* Includes ------------------------------------------------------------------*/
#include "diskio.h"		/* FatFs lower layer API */
#include "drv_qspi_w25q128.h"
#include "ff_gen_drv.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern const Diskio_drvTypeDef  Diskio_Driver;

/**
  * @brief  Link the disk I/O driver.
  * @param  None 
  * @retval None
  */
void FATFSLinkDriver(void);

extern FATFS filesystem;

#endif /* _W25QXX_DISKIO */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/