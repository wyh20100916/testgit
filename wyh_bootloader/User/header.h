#ifndef __HEADER_H
#define __HEADER_H
//typedef unsigned char   BOOL;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

#define TRUE    1
#define FALSE   0

#define COMn    4

#define PACK_SIZE	        255     // ���ݰ���С
#define COM_BUFFER_SIZE   250
#define COM_CACHE_PACK_COUNT 3
//Э�����ݰ��ṹ
typedef struct
{
	BYTE	m_pBuffer[PACK_SIZE];
	BYTE	m_Pos;
	unsigned short	m_Len;
} All_PACK;

//��д��������
typedef struct
{
	BYTE m_IPAddress[4];	// ��д��IP��ַ
	BYTE m_NetMask[4];		// ��������
	BYTE m_GateWay[4];		// ����
	BYTE m_MacAddress[6];	// MAC��ַ
}ReaderParam;

// ͨѶ����
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
    COMMTYPE    CommType;                 // ͨѶ����
}GlobalSet;

/*    ������������    */
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
// ������д����������
// ������д����������
#define CMD_GET_READER_APPVERSION		0xF0
#define CMD_WRITE_UPDATE_DATA				0xF1
#define CMD_JUMP_APPLICATION				0xF2
#define CMD_ENTER_BOOTLOADER				0xF3

// ���ڽ������ݣ�ÿ��������֮��ĳ�ʱʱ��
#define BYTE_SEP			10
// ��������
#define ERR_SUCCESS			0					/* �����ɹ�									*/
#define	ERR_CRC				0x01				/* CRCУ�����								*/
#define ERR_DATA			0x02				/* ��ǩӦ��ʱ��ʾ���ݴ���					*/
#define ERR_LOGIC			0x03				/* �߼����󣬱�����Ӧ��㲥����ʱ��TagStatusΪP2P���� */
#define ERR_INTID			0x04				/* ���ص�IntID�뱾����IntID��ͬ				*/
#define ERR_STREAM			0x05				/* ��ȡ����������							*/
#define ERR_TIMEOUT_APP			0x06				/* ��ȡ���ݳ�ʱ								*/
#define ERR_TIMEOUT_NA			0x07				/* ��ȡ���ݳ�ʱ��������						*/
#define ERR_DATAFORMAT			0x08				/* ���ݸ�ʽ����*/

GlobalSet gGlobal;
extern void ProcessUpCmd(All_PACK *pack);
//extern void ProcessRFIDData(All_PACK *pack);
//extern void LoadReaderParam();

#define NOW_CLICK_32US  (DWORD)(LocalTime*31 + (SysTick->LOAD-SysTick->VAL)/SysTick->LOAD * 31)
#define NOW_CLICK_1MS   LocalTime

unsigned char Check1MSTick(DWORD dwCurTickCount, DWORD dwStart, DWORD DelayTime);
unsigned char Check32USTick(DWORD dwCurTickCount, DWORD dwStart, DWORD DelayTime);

// Ӧ�ó���λ��
#define APPLICATION_ADDRESS   (uint32_t)0x08010000
//��д�������洢λ��
#define PARAM_ADDRESS   (uint32_t)0x080E0000 

#define C2R_DATA_BEGIN	2
#define COM_CACHE_PACK_COUNT 3

#define NET_PORT_TCP 9222
#define NET_PORT_UDP 8222

#define BOOTLOADER_ID 2
//typedef  void (*pFunction)(void);
#endif
