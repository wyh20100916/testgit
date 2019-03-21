/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : RL-TCPnet的TCP服务器实现
*              实验目的：
*                1. 学习RL-TCPnet的TCP服务器创建和数据收发。
*              实验内容：
*                1. 强烈推荐将网线接到路由器或者交换机上面测试，因为已经使能了DHCP，可以自动获取IP地址。
*                2. 创建了一个TCP Server，而且使能了局域网域名NetBIOS，用户只需在电脑端ping armfly
*                   就可以获得板子的IP地址，端口号1001。
*                3. 用户可以在电脑端用网络调试软件创建TCP Client连接此服务器端。
*                4. 按键K1按下，发送8字节的数据给TCP Client。
*                5. 按键K2按下，发送1024字节的数据给TCP Client。
*                6. 按键K3按下，发送5MB字节的数据给TCP Client。
*              注意事项：
*                1. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期         作者            说明
*       V1.0    2017-04-17   Eric2013    1. ST固件库1.5.0版本
*                                        2. BSP驱动包V1.2
*                                        3. RL-TCPnet版本V4.74
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"			/* 底层硬件驱动 */
#include "app_tcpnet_lib.h"
//#include "header.h"
volatile unsigned long LocalTime; 
extern void bsp_InitRtc(void);
typedef  void (*pFunction)(void);
/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint32_t JumpAddress;
pFunction Jump_To_Application;
int main (void) 
{	
	   uint8_t data;
	   LocalTime = 0;
	   bsp_InitRtc();//初始化备份寄存器的时钟和使能
	   data=RTC_ReadBackupRegister(RTC_BKP_DR0);
	   if (data == 0xAE)
     {
			  //跳转至应用程序执行
        RTC_WriteBackupRegister(RTC_BKP_DR0, 0x00);
        JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
        // Jump to user application
        Jump_To_Application = (pFunction) JumpAddress;
        // Initialize user application's Stack Pointer
        __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
        Jump_To_Application();
    }
		else
		{
        RTC_WriteBackupRegister(RTC_BKP_DR0, 0xAE);
    }
		/* 初始化外设 */
		bsp_Init();
		/* 进入RL-TCPnet测试函数 */
		TCPnetTest();
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
