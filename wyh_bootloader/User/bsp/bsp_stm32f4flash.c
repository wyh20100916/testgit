#include "bsp.h"


u32 STM32F4FLASH_ReadWord(u32 FlashAddr)
{
	return *(vu32*)FlashAddr;
}

u32 STM32F4FLASH_GetFlashSector(u32 addr)
{
	if(addr < ADDR_FLASH_SECTOR_1)
		return FLASH_Sector_0;
	else if(addr < ADDR_FLASH_SECTOR_2)
		return FLASH_Sector_1;
	else if(addr < ADDR_FLASH_SECTOR_3)
		return FLASH_Sector_2;
	else if(addr < ADDR_FLASH_SECTOR_4)
		return FLASH_Sector_3;
	else if(addr < ADDR_FLASH_SECTOR_5)
		return FLASH_Sector_4;
	else if(addr < ADDR_FLASH_SECTOR_6)
		return FLASH_Sector_5;
	else if(addr < ADDR_FLASH_SECTOR_7)
		return FLASH_Sector_6;
	else if(addr < ADDR_FLASH_SECTOR_8)
		return FLASH_Sector_7;
	else if(addr < ADDR_FLASH_SECTOR_9)
		return FLASH_Sector_8;
	else if(addr < ADDR_FLASH_SECTOR_10)
		return FLASH_Sector_9;
	else if(addr < ADDR_FLASH_SECTOR_11)
		return FLASH_Sector_10;
	else return FLASH_Sector_11;
}

FLASH_Status STM32F4FLASH_CheckEmpty(u32 StartAddr,u32 EndAddr)
{
	FLASH_Status status = FLASH_COMPLETE;
	
	if(StartAddr < 0x1fff0000)
	{
		while(StartAddr < EndAddr)
		{
			if(STM32F4FLASH_ReadWord(StartAddr) != 0xffffffff)
			{
				status = FLASH_EraseSector(STM32F4FLASH_GetFlashSector(StartAddr),VoltageRange_3);
				if(status != FLASH_COMPLETE)
					break;
			}
			else
				StartAddr+=4;
		}
	}
	else
	{
		status = FLASH_ERROR_PGS;
	}
	return status;
}

void STM32F4FLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)
{
	FLASH_Status status = FLASH_COMPLETE;
	u32 endaddr=0;
	
	if(WriteAddr<STM32_FLASH_BASE || WriteAddr%4)
		return;
	FLASH_Unlock();
	FLASH_DataCacheCmd(DISABLE);
	
	endaddr = WriteAddr+NumToWrite*4;
//	if(addrx < 0x1fff0000)
//	{
//		while(addrx<endaddr)
//		{
//			if(STM32F4FLASH_ReadWord(addrx) != 0xffffffff)
//			{
//				status = FLASH_EraseSector(STM32F4FLASH_GetFlashSector(addrx),VoltageRange_3);
//				if(status != FLASH_COMPLETE)
//					break;
//			}
//			else
//				addrx+=4;
//		}
//	}
	status = STM32F4FLASH_CheckEmpty(WriteAddr,endaddr);
	if(status == FLASH_COMPLETE)
	{
		while(WriteAddr < endaddr)
		{
			if(FLASH_ProgramWord(WriteAddr,*pBuffer) != FLASH_COMPLETE)
			{
				break;
			}
			WriteAddr+=4;
			pBuffer++;
		}
	}
	FLASH_DataCacheCmd(DISABLE);
	FLASH_Lock();
}

void STM32F4FLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)
{
	while(NumToRead--)
	{
		*pBuffer=STM32F4FLASH_ReadWord(ReadAddr);
		pBuffer++;
		ReadAddr+=4;
	}
}
