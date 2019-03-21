#ifndef __HEADER_H
#define __HEADER_H
//typedef unsigned char   BOOL;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

#define TRUE    1
#define FALSE   0

#define COMn    4

#define PACK_SIZE	        255     // 数据包大小
#define COM_BUFFER_SIZE   250
#define COM_CACHE_PACK_COUNT 3
//协议数据包结构
typedef struct
{
	BYTE	m_pBuffer[PACK_SIZE];
	BYTE	m_Pos;
	unsigned short	m_Len;
} All_PACK;

//读写器参数区
typedef struct
{
	BYTE m_IPAddress[4];	// 读写器IP地址
	BYTE m_NetMask[4];		// 子网掩码
	BYTE m_GateWay[4];		// 网关
	BYTE m_MacAddress[6];	// MAC地址
}ReaderParam;

// 通讯类型
typedef enum{
    COMM_TYPE_NONE = 0xFF,
    COMM_TYPE_COM = 0x00,
    COMM_TYPE_NET = 0x01,
    COMM_TYPE_USB_VCP = 0x02,
    COMM_TYPE_USB_HID = 0x03,
}COMMTYPE;
typedef struct 
{
    unsigned char       m_SendCache[COM_BUFFER_SIZE+1];
    __IO unsigned char  m_SendSize;
    unsigned char       m_SendCursor;
    unsigned char       m_RecvCache[COM_BUFFER_SIZE+1];
    unsigned char       m_RecvLen;
    unsigned char       m_btRecvState;
    All_PACK            m_RecvPack[COM_CACHE_PACK_COUNT];
    unsigned char       m_PushIndex;
    unsigned char       m_PopIndex;
}ComSet;
typedef struct
{
    ReaderParam m_ReaderParam;
    unsigned short ReaderID;
    COMMTYPE    CommType;                 // 通讯类型
}GlobalSet;

/*    数据流操作宏    */
#define InitPack(pack)	\
	(pack).m_Pos = 0;	\
	(pack).m_Len = 0;	\
	(pack).m_pBuffer[0] = 0;

#define BYTEToPack(pack,btVal)\
	if ((pack).m_Pos + 1 < PACK_SIZE)	\
	{\
		(pack).m_pBuffer[(pack).m_Pos++] = btVal;\
	}\
	(pack).m_Len = (pack).m_Pos;


#define WORDToPack(pack,wVal) \
	if ((pack).m_Pos + 2 < PACK_SIZE) \
	{\
		(pack).m_pBuffer[(pack).m_Pos++] = (BYTE)(wVal&0x00ff);\
		(pack).m_pBuffer[(pack).m_Pos++] = (BYTE)((wVal>>8)&0x00ff);\
	}\
	(pack).m_Len = (pack).m_Pos;


#define DWORDToPack(pack,wVal) \
	if ((pack).m_Pos + 4 < PACK_SIZE) \
	{\
		(pack).m_pBuffer[(pack).m_Pos++] = (BYTE)(wVal&0x00ff);\
                (pack).m_pBuffer[(pack).m_Pos++] = (BYTE)((wVal>>8)&0x00ff);\
                (pack).m_pBuffer[(pack).m_Pos++] = (BYTE)((wVal>>16)&0x00ff);\
                (pack).m_pBuffer[(pack).m_Pos++] = (BYTE)((wVal>>24)&0x00ff);\
	}\
	(pack).m_Len = (pack).m_Pos;

#define BYTEFromPack(pack,btVal)\
	if ((pack).m_Pos + 1 <= (pack).m_Len) \
	{\
		(btVal) = (pack).m_pBuffer[(pack).m_Pos++];\
	}\
	else\
	{\
		(btVal) = 0;\
	}

#define WORDFromPack(pack,wVal)\
	if ((pack).m_Pos + 2 <= (pack).m_Len)\
	{\
		(wVal) = (pack).m_pBuffer[(pack).m_Pos++];\
                (wVal) += (((WORD)(pack).m_pBuffer[(pack).m_Pos++]) << 8);\
	}

#define DWORDFromPack(pack,dwVal) \
    if ((pack).m_Pos + 4 <= (pack).m_Len)\
    {\
        (dwVal) = (pack).m_pBuffer[(pack).m_Pos++];\
        (dwVal) += (((DWORD)(pack).m_pBuffer[(pack).m_Pos++]) << 8);\
        (dwVal) += (((DWORD)(pack).m_pBuffer[(pack).m_Pos++]) << 16);\
	(dwVal) += (((DWORD)(pack).m_pBuffer[(pack).m_Pos++]) << 24);\
    }

#define PUSHPackHeader(pack) \
        BYTEToPack(pack,0x7F);\
        BYTEToPack(pack,0x7F);\
        WORDToPack(pack, gGlobal.ReaderID);

//////////////////////////////////////////////////////////////////////////////
// 操作读写器的命令字
// 操作读写器的命令字
#define CMD_GET_READER_APPVERSION		0xF0
#define CMD_WRITE_UPDATE_DATA				0xF1
#define CMD_JUMP_APPLICATION				0xF2
#define CMD_ENTER_BOOTLOADER				0xF3

// 串口接收数据，每两个数据之间的超时时间
#define BYTE_SEP			10
// 错误类型
#define ERR_SUCCESS			0					/* 操作成功									*/
#define	ERR_CRC				0x01				/* CRC校验错误								*/
#define ERR_DATA			0x02				/* 标签应答时表示数据错误					*/
#define ERR_LOGIC			0x03				/* 逻辑错误，比如在应答广播命令时，TagStatus为P2P命令 */
#define ERR_INTID			0x04				/* 返回的IntID与本机的IntID不同				*/
#define ERR_STREAM			0x05				/* 读取数据流错误							*/
#define ERR_TIMEOUT_APP			0x06				/* 读取数据超时								*/
#define ERR_TIMEOUT_NA			0x07				/* 读取数据超时，无数据						*/
#define ERR_DATAFORMAT			0x08				/* 数据格式错误*/

GlobalSet gGlobal;
extern void ProcessUpCmd(All_PACK *pack);
//extern void ProcessRFIDData(All_PACK *pack);
//extern void LoadReaderParam();

#define NOW_CLICK_32US  (DWORD)(LocalTime*31 + (SysTick->LOAD-SysTick->VAL)/SysTick->LOAD * 31)
#define NOW_CLICK_1MS   LocalTime

unsigned char Check1MSTick(DWORD dwCurTickCount, DWORD dwStart, DWORD DelayTime);
unsigned char Check32USTick(DWORD dwCurTickCount, DWORD dwStart, DWORD DelayTime);

// 应用程序位置
#define APPLICATION_ADDRESS   (uint32_t)0x08010000
//读写器参数存储位置
#define PARAM_ADDRESS   (uint32_t)0x080E0000 

#define C2R_DATA_BEGIN	2
#define COM_CACHE_PACK_COUNT 3

#define NET_PORT_TCP 9222
#define NET_PORT_UDP 8222

#define BOOTLOADER_ID 2
//typedef  void (*pFunction)(void);
#endif
