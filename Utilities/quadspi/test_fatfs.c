/**
  ******************************************************************************
  * File Name          : test_fatfs.c
  * Description        : This file provides test FatFS code.
  * 
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "test_fatfs.h"
#include "ff.h"
#include "string.h"
#include "ff_gen_drv.h"
#include "w25qxx_diskio.h"
#include "printf_dbg.h"
#include <stdlib.h>
#include <stdbool.h>

FATFS USERFatFS;
uint8_t work_buff[FF_MAX_SS];
char FlashPath[4]; /* flash card logical drive path */

FIL USERFile; /* File object for USER */

uint32_t NotifyValue;
FRESULT StatRESULT; 
uint32_t real_num_byte; 

uint8_t TestBuffer[4096];

/**
  * @brief  Выдача статуса code (FRESULT) в терминал.
  * @param  None 
  * @retval None
  */
void StatusDiscPrint(FRESULT RezMesh)
{
	/* Анализ полученного code (FRESULT) */
	switch ((uint8_t)RezMesh)
	{
	case FR_OK                 : printf("Succeeded                                                       \n"); break;      
	case FR_DISK_ERR           : printf("A hard error occurred in the low level disk I/O layer           \n"); break; 
	case FR_INT_ERR            : printf("Assertion failed                                                \n"); break; 
	case FR_NOT_READY          : printf("The physical drive cannot work                                  \n"); break; 
	case FR_NO_FILE            : printf("Could not find the file                                         \n"); break;      
	case FR_NO_PATH            : printf("Could not find the path                                         \n"); break; 
	case FR_INVALID_NAME       : printf("The path name format is invalid                                 \n"); break; 
	case FR_DENIED             : printf("Access denied due to prohibited access or directory full        \n"); break;          
	case FR_EXIST              : printf("Access denied due to prohibited access                          \n"); break;      
	case FR_INVALID_OBJECT     : printf("The file/directory object is invalid                            \n"); break; 
	case FR_WRITE_PROTECTED    : printf("The physical drive is write protected                           \n"); break; 
	case FR_INVALID_DRIVE      : printf("The logical drive number is invalid                             \n"); break;    
	case FR_NOT_ENABLED        : printf("The volume has no work area                                     \n"); break;      
	case FR_NO_FILESYSTEM      : printf("There is no valid FAT volume                                    \n"); break; 
	case FR_MKFS_ABORTED       : printf("The f_mkfs() aborted due to any problem                         \n"); break; 
	case FR_TIMEOUT            : printf("Could not get a grant to access the volume within defined period\n"); break;    
	case FR_LOCKED             : printf("The operation is rejected according to the file sharing policy  \n"); break;      
	case FR_NOT_ENOUGH_CORE    : printf("LFN working buffer could not be allocated                       \n"); break; 
	case FR_TOO_MANY_OPEN_FILES: printf("Number of open files > FF_FS_LOCK                               \n"); break; 
	case FR_INVALID_PARAMETER  : printf("Given parameter is invalid                                      \n"); break;    
	default: 
		printf("Unknown parameter                                               \n");
		break;
	}
}


/**
  * @brief  Link the disk I/O driver.
  * @param  None 
  * @retval None
  */
void CmdLinkDriver(void)
{
	/* Link the disk I/O driver */
	FATFS_LinkDriver(&Diskio_Driver, FlashPath);   
}

/**
  * @brief  Форматирование диска.
  * @param  None 
  * @retval None
  */
void CmdFmkfs(void)
{
	/* Форматирование диска */
	StatusDiscPrint(f_mkfs("0:", FM_ANY, 0, work_buff, sizeof(work_buff)));
}

/**
  * @brief  Форматирование диска.
  * @param  None 
  * @retval None
  */
void CmdFmount(void)
{
	FRESULT res;
	
	/* Монтирование диска */
	StatusDiscPrint(f_mount(&USERFatFS, "0:", 1));
	
	uint32_t freeClust;
	FATFS* fs_ptr = &USERFatFS;
	// Warning! This fills fs.n_fatent and fs.csize!
	if (f_getfree("", &freeClust, &fs_ptr) != FR_OK) {
		printf("f_getfree() failed\r\n");
		return;
	}	
	
	printf("f_getfree() done!\r\n");

	uint32_t totalBlocks = (USERFatFS.n_fatent - 2) * USERFatFS.csize;
	uint32_t freeBlocks = freeClust * USERFatFS.csize;

	printf("Total blocks: %lu (%lu Mb)\r\n", totalBlocks, totalBlocks / 256);
	printf("Free  blocks: %lu (%lu Mb)\r\n", freeBlocks, freeBlocks / 256);	
	
	
	
	DIR dir;
	res = f_opendir(&dir, "/");
	if (res != FR_OK) {
		printf("f_opendir() failed, res = %d\r\n", res);
		return;
	}

	FILINFO fileInfo;
	uint32_t totalFiles = 0;
	uint32_t totalDirs = 0;
	printf("--------\r\nRoot directory:\r\n");
	for (;;) {
		res = f_readdir(&dir, &fileInfo);
		if ((res != FR_OK) || (fileInfo.fname[0] == '\0')) {
			break;
		}
       
		if (fileInfo.fattrib & AM_DIR) {
			printf("  DIR  %s\r\n", fileInfo.fname);
			totalDirs++;
		}
		else {
			printf("  FILE %s\r\n", fileInfo.fname);
			totalFiles++;
		}
	}

	printf("(total: %lu dirs, %lu files)\r\n--------\r\n",
		totalDirs,
		totalFiles);

	res = f_closedir(&dir);
	if (res != FR_OK) {
		printf("f_closedir() failed, res = %d\r\n", res);
		return;
	}	
	
	
//	printf("Writing to log.txt...\r\n");
//
//	char writeBuff[128];
//	snprintf(writeBuff,
//		sizeof(writeBuff),
//		"Total blocks: %lu (%lu Mb); Free blocks: %lu (%lu Mb)\r\n",
//		totalBlocks,
//		totalBlocks / 2000,
//		freeBlocks,
//		freeBlocks / 2000);
//
//	FIL logFile;
//	res = f_open(&logFile, "log.txt", FA_OPEN_APPEND | FA_WRITE);
//	if (res != FR_OK) {
//		printf("f_open() failed, res = %d\r\n", res);
//		return;
//	}
//
//	unsigned int bytesToWrite = strlen(writeBuff);
//	unsigned int bytesWritten;
//	res = f_write(&logFile, writeBuff, bytesToWrite, &bytesWritten);
//	if (res != FR_OK) {
//		printf("f_write() failed, res = %d\r\n", res);
//		return;
//	}
//
//	if (bytesWritten < bytesToWrite) {
//		printf("WARNING! Disk is full.\r\n");
//	}
//
//	res = f_close(&logFile);
//	if (res != FR_OK) {
//		printf("f_close() failed, res = %d\r\n", res);
//		return;
//	}

	printf("Reading file...\r\n");
	FIL msgFile;
	res = f_open(&msgFile, "log.txt", FA_READ);
	if (res != FR_OK) {
		printf("f_open() failed, res = %d\r\n", res);
		return;
	}

	char readBuff[128];
	unsigned int bytesRead;
	res = f_read(&msgFile, readBuff, sizeof(readBuff) - 1, &bytesRead);
	if (res != FR_OK) {
		printf("f_read() failed, res = %d\r\n", res);
		return;
	}

	readBuff[bytesRead] = '\0';
	printf("```\r\n%s\r\n```\r\n", readBuff);

	res = f_close(&msgFile);
	if (res != FR_OK) {
		printf("f_close() failed, res = %d\r\n", res);
		return;
	}	

}

/**
  * @brief  Запись файла.
  * @param  None 
  * @retval None
  */
void CmdFwrite(void)
{
    /* Записать Файл */
    /* Открыть файл */
	StatRESULT = f_open(&USERFile, "start.txt", FA_CREATE_ALWAYS | FA_WRITE);
 
	printf("Open file ...");      
      
	/* Если файл открыт успешно */
	if (StatRESULT == FR_OK)
	{
		/* Записать данные */
		StatRESULT = f_write(&USERFile, TestBuffer, (sizeof(TestBuffer)) / 2, &real_num_byte);
		printf(" write file %lu byte", real_num_byte);
		/* Закрыть файл */
		f_close(&USERFile);      
		printf(" close file\n");
	}
	else
	{
		printf(" error close file\n");
	} 
}

/**
  * @brief  Дописать файл.
  * @param  None 
  * @retval None
  */
void CmdFread(void)
{
	/* Записать Файл */
	/* Открыть файл */
	StatRESULT = f_open(&USERFile, "start.txt", FA_CREATE_ALWAYS | FA_WRITE);
            
	printf("Open file ...");      
      
	/* Если файл открыт успешно */
	if (StatRESULT == FR_OK)
	{
		/* Записать данные */
		StatRESULT = f_write(&USERFile, TestBuffer, (sizeof(TestBuffer)) / 2, &real_num_byte);
        
		printf(" write file %lu byte", real_num_byte);
        
		/* Закрыть файл */
		f_close(&USERFile);      
		printf(" close file\n");
	}
	else
	{
		printf(" error close file\n");
	} 
}







