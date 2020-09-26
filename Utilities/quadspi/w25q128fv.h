/**
  ******************************************************************************
  * @file    w25q128fv.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-May-2015
  * @brief   This file contains all the description of the N25Q128A QSPI memory.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W25Q128FV_H
#define __W25Q128FV_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/

typedef struct {
	 uint32_t FlashSize; /*!< Size of the flash */
	 uint32_t EraseSectorSize; /*!< Size of sectors for the erase operation */
	 uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
	 uint32_t ProgPageSize; /*!< Size of pages for the program operation */
	 uint32_t ProgPagesNumber; /*!< Number of pages for the program operation */
} W25Q128FV_Info;

	 /* W25Q128FV Component Error codes *********************************************/
#define W25Q128FV_OK                           0
#define W25Q128FV_ERROR_INIT                  -1
#define W25Q128FV_ERROR_COMMAND               -2
#define W25Q128FV_ERROR_TRANSMIT              -3
#define W25Q128FV_ERROR_RECEIVE               -4
#define W25Q128FV_ERROR_AUTOPOLLING           -5
#define W25Q128FV_ERROR_MEMORYMAPPED          -6
/**exported type **/

/******************W25Q128FV_Info_t**********************/
typedef struct
{
	 uint32_t FlashSize; /*!< Size of the flash */
	 uint32_t EraseSectorSize; /*!< Size of sectors for the erase operation */
	 uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
	 uint32_t ProgPageSize; /*!< Size of pages for the program operation */
	 uint32_t ProgPagesNumber; /*!< Number of pages for the program operation */
} W25Q128FV_Info_t;

/******************W25Q128FV_Transfer_t**********************/
typedef enum
{
	 W25Q128FV_SPI_MODE = 0,
	 /*!< 1-1-1 commands, Power on H/W default setting */
	 W25Q128FV_SPI_2IO_MODE,
	 /*!< 1-1-2 read commands                   */
	 W25Q128FV_SPI_4IO_MODE,
	 /*!< 1-1-4 commands                   */
	 W25Q128FV_QPI_MODE                      /*!< 4-4-4 commands                               */
} W25Q128FV_Interface_t;

/******************W25Q128FV_DualFlash_t**********************/
typedef enum
{
	 W25Q128FV_DUALFLASH_DISABLE = QSPI_DUALFLASH_DISABLE,
	 /*!<  Single flash mode              */
	 W25Q128FV_DUALFLASH_ENABLE = QSPI_DUALFLASH_ENABLE
} W25Q128FV_DualFlash_t;

/******************W25Q128FV_Erase_t**********************/

typedef enum
{
	 W25Q128FV_ERASE_4K = 0, /*!< 4K size Sector erase */
	 W25Q128FV_ERASE_32K, /*!< 32K size Block erase */
	 W25Q128FV_ERASE_64K, /*!< 64K size Block erase */
	 W25Q128FV_ERASE_CHIP  /*!< Whole chip erase     */
} W25Q128FV_Erase_t;
	 
/** 
  * @brief  W25Q128FV Configuration  
  */  
#define W25Q128FV_FLASH_SIZE                  0x1000000 /* 128 MBits => 16MBytes */
#define W25Q128FV_SECTOR_SIZE                 0x10000   /* 256 sectors of 64KBytes */
#define W25Q128FV_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q128FV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */

#define W25Q128FV_BULK_ERASE_MAX_TIME         250000
#define W25Q128FV_SECTOR_ERASE_MAX_TIME       3000
#define W25Q128FV_SUBSECTOR_ERASE_MAX_TIME    800

/* Flag Status Register */
#define W25Q128FV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q128FV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q128FV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */
	 
/**
  * @brief  W25Q128FV Commands
  */
/* Reset Operations */
#define W25Q128FV_RESET_ENABLE_CMD                     0x66
#define W25Q128FV_RESET_MEMORY_CMD                     0x99

/* Identification Operations */
#define W25Q128FV_READ_ID_CMD                          0x9F
#define W25Q128FV_MULTIPLE_IO_READ_ID_CMD              0x9F
#define W25Q128FV_READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A

/* Read Operations */
#define W25Q128FV_READ_CMD                             0x03
#define W25Q128FV_FAST_READ_CMD                        0x0B
#define W25Q128FV_DUAL_OUT_FAST_READ_CMD               0x3B	 
#define W25Q128FV_QUAD_OUT_FAST_READ_CMD               0x6B

/* Write Operations */
#define W25Q128FV_WRITE_ENABLE_CMD                     0x06
#define W25Q128FV_WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define W25Q128FV_READ_STATUS_REG_1_CMD                0x05
#define W25Q128FV_WRITE_STATUS_REG_1_CMD               0x01

#define W25Q128FV_READ_STATUS_REG_2_CMD                0x35
#define W25Q128FV_WRITE_STATUS_REG_2_CMD               0x31	 
	 
#define W25Q128FV_READ_STATUS_REG_3_CMD                0x15
#define W25Q128FV_WRITE_STATUS_REG_3_CMD               0x11	 

/* Program Operations */
#define W25Q128FV_PAGE_PROG_CMD                        0x02
#define W25Q128FV_QUAD_IN_FAST_PROG_CMD                0x32

/* Erase Operations */
#define W25Q128FV_SUBSECTOR_ERASE_CMD_4K               0x20
#define W25Q128FV_SUBSECTOR_ERASE_CMD_32K              0x52
#define W25Q128FV_SUBSECTOR_ERASE_CMD_64K              0xD8
#define W25Q128FV_CHIP_ERASE_CMD                       0xC7	 

#define W25Q128FV_PROG_ERASE_RESUME_CMD                0x7A
#define W25Q128FV_PROG_ERASE_SUSPEND_CMD               0x75

/* Quad Operations */
#define W25Q128FV_ENTER_QUAD_CMD                       0x38
#define W25Q128FV_EXIT_QUAD_CMD                        0xFF
	 
#define W25Q128FV_ENTER_DEEP_POWER_DOWN                0xB9
#define W25Q128FV_RELEASE_FROM_DEEP_POWER_DOWN         0xAB
 
	 
/**
  * @brief  W25Q128FV Registers
  */
/* Status Register 1*/
#define W25Q128FV_SR1_BUSY                     ((uint32_t)0x00000001)    /*!< Erase/Write In Progress */
#define W25Q128FV_SR1_WEL                      ((uint32_t)0x00000002)    /*!< Write enable latch */
#define W25Q128FV_SR1_BPB                      ((uint32_t)0x0000005C)    /*!< Block protected against program and erase operations */
#define W25Q128FV_SR1_TB                       ((uint32_t)0x00000020)    /*!< Top/Bottom Block Protect */
#define W25Q128FV_SR1_SEC                      ((uint32_t)0x00000040)    /*!< Sector/Block Protect Bit */
#define W25Q128FV_SR1_SRP0                     ((uint32_t)0x00000080)    /*!< Status Register Protect  */	 

/* Status Register 2*/
#define W25Q128FV_SR2_SRP1                     ((uint32_t)0x00000001)    /*!< Status Register Protect */
#define W25Q128FV_SR2_QE                       ((uint32_t)0x00000002)    /*!< Quad Enable */
#define W25Q128FV_SR2_SRLB                     ((uint32_t)0x00000038)    /*!< Security Register Lock Bits */
#define W25Q128FV_SR2_CMP                      ((uint32_t)0x00000040)    /*!< Complement Protect */
#define W25Q128FV_SR2_SUS                      ((uint32_t)0x00000080)    /*!< Erase/Program Suspend Status */	 

/* Status Register 3*/	 
#define W25Q128FV_SR3_WPS                      ((uint32_t)0x00000004)    /*!< Write Protect Selection */
#define W25Q128FV_SR3_DRV                      ((uint32_t)0x00000060)    /*!< Output Driver Strength */
#define W25Q128FV_SR3_HOLD_RST                 ((uint32_t)0x00000080)    /*!< /HOLD or /RESET Pin Function */
	 
/* Status Register */
#define W25Q128FV_SR_BUSY                      ((uint32_t)0x00000001)    /*!< Erase/Write In Progress */
#define W25Q128FV_SR_WEL                       ((uint32_t)0x00000002)    /*!< Write enable latch */
#define W25Q128FV_SR_BPB                       ((uint32_t)0x0000005C)    /*!< Block protected against program and erase operations */
#define W25Q128FV_SR_TB                        ((uint32_t)0x00000020)    /*!< Top/Bottom Block Protect */
#define W25Q128FV_SR_SEC                       ((uint32_t)0x00000040)    /*!< Sector/Block Protect Bit */
#define W25Q128FV_SR_SRP0                      ((uint32_t)0x00000080)    /*!< Status Register Protect  */	 
#define W25Q128FV_SR_SRP1                      ((uint32_t)0x00000100)    /*!< Status Register Protect */
#define W25Q128FV_SR_QE                        ((uint32_t)0x00000200)    /*!< Quad Enable */
#define W25Q128FV_SR_SRLB                      ((uint32_t)0x00003800)    /*!< Security Register Lock Bits */
#define W25Q128FV_SR_CMP                       ((uint32_t)0x00004000)    /*!< Complement Protect */
#define W25Q128FV_SR_SUS                       ((uint32_t)0x00008000)    /*!< Erase/Program Suspend Status */	 
#define W25Q128FV_SR_WPS                       ((uint32_t)0x00004000)    /*!< Write Protect Selection */
#define W25Q128FV_SR_DRV                       ((uint32_t)0x00600000)    /*!< Output Driver Strength */
#define W25Q128FV_SR_HOLD_RST                  ((uint32_t)0x00800000)    /*!< /HOLD or /RESET Pin Function */
	 
	 
 int32_t W25Q128FV_GetFlashInfo(W25Q128FV_Info_t *pInfo);
 int32_t W25Q128FV_AutoPollingMemReady(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
 /* Register/Setting Commands *************************************************/
 int32_t W25Q128FV_WriteEnable(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
 int32_t W25Q128FV_BlockErase(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode, uint32_t BlockAddress, W25Q128FV_Erase_t BlockSize);
 int32_t W25Q128FV_ChipErase(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
 int32_t W25Q128FV_PageProgram(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode, uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
 int32_t W25Q128FV_Read(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
 int32_t W25Q128FV_ReadStatusRegister(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode, uint8_t *Value);
 int32_t W25Q128FV_EnterQPIMode(QSPI_HandleTypeDef *Ctx);
 int32_t W25Q128FV_ExitQPIMode(QSPI_HandleTypeDef *Ctx);

 int32_t W25Q128FV_EnableMemoryMappedMode(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
 int32_t W25Q128FV_WriteDisable(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
 int32_t W25Q128FV_ReadID(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode, uint8_t *ID);

 int32_t W25Q128FV_ResetMemory(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
 int32_t W25Q128FV_ResetEnable(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);

 int32_t W25Q128FV_ReleaseFromDeepPowerDown(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
 int32_t W25Q128FV_EnterDeepPowerDown(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
 int32_t W25Q128FV_ProgEraseResume(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
 int32_t W25Q128FV_ProgEraseSuspend(QSPI_HandleTypeDef *Ctx, W25Q128FV_Interface_t Mode);
  
#ifdef __cplusplus
}
#endif

#endif /* __W25Q128FV_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
