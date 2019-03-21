/**
  ********************************  STM32L152RE  *********************************
  * @文件名     ： IAP.c
  * @作者       ： liyang
  * @库版本     ： V1.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2016年11月11日
  * @摘要       ： BSP板级支持包源文件
  ******************************************************************************/
//#include "includes.h"
#include "stm_flash.h"
///////////////////////////////////////////////////////////////////////////////////
#define WRITE_PROTECTION_DISABLE
#define FLASH_PAGE_TO_BE_PROTECTED     ((uint32_t)0x00000004 | (uint32_t)0x00000008)  

#define DATA_32                     ((uint32_t)0x12345678)
unsigned char  AT_QIOPEN[40];
/* Check the status of the switches */
/* Enable by default the disable protection */
#if !defined(WRITE_PROTECTION_ENABLE)&&!defined(WRITE_PROTECTION_DISABLE)
#define WRITE_PROTECTION_DISABLE
#endif /* !WRITE_PROTECTION_ENABLE && !WRITE_PROTECTION_DISABLE */

/* Both switches cannot be enabled in the same time */
#if defined(WRITE_PROTECTION_ENABLE)&&defined(WRITE_PROTECTION_DISABLE)
#error "Switches WRITE_PROTECTION_ENABLE & WRITE_PROTECTION_DISABLE cannot be enabled in the time!"
#endif /* WRITE_PROTECTION_ENABLE && WRITE_PROTECTION_DISABLE */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Address = 0;
uint32_t PageError = 0;
uint32_t ProtectedPAGE = 0x0;
/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
/*Variable used to handle the Options Bytes*/
static FLASH_OBProgramInitTypeDef OptionsBytesStruct;
/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();
	/* Unlock the Options Bytes **/
  HAL_FLASH_OB_Unlock();
	/* Get pages write protection status**/
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  StartSector: start of user flash area
  * @retval 0: user flash area successfully erased
  *         1: error occurred
  */
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
#ifdef WRITE_PROTECTION_DISABLE
  /* Check if desired pages are already write protected ***********************/
  if((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == FLASH_PAGE_TO_BE_PROTECTED)
  {
    /* Restore write protected pages */
    OptionsBytesStruct.OptionType   = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPState     = OB_WRPSTATE_DISABLE;
    OptionsBytesStruct.WRPSector0To31 = FLASH_PAGE_TO_BE_PROTECTED;
    if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
    {
      /* Error occurred while options bytes programming. **********************/
      while (1)
      {
        HAL_Delay(2000);
				break;
      }
    }

    /* Generate System Reset to load the new option byte values ***************/
    HAL_FLASH_OB_Launch();
  }
#elif defined WRITE_PROTECTION_ENABLE
  /* Get current write protected pages and the new pages to be protected ******/
  ProtectedPAGE =  OptionsBytesStruct.WRPSector0To31 | FLASH_PAGE_TO_BE_PROTECTED; 
  /* Check if desired pages are not yet write protected ***********************/
  if((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED )!= FLASH_PAGE_TO_BE_PROTECTED)
  {
    /* Enable the pages write protection **************************************/
    OptionsBytesStruct.OptionType = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPState   = OB_WRPSTATE_ENABLE;
    OptionsBytesStruct.WRPSector0To31    = FLASH_PAGE_TO_BE_PROTECTED;
    if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
    {
      /* Error occurred while options bytes programming. **********************/
      while (1)
      {
        HAL_Delay(2000);
				break;
      }
    }

    /* Generate System Reset to load the new option byte values ***************/
    HAL_FLASH_OB_Launch();
  }
#endif /* WRITE_PROTECTION_DISABLE */
  /* Lock the Options Bytes *************************************************/
  HAL_FLASH_OB_Lock();	
  /* The selected pages are not write protected *******************************/
  if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == 0x00)
  {
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = StartSector;
   	EraseInitStruct.NbPages     = (USER_FLASH_END_ADDRESS - StartSector)/FLASH_PAGE_SIZE;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
      while (1)
      {
        HAL_Delay(2000);
				break;
      }
    }
  }		
	return 1;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
  uint32_t i = 0;
  for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,*FlashAddress, *(uint32_t*)(Data+i)) == HAL_OK)
    {
     /* Check the written value */
      if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
      {
        /* Flash content doesn't match SRAM content */
        return(2);
      }
      /* Increment FLASH destination address */
      *FlashAddress += 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
      return (1);
    }
  }
  return (0);
}

/**
  * @brief  Disables the write protection of user flash area.
  * @param  None
  * @retval 0: Write Protection successfully disabled
  *         1: Error: Flash write unprotection failed
  *         2: Flash memory is not write protected
  */
void FLASH_If_DisableWriteProtection(void)
{
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();
	/* Unlock the Options Bytes **/
  HAL_FLASH_OB_Unlock();
	/* Get pages write protection status**/
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);
}

void FLASH_If_EnableWriteProtection(void)
{
  HAL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();
}


/********************************************************************
* 名  称: void SaveParam(void)
* 功  能: 
* 输  入: 无
* 返回值: 无
**********************************************************************/
void SaveParam(void)
{
	uint32_t flashdestination,ramsource;
	FLASH_If_DisableWriteProtection();
	FLASH_If_Erase(PARAM_ADDRESS);
	flashdestination = PARAM_ADDRESS;		
	ramsource = (uint32_t)gGlobal.m_btDataCache;
  FLASH_If_Write(&flashdestination,(uint32_t *)ramsource,strlen((char*)ramsource));
  HAL_FLASH_Lock();
}

/********************************************************************
* 名  称: void LoadParam(void)
* 功  能: 
* 输  入: 无
* 返回值: 无
**********************************************************************/
void LoadParam(void)
{
	BYTE i;
	BYTE *pSrcAddr = (BYTE *)PARAM_ADDRESS;
	for (i=0;i<40;i++)
	{
		gGlobal.m_btDataCache[i] = pSrcAddr[i];
	}
  if(gGlobal.m_btDataCache[0] == 'A' && gGlobal.m_btDataCache[1] == 'T'&& gGlobal.m_btDataCache[2] == '+') 
  {
    for(i=0;i<40;i++)
		{
      AT_QIOPEN[i] = gGlobal.m_btDataCache[i];
    }
  }
	else
	{
		for(i=0;i<40;i++)
	  {
		  //gGlobal.m_btDataCache[i] = AT_QIOPEN_Pri[i];
	  }
		SaveParam();
  }

}






