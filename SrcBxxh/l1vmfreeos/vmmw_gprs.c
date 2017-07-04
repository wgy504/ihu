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
#

//全局变量，引用外部
extern int8_t 	zIhuBspStm32SpsGprsRxBuff[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN];				//串口GPRS数据接收缓冲区 
extern int16_t 	zIhuBspStm32SpsGprsRxCount;																						//当前接收数据的字节数 	  

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
	
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: CALL Session starting...!\n");	
	//参数检查
	if ((strlen(calledNumber) < 8) || (strlen(calledNumber) > 16)){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
		
	//会话前的初始化
	IhuDebugPrint("ihu_vmmw_gprsmod_call_perform: func_gprsmod_module_session_init()(%d)\n", osKernelSysTick());
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software sessioin failure!\n");
		return IHU_FAILURE;				
	}
	
	//拨号
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, "ATD");
	strcat((char*)temp, calledNumber);
	strcat((char*)temp, ";");
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK", 8) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Called number = %s, Call successful!\n", calledNumber);
	}
	else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Called failure, Please re-dial!\n");
		return IHU_FAILURE;						
	}
		
	//发送DTMF
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, "AT+VTS=");
	strcat((char*)temp, "X");
	if (func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK", 3) != IHU_SUCCESS)
	{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: SIM800A Call, DTMF failure!\n");
		return IHU_FAILURE;		
	}
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"DTMF:"))
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Peer press key-down: X\n");
		func_gprsmod_clear_receive_buffer();
	}
				
	//挂断
	if(func_gprsmod_send_AT_command((uint8_t*)"ATH", (uint8_t *)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Hand-off successful\n");
	}else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: SIM800A Call hand-on failure!\n");
		return IHU_FAILURE;				
	}
	func_gprsmod_clear_receive_buffer();
		
	//等待是否有来电：最长10s等待时间
	func_gprsmod_clear_receive_buffer();
	ihu_sleep(10);
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "RING"))
	{
		func_gprsmod_clear_receive_buffer();
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: CALL Coming\n");
	}

	//接听
	if(func_gprsmod_send_AT_command((uint8_t*)"ATA", (uint8_t *)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Hand-on successful\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: SIM800A Call hand-off failure!\n");
		return IHU_FAILURE;		
	}	
	
	//发送DTMF
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, "AT+VTS=");
	strcat((char*)temp, "X");
	if (func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK",3) != IHU_SUCCESS)
	{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: SIM800A Call, DTMF failure!\n");
		return IHU_FAILURE;		
	}
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff,"DTMF:"))
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Peer press key-down: X\n");
		func_gprsmod_clear_receive_buffer();
	}	
	
	//挂断
	if(func_gprsmod_send_AT_command((uint8_t*)"ATH", (uint8_t *)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Hand-off successful\n");
	}else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: SIM800A Call hand-on failure!\n");
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
//接收到的SMS长度为0，则表明没有接收到返回的短信
//PDU格式的短信支持，待有需求时再完善
OPSTAT ihu_vmmw_gprsmod_sms_transmit_with_confirm(char *calledNumber, char *input, int16_t inlen, char *output, uint16_t *outlen)
{
	uint8_t temp[50];
	uint8_t loc=0;
	uint8_t *p,*p1;

	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: SMS Session starting...!\n");
	//参数检查
	if ((strlen(calledNumber) < 8) || (strlen(calledNumber) > 16) || (input == NULL)){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	IhuDebugPrint("ihu_vmmw_gprsmod_sms_transmit_with_confirm: func_gprsmod_module_session_init()(%d)\n", osKernelSysTick());
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software session failure!\n");
		return IHU_FAILURE;
	}
	
	//设置短信发送模式	
	if (func_gprsmod_send_AT_command((uint8_t*)"AT+CMGF=1", (uint8_t *)"OK", 4) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Set SMS Send mode correctly!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Set SMS Send mode wrong!\n");
		return IHU_FAILURE;
	}
	
	//设置短信上报模式，上报位置
  if (func_gprsmod_send_AT_command((uint8_t*)"AT+CNMI=3,1,0,0,0", (uint8_t *)"OK", 4) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Set SMS report mode!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Set SMS report mode wrong!\n");
		return IHU_FAILURE;
	}

  //所有操作在SIM卡中进行
	if (func_gprsmod_send_AT_command((uint8_t*)"AT+CPMS=\"SM\",\"SM\",\"SM\"", (uint8_t *)"OK", 2) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Set SMS SIM card operation!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Set SMS Sim card operation wrong!\n");
		return IHU_FAILURE;
	}
	
	//设置短信中心号
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp,(const char *)"AT+CSCA=\"");
	strcat((char*)temp, IHU_VMMW_GPRSMOD_SMS_CENTER_NUMBER);
	strcat((char*)temp, "\"");
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK", 4) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Set SMS center successful!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Set SMS center failure, CMD = [%s].\n", temp);
		return IHU_FAILURE;		
	}
		
	//检查是否已经收到短信
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "+CMTI:"))
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS:  New SMS! SMS Position: [%s]\n", zIhuBspStm32SpsGprsRxBuff);
		func_gprsmod_clear_receive_buffer();
	}

	//发送短信
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp,(const char *)"AT+CMGS=\"");
	strcat((char*)temp, calledNumber);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, ">", 4);//发送接收方号码
	memset(temp, 0, sizeof(temp));
	strncpy((char*)temp, input, (inlen <= (sizeof(temp)-3))?inlen:(sizeof(temp)-3));
	loc = strlen((const char*)temp);
	temp[loc]= IHU_VMWM_GPRSMOD_STRING_CHAR_EOF;  //0x1A, EOF
	//0x0D, Return，其实这里也许不是很必要多加一个回车，因为func_gprsmod_send_AT_command里面会自动带LR的
	temp[loc+1]= IHU_VMWM_GPRSMOD_STRING_CHAR_CR;	
	temp[loc+2]='\0';
	//发送短信内容
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t *)"OK", 5) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Send SMS successful!\n");
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Send SMS failure!\n");
		return IHU_FAILURE;		
	}

	//查看短信
	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Inquery SMS.\n");
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp,(const char*)"AT+CMGR=1");  //固定读取第一条SMS
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 5)==0)
	{
		p=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\r\n");
		p1=(uint8_t*)strstr((const char*)(p+2),"\r\n");
		//没找到，不报错，这也很正常的
		if ((p == NULL) || (p1 == NULL) || (p>=p1)){
			*outlen = 0;
			return IHU_SUCCESS;
		}
		//由上层来确保不会溢出，总长度总归不会超过接收缓冲区zIhuBspStm32SpsGprsRxBuff的最长长度IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN = IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX
		*outlen = (uint16_t) (p1-p);
		memcpy((char*)output, (char*)p+2, *outlen);
	}else{
		*outlen = 0;
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
* 注意   :  
*******************************************************************************/
//往后台发送的POST功能
//input/inlen是输入参数，output/outlen是输出参数
//output部分的长度，由上层来保证，确保不越界，这里不保证，而只是保证不超越IHU_SYSDIM_MSG_BODY_LEN_MAX而已
//由于GPRSMOD接收缓冲区zIhuBspStm32SpsGprsRxBuff的最长长度=IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX=IHU_SYSDIM_MSG_BODY_LEN_MAX-4，所以从系统角度应该不会出现问题
OPSTAT ihu_vmmw_gprsmod_http_data_transmit_with_receive(char *input, int16_t inlen, char *output, uint16_t *outlen)
{	
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	uint8_t *p1,*p2;
	int i = 0;
	
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: HTTP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	IhuDebugPrint("ihu_vmmw_gprsmod_http_data_transmit_with_receive: func_gprsmod_module_session_init()(%d)\n", osKernelSysTick());
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}
	
	//建立GPRS连接
	IhuDebugPrint("ihu_vmmw_gprsmod_http_data_transmit_with_receive: func_gprsmod_setup_gprs_session_connection()(%d)\n", osKernelSysTick());
	if (func_gprsmod_setup_gprs_session_connection() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Setup GPRS connection session failure!\n");
		return IHU_FAILURE;				
	}
	
	//HTTP设置配置
	//设置GPRS/HTTP工作模式，打开承载, Check whether bearer 1 is open.
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: HTTP setting failure on set bearer parameter - CONTYPE!\n");
		return IHU_FAILURE;
	}	
	//设置GPRS/HTTP工作模式
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"APN\",\"CMNET\"", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: HTTP setting failure on set bearer parameter - APN!\n");
		return IHU_FAILURE;
	}	
	//设置GPRS/HTTP工作模式, Enable bearer 1  //考虑到重复工作的调试和正式情况，这里不再检查是否完全正确	
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=1,1", (uint8_t*)"OK", 2);
	//设置GPRS/HTTP工作模式  //考虑到重复工作的调试和正式情况，这里不再检查是否完全正确
	func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPINIT", (uint8_t*)"OK", 2);
	//HTTP参数配置，设置CID缺省为1
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPPARA=\"CID\",1", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: HTTP setting failure on CID selection!\n");
		return IHU_FAILURE;
	}			
	//HTTP参数配置：设置URL目的地址
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+HTTPPARA=\"URL\",\"", sizeof("AT+HTTPPARA=\"URL\",\""));
	strcat((char*)temp, zIhuSysEngPar.cloud.cloudBhServerAddr);
	strcat((char*)temp, "\"");
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: HTTP/Para setting failure on URL setting!\n");
		return IHU_FAILURE;
	}	
	//HTTP参数配置：设置发送数据的长度
	memset(temp, 0, sizeof(temp));
	sprintf((char*)temp, "AT+HTTPDATA=%d, 6000", inlen);  //延时6秒
	IhuDebugPrint("VMMWGPRS: Sending cloud data buffer input length = %d\n", inlen);
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"DOWNLOAD", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: HTTP POST data failure on download starting!\n");
		return IHU_FAILURE;
	}
	ihu_usleep(250);  //稍微暂停一下
	//真正发送数据:HTTP方式！
	func_gprsmod_send_string(input);	
	func_gprsmod_send_LR();
	ihu_usleep(250);  //稍微暂停一下
	//POST动作指令, 返回字符串中的"200"表示POST成功，1表示POST
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPACTION=1", (uint8_t*)"+HTTPACTION: 1,200", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: HTTP POST data failure on action execution feedback!\n");
		return IHU_FAILURE;
	}		
	//读取服务器后台的反馈数据
	func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPACTION=0", (uint8_t*)"OK", 2);
	//收到200表示反馈成功，0表示GET
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPREAD", (uint8_t*)"+HTTPACTION: 0,200", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: HTTP GET back data failure from cloud!\n");
		return IHU_FAILURE;
	}
	
	//准备解码并送往上层
	//暂时返回的都是垃圾，所以自行生成测试数据
	i = rand() % 11;
	if (i ==1) strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_HEAD_VALID);
	else if(i==2) strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_AUTH_IND);
	else if(i==3) strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_AUTH_RESP_YES);		
	else if(i==4) strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_AUTH_RESP_NO);		
	else if(i==5) strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_REPORT_PERIOD);
	else if(i==6) strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_REPORT_CLOSE);
	else if(i==7) strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_REPORT_FAULT);
	else if(i==8) strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_CONFIRM_PERIOD);
	else if(i==9) strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_CONFIRM_CLOSE);
	else strcpy((char*)zIhuBspStm32SpsGprsRxBuff, HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_CONFIRM_FAULT);
	
	//转化为小写与否。同时，大小写本来就是接收消息的一部分
	if (IHU_VMWM_GPRSMOD_RECEIVE_DATA_TRANSFER_TO_LOWER_CASE_SET == IHU_VMWM_GPRSMOD_RECEIVE_DATA_TRANSFER_TO_LOWER_CASE_YES){
		for (i=0; i<strlen((const char*)zIhuBspStm32SpsGprsRxBuff); i++){
			zIhuBspStm32SpsGprsRxBuff[i] = (char)tolower((char)zIhuBspStm32SpsGprsRxBuff[i]);
		}
	}
	
	//接收数据处理
	p1 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "<xml>");
	p2 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "</xml>");
	if((p1 != NULL) && (p2 != NULL) && (p1 < p2)){
		//直接返回接收到的数据：目前HTTP暂时采用了字符串方式，所以可以使用strcpy，未来可能换成
		//TCP/UDP必然要使用memcpy模式
		//有关outlen的长度，肯定不会越界，本函数的头已经解释过了
		*outlen = p2-p1+9;
		memcpy((uint8_t*)output, (uint8_t*)p1, *outlen); //一直延伸到</xml>的尾巴并多一个字节，外加length本身包含的2B长度		
	}else{
		//HTTP发送结束
		func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPTERM", (uint8_t*)"OK", 2);	//关闭HTTP连接
		func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=0,1", (uint8_t*)"OK", 2);	//关闭HTTP连接
		//关闭连接
		func_gprsmod_close_gprs_session_connection();
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS receive invalid format data from cloud: can not find right <XML> head!\n");
		return IHU_FAILURE;
	}
		
	//HTTP发送结束
	func_gprsmod_send_AT_command((uint8_t*)"AT+HTTPTERM", (uint8_t*)"OK", 2);	//关闭HTTP连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=0,1", (uint8_t*)"OK", 2);	//关闭HTTP连接
	
	//关闭连接
	func_gprsmod_close_gprs_session_connection();
	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_tcp_text_data_transmit_with_receive(char *input, int16_t inlen, char *output, uint16_t *outlen)
{	
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	uint8_t *p1,*p2;
	
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: TCP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	IhuDebugPrint("ihu_vmmw_gprsmod_tcp_text_data_transmit_with_receive: func_gprsmod_module_session_init()(%d)\n", osKernelSysTick());
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}
		
	//建立GPRS连接
	if (func_gprsmod_setup_gprs_session_connection() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Setup GPRS connection session failure!\n");
		return IHU_FAILURE;				
	}
	
	//TCP工作模式设置：默认非透明模式
	//发起TCP连接
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, (const char*)"AT+CIPSTART=");
	strcat((char*)temp, "\"TCP\", \""); 
	strcat((char*)temp, IHU_SYSCFG_BH_TCP_SVR_IP_ADDR);
	strcat((char*)temp, "\", \"");
	strcat((char*)temp, IHU_SYSCFG_BH_TCP_SVR_PORT);	
	strcat((char*)temp, "\"");	
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"CONNECT OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Start TCP connection session failure!\n");
		return IHU_FAILURE;
	}
		
	//启动发送内容：完全可以采用固定长度的发送方式，这里尝试只采用Ctrl+Z的灵活方式
	memset(temp, 0, sizeof(temp));
	sprintf((char*)temp, "AT+CIPSEND=%d", inlen);
	//if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)">", 4) == IHU_FAILURE){
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 4) == IHU_FAILURE){
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Enter TCP send session failure!\n");
		return IHU_FAILURE;
	}
	
	//真正发送数据体: TCP方式！
	func_gprsmod_send_string(input);
	memset(temp, 0, sizeof(temp));
	//CTRL+Z,结束数据发送,启动一次传输	
	temp[0] = 0x1A;
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"SEND OK", 4) == IHU_FAILURE){		
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: TCP data send failure!\n");
		return IHU_FAILURE;
	}

	//TCP等待后台反馈
	if (func_gprsmod_wait_AT_command_fb((uint8_t*)"+IPD", 4) == IHU_FAILURE){
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: TCP data transmission fail to get feedback from cloud!\n");
		return IHU_FAILURE;			
	}
	
	//接收数据：这里假设TCP收到的数据依然是HUIXML的数据CHAR格式
	//转化为小写与否。同时，大小写本来就是接收消息的一部分
	if (IHU_VMWM_GPRSMOD_RECEIVE_DATA_TRANSFER_TO_LOWER_CASE_SET == IHU_VMWM_GPRSMOD_RECEIVE_DATA_TRANSFER_TO_LOWER_CASE_YES){
		int i = 0;
		for (i=0; i<strlen((const char*)zIhuBspStm32SpsGprsRxBuff); i++){
			zIhuBspStm32SpsGprsRxBuff[i] = (char)tolower((char)zIhuBspStm32SpsGprsRxBuff[i]);
		}
	}

	//接收数据处理
	p1 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "<xml>");
	p2 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "</xml>");
	if((p1 != NULL) && (p2 != NULL) && (p1 < p2)){
		//有关outlen的长度，肯定不会越界，本函数的头已经解释过了
		*outlen = p2-p1+9;
		memcpy((uint8_t*)output, (uint8_t*)p1, *outlen); //一直延伸到</xml>的尾巴并多一个字节，外加length本身包含的2B长度		
	}else{
		//关闭连接
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		func_gprsmod_close_gprs_session_connection();
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: TCP char mode transmission get feedback failure: can not find right <XML> head!\n");
		return IHU_FAILURE;
	}
	
	//关闭连接
	ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
	func_gprsmod_close_gprs_session_connection();
	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_tcp_u8_data_transmit_with_receive(int8_t *input, int16_t inlen, int8_t *output, uint16_t *outlen)
{	
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: TCP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	IhuDebugPrint("ihu_vmmw_gprsmod_tcp_u8_data_transmit_with_receive: func_gprsmod_module_session_init()(%d)\n", osKernelSysTick());
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}

	//建立GPRS连接
	if (func_gprsmod_setup_gprs_session_connection() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Setup GPRS connection session failure!\n");
		return IHU_FAILURE;				
	}
	
	//TCP工作模式设置：默认非透明模式
	//发起TCP连接
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, (const char*)"AT+CIPSTART=");
	strcat((char*)temp, "\"TCP\", \""); 
	strcat((char*)temp, IHU_SYSCFG_BH_TCP_SVR_IP_ADDR);
	strcat((char*)temp, "\", \"");
	strcat((char*)temp, IHU_SYSCFG_BH_TCP_SVR_PORT);	
	strcat((char*)temp, "\"");	
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"CONNECT OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Start TCP connection session failure!\n");
		return IHU_FAILURE;
	}
		
	//启动发送内容：完全可以采用固定长度的发送方式，这里尝试只采用Ctrl+Z的灵活方式
	memset(temp, 0, sizeof(temp));
	sprintf((char*)temp, "AT+CIPSEND=%d", inlen);
	//if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)">", 4) == IHU_FAILURE){
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 4) == IHU_FAILURE){
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Enter TCP send session failure!\n");
		return IHU_FAILURE;
	}
	
	//真正发送数据体: TCP方式！
	ihu_l1hd_sps_gprs_send_data((uint8_t*)input, inlen);
	memset(temp, 0, sizeof(temp));
	//CTRL+Z,结束数据发送,启动一次传输	
	temp[0] = 0x1A;
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"SEND OK", 4) == IHU_FAILURE){		
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: TCP data send failure!\n");
		return IHU_FAILURE;
	}

	//TCP等待后台反馈
	if (func_gprsmod_wait_AT_command_fb((uint8_t*)"+IPD", 4) == IHU_FAILURE){
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: TCP data transmission fail to get feedback from cloud!\n");
		return IHU_FAILURE;			
	}
	
	//接收数据处理：将收到的数据全部返回给上层，由上层进行处理
	*outlen = strlen((char*)zIhuBspStm32SpsGprsRxBuff);
	memcpy((uint8_t*)output, (uint8_t*)zIhuBspStm32SpsGprsRxBuff, *outlen);
	
	//关闭连接
	ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
	func_gprsmod_close_gprs_session_connection();
	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_udp_text_data_transmit_with_receive(char *input, int16_t inlen)
{
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: UDP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	IhuDebugPrint("ihu_vmmw_gprsmod_udp_text_data_transmit_with_receive: func_gprsmod_module_session_init()(%d)\n", osKernelSysTick());
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}

	//建立GPRS连接
	if (func_gprsmod_setup_gprs_session_connection() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Setup GPRS connection session failure!\n");
		return IHU_FAILURE;				
	}
	
	//UDP工作模式设置：默认非透明模式
	//发起UDP连接
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, (const char*)"AT+CIPSTART=");
	strcat((char*)temp, "\"UDP\", \""); 
	strcat((char*)temp, IHU_SYSCFG_BH_TCP_SVR_IP_ADDR);
	strcat((char*)temp, "\", \"");
	strcat((char*)temp, IHU_SYSCFG_BH_TCP_SVR_PORT);	
	strcat((char*)temp, "\"");	
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"CONNECT OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Start TCP connection session failure!\n");
		return IHU_FAILURE;
	}
		
	//启动发送内容：完全可以采用固定长度的发送方式，这里尝试只采用Ctrl+Z的灵活方式
	memset(temp, 0, sizeof(temp));
	sprintf((char*)temp, "AT+CIPSEND=%d", inlen);
	//if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)">", 4) == IHU_FAILURE){
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 4) == IHU_FAILURE){
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Enter TCP send session failure!\n");
		return IHU_FAILURE;
	}
	
	//真正发送数据体: UDP方式！
	func_gprsmod_send_string(input);
	memset(temp, 0, sizeof(temp));
	//CTRL+Z,结束数据发送,启动一次传输	
	temp[0] = 0x1A;
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"SEND OK", 4) == IHU_FAILURE){		
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: TCP data send failure!\n");
		return IHU_FAILURE;
	}

	//UDP等待后台反馈：UDP无法获得反馈，只能单独发送。如果想要建立往下的发送通道，貌似只能单独建立UDP服务器，让后台主动连过来
	
	//关闭连接
	ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
	func_gprsmod_close_gprs_session_connection();
	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_udp_u8_data_transmit_with_receive(int8_t *input, int16_t inlen)
{	
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: UDP Session starting...!\n");
	//参数检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	IhuDebugPrint("ihu_vmmw_gprsmod_udp_u8_data_transmit_with_receive: func_gprsmod_module_session_init()(%d)\n", osKernelSysTick());
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}

	//建立GPRS连接
	if (func_gprsmod_setup_gprs_session_connection() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Setup GPRS connection session failure!\n");
		return IHU_FAILURE;				
	}
	
	//UDP工作模式设置：默认非透明模式
	//发起UDP连接
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp, (const char*)"AT+CIPSTART=");
	strcat((char*)temp, "\"UDP\", \""); 
	strcat((char*)temp, IHU_SYSCFG_BH_TCP_SVR_IP_ADDR);
	strcat((char*)temp, "\", \"");
	strcat((char*)temp, IHU_SYSCFG_BH_TCP_SVR_PORT);	
	strcat((char*)temp, "\"");	
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"CONNECT OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Start TCP connection session failure!\n");
		return IHU_FAILURE;
	}
		
	//启动发送内容：完全可以采用固定长度的发送方式，这里尝试只采用Ctrl+Z的灵活方式
	memset(temp, 0, sizeof(temp));
	sprintf((char*)temp, "AT+CIPSEND=%d", inlen);
	//if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)">", 4) == IHU_FAILURE){
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 4) == IHU_FAILURE){
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Enter TCP send session failure!\n");
		return IHU_FAILURE;
	}
	
	//真正发送数据体:UDP方式！
	ihu_l1hd_sps_gprs_send_data((uint8_t*)input, inlen);
	memset(temp, 0, sizeof(temp));
	//CTRL+Z,结束数据发送,启动一次传输	
	temp[0] = 0x1A;
	if(func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"SEND OK", 4) == IHU_FAILURE){		
		ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: TCP data send failure!\n");
		return IHU_FAILURE;
	}

	//UDP等待后台反馈：UDP无法获得反馈，只能单独发送。如果想要建立往下的发送通道，貌似只能单独建立UDP服务器，让后台主动连过来

	//关闭连接
	ihu_l1hd_sps_gprs_send_data((uint8_t *)0x1B, 1);//ESC,取消发送
	func_gprsmod_close_gprs_session_connection();
	return IHU_SUCCESS;
}

//从后台GET数据的功能
OPSTAT ihu_vmmw_gprsmod_ftp_data_transmit_with_receive(int8_t *output, uint16_t *outlen)
{
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	//uint8_t *p1,*p2;
	
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: FTP Session starting...!\n");
	//参数检查
	if (output == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	IhuDebugPrint("ihu_vmmw_gprsmod_ftp_data_transmit_with_receive: func_gprsmod_module_session_init()(%d)\n", osKernelSysTick());
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;				
	}

	//建立GPRS连接
	if (func_gprsmod_setup_gprs_session_connection() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Setup GPRS connection session failure!\n");
		return IHU_FAILURE;				
	}
	
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
	strcat((char*)temp, IHU_SYSCFG_BH_FTP_USR_NAME);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2);
	//设置FTP密码
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+FTPPW=\"", sizeof("AT+FTPPW=\""));
	strcat((char*)temp, IHU_SYSCFG_BH_FTP_PSWD);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2);
	//设置FTP下载的文件名
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+FTPGETNAME=\"", sizeof("AT+FTPGETNAME=\""));
	strcat((char*)temp, IHU_SYSCFG_BH_FTP_FILE_NAME);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 2);
	//设置FTP下载的目录
	memset(temp, 0, sizeof(temp));
	memcpy((char*)temp, "AT+FTPGETPATH=\"", sizeof("AT+FTPGETPATH=\""));
	strcat((char*)temp, IHU_SYSCFG_BH_FTP_FILE_PATH);
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
	
	//关闭连接
	func_gprsmod_close_gprs_session_connection();
	return IHU_SUCCESS;
}

//往后台发送的POST功能
OPSTAT ihu_vmmw_gprsmod_email_data_transmit_with_receive(char *emailTo, char *emailFrom, char *emailTitle, char *emailContent, int16_t inlen)
{
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: EMAIL Session starting...!\n");
	//参数检查
	if (emailTo == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	//会话前的初始化
	IhuDebugPrint("ihu_vmmw_gprsmod_email_data_transmit_with_receive: func_gprsmod_module_session_init()(%d)\n", osKernelSysTick());
	if (func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software before sessioin failure!\n");
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
OPSTAT ihu_vmmw_gprsmod_bs_position_perform(StrVmmwGprsmodBasestationPosition_t *output)
{
	uint8_t *p1, *p2, *p3;
	
	//入参检查
	
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Base Station positioning Session starting...!\n");
	//会话前的初始化
	if(func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;
	}	

	//建立GPRS连接
	IhuDebugPrint("ihu_vmmw_gprsmod_bs_position_perform: func_gprsmod_setup_gprs_session_connection()(%d)\n", osKernelSysTick());
	if (func_gprsmod_setup_gprs_session_connection() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Setup GPRS connection session failure!\n");
		return IHU_FAILURE;				
	}
	
	//基站定位配置
	//设置承载
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"Contype\",\"GPRS\"", (uint8_t*)"OK", 2) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure!\n");
		return IHU_FAILURE;
	}
	//设置APN
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=3,1,\"APN\",\"CMNET\"", (uint8_t*)"OK", 2) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure!\n");
		return IHU_FAILURE;
	}
	//激活
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=1,1", (uint8_t*)"OK", 5) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure!\n");
		return IHU_FAILURE;
	}

	//获取经纬度和时间  SIM900采用地方采用"AT+CIPGSMLOC=1,1","OK",10)
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSIM800ALOC=1,1", (uint8_t*)"OK", 10) == IHU_SUCCESS)
	{
		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),",");
		p3=(uint8_t*)strstr((const char*)p1,"\r\n");
		if((p1 != NULL) && (p3 != NULL) && (p1 < p2))
		{	
			p2=(uint8_t*)strtok((char*)(p1),",");
			output->latitude = strtof((char*)p2, NULL);
			p2=(uint8_t*)strtok(NULL,",");
			output->longitude = strtof((char*)p2, NULL);
			p2=(uint8_t*)strtok(NULL,",");
			strncpy(output->date, (char*)p2, sizeof(p2) < (sizeof(output->date))? sizeof(p2): sizeof(output->date));
			p2=(uint8_t*)strtok(NULL,",");
			*p3='\0';//插入结束符
			strncpy(output->time, (char*)p2, sizeof(p2) < (sizeof(output->time))? sizeof(p2): sizeof(output->time));
		}else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS basestation positioning receive buffer error!\n");
		return IHU_FAILURE;	
		}
	}else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS basestation positioning error!\n");
		return IHU_FAILURE;	
	}
	
	//关闭基站定位
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+SAPBR=0,1", (uint8_t*)"OK", 2) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS positioning failure!\n");
		return IHU_FAILURE;
	}
	
	//关闭连接
	func_gprsmod_close_gprs_session_connection();
	return IHU_SUCCESS;
}

/*******************************************************************************
* 函数名 : ihu_vmmw_gprsmod_tts_perform
* 描述   : TTS文本语音测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : SIM800A测试过，其它模块需要待测试改进，AT CMD可能不太一样
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_tts_perform(char *input, int16_t inlen)
{
  uint16_t len=0;
	uint8_t temp[IHU_VMMW_GPRSMOD_TTS_MAX_LEN + 15];
	uint8_t loc=0;
	
	//入参检查
	if (input == NULL){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: invalid called number or other parameter received!\n");
		return IHU_FAILURE;				
	}
	
	if((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Base Station positioning Session starting...!\n");
	//会话前的初始化
	if(func_gprsmod_module_session_init() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Init GPRSMOD hardware/software before sessioin failure!\n");
		return IHU_FAILURE;
	}	

	//建立GPRS连接
	IhuDebugPrint("ihu_vmmw_gprsmod_tts_perform: func_gprsmod_setup_gprs_session_connection()(%d)\n", osKernelSysTick());
	if (func_gprsmod_setup_gprs_session_connection() == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Setup GPRS connection session failure!\n");
		return IHU_FAILURE;				
	}
	
	//确保TTS被支持
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CTTS=?", (uint8_t*)"OK", 2) == IHU_FAILURE)
	{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS support TTS failure!\n");
		return IHU_FAILURE;
	}
	
	//发出TTS语音内容
	strcpy((char*)&temp,(const char*)"AT+CTTS=2,\"");
	loc=sizeof("AT+CTTS=2,\"");
  len=strlen((const char*)input);
	strncpy((char*)&temp[loc-1],(const char*)input, (len < IHU_VMMW_GPRSMOD_TTS_MAX_LEN)?len:IHU_VMMW_GPRSMOD_TTS_MAX_LEN);
	temp[loc+len-1]='\"';
	temp[loc+len]='\0';
  if (func_gprsmod_send_AT_command((uint8_t*)temp, (uint8_t*)"OK", 3) == IHU_FAILURE)
	{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS support TTS failure!\n");
		return IHU_FAILURE;
	}

	//关闭连接
	func_gprsmod_close_gprs_session_connection();
	return IHU_SUCCESS;
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
/*
另外一种方式获取模块基本信息如下：
//Get Product name
ATI
//Get manufacturer Info
AT+GSV
//Get device phone number
AT+CIMI
//Get the device serial number
AT+GSN
*/
OPSTAT func_gprsmod_module_info_retrieve()
{
	char temp[20];
	int8_t *p1, *p2; 
	
	func_gprsmod_clear_receive_buffer();
	
	//查询制造商名称
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGMI",(uint8_t*)"OK", 4) == IHU_SUCCESS)
	{ 
		p1=(int8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\n");
		strncpy(temp, (char*)zIhuBspStm32SpsGprsRxBuff+2, (p1-zIhuBspStm32SpsGprsRxBuff>=sizeof(temp))?sizeof(temp):(p1-zIhuBspStm32SpsGprsRxBuff));
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMMWGPRS: Maunfacture %s", temp);
		}
		func_gprsmod_clear_receive_buffer();
	}
	else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Can not detect manfacture vendor!\n");
		return IHU_FAILURE;
	}
	
	//查询模块名字
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGMM", (uint8_t*)"OK", 4)== IHU_SUCCESS)
	{ 
		p1=(int8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\n"); 
		strncpy(temp, (char*)zIhuBspStm32SpsGprsRxBuff+2, (p1-zIhuBspStm32SpsGprsRxBuff>=sizeof(temp))?sizeof(temp):(p1-zIhuBspStm32SpsGprsRxBuff));
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMMWGPRS: Module Type %s", temp);
		}
		func_gprsmod_clear_receive_buffer();
	}
	else
	{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Can not detect module name!\n");
		return IHU_FAILURE;
	}
	
	//查询产品序列号
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGSN", (uint8_t*)"OK", 4)== IHU_SUCCESS)
	{ 
		p1=(int8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff+2),"\n"); 
		strncpy(temp, (char*)zIhuBspStm32SpsGprsRxBuff+2, (p1-zIhuBspStm32SpsGprsRxBuff>=sizeof(temp))?sizeof(temp):(p1-zIhuBspStm32SpsGprsRxBuff));
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMMWGPRS: Product Serial ID %s", temp);
		}
		func_gprsmod_clear_receive_buffer();		
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Can not detect product serial number!\n");
		return IHU_FAILURE;
	}
	
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CSQ", (uint8_t*)"OK", 4) == IHU_SUCCESS)		
	{ 
		p1=(int8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff), ":");
		if(p1)
		{
			p2=(int8_t*)strstr((const char*)(p1+1), ",");
			strncpy(temp, (char*)p1+2, (p2-p1-2>=sizeof(temp))?sizeof(temp):(p2-p1-2));
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
				IhuDebugPrint("VMMWGPRS: Singal quality: %s\n", temp);	
			}			
		}
		func_gprsmod_clear_receive_buffer();
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Please check whether valid signal strength!\n");
		return IHU_FAILURE;
	}
	
	//查询本机号码
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CNUM", (uint8_t*)"OK", 4)== IHU_SUCCESS)
	{ 
		int8_t *p2;
		p1=(int8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff), "\""); 
		p2=(int8_t*)strstr((const char*)(p1+1), "\"");
    p1=(int8_t*)strstr((const char*)(p2+1), "\"");
		strncpy(temp, (char*)p1+1, 11);
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMMWGPRS: Local Number %s", temp);		
		}
		func_gprsmod_clear_receive_buffer();		
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Can not detect local phone number!\n");
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
	
	//设置显示详细错误信息
	func_gprsmod_send_AT_command((uint8_t*)"AT+CMEE=2", (uint8_t*)"OK", 4);
	
	//查询SIM卡是否在位
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CPIN?", (uint8_t*)"OK", 4) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMMWGPRS: SIM card inserted!\n");			
		}
		func_gprsmod_clear_receive_buffer(); 
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Please check whether SIM card inserted!\n");
		return IHU_FAILURE;
	}

	//查询运营商名字
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+COPS?", (uint8_t*)"OK", 4)== IHU_SUCCESS)		
	{ 
		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),"\""); 
		if(p1)//有有效数据
		{
			p2=(uint8_t*)strstr((const char*)(p1+1),"\"");
			strncpy(temp, (char*)p1+1, (p2-p1-1>=sizeof(temp))?sizeof(temp):(p2-p1-1));
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
				IhuDebugPrint("VMMWGPRS: Operator: %s\n", temp);				
			}
		}
		func_gprsmod_clear_receive_buffer(); 
	}
	else 
	{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Please check whether valid operator!\n");
		return IHU_FAILURE;
	}
 
	//查询信号质量
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CSQ", (uint8_t*)"OK", 4) == IHU_SUCCESS)		
	{ 
		p1=(uint8_t*)strstr((const char*)(zIhuBspStm32SpsGprsRxBuff),":");
		if(p1)
		{
			p2=(uint8_t*)strstr((const char*)(p1+1),",");
			strncpy(temp, (char*)p1+2, (p2-p1-2>=sizeof(temp))?sizeof(temp):(p2-p1-2));
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
				IhuDebugPrint("VMMWGPRS: Singal quality: %s\n", temp);	
			}			
		}
		func_gprsmod_clear_receive_buffer();
	}
	else{
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Please check whether valid signal strength!\n");
		return IHU_FAILURE;
	}

	//General information inquery
	//下面的查询结果不影响该模块投入应用
	//DTMF支持与否
  if(func_gprsmod_send_AT_command((uint8_t*)"AT+DDET=1", (uint8_t*)"OK", 4) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Module support DTMF audio-decode, support key-down during peer conversation.\n");
	}
	else{
    if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Module not support DTMF audio-decode, Not support key-down during peer conversation.\n");
	}
	//TTS支持与否
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CTTS=?", (uint8_t*)"OK", 4) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Support TTS local voice, Support to convert TEXT into voice.\n");
	}
	else{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Module not support TTS local voice.\n");
	}
	//基站定位支持与否：该AT命令跟SIM800A紧密结合，换做其他硬件模块，命令可能会发生变化 
	//之前使用CIPSIM800ALOC，，目前改为CIPGSMLOC
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPGSMLOC=?", (uint8_t*)"OK", 4) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Module support base station positioning, able to fetch position information.\n");
	}
	else{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: Module not support base station positioning.\n");
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
	
	//先重启启动一下，防止多次操作导致的连续性错误，重复操作没有效果
	//func_gprsmod_send_AT_command((uint8_t*)"AT+CFUN=1,1", (uint8_t*)"OK", 4);
	
	//最大循环次数
	while((repeatCnt > 0) &&(func_gprsmod_send_AT_command((uint8_t*)"AT", (uint8_t*)"OK", 4) != IHU_SUCCESS))//查询是否应到AT指令
	{
		repeatCnt--;		
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMMWGPRS: Not detect GPRS module, trying to reconnecting!\n");
		}
		ihu_usleep(200);
		if (repeatCnt == 0){
			IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Can not detect SIM card or GPRS module!\n");
			return IHU_FAILURE;
		}
	}
	
	//不回显：测试过程中，可以设置为ATE1,回显，方便调测。一旦调测完成，需要移植，简化调测界面
	if (func_gprsmod_send_AT_command((uint8_t*)"ATE0", (uint8_t*)"OK", 4) != IHU_SUCCESS) {
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Can not set display status!\n");
		return IHU_FAILURE;
	}
	
	//基础信息查阅
	if (func_gprsmod_module_info_retrieve() != IHU_SUCCESS){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Can not detect basic info!\n");
		return IHU_FAILURE;
	}
	
	//查阅SIM卡信息
	if(func_gprsmod_gsm_info_retrieve() != IHU_SUCCESS){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Get GPRS module SIM800A related info error!\n");
		return IHU_FAILURE;
	}
	
	//确定是否注册
	if (func_gprsmod_send_AT_command((uint8_t*)"At+CREG?", (uint8_t*)"OK", 4) != IHU_SUCCESS) {
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: Not yet registered into live network!\n");
		return IHU_FAILURE;
	}
	
	//清除缓冲区
	func_gprsmod_clear_receive_buffer();
	
	//正常返回
	return IHU_SUCCESS;
}

//继承老旧程序中，先检查接收的数据。本例中暂时不必要，但这个检查过程具备参考意义，先保留在这儿
void func_grpsmod_check_receive_buffer_ipd(void)
{	
	uint8_t *p1, *p2;
	uint8_t temp[IHU_BSP_STM32_SPS_GPRS_REC_MAX_LEN+1];	
	
	//检查是否有下发的数据：由于前面有初始化结果，一般不可能出现
	if(strstr((const char*)zIhuBspStm32SpsGprsRxBuff, "+IPD"))//判断上位机是否有数据下发
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWGPRS: New content received!\n");
		p1=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,",");
		p2=(uint8_t*)strstr((const char*)zIhuBspStm32SpsGprsRxBuff,":");
		if ((p1!=NULL) && (p2!=NULL) && (p1<p2)){
			memset(temp, 0, sizeof(temp));
			memcpy(temp, p1+1, (p2-p1-1<sizeof(temp)?(p2-p1-1):sizeof(temp)));
			//这里采用%x打印数据块可能比较好，但特别麻烦，反正就是打印下，无所谓
			IhuDebugPrint("VMMWGPRS: received data in char = [%s]\n", temp); 
		}
		func_gprsmod_clear_receive_buffer();
	}
}

//GPRS连接建立过程
OPSTAT func_gprsmod_setup_gprs_session_connection(void)
{
	//为了防止上一次通信的影响，先发送结束。如果不存在，也不影响后续的执行操作
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 1);	//关闭连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSHUT", (uint8_t*)"SHUT OK", 1);		//关闭移动场景
	
	//设置GPRS/IP层配置
	//查阅注册状态
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGREG?", (uint8_t*)"OK", 2) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS inquery register status error!\n");
		return IHU_FAILURE;
	}
	
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGREG=2", (uint8_t*)"OK", 2) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS inquery register status error!\n");
		return IHU_FAILURE;
	}
		
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGREG?", (uint8_t*)"OK", 2) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS inquery register status error!\n");
		return IHU_FAILURE;
	}
	
	//设置GPRS移动台类别为B,支持包交换和数据交换
	//之前采用CLASS B，改为CG模式，就是Class C in GPRS only mode
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGCLASS=\"CG\"", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on class!\n");
		return IHU_FAILURE;
	}
	
	//设置PDP上下文,互联网接协议,接入点等信息, Set up PDP context.
	//据说这个在PPP拨号时才会使用的，不强求一定设置成功
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGDCONT=?", (uint8_t*)"OK", 2);
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGDCONT=1,\"IP\",\"CMNET\"", (uint8_t*)"OK", 2);
	
	//附着GPRS业务, Perform a GPRS Attach
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT=?", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: AT+CGATT=? NOK\n");
		return IHU_FAILURE;
	}		
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT?", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: AT+CGATT? NOK\n");
		return IHU_FAILURE;
	}
	
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CSQ", (uint8_t*)"OK", 4) == IHU_SUCCESS)		
	{ 
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: AT+CSQ NOK, Ignore the error and continue!\n");
		//return IHU_FAILURE;
	}
	
	//先确认是否设置好了：设置APN
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CSTT", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on SET of APN, USER and PSWD!\n");
		return IHU_FAILURE;
	}

	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGATT=1", (uint8_t*)"OK", 10) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: AT+CGATT=1 NOK\n");
		return IHU_FAILURE;
	}
	//设置为GPRS连接模式
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP?", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on CSD and GPRS of connected mode!\n");
		return IHU_FAILURE;
	}
		
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP=1,\"CMNET\"", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on CSD and GPRS of connected mode, Ignore the error and continue!\n");
		//return IHU_FAILURE;
	}
	
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCSGP=1", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on CSD and GPRS of connected mode, Ignore the error and continue!\n");
		//return IHU_FAILURE;
	}
		
	//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPHEAD=1", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on IP head!\n");
		return IHU_FAILURE;
	}
	//设置单路连接
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMUX?", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on IP MUX!\n");
		//return IHU_FAILURE;
	}
		
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIPMUX=0", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on IP MUX, Ignore the error and continue!\n");
		//return IHU_FAILURE;
	}
	//连接承载, Activate the PDP context
	func_gprsmod_send_AT_command((uint8_t*)"AT+CGACT?", (uint8_t*)"OK", 4);
	if (strstr((char*)zIhuBspStm32SpsGprsRxBuff, "1,1") == NULL){
		if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGACT=1, 1", (uint8_t*)"OK", 4) == IHU_FAILURE){
			IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on active PDP context!\n");
			return IHU_FAILURE;
		}
	}
	//连接承载
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGPADDR= 1", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on PDP address!\n");
		return IHU_FAILURE;
	}	
	//先确认是否设置好了：设置APN
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CSTT", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on SET of APN, USER and PSWD, Ignore the error and continue!\n");
		//return IHU_FAILURE;
	}
	//先确认是否设置好了：激活移动场景
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIICR", (uint8_t*)"OK", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure to bring up wireless connection!\n");
		return IHU_FAILURE;
	}
	//先确认是否设置好了：获得本地IP
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CIFSR", (uint8_t*)".", 4) == IHU_FAILURE){
		IHU_ERROR_PRINT_GPRSMOD("VMMWGPRS: GPRS setting failure on getting local IP address!\n");
		return IHU_FAILURE;
	}
	
	return IHU_SUCCESS;
}

//关闭GPRS连接
void func_gprsmod_close_gprs_session_connection(void)
{
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 5);	//快速关闭连接
	func_gprsmod_send_AT_command((uint8_t*)"AT+CIPSHUT", (uint8_t*)"SHUT OK", 5);		//关闭移动场景
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
char atCommandPrintBuf[256];
void CopyReplaceCrLf(char *pOut, char *pIn)
{
		int i = 0;
		if((NULL == pIn)||(NULL == pOut))
		{
				return;
		}
		
		for(i = 0; i < 256; i++)
		{
				if(0 == pIn[i])
				{
						pOut[i] = pIn[i];
						break;
				}
				
				if( (0x0d == pIn[i]) || (0x0a == pIn[i]) )
				{
						pOut[i] = '.';
						//pOut[i] = pIn[i];
				}
				else
				{
						pOut[i] = pIn[i];
				}
		}		
}

OPSTAT func_gprsmod_send_AT_command(uint8_t *cmd, uint8_t *ack, uint16_t wait_time) //in Second
{
	int ret = 0;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
	func_gprsmod_clear_receive_buffer();
	ihu_l1hd_sps_gprs_send_data((uint8_t *)cmd, strlen((char*)cmd));
	func_gprsmod_send_LR();	
	
	memset(atCommandPrintBuf, 0, 256);
	CopyReplaceCrLf(atCommandPrintBuf, (char *)cmd);
	IhuDebugPrint("AT SEND(%d): [%s]\n", osKernelSysTick(), atCommandPrintBuf);
	
	memset(atCommandPrintBuf, 0, 256);
	CopyReplaceCrLf(atCommandPrintBuf, (char *)ack);
	IhuDebugPrint("AT EXPC(%d): [%s]\n", osKernelSysTick(), atCommandPrintBuf);
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
	
	memset(atCommandPrintBuf, 0, 256);
	CopyReplaceCrLf(atCommandPrintBuf, (char *)zIhuBspStm32SpsGprsRxBuff);
	IhuDebugPrint("AT RECV(%d): [%s]\n", osKernelSysTick(), atCommandPrintBuf);
	return ret;
}

//in Second
OPSTAT func_gprsmod_wait_AT_command_fb(uint8_t *ack, uint16_t wait_time)
{
	int ret = 0;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
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
		IhuDebugPrint("VMMWGPRS: Under Registering.....");  //注册中。。。
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



