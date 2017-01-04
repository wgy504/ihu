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
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_L);
	strcat(output->buf, zIhuSysEngPar.cloud.cloudBhServerName);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_R);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L);
	strcat(output->buf, zIhuSysEngPar.cloud.cloudBhIhuName);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_R);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L);
	//time(0)：取得RTC时间的方式，通过跟BSP相连，得到解决了
	UINT32 timeStamp = ihu_l1hd_rtc_get_current_unix_time(); 
  sprintf(s, "%d", timeStamp);
	strcat(output->buf, s);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_R);
	
	//Message Type content
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L);
	if      (msgType == HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_STRING);
	else {
		IhuErrorPrint("SPSVIRGO: Error Message Type input!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_R);
	
	//Content starting
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_L);

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
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_R);
	
	//固定尾部分
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L);
	if (funcFlag == NULL) strcat(output->buf, "0");
	else strcat(output->buf, funcFlag);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_R);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_R);

	//存入返回参量中：这个长度域其实也没有太大的用处
	output->bufferLen = strlen(s);

	//返回
	return IHU_SUCCESS;
}

//解码接收到的消息
//该消息以CHAR为单位，从纯CDATA模式修改为<xml>格式，所以需要加入这个内容
OPSTAT func_cloud_standard_xml_unpack(msg_struct_ccl_com_cloud_data_rx_t *rcv)
{
	UINT32 index=0, msgId=0, msgLen=0;
	char tmp[5] = "";
	UINT8 tt[HUITP_MSG_BUF_BODY_ONLY_MAX_LEN];
	//int ret = 0;
	int i = 0, dif = 0;
	char *pIndexT1, *pIndexT2;  //临时位置
	UINT8 msgType;
	UINT64 msgCreateTime;
	char msgToUser[IHU_FILE_NAME_LENGTH_MAX], msgFromUser[IHU_FILE_NAME_LENGTH_MAX], msgFuncFlag[IHU_FILE_NAME_LENGTH_MAX];
	char msgTmp[IHU_FILE_NAME_LENGTH_MAX];
	char msgContent[HUITP_MSG_BUF_WITH_HEAD_MAX_LEN];
	
	//检查参数
	if (rcv == NULL){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid received data buffer!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	//控制命令不带XML格式头，接收的内容以裸控制命令，所以必须是偶数字节
	if ((rcv->length > MAX_IHU_MSG_BUF_LENGTH_CLOUD) || (rcv->length < 8) || (rcv->length > (HUITP_MSG_BUF_BODY_ONLY_MAX_LEN*2))){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid data length by XML content format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	
	//寻找<xml>的头部
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L))>= pIndexT2) || (dif > MAX_IHU_MSG_BUF_LENGTH_CLOUD)){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	
	//寻找<ToUserName>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_TO_USER_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_TO_USER_L))>= pIndexT2) || (dif > IHU_FILE_NAME_LENGTH_MAX)){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memset(msgToUser, 0, sizeof(msgToUser));
	i = 0;
	pIndexT1 += strlen(HUITP_MSG_HUIXML_CONSTANT_TO_USER_L); //去掉头部
	while(pIndexT1 < pIndexT2){
		msgToUser[i++] = *pIndexT1++;
	}	
	if (strcmp(msgToUser, zIhuSysEngPar.cloud.cloudBhIhuName) !=0){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid toUser field!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;		
	}
	
	//寻找<fromUserName>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L))>= pIndexT2) || (dif > IHU_FILE_NAME_LENGTH_MAX)){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memset(msgFromUser, 0, sizeof(msgFromUser));
	i = 0;
	pIndexT1 += strlen(HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L); //去掉头部
	while(pIndexT1 < pIndexT2){
		msgFromUser[i++] = *pIndexT1++;
	}	
	if (strcmp(msgFromUser, zIhuSysEngPar.cloud.cloudBhServerName) !=0){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid fromUser field!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;		
	}	
	
	//寻找<CreateTime>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L))>= pIndexT2) || (dif > HUITP_MSG_HUIXML_CONTSANT_CREATE_TIME_MAX_LEN)){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memset(msgTmp, 0, sizeof(msgTmp));
	i = 0;
	pIndexT1 += strlen(HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L); //去掉头部
	while(pIndexT1 < pIndexT2){
		msgTmp[i++] = *pIndexT1++;
	}
	msgCreateTime = strtoul(msgTmp, NULL, 10); //10进制，并非16进制
	//暂时不判定，存下即可，以后再完善
	if (msgCreateTime == 0) {}
	
	//寻找<msgType>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L))>= pIndexT2) || (dif > IHU_FILE_NAME_LENGTH_MAX)){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memset(msgTmp, 0, sizeof(msgTmp));
	i = 0;
	pIndexT1 += strlen(HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L); //去掉头部
	while(pIndexT1 < pIndexT2){
		msgTmp[i++] = *pIndexT1++;
	}
	if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_ID;
	else{
		IhuErrorPrint("SPSVIRGO: Received message error, invalid format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;	
	}
	//暂时不用，存下即可，以后再完善	
	if (msgType == HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID){}

	//寻找<funcFlag>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L))>= pIndexT2) || (dif > IHU_FILE_NAME_LENGTH_MAX)){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memset(msgFuncFlag, 0, sizeof(msgFuncFlag));
	i = 0;
	pIndexT1 += strlen(HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L); //去掉头部
	while(pIndexT1 < pIndexT2){
		msgFuncFlag[i++] = *pIndexT1++;
	}
	//msgFuncFlag的用途未来待定
	if (msgFuncFlag[0] == '0') {}
	
	//寻找<Content>：长度必须是偶数
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_CONTENT_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_CONTENT_L))>= pIndexT2) ||\
		(dif > HUITP_MSG_BUF_WITH_HEAD_MAX_LEN) || (dif != ((dif/2)*2))){
		IhuErrorPrint("SPSVIRGO: Received message error, invalid format!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	memset(msgContent, 0, sizeof(msgContent));
	i = 0;
	pIndexT1 += strlen(HUITP_MSG_HUIXML_CONSTANT_CONTENT_L); //去掉头部
	while(pIndexT1 < pIndexT2){
		msgContent[i++] = *pIndexT1++;
	}
		
	//msgContent中解出msgId/msgLen
	index = 0;
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, &msgContent[index], 4);
	msgId = strtoul(tmp, NULL, 16);
	index = index + 4;
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, &msgContent[index], 4);
	msgLen = strtoul(tmp, NULL, 16);
	if ((msgId < HUITP_MSGID_uni_min) || (msgId > HUITP_MSGID_uni_max)){
		IhuErrorPrint("SPSVIRGO: Invalid received data msgId info!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;	
	}
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
	//转码，从CHAR进制转化为16进制
	for(index = 4; index < dif/2; index++){
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp, &msgContent[index * 2], 2);
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


#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
		//心跳请求
		case HUITP_MSGID_uni_heart_beat_req:
		{
			StrMsg_HUITP_MSGID_uni_heart_beat_req_t *snd1;
			memset(snd1, 0, sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd1 = (StrMsg_HUITP_MSGID_uni_heart_beat_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_heart_beat_req_received_handle(snd1);
		}
			break;

		//心跳证实
		case HUITP_MSGID_uni_heart_beat_confirm:
		{
			StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t *snd2;
			memset(snd2, 0, sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd2 = (StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_heart_beat_confirm_received_handle(snd2);			
		}
			break;
		
		case HUITP_MSGID_uni_ccl_lock_req:	
		{
			StrMsg_HUITP_MSGID_uni_ccl_lock_req_t *snd3;
			memset(snd3, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd3 = (StrMsg_HUITP_MSGID_uni_ccl_lock_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_lock_req_received_handle(snd3);	
		}
			break;
			
		case HUITP_MSGID_uni_ccl_lock_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t *snd4;
			memset(snd4, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd4 = (StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_lock_confirm_received_handle(snd4);
		}
			break;
			
		case HUITP_MSGID_uni_ccl_lock_auth_resp:
		{
			StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t *snd5;
			memset(snd5, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd5 = (StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_lock_auth_resp_received_handle(snd5);			
		}
			break;
			
		case HUITP_MSGID_uni_ccl_door_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_door_req_t *snd6;
			memset(snd6, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd6 = (StrMsg_HUITP_MSGID_uni_ccl_door_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_door_req_received_handle(snd6);			
		}
			break;
		
		case HUITP_MSGID_uni_ccl_door_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t *snd7;
			memset(snd7, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd7 = (StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_door_confirm_received_handle(snd7);						
		}
			break;
				
		case HUITP_MSGID_uni_ccl_rfid_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t *snd8;
			memset(snd8, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd8 = (StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_rfid_req_received_handle(snd8);						
		}
			break;

		case HUITP_MSGID_uni_ccl_rfid_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t *snd9;
			memset(snd9, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd9 = (StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_rfid_confirm_received_handle(snd9);						
		}
			break;

		case HUITP_MSGID_uni_ccl_ble_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_ble_req_t *snd10;
			memset(snd10, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_ble_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_ble_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd10 = (StrMsg_HUITP_MSGID_uni_ccl_ble_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_ble_req_received_handle(snd10);					
		}
			break;

		case HUITP_MSGID_uni_ccl_ble_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t *snd11;
			memset(snd11, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd11 = (StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_ble_confirm_received_handle(snd11);						
		}
			break;

		case HUITP_MSGID_uni_ccl_gprs_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t *snd12;
			memset(snd12, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd12 = (StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_gprs_req_received_handle(snd12);						
		}
			break;

		case HUITP_MSGID_uni_ccl_gprs_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t *snd13;
			memset(snd13, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd13 = (StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_gprs_confirm_received_handle(snd13);						
		}
			break;

		case HUITP_MSGID_uni_ccl_battery_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_battery_req_t *snd14;
			memset(snd14, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_battery_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_battery_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd14 = (StrMsg_HUITP_MSGID_uni_ccl_battery_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_battery_req_received_handle(snd14);							
		}
			break;

		case HUITP_MSGID_uni_ccl_battery_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t *snd15;
			memset(snd15, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd15 = (StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_battery_confirm_received_handle(snd15);								
		}
			break;

		case HUITP_MSGID_uni_ccl_shake_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_shake_req_t *snd16;
			memset(snd16, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_shake_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_shake_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd16 = (StrMsg_HUITP_MSGID_uni_ccl_shake_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_shake_req_received_handle(snd16);					
		}
			break;

		case HUITP_MSGID_uni_ccl_shake_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t *snd17;
			memset(snd17, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd17 = (StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_shake_confirm_received_handle(snd17);							
		}
			break;

		case HUITP_MSGID_uni_ccl_smoke_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t *snd18;
			memset(snd18, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd18 = (StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_smoke_req_received_handle(snd18);								
		}
			break;

		case HUITP_MSGID_uni_ccl_smoke_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t *snd19;
			memset(snd19, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd19 = (StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_smoke_confirm_received_handle(snd19);						
		}
			break;

		case HUITP_MSGID_uni_ccl_water_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_water_req_t *snd20;
			memset(snd20, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_water_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_water_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd20 = (StrMsg_HUITP_MSGID_uni_ccl_water_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_water_req_received_handle(snd20);				
		}
			break;

		case HUITP_MSGID_uni_ccl_water_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t *snd21;
			memset(snd21, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd21 = (StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_water_confirm_received_handle(snd21);							
		}
			break;

		case HUITP_MSGID_uni_ccl_temp_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_temp_req_t *snd22;
			memset(snd22, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_temp_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_temp_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd22 = (StrMsg_HUITP_MSGID_uni_ccl_temp_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_temp_req_received_handle(snd22);				
		}
			break;

		case HUITP_MSGID_uni_ccl_temp_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t *snd23;
			memset(snd23, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd23 = (StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_temp_confirm_received_handle(snd23);							
		}
			break;

		case HUITP_MSGID_uni_ccl_humid_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_humid_req_t *snd24;
			memset(snd24, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_humid_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_humid_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd24 = (StrMsg_HUITP_MSGID_uni_ccl_humid_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_humid_req_received_handle(snd24);						
		}
			break;

		case HUITP_MSGID_uni_ccl_humid_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t *snd25;
			memset(snd25, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd25 = (StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_humid_confirm_received_handle(snd25);								
		}
			break;

		case HUITP_MSGID_uni_ccl_fall_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_fall_req_t *snd26;
			memset(snd26, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_fall_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_fall_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd26 = (StrMsg_HUITP_MSGID_uni_ccl_fall_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_fall_req_received_handle(snd26);						
		}
			break;

		case HUITP_MSGID_uni_ccl_fall_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t *snd27;
			memset(snd27, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd27 = (StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_fall_confirm_received_handle(snd27);
		}
			break;

		case HUITP_MSGID_uni_ccl_state_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_state_req_t *snd28;
			memset(snd28, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd28 = (StrMsg_HUITP_MSGID_uni_ccl_state_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_state_req_received_handle(snd28);			
		}
			break;

		case HUITP_MSGID_uni_ccl_state_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t *snd29;
			memset(snd29, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd29 = (StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_ccl_state_confirm_received_handle(snd29);						
		}
			break;

		case HUITP_MSGID_uni_inventory_req:			
		{
			StrMsg_HUITP_MSGID_uni_inventory_req_t *snd30;
			memset(snd30, 0, sizeof(StrMsg_HUITP_MSGID_uni_inventory_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_inventory_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd30 = (StrMsg_HUITP_MSGID_uni_inventory_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_inventory_req_received_handle(snd30);			
		}
			break;

		case HUITP_MSGID_uni_inventory_confirm:
		{
			StrMsg_HUITP_MSGID_uni_inventory_confirm_t *snd31;
			memset(snd31, 0, sizeof(StrMsg_HUITP_MSGID_uni_inventory_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_inventory_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd31 = (StrMsg_HUITP_MSGID_uni_inventory_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_inventory_confirm_received_handle(snd31);					
		}
			break;		

		case HUITP_MSGID_uni_sw_package_req:
		{
			StrMsg_HUITP_MSGID_uni_sw_package_req_t *snd32;
			memset(snd32, 0, sizeof(StrMsg_HUITP_MSGID_uni_sw_package_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_sw_package_req_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd32 = (StrMsg_HUITP_MSGID_uni_sw_package_req_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_sw_package_req_received_handle(snd32);							
		}
			break;		

		case HUITP_MSGID_uni_sw_package_confirm:
		{
			StrMsg_HUITP_MSGID_uni_sw_package_confirm_t *snd33;
			memset(snd33, 0, sizeof(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t) - 4)){
				zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
				IhuErrorPrint("SPSVIRGO: Error unpack message on length!\n");
				return IHU_FAILURE;				
			}
			snd33 = (StrMsg_HUITP_MSGID_uni_sw_package_confirm_t *)(pMsgBuf);
			func_cloud_spsvirgo_ccl_msg_sw_package_confirm_received_handle(snd33);						
		}
			break;
		
#endif
		
		default:
		{
			zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
			IhuErrorPrint("SPSVIRGO: Receive unknown message id and not able to process!\n");
			return IHU_FAILURE;			
		}
			//break;
	}	

	//返回
	return IHU_SUCCESS;
}

/*
OPSTAT func_cloudvela_standard_xml_heart_beat_req_msg_processing(StrMsg_HUITP_MSGID_uni_heart_beat_req_t *rcv)
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








