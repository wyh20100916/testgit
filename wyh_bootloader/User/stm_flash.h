/**
  *********************************  STM32L152RE  ********************************
  * @文件名     ： usart.h
  * @作者       ： liyang
  * @库版本     ： V1.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2016年11月07日
  * @摘要       ： BSP板级支持包头文件
  ******************************************************************************/

/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef _STM_FLASH_H
#define _STM_FLASH_H
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#define ABS_RETURN(x,y)         (x < y) ? (y-x) : (x-y)

//#define FLASH_PAGE_SIZE               0x100     /* 256 Bytes */

/* define the address from where user application will be loaded,
   the application address should be a start sector address */
//#define APPLICATION_ADDRESS   (uint32_t)0x08015000

/* define the address from where IAP will be loaded, 0x08000000:BANK1 or 
   0x08030000:BANK2 */
#define FLASH_START_ADDRESS   (uint32_t)0x08000000

/* Get the number of sectors from where the user program will be loaded */
#define FLASH_SECTOR_NUMBER  ((uint32_t)(ABS_RETURN(APPLICATION_ADDRESS,FLASH_START_ADDRESS))>>12)

#ifdef STM32F2XX
  #define USER_FLASH_LAST_PAGE_ADDRESS  0x0801FF00
  #define USER_FLASH_END_ADDRESS        0x0801FFFF
  /* Compute the mask to test if the Flash memory, where the user program will be
  loaded, is write protected */
  #define  FLASH_PROTECTED_SECTORS   ((uint32_t)~((1 << FLASH_SECTOR_NUMBER) - 1))
#elif defined STM32F40_41xxx
  #define USER_FLASH_LAST_PAGE_ADDRESS  0x0807FF00
  /* define the address from where user application will be loaded,
   the application address should be a start sector address */
  #define USER_FLASH_END_ADDRESS        0x0807FFFF
  
#else
 #error "Please select first the STM32 device to be used (in stm32l1xx.h)"    
#endif 
/* define the user application size */
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1)
extern unsigned char  AT_QIOPEN_Pri[];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t FLASH_If_Write(uint32_t* FlashAddress, uint32_t* Data, uint16_t DataLength);
void FLASH_If_DisableWriteProtection(void);
void SaveParam(void);
void LoadParam(void);
void FLASH_If_EnableWriteProtection(void);



#endif /* _BSP_H */


