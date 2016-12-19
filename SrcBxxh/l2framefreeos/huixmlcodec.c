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
//完全自己手动编码的方式，未来可以灵活的改动
//也可以考虑使用XML的标准函数来做，但其实最大的内容部分，也是靠手工编码的，并没有省略多少复杂度
OPSTAT func_cloud_standard_xml_pack(CloudBhItfDevReportStdXml_t *xmlFormat, CloudDataSendBuf_t *buf)
{	
	//参数检查，其它参数无所谓
	if (xmlFormat == NULL){
		IhuErrorPrint("SPSVIRGO: Error CloudBhItfDevReportStdXml_t pointer!\n");
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}
	if (buf == NULL){
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		IhuErrorPrint("SPSVIRGO: Error CloudDataSendBuf_t pointer!\n");
		return IHU_FAILURE;
	}

	//格式固定区域
	strcpy(xmlFormat->xml_l, "<xml>");
	strcpy(xmlFormat->xml_r, "</xml>");
	strcpy(xmlFormat->ToUserName_l, "<ToUserName><![CDATA[");
	strcpy(xmlFormat->ToUserName_r, "]]></ToUserName>");
	strcpy(xmlFormat->FromUserName_l, "<FromUserName><![CDATA[");
	strcpy(xmlFormat->FromUserName_r, "]]></FromUserName>");
	strcpy(xmlFormat->CreateTime_l, "<CreateTime>");
	strcpy(xmlFormat->CreateTime_r, "</CreateTime>");
	strcpy(xmlFormat->MsgType_l, "<MsgType><![CDATA[");
	strcpy(xmlFormat->MsgType_r, "]]></MsgType>");
	strcpy(xmlFormat->Content_l, "<Content><![CDATA[");
	strcpy(xmlFormat->Content_r, "]]></Content>");
	strcpy(xmlFormat->FuncFlag_l, "<FuncFlag>");
	strcpy(xmlFormat->FuncFlag_r, "</FuncFlag>");

	//对于目前来说，数值固定内容
	strcpy(xmlFormat->ToUserName, zIhuSysEngPar.cloud.cloudBhServerName);
	strcpy(xmlFormat->FromUserName, zIhuSysEngPar.cloud.cloudBhIhuName);

	UINT32 timeStamp = 0; //time(0); 如果取得时钟，待定
  sprintf(xmlFormat->CreateTime, "%d", timeStamp);

	//MsgType参数，必须由调用函数填入，因为它才能知晓这是什么样的内容体
	//strcpy(xmlFormat->MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT);
	if (strlen(xmlFormat->FuncFlag) <=0 )	sprintf(xmlFormat->FuncFlag, "%1d", 0);

	//准备接龙字符串成为一整串
	char s[MAX_IHU_MSG_BUF_LENGTH];
	memset(s, 0, sizeof(s));
	char da[MAX_IHU_MSG_BUF_LENGTH];
	memset(da, 0, sizeof(da));
	char tmp[3] = "";

	//固定头部分
	strcat(s, xmlFormat->xml_l);
	strcat(s, xmlFormat->ToUserName_l);
	strcat(s, xmlFormat->ToUserName);
	strcat(s, xmlFormat->ToUserName_r);
	strcat(s, xmlFormat->FromUserName_l);
	strcat(s, xmlFormat->FromUserName);
	strcat(s, xmlFormat->FromUserName_r);
	strcat(s, xmlFormat->CreateTime_l);
	strcat(s, xmlFormat->CreateTime);
	strcat(s, xmlFormat->CreateTime_r);
	strcat(s, xmlFormat->MsgType_l);
	strcat(s, xmlFormat->MsgType);
	strcat(s, xmlFormat->MsgType_r);
	strcat(s, xmlFormat->Content_l);

	//顺序是编码的黄金规则，千万不能错，否则就无法解开了!!!
	//char conCmdId[3];
	strcat(s, xmlFormat->conCmdId);

	//所有变长部分
	//char conOptId[3]; //1B
	strcat(da, xmlFormat->conOptId);

	//char conEqpId[3];  //1B
	strcat(da, xmlFormat->conEqpId);

	//char conBackType[3]; //1B
	strcat(da, xmlFormat->conBackType);// by Shanchun: move it after EqpId to align with Pack sequence in Cloud

	//char conDataFormat[3]; //1B
	strcat(da, xmlFormat->conDataFormat);

	//char conEmc[5];   //2B
	strcat(da, xmlFormat->conEmc);

	//char conPm1d0[9];   //4B
	strcat(da, xmlFormat->conPm1d0);

	//char conPm2d5[9];   //4B
	strcat(da, xmlFormat->conPm2d5);

	//char conPm10d[9];   //4B
	strcat(da, xmlFormat->conPm10d);

	//char conWinddir[5];   //2B
	strcat(da, xmlFormat->conWinddir);

	//char conWindspd[5];   //2B
	strcat(da, xmlFormat->conWindspd);

	//char conTemp[5];   //2B
	strcat(da, xmlFormat->conTemp);

	//char conHumid[5];   //2B
	strcat(da, xmlFormat->conHumid);

	//char conNoise[9];   //4B
	strcat(da, xmlFormat->conNoise);

	//char conew; //1B
	strcat(da, xmlFormat->conEW);

	//char conGpsx[9];   //4B
	strcat(da, xmlFormat->conGpsx);

	//char conns; //1B
	strcat(da, xmlFormat->conNS);

	//char conGpsy[9];   //4B
	strcat(da, xmlFormat->conGpsy);

	//char conGpsz[9];   //4B
	strcat(da, xmlFormat->conGpsz);

	//Adding by Shanchun for alarm report
	//char conAlarmType[5]; //2B
	strcat(da, xmlFormat->conAlarmType);

	//char conAlarmContent[5]; //2B
	strcat(da, xmlFormat->conAlarmContent);


	//char conPmCloudVelaConnCnt[5];   //2B
	strcat(da, xmlFormat->conPmCloudVelaConnCnt);
	//char conPmCloudVelaConnFailCnt[5];   //2B
	strcat(da, xmlFormat->conPmCloudVelaConnFailCnt);
	//char conPmCloudVelaDiscCnt[5];//2B
	strcat(da, xmlFormat->conPmCloudVelaDiscCnt);
	//char conPmSocketDiscCnt[5];//2B
	strcat(da, xmlFormat->conPmSocketDiscCnt);


	//Adding by Shanchun for control cmd
	//char conPowerOnOff[3]; //1B
	strcat(da, xmlFormat->conPowerOnOff);

	//char conInterSample[3]; //1B
	strcat(da, xmlFormat->conInterSample);

	//char conMeausTimes[3]; //1B
	strcat(da, xmlFormat->conMeausTimes);

	//char conNewEquId[3]; //1B
	strcat(da, xmlFormat->conNewEquId);

	//char conWorkCycle[3]; //1B
	strcat(da, xmlFormat->conWorkCycle);

	//char conSwDownload[3]; //1B
	strcat(da, xmlFormat->conSwDownload);


	//strcat(da, xmlFormat->conHwUuid);
	strcat(da, xmlFormat->conHwType);
	strcat(da, xmlFormat->conHwVersion);
	strcat(da, xmlFormat->conSwDelivery);
	strcat(da, xmlFormat->conSwRelease);


	//char conSwInventory[3]; //1B
	strcat(da, xmlFormat->conAvUpload);
	//strcat(da, xmlFormat->conAvFileName);

	//char conTimeStamp[9]; //4B
	strcat(da, xmlFormat->conTimeStamp);

	//char conNtimes[5];   //2B
	strcat(da, xmlFormat->conNtimes);

	IhuDebugPrint("SPSVIRGO: Send out data DA: %s!!!\n", da);

	//获取变长部分的长度, Len=0的情况存在，比如Heart_Beat消息，这里为了统一处理函数的简化，不做过分的区别对待和处理，尽量让处理函数通用化
	int len = 0;
	len = strlen(da);
	//if ((len < 0) || ((len % 2) != 0) || (len > MAX_IHU_MSG_BUF_LENGTH)){
	if ((len < 0) || (len > MAX_IHU_MSG_BUF_LENGTH)){
		IhuErrorPrint("SPSVIRGO: No data to be pack or too long length of data content %d!!!\n", len);
		zIhuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return IHU_FAILURE;
	}

	len = len / 2;  //字节长度，而非字符串长度


	//如果长度=0,则正好，包含一个长度域=0的东东，非常好！省得底层收到的长度=1的HEART_BEAT消息
	//char conLen[3];  //1B
	sprintf(tmp, "%02X", len & 0xFF);
	strcat(s, tmp);

	//变长部分
	strcat(s, da);

	//Finish content part
	strcat(s, xmlFormat->Content_r);

	//固定尾部分
	strcat(s, xmlFormat->FuncFlag_l);
	strcat(s, xmlFormat->FuncFlag);
	strcat(s, xmlFormat->FuncFlag_r);
	strcat(s, xmlFormat->xml_r);

	//存入返回参量中
	strcpy(buf->buf, s);
	buf->bufferLen = strlen(s) + 4;
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

		return SUCCESS;
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

	return SUCCESS;
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

	return SUCCESS;
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

	return SUCCESS;
}
*/








