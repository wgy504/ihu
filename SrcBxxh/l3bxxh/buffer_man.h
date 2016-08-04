/****************************** TCP/GUI MESSAGE INTERFACE STARTT *********************************/
/* 2014/12/15 MYC
** ===================== Usage Part 1: .h file ====================
*/
/* 2014/12/15 MYC
** .h   // put the following section into a .h file
*/

#include "vmlayer.h"

#define	        AMT_MAINLOOP_GUI_BUFFER_SIZE 	                128
#define	        AMT_MAX_NUMBER_OF_OVERFLOW_BEFORE_REINIT 			8

typedef struct AmtMainLoopGuiMsgBuffer
{
    uint8_t    Buf[AMT_MAINLOOP_GUI_BUFFER_SIZE];

    uint32_t   FirstByteToScan;
    uint32_t   NextByteToSave;

    int32_t    LastByteSendToGuiToProcess;
    int32_t    LastByteGuiProcessed;

    uint32_t	 NumPacketTcpReceived;
    uint32_t	 NumPacketTcpDiscard;
    uint32_t	 NumMsgTcpProcessed;

}AmtMainLoopMsgBuffer_t;

typedef struct MsgAcsHeader
{
    uint8_t    start;
	  uint8_t		 version;
		uint16_t	 len;
	  uint16_t   msgId;
	  uint16_t   seqNum;
}MsgAcsHeader_t;

#define 				ACS_MSG_SOP																		(0xFE)
#define 				ACS_MSG_HEADER_LENTH													sizeof(MsgAcsHeader_t)
	
extern AmtMainLoopMsgBuffer_t 	gAmtMainLoopMsgBuffer;
extern uint16_t asy_seqid;

/* External APIs for message echange between TCP and GUI Thread */
void AmtMainLoopGuiMsgBufferInit(AmtMainLoopMsgBuffer_t *atmb);
void AmtMainLoopGuiMsgBufferPut(uint8_t *ptrMsg, uint32_t MsgSize, AmtMainLoopMsgBuffer_t *atmb);
void UpdateLastByteSendToGuiToProcess(AmtMainLoopMsgBuffer_t *atmb, uint8_t *pMsgTcpSendToGui, uint32_t MsgSize);
void UpdateLastByteGuiProcessed(AmtMainLoopMsgBuffer_t *atmb, uint8_t *pMsgGuiJustProcessed, uint32_t MsgSize);
void AirSyncMessageProcessing( uint8_t *pAcsMsgHeader, uint16_t len );

/* Internal API, not do used by TCP and GUI Thread */
uint8_t IaValidMsgHeader(uint8_t *Msg);


/******************************** TCP/GUI MESSAGE INTERFACE END ***********************************/
