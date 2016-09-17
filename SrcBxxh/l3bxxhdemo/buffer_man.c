/*
**
**
*/
#include "buffer_man.h"
#include "bxxh_adapt.h"
#include "l3_asylibra_task.h"
#include "i2c_led.h"

/*
** 2014/12/15 MYC
** Copy the mesasge to be sent to GUI from Mainloop, save to the buffer in AMT while is shared between MainLoop and GUI thread
** The mesasge could be either from TCP socket, or generated via AMT MainLoop itself.
**
** 1. Buffer Definition: AMT_MAINLOOP_GUI_BUFFER_SIZE byte array
** 2. Shift buffer, not ring buffer, if the bytes has been porcessed/used by TCP/GUI,
**    remaining bytes will be shift to the beginning of the buffer
**
** Initial: |-------------------------------------------------------------------------------------------|
**           0                                                                                         AMT_MAINLOOP_GUI_BUFFER_SIZE byte
**
** ===== Input/Save =====
** Case 1:  Receive one socket packet:
**     IN:  |====================| size = TcpSize
**    OUT:  |====================|----------------------------------------------------------------------|
**
** Case 2:  Receive 2nd socket packet
**    OUT:  |====================|=====================================================|----------------|
**
** Case 3:  Receive 3rd socket packet, buffer full, discasrd
**     IN:  |==============================|
**
** ===== Scan/Output =====
** The whole buffer will be scanned after each called of this function, following cases:
** Case 1:  |====================|===================================|----------------------------------|
**                valid                          valid
**
** Case 2:  |====================|=====================================================|----------------|
**                valid              invalid header, search for valid header, if nothing found, discard all
**
** Case 3:  |====================|===================================|===================---------------|
**                valid                        valid                     valid header, partial, length not enough, wait for reminging bytes
**                                                                             |
**                   |<<<<<<<<<<<<<<<<<<<<<---copy---<<<<<<<<<<<<<<<<<<<<<<<<<<|
**                   |
**                keep the valid header and patial msg
**          |==================|------------------------------------------------------------------------|
**
** NOTE: This is NOT a Ring buffer, as nearly all of the messsage from ACS has valid frame structure, and patial mesasge moving should
**       NOT happend frequently.
**
** Case 4:  |====================|xxxxxxxxxxxxxxxx|==================|=================----------------|
**                valid             wasted bytes       valid              valid header, patial
**
** ===== Interface to GUI msg Q =====
** pass pointer to each valid message to pend in the TCPToGUIMsgQ, but need to wait for GUI process to free/occupy to save new
**
** Four variables to save the status of the buffer, and also the communication between TCP and GUI thread.
**
**
**                                                                  | LastByteSendToGuiToProcess
**                                                                  |
**          |====================|===================================|==================----------------|
**           0                    1                                   2                 |
**                              |                                     |                 | NextByteToSave
**                              | LastByteGuiProcessed                | FirstByteToScan
**
** if (LastByteGuiProcessed == LastByteSendToGuiToProcess)
**    Clear All Buffer
** Else
**    Move unprcessed message to the Begining of the Buffer
**
*/

/* 2014/12/15 MYC
** ===================== Usage Part 0: Overall ====================
*/
/*
**
** 2.1 In MainLoop Thread:
** Call AmtMainLoopGuiMsgBufferInit(AmtMainLoopMsgBuffer_t *atmb) at init
** Call AmtMainLoopGuiMsgBufferPut(uint8_t *ptrMsg, uint32_t MsgSize, AmtMainLoopMsgBuffer_t *atmb) for each 
**      1) TCP scoket received
**      2) Message generated from MainLoop
**
** MAKE SURE AFTER PUT TO Q:
**      UpdateLastByteSendToGuiToProcess(AmtMainLoopMsgBuffer_t *atmb, uint8_t *pMsgTcpSendToGui, uint32_t MsgSize);
**
** 2.2 In GUI Thread:
** After GUI Get message from Q (p_msg = OSQPend(&TCPtoGUIMsgQ, 1, OS_OPT_PEND_NON_BLOCKING, &msg_size, &ts, &os_err);), 
**      Update with:
**      UpdateLastByteGuiProcessed(AmtMainLoopMsgBuffer_t *atmb, uint8_t *pMsgGuiJustProcessed, uint32_t MsgSize);
**
**      THIS HAS ONLY TO BE ADDED FOR MESSAGE FROM MainLoop via AmtMainLoopGuiMsgBufferPut() !!!!!
**      IF THE MESSAGE IS PUT INTO Q DIRECTLY, NOT VIA AmtMainLoopGuiMsgBufferPut(), DO NOT CALL UpdateLastByteGuiProcessed()
*/

///* 2014/12/15 MYC
//** ===================== Usage Part 1: .h file ====================
//*/
///* 2014/12/15 MYC
//** .h   // put the following section into a .h file
//*/
//
//#define	        AMT_MAINLOOP_GUI_BUFFER_SIZE 	4096
//
//typedef struct AmtMainLoopGuiMsgBuffer
//{
//    uint8_t    Buf[AMT_MAINLOOP_GUI_BUFFER_SIZE];
//
//    uint32_t   FirstByteToScan;
//    uint32_t   NextByteToSave;
//
//    int32_t    LastByteSendToGuiToProcess;
//    int32_t    LastByteGuiProcessed;
//
//    uint32_t	 NumPacketTcpReceived;
//    uint32_t	 NumPacketTcpDiscard;
//    uint32_t	 NumMsgTcpProcessed;
//}AmtMainLoopMsgBuffer_t;
//
//extern AmtMainLoopMsgBuffer_t 	gAmtMainLoopMsgBuffer;
//
///* External APIs for message echange between TCP and GUI Thread */
//void AmtMainLoopGuiMsgBufferInit(AmtMainLoopMsgBuffer_t *atmb);
//void AmtMainLoopGuiMsgBufferPut(uint8_t *ptrMsg, uint32_t MsgSize, AmtMainLoopMsgBuffer_t *atmb);
//void UpdateLastByteSendToGuiToProcess(AmtMainLoopMsgBuffer_t *atmb, uint8_t *pMsgTcpSendToGui, uint32_t MsgSize);
//void UpdateLastByteGuiProcessed(AmtMainLoopMsgBuffer_t *atmb, uint8_t *pMsgGuiJustProcessed, uint32_t MsgSize);
//
///* Internal API, not do used by TCP and GUI Thread */
//uint8_t IaValidMsgHeader(uint8_t *Msg);

/* 2014/12/15 MYC
** ===================== Usage Part 2: .c file ====================
**
*/

/* 2014/12/15 MYC
** .c // put the following section into a .c file
*/

/* Global variable declaration */
AmtMainLoopMsgBuffer_t 	gAmtMainLoopMsgBuffer;

/* External APIs */
void AmtMainLoopGuiMsgBufferInit(AmtMainLoopMsgBuffer_t *atmb)
{
    if(NULL == atmb)
    {
        bxxh_print_console("AmtMainLoopGuiMsgBufferInit: NULL == atmb.\r\n, return");
        return;
    }

    memset((void *)atmb->Buf, AMT_MAINLOOP_GUI_BUFFER_SIZE, 0);

    atmb->FirstByteToScan = 0;
    atmb->NextByteToSave = 0;

    atmb->LastByteSendToGuiToProcess = -1;       //update in MainLoop
    atmb->LastByteGuiProcessed = -1;             //update in GUI

    atmb->NumPacketTcpReceived = 0;
    atmb->NumPacketTcpDiscard = 0;
    atmb->NumMsgTcpProcessed = 0;

    return;
}

/* 2014/12/15 MYC */
/*
** Core API for TCP thread to save the TCP sock packet into the buffer, and scan the buffer
*/
void AmtMainLoopGuiMsgBufferPut(uint8_t *ptrMsg, uint32_t MsgSize, AmtMainLoopMsgBuffer_t *atmb)
{

    uint32_t          offset;
    uint32_t          ii;
    //OS_ERR            err;
    MsgAcsHeader_t    *pAcsMsgHeader;

    if((NULL == atmb) || (NULL == ptrMsg))
    {
        bxxh_print_console("AmtMainLoopGuiMsgBufferPut: NULL == atmb, or NULL == ptrMsg.\r\n, return");
        return;
    }

    if(0 == MsgSize)
    {
        bxxh_print_console("AmtMainLoopGuiMsgBufferPut: 0 == MsgSize.\r\n, return");
        return;
    }

    if(atmb->LastByteGuiProcessed > atmb->LastByteSendToGuiToProcess)
    {
        sprintf(pDebugPrintBuf, "AmtMainLoopGuiMsgBufferPut: FATAL ERROR, LastByteGuiProcessed(%d)>LastByteSendToGuiToProcess(%d).\r\n, return",
                       atmb->LastByteGuiProcessed > atmb->LastByteSendToGuiToProcess, 1);
        bxxh_print_console(pDebugPrintBuf);
        return;
    }

    /* for debug, should be remove after tested */
//    bxxh_print_console("AmtMainLoopGuiMsgBufferPut:START: ToScan=%d, ToSave=%d, Post=%d, Proc=%d, #Recv=%d, #Discd=%d, %Proc=%d\r\n",
//            atmb->FirstByteToScan, atmb->NextByteToSave, atmb->LastByteSendToGuiToProcess, atmb->LastByteGuiProcessed,
//            atmb->NumPacketTcpReceived, atmb->NumPacketTcpDiscard, atmb->NumMsgTcpProcessed);

    /* Step 1: at each of the packet received from TCP socekt, buffer roll back to the beginning of the buffer if needed */
    if(atmb->LastByteGuiProcessed == atmb->LastByteSendToGuiToProcess)
    {
        if( atmb->FirstByteToScan == atmb->NextByteToSave )
        {
            atmb->LastByteGuiProcessed = -1;
            atmb->LastByteSendToGuiToProcess = -1;
            atmb->FirstByteToScan = 0;
            atmb->NextByteToSave = 0;
        }
        else
        {
            /* move the remaining of the buffer to the beginning of the buffer */
            for(ii = atmb->FirstByteToScan; ii < atmb->NextByteToSave; ii++)
                atmb->Buf[ii - atmb->FirstByteToScan] = atmb->Buf[ii];

            atmb->LastByteGuiProcessed = -1;
            atmb->LastByteSendToGuiToProcess = -1;
            atmb->NextByteToSave = atmb->NextByteToSave - atmb->FirstByteToScan;
            atmb->FirstByteToScan = 0;
        }
    }

    /* for debug, should be remove after tested */
//    bxxh_print_console("AmtMainLoopGuiMsgBufferPut: ROLL: ToScan=%d, ToSave=%d, Post=%d, Proc=%d, #Recv=%d, #Discd=%d, %Proc=%d\r\n",
//            atmb->FirstByteToScan, atmb->NextByteToSave, atmb->LastByteSendToGuiToProcess, atmb->LastByteGuiProcessed,
//            atmb->NumPacketTcpReceived, atmb->NumPacketTcpDiscard, atmb->NumMsgTcpProcessed);

    /* Step 2: check whether the new TCP packet will overflow the buffer */
    if(atmb->NextByteToSave + MsgSize > AMT_MAINLOOP_GUI_BUFFER_SIZE)
    {
        atmb->NumPacketTcpReceived = atmb->NumPacketTcpReceived + 1;
        atmb->NumPacketTcpDiscard = atmb->NumPacketTcpDiscard + 1;
			
        sprintf(pDebugPrintBuf, "AmtMainLoopGuiMsgBufferPut: Buffer Overflow, NextByteToSave(%d) + MsgSize(%d) > AMT_MAINLOOP_GUI_BUFFER_SIZE(%d), NumPacketTcpDiscard = %d.\r\n, return",
                       atmb->NextByteToSave, MsgSize, AMT_MAINLOOP_GUI_BUFFER_SIZE, atmb->NumPacketTcpDiscard);
				bxxh_print_console(pDebugPrintBuf);
        
        if(atmb->NumPacketTcpDiscard >= AMT_MAX_NUMBER_OF_OVERFLOW_BEFORE_REINIT) 
        {
            AmtMainLoopGuiMsgBufferInit(atmb); 
            sprintf(pDebugPrintBuf, "AmtMainLoopGuiMsgBufferPut: Buffer Overflow Reach Max, NextByteToSave(%d) + MsgSize(%d) > AMT_MAINLOOP_GUI_BUFFER_SIZE(%d), NumPacketTcpDiscard = %d.\r\n, CLEAR ALL BUFFER, return",
                       atmb->NextByteToSave, MsgSize, AMT_MAINLOOP_GUI_BUFFER_SIZE, atmb->NumPacketTcpDiscard);
						bxxh_print_console(pDebugPrintBuf);
        }
        return;
    }

    /* for debug, should be remove after tested */
//    bxxh_print_console("AmtMainLoopGuiMsgBufferPut:CHECK: ToScan=%d, ToSave=%d, Post=%d, Proc=%d, #Recv=%d, #Discd=%d, %Proc=%d\r\n",
//            atmb->FirstByteToScan, atmb->NextByteToSave, atmb->LastByteSendToGuiToProcess, atmb->LastByteGuiProcessed,
//            atmb->NumPacketTcpReceived, atmb->NumPacketTcpDiscard, atmb->NumMsgTcpProcessed);

    /* Step 3: Saved in the Buffer */
    memcpy(atmb->Buf + atmb->NextByteToSave, ptrMsg, MsgSize);
    atmb->NumPacketTcpReceived = atmb->NumPacketTcpReceived + 1;                //increase the counter
    atmb->NextByteToSave = atmb->NextByteToSave + MsgSize;             //move the offset

    /* for debug, should be remove after tested */
//    bxxh_print_console("AmtMainLoopGuiMsgBufferPut: SAVE: ToScan=%d, ToSave=%d, Post=%d, Proc=%d, #Recv=%d, #Discd=%d, %Proc=%d\r\n",
//            atmb->FirstByteToScan, atmb->NextByteToSave, atmb->LastByteSendToGuiToProcess, atmb->LastByteGuiProcessed,
//            atmb->NumPacketTcpReceived, atmb->NumPacketTcpDiscard, atmb->NumMsgTcpProcessed);

    /* Step 4: Scan in the Buffer */
    for(offset = atmb->FirstByteToScan; offset < atmb->NextByteToSave; offset++)
    {
        if( TRUE == IaValidMsgHeader(atmb->Buf + offset) )
        {
            /* check whether the packet is complete */
            pAcsMsgHeader = (MsgAcsHeader_t *)(atmb->Buf + offset);
            sprintf(pDebugPrintBuf, "A valid hdr, offset(%d), MsgId=0x%04x, len=%d\r\n",
                              offset, b2l_uint16(pAcsMsgHeader->msgId), b2l_uint16(pAcsMsgHeader->len));
					  bxxh_print_console(pDebugPrintBuf);
                            
            //if( offset + pAcsMsgHeader->len + ACS_MSG_HEADER_LENTH <= atmb->NextByteToSave )
						if( offset + b2l_uint16(pAcsMsgHeader->len) <= atmb->NextByteToSave )
            {
                sprintf(pDebugPrintBuf, "A valid msg, offset(%d), MsgId=0x%04x, len=%d, Header=%d\r\n",
                              offset, b2l_uint16(pAcsMsgHeader->msgId), b2l_uint16(pAcsMsgHeader->len), pAcsMsgHeader);
								bxxh_print_console(pDebugPrintBuf);

                /* decode message and put to Q if needed */
								GLOBAL_INT_DISABLE();
                AirSyncMessageProcessing((uint8_t *)pAcsMsgHeader, (uint16_t)(b2l_uint16(pAcsMsgHeader->len)));
								GLOBAL_INT_RESTORE();

                /* Increase the counter */
                atmb->NumMsgTcpProcessed = atmb->NumMsgTcpProcessed + 1;  //increase the counter

                /* The below update HAS to put after message put into the Q */
                /* atmb->LastByteSendToGuiToProcess = offset + pAcsMsgHeader->len + ACS_MSG_HEADER_LENTH - 1; */

                /* directly jump to the start of next mesasge, -1 to let for to increase offset */
                offset = offset + b2l_uint16(pAcsMsgHeader->len) - 1;

                /* Schedule GUI Thread of Possible */
                //OSTimeDlyHMSM(0, 0, 0, 1, OS_OPT_TIME_DLY, &err);
            }
            else /* should be a partial received TCP message with header */
            {
                /* !!!! Break the loop, and no need to search anymore */
								//sprintf(pDebugPrintBuf, "AmtMainLoopGuiMsgBufferPut: SCAN: offset=%d, partial message received\r\n", offset);
								//bxxh_print_console(pDebugPrintBuf);
						
								//sprintf(pDebugPrintBuf, "AmtMainLoopGuiMsgBufferPut: Only a valid message header found in offset(%d), MsgId=0x%04x, len=%d\r\n",
								//						offset, b2l_uint16(pAcsMsgHeader->msgId), b2l_uint16(pAcsMsgHeader->len));
								//bxxh_print_console(pDebugPrintBuf);
                break;
            }
        }
        else
        {
//            sprintf(pDebugPrintBuf,"Scan Offset = %d\r\n", offset);
//						bxxh_print_console(pDebugPrintBuf);
            /* No msg header found, do nothing */
        }

    }/* end of for() search */

    /* Step 5: save to where we have just scanned, for next time */
    atmb->FirstByteToScan = offset;
//    sprintf(pDebugPrintBuf, "AmtMainLoopGuiMsgBufferPut: SCAN: offset=%d, atmb->FirstByteToScan = offset;\r\n", offset);
//		bxxh_print_console(pDebugPrintBuf);
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

    /* for debug, should be remove after tested */
//    bxxh_print_console("AmtMainLoopGuiMsgBufferPut:  END: ToScan=%d, ToSave=%d, Post=%d, Proc=%d, #Recv=%d, #Discd=%d, %Proc=%d\r\n",
//            atmb->FirstByteToScan, atmb->NextByteToSave, atmb->LastByteSendToGuiToProcess, atmb->LastByteGuiProcessed,
//            atmb->NumPacketTcpReceived, atmb->NumPacketTcpDiscard, atmb->NumMsgTcpProcessed);
        
}


/* 2014/12/15 MYC */
/* gAmtMainLoopiMsgBuffer.LastByteSendToGuiToProcess = (uint32_t)(pMsgHeader) - (uint32_t)(gAmtMainLoopiMsgBuffer.Buf) + rx_size - 1; */
void UpdateLastByteSendToGuiToProcess(AmtMainLoopMsgBuffer_t *atmb, uint8_t *pMsgTcpSendToGui, uint32_t MsgSize)
{
    uint32_t offset;

    if(NULL == atmb)
    {
        bxxh_print_console("UpdateLastByteSendToGuiToProcess: NULL == atmb.\r\n, return");
        return;
    }

    if(NULL == pMsgTcpSendToGui)
    {
        bxxh_print_console("UpdateLastByteSendToGuiToProcess: NULL == pMsgTcpSendToGui.\r\n, return");
        return;
    }

    /* A protection is added here to avoid get an invalid value of LastByteSendToGuiToProcess */
    offset = (uint32_t)(pMsgTcpSendToGui) - (uint32_t)(atmb->Buf) + MsgSize - 1;
    if(offset < AMT_MAINLOOP_GUI_BUFFER_SIZE)
    {
        atmb->LastByteSendToGuiToProcess = offset;
    }
		
//		sprintf(pDebugPrintBuf, "LastByteSendToGuiToProcess=%d\r\n", atmb->LastByteSendToGuiToProcess);
//		bxxh_print_console(pDebugPrintBuf);

}

/* 2014/12/15 MYC */
/* gAmtMainLoopiMsgBuffer.LastByteGuiProcessed = (uint32_t)(p_msg) - (uint32_t)(gAmtMainLoopiMsgBuffer.Buf) + msg_size - 1; */
void UpdateLastByteGuiProcessed(AmtMainLoopMsgBuffer_t *atmb, uint8_t *pMsgGuiJustProcessed, uint32_t MsgSize)
{
    uint32_t offset;
    
    if(NULL == atmb)
    {
        bxxh_print_console("UpdateLastByteGuiProcessed: NULL == atmb.\r\n, return");
        return;
    }

    if(NULL == pMsgGuiJustProcessed)
    {
        bxxh_print_console("UpdateLastByteGuiProcessed: NULL == pMsgGuiJustProcessed.\r\n, return");
        return;
    }

    /* A protection is added here to avoid get an invalid value of LastByteGuiProcessed */
    offset = (uint32_t)(pMsgGuiJustProcessed) - (uint32_t)(atmb->Buf) + MsgSize - 1;
    if(offset < AMT_MAINLOOP_GUI_BUFFER_SIZE)
    {
        atmb->LastByteGuiProcessed = offset;
    }
		
//		sprintf(pDebugPrintBuf, "LastByteGuiProcessed=%d\r\n", atmb->LastByteGuiProcessed);
//		bxxh_print_console(pDebugPrintBuf);
}

/*
void AcsCmdProcessing((uint8_t *)pAcsMsgHeader, (uint16_t)(pAcsMsgHeader->len + ACS_MSG_HEADER_LENTH));
*/

void AirSyncMessageProcessing( uint8_t *pMsg, uint16_t len )
{

		MsgAcsHeader_t *pAcsMsgHeader = (MsgAcsHeader_t *)pMsg;
	
	if (b2l_uint16(pAcsMsgHeader->msgId) == ECI_resp_auth) //auth_resp
	{
		//涉及到内存COPY的操作，都会导致任务调度不成功，从而丢失消息，应该是堆栈溢出或者其他类似问题，一般来说，需要独立做MEMALLOC，而不是直接使用内存拷贝
		//这里直接采用原始PARAM的内存，反而不会出错，因为这个指针来自于最底层，通过KE_MSG_ALLOC，确保内存地址不出错
		//其它的Param指针对应的操作都不成功，没办法，只能将该数据结构做成跟最低成一样的结构，才可以直接套用
		//首先处理SEQ
		bxxh_print_console("ASYLIBRA: received auth_resp, seqid=");
        
        sprintf(pDebugPrintBuf, "%02X ", b2l_uint16(pAcsMsgHeader->seqNum));
        bxxh_print_console(pDebugPrintBuf);
		//bxxh_print_console(hexword2string(asy_seqid));	
		
        sprintf(pDebugPrintBuf, "Len=%d ", b2l_uint16(pAcsMsgHeader->len));
        bxxh_print_console(pDebugPrintBuf);
        
        bxxh_print_console(" \r\n");
		asy_seqid++;
		//再发送消息给处理函数
		bxxh_message_send(MSG_ASYLIBRA_AUTH_RESP, TASK_ASYLIBRA, TASK_ASYLIBRA, (struct msg_struct_asylibra_auth_resp *)pAcsMsgHeader, len);
	}
	else if (b2l_uint16(pAcsMsgHeader->msgId) == ECI_resp_init) //init_resp
	{
		//首先处理SEQ
		bxxh_print_console("ASYLIBRA: received init_resp, seqid=");
        
        sprintf(pDebugPrintBuf, "%02X ", b2l_uint16(pAcsMsgHeader->seqNum));
        bxxh_print_console(pDebugPrintBuf);
		//bxxh_print_console(hexword2string(asy_seqid));	
        
        sprintf(pDebugPrintBuf, "Len=%d ", b2l_uint16(pAcsMsgHeader->len));
        bxxh_print_console(pDebugPrintBuf);
        
		bxxh_print_console(" \r\n");			
		asy_seqid++;
		//再发送消息给处理函数
		
		/* ASYLIBRA FULL CONNECTED */
		bxxh_led_off(LED_ID_7);
		
		bxxh_message_send(MSG_ASYLIBRA_INIT_RESP, TASK_ASYLIBRA, TASK_ASYLIBRA, (struct msg_struct_asylibra_init_resp *)pAcsMsgHeader, len);
	}
	//这里解析出的数据业务消息，应该直接发给L3-WXAPP处理，但对应的ASYLIBRA-IE部分，需要这里处理
	//在往L3-WXAPP发送数据时，必要的信息将通过消息中的自定义字段带上去
	//简化处理，先！
	else if (b2l_uint16(pAcsMsgHeader->msgId) == ECI_resp_sendData) // data_resp
	{
		//首先处理SEQ
		//先检查收到的RESP是否等于系统中设置的SEQ，如果不等于，说明有问题
		//系统采用最简单的WAIT-STOP机制，意味着上一次的REQ对应的RESP没有收到，则下一次的REQ不能发出，至于由此导致的消息丢失等问题，全部由高层自行处理，链路层不保证		
		bxxh_print_console("ASYLIBRA: received data_resp, seqid=");
        
        sprintf(pDebugPrintBuf, "%02X ", b2l_uint16(pAcsMsgHeader->seqNum));
        bxxh_print_console(pDebugPrintBuf);
		//bxxh_print_console(hexword2string(asy_seqid));

        sprintf(pDebugPrintBuf, "Len=%d ", b2l_uint16(pAcsMsgHeader->len));
        bxxh_print_console(pDebugPrintBuf);
        
		bxxh_print_console(" \r\n");
		//这里的SEQID可能不需要增加1，再仔细考究
		asy_seqid++;
		//再发送消息给处理函数
		bxxh_message_send(MSG_ASYLIBRA_DATA_RESP, TASK_ASYLIBRA, TASK_ASYLIBRA, (struct msg_struct_asylibra_data_resp *)pAcsMsgHeader, len);
	} //nCmdid == 0x224E
	else if (b2l_uint16(pAcsMsgHeader->msgId) == ECI_push_recvData) // data_push
	{
		//先解码，然后将纯数据发送到高层 (其实，这个函数有问题，还是得依靠手工解码先)
		//RecvDataPush *recv = epb_unpack_recv_data_push(param->value, param->length);
		//epb_unpack_recv_data_push_free(recv);		
		//消息头部分，只需要检查一下seq是否等于0，其它的没有特别需要处理的东西
		if (b2l_uint16(pAcsMsgHeader->seqNum) != 0){
			bxxh_print_console("ASYLIBRA: Receive Data_Push Seq not zero. \r\n");
		}
		//即使SEQ!=0，依然不影响数据的处理，但会送出打印告警
		struct msg_struct_asylibra_data_push m;
		m.UserCmdId = pMsg[12];
		m.length = pMsg[13]; //长度为2个字节的情形先不考虑
		if (m.length <=ASY_DATA_PUSH_LENGTH_MAX){
			memcpy(&m.data[0], &pMsg[14], m.length);
		}else{
			bxxh_print_console("ASYLIBRA: Received wrong length push data! \r\n");
		}
		//增加JSAPI微信界面H5会话操作功能，就是一直看到的18 00 OR 18 02 27 11
		//使用编解码函数将会更加准确，这里使用傻瓜解码方式，可能会出错的，特别是IE字段顺序被WXAPP调整的话
		uint16_t nManufacture = 0;  //Default为厂商自定义数据
		if ((pMsg[14 + m.length] == TAG_RecvDataPush_Type) && (pMsg[15 + m.length] == 2))
		{
			nManufacture = (pMsg[16 + m.length] <<8) & 0xFF00 + (pMsg[17 + m.length]) & 0xFF;
			if (nManufacture == EDDT_wxDeviceHtmlChatView)
			{
				m.EmDeviceDataType = EDDT_wxDeviceHtmlChatView;
			}
			else 
			{
				m.EmDeviceDataType = EDDT_manufatureSvr;			
			}
		}
		else
		{
			m.EmDeviceDataType = EDDT_manufatureSvr;
		}		
		//将内容体发送到WXAPP进行处理
        sprintf(pDebugPrintBuf, "ASYLIBRA: push_recvData Len=%d\r\n", b2l_uint16(pAcsMsgHeader->len));
        bxxh_print_console(pDebugPrintBuf);

		bxxh_message_send(MSG_ASYLIBRA_DATA_PUSH, TASK_WXAPP, TASK_ASYLIBRA, &m, sizeof(m));
		//依然在连接态，故而继续保持，确保状态的稳定性
		ke_state_set(TASK_ASYLIBRA, STATE_ASYLIBRA_INIT_OK_DATA_READY);
	}
	else
	{
		//收到错误的无法处理的消息
		bxxh_print_console("ASYLIBRA: Receive un-recoginized message, buffer=");
		int i=0;
		for (i=0; i<10; i++)
		{
			sprintf(pDebugPrintBuf, "%02X ", pMsg[i]);
            bxxh_print_console(pDebugPrintBuf);
            //bxxh_print_console(hexbyte2string(param->value[i]));
 
            //bxxh_print_console(" ");
		}
        
        sprintf(pDebugPrintBuf, "Len=%d ", b2l_uint16(pAcsMsgHeader->len));
        bxxh_print_console(pDebugPrintBuf);
		bxxh_print_console(" \r\n");
	}
			
			//As all message will copied to a new buffer newly allocated, we consider the masage could be delete from buffer man 
			UpdateLastByteSendToGuiToProcess(&gAmtMainLoopMsgBuffer, (uint8_t *)pMsg, (uint32_t)len);
			UpdateLastByteGuiProcessed(&gAmtMainLoopMsgBuffer, (uint8_t *)pMsg, (uint32_t)len);
}

/* Internal API */
/* 2014/12/15 MYC */
uint8_t IaValidMsgHeader(uint8_t *Msg)
{
    MsgAcsHeader_t 	*pAcsMsgHeader;

    if(NULL == Msg)
        return FALSE;

    pAcsMsgHeader = (MsgAcsHeader_t *)Msg;
		
//		sprintf(pDebugPrintBuf, "IaValidMsgHeader: start = 0x%02X, MsgId=0x%04x, len=%d\r\n", pAcsMsgHeader->start, pAcsMsgHeader->msgId, pAcsMsgHeader->len);
//		bxxh_print_console(pDebugPrintBuf);

    if( ACS_MSG_SOP == pAcsMsgHeader->start )
    {
        switch (b2l_uint16(pAcsMsgHeader->msgId))
        {
        	  /* =========================================================================*/
            /* CASES for MESSAGE GENERATE FROM ACS via TCP SOCKET NEED TO BE ADDED HERE */
            /* =========================================================================*/
							case ECI_resp_auth:
                if(MSG_PAYLOAD_LEN_ECI_RESP_AUTH == b2l_uint16(pAcsMsgHeader->len))
										//bxxh_print_console("Found ECI_resp_auth\r\n");
                    return TRUE;
                else
                    return FALSE;
								//break;
					
							case ECI_resp_init:
                if(MSG_PAYLOAD_LEN_ECI_RESP_INIT != b2l_uint16(pAcsMsgHeader->len))
										//bxxh_print_console("Found ECI_resp_init\r\n");
                    return TRUE;
                else
                    return FALSE;								
								//break;
								
							case ECI_resp_sendData:
                if(MSG_PAYLOAD_LEN_ECI_RESP_SENDDATA != b2l_uint16(pAcsMsgHeader->len))
										//bxxh_print_console("Found ECI_resp_sendData\r\n");
                    return TRUE;
                else
                    return FALSE;
								//break;								

							case ECI_push_recvData:
                if(MSG_PAYLOAD_LEN_ECI_RESP_RECVDATA != b2l_uint16(pAcsMsgHeader->len))
										//bxxh_print_console("Found ECI_push_recvData\r\n");
                    return TRUE;
                else
                    return FALSE;								
								//break;
								
//            case MSG_ACS_GET_CONFIG_RESP:
//                if(MSG_PAYLOAD_LEN_GET_CONFIG_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_SET_CONFIG_RESP:
//                if(MSG_PAYLOAD_LEN_SET_CONFIG_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_START_RESP:
//                if(MSG_PAYLOAD_LEN_START_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_STOP_RESP:
//                if(MSG_PAYLOAD_LEN_STOP_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_TEST_RESP:
//                if(MSG_PAYLOAD_LEN_TEST_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_GET_RECIPE_RESP:
//                if(MSG_PAYLOAD_LEN_GET_RECIPE_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_SET_RECIPE_RESP:
//                if(MSG_PAYLOAD_LEN_SET_RECIPE_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_GET_STAT_RESP:
//                if(MSG_PAYLOAD_LEN_GET_STAT_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_STAT_AUTO_REPORT:
//                if(MSG_PAYLOAD_LEN_STAT_AUTO_REPORT == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_DOOR_LIGHT_REPORT:
//                if(MSG_PAYLOAD_LEN_DOOR_LIGHT_REPORT == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_GLOBAL_STATUS_REPORT:
//                if(MSG_PAYLOAD_LEN_GLOBAL_STATUS_REPORT == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_SET_TIME_RESP:
//                if(MSG_PAYLOAD_LEN_SET_TIME_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_SENSOR_COMMAND_RESP:
//                if(MSG_PAYLOAD_LEN_SENSOR_COMMAND_RESP == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;

//            case MSG_ACS_ERROR_REPORT:
//                if(MSG_PAYLOAD_LEN_ERROR_REPORT == pAcsMsgHeader->len)
//                    return TRUE;
//                else
//                    return FALSE;
            /* ===================================================================*/
            /* CASES for MESSAGE GENERATE FROM AMT MAINLOOP NEED TO BE ADDED HERE */
            /* ===================================================================*/
            default:
//							  sprintf(pDebugPrintBuf, "Invalid MsgId=%d\r\n", b2l_uint16(pAcsMsgHeader->msgId));
//								bxxh_print_console(pDebugPrintBuf);
                return FALSE;
        }

    }
    return FALSE;
}
