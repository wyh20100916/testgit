/*
*********************************************************************************************************
*
*	ģ������ : TCPnet����Э��ջ����
*	�ļ����� : app_tcpnet_lib.c
*	��    �� : V1.0
*	˵    �� : ���ԵĹ���˵��
*              1. ǿ���Ƽ������߽ӵ�·�������߽�����������ԣ���Ϊ�Ѿ�ʹ����DHCP�������Զ���ȡIP��ַ��
*              2. ������һ��TCP Server������ʹ���˾���������NetBIOS���û�ֻ���ڵ��Զ�ping armfly
*                 �Ϳ��Ի�ð��ӵ�IP��ַ���˿ں�1001��
*              3. �û������ڵ��Զ�����������������TCP Client���Ӵ˷������ˡ�
*              4. ����K1���£�����8�ֽڵ����ݸ�TCP Client��
*              5. ����K2���£�����1024�ֽڵ����ݸ�TCP Client��
*              6. ����K3���£�����5MB�ֽڵ����ݸ�TCP Client��
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2017-04-17   Eric2013     �׷�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/	
#include "bsp.h"		
#include <RTL.h>
#include <stdio.h>
#include <Net_Config.h>
#include "header.h"
#include "flash_if.h"

/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 1
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif


/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#define PORT_NUM       1001    /* TCP�����������˿ں� */


/*
*********************************************************************************************************
*	                                     ����
*********************************************************************************************************
*/
//High-Order Byte Table
const unsigned char  auchCRCHi[256] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
} ;

//Low-Order Byte Table
/* Table of CRC values for low�Corder byte */
const unsigned char  auchCRCLo[256] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};
extern volatile unsigned long LocalTime;
void tcpnet_poll(void);
uint8_t socket_tcp;
BYTE bOK = 0;
ComSet gComSet[COMn];
GlobalSet gGlobal;
All_PACK RecvPack;
All_PACK SendPack;
WORD CalcCRC16(BYTE *pData, WORD wDataLen)
{
		unsigned char uchCRCHi = 0xFF ; 
		unsigned char uchCRCLo = 0xFF ; 
		unsigned uIndex ; 			

		unsigned short wCRC = 0;
		while (wDataLen--) {
			uIndex = uchCRCLo ^ *pData++; 
			uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex]; 
			uchCRCHi = auchCRCLo[uIndex] ; 
		}

		wCRC = (uchCRCHi << 8 | uchCRCLo);
		return wCRC;
}
void SendUp(All_PACK *pPack)
{	        
					uint8_t iCount,i;
					uint16_t maxlen;
	        uint8_t *sendbuf;
					iCount = pPack->m_Len;
					//do
					{
						tcpnet_poll();
						if (tcp_check_send (socket_tcp) == __TRUE) 
						{
							maxlen = tcp_max_dsize (socket_tcp);
							iCount -= maxlen;
							
							if(iCount < 0)
							{
									maxlen = iCount + maxlen;
							}							
							sendbuf = tcp_get_buf(maxlen);
							for(i=0;i<pPack->m_Len;i++)
							{
									sendbuf[i] = pPack->m_pBuffer[i];
							}						
							tcp_send (socket_tcp, sendbuf, pPack->m_Len);
						}
						
					}//while(iCount > 0);	
}

void ProcessData(COMMTYPE COM,unsigned char c)
{
    unsigned char i;
    switch(gComSet[COM].m_btRecvState)
    {
    case 0:        
        if (c == 0x7F)
        {
            gComSet[COM].m_btRecvState = 1;
        }
        break;
    case 1:         
        if (c == 0x7F)
        {
            gComSet[COM].m_btRecvState = 2;
            gComSet[COM].m_RecvLen = 0;
        }
        else
        {
            gComSet[COM].m_btRecvState = 0;
        }
        break;
    case 2:
        if (c == 0x7F)
        {
            gComSet[COM].m_btRecvState = 3;        //??0x81
        }
        else
        {
            gComSet[COM].m_RecvCache[gComSet[COM].m_RecvLen++] = c;
            if (gComSet[COM].m_RecvLen >= COM_BUFFER_SIZE)
            {
                gComSet[COM].m_btRecvState = 0;
                gComSet[COM].m_RecvLen = 0;
            }
        }
        break;
    case 3:
        if (c == 0x81)
        {
            gComSet[COM].m_RecvCache[gComSet[COM].m_RecvLen++] = 0x7F;
            if (gComSet[COM].m_RecvLen >= COM_BUFFER_SIZE)
            {
                gComSet[COM].m_btRecvState = 0;
                gComSet[COM].m_RecvLen = 0;
            }
            else
            {
                gComSet[COM].m_btRecvState = 2;
            }
        }
        else if (c == 0x7F)
        {
            if (gComSet[COM].m_RecvLen == 0)
            {
                gComSet[COM].m_btRecvState = 2;
            }
            else
            {
                gComSet[COM].m_RecvPack[gComSet[COM].m_PushIndex].m_Len = gComSet[COM].m_RecvLen;
                gComSet[COM].m_RecvPack[gComSet[COM].m_PushIndex].m_Pos = 0;                
                for (i=0;i<gComSet[COM].m_RecvLen;i++)
                {
                    gComSet[COM].m_RecvPack[gComSet[COM].m_PushIndex].m_pBuffer[i] = gComSet[COM].m_RecvCache[i];
                }
                gComSet[COM].m_PushIndex++;
                if (gComSet[COM].m_PushIndex >= COM_CACHE_PACK_COUNT)
                {
                    gComSet[COM].m_PushIndex = 0;
                }
                gComSet[COM].m_btRecvState = 0;
            }
        }
        else
        {
            gComSet[COM].m_btRecvState = 0;
        }
        break;
    default:
        break;
    }  
}
void FinishPack(All_PACK *pPack)
{
    BYTE i, bt7fCount;
    WORD wCRC;
    wCRC = CalcCRC16(pPack->m_pBuffer + C2R_DATA_BEGIN,pPack->m_Len - C2R_DATA_BEGIN);
    BYTEToPack((*pPack),wCRC&0xFF);
    BYTEToPack((*pPack),(wCRC>>8)&0xFF);
    bt7fCount = 0;
    for (i = C2R_DATA_BEGIN; i < pPack->m_Len; ++i) 
    {
            if (pPack->m_pBuffer[i] == 0x7F) 
            {
                    ++bt7fCount;
            }
    }

    if (bt7fCount > 0) 
    {
            pPack->m_Len += bt7fCount;
            for (i = pPack->m_Len - 1 - bt7fCount; i >= C2R_DATA_BEGIN; --i) 
            {
                    
                    if (pPack->m_pBuffer[i] == 0x7F) 
                    {
                            pPack->m_pBuffer[i + bt7fCount] = 0x81;
                            --bt7fCount;
                    }
                    // ????
                    if (bt7fCount == 0) 
                    {
                            break;
                    }
                    pPack->m_pBuffer[i + bt7fCount] = pPack->m_pBuffer[i];
            } // end for
    } // end if
    pPack->m_pBuffer[pPack->m_Len] = 0x7F;
    pPack->m_pBuffer[pPack->m_Len + 1] = 0x7F;
    pPack->m_Len += 2;
}
BYTE btValue[5];
BYTE Data[200];
void ProcessUpCmd(All_PACK *pack)
{
    DWORD dwStart;
    uint32_t flashdestination;
    BYTE btCmd, i;
    DWORD dwAddress;
    BYTEFromPack((*pack), btCmd);
    BYTEFromPack((*pack), btCmd);

    InitPack(SendPack);
    BYTEFromPack((*pack), btCmd);
    switch(btCmd)
    {
    case CMD_GET_READER_APPVERSION:
       BYTEFromPack((*pack), btValue[0]);
        BYTEFromPack((*pack), btValue[1]);
        BYTEFromPack((*pack), btValue[2]);
        BYTEFromPack((*pack), btValue[3]);
        BYTEFromPack((*pack), btValue[4]);
        if (btValue[0] == 'B' && btValue[1] == 'L' && btValue[2] == 'K' && btValue[3] == 'E' && btValue[4] == 'Y')
        {
            PUSHPackHeader(SendPack);
            BYTEToPack(SendPack, CMD_GET_READER_APPVERSION);
            BYTEToPack(SendPack, 0x00);
            for (i=0; i<20; i++)
            {
                BYTEToPack(SendPack, Data[i]);
            }
        }else{
            PUSHPackHeader(SendPack);
            BYTEToPack(SendPack, CMD_GET_READER_APPVERSION);
            BYTEToPack(SendPack, 0x01);
        }
        FinishPack(&SendPack);        				
        SendUp(&SendPack);
        break;
    case CMD_WRITE_UPDATE_DATA:
        BYTEFromPack((*pack), btValue[0]);
        DWORDFromPack((*pack), dwAddress);
        BYTEFromPack((*pack), btValue[1]);
        for (i=0; i<btValue[1]; i++)
        {
            BYTEFromPack((*pack), Data[i]);
        }        
        flashdestination = APPLICATION_ADDRESS+dwAddress;
        Data[0] = FLASH_If_Write(&flashdestination, (uint32_t *)Data, btValue[1]/4);
        
        PUSHPackHeader(SendPack);
        BYTEToPack(SendPack, CMD_WRITE_UPDATE_DATA);
        BYTEToPack(SendPack, Data[0]);
        DWORDToPack(SendPack, dwAddress);
        FinishPack(&SendPack);
        SendUp(&SendPack);
        break;
    case CMD_JUMP_APPLICATION:
        BYTEFromPack((*pack), btValue[0]);
        BYTEFromPack((*pack), btValue[1]);
        BYTEFromPack((*pack), btValue[2]);
        BYTEFromPack((*pack), btValue[3]);
        BYTEFromPack((*pack), btValue[4]);
        if (btValue[0] == 'B' && btValue[1] == 'L' && btValue[2] == 'K' && btValue[3] == 'E' && btValue[4] == 'Y')
        {
            PUSHPackHeader(SendPack);
            BYTEToPack(SendPack, CMD_JUMP_APPLICATION);
            BYTEToPack(SendPack, 0x00);
        }else{
            PUSHPackHeader(SendPack);
            BYTEToPack(SendPack, CMD_JUMP_APPLICATION);
            BYTEToPack(SendPack, 0x01);
        }
        FinishPack(&SendPack);
        SendUp(&SendPack);

        FLASH_If_EnableWriteProtection(APPLICATION_ADDRESS);
        
        dwStart = LocalTime;
        while(LocalTime-dwStart<5000)
        {
        }
        NVIC_SystemReset();
        break;
    case CMD_ENTER_BOOTLOADER:
        BYTEFromPack((*pack), btValue[0]);
        BYTEFromPack((*pack), btValue[1]);
        BYTEFromPack((*pack), btValue[2]);
        BYTEFromPack((*pack), btValue[3]);
        BYTEFromPack((*pack), btValue[4]);
        if (btValue[0] == 'B' && btValue[1] == 'L' && btValue[2] == 'K' && btValue[3] == 'E' && btValue[4] == 'Y')
        {
            WORDFromPack((*pack), gGlobal.ReaderID);
            FLASH_If_DisableWriteProtection();
            FLASH_If_Erase(0);//wyh
            bOK = 1;
            PUSHPackHeader(SendPack);
            BYTEToPack(SendPack, CMD_ENTER_BOOTLOADER);
            BYTEToPack(SendPack, 0x00);
        }else{
            bOK = 0;
            PUSHPackHeader(SendPack);
            BYTEToPack(SendPack, CMD_ENTER_BOOTLOADER);
            BYTEToPack(SendPack, 0x01);
        }
        FinishPack(&SendPack);
        SendUp(&SendPack);
        break;
    default:
        break;
    }
}
void UARTLoop(COMMTYPE COM)
{
    unsigned char i,btPackIndex;
    btPackIndex = 0xFF;
    for (i=0;i<COM_CACHE_PACK_COUNT;i++)
    {
        if (gComSet[COM].m_RecvPack[gComSet[COM].m_PopIndex].m_Len > 0)
        {
            btPackIndex = gComSet[COM].m_PopIndex;
            gComSet[COM].m_PopIndex++;
            if (gComSet[COM].m_PopIndex >= COM_CACHE_PACK_COUNT)
            {
                gComSet[COM].m_PopIndex = 0;
            }
            break;
        }
        gComSet[COM].m_PopIndex++;
        if (gComSet[COM].m_PopIndex >= COM_CACHE_PACK_COUNT)
        {
            gComSet[COM].m_PopIndex = 0;
        }
    }
    if (btPackIndex != 0xFF)
    {
        ProcessUpCmd(&gComSet[COM].m_RecvPack[btPackIndex]);
        gComSet[COM].m_RecvPack[btPackIndex].m_Len = 0;
    }
}
/*
*********************************************************************************************************
*	�� �� ��: tcp_callback
*	����˵��: TCP Socket�Ļص�����
*	��    ��: soc  TCP Socket����
*             evt  �¼�����
*             ptr  �¼�������TCP_EVT_DATA��ptrָ��Ļ�������¼�Ž��յ���TCP���ݣ������¼���¼IP��ַ
*             par  �¼�������TCP_EVT_DATA����¼���յ������ݸ����������¼���¼�˿ں�
*	�� �� ֵ: 
*********************************************************************************************************
*/
U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par)
{
	char buf[50];
	uint16_t i;
	
	/* ȷ����socket_tcp�Ļص� */
	if (soc != socket_tcp) 
	{
		return (0);
	}

	switch (evt) 
	{
		/*
			Զ�̿ͻ���������Ϣ
		    1������ptr�洢Զ���豸��IP��ַ��par�д洢�˿ںš�
		    2��������ֵ1�������ӣ�������ֵ0��ֹ���ӡ�
		*/
		case TCP_EVT_CONREQ:
			sprintf(buf, "Զ�̿ͻ�����������IP: %d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
			printf_debug("IP:%s  port:%d\r\n", buf, par);
			return (1);
		
		/* ������ֹ */
		case TCP_EVT_ABORT:
			break;
		
		/* SocketԶ�������Ѿ����� */
		case TCP_EVT_CONNECT:
			printf_debug("Socket is connected to remote peer\r\n");
			break;
		
		/* ���ӶϿ� */
		case TCP_EVT_CLOSE:
		   	printf_debug("Connection has been closed\r\n");
			break;
		
		/* ���͵������յ�Զ���豸Ӧ�� */
		case TCP_EVT_ACK:
	  break;
		
		/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
		case TCP_EVT_DATA:
			printf_debug("Data length = %d\r\n", par);
			for(i = 0; i < par; i++)
			{
				  printf_debug("ptr[%d] = %d\r\n", i, ptr[i]);
				  ProcessData(gGlobal.CommType, ptr[i]);
			}
			break;
	}	
	return (0);
}

/*
*********************************************************************************************************
*	�� �� ��: TCP_StatusCheck
*	����˵��: ���TCP������״̬����Ҫ�������߲�ε��ж�
*	��    ��: ��
*	�� �� ֵ: __TRUE  ����
*             __FALSE �Ͽ�
*********************************************************************************************************
*/
uint8_t TCP_StatusCheck(void) 
{
	uint8_t res;
	
	switch (tcp_get_state(socket_tcp)) 
	{
		case TCP_STATE_FREE:
		case TCP_STATE_CLOSED:
			res = tcp_listen (socket_tcp, PORT_NUM);
			printf_debug("tcp listen res = %d\r\n", res);
			break;
		
		case TCP_STATE_LISTEN:
			break;
		
		case TCP_STATE_CONNECT:
			return (__TRUE);
			
		default:  
			break;
	}
	
	return (__FALSE);
}

/*
*********************************************************************************************************
*	�� �� ��: tcpnet_poll
*	����˵��: ʹ��TCPnet����Ҫһֱ���õĺ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void tcpnet_poll(void)
{
	if(bsp_CheckTimer(0))
	{
		/* �˺�����������Է����ж������� */
		timer_tick ();
	}	
	main_TcpNet ();
}

/*
*********************************************************************************************************
*	�� �� ��: TCPnetTest
*	����˵��: TCPnetӦ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TCPnetTest(void)
{  
	uint8_t tcp_status;
	uint8_t res,i;
	uint8_t ucKeyCode;
	FLASH_If_Init();
	/* ��ʼ������Э��ջ */
	init_TcpNet ();
	/* 
	   ����TCP Socket�������������ͻ������ӷ�������10����������ͨ�Ž��Ͽ����ӡ�
	   ������������ʹ����TCP_TYPE_KEEP_ALIVE����һֱ�������ӣ�����10���ʱ�����ơ�
	*/
  socket_tcp = tcp_get_socket (TCP_TYPE_SERVER | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
	if(socket_tcp != 0)
	{
		 res = tcp_listen (socket_tcp, PORT_NUM);
		 printf_debug("tcp listen res = %d\r\n", res);
	}
	/* ����һ��������100ms����ʱ�� */
  bsp_StartAutoTimer(0, 100);	
	while (1) 
	{
		if (bOK==0)
		{
			 if (LocalTime>10000)//�����ʱ
			 {
						//�ر�����
						tcp_close (socket_tcp);
						//������������λ
						NVIC_SystemReset();
			 }
		}
		/* TCP��ѯ */
		tcpnet_poll();		
		/* �������߲�εĴ��� */
		tcp_status = TCP_StatusCheck();
		ucKeyCode = bsp_GetKey();
		if ((ucKeyCode != KEY_NONE)&&(tcp_status == __TRUE))
		{
				switch (ucKeyCode)
				{
					case KEY_DOWN_K1:
					{
						 for(i=0;i<8;i++)
						 {
							  BYTEToPack(SendPack, i);;
						 }
						 SendUp(&SendPack);
						 InitPack(SendPack);
					}
					break;
					default:
					{
						;
					}
					break;
				}
			
		}			
		if (gGlobal.CommType != 0xFF)
		{
        UARTLoop(gGlobal.CommType);
		}

	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
