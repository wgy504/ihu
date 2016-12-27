/**
 ****************************************************************************************
 *
 * @file huixmlcodec.c
 *
 * @brief HUIXMLCODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
#include "huixmlcodec.h"

//Task Global variables
extern IhuSysEngParTable_t zIhuSysEngPar; //全局工程参数控制表

//XML自定义标准的编码函数方式
//inputLen：这是包括MsgHead在内的所有缓冲区长度，正常情况下=sizeof(StrMsg_HUITP_MSGID_uni_general_message_t)，或者IE_BODY+4
OPSTAT func_cloud_standard_xml_pack(UINT8 msgType, char *funcFlag, UINT16 msgId, StrMsg_HUITP_MSGID_uni_general_message_t *inputPar, UINT16 inputLen, CloudDataSendBuf_t *output)
{
	//声明一个缓冲区长度，不能超越消息体内容的最长长度
	char s[MAX_IHU_MSG_BUF_LENGTH_CLOUD - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN];
	UINT8 tt[HUITP_MSG_BUF_BODY_ONLY_MAX_LEN];
	
	//参数检查：特别要检查输入的数据长度，正常不能超过100，因为HUIXML的数据区= (500(最长消息长度)-300(XML头))/2=100，这在大多数情况下都够用的，但在
	//文件传输的条件下有可能不够。幸好，文件下载使用FFP模式，不用再担心这个了。
	if ((inputLen <4) || (inputPar == NULL) || (inputLen > (MAX_IHU_MSG_BUF_LENGTH_CLOUD - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)/2) || \
		(inputLen > (sizeof(StrMsg_HUITP_MSGID_uni_general_message_t) - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)))
	{
		IhuErrorPrint("SPSVIRGO: Error input pointer or message length!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	if (output == NULL)
	{
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Error CloudDataSendBuf_t pointer!\n");
		return IHU_FAILURE;
	}
	
	//准备输出缓冲区
	memset(output, 0, sizeof(CloudDataSendBuf_t));

	//格式固定区域
	strcat(output->buf, "<xml>");
	strcat(output->buf, "<ToUserName><![CDATA[");
	strcat(output->buf, zIhuSysEngPar.cloud.cloudBhServerName);
	strcat(output->buf, "]]></ToUserName>");
	strcat(output->buf, "<FromUserName><![CDATA[");
	strcat(output->buf, zIhuSysEngPar.cloud.cloudBhIhuName);
	strcat(output->buf, "]]></FromUserName>");
	strcat(output->buf, "<CreateTime>");
	//time(0); 如何取得时间戳，待完成，FreeRTOS里暂时没找到合适取得时间戳的方法
	UINT32 timeStamp = (UINT32)__TIME__; 
  sprintf(s, "%d", timeStamp);
	strcat(output->buf, s);
	strcat(output->buf, "</CreateTime>");
	
	//Message Type content
	strcat(output->buf, "<MsgType><![CDATA[");
	if      (msgType == IHU_CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcat(output->buf, IHU_CLOUD_BH_MSG_TYPE_DEVICE_REPORT_STRING);
	else if (msgType == IHU_CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcat(output->buf, IHU_CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
	else if (msgType == IHU_CLOUD_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcat(output->buf, IHU_CLOUD_BH_MSG_TYPE_HEAT_BEAT_STRING);
	else if (msgType == IHU_CLOUD_BH_MSG_TYPE_BIZ_ITG_UINT8) strcat(output->buf, IHU_CLOUD_BH_MSG_TYPE_BIZ_ITG_STRING);
	else if (msgType == IHU_CLOUD_BH_MSG_TYPE_ALARM_REPORT_UINT8) strcat(output->buf, IHU_CLOUD_BH_MSG_TYPE_ALARM_REPORT_STRING);
	else if (msgType == IHU_CLOUD_BH_MSG_TYPE_PM_REPORT_UINT8) strcat(output->buf, IHU_CLOUD_BH_MSG_TYPE_PM_REPORT_STRING);
	else {
		IhuErrorPrint("SPSVIRGO: Error Message Type input!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	strcat(output->buf, "]]></MsgType>");
	
	//Content starting
	strcat(output->buf, "<Content><![CDATA[");

	//筛选出变长的消息结构，独立进行处理，剩下的统一处理
	switch(msgId)
	{
		case HUITP_MSGID_uni_ccl_lock_report:
			if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_lock_report structure!\n");
				return IHU_FAILURE;
			}
			else if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意inputLen跟着系统配置的IHU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER走
			}
			break;
			
		case HUITP_MSGID_uni_ccl_door_report:
			if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_door_report structure!\n");
				return IHU_FAILURE;
			}
			else if ((IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意inputLen跟着系统配置的IHU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER走				
			}	
			break;
			
		case HUITP_MSGID_uni_ccl_state_report:
			if ((IHU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (IHU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_state_report structure!\n");
				return IHU_FAILURE;
			}
			else if ((IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//将StrIe_HUITP_IEID_uni_ccl_door_state_t移上来
				memset(tt, 0, sizeof(tt));
				memcpy(tt, inputPar+4+sizeof(StrIe_HUITP_IEID_uni_com_report_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t), IHU_CCL_SENSOR_LOCK_NUMBER_MAX);
				memcpy(inputPar+4+sizeof(StrIe_HUITP_IEID_uni_com_report_t)+IHU_CCL_SENSOR_LOCK_NUMBER_MAX, tt, IHU_CCL_SENSOR_LOCK_NUMBER_MAX);
				//将剩下的移上来
				memset(tt, 0, sizeof(tt));
				memcpy(tt, inputPar+4+sizeof(StrIe_HUITP_IEID_uni_com_report_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_door_state_t), \
					inputLen-4-sizeof(StrIe_HUITP_IEID_uni_com_report_t)-2*IHU_CCL_SENSOR_LOCK_NUMBER_MAX);
				memcpy(inputPar+4+sizeof(StrIe_HUITP_IEID_uni_com_report_t)+ 2*IHU_CCL_SENSOR_LOCK_NUMBER_MAX, tt, \
					inputLen-4-sizeof(StrIe_HUITP_IEID_uni_com_report_t)-2*IHU_CCL_SENSOR_LOCK_NUMBER_MAX);
				//移动之后，将末尾清0
				memset(inputPar+inputLen, 0, (HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER + HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER - 2*IHU_CCL_SENSOR_LOCK_NUMBER_MAX));
			}
			break;
			
		default:
			break;
	}
	
	//准备接龙字符串成为一整串
	memset(s, 0, sizeof(s));
	int i = 0;
	char tmp[3]="";
	for (i=0; i<inputLen; i++){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02X", (UINT8*)(inputPar+i));
		strcat(s, tmp);
	}
	if ((strlen(s) < 4) || (strlen(s) > (MAX_IHU_MSG_BUF_LENGTH_CLOUD - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN))){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: No data to be pack or too long length of data content %d!\n", strlen(s));
		return IHU_FAILURE;
	}
	//消息BODY的长度已经在msgLen中，不需要再填入，已经由上层在生成消息的时候填好了，所以这里不再需要再行统计
	strcat(output->buf, s);	

	//Finish content part
	strcat(output->buf, "]]></Content>");
	
	//固定尾部分
	strcat(output->buf, "<FuncFlag>");
	if (funcFlag == NULL) strcat(output->buf, "0");
	else strcat(output->buf, funcFlag);
	strcat(output->buf, "</FuncFlag>");
	strcat(output->buf, "</xml>");

	//存入返回参量中：这个长度域其实也没有太大的用处
	output->bufferLen = strlen(s);

	//返回
	return IHU_SUCCESS;
}

//解码接收到的消息
OPSTAT func_cloud_standard_xml_unpack(msg_struct_ccl_com_cloud_data_rx_t *rcv)
{
	UINT32 index=0, msgId=0, msgLen=0;
	char tmp[5] = "";
	UINT8 tt[HUITP_MSG_BUF_BODY_ONLY_MAX_LEN];
	//int ret = 0;

	//检查参数
	if (rcv == NULL){
		IhuErrorPrint("SPSVIRGO: Invalid received data buffer!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	//控制命令不带XML格式头，接收的内容以裸控制命令，所以必须是偶数字节
	if (((rcv->length %2) != 0) || (rcv->length > MAX_IHU_MSG_BUF_LENGTH_CLOUD) || (rcv->length < 8) || (rcv->length > (HUITP_MSG_BUF_BODY_ONLY_MAX_LEN*2))){
		IhuErrorPrint("SPSVIRGO: Received invalid data length by XML content format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	
	//解出msgId/msgLen
	index = 0;
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, &rcv->buf[index], 4);
	msgId = strtoul(tmp, NULL, 16);
	index = index + 4;
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, &rcv->buf[index], 4);
	msgLen = strtoul(tmp, NULL, 16);
	if (msgLen > (MAX_IHU_MSG_BUF_LENGTH_CLOUD - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)/2){
		IhuErrorPrint("SPSVIRGO: Invalid received data msgLen info!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	index = index + 4;
	
	//解码到目标缓冲区
	StrMsg_HUITP_MSGID_uni_general_message_t *pMsgBuf;
	memset(pMsgBuf, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	pMsgBuf->msgId.cmdId = (msgId>>8)&0xFF;
	pMsgBuf->msgId.optId = msgId&0xFF;
	pMsgBuf->msgLen = msgLen;

	for(index = 4; index < (rcv->length)/2; index++){
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp, &rcv->buf[index * 2], 2);
		pMsgBuf->data[index-4] = strtoul(tmp, NULL, 16) & 0xFF;
	}
	
	//通过msgId将变长的消息结构进行独立处理
	//筛选出变长的消息结构，独立进行处理，剩下的统一处理
	switch(msgId)
	{
		//解码接收不可能收到这个消息，这里只是展示处理技巧
		case HUITP_MSGID_uni_ccl_lock_resp:
			if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_lock_report structure!\n");
				return IHU_FAILURE;
			}
			else if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意跟着系统配置的IHU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER走
				if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_resp_t) - 4 - (HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - IHU_CCL_SENSOR_LOCK_NUMBER_MAX))){
					zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
					IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
					return IHU_FAILURE;					
				}
				pMsgBuf->msgLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_resp_t) - 4;
			}		
			break;
			
		//解码接收不可能收到这个消息，这里只是展示处理技巧
		case HUITP_MSGID_uni_ccl_door_resp:
			if (IHU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_door_report structure!\n");
				return IHU_FAILURE;
			}
			else if ((IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意跟着系统配置的IHU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER走
				if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_resp_t) - 4 - (HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - IHU_CCL_SENSOR_LOCK_NUMBER_MAX))){
					zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
					IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
					return IHU_FAILURE;					
				}
				pMsgBuf->msgLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_resp_t) - 4;				
			}
			break;
			
		//解码接收不可能收到这个消息，这里只是展示处理技巧
		case HUITP_MSGID_uni_ccl_state_resp:
			if ((IHU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (IHU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_state_report structure!\n");
				return IHU_FAILURE;
			}
			else if ((IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (IHU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//将剩下的移上来
				if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_resp_t) - 4 - (HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER + HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - 2 * IHU_CCL_SENSOR_LOCK_NUMBER_MAX))){
					zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
					IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
					return IHU_FAILURE;					
				}
				//通过tt过度，不然有可能拷贝会自己覆盖自己
				memset(tt, 0, sizeof(tt));
				memcpy(tt, pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+ 2*IHU_CCL_SENSOR_LOCK_NUMBER_MAX,\
					sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_resp_t)-4-sizeof(StrIe_HUITP_IEID_uni_com_resp_t)-sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t) - sizeof(StrIe_HUITP_IEID_uni_ccl_door_state_t));
				memcpy(pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_door_state_t), tt, \
					sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_resp_t)-4-sizeof(StrIe_HUITP_IEID_uni_com_resp_t)-sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t) - sizeof(StrIe_HUITP_IEID_uni_ccl_door_state_t));
			
				//将StrIe_HUITP_IEID_uni_ccl_door_state_t移下去
				memset(tt, 0, sizeof(tt));
				memcpy(tt, pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+IHU_CCL_SENSOR_LOCK_NUMBER_MAX, IHU_CCL_SENSOR_LOCK_NUMBER_MAX);		
				memcpy(pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t), tt, IHU_CCL_SENSOR_LOCK_NUMBER_MAX);
				//将LOCK_IE/DOOR_IE空余部分清0
				memset(pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+IHU_CCL_SENSOR_LOCK_NUMBER_MAX, 0, \
					HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER - IHU_CCL_SENSOR_LOCK_NUMBER_MAX);
				memset(pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t)+IHU_CCL_SENSOR_LOCK_NUMBER_MAX, 0, \
					HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - IHU_CCL_SENSOR_LOCK_NUMBER_MAX);
				pMsgBuf->msgLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_resp_t) - 4;
			}
			break;
			
		case HUITP_MSGID_uni_sw_package_req:
			//因为只有一个边长IE，且IE正好处于最后一个结构部分，所以不需要干啥
			//将消息长度恢复到消息结构长度，以便下面统一处理
			pMsgBuf->msgLen = sizeof(StrMsg_HUITP_MSGID_uni_sw_package_req_t) - 4;
			break;
		
		case HUITP_MSGID_uni_sw_package_confirm:
			//因为只有一个边长IE，且IE正好处于最后一个结构部分，所以不需要干啥
			//将消息长度恢复到消息结构长度，以便下面统一处理
			pMsgBuf->msgLen = sizeof(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t) - 4;
			break;
		
		default:
			break;
	}
	
	//再来进行消息的统一处理
	switch(msgId)
	{
		//心跳请求
		case HUITP_MSGID_uni_heart_beat_req:
		{
			StrMsg_HUITP_MSGID_uni_heart_beat_req_t snd1;
			memset(&snd1, 0, sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_req_t));
		}
			break;

		//心跳证实
		case HUITP_MSGID_uni_heart_beat_confirm:
		{
		}
			break;
		
		case HUITP_MSGID_uni_ccl_lock_req:	
		{
		}
			break;
			
		case HUITP_MSGID_uni_ccl_lock_confirm:
		{
		}
			break;
			
		case HUITP_MSGID_uni_ccl_lock_auth_resp:
		{
		}
			break;
			
		case HUITP_MSGID_uni_ccl_door_req:
		{
		}
			break;
		
		case HUITP_MSGID_uni_ccl_door_confirm:
		{
		}
			break;
				
		case HUITP_MSGID_uni_ccl_rfid_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_rfid_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_ble_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_ble_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_gprs_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_gprs_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_battery_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_battery_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_shake_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_shake_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_smoke_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_smoke_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_water_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_water_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_temp_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_temp_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_humid_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_humid_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_fall_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_fall_confirm:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_state_req:
		{
		}
			break;

		case HUITP_MSGID_uni_ccl_state_confirm:
		{
		}
			break;

		default:
		{
		}
			break;
	}	
	
//	if (cmdId ==L3CI_heart_beat)
//	{
//		if (func_cloudvela_standard_xml_heart_beat_msg_unpack(rcv) == IHU_FAILURE){
//			zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
//			IhuErrorPrint("SPSVIRGO: Error unpack receiving message!\n");
//			return IHU_FAILURE;
//		}

//		return IHU_SUCCESS;
//	}

//	switch(cmdId)
//	{
//		case L3CI_emc:

//			if (func_cloudvela_standard_xml_emc_msg_unpack(rcv) == IHU_FAILURE){
//				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
//				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of EMC!\n");
//				return IHU_FAILURE;
//			}
//			break;

//		default:
//			zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
//			IhuErrorPrint("SPSVIRGO: Receive cloud data error with CmdId = %d\n", cmdId);
//			return IHU_FAILURE;

//			break;

//	}

	return IHU_SUCCESS;
}


/*
OPSTAT func_cloud_standard_xml_unpack(msg_struct_ccl_com_cloud_data_rx_t *rcv)
{
	UINT32 index=0, cmdId=0;
	//int ret = 0;

	//检查参数
	if (rcv == NULL){
		IhuErrorPrint("SPSVIRGO: Invalid received data buffer!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	//控制命令不带XML格式头，接收的内容以裸控制命令，所以必须是偶数字节
	if ((rcv->length %2) != 0){
		IhuErrorPrint("SPSVIRGO: Received invalid data length by XML content format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}

	//获取控制CMDID
	char tmp[3] = "";
	strncpy(tmp, &rcv->buf[index], 2);
	IhuDebugPrint("SPSVIRGO: received comId %s !\n", tmp);
	cmdId = strtoul(tmp, NULL, 16);

	if (cmdId ==L3CI_heart_beat)
	{
		if (func_cloudvela_standard_xml_heart_beat_msg_unpack(rcv) == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("SPSVIRGO: Error unpack receiving message!\n");
			return IHU_FAILURE;
		}

		return IHU_SUCCESS;
	}

	switch(cmdId)
	{
		case L3CI_emc:

			if (func_cloudvela_standard_xml_emc_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of EMC!\n");
				return IHU_FAILURE;
			}
			break;

		case L3CI_pm25:
			if (func_cloudvela_standard_xml_pm25_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of PM25!\n");
				return IHU_FAILURE;
			}
			break;

		case L3CI_winddir:
			if (func_cloudvela_standard_xml_winddir_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of WINDDIR!\n");
				return IHU_FAILURE;
			}
			break;

		case L3CI_windspd:
			if (func_cloudvela_standard_xml_windspd_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of WINDSPD!\n");
				return IHU_FAILURE;
			}
			break;

		case L3CI_temp:
			if (func_cloudvela_standard_xml_temp_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of TEMP!\n");
				return IHU_FAILURE;
			}
			break;

		case L3CI_humid:
			if (func_cloudvela_standard_xml_humid_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message HUMID!\n");
				return IHU_FAILURE;
			}
			break;

		case L3CI_noise:
			if (func_cloudvela_standard_xml_noise_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of NOISE!\n");
				return IHU_FAILURE;
			}
			break;


		case L3CI_hsmmp:
			if (func_cloudvela_standard_xml_hsmmp_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of HSMMP!\n");
				return IHU_FAILURE;
			}
			break;

		case L3CI_ihu_inventory:
			if (func_cloudvela_standard_xml_ihuinventory_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of SW INVENTORY!\n");
				return IHU_FAILURE;
			}
			break;

		case L3CI_sw_package:
			if (func_cloudvela_standard_xml_swpackage_msg_unpack(rcv) == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack receiving message of SW PACKAGE!\n");
				return IHU_FAILURE;
			}
			break;

		default:
			zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("SPSVIRGO: Receive cloud data error with CmdId = %d\n", cmdId);
			return IHU_FAILURE;

			break;

	}

	return IHU_SUCCESS;
}

OPSTAT func_cloudvela_standard_xml_heart_beat_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0;  //, cmdId=0
	char st[IHU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	//cmdId = L3CI_heart_beat;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<0) ||(len>MAX_IHU_MSG_BUF_LENGTH)){
		IhuErrorPrint("SPSVIRGO: Error unpack on length!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}

	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		IhuErrorPrint("SPSVIRGO: Error unpack on length, received length != embeded length!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}

	//没有了其它的字段，所以长度=0就对了
	if (len != 0){
		IhuErrorPrint("SPSVIRGO: Error unpack on length, HearT_beat frame, Len!=0!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}

	//暂时没有层2的帧号，所以不用对帧号进行处理

	//也许会有其它潜在的状态转移困惑，所以这里的ONLINE状态只是为了做一定的检查，防止出现各种奇怪现象，而不是为了设置状态
	if (func_cloudvela_heart_beat_received_handle() == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Heart_beat processing error!\n");
		return IHU_FAILURE;
	}

	return IHU_SUCCESS;
}


OPSTAT func_cloudvela_standard_xml_emc_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[IHU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_emc;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_IHU_MSG_BUF_LENGTH)){
		IhuErrorPrint("SPSVIRGO: Error unpack on length!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		IhuErrorPrint("SPSVIRGO: Error unpack on length!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_emc_min) || (it >= L3PO_emc_max)){
		IhuErrorPrint("SPSVIRGO: Error unpack on operation Id!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		IhuErrorPrint("SPSVIRGO: Error unpack on equipment Id!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_emc_data_req) || (optId == L3PO_emc_data_report)){

		//初始化发送函数
		msg_struct_cloudvela_emc_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloudvela_emc_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护IHU的安全执行
		//这里的格式是，CLOUD->IHU读取PM3:  25 02(len) 01(opt) 01(equ)
		//如果想测试，可以先把这里的严格检查放开，以方便测试
		if (len != 0 ){
			IhuErrorPrint("SPSVIRGO: Error unpack on message length!\n");
			zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
			return IHU_FAILURE;
		}

		//将消息命令发送给EMC任务
		snd.length = sizeof(msg_struct_cloudvela_emc_data_req_t);
		ret = ihu_message_send(MSG_ID_CLOUDVELA_EMC_DATA_REQ, TASK_ID_EMC, TASK_ID_SPSVIRGO, &snd, snd.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("EMC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[TASK_ID_EMC]);
			return IHU_FAILURE;
		}
	}

	else if ((optId == L3PO_emc_set_switch) || (optId == L3PO_emc_set_modbus_address) || (optId == L3PO_emc_set_work_cycle) || \
			(optId == L3PO_emc_set_sample_cycle) || (optId == L3PO_emc_set_sample_number) || (optId == L3PO_emc_read_switch) || \
			(optId == L3PO_emc_read_modbus_address) || (optId == L3PO_emc_read_work_cycle) || (optId == L3PO_emc_read_sample_cycle)\
			|| (optId == L3PO_emc_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloudvela_emc_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloudvela_emc_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.opt.equId = equId;

		if (optId == L3PO_emc_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护IHU的安全执行
			//这里的格式是，CLOUD->IHU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.powerOnOff = it;

			if (len != 0 ){
				IhuErrorPrint("SPSVIRGO: Error unpack on message length!\n");
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				return IHU_FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护IHU的安全执行
			//这里的格式是，CLOUD->IHU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.newEquId = it;

			if (len != 0 ){
				IhuErrorPrint("SPSVIRGO: Error unpack on message length!\n");
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				return IHU_FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护IHU的安全执行
			//这里的格式是，CLOUD->IHU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.workCycle = it;

			if (len != 0 ){
				IhuErrorPrint("SPSVIRGO: Error unpack on message length!\n");
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				return IHU_FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护IHU的安全执行
			//这里的格式是，CLOUD->IHU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.interSample = it;

			if (len != 0 ){
				IhuErrorPrint("SPSVIRGO: Error unpack on message length!\n");
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				return IHU_FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护IHU的安全执行
			//这里的格式是，CLOUD->IHU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.meausTimes = it;

			if (len != 0 ){
				IhuErrorPrint("SPSVIRGO: Error unpack on message length!\n");
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				return IHU_FAILURE;
			}
		}

		else if ((optId == L3PO_emc_read_switch) || (optId == L3PO_emc_read_modbus_address) || (optId == L3PO_emc_read_work_cycle) ||\
				(optId == L3PO_emc_read_sample_cycle) || (optId == L3PO_emc_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护IHU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				IhuErrorPrint("SPSVIRGO: Error unpack on message length!\n");
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				return IHU_FAILURE;
			}
		}

		//将消息命令发送给EMC任务
		snd1.length = sizeof(msg_struct_cloudvela_emc_control_cmd_t);
		ret = ihu_message_send(MSG_ID_CLOUDVELA_EMC_CONTROL_CMD, TASK_ID_EMC, TASK_ID_SPSVIRGO, &snd1, snd1.length);
		if (ret == IHU_FAILURE){
			zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("EMC: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_SPSVIRGO], zIhuTaskNameList[TASK_ID_EMC]);
			return IHU_FAILURE;
		}

	} //end of if complex ||||||


	else{
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Error unpack on operational Id!\n");
		return IHU_FAILURE;
	}

	return IHU_SUCCESS;
}
*/








