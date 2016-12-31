/**
 ****************************************************************************************
 *
 * @file mod_gprs.c
 *
 * @brief GPRSMOD module control, (VMBMWC => Virtual Machine Based MiddleWare Components) 
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */
 
#include "vmmw_gprs.h"

//全局变量，引用外部
extern int8_t 	zIhuBspStm32SpsGprsRxBuff[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN];				//串口GPRS数据接收缓冲区 
extern int8_t 	zIhuBspStm32SpsGprsRxState;																						//串口GPRS接收状态
extern int16_t 	zIhuBspStm32SpsGprsRxCount;																						//当前接收数据的字节数 	  
extern int16_t 	zIhuBspStm32SpsGprsRxLen;
extern uint8_t 	zIhuBspStm32SpsPrintRxBuff[IHU_BSP_STM32_SPS_PRINT_REC_MAX_LEN];			//串口1数据缓存区

//待研究，以下全局变量是否必要
uint8_t 	GPRS_UART_TIMER_RECON_Count=0;							//链路重连接的时长计数器，防止超时
uint16_t 	SPS_PRINT_RX_STA;

/*******************************************************************************
*
*   循环测试业务流程函数
*
*******************************************************************************/
//Delay => osDelay(ms)
//Delay => HAL_Delay(ms)
//Delay => ihu_usleep(ms)
//只有ihu_usleep()才在os级别上进行了任务优先级的调度，上面两个均死等待

/*******************************************************************************
* 函数名 : func_gprsmod_sim800a_call_test
* 描述   : 拨号测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : SIM800A测试过，其它模块需要待测试改进，AT CMD可能不太一样
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_call_perform(char *calledNumber)
{	
	uint8_t temp[50];
	
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: CALL Session starting...!\n");	
	//参数检查
	if ((strlen(calledNumber) < 8) || (strlen(calledNumber) > 16)){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
		
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}
	
	//拨号
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, "ATD");
	strcat((char*)temp, calledNumber);
	strcat((char*)temp, ";");
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK", 8) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Called number = %s, Call successful!\n", calledNumber);
	}
	else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Called failure, Please re-dial!\n");
		return IHU_FAILURE;						
	}
		
	//发送DTMF
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, "AT+VTS=");
	strcat((char*)temp, "X");
	if (func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK", 3) != IHU_SUCCESS)
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: SIM800A Call, DTMF failure!\n");
		return IHU_FAILURE;		
	}
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"DTMF:"))
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Peer press key-down: X\n");
		func_gprsmod_clear_receive_buffer();
	}
				
	//挂断
	if(func_gprsmod_send_AT_command((uint8_t*)"ATH", (uint8_t *)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Hand-off successful\n");
	}else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: SIM800A Call hand-on failure!\n");
		return IHU_FAILURE;				
	}
	func_gprsmod_clear_receive_buffer();
		
	//等待是否有来电：最长10s等待时间
	func_gprsmod_clear_receive_buffer();
	ihu_sleep(10);
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "RING"))
	{
		func_gprsmod_clear_receive_buffer();
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: CALL Coming\n");
	}

	//接听
	if(func_gprsmod_send_AT_command((uint8_t*)"ATA", (uint8_t *)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Hand-on successful\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: SIM800A Call hand-off failure!\n");
		return IHU_FAILURE;		
	}	
	
	//发送DTMF
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, "AT+VTS=");
	strcat((char*)temp, "X");
	if (func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK",3) != IHU_SUCCESS)
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: SIM800A Call, DTMF failure!\n");
		return IHU_FAILURE;		
	}
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"DTMF:"))
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Peer press key-down: X\n");
		func_gprsmod_clear_receive_buffer();
	}	
	
	//挂断
	if(func_gprsmod_send_AT_command((uint8_t*)"ATH", (uint8_t *)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Hand-off successful\n");
	}else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: SIM800A Call hand-on failure!\n");
		return IHU_FAILURE;				
	}
	func_gprsmod_clear_receive_buffer();
	
	//返回
	return IHU_SUCCESS;
}


/*******************************************************************************
* 函数名 : ihu_vmmw_gprsmod_sms_transmit_with_confirm
* 描述   : 短信测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : SIM800A测试过，其它模块需要待测试改进，AT CMD可能不太一样
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_sms_transmit_with_confirm(char *calledNumber, char *input)
{
	uint8_t temp[50];
	uint8_t loc=0;
	uint8_t *p,*p1;

	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: SMS Session starting...!\n");
	//参数检查
	if ((strlen(calledNumber) < 8) || (strlen(calledNumber) > 16) || (input == NULL)){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;
	}
	
	//设置短信发送模式	
	if (func_gprsmod_send_AT_command((uint8_t*)"AT+CMGF=1", (uint8_t *)"OK", 2) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Set SMS Send mode correctly!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Set SMS Send mode wrong!\n");
		return IHU_FAILURE;
	}
	
	//设置短信上报模式，上报位置
  if (func_gprsmod_send_AT_command((uint8_t*)"AT+CNMI=3,1,0,0,0", (uint8_t *)"OK", 2) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Set SMS report mode!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Set SMS report mode wrong!\n");
		return IHU_FAILURE;
	}

  //所有操作在SIM卡中进行
	if (func_gprsmod_send_AT_command((uint8_t*)"AT+CPMS=\"SM\",\"SM\",\"SM\"", (uint8_t *)"OK", 2) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Set SMS SIM card operation!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Set SMS Sim card operation wrong!\n");
		return IHU_FAILURE;
	}
	
	//设置短信中心号
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp,(const char *)"AT+CSCA=\"");
	strcat((char*)temp, IHU_VMMW_GPRSMOD_SMS_CENTER_NUMBER);
	strcat((char*)temp, "\"");
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK", 2) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Set SMS center successful!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Set SMS center failure, CMD = [%s].\n", temp);
		return IHU_FAILURE;		
	}
		
	//检查是否已经收到短信
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "+CMTI:"))
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO:  New SMS! SMS Position: [%s]\n", zIhuBspStm32SpsGprsRxBuff);
		func_gprsmod_clear_receive_buffer();
	}

	//发送短信
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp,(const char *)"AT+CMGS=\"");
	strcat((char*)temp, calledNumber);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, ">", 2);//发送接收方号码
	memset(temp, 0, sizeof(temp));
	strncpy((char*)temp, input, (sizeof(input) <= (sizeof(temp)-3))?sizeof(input):(sizeof(temp)-3));
	loc = strlen((const char*)temp);
	temp[loc]= IHU_VMWM_GPRSMOD_STRING_CHAR_EOF;  //0x1A, EOF
	//0x0D, Return，其实这里也许不是很必要多加一个回车，因为func_gprsmod_send_AT_command里面会自动带LR的
	temp[loc+1]= IHU_VMWM_GPRSMOD_STRING_CHAR_CR;	
	temp[loc+2]='\0';
	//发送短信内容
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK", 5) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Send SMS successful!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Send SMS failure!\n");
		return IHU_FAILURE;		
	}

	//查看短信
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Inquery SMS.\n");
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp,(const char*)"AT+CMGR=1");  //固定读取第一条SMS
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 5)==0)
	{
		p=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\r\n");
		p1=(uint8_t*)strstr((const char*)(p+2),"\r\n");
		//没找到，不报错，这也很正常的
		if ((p == NULL) || (p1 == NULL) || (p>=p1)){
			return IHU_SUCCESS;
		}
		loc=p1-p;
		memset(temp, 0, sizeof(temp));
		memcpy((char*)temp, (char*)p+2, loc);
		IhuDebugPrint("VMFO: SMS received content: %s\n", temp);
	}
	func_gprsmod_clear_receive_buffer();
	return IHU_SUCCESS;
}


/*******************************************************************************
* 函数名 : ihu_vmmw_gprsmod_http_data_transmit_with_receive
* 描述   : GPRS数据发送与接收代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   :  为了保持连接，每空闲隔10秒发送一次心跳
*******************************************************************************/
//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_http_data_transmit_with_receive(char *input)
{	
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	uint8_t *p1,*p2;
	
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: HTTP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}
	
	//检查是否有下发的数据：由于前面有初始化结果，一般不可能出现
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "+IPD"))//判断上位机是否有数据下发
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New content received!\n");
		p1=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,",");
		p2=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,":");
		if ((p1!=NULL) && (p2!=NULL) && (p1<p2)){
			memset(temp, 0, sizeof(temp));
			memcpy(temp, p1+1, (p2-p1-1<sizeof(temp)?(p2-p1-1):sizeof(temp)));
			//这里采用%x打印数据块可能比较好，但特别麻烦，反正就是打印下，无所谓
			IhuDebugPrint("VMFO: received data in char = [%s]\n", temp); 
		}
		func_gprsmod_clear_receive_buffer();
	}
		
	//设置GPRS/IP层配置
	//设置GPRS移动台类别为B,支持包交换和数据交换 
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGCLASS=\"B\"", (uint8_t*)"OK", 2);
	//设置PDP上下文,互联网接协议,接入点等信息
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGDCONT=1,\"IP\",\"CMNET\"", (uint8_t*)"OK",2);
	//附着GPRS业务
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT=1", (uint8_t*)"OK", 2);
	//设置为GPRS连接模式
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP=1,\"CMNET\"", (uint8_t*)"OK", 2);
	//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPHEAD=1", (uint8_t*)"OK", 2);
	//设置单路连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMUX=0", (uint8_t*)"OK", 2);
	
	//HTTP设置配置
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", (uint8_t*)"OK", 2); //设置GPRS/HTTP工作模式，打开承载
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"APN\",\"CMNET\"", (uint8_t*)"OK", 2); //设置GPRS/HTTP工作模式
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=1,1", (uint8_t*)"OK", 2); //设置GPRS/HTTP工作模式	
	func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPINIT", (uint8_t*)"OK", 2); //设置GPRS/HTTP工作模式
	func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPPARA=\"CID\",1", (uint8_t*)"OK", 2); //HTTP参数配置
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+HTTPPARA=\"URL\",\"", sizeof("AT+HTTPPARA=\"URL\",\""));
	strcat((char*)temp, zIhuSysEngPar.cloud.cloudBhServerAddr);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2); //HTTP参数配置
	
	//组装并发送内容, 发送POST内容：尝试HTTP方式
	memset(temp, 0, sizeof(temp));
	sprintf((char*)temp, "AT+HTTPDATA=%d, 4000", sizeof(input));  //延时4秒
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2); //设置GPRS/HTTP工作模式，
	func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPACTION=1", (uint8_t*)"OK", 2); //设置HTTP方式动作
	if(func_gprsmod_send_AT_command((uint8_t*)input, (uint8_t*)"+HTTPACTION:", 4) == IHU_SUCCESS)  //下载数据，同样设置延时4秒
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Send successful!\n");
	}else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS data send failure!\n");
		return IHU_FAILURE;
	}
	
	//接收数据
	int i = 0;
	for (i=0; i<strlen((const char*)zIhuBspStm32SpsGprsRxBuff); i++){
		zIhuBspStm32SpsGprsRxBuff[i] = (char)tolower((char)zIhuBspStm32SpsGprsRxBuff[i]);
	}
	p1 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "<xml>");
	p2 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "</xml>");
	if((p1 == NULL) || (p2 == NULL) || (p1 >= p2)){
		msg_struct_spsvirgo_l2frame_rcv_t snd;
		//发送数据到上层SPSVIRGO模块
		memset(&snd, 0, sizeof(msg_struct_spsvirgo_l2frame_rcv_t));
		memcpy(snd.data, (uint8_t*)zIhuBspStm32SpsGprsRxBuff, sizeof(zIhuBspStm32SpsGprsRxBuff));
		snd.length = sizeof(msg_struct_spsvirgo_l2frame_rcv_t);
		ihu_message_send(MSG_ID_SPS_L2FRAME_RCV, TASK_ID_SPSVIRGO, TASK_ID_VMFO, &snd, snd.length);
	}else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS data receive failure 1!\n");
		return IHU_FAILURE;
	}
		
	//HTTP发送结束
	func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPTERM", (uint8_t*)"OK", 2);	//关闭HTTP连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=0,1", (uint8_t*)"OK", 2);	//关闭HTTP连接
		
	//退出
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 2);	//关闭连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSHUT", (uint8_t*)"SHUT OK", 2);		//关闭移动场景

	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_tcp_text_data_transmit_with_receive(char *input)
{	
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	uint8_t *p1,*p2;
	
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: TCP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}

	//检查是否有下发的数据：由于前面有初始化结果，一般不可能出现
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "+IPD"))//判断上位机是否有数据下发
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New content received!\n");
		p1=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,",");
		p2=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,":");
		if ((p1!=NULL) && (p2!=NULL) && (p1<p2)){
			memset(temp, 0, sizeof(temp));
			memcpy(temp, p1+1, (p2-p1-1<sizeof(temp)?(p2-p1-1):sizeof(temp)));
			//这里采用%x打印数据块可能比较好，但特别麻烦，反正就是打印下，无所谓
			IhuDebugPrint("VMFO: received data in char = [%s]\n", temp); 
		}
		func_gprsmod_clear_receive_buffer();
	}
		
	//设置GPRS/IP层配置
	//设置GPRS移动台类别为B,支持包交换和数据交换 
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGCLASS=\"B\"", (uint8_t*)"OK", 2);
	//设置PDP上下文,互联网接协议,接入点等信息
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGDCONT=1,\"IP\",\"CMNET\"", (uint8_t*)"OK",2);
	//附着GPRS业务
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT=1", (uint8_t*)"OK", 2);
	//设置为GPRS连接模式
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP=1,\"CMNET\"", (uint8_t*)"OK", 2);
	//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPHEAD=1", (uint8_t*)"OK", 2);
	//设置单路连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMUX=0", (uint8_t*)"OK", 2);
	
	//TCPIP层面设置
	memset(temp, 0, sizeof(temp));
	strcpy((char*)&temp, (const char*)"AT+CIPSTART=");
	strcat((char*)&temp, "TCP");
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2 ) == IHU_SUCCESS)//发起连接
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New connecting!\n");
	}else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Set GPRS parameter error!\n");
		return IHU_FAILURE;
	}
		
	//发送内容	
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Begin to send..........!\n");
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 2) == IHU_SUCCESS)
	{
		if(func_gprsmod_send_AT_command((uint8_t*)input, (uint8_t*)"SEND OK", 8) == IHU_SUCCESS)
		{ 								
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Send successful!\n");
		}else
		{
			IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure 1!\n");
			return IHU_FAILURE;
		}

		//接收数据：这里假设TCP收到的数据依然是HUIXML的数据CHAR格式
		//如果接收数据是INT8类型的数据，则不能做字符转换，最终这取决于我们应用的需求和方式
		int i = 0;
		for (i=0; i<strlen((const char*)zIhuBspStm32SpsGprsRxBuff); i++){
			zIhuBspStm32SpsGprsRxBuff[i] = (char)tolower((char)zIhuBspStm32SpsGprsRxBuff[i]);
		}
		p1 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "<xml>");
		p2 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "</xml>");

		if((p1 == NULL) || (p2 == NULL) || (p1 >= p2)){
			msg_struct_spsvirgo_l2frame_rcv_t snd;
			//发送数据到上层SPSVIRGO模块
			memset(&snd, 0, sizeof(msg_struct_spsvirgo_l2frame_rcv_t));
			memcpy(snd.data, (uint8_t*)p1, p2-p1);
			snd.length = sizeof(msg_struct_spsvirgo_l2frame_rcv_t);
			ihu_message_send(MSG_ID_SPS_L2FRAME_RCV, TASK_ID_SPSVIRGO, TASK_ID_VMFO, &snd, snd.length);
		}else{
			IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS data receive failure 1!\n");
			return IHU_FAILURE;
		}
	}
	else
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure2!\n");
		return IHU_FAILURE;
	}
	
	//发送结束
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 2)== IHU_SUCCESS)
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x00, 1);
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1A, 1);//CTRL+Z,结束数据发送,启动一次传输								
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Heart-beat successful!\n");		 
	}else
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure3!\n");
		return IHU_FAILURE;
	}
	
	//退出	
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 5);	//关闭连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSHUT", (uint8_t*)"SHUT OK", 5);		//关闭移动场景
		
	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_tcp_u8_data_transmit_with_receive(int8_t *input)
{	
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	uint8_t *p1,*p2;
	
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: TCP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}

	//检查是否有下发的数据：由于前面有初始化结果，一般不可能出现
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "+IPD"))//判断上位机是否有数据下发
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New content received!\n");
		p1=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,",");
		p2=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,":");
		if ((p1!=NULL) && (p2!=NULL) && (p1<p2)){
			memset(temp, 0, sizeof(temp));
			memcpy(temp, p1+1, (p2-p1-1<sizeof(temp)?(p2-p1-1):sizeof(temp)));
			//这里采用%x打印数据块可能比较好，但特别麻烦，反正就是打印下，无所谓
			IhuDebugPrint("VMFO: received data in char = [%s]\n", temp); 
		}
		func_gprsmod_clear_receive_buffer();
	}
		
	//设置GPRS/IP层配置
	//设置GPRS移动台类别为B,支持包交换和数据交换 
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGCLASS=\"B\"", (uint8_t*)"OK", 2);
	//设置PDP上下文,互联网接协议,接入点等信息
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGDCONT=1,\"IP\",\"CMNET\"", (uint8_t*)"OK",2);
	//附着GPRS业务
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT=1", (uint8_t*)"OK", 2);
	//设置为GPRS连接模式
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP=1,\"CMNET\"", (uint8_t*)"OK", 2);
	//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPHEAD=1", (uint8_t*)"OK", 2);
	//设置单路连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMUX=0", (uint8_t*)"OK", 2);
	
	//TCPIP层面设置
	memset(temp, 0, sizeof(temp));
	strcpy((char*)&temp, (const char*)"AT+CIPSTART=");
	strcat((char*)&temp, "TCP");
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2 ) == IHU_SUCCESS)//发起连接
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New connecting!\n");
	}else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Set GPRS parameter error!\n");
		return IHU_FAILURE;
	}
		
	//发送内容	
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Begin to send..........!\n");
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 2) == IHU_SUCCESS)
	{
		if(func_gprsmod_send_AT_command((uint8_t*)input, (uint8_t*)"SEND OK", 8) == IHU_SUCCESS)
		{ 								
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Send successful!\n");
		}else
		{
			IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure 1!\n");
			return IHU_FAILURE;
		}

		//接收数据：因为是U8，所以不需要做转化，而且将接收数据全部送上去，等待L2FRAME进行处理
		msg_struct_spsvirgo_l2frame_rcv_t snd;
		//发送数据到上层SPSVIRGO模块
		memset(&snd, 0, sizeof(msg_struct_spsvirgo_l2frame_rcv_t));
		memcpy(snd.data, (uint8_t*)zIhuBspStm32SpsGprsRxBuff, sizeof(zIhuBspStm32SpsGprsRxBuff));
		snd.length = sizeof(msg_struct_spsvirgo_l2frame_rcv_t);
		ihu_message_send(MSG_ID_SPS_L2FRAME_RCV, TASK_ID_SPSVIRGO, TASK_ID_VMFO, &snd, snd.length);
	}
	else
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure2!\n");
		return IHU_FAILURE;
	}
	
	//发送结束
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 2)== IHU_SUCCESS)
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x00, 1);
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1A, 1);//CTRL+Z,结束数据发送,启动一次传输								
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Heart-beat successful!\n");		 
	}else
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure3!\n");
		return IHU_FAILURE;
	}
	
	//退出	
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 5);	//关闭连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSHUT", (uint8_t*)"SHUT OK", 5);		//关闭移动场景
		
	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_udp_text_data_transmit_with_receive(char *input)
{
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	uint8_t *p1,*p2;
	
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: UDP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}

	//检查是否有下发的数据：由于前面有初始化结果，一般不可能出现
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "+IPD"))//判断上位机是否有数据下发
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New content received!\n");
		p1=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,",");
		p2=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,":");
		if ((p1!=NULL) && (p2!=NULL) && (p1<p2)){
			memset(temp, 0, sizeof(temp));
			memcpy(temp, p1+1, (p2-p1-1<sizeof(temp)?(p2-p1-1):sizeof(temp)));
			//这里采用%x打印数据块可能比较好，但特别麻烦，反正就是打印下，无所谓
			IhuDebugPrint("VMFO: received data in char = [%s]\n", temp); 
		}
		func_gprsmod_clear_receive_buffer();
	}
		
	//设置GPRS/IP层配置
	//设置GPRS移动台类别为B,支持包交换和数据交换 
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGCLASS=\"B\"", (uint8_t*)"OK", 2);
	//设置PDP上下文,互联网接协议,接入点等信息
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGDCONT=1,\"IP\",\"CMNET\"", (uint8_t*)"OK",2);
	//附着GPRS业务
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT=1", (uint8_t*)"OK", 2);
	//设置为GPRS连接模式
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP=1,\"CMNET\"", (uint8_t*)"OK", 2);
	//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPHEAD=1", (uint8_t*)"OK", 2);
	//设置单路连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMUX=0", (uint8_t*)"OK", 2);
	
	//TCPIP层面设置
	memset(temp, 0, sizeof(temp));
	strcpy((char*)&temp, (const char*)"AT+CIPSTART=");
	strcat((char*)&temp, "UDP");
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2 ) == IHU_SUCCESS)//发起连接
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New connecting!\n");
	}else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Set GPRS parameter error!\n");
		return IHU_FAILURE;
	}
		
	//发送内容	
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Begin to send..........!\n");
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 2) == IHU_SUCCESS)
	{
		if(func_gprsmod_send_AT_command((uint8_t*)input, (uint8_t*)"SEND OK", 8) == IHU_SUCCESS)
		{ 								
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Send successful!\n");
		}else
		{
			IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure 1!\n");
			return IHU_FAILURE;
		}

		//接收数据：这里假设TCP收到的数据依然是HUIXML的数据CHAR格式
		int i = 0;
		for (i=0; i<strlen((const char*)zIhuBspStm32SpsGprsRxBuff); i++){
			zIhuBspStm32SpsGprsRxBuff[i] = (char)tolower((char)zIhuBspStm32SpsGprsRxBuff[i]);
		}
		p1 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "<xml>");
		p2 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "</xml>");

		if((p1 == NULL) || (p2 == NULL) || (p1 >= p2)){
			msg_struct_spsvirgo_l2frame_rcv_t snd;
			//发送数据到上层SPSVIRGO模块
			memset(&snd, 0, sizeof(msg_struct_spsvirgo_l2frame_rcv_t));
			memcpy(snd.data, (uint8_t*)p1, p2-p1);
			snd.length = sizeof(msg_struct_spsvirgo_l2frame_rcv_t);
			ihu_message_send(MSG_ID_SPS_L2FRAME_RCV, TASK_ID_SPSVIRGO, TASK_ID_VMFO, &snd, snd.length);
		}else{
			IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS data receive failure 1!\n");
			return IHU_FAILURE;
		}
	}
	else
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure2!\n");
		return IHU_FAILURE;
	}
	
	//发送结束
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 2)== IHU_SUCCESS)
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x00, 1);
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1A, 1);//CTRL+Z,结束数据发送,启动一次传输								
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Heart-beat successful!\n");		 
	}else
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure3!\n");
		return IHU_FAILURE;
	}
	
	//退出	
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 5);	//关闭连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSHUT", (uint8_t*)"SHUT OK", 5);		//关闭移动场景
		
	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_udp_u8_data_transmit_with_receive(int8_t *input)
{	
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	uint8_t *p1,*p2;
	
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: UDP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}

	//检查是否有下发的数据：由于前面有初始化结果，一般不可能出现
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "+IPD"))//判断上位机是否有数据下发
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New content received!\n");
		p1=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,",");
		p2=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,":");
		if ((p1!=NULL) && (p2!=NULL) && (p1<p2)){
			memset(temp, 0, sizeof(temp));
			memcpy(temp, p1+1, (p2-p1-1<sizeof(temp)?(p2-p1-1):sizeof(temp)));
			//这里采用%x打印数据块可能比较好，但特别麻烦，反正就是打印下，无所谓
			IhuDebugPrint("VMFO: received data in char = [%s]\n", temp); 
		}
		func_gprsmod_clear_receive_buffer();
	}
		
	//设置GPRS/IP层配置
	//设置GPRS移动台类别为B,支持包交换和数据交换 
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGCLASS=\"B\"", (uint8_t*)"OK", 2);
	//设置PDP上下文,互联网接协议,接入点等信息
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGDCONT=1,\"IP\",\"CMNET\"", (uint8_t*)"OK",2);
	//附着GPRS业务
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT=1", (uint8_t*)"OK", 2);
	//设置为GPRS连接模式
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP=1,\"CMNET\"", (uint8_t*)"OK", 2);
	//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPHEAD=1", (uint8_t*)"OK", 2);
	//设置单路连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMUX=0", (uint8_t*)"OK", 2);
	
	//TCPIP层面设置
	memset(temp, 0, sizeof(temp));
	strcpy((char*)&temp, (const char*)"AT+CIPSTART=");
	strcat((char*)&temp, "UDP");
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2 ) == IHU_SUCCESS)//发起连接
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New connecting!\n");
	}else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Set GPRS parameter error!\n");
		return IHU_FAILURE;
	}
		
	//发送内容	
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Begin to send..........!\n");
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 2) == IHU_SUCCESS)
	{
		if(func_gprsmod_send_AT_command((uint8_t*)input, (uint8_t*)"SEND OK", 8) == IHU_SUCCESS)
		{ 								
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Send successful!\n");
		}else
		{
			IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure 1!\n");
			return IHU_FAILURE;
		}

		//接收数据：因为是U8，所以不需要做转化，而且将接收数据全部送上去，等待L2FRAME进行处理
		msg_struct_spsvirgo_l2frame_rcv_t snd;
		//发送数据到上层SPSVIRGO模块
		memset(&snd, 0, sizeof(msg_struct_spsvirgo_l2frame_rcv_t));
		memcpy(snd.data, (uint8_t*)zIhuBspStm32SpsGprsRxBuff, sizeof(zIhuBspStm32SpsGprsRxBuff));
		snd.length = sizeof(msg_struct_spsvirgo_l2frame_rcv_t);
		ihu_message_send(MSG_ID_SPS_L2FRAME_RCV, TASK_ID_SPSVIRGO, TASK_ID_VMFO, &snd, snd.length);
	}
	else
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure2!\n");
		return IHU_FAILURE;
	}
	
	//发送结束
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 2)== IHU_SUCCESS)
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x00, 1);
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1A, 1);//CTRL+Z,结束数据发送,启动一次传输								
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Heart-beat successful!\n");		 
	}else
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMFO: GPRS Send failure3!\n");
		return IHU_FAILURE;
	}
	
	//退出	
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 5);	//关闭连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSHUT", (uint8_t*)"SHUT OK", 5);		//关闭移动场景
		
	return IHU_SUCCESS;
}

//从后台GET数据的功能
OPSTAT ihu_vmmw_gprsmod_ftp_data_transmit_with_receive(int8_t *output)
{
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	//uint8_t *p1,*p2;
	
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: FTP Session starting...!\n");
	//参数检查
	if (output == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}

	//设置GPRS/IP层配置
	//设置GPRS移动台类别为B,支持包交换和数据交换 
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGCLASS=\"B\"", (uint8_t*)"OK", 2);
	//设置PDP上下文,互联网接协议,接入点等信息
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGDCONT=1,\"IP\",\"CMNET\"", (uint8_t*)"OK",2);
	//附着GPRS业务
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT=1", (uint8_t*)"OK", 2);
	//设置为GPRS连接模式
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP=1,\"CMNET\"", (uint8_t*)"OK", 2);
	//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPHEAD=1", (uint8_t*)"OK", 2);
	//设置单路连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMUX=0", (uint8_t*)"OK", 2);
	
	//FTP设置配置
	//选择GPRS
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", (uint8_t*)"OK", 2); 
	//设置APN
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"APN\",\"CMNET\"", (uint8_t*)"OK", 2);
	//激活PDP上下文
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=1,1", (uint8_t*)"OK", 2); 
	//设置FTP格式
	func_gprsmod_send_AT_command((uint8_t*)"AT+FTPCID=1", (uint8_t*)"OK", 2);  
	//设置传输类型
	func_gprsmod_send_AT_command((uint8_t*)"AT+FTPTYPE=\"A\"", (uint8_t*)"OK", 2);  
	//设置传输类型  
	func_gprsmod_send_AT_command((uint8_t*)"AT+FTPTYPE=\"A\"", (uint8_t*)"OK", 2);  
	//设置FTP服务器
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+FTPSERV=\"", sizeof("AT+FTPSERV=\""));
	strcat((char*)temp, zIhuSysEngPar.cloud.cloudBhFtpSvrAddr);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2);
	//设置FTP用户名 
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+FTPUN=\"", sizeof("AT+FTPUN=\""));
	strcat((char*)temp, IHU_CLOUDXHUI_FTP_BH_SERVER_USER_NAME);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2);
	//设置FTP密码
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+FTPPW=\"", sizeof("AT+FTPPW=\""));
	strcat((char*)temp, IHU_CLOUDXHUI_FTP_BH_SERVER_USER_PSWD);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2);
	//设置FTP下载的文件名
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+FTPGETNAME=\"", sizeof("AT+FTPGETNAME=\""));
	strcat((char*)temp, IHU_CLOUDXHUI_FTP_BH_SERVER_FILE_NAME);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2);
	//设置FTP下载的目录
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+FTPGETPATH=\"", sizeof("AT+FTPGETPATH=\""));
	strcat((char*)temp, IHU_CLOUDXHUI_FTP_BH_SERVER_FILE_PATH);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2);
	//执行该指令后，请一直处于等待状态，不要做其它操作，以OK免返回error。
	//还需要等待这个+FTPGET:1,1，因为OK只是表示发送命令成了， 等到上报该信息后，在进行获取数据操作
	func_gprsmod_send_AT_command((uint8_t*)"AT+FTPGET=1", (uint8_t*)"+FTPGET:1,1", 10); //10秒
	//下载数据：先返回+FTPGET:2,1024，然后是数据，最后是OK信号
	func_gprsmod_send_AT_command((uint8_t*)"AT+FTPGET=2,1024", (uint8_t*)"OK", 2);
	// 主动查询数据获取结束
	func_gprsmod_send_AT_command((uint8_t*)"AT+FTPGET=2,1024", (uint8_t*)"+FTPGET:2,0", 2);
	//模块自动上报获取数据完成: +FTPGET:1,0，这个如何处理，待定
	
	//FTP发送结束
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=0,1", (uint8_t*)"OK", 2);	//关闭PDP上下文
	
	//退出	
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 5);	//关闭连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSHUT", (uint8_t*)"SHUT OK", 5);		//关闭移动场景
		
	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_email_data_transmit_with_receive(char *emailAddr)
{
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: EMAIL Session starting...!\n");
	//参数检查
	if (emailAddr == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}	
		
	return IHU_SUCCESS;
}

/*******************************************************************************
* 函数名 : ihu_vmmw_gprsmod_bs_position_perform
* 描述   : 基站定位测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : SIM800A测试过，其它模块需要待测试改进，AT CMD可能不太一样
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_bs_position_perform(void)
{
	//uint8_t *p1, *p3;
	//uint8_t *p2;
	
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Base Station positioning Session starting...!\n");
	//会话前的初始化
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}	
		
	IhuDebugPrint("VMFO: EXIT TEST: EXIT + ENTER\n");
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"Contype\",\"GPRS\"", (uint8_t*)"OK", 2);
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"APN\",\"CMNET\"", (uint8_t*)"OK", 2);
  if(func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=1,1", (uint8_t*)"OK", 5))//激活
	{
	
		IhuDebugPrint("VMFO: Open failure, please conform module own this function or not\n");
		return 1;		
	}
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=2,1", (uint8_t*)"OK", 5))
	{
    IhuDebugPrint("VMFO: Fetch local IP error\n");
		return 2;
	}
  GPRS_UART_TIMER_RECON_Count=1;
	while(1)
	{
    if(SPS_PRINT_RX_STA&0x8000)
		{
			SPS_PRINT_RX_STA=0;
			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff, "EXIT"))//退出
			{
				 func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=0,1", (uint8_t*)"OK", 2);//关闭
				 return 0;
			}
			else
			IhuDebugPrint("VMFO: ENTER ERROR\n");
		}
    if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSIM800ALOC=1,1", (uint8_t*)"OK", 10)==0)//获取经纬度和时间
		{
        GPRS_UART_TIMER_RECON_Count=1;
//// 			  p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),",");
////				p3=(uint8_t*)strstr((const char*)p1,"\r\n");
//				if(p1)//有效数据
//				{	
//					p2=(uint8_t*)strtok((char*)(p1),",");
//          IhuDebugPrint("VMFO: Longitude:");
//					//func_gprsmod_print_send_string((char *)p2);
//					//func_gprsmod_print_data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          //IhuDebugPrint("VMFO: Latitude:");func_gprsmod_print_send_string((char *)p2);func_gprsmod_print_data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          //IhuDebugPrint("VMFO: Date:");func_gprsmod_print_send_string((char *)p2);func_gprsmod_print_data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//					*p3='\0';//插入结束符
//          //IhuDebugPrint("VMFO: Time:");func_gprsmod_print_send_string((char *)p2);IhuDebugPrint("\r\n");
//				}
		}
		if(GPRS_UART_TIMER_RECON_Count>11)
		{
      GPRS_UART_TIMER_RECON_Count=0;
			//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
			IhuDebugPrint("VMFO: Open failure, please confirm this module own this function\n");
			return 1;
		}
	}
}

/*******************************************************************************
* 函数名 : ihu_vmmw_gprsmod_tts_perform
* 描述   : TTS文本语音测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : SIM800A测试过，其它模块需要待测试改进，AT CMD可能不太一样
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_tts_perform(void)
{
  uint16_t len=0;
  uint8_t temp_src[]="Hello, this is a test from BXXH!";
	uint8_t temp[IHU_VMMW_GPRSMOD_TTS_MAX_LEN+15];
	uint8_t loc=0;
	
	//会话前的初始化
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: TTS Session starting...!\n");
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}	
		
	IhuDebugPrint("VMFO: EXIT TEST: EXIT + ENTER\n");
	IhuDebugPrint("VMFO: VOICE Brocasting: Please enter content + Enter\n");
  IhuDebugPrint("VMFO: Attention, support max 100 byte\n");
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CTTS=?", (uint8_t*)"OK", 2))
	{
		IhuDebugPrint("VMFO: Error: Please inquery this module own this function or not\n");
		 return 1;
	}
	strcpy((char*)&temp,(const char*)"AT+CTTS=2,\"");
	loc=sizeof("AT+CTTS=2,\"");
  len=strlen((const char*)temp_src);
	strcpy((char*)&temp[loc-1],(const char*)temp_src);
	temp[loc+len-1]='\"';
	temp[loc+len]='\0';
  func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK",3);
	while(1)
	{
		if(SPS_PRINT_RX_STA&0x8000)
		{
      len=SPS_PRINT_RX_STA&0X3FFF;
			if(len>(IHU_VMMW_GPRSMOD_TTS_MAX_LEN/2))len=(IHU_VMMW_GPRSMOD_TTS_MAX_LEN/2);
      SPS_PRINT_RX_STA=0;
			if((len==4)&&(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"EXIT")))//退出	
			return 0;
			func_gprsmod_send_string("AT+CTTS=2,\"");
			zIhuBspStm32SpsPrintRxBuff[len]='\"';
			zIhuBspStm32SpsPrintRxBuff[len+1]='\0';
			if(func_gprsmod_send_AT_command((uint8_t*)zIhuBspStm32SpsPrintRxBuff, (uint8_t*)"OK", 2))
				IhuDebugPrint("VMFO: Wait for previous frame accomplish\n");
		}
	}
}

//获取RSSI数值，完全针对SIM800A模块
//返回的数据格式将采用两位小数NF2格式，扩大100倍后取整
int16_t ihu_vmmw_gprsmod_get_rssi_value(void)
{
	char temp[9];
	char *p1, *p2;
	int16_t rssi = 0;
	float rssiTmp = 0;
	
	func_gprsmod_clear_receive_buffer();	
	if (func_gprsmod_send_AT_command((uint8_t*)"AT+CSQ", (uint8_t *)"OK", 2) == IHU_SUCCESS){
		memset(temp, 0, sizeof(temp));
		p1=strstr((const char*)(zIhuBspStm32SpsGprsRxBuff), "+CSQ:");
		p2=strstr((const char*)(zIhuBspStm32SpsGprsRxBuff), ",");
		if ((p1 == NULL) || (p2 == NULL) || (p2 <= p1)) return -12000;
		strncpy(temp, p1+5, ((p2-p1) <= sizeof(temp))?(p2-p1):sizeof(temp));
		rssi = strtol(temp, NULL, 10);
		if (rssi == 0) return -11500;
		else if (rssi == 1) return -11100;
		else if ((rssi >= 2) && (rssi <= 30)){
			rssiTmp = -110 + (float)((rssi-2)*(110-54)/(30-2));
			rssiTmp = rssiTmp * 100;
			return (int16_t)(rssiTmp);
		}
		else if (rssi == 31) return -5400;
		else return -12000;
	}
	else{
		return -12000;
	}	
}


/*******************************************************************************
*
*
*   本地核心支持函数
*
*
*******************************************************************************/

/*******************************************************************************
* 函数名 : func_gprsmod_module_info_retrieve
* 描述   : SIM800A/GPRS主测试控制部分
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT func_gprsmod_module_info_retrieve()
{
	char temp[20];
	int8_t *p1; 
	
	func_gprsmod_clear_receive_buffer();
	
	//查询制造商名称
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGMI",(uint8_t*)"OK", 2) == IHU_SUCCESS)
	{ 
		p1=(int8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\n");
		strncpy(temp, (char*)zIhuBspStm32SpsGprsRxBuff+2, (p1-zIhuBspStm32SpsGprsRxBuff>=sizeof(temp))?sizeof(temp):(p1-zIhuBspStm32SpsGprsRxBuff));
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: Maunfacture %s", temp);
		}
		func_gprsmod_clear_receive_buffer();
	}
	else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Can not detect manfacture vendor!\n");
		return IHU_FAILURE;
	}
	
	//查询模块名字
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGMM", (uint8_t*)"OK", 2)== IHU_SUCCESS)
	{ 
		p1=(int8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\n"); 
		strncpy(temp, (char*)zIhuBspStm32SpsGprsRxBuff+2, (p1-zIhuBspStm32SpsGprsRxBuff>=sizeof(temp))?sizeof(temp):(p1-zIhuBspStm32SpsGprsRxBuff));
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: Module Type %s", temp);
		}
		func_gprsmod_clear_receive_buffer();
	}
	else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Can not detect module name!\n");
		return IHU_FAILURE;
	}
	
	//查询产品序列号
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGSN", (uint8_t*)"OK", 2)== IHU_SUCCESS)
	{ 
		p1=(int8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\n"); 
		strncpy(temp, (char*)zIhuBspStm32SpsGprsRxBuff+2, (p1-zIhuBspStm32SpsGprsRxBuff>=sizeof(temp))?sizeof(temp):(p1-zIhuBspStm32SpsGprsRxBuff));
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: Product Serial ID %s", temp);
		}
		func_gprsmod_clear_receive_buffer();		
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Can not detect product serial number!\n");
		return IHU_FAILURE;
	}
	
	//查询本机号码
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CNUM", (uint8_t*)"+CNUM", 2)== IHU_SUCCESS)
	{ 
		int8_t *p2;
		p1=(int8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),"\""); 
		p2=(int8_t*)strstr((const char*)(p1+1),"\"");
    p1=(int8_t*)strstr((const char*)(p2+1),"\"");
		strncpy(temp, (char*)p1+1, 11);
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: Local Number %s", temp);		
		}
		func_gprsmod_clear_receive_buffer();		
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Can not detect local phone number!\n");
		return IHU_FAILURE;
	}
	
	//正常返回
	return IHU_SUCCESS;
}

/*******************************************************************************
* 函数名 : func_gprsmod_gsm_info_retrieve
* 描述   : SIM800A/GPRS状态信息检测(信号质量,电池电量,日期时间)
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT func_gprsmod_gsm_info_retrieve(void)
{
	char temp[20];
	uint8_t *p1;
	uint8_t *p2;
	
	func_gprsmod_clear_receive_buffer();
	
	//查询SIM卡是否在位
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CPIN?", (uint8_t*)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: SIM card inserted!\n");			
		}
		func_gprsmod_clear_receive_buffer(); 
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Please check whether SIM card inserted!\n");
		return IHU_FAILURE;
	}

	//查询运营商名字
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+COPS?", (uint8_t*)"OK", 2)== IHU_SUCCESS)		
	{ 
		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),"\""); 
		if(p1)//有有效数据
		{
			p2=(uint8_t*)strstr((const char*)(p1+1),"\"");
			strncpy(temp, (char*)p1+1, (p2-p1-1>=sizeof(temp))?sizeof(temp):(p2-p1-1));
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
				IhuDebugPrint("VMFO: Operator: %s\n", temp);				
			}
		}
		func_gprsmod_clear_receive_buffer(); 
	}
	else 
	{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Please check whether valid operator!\n");
		return IHU_FAILURE;
	}
 
	//查询信号质量
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CSQ", (uint8_t*)"OK", 2) == IHU_SUCCESS)		
	{ 
		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),":");
		if(p1)
		{
			p2=(uint8_t*)strstr((const char*)(p1+1),",");
			strncpy(temp, (char*)p1+2, (p2-p1-2>=sizeof(temp))?sizeof(temp):(p2-p1-2));
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
				IhuDebugPrint("VMFO: Singal quality: %s\n", temp);	
			}			
		}
		func_gprsmod_clear_receive_buffer();
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Please check whether valid signal strength!\n");
		return IHU_FAILURE;
	}

	//General information inquery
	//下面的查询结果不影响该模块投入应用
	//DTMF支持与否
  if(func_gprsmod_send_AT_command((uint8_t*)"AT+DDET=1", (uint8_t*)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module support DTMF audio-decode, support key-down during peer conversation.\n");
	}
	else{
    if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module not support DTMF audio-decode, Not support key-down during peer conversation.\n");
	}
	//TTS支持与否
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CTTS=?", (uint8_t*)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Support TTS local voice, Support to convert TEXT into voice.\n");
	}
	else{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module not support TTS local voice.\n");
	}
	//基站定位支持与否：该AT命令跟SIM800A紧密结合，换做其他硬件模块，命令可能会发生变化 
	//之前使用CIPSIM800ALOC，，目前改为CIPGSMLOC
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPGSMLOC=?", (uint8_t*)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module support base station positioning, able to fetch position information.\n");
	}
	else{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module not support base station positioning.\n");
	}
	func_gprsmod_clear_receive_buffer();
	
	//返回
	return IHU_SUCCESS;
}

/*******************************************************************************
* 函数名 : func_gprsmod_module_session_init
* 描述   : SIM800A主测试程序
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT func_gprsmod_module_session_init(void)
{
	uint8_t repeatCnt = IHU_VMMW_GPRSMOD_REPEAT_CNT;
	
	//最大循环次数
	while((repeatCnt > 0) &&(func_gprsmod_send_AT_command((uint8_t*)"AT", (uint8_t*)"OK", 2) != IHU_SUCCESS))//查询是否应到AT指令
	{
		repeatCnt--;		
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: Not detect GPRS module, trying to reconnecting!\n");
		}
		ihu_usleep(200);
		if (repeatCnt == 0){
			IHU_ERROR_PRINT_GPRSMOD("VMFO: Can not detect SIM card or GPRS module!\n");
			return IHU_FAILURE;
		}
	}
	
	//不回显
	if (func_gprsmod_send_AT_command((uint8_t*)"ATE0", (uint8_t*)"OK", 2) != IHU_SUCCESS) {
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Can not set display status!\n");
		return IHU_FAILURE;
	}
	
	//基础信息查阅
	if (func_gprsmod_module_info_retrieve() != IHU_SUCCESS){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Can not detect basic info!\n");
		return IHU_FAILURE;
	}
	
	//查阅SIM卡信息
	if(func_gprsmod_gsm_info_retrieve() != IHU_SUCCESS){
		IHU_ERROR_PRINT_GPRSMOD("VMFO: Get GPRS module SIM800A related info error!\n");
		return IHU_FAILURE;
	}
	
	//清除缓冲区
	func_gprsmod_clear_receive_buffer();
	
	//正常返回
	return IHU_SUCCESS;
}

/*******************************************************************************
*
*
*
*   与SPS_USART_GPRS相关的核心函数
*
*
*
*
*******************************************************************************/
/*******************************************************************************
* 函数名 : func_gprsmod_clear_receive_buffer
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_gprsmod_clear_receive_buffer(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsGprsRxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsGprsRxCount = 0;               //接收字符串的起始存储位置
}


/*******************************************************************************
* 函数名 : func_gprsmod_send_AT_command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 1:正常  -2:错误
* 注意   : 
* OPSTAT func_gprsmod_send_AT_command(uint8_t *cmd, uint8_t *ack, uint8_t wait_time)  
* 这里的发送，只有成功返回ACK对应的回复时，才算成功
*******************************************************************************/
OPSTAT func_gprsmod_send_AT_command(uint8_t *cmd, uint8_t *ack, uint16_t wait_time) //in Second
{
	int ret = 0;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
	func_gprsmod_clear_receive_buffer();
	ihu_l1hd_sps_gprs_send_data((uint8_t *)cmd, strlen((char*)cmd));
	func_gprsmod_send_LR();	

	ret = IHU_FAILURE;
	while((tickTotal > 0) && (ret == IHU_FAILURE))
	{
		ihu_usleep(IHU_BSP_STM32_SPS_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
		tickTotal--;
		if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, (char*)ack)==NULL)
			 ret = IHU_FAILURE;
		else
			 ret = IHU_SUCCESS;
	}
	return ret;
}

/*******************************************************************************
* 函数名  : USART_GPRS_SendString
* 描述    : USART_GPRS发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void func_gprsmod_send_string(char* s)
{
	while(*s != '\0')//检测字符串结束符
	{
		ihu_l1hd_sps_gprs_send_data((uint8_t *)s++, 1);
	}
}

/*******************************************************************************
* 函数名 : Wait_CREG
* 描述   : 等待模块注册成功
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_gprsmod_wait_CREG(void)
{
	uint8_t i;
	uint8_t k;
	i = 0;
  while(i == 0)        			
	{
		func_gprsmod_clear_receive_buffer();        
		func_gprsmod_send_string("AT+CREG?");   //查询等待模块注册成功
		func_gprsmod_send_LR();
		ihu_usleep(500);  //等待500ms
		for(k=0;k<IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN;k++)      			
		{
			if(zIhuBspStm32SpsGprsRxBuff[k] == ':')
			{
				if((zIhuBspStm32SpsGprsRxBuff[k+4] == '1')||(zIhuBspStm32SpsGprsRxBuff[k+4] == '5'))   //说明注册成功
				{
					i = 1;
					IhuDebugPrint("\r");
					break;
				}
			}
		}
		IhuDebugPrint("VMFO: Under Registering.....");  //注册中。。。
	}
}

/*******************************************************************************
* 函数名 : Set_ATE0
* 描述   : 取消回显
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_grpsmod_set_ATE0(void)
{
	func_gprsmod_send_AT_command((uint8_t*)"ATE0", (uint8_t*)"OK", 2);								//取消回显		
}


/*******************************************************************************
*
*
*
*   公共无关性函数区域
*
*
*
*
*******************************************************************************/

/*******************************************************************************
* 函数名 : func_gprsmod_convert_str_to_int
* 描述   : 字符串转整型
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT func_gprsmod_convert_str_to_int(uint8_t *p,uint8_t len)
{
  uint8_t i=0;
	uint8_t value=0;
	for(i=0;i<len;i++)
	{
    value=value*10;
		value+=(*(p++)-'0');
	}
	return value;
}
/*******************************************************************************
* 函数名 : func_gprsmod_two_char_swap
* 描述   : 交换
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_gprsmod_two_char_swap(char *ch1, char *ch2)
{
	char tmp = *ch1;
	*ch1 = *ch2;
	*ch2 = tmp;
}
/*******************************************************************************
* 函数名 : Convert
* 描述   : 转化成整型
* 输入   : n:要转换的整型数据 ,str:保存的字符
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_gprsmod_convert_int_to_str(int n, char str[])
{
	int i, len;
	for(i = 0; n != 0; ++i)
	{
		str[i] = n % 10 + '0';
		n /= 10;
	}
	str[i] = '\0';
	len = i;
	/* 翻转 */
	for(i = 0; i < len/2; ++i)
		func_gprsmod_two_char_swap(str+i, str+len-i-1);
	str[len] = '\0';
}

/*******************************************************************************
* 函数名 : func_gprsmod_convert_hex_to_str
* 描述   : 十六进制转换成字符串 如0xAC->'A''C';
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_gprsmod_convert_hex_to_str(uint8_t dest[],uint8_t src[],uint8_t len)
{
    uint8_t i=0;
    uint8_t temp_h;
    uint8_t temp_l;
    for(i=0;i<len;i++)
    {
        temp_h=src[i]>>4;
        temp_l=src[i]&0X0F;
        if(temp_h>9)
            dest[2*i]=(src[i]>>4)+55;
        else
            dest[2*i]=(src[i]>>4)+48;
        if(temp_l>9)
            dest[2*i+1]=(src[i]&0X0F)+55;
        else
            dest[2*i+1]=(src[i]&0X0F)+48;
    }
		dest[2*len]='\0';
}

/*******************************************************************************
* 函数名 : Find
* 描述   : 判断缓存中是否含有指定的字符串
* 输入   : 
* 输出   : 
* 返回   : unsigned char:1 找到指定字符，0 未找到指定字符 
* 注意   : 
*******************************************************************************/
OPSTAT func_gprsmod_find_char(char *a)
{ 
  if(strstr((char *)zIhuBspStm32SpsGprsRxBuff, a)!=NULL)
	    return IHU_SUCCESS;
	else
		return IHU_FAILURE;
}

/*******************************************************************************
*
*
*
*   循环测试业务流程函数
*		
*		以下函数，都是等待全部删掉的内容
*
*
*******************************************************************************/

/*******************************************************************************
* 函数名 : Connect_Server
* 描述   : GPRS连接服务器函数
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_gprsmod_sim800a_connect_server(void)
{
	const char *GPRS_UART_string = "AT+CIPSTART=\"TCP\",\"14.125.48.205\",9015";//IP登录服务器,需要根据自己的IP和端口做修改

	func_gprsmod_clear_receive_buffer();
	func_gprsmod_send_string("AT+CIPCLOSE=1");	//关闭连接
  ihu_usleep(100);
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSHUT", (uint8_t*)"SHUT OK", 3);		//关闭移动场景
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGCLASS=\"B\"", (uint8_t*)"OK", 3);//设置GPRS移动台类别为B,支持包交换和数据交换 
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGDCONT=1,\"IP\",\"CMNET\"", (uint8_t*)"OK", 3);//设置PDP上下文,互联网接协议,接入点等信息
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT=1", (uint8_t*)"OK", 3);//附着GPRS业务
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP=1,\"CMNET\"", (uint8_t*)"OK", 3);//设置为GPRS连接模式
  func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMUX=0", (uint8_t*)"OK", 3);//设置为单路连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPHEAD=1", (uint8_t*)"OK", 3);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMODE=1", (uint8_t*)"OK", 3);//打开透传功能
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCCFG=4,5,200,1", (uint8_t*)"OK", 3);//配置透传模式：单包重发次数:2,间隔1S发送一次,每次发送200的字节
  func_gprsmod_send_AT_command((uint8_t *)GPRS_UART_string, (uint8_t*)"OK", 5);//建立连接
  
  ihu_usleep(100);                                //等待串口数据接收完毕
  func_gprsmod_clear_receive_buffer();            //清串口接收缓存为透传模式准备
}

/*******************************************************************************
* 函数名 : Rec_Server_Data
* 描述   : 接收服务器数据函数,并原样返回
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_gprsmod_sim800a_rec_server_data(void)
{
//p2指向发送数据，p1指向接收数据，这里可以使用zIhuBspStm32SpsGprsRxCount来判断
//	if(p2!=p1)   		//说明还有数据未发出
//	{	
//		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET);
//		BSP_STM32_GPRS_UART_SendData(USART_GPRS, *p2);
//		p2++;
//    if(p2>&zIhuBspStm32SpsGprsRxBuff[BSP_STM32_SPS_GPRS_REC_MAXLEN])
//			p2=zIhuBspStm32SpsGprsRxBuff;
//	}
}

//一个完整的数据流程，且不断回送数据
void func_gprsmod_sim800a_data_connection_and_receive_process(void)
{
	IhuDebugPrint("VMFO: GPRS Module GPRS register network!\n");
	func_gprsmod_send_string("+++");//退出透传模式，避免模块还处于透传模式中
  ihu_usleep(500);  //500ms
	func_gprsmod_wait_CREG();   //等待模块注册成功
	IhuDebugPrint("VMFO: GPRS Register Success!\n");
	IhuDebugPrint("VMFO: GPRS Module connect server!\n");
	func_grpsmod_set_ATE0();    //取消回显
	func_gprsmod_sim800a_connect_server(); //设置GPRS参数
	IhuDebugPrint("VMFO: Connect success!\n");

	while(1)
	{
		func_gprsmod_sim800a_rec_server_data();//接收数据并原样返回	
	}

}

///*******************************************************************************
//* 函数名 : main 
//* 描述   : 主函数
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 串口2负责与SIM800A模块通信，串口1用于串口调试，可以避免在下载程序时数据
//					 还发送到模块
//*******************************************************************************/
//OPSTAT func_gprsmod_sim800a_gsm_loop_test_main(void)
//{
//	//Print忽略，因为PRINT_USART3已经在BSP_SER中初始化了
//	//USART_PRINT_Init_Config(115200);
//	//SPS_GRPS忽略，因为调用模块本来就会初始发串口
//	GPRS_UART_Init_Config(115200);
//	TIM_USART_GPRS_Init_Config();
//	IhuDebugPrint("VMFO: SIM800A Module SMS test procedure!\n");//打印信息
//	IhuDebugPrint("VMFO: SIM800A Module Register network!\n");
//  func_gprsmod_sim800a_test_loop(); //SIM800A测试程序
//	return 1;
//}


///*******************************************************************************
//* 函数名 : func_gprsmod_sim800a_test_loop
//* 描述   : SIM800A主测试程序
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//OPSTAT func_gprsmod_sim800a_test_loop(void)
//{
//	uint8_t sim_ready=0;
//	while(func_gprsmod_send_AT_command((uint8_t*)"AT",(uint8_t*)"OK",5))//查询是否应到AT指令
//	{
//		IhuDebugPrint("VMFO: Not dected module!\n");
//		ihu_usleep(800);
//		IhuDebugPrint("VMFO: Trying to reconnecting!\n");
//		ihu_usleep(400);  
//	} 	 
//	func_gprsmod_send_AT_command((uint8_t*)"ATE0",(uint8_t*)"OK",200);//不回显
//	func_gprsmod_sim800a_mtest();
//	if(func_gprsmod_sim800a_gsm_test_info()==0)
//	{
//		sim_ready=1;
//		IhuDebugPrint("VMFO: Please select: Chinese+Enter, then re-send\n"); 				    	 
//		IhuDebugPrint("VMFO: DIAL\t"); 				    	 
//		IhuDebugPrint("VMFO: SMS\t");				    	 
//		IhuDebugPrint("VMFO: GPRS\t");
//		IhuDebugPrint("VMFO: LOC\t");
//		IhuDebugPrint("VMFO: VOICE\r\n");
//	}
//	while(1)
//	{
//		if(sim_ready)
//		{
//			if(SPS_PRINT_RX_STA & 0x8000)
//			{
//				SPS_PRINT_RX_STA=0;
//				if(strstr((char*)zIhuBspStm32SpsPrintRxBuff,"DIAL"))
//					func_gprsmod_sim800a_call_test();	//电话测试
//				else
//				if(strstr((char*)zIhuBspStm32SpsPrintRxBuff,"SMS"))
//					func_gprsmod_sim800a_sms_test();		//短信测试
//				else
//				if(strstr((char*)zIhuBspStm32SpsPrintRxBuff,"GPRS"))
//					func_gprsmod_sim800a_gprs_test();	//GPRS测试
//				else
//				if(strstr((char*)zIhuBspStm32SpsPrintRxBuff,"LOC"))
//				  func_gprsmod_sim800a_jz_test();		//基站测试
//				else				if(strstr((char*)zIhuBspStm32SpsPrintRxBuff,"VOICE"))
//					func_gprsmod_sim800a_tts_test();		//语音测试
//				
//				IhuDebugPrint("VMFO: Please select: Chinese+Enter, then re-send\n"); 				    	 
//				IhuDebugPrint("VMFO: DIAL\t"); 				    	 
//				IhuDebugPrint("VMFO: SMS\t");				    	 
//				IhuDebugPrint("VMFO: GPRS\t");
//				IhuDebugPrint("VMFO: LOC\t");
//				IhuDebugPrint("VMFO: VOICE\r\n");
//			}
//		}			
//	} 	
//}

///*******************************************************************************
//* 函数名 : func_gprsmod_sim800a_mtest
//* 描述   : SIM800A/GPRS主测试控制部分
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//OPSTAT func_gprsmod_sim800a_mtest()
//{
//	uint8_t *p1; 
//	IhuDebugPrint("VMFO: \nSIM800A/GPRS Test Program\n");  
//	func_gprsmod_clear_receive_buffer(); 
//	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGMI",(uint8_t*)"OK", 5)==0)//查询制造商名称
//	{ 
//		IhuDebugPrint("VMFO: Maunfacture:");
//		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\r\n");
//		func_gprsmod_print_send_len((char*)zIhuBspStm32SpsGprsRxBuff+2,p1-zIhuBspStm32SpsGprsRxBuff);
//		func_gprsmod_clear_receive_buffer(); 		
//	} 
//	if(func_gprsmod_send_AT_command("AT+CGMM","OK", 5)==0)//查询模块名字
//	{ 
//		IhuDebugPrint("VMFO: Module Type:");
//		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\r\n"); 
//		func_gprsmod_print_send_len((char*)zIhuBspStm32SpsGprsRxBuff+2,p1-zIhuBspStm32SpsGprsRxBuff);
//		func_gprsmod_clear_receive_buffer();
//	} 
//	if(func_gprsmod_send_AT_command("AT+CGSN","OK", 5)==0)//查询产品序列号
//	{ 
//		IhuDebugPrint("VMFO: Product Serial ID:");
//		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\r\n"); 
//		func_gprsmod_print_send_len((char*)zIhuBspStm32SpsGprsRxBuff+2,p1-zIhuBspStm32SpsGprsRxBuff);
//		func_gprsmod_clear_receive_buffer();		
//	}
//	if(func_gprsmod_send_AT_command("AT+CNUM","+CNUM",2)==0)//查询本机号码
//	{ 
//		uint8_t *p2;
//		IhuDebugPrint("VMFO: Local Number:");
//		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),"\""); 
//		p2=(uint8_t*)strstr((const char*)(p1+1),"\"");
//    p1=(uint8_t*)strstr((const char*)(p2+1),"\"");
//		func_gprsmod_print_send_len((char*)(p1+1),11);
//		func_gprsmod_clear_receive_buffer();		
//	}
//	IhuDebugPrint("\r\n");
//	return 1;
//}

///*******************************************************************************
//* 函数名 : func_gprsmod_sim800a_gsm_test_info
//* 描述   : SIM800A/GPRS状态信息检测(信号质量,电池电量,日期时间)
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//OPSTAT func_gprsmod_sim800a_gsm_test_info(void)
//{
//	uint8_t *p1;
//	uint8_t *p2;
//	uint8_t res=0;
//	func_gprsmod_clear_receive_buffer();
//	if(func_gprsmod_send_AT_command("AT+CPIN?","OK",3))
//	{
//    IhuDebugPrint("VMFO: Please check whether SIM card inserted!\n");
//		res|=1<<0;	//查询SIM卡是否在位 
//	}
//	func_gprsmod_clear_receive_buffer(); 
//	
//	if(func_gprsmod_send_AT_command("AT+COPS?","OK",3)==0)		//查询运营商名字
//	{ 
//		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),"\""); 
//		if(p1)//有有效数据
//		{
//			p2=(uint8_t*)strstr((const char*)(p1+1),"\"");				
//			IhuDebugPrint("VMFO: Opertor:");
//			func_gprsmod_print_send_len((char*)p1+1,p2-p1-1);
//			IhuDebugPrint("\r\n");
//		}
//	}else res|=1<<1;
//	func_gprsmod_clear_receive_buffer();
// 
//	if(func_gprsmod_send_AT_command("AT+CSQ","OK",3)==0)		//查询信号质量
//	{ 
//		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),":");
//		if(p1)
//		{
//			p2=(uint8_t*)strstr((const char*)(p1+1),",");
//			IhuDebugPrint("VMFO: Singal quality:");
//			func_gprsmod_print_send_len((char*)p1+2,p2-p1-2);
//			IhuDebugPrint("\r\n");
//		}		
//	}else res|=1<<2;

//  if(func_gprsmod_send_AT_command("AT+DDET=1","OK",3)==0)
//	{
//		IhuDebugPrint("VMFO: Module support DTMF audio-decode, support key-down during peer conversation");
//    IhuDebugPrint("\r\n");	
//	}else
//	{
//    IhuDebugPrint("VMFO: Module not support DTMF audio-decode, Not support key-down during peer conversation");
//    IhuDebugPrint("\r\n");	
//	}
//	if(func_gprsmod_send_AT_command("AT+CTTS=?","OK",3)==0)
//	{
//		IhuDebugPrint("VMFO: Support TTS local voice, Support to convert TEXT into voice");
//    IhuDebugPrint("\r\n");	
//	}
//	else
//	{
//		IhuDebugPrint("VMFO: Module not support TTS local voice");
//    IhuDebugPrint("\r\n");			
//	}
//	if(func_gprsmod_send_AT_command("AT+CIPSIM800ALOC=?","OK",3)==0)
//	{
//		IhuDebugPrint("VMFO: Module support base station positioning, able to fetch position information");
//    IhuDebugPrint("\r\n");	
//	}
//	else
//	{
//		IhuDebugPrint("VMFO: Module not support base station positioning");
//    IhuDebugPrint("\r\n");			
//	}
//	func_gprsmod_clear_receive_buffer();
//	
//	return res;
//}
///*******************************************************************************
//* 函数名 : func_gprsmod_sim800a_call_test
//* 描述   : 拨号测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//OPSTAT func_gprsmod_sim800a_call_test(void)
//{	
//	uint8_t temp[50];
//	uint16_t len=0;
//	uint16_t i=0;
//	uint8_t mode=0;
//	uint8_t flag=0;
//  uint8_t *p1=NULL;
//	IhuDebugPrint("VMFO: @@@@@@@@@@@DIAL TEST@@@@@@@@@@@\n");
//	IhuDebugPrint("VMFO: DIAL CALL: Input 'CALL xxxx + ENTER'\n");
//	IhuDebugPrint("VMFO: HAND-ON CALL: Input 'HAND-ON + ENTER'\n");
//  IhuDebugPrint("VMFO: HAND-OFF CALL: Input 'HAND-OFF + ENTER'\n");
//	IhuDebugPrint("VMFO: DTMF VOICE: Input 'Single Char + ENTER'\n");
//	IhuDebugPrint("VMFO: EXIT TEST: Input 'EXIT + ENTER'\n");
//	while(1)
//	{
//		if(SPS_PRINT_RX_STA&0x8000)
//		{
//			len=SPS_PRINT_RX_STA&0X3FFF;
//			SPS_PRINT_RX_STA=0;
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"EXIT"))return 0;
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"HAND-ON"))mode=1;
//			else
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"HAND-OFF"))mode=2;
//			else
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"CALL"))mode=3;
//			else
//			if(mode==0)mode=0;//无效
//			else 
//			{
//				mode=4;
//				flag=1;
//			}
//		}
//		switch(mode)
//		{
//			case 0://有来电
//				if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"RING"))
//				{
//					func_gprsmod_clear_receive_buffer();
//					IhuDebugPrint("VMFO: CALL Coming\n");
//				}
//				break;
//			case 1:
//						 if(func_gprsmod_send_AT_command("ATA","OK",2)==0)//接听
//						 {
//							IhuDebugPrint("VMFO: Hand-on successful\n");
//						  mode=4; 
//						 }
//				break;
//			case 2:if(func_gprsmod_send_AT_command("ATH","OK",2)==0)//挂断
//							IhuDebugPrint("VMFO: Hand-off successful\n");		
//						 mode=0;
//				break;
//			case 3://拨号
//						IhuDebugPrint("VMFO:Call:");
//						func_gprsmod_print_send_len((char*)zIhuBspStm32SpsPrintRxBuff+4,len-4);
//						strcpy((char*)temp,"ATD");
//						for(i=3;i<(len-4+3);i++)
//						temp[i]=zIhuBspStm32SpsPrintRxBuff[i+1];
//						temp[i++]=';';
//						temp[i++]='\0';
//						if(func_gprsmod_send_AT_command(temp,"OK",8)==0)
//						{
//							IhuDebugPrint("VMFO: Call successful\n");
//							mode=4;
//						}
//						else
//						{
//							IhuDebugPrint("VMFO: Please re-dial\n");
//						  mode=0;
//						}
//			  break;
//			case 4://发送DTMF
//				if(flag)
//				{
//				flag=0;
//				strcpy((char*)temp,"AT+VTS=");
//				temp[7]=zIhuBspStm32SpsPrintRxBuff[0];
//				temp[8]='\0';
//				func_gprsmod_send_AT_command(temp,"OK",3);
//				}
//				if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"DTMF:"))
//				{
//          ihu_usleep(10);
//					p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),":");
//					IhuDebugPrint("VMFO: Peer press key-down: ");
//					func_gprsmod_print_data(*(p1+1));
//					IhuDebugPrint("\r\n");	
//					func_gprsmod_clear_receive_buffer();
//				}
//				break;
//		}
//		
//	}
//}
///*******************************************************************************
//* 函数名 : func_gprsmod_sim800a_sms_test
//* 描述   : 短信测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//OPSTAT func_gprsmod_sim800a_sms_test(void)
//{
//	uint16_t len=0;
//	uint8_t mode=0;
//	uint8_t temp[50];
//	uint8_t *p,*p1;
//	uint8_t loc=0;
//	IhuDebugPrint("VMFO: @@@@@@@@@@@@@@@@@@@@@@@@@ SMS TEST @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
//	IhuDebugPrint("VMFO: Set SMS Center: Set +8613800756500, set firstly and then make SMS!\n");
//	IhuDebugPrint("VMFO: Send SMS: Please enter telephone number + content with ENTER. Eg: 137xxxx+ This is a test!\n");
//  IhuDebugPrint("VMFO: Check SMS: Inquery xx, xx represent the storage position\n");
//	IhuDebugPrint("VMFO: Exit test: Enter 'EXIT + ENTER'\n");
//	func_gprsmod_send_AT_command("AT+CMGF=1","OK",2);//设置短信发送模式
//  func_gprsmod_send_AT_command("AT+CNMI=3,1,0,0,0","OK",2);//设置短信上报模式，上报位置
//  func_gprsmod_send_AT_command("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);		//所有操作在SIM卡中进行
//	while(1)
//	{
//		if(SPS_PRINT_RX_STA&0x8000)
//		{
//			len=SPS_PRINT_RX_STA&0X3FFF;
//			if(len>BSP_STM32_SPS_GPRS_REC_MAXLEN-2)len=BSP_STM32_SPS_GPRS_REC_MAXLEN-2;
//			SPS_PRINT_RX_STA=0;
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"EXIT"))return 0;
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"SET"))mode=3;//设置短信中心号
//			else
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"INQUERY"))mode=2;//查看短信
//			else 
//      if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"+"))mode=1;//发送短信
//			else
//			  mode=0;

//		}
//		switch(mode)
//		{
//			case 0:
//        if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"+CMTI:"))
//				{
//          IhuDebugPrint("VMFO: New SMS\n");
//					IhuDebugPrint("VMFO: SMS Position: ");
//					p=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,",");
//					func_gprsmod_print_send_string((char*)p+1);
//					func_gprsmod_clear_receive_buffer();
//				}
//				break;
//			case 1:
//				IhuDebugPrint("VMFO: Send SMS\n");
//				zIhuBspStm32SpsPrintRxBuff[len]='\0';//添加结束符;
//				strcpy((char*)temp,(const char *)"AT+CMGS=\"+86");
//				loc=sizeof("AT+CMGS=\"+86");
//				p=(uint8_t*)strstr((char*)zIhuBspStm32SpsPrintRxBuff,(char*)"+");//查找发送内容
//				zIhuBspStm32SpsPrintRxBuff[(p-zIhuBspStm32SpsPrintRxBuff)]='\0';//添加结束符
//				strcpy((char*)(&temp[loc-1]),(const char*)zIhuBspStm32SpsPrintRxBuff);
//				loc=strlen((const char*)temp);
//			  temp[loc]='\"';
//				temp[loc+1]='\0';
//				func_gprsmod_send_AT_command(temp,">",2);//发送接收方号码
//        strcpy((char*)&temp,(const char*)(p+1));
//			  loc=strlen((const char*)temp);
//				temp[loc]='\32';
//				temp[loc+1]='\0';
//			  if(func_gprsmod_send_AT_command(temp,"OK",20)==0)//发送短信内容
//					IhuDebugPrint("VMFO: Send SMS successful\n");
//        else
//					IhuDebugPrint("VMFO: Send SMS failure\n");
//        mode=0;
//			break;
//			case 2:
//				IhuDebugPrint("VMFO: Inquery SMS\n");
//        zIhuBspStm32SpsPrintRxBuff[len]='\0';//添加结束符;
//				p=(uint8_t*)strstr((const char*)zIhuBspStm32SpsPrintRxBuff, ": ");
//				strcpy((char*)temp,(const char*)"AT+CMGR=");
//			  loc=strlen((const char*)temp);
//				strcpy((char*)(&temp[loc]),(const char*)(p+2));
//				loc=strlen((const char*)temp);
//			  if(func_gprsmod_send_AT_command(temp,"OK",5)==0)
//				{
//					p=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\r\n");
//					p1=(uint8_t*)strstr((const char*)(p+2),"\r\n");
//					loc=p1-p;
//					IhuDebugPrint("VMFO: SMS Content: ");
//					func_gprsmod_print_send_len((char*)p+2,loc);
//				}
//        func_gprsmod_clear_receive_buffer();
//				mode=0;
//				break;
//			case 3:
//				IhuDebugPrint("VMFO: Set SMS Center Number\n");
//				zIhuBspStm32SpsPrintRxBuff[len]='\0';//添加结束符;
//				strcpy((char*)temp,(const char *)"AT+CSCA=");
//			  loc=sizeof("AT+CSCA=");
//				strcpy((char*)(&temp[loc-1]),(const char*)&zIhuBspStm32SpsPrintRxBuff[4]);
//				if(func_gprsmod_send_AT_command(temp,"OK",3)==0)
//					IhuDebugPrint("VMFO: Set SMS center successful\n");
//				else
//					IhuDebugPrint("VMFO: Set SMS center failure\n");
//				mode=0;
//				break;
//		}
//	}
//}

///*******************************************************************************
//* 函数名 : func_gprsmod_sim800a_gprs_test
//* 描述   : GPRS测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   :  为了保持连接，每空闲隔10秒发送一次心跳
//*******************************************************************************/
//OPSTAT func_gprsmod_sim800a_gprs_test(void)
//{	
//  uint16_t len=0;
//	uint8_t mode=0;
//	uint8_t temp[200];
//	uint8_t flag=0;
//	uint8_t *p1,*p2;
//	IhuDebugPrint("VMFO: @@@@@@@@@@@@@@GPRS TEST@@@@@@@@@@@@@@@@@@@@@\n");
//	IhuDebugPrint("VMFO: Set Parameter: Set\"MODE\",\"IP\", Port+ Enter\n Eg: Set\"TCP\",\"125.89.18.79\",12345\n");
//	IhuDebugPrint("VMFO: Send content: Send + Content + Enter \n, Eg: This is a test!\n");
//	IhuDebugPrint("VMFO: Exit Test: Exit + Enter\n");
//	
//	while(1)
//	{
//		if(SPS_PRINT_RX_STA&0x8000)
//		{
//			len=SPS_PRINT_RX_STA&0X3FFF;
//			if(len>BSP_STM32_SPS_GPRS_REC_MAXLEN-2)len=BSP_STM32_SPS_GPRS_REC_MAXLEN-2;
//			SPS_PRINT_RX_STA=0;
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"EXIT"))mode=4;//退出
//			else
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"SET"))mode=1;//设置配置
//			else
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"SEND"))mode=2;//发送内容
//			else 												mode=0;			
//		}
//		else
//		{
//			if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"CONNECT OK")&&(flag==1))//连接成功
//			{
//					IhuDebugPrint("VMFO: Connect Successful\n");
//					GPRS_UART_TIMER_RECON_Count=1;
//					flag=2;
//					mode=3;
//					func_gprsmod_clear_receive_buffer();
//			}else
//			if((flag==1)&&((strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"CLOSED"))&&(GPRS_UART_TIMER_RECON_Count>10)))//连接失败或超时
//			{
//					IhuDebugPrint("VMFO: Connect failure\n");
//					
//				  while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
//					USART_SendData(USART_PRINT ,GPRS_UART_TIMER_RECON_Count);//发送当前字符
//				
//					GPRS_UART_TIMER_RECON_Count=0;
//					flag=0;
//					mode=1; //重新连接
//					func_gprsmod_clear_receive_buffer();
//			}	
//			if((flag==2)&&(GPRS_UART_TIMER_RECON_Count>10))//每10秒一个心跳包
//			{
//				GPRS_UART_TIMER_RECON_Count=1;
//				mode=3;
//			}
//	  }
//    switch(mode)
//		{
//			case 0:        
//						if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"+IPD"))//判断上位机是否有数据下发
//						{
//							IhuDebugPrint("VMFO: New content\n");
//							p1=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,",");
//							p2=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,":");
//							func_gprsmod_print_send_len((char*)(p2+1),func_gprsmod_convert_str_to_int((p1+1),(p2-p1-1)));
//							IhuDebugPrint("\r\n");
//							func_gprsmod_clear_receive_buffer();
//						}
//			break;
//			case 1: 
//				     if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"TCP")||strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"UDP"))
//						 {
//                strcpy((char*)&temp,(const char*)"AT+CIPSTART=");
//							  memcpy((char*)&temp[12],(const char*)(&zIhuBspStm32SpsPrintRxBuff[4]),len-4);
//							  temp[len+12]='\0';
//							  func_gprsmod_send_AT_command("AT+CIPCLOSE=1","CLOSE OK",2);	//关闭连接
//								func_gprsmod_send_AT_command("AT+CIPSHUT","SHUT OK",2);		//关闭移动场景
// 							  func_gprsmod_send_AT_command("AT+CGCLASS=\"B\"","OK",2);//设置GPRS移动台类别为B,支持包交换和数据交换 
//								func_gprsmod_send_AT_command("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",2);//设置PDP上下文,互联网接协议,接入点等信息
//								func_gprsmod_send_AT_command("AT+CGATT=1","OK",2);//附着GPRS业务
//								func_gprsmod_send_AT_command("AT+CIPCSGP=1,\"CMNET\"","OK",2);//设置为GPRS连接模式
//								func_gprsmod_send_AT_command("AT+CIPHEAD=1","OK",2);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
//                func_gprsmod_send_AT_command("AT+CIPMUX=0","OK",2);//设置单路连接
//                if(func_gprsmod_send_AT_command(temp,"OK",2 )==0)//发起连接
//								{
//                  IhuDebugPrint("VMFO: New connecting\n");
//									GPRS_UART_TIMER_RECON_Count=1;
//									flag=1;
//								}
//								else
//								{
//									IhuDebugPrint("VMFO: Enter error: Please re-enter\n");
//									GPRS_UART_TIMER_RECON_Count=0;
//									flag=0;
//								}
//						 }
//						 else
//						 {
//							  IhuDebugPrint("VMFO: Enter error: Please re-enter\n");
//						 }
//             mode=0;
//			break;
//			case 2:
//              if(flag==2)
//							{
//                IhuDebugPrint("VMFO: Begin to send..........\n");
//								if(func_gprsmod_send_AT_command("AT+CIPSEND",">",2)==0)
//								{
//										 zIhuBspStm32SpsPrintRxBuff[len]='\32';
//										 zIhuBspStm32SpsPrintRxBuff[len+1]='\0';
//										 if(func_gprsmod_send_AT_command(&zIhuBspStm32SpsPrintRxBuff[4],"SEND OK",8)==0)
//										 { 								
//													IhuDebugPrint("VMFO: Send successful\n");
//													GPRS_UART_TIMER_RECON_Count=1;
//										 }
//										 else
//											 IhuDebugPrint("VMFO: Send failure\n");
//										 
//								}else
//								{
//										 func_gprsmod_data_byte_send(0X1B);//ESC,取消发送
//                     IhuDebugPrint("VMFO: Send failure\n");
//								}
//						  }else IhuDebugPrint("VMFO: Not connected\n");
//							mode=0;
//			break;
//			case 3:
//						if(func_gprsmod_send_AT_command("AT+CIPSEND",">",2)==0)
//						{
//							func_gprsmod_data_byte_send(0x00);
//							func_gprsmod_data_byte_send(0X1A);//CTRL+Z,结束数据发送,启动一次传输								
//							IhuDebugPrint("VMFO: Heart-beat successful\n");

//								 
//						}else
//						{
//								 func_gprsmod_data_byte_send(0X1B);//ESC,取消发送
//							IhuDebugPrint("VMFO: Heart-beat failure\n");
//						}
//				mode=0;
//			break;
//			case 4:
//						func_gprsmod_send_AT_command("AT+CIPCLOSE=1","CLOSE OK",5);	//关闭连接
//						func_gprsmod_send_AT_command("AT+CIPSHUT","SHUT OK",5);		//关闭移动场景
//						return 1;
//		}      
//	}
//}

///*******************************************************************************
//* 函数名 : func_gprsmod_sim800a_jz_test
//* 描述   : 基站定位测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//OPSTAT func_gprsmod_sim800a_jz_test(void)
//{
//  uint8_t *p1,*p2,*p3;
//	IhuDebugPrint("VMFO: @@@@@@@@@@@@@@@@@@@@@Base Station POSITON TEST@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
//	IhuDebugPrint("VMFO: EXIT TEST: EXIT + ENTER\n");
//	func_gprsmod_send_AT_command("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",2);
//	func_gprsmod_send_AT_command("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",2);
//  if(func_gprsmod_send_AT_command("AT+SAPBR=1,1","OK",5))//激活
//	{
//	
//		IhuDebugPrint("VMFO: Open failure, please conform module own this function or not\n");
//		return 1;		
//	}
//	if(func_gprsmod_send_AT_command("AT+SAPBR=2,1","OK",5))
//	{
//    IhuDebugPrint("VMFO: Fetch local IP error\n");
//		return 2;
//	}
//  GPRS_UART_TIMER_RECON_Count=1;
//	while(1)
//	{
//    if(SPS_PRINT_RX_STA&0x8000)
//		{
//			SPS_PRINT_RX_STA=0;
//			if(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"EXIT"))//退出
//			{
//				 func_gprsmod_send_AT_command("AT+SAPBR=0,1","OK",2);//关闭
//				 return 0;
//			}
//			else
//			IhuDebugPrint("VMFO: ENTER ERROR\n");
//		}
//    if(func_gprsmod_send_AT_command("AT+CIPSIM800ALOC=1,1","OK",10)==0)//获取经纬度和时间
//		{
//        GPRS_UART_TIMER_RECON_Count=1;
// 			  p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),",");
//				p3=(uint8_t*)strstr((const char*)p1,"\r\n");
//				if(p1)//有效数据
//				{	
//					p2=(uint8_t*)strtok((char*)(p1),",");
//          IhuDebugPrint("VMFO: Longitude:");func_gprsmod_print_send_string((char *)p2);func_gprsmod_print_data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          IhuDebugPrint("VMFO: Latitude:");func_gprsmod_print_send_string((char *)p2);func_gprsmod_print_data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          IhuDebugPrint("VMFO: Date:");func_gprsmod_print_send_string((char *)p2);func_gprsmod_print_data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//					*p3='\0';//插入结束符
//          IhuDebugPrint("VMFO: Time:");func_gprsmod_print_send_string((char *)p2);IhuDebugPrint("\r\n");
//				}
//		}
//		if(GPRS_UART_TIMER_RECON_Count>11)
//		{
//      GPRS_UART_TIMER_RECON_Count=0;
//			//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
//			IhuDebugPrint("VMFO: Open failure, please confirm this module own this function\n");
//			return 1;
//		}
//	}
//}

///*******************************************************************************
//* 函数名 : SIM800A_JZ_test
//* 描述   : 基站定位测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
///*
//OPSTAT func_gprsmod_sim800a_jz_test(void)
//{
//  uint8_t *p1,*p2,*p3;
//	IhuDebugPrint("VMFO: @@@@@@@@@@@@@@@@@@@@@Base Station POSITON TEST@@@@@@@@@@@@@@@@@@@@@@@@@\n");
//	IhuDebugPrint("VMFO: EXIT TEST：EXIT + ENTER\n");
//	func_gprsmod_send_AT_command("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",2);
//	func_gprsmod_send_AT_command("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",2);
//  if(func_gprsmod_send_AT_command("AT+SAPBR=1,1","OK",5))//激活
//	{
//		//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
//		IhuDebugPrint("VMFO: Open failure, please conform module own this function or not\n");
//		return 1;		
//	}
//	if(func_gprsmod_send_AT_command("AT+SAPBR=2,1","OK",5))
//	{
//    IhuDebugPrint("VMFO: Fetch local IP error\n");
//		return 2;
//	}
//  GPRS_UART_TIMER_RECON_Count=1;
//	while(1)
//	{
//    if(SPS_PRINT_RX_STA&0x8000)
//		{
//			SPS_PRINT_RX_STA=0;
//			if(strstr(zIhuBspStm32SpsPrintRxBuff,"EXIT"))//退出
//			{
//				 func_gprsmod_send_AT_command("AT+SAPBR=0,1","OK",2);//关闭
//				 return 0;
//			}
//			else
//			IhuDebugPrint("VMFO: Enter Error\n");
//		}
//    if(func_gprsmod_send_AT_command("AT+CIPSIM800ALOC=1,1","OK",10)==0)//获取经纬度和时间
//		{
//        GPRS_UART_TIMER_RECON_Count=1;
// 			  p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),",");
//				p3=(uint8_t*)strstr(p1,"\r\n");
//				if(p1)//有效数据
//				{	
//					p2=(uint8_t*)strtok((char*)(p1),",");
//          IhuDebugPrint("VMFO: Logiture:");func_gprsmod_print_send_string(p2);UART1_SendData('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          IhuDebugPrint("VMFO: Latitude:");func_gprsmod_print_send_string(p2);UART1_SendData('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          IhuDebugPrint("VMFO: Date:");func_gprsmod_print_send_string(p2);UART1_SendData('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//					*p3='\0';//插入结束符
//          IhuDebugPrint("VMFO: Time:");func_gprsmod_print_send_string(p2);IhuDebugPrint("\r\n");
//				}
//		}
//		if(GPRS_UART_TIMER_RECON_Count>11)
//		{
//      GPRS_UART_TIMER_RECON_Count=0;
//			//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
//			IhuDebugPrint("VMFO: Open failure, please confirm this module own this function\n");
//			return 1;
//		}
//	}
//}
//*/

///*******************************************************************************
//* 函数名 : func_gprsmod_sim800a_tts_test
//* 描述   : TTS文本语音测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//OPSTAT func_gprsmod_sim800a_tts_test(void)
//{
//  uint16_t len=0;
//  uint8_t temp_src[]="Hello, this is a test from BXXH!";
//	uint8_t temp[IHU_VMMW_GPRSMOD_TTS_MAX_LEN+15];
//	uint8_t loc=0;
//	IhuDebugPrint("VMFO: @@@@@@@@@@@@@@@@@@@@@TTS TEXT VOICE TEST@@@@@@@@@@@@@@@@@@@@@@@\n");
//	IhuDebugPrint("VMFO: EXIT TEST: EXIT + ENTER\n");
//	IhuDebugPrint("VMFO: VOICE Brocasting: Please enter content + Enter\n");
//  IhuDebugPrint("VMFO: Attention, support max 100 byte\n");
//	if(func_gprsmod_send_AT_command("AT+CTTS=?","OK",2))
//	{
//		IhuDebugPrint("VMFO: Error: Please inquery this module own this function or not\n");
//		 return 1;
//	}
//	strcpy((char*)&temp,(const char*)"AT+CTTS=2,\"");
//	loc=sizeof("AT+CTTS=2,\"");
//  len=strlen((const char*)temp_src);
//	strcpy((char*)&temp[loc-1],(const char*)temp_src);
//	temp[loc+len-1]='\"';
//	temp[loc+len]='\0';
//  func_gprsmod_send_AT_command(temp,"OK",3);
//	while(1)
//	{
//		if(SPS_PRINT_RX_STA&0x8000)
//		{
//      len=SPS_PRINT_RX_STA&0X3FFF;
//			if(len>(IHU_VMMW_GPRSMOD_TTS_MAX_LEN/2))len=(IHU_VMMW_GPRSMOD_TTS_MAX_LEN/2);
//      SPS_PRINT_RX_STA=0;
//			if((len==4)&&(strstr((const char*)zIhuBspStm32SpsPrintRxBuff,"EXIT")))//退出	
//			return 0;
//			func_gprsmod_send_string("AT+CTTS=2,\"");
//			zIhuBspStm32SpsPrintRxBuff[len]='\"';
//			zIhuBspStm32SpsPrintRxBuff[len+1]='\0';
//			if(func_gprsmod_send_AT_command(zIhuBspStm32SpsPrintRxBuff,"OK",2))
//				IhuDebugPrint("VMFO: Wait for previous frame accomplish\n");
//		}
//	}
//}



