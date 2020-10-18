/**
  ******************************************************************************
  * @file    w25qxx_diskio.h
  * @author  
  * @brief   w25qxx_diskio.c.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

#include "ff_gen_drv.h"
#include "w25qxx_diskio.h"	
#include "printf_dbg.h"

FATFS filesystem;

/*W25Q128 256 Block   Block16 Sector  Sector 4KB */
/* Размер сегмента */
#define W25QX_SIZE_SECTOR         (4096)    
/* Маска адреса по сегменту */   
#define W25QX_MASK_SECTOR          ( 0x00000FFF )    
#define FLASH_SUBSECTOR_SIZE 	    4096	  
#define FLASH_BLOCK_SIZE   	        1  
#define FLASH_SECTOR_COUNT          4096	
   	
char FlashPath[4]; /* flash card logical drive path */

/* Private function prototypes -----------------------------------------------*/
DSTATUS w25qxx_initialize (BYTE);
DSTATUS w25qxx_status (BYTE);
DRESULT w25qxx_read (BYTE, BYTE*, DWORD, UINT);
#if FF_FS_READONLY == 0
  DRESULT w25qxx_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* FF_FS_READONLY == 0 */
DRESULT w25qxx_ioctl (BYTE, BYTE, void*);

/**
  * @brief  Link the disk I/O driver.
  * @param  None 
  * @retval None
  */
void FATFSLinkDriver(void)
{
	/* Link the disk I/O driver */
	FATFS_LinkDriver(&Diskio_Driver, FlashPath);   
}

const Diskio_drvTypeDef  Diskio_Driver =
{
  w25qxx_initialize,
  w25qxx_status,
  w25qxx_read,
#if  FF_FS_READONLY == 0
  w25qxx_write,
#endif /* FF_FS_READONLY == 0 */
  w25qxx_ioctl,
};

/**
  * @brief  Initializes a Drive
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS w25qxx_initialize(BYTE lun)
{
	DSTATUS  Stat = STA_NOINIT; 
	uint8_t id_code[3];

	printf("\n Start Init W25Q128FV QSPI Drive... \n\n");	
	
	/*Initialize the QSPI in memory mapped mode*/
	BSP_QSPI_Init();
	BSP_QSPI_ConfigFlash(W25Q128FV_QPI_MODE);
	BSP_QSPI_ReadID(id_code);
	
	if ( (id_code[0] == 0xEF) && (id_code[1] == 0x40) && (id_code[2] == 0x18) )
	{
		printf(" Init Drive Complite. \n\n");
		Stat = RES_OK;
		return Stat;		
	}
	else
	{
		printf(" Error Init Drive. \n\n");
		Stat = RES_ERROR;
		return Stat;		
	}
}

/**
  * @brief  Gets Disk Status
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS w25qxx_status(BYTE lun)
{
  return RES_OK;
}

/**
  * @brief  Reads Sector(s)
  * @param  lun : not used
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT w25qxx_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
	DRESULT res = RES_ERROR;
  
	while (count--)
	{
		//printf(" Read sector %.5lu \n", sector); 
		//BSP_QSPI_DMARead(buff, sector*FLASH_SUBSECTOR_SIZE, FLASH_SUBSECTOR_SIZE);  
		BSP_QSPI_Read(buff, sector*FLASH_SUBSECTOR_SIZE, FLASH_SUBSECTOR_SIZE); 		
		sector++;
		buff += FLASH_SUBSECTOR_SIZE;
	}  
	res = RES_OK;
  
	return res;
}

/**
  * @brief  Writes Sector(s)
  * @param  lun : not used
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if FF_FS_READONLY == 0
DRESULT w25qxx_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
	DRESULT res = RES_ERROR;

	for (BYTE i = 0; i < count; i++)
	{	  
		//printf(" Erase sector %.5lu \n", sector); 
		BSP_QSPI_EraseBlock(sector*((DWORD)(FLASH_SUBSECTOR_SIZE)), W25Q128FV_ERASE_4K);  
		//printf(" Write sector %.5lu \n", sector); 
		BSP_QSPI_Write((uint8_t*)buff, sector*((DWORD)(FLASH_SUBSECTOR_SIZE)), (DWORD)(FLASH_SUBSECTOR_SIZE));
		sector++;
		buff += FLASH_SUBSECTOR_SIZE;
	}  
 
	res = RES_OK;
  
	return res;
}
#endif /* FF_FS_READONLY == 0 */

/**
  * @brief  I/O control operation
  * @param  lun : not used
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
DRESULT w25qxx_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  
  switch (cmd)
  {
    /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
    
    /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
      *(uint16_t*)buff = FLASH_SECTOR_COUNT;
      res = RES_OK;
    break;
    
    /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
	  *(uint16_t*)buff = FLASH_SUBSECTOR_SIZE;
    res = RES_OK;
    break;
    
    /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(uint16_t*)buff = FLASH_BLOCK_SIZE;
    res = RES_OK;
    break;
    
  default:
    res = RES_PARERR;
  }
  
  return res;
}

 
  
