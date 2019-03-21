/*
*********************************************************************************************************
*	                                  
*	模块名称 : flash_if模块
*	文件名称 : flash_if.c
*	版    本 : V1.0
*	说    明 : flash编程驱动
*	修改记录 :
*		版本号    日期          作者             说明
*		v1.0    2015-03-05     Eric2013	      整理自官方驱动
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "stm32f4xx.h"
#include "bsp_flash_if.h"

static uint32_t GetSector(uint32_t Address);

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_Init
*	功能说明: flash解锁后可进行写操作
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void FLASH_If_Init(void)
{ 
	FLASH_Unlock(); 

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
				  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
}

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_Erase
*	功能说明: 删除所有用户flash区
*	形    参: StartSector  起始扇区
*	返 回 值: 0 用户flash区成功删除
*           1 发送错误
*********************************************************************************************************
*/
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
	uint32_t UserStartSector = FLASH_Sector_1, i = 0;

	/* 用户flash区所在的扇区 */
	UserStartSector = GetSector(APPLICATION_ADDRESS);

	for(i = UserStartSector; i <= FLASH_Sector_11; i += 8)
	{
		if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
		{
			return (1);
		}
	}
  
	return (0);
}

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_Write
*	功能说明: 向flash中写数据，4字节对齐
*	形    参: FlashAddress flash起始地址
*             Data         数据地址
*             DataLength   数据长度（4字节）
*	返 回 值: 0 成功
*             1 写flash过程中出错
*             2 写到flash中的数据跟读出的不相同
*********************************************************************************************************
*/
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength)
{
	uint32_t i = 0;

	for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
	{
		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
		be done by word */ 
		if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i)) == FLASH_COMPLETE)
		{
			/* 检查写入的数据 */
			if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
			{
				/* 读出的和写入的不相同 */
				return(2);
			}
			
			/* 地址递增 */
			*FlashAddress += 4;
		}
		else
		{
			/* 向flash中写数据时发生错误 */
			return (1);
		}
	}

	return (0);
}

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_GetWriteProtectionStatus
*	功能说明: 用户flash区的写保护状态
*	形    参: 无
*	返 回 值: 0 用户flash区中没有写保护
*           1 用户flash区某些山扇区有写保护
*********************************************************************************************************
*/
uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
	uint32_t UserStartSector = FLASH_Sector_1;

	/* 用户flash区所在的扇区 */
	UserStartSector = GetSector(APPLICATION_ADDRESS);

	/* 检测用户flash区中的某些扇区是否有写保护 */
	if ((FLASH_OB_GetWRP() >> (UserStartSector/8)) == (0xFFF >> (UserStartSector/8)))
	{ 
		/* 没有写保护 */
		return 1;
	}
	else
	{ 
		/* 某些扇区有写保护 */
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: FLASH_If_DisableWriteProtection
*	功能说明: 解开写保护
*	形    参: 无
*	返 回 值: 0 解保护成功
*           1 解保护失败
*********************************************************************************************************
*/
uint32_t FLASH_If_DisableWriteProtection(void)
{
	__IO uint32_t UserStartSector = FLASH_Sector_1, UserWrpSectors = OB_WRP_Sector_1;

	/* 用户flash区所在的扇区 */
	UserStartSector = GetSector(APPLICATION_ADDRESS);

	UserWrpSectors = 0xFFF-((1 << (UserStartSector/8))-1);

	/* 解锁 Option Bytes */
	FLASH_OB_Unlock();

	/* 所有用户扇区解保护 */
	FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);

	/* 进行 Option Bytes 编程. */  
	if (FLASH_OB_Launch() != FLASH_COMPLETE)
	{
		/* 解保护失败 */
		return (2);
	}

	/* 解保护成功 */
	return (1);
}
/**
  * @brief  Enable the write protection of user flash area.
  * @param  None
  * @retval 1: Write Protection successfully enabled
  *         2: Error: Flash write protection failed
  */
uint32_t FLASH_If_EnableWriteProtection(uint32_t FlashAddress)
{
  __IO uint32_t UserStartSector = FLASH_Sector_1, UserWrpSectors = OB_WRP_Sector_1;

  /* Get the sector where start the user flash area */
  UserStartSector = GetSector(FlashAddress);

  /* Mark all sectors inside the user flash area as non protected */  
  UserWrpSectors = 0xFFF-((1 << (UserStartSector/8))-1);
   
  /* Unlock the Option Bytes */
  FLASH_OB_Unlock();

  /* Disable the write protection for all sectors inside the user flash area */
  FLASH_OB_WRPConfig(UserWrpSectors, ENABLE);

  /* Start the Option Bytes programming process. */  
  if (FLASH_OB_Launch() != FLASH_COMPLETE)
  {
    /* Error: Flash write unprotection failed */
    return (2);
  }

  /* Write Protection successfully disabled */
  return (1);
}
/*
*********************************************************************************************************
*	函 数 名: GetSector
*	功能说明: 根据地址计算扇区首地址
*	形    参: Address  指定地址
*	返 回 值: 扇区首地址
*********************************************************************************************************
*/
static uint32_t GetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_Sector_0;  
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_Sector_1;  
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_Sector_2;  
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_Sector_3;  
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_Sector_4;  
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_Sector_5;  
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_Sector_6;  
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_Sector_7;  
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_Sector_8;  
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_Sector_9;  
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_Sector_10;  
	}
	else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
	{
		sector = FLASH_Sector_11;  
	}
	return sector;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
