/**
  ******************************************************************************
  * @file    drv_qspi_w25q128.c
  * @author  MCD Application Team
  * @brief   This file includes a standard driver for the QSPI memory .
  ******************************************************************************
  @verbatim
  How To use this driver:
  -----------------------
   - This driver is used to drive the W25Q128FV QSPI external
       memory mounted on STM32H743I-EVAL evaluation board.

   - This driver need a specific component driver (W25Q128FV) to be included with.

  Driver description:
  -------------------
   - Initialization steps:
       + Initialize the QPSI external memory using the BSP_QSPI_Init() function. This
            function includes the MSP layer hardware resources initialization and the
            QSPI interface with the external memory.
         STR and DTR transfer rates are supported.
         SPI, SPI 2-IO, SPI-4IO and QPI modes are supported

   - QSPI memory operations
       + QSPI memory can be accessed with read/write operations once it is
            initialized.
            Read/write operation can be performed with AHB access using the functions
            BSP_QSPI_Read()/BSP_QSPI_Write().
       + The function BSP_QSPI_GetInfo() returns the configuration of the QSPI memory.
            (see the QSPI memory data sheet)
       + Perform erase block operation using the function BSP_QSPI_EraseBlock() and by
            specifying the block address. You can perform an erase operation of the whole
            chip by calling the function BSP_QSPI_EraseChip().
       + The function BSP_QSPI_GetStatus() returns the current status of the QSPI memory.
            (see the QSPI memory data sheet)
       + The function BSP_QSPI_EnableMemoryMappedMode enables the QSPI memory mapped mode
       + The function BSP_QSPI_DisableMemoryMappedMode disables the QSPI memory mapped mode
       + The function BSP_QSPI_ConfigFlash() allow to configure the QSPI mode and transfer rate

  Note:
  --------
    Regarding the "Instance" parameter, needed for all functions, it is used to select
    an QSPI instance. On the STM32H743I_EVAL board, there's one instance. Then, this
    parameter should be 0.

  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "drv_qspi_w25q128.h"
#include "pin_dbg.h"

BSP_QSPI_Hndlr_t    pqspi;

/* Private functions ---------------------------------------------------------*/

static void QSPI_MspInit(QSPI_HandleTypeDef *hQspi);
static void QSPI_MspDeInit(QSPI_HandleTypeDef *hSspi);
static int32_t QSPI_ResetMemory(void);

/**
  * @brief  Initializes the QSPI interface.
  * @param  Instance   QSPI Instance
  * @retval BSP status
  */
int32_t BSP_QSPI_Init(void)
{
	BSP_QSPI_Init_t Init;
	int32_t ret = BSP_ERROR_NONE;
	BSP_QSPI_Info_t pInfo;
	MX_QSPI_Init_t qspi_init;
	static const uint32_t Prescaler = 0;
	
	Init.InterfaceMode = W25Q128FV_QPI_MODE;
	Init.DualFlashMode = W25Q128FV_DUALFLASH_DISABLE;	

	/* Check if instance is already initialized */
	if (pqspi.IsInitialized == QSPI_ACCESS_NONE)
	{
#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
		/* Register the QSPI MSP Callbacks */
		if (pqspi.IsMspCallbacksValid == 0UL)
		{
			if (BSP_QSPI_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
			{
				ret = BSP_ERROR_PERIPH_FAILURE;
			}
		}
#else
		/* Msp QSPI initialization */
		QSPI_MspInit(&(pqspi.hqspi));
#endif /* USE_HAL_QSPI_REGISTER_CALLBACKS */

		/* STM32 QSPI interface initialization */
		(void)W25Q128FV_GetFlashInfo(&pInfo);
		qspi_init.ClockPrescaler = Prescaler;
		qspi_init.DualFlashMode  = QSPI_DUALFLASH_DISABLE;
		qspi_init.FlashSize      = (uint32_t)POSITION_VAL((uint32_t)pInfo.FlashSize) - 1U;
		qspi_init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;

		if (MX_QSPI_Init(&(pqspi.hqspi), &qspi_init) != HAL_OK)
		{
			ret = BSP_ERROR_PERIPH_FAILURE;
		}/* QSPI memory reset */
		else if(QSPI_ResetMemory() != BSP_ERROR_NONE)
		{
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}/* Force Flash enter 4 Byte address mode */
		else if(W25Q128FV_AutoPollingMemReady(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
		{
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}
	}
	/* Return BSP status */
	return ret;
}

/**
  * @brief  De-Initializes the QSPI interface.
  * @retval BSP status
  */
int32_t BSP_QSPI_DeInit(void)
{
  int32_t ret = BSP_ERROR_NONE;

    if(pqspi.IsInitialized == QSPI_ACCESS_MMP)
    {
      if(BSP_QSPI_DisableMemoryMappedMode() != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
    }

    if(ret == BSP_ERROR_NONE)
    {
      /* Set default QSPI_Ctx values */
      pqspi.IsInitialized = QSPI_ACCESS_NONE;
      pqspi.InterfaceMode = BSP_QSPI_SPI_MODE;
      pqspi.DualFlashMode = QSPI_DUALFLASH_ENABLE;

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 0)
      QSPI_MspDeInit(&(pqspi.hqspi));
#endif /* (USE_HAL_QSPI_REGISTER_CALLBACKS == 0) */

      /* Call the DeInit function to reset the driver */
      if (HAL_QSPI_DeInit(&(pqspi.hqspi)) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Initializes the QSPI interface.
  * @param  hQspi       QSPI handle
  * @param  Config      QSPI configuration structure
  * @retval BSP status
  */
__weak HAL_StatusTypeDef MX_QSPI_Init(QSPI_HandleTypeDef *hQspi, MX_QSPI_Init_t *Config)
{
  /* QSPI initialization */
  /* QSPI freq = SYSCLK /(1 + ClockPrescaler) Mhz */
  hQspi->Instance                = QUADSPI;
  hQspi->Init.ClockPrescaler     = Config->ClockPrescaler;
  hQspi->Init.FifoThreshold      = 1;
  hQspi->Init.SampleShifting     = Config->SampleShifting;
  hQspi->Init.FlashSize          = Config->FlashSize;
  hQspi->Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE; /* Min 50ns for nonRead */
  hQspi->Init.ClockMode          = QSPI_CLOCK_MODE_0;
  hQspi->Init.FlashID            = QSPI_FLASH_ID_1;
  hQspi->Init.DualFlash          = Config->DualFlashMode;

  return HAL_QSPI_Init(hQspi);
}

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
/**
  * @brief Default BSP QSPI Msp Callbacks
  * @param Instance      QSPI Instance
  * @retval BSP status
  */
int32_t BSP_QSPI_RegisterDefaultMspCallbacks (uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_QSPI_RegisterCallback(&(pqspi.hqspi), HAL_QSPI_MSPINIT_CB_ID, QSPI_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_QSPI_RegisterCallback(&(pqspi.hqspi), HAL_QSPI_MSPDEINIT_CB_ID, QSPI_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      pqspi.IsMspCallbacksValid = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief BSP QSPI Msp Callback registering
  * @param Instance     QSPI Instance
  * @param CallBacks    pointer to MspInit/MspDeInit callbacks functions
  * @retval BSP status
  */
int32_t BSP_QSPI_RegisterMspCallbacks (uint32_t Instance, BSP_QSPI_Cb_t *CallBacks)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_QSPI_RegisterCallback(&(pqspi.hqspi), HAL_QSPI_MSPINIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_QSPI_RegisterCallback(&(pqspi.hqspi), HAL_QSPI_MSPDEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      pqspi.IsMspCallbacksValid = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}
#endif /* (USE_HAL_QSPI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData     Pointer to data to be read
  * @param  ReadAddr  Read start address
  * @param  Size      Size of data to read
  * @retval BSP status
  */
int32_t BSP_QSPI_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  int32_t ret = BSP_ERROR_NONE;
	T1_HI;   
      if(W25Q128FV_Read(&(pqspi.hqspi), pqspi.InterfaceMode, pData, ReadAddr, Size) != W25Q128FV_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
	T1_LO;
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  pData      Pointer to data to be written
  * @param  WriteAddr  Write start address
  * @param  Size       Size of data to write
  * @retval BSP status
  */
int32_t BSP_QSPI_Write( uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t end_addr, current_size, current_addr;
  uint8_t *write_data;

    /* Calculation of the size between the write address and the end of the page */
    current_size = W25Q128FV_PAGE_SIZE - (WriteAddr % W25Q128FV_PAGE_SIZE);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > Size)
    {
      current_size = Size;
    }

    /* Initialize the address variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;
    write_data = pData;

    /* Perform the write page by page */
    do
    {
	    T2_HI; 
	    
      /* Check if Flash busy ? */
      if(W25Q128FV_AutoPollingMemReady(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }/* Enable write operations */
      else if(W25Q128FV_WriteEnable(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }/* Issue page program command */
      else if(W25Q128FV_PageProgram(&(pqspi.hqspi), pqspi.InterfaceMode, write_data, current_addr, current_size) != W25Q128FV_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }/* Configure automatic polling mode to wait for end of program */
      else if (W25Q128FV_AutoPollingMemReady(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        write_data += current_size;
        current_size = ((current_addr + W25Q128FV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128FV_PAGE_SIZE;
      }
	    T2_LO;
    } while ((current_addr < end_addr) && (ret == BSP_ERROR_NONE));

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  *         W25Q128FV support 4K, 32K, 64K size block erase commands.
  * @param  BlockAddress Block address to erase
  * @param  BlockSize    Erase Block size
  * @retval BSP status
  */
int32_t BSP_QSPI_EraseBlock(uint32_t BlockAddress, BSP_QSPI_Erase_t BlockSize)
{
  int32_t ret = BSP_ERROR_NONE;
	T1_HI;
    /* Check Flash busy ? */
    if(W25Q128FV_AutoPollingMemReady(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Enable write operations */
    else if(W25Q128FV_WriteEnable(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Issue Block Erase command */
      if(W25Q128FV_BlockErase(&(pqspi.hqspi), pqspi.InterfaceMode, BlockAddress, BlockSize) != W25Q128FV_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
	T1_LO;
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Erases the entire QSPI memory.
  * @retval BSP status
  */
int32_t BSP_QSPI_EraseChip(void)
{
  int32_t ret = BSP_ERROR_NONE;

    /* Check Flash busy ? */
    if(W25Q128FV_AutoPollingMemReady(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Enable write operations */
    else if (W25Q128FV_WriteEnable(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Issue Chip erase command */
      if(W25Q128FV_ChipErase(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Reads current status of the QSPI memory.
  *         If WIP != 0 then return busy.
  * @retval QSPI memory status: whether busy or not
  */
int32_t BSP_QSPI_GetStatus(void)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t reg;

    if(W25Q128FV_ReadStatusRegister(&(pqspi.hqspi), pqspi.InterfaceMode, &reg) != W25Q128FV_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Check the value of the register */
	    if ((reg & (uint8_t)W25Q128FV_SR1_BUSY) != 0U)
      {
        ret = BSP_ERROR_BUSY;
      }
    }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Return the configuration of the QSPI memory.
  * @param  pInfo     pointer on the configuration structure
  * @retval BSP status
  */
int32_t BSP_QSPI_GetInfo(BSP_QSPI_Info_t *pInfo)
{
  int32_t ret = BSP_ERROR_NONE;

    (void)W25Q128FV_GetFlashInfo(pInfo);

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Configure the QSPI in memory-mapped mode
  *         Only 1 Instance can running MMP mode. And it will lock system at this mode.
  * @retval BSP status
  */
int32_t BSP_QSPI_EnableMemoryMappedMode(void)
{
  int32_t ret = BSP_ERROR_NONE;

      if(W25Q128FV_EnableMemoryMappedMode(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else /* Update QSPI context if all operations are well done */
      {
        pqspi.IsInitialized = QSPI_ACCESS_MMP;
      }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Exit form memory-mapped mode
  *         Only 1 Instance can running MMP mode. And it will lock system at this mode.
  * @retval BSP status
  */
int32_t BSP_QSPI_DisableMemoryMappedMode(void)
{
	uint8_t Dummy;
	int32_t ret = BSP_ERROR_NONE;

	/* Check if the instance is supported */
	if (pqspi.IsInitialized != QSPI_ACCESS_MMP)
	{
		ret = BSP_ERROR_QSPI_MMP_UNLOCK_FAILURE;
	}/* Abort MMP back to indirect mode */
	else if(HAL_QSPI_Abort(&(pqspi.hqspi)) != HAL_OK)
	{
		ret = BSP_ERROR_PERIPH_FAILURE;
	}
	else
	{
		/* Force QSPI interface Sampling Shift to half cycle */
		pqspi.hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;

		if (HAL_QSPI_Init(&(pqspi.hqspi)) != HAL_OK)
		{
			ret = BSP_ERROR_PERIPH_FAILURE;
		}
		/* Dummy read for exit from Performance Enhance mode */
		else if(W25Q128FV_Read(&(pqspi.hqspi), pqspi.InterfaceMode, &Dummy, 0, 1) != W25Q128FV_OK)
		{
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}
		else /* Update QSPI context if all operations are well done */
		{
			pqspi.IsInitialized = QSPI_ACCESS_INDIRECT;
		}
	}
	/* Return BSP status */
	return ret;
}

/**
  * @brief  Get flash ID, 3 Byte
  *         Manufacturer ID, Memory type, Memory density
  * @param  Id QSPI Identifier
  * @retval BSP status
  */
int32_t BSP_QSPI_ReadID(uint8_t *Id)
{
  int32_t ret = BSP_ERROR_NONE;

    if(W25Q128FV_ReadID(&(pqspi.hqspi), pqspi.InterfaceMode, Id) != W25Q128FV_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set Flash to desired Interface mode. And this instance becomes current instance.
  *         If current instance running at MMP mode then this function isn't work.
  *         Indirect -> Indirect
  * @param  Mode      QSPI mode
  * @retval BSP status
  */
int32_t BSP_QSPI_ConfigFlash(BSP_QSPI_Interface_t Mode)
{
  int32_t ret = BSP_ERROR_NONE;

    /* Check if MMP mode locked ************************************************/
    if(pqspi.IsInitialized == QSPI_ACCESS_MMP)
    {
      ret = BSP_ERROR_QSPI_MMP_LOCK_FAILURE;
    }
    else
    {
      /* Setup MCU transfer rate setting ***************************************************/
	  pqspi.hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;

      if(HAL_QSPI_Init(&(pqspi.hqspi))!= HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      else
      {
        /* Setup Flash interface ***************************************************/
        switch(pqspi.InterfaceMode)
        {
        case W25Q128FV_QPI_MODE :               /* 4-4-4 commands */
          if(Mode != W25Q128FV_QPI_MODE)
          {
            if(W25Q128FV_ExitQPIMode(&(pqspi.hqspi)) != W25Q128FV_OK)
            {
              ret = BSP_ERROR_COMPONENT_FAILURE;
            }
          }
          break;

        case BSP_QSPI_SPI_MODE :               /* 1-1-1 commands, Power on H/W default setting */
        case BSP_QSPI_SPI_2IO_MODE :           /* 1-2-2 read commands */
        case BSP_QSPI_SPI_4IO_MODE :           /* 1-4-4 read commands */
        default :
          if(Mode == W25Q128FV_QPI_MODE)
          {
            if(W25Q128FV_EnterQPIMode(&(pqspi.hqspi)) != W25Q128FV_OK)
            {
              ret = BSP_ERROR_COMPONENT_FAILURE;
            }
          }
          break;
        }

        /* Update QSPI context if all operations are well done */
        if(ret == BSP_ERROR_NONE)
        {
          /* Update current status parameter *****************************************/
          pqspi.IsInitialized = QSPI_ACCESS_INDIRECT;
          pqspi.InterfaceMode = Mode;
        }
      }
    }
  /* Return BSP status */
  return ret;
}

/**
  * @brief QSPI MSP Initialization
  * @param hQspi : QSPI handle
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - NVIC configuration for QSPI interrupt
  * @retval None
  */
static void QSPI_MspInit(QSPI_HandleTypeDef *hQspi)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hQspi);

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable the QuadSPI memory interface clock */
  QSPI_CLK_ENABLE();
  /* Reset the QuadSPI memory interface */
  QSPI_FORCE_RESET();
  QSPI_RELEASE_RESET();
  /* Enable GPIO clocks */
  QSPI_CLK_GPIO_CLK_ENABLE();
  QSPI_BK1_CS_GPIO_CLK_ENABLE();
  QSPI_BK1_D0_GPIO_CLK_ENABLE();
  QSPI_BK1_D1_GPIO_CLK_ENABLE();
  QSPI_BK1_D2_GPIO_CLK_ENABLE();
  QSPI_BK1_D3_GPIO_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* QSPI CLK GPIO pin configuration  */
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Pin       = QSPI_CLK_PIN;
  gpio_init_structure.Alternate = QSPI_CLK_GPIO_ALTERNATE;
  HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &gpio_init_structure);

  /* QSPI CS GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_CS_PIN;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Alternate = QSPI_BK1_CS_GPIO_ALTERNATE;
  HAL_GPIO_Init(QSPI_BK1_CS_GPIO_PORT, &gpio_init_structure);

  /* QSPI D0 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D0_PIN;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Alternate = QSPI_BK1_D0_GPIO_ALTERNATE;
  HAL_GPIO_Init(QSPI_BK1_D0_GPIO_PORT, &gpio_init_structure);

  /* QSPI D1 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D1_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D1_GPIO_ALTERNATE;
  HAL_GPIO_Init(QSPI_BK1_D1_GPIO_PORT, &gpio_init_structure);

  /* QSPI D2 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D2_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D2_GPIO_ALTERNATE;
  HAL_GPIO_Init(QSPI_BK1_D2_GPIO_PORT, &gpio_init_structure);

  /* QSPI D3 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D3_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D3_GPIO_ALTERNATE;	
  HAL_GPIO_Init(QSPI_BK1_D3_GPIO_PORT, &gpio_init_structure);

  /*##-3- Configure the NVIC for QSPI #########################################*/
  /* NVIC configuration for QSPI interrupt */
  //HAL_NVIC_SetPriority(QUADSPI_IRQn, 0x0F, 0);
  //HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
}

/**
  * @brief QSPI MSP De-Initialization
  * @param hQspi  QSPI handle
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @retval None
  */
static void QSPI_MspDeInit(QSPI_HandleTypeDef *hQspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hQspi);

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-Configure QSPI pins */
  HAL_GPIO_DeInit(QSPI_CLK_GPIO_PORT, QSPI_CLK_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_CS_GPIO_PORT, QSPI_BK1_CS_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_D0_GPIO_PORT, QSPI_BK1_D0_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_D1_GPIO_PORT, QSPI_BK1_D1_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_D2_GPIO_PORT, QSPI_BK1_D2_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_D3_GPIO_PORT, QSPI_BK1_D3_PIN);

  /*##-3- Reset peripherals ##################################################*/
  /* Reset the QuadSPI memory interface */
  QSPI_FORCE_RESET();
  QSPI_RELEASE_RESET();

  /* Disable the QuadSPI memory interface clock */
  QSPI_CLK_DISABLE();
}

/**
  * @brief  This function reset the QSPI Flash memory.
  *         Fore QPI+SPI reset to avoid system come from unknown status.
  *         Flash accept 1-1-1, 1-1-2, 1-2-2 commands after reset.
  * @param  none
  * @retval BSP status
  */
static int32_t QSPI_ResetMemory(void)
{
	int32_t ret = BSP_ERROR_NONE;

	/* Send RESET ENABLE command in QPI mode (QUAD I/Os, 4-4-4) */
	if (W25Q128FV_ResetEnable(&(pqspi.hqspi), W25Q128FV_QPI_MODE) != W25Q128FV_OK)
	{
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Send RESET memory command in QPI mode (QUAD I/Os, 4-4-4) */
	else if(W25Q128FV_ResetMemory(&(pqspi.hqspi), W25Q128FV_QPI_MODE) != W25Q128FV_OK)
	{
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Wait Flash ready */
	else if(W25Q128FV_AutoPollingMemReady(&(pqspi.hqspi), pqspi.InterfaceMode) != W25Q128FV_OK)
	{
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Send RESET ENABLE command in SPI mode (1-1-1) */
	else if(W25Q128FV_ResetEnable(&(pqspi.hqspi), BSP_QSPI_SPI_MODE) != W25Q128FV_OK)
	{
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Send RESET memory command in SPI mode (1-1-1) */
	else if(W25Q128FV_ResetMemory(&(pqspi.hqspi), BSP_QSPI_SPI_MODE) != W25Q128FV_OK)
	{
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}
	else
	{
		pqspi.IsInitialized = QSPI_ACCESS_INDIRECT; /* After reset S/W setting to indirect access   */
		pqspi.InterfaceMode = BSP_QSPI_SPI_MODE; /* After reset H/W back to SPI mode by default  */
	}

	/* Return BSP status */
	return ret;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

