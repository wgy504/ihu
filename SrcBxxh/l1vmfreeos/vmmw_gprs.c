/**
 ****************************************************************************************
 *
 * @file mod_gprs.c
 *
 * @brief GPRS module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */
 
#include "vmmw_gprs.h"

//全局变量，引用外部
extern int8_t BSP_STM32_SPS_GPRS_R_Buff[BSP_STM32_SPS_GPRS_REC_MAXLEN];			//串口GPRS数据接收缓冲区 
extern int8_t BSP_STM32_SPS_GPRS_R_State;												//串口GPRS接收状态
extern int16_t BSP_STM32_SPS_GPRS_R_Count;					//当前接收数据的字节数 	  
extern uint8_t 	BSP_STM32_SPS_PRINT_R_Buff[BSP_STM32_SPS_PRINT_REC_MAXLEN];		//串口1数据缓存区

//跟时钟/USART相关的本地变量
uint8_t 	GPRS_UART_TIMER_WAIT_Duration=0;        		//传递等待时长
uint16_t 	GPRS_UART_TIMER_DELAY_Count=0;      				//延时变量
uint8_t 	GPRS_UART_TIMER_START_Flag=0;							//定时器0延时启动计数器
uint8_t 	GPRS_UART_TIMER_RECON_Count=0;							//链路重连接的时长计数器，防止超时
uint8_t 	Time_UART_PRINT=0;  											//串口1计时器
uint16_t 	SPS_PRINT_RX_STA;

const char *GPRS_UART_string = "AT+CIPSTART=\"TCP\",\"14.125.48.205\",9015";//IP登录服务器,需要根据自己的IP和端口做修改

/*******************************************************************************
*
*
*
*   循环测试业务流程函数
*
*
*
*
*******************************************************************************/
//Delay => osDelay(ms)
//Delay => HAL_Delay(ms)
//Delay => ihu_usleep(ms)

/*******************************************************************************
* 函数名 : ihu_vmmw_gprsmod_sim800a_test_selection
* 描述   : SIM800A主测试程序
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_gsm_working_procedure_selection(uint8_t option, uint8_t sub_opt)
{
	uint8_t repeatCnt = IHU_VMMW_GPRSMOD_REPEAT_CNT;
	
	//最大循环次数
	while((repeatCnt > 0) &&(func_gprsmod_send_AT_command((uint8_t*)"AT", (uint8_t*)"OK", 2) != IHU_SUCCESS))//查询是否应到AT指令
	{
		repeatCnt--;
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Not detect GPRS module, trying to reconnecting!\n");
		}
		ihu_usleep(200);
		if (repeatCnt == 0){
			zIhuRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: Can not detect SIM card or GPRS module!\n");
			return IHU_FAILURE;
		}
	}
	
	//不回显
	if (func_gprsmod_send_AT_command((uint8_t*)"ATE0", (uint8_t*)"OK", 2) != IHU_SUCCESS) {
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Can not set display status!\n");
		return IHU_FAILURE;
	}
	
	//基础信息查阅
	if (ihu_vmmw_gprsmod_module_procedure() != IHU_SUCCESS){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Can not detect basic info!\n");
		return IHU_FAILURE;
	}
	
	//查阅SIM卡信息
	if(ihu_vmmw_gprsmod_gsm_info_procedure() != IHU_SUCCESS){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Get GPRS module SIM800A related info error!\n");
		return IHU_FAILURE;
	}
	
	if (option == 1) return ihu_vmmw_gprsmod_call_procedure(sub_opt);	//电话测试
	else if (option == 2) return ihu_vmmw_gprsmod_sms_procedure();		//短信测试
	else if (option == 3) return ihu_vmmw_gprsmod_gprs_procedure(sub_opt);	//GPRS测试
	else if (option == 4) return ihu_vmmw_gprsmod_bs_procedure();		//基站测试
	else if (option == 5) return ihu_vmmw_gprsmod_tts_procedure();		//语音测试
	else
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error selection!\n");
		return IHU_FAILURE;
	}
	
	//正常返回
	//return IHU_SUCCESS;
}

/*******************************************************************************
* 函数名 : ihu_vmmw_gprsmod_module_procedure
* 描述   : SIM800A/GPRS主测试控制部分
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_module_procedure()
{
	char temp[20];
	int8_t *p1; 
	
	func_gprsmod_clear_receive_buffer();
	
	//查询制造商名称
	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGMI",(uint8_t*)"OK", 2) == IHU_SUCCESS)
	{ 
		p1=(int8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff+2),"\n");
		strncpy(temp, (char*)BSP_STM32_SPS_GPRS_R_Buff+2, (p1-BSP_STM32_SPS_GPRS_R_Buff>=sizeof(temp))?sizeof(temp):(p1-BSP_STM32_SPS_GPRS_R_Buff));
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: Maunfacture %s", temp);
		}
		func_gprsmod_clear_receive_buffer();
	}
	else
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Can not detect manfacture vendor!\n");
		return IHU_FAILURE;
	}
	
	//查询模块名字
	if(func_gprsmod_send_AT_command("AT+CGMM", (uint8_t*)"OK", 2)== IHU_SUCCESS)
	{ 
		p1=(int8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff+2),"\n"); 
		strncpy(temp, (char*)BSP_STM32_SPS_GPRS_R_Buff+2, (p1-BSP_STM32_SPS_GPRS_R_Buff>=sizeof(temp))?sizeof(temp):(p1-BSP_STM32_SPS_GPRS_R_Buff));
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: Module Type %s", temp);
		}
		func_gprsmod_clear_receive_buffer();
	}
	else
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Can not detect module name!\n");
		return IHU_FAILURE;
	}
	
	//查询产品序列号
	if(func_gprsmod_send_AT_command("AT+CGSN", (uint8_t*)"OK", 2)== IHU_SUCCESS)
	{ 
		p1=(int8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff+2),"\n"); 
		strncpy(temp, (char*)BSP_STM32_SPS_GPRS_R_Buff+2, (p1-BSP_STM32_SPS_GPRS_R_Buff>=sizeof(temp))?sizeof(temp):(p1-BSP_STM32_SPS_GPRS_R_Buff));
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: Product Serial ID %s", temp);
		}
		func_gprsmod_clear_receive_buffer();		
	}
	else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Can not detect product serial number!\n");
		return IHU_FAILURE;
	}
	
	//查询本机号码
	if(func_gprsmod_send_AT_command("AT+CNUM", (uint8_t*)"+CNUM", 2)== IHU_SUCCESS)
	{ 
		int8_t *p2;
		p1=(int8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),"\""); 
		p2=(int8_t*)strstr((const char*)(p1+1),"\"");
    p1=(int8_t*)strstr((const char*)(p2+1),"\"");
		strncpy(temp, (char*)p1+1, 11);
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: Local Number %s", temp);		
		}
		func_gprsmod_clear_receive_buffer();		
	}
	else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Can not detect local phone number!\n");
		return IHU_FAILURE;
	}
	
	//正常返回
	return IHU_SUCCESS;
}

/*******************************************************************************
* 函数名 : ihu_vmmw_gprsmod_gsm_info_procedure
* 描述   : SIM800A/GPRS状态信息检测(信号质量,电池电量,日期时间)
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_gsm_info_procedure(void)
{
	char temp[20];
	uint8_t *p1;
	uint8_t *p2;
	
	func_gprsmod_clear_receive_buffer();
	
	//查询SIM卡是否在位
	if(func_gprsmod_send_AT_command("AT+CPIN?", (uint8_t*)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: SIM card inserted!\n");
			func_gprsmod_clear_receive_buffer(); 
		}
	}
	else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Please check whether SIM card inserted!\n");
		return IHU_FAILURE;
	}

	//查询运营商名字
	if(func_gprsmod_send_AT_command("AT+COPS?", (uint8_t*)"OK", 2)== IHU_SUCCESS)		
	{ 
		p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),"\""); 
		if(p1)//有有效数据
		{
			p2=(uint8_t*)strstr((const char*)(p1+1),"\"");
			strncpy(temp, (char*)p1+1, (p2-p1-1>=sizeof(temp))?sizeof(temp):(p2-p1-1));
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
				IhuDebugPrint("VMFO: Operator: %s", temp);				
			}
		}
		func_gprsmod_clear_receive_buffer(); 
	}
	else 
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Please check whether valid operator!\n");
		return IHU_FAILURE;
	}
 
	//查询信号质量
	if(func_gprsmod_send_AT_command("AT+CSQ", (uint8_t*)"OK", 2) == IHU_SUCCESS)		
	{ 
		p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),":");
		if(p1)
		{
			p2=(uint8_t*)strstr((const char*)(p1+1),",");
			strncpy(temp, (char*)p1+2, (p2-p1-2>=sizeof(temp))?sizeof(temp):(p2-p1-2));
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
				IhuDebugPrint("VMFO: Singal quality: %s", temp);	
			}			
		}
		func_gprsmod_clear_receive_buffer();
	}
	else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Please check whether valid signal strength!\n");
		return IHU_FAILURE;
	}

	//General information inquery
  if(func_gprsmod_send_AT_command("AT+DDET=1", (uint8_t*)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module support DTMF audio-decode, support key-down during peer conversation.\n");
	}
	else{
    if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module not support DTMF audio-decode, Not support key-down during peer conversation.\n");
	}
	if(func_gprsmod_send_AT_command("AT+CTTS=?", (uint8_t*)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Support TTS local voice, Support to convert TEXT into voice.\n");
	}
	else{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module not support TTS local voice.\n");
	}
	if(func_gprsmod_send_AT_command("AT+CIPSIM800ALOC=?", (uint8_t*)"OK", 2) == IHU_SUCCESS)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module support base station positioning, able to fetch position information.\n");
	}
	else{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Module not support base station positioning.\n");
	}
	func_gprsmod_clear_receive_buffer();
	
	return IHU_SUCCESS;
}

/*******************************************************************************
* 函数名 : func_gprsmod_sim800a_sms_test
* 描述   : 短信测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_sms_procedure(void)
{
	uint8_t temp[50];
	uint8_t loc=0;
	
	//设置短信发送模式
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: @@@@ SMS TEST @@@@!\n");
	if (func_gprsmod_send_AT_command("AT+CMGF=1", (uint8_t *)"OK", 2) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Set SMS Send mode correctly!\n");
	}
	else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Set SMS Send mode wrong!\n");
		return IHU_FAILURE;
	}
	
	//设置短信上报模式，上报位置
  if (func_gprsmod_send_AT_command("AT+CNMI=3,1,0,0,0", (uint8_t *)"OK", 2) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Set SMS report mode!\n");
	}
	else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Set SMS report mode wrong!\n");
		return IHU_FAILURE;
	}

  //所有操作在SIM卡中进行
	if (func_gprsmod_send_AT_command("AT+CPMS=\"SM\",\"SM\",\"SM\"", (uint8_t *)"OK", 2) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Set SMS SIM card operation!\n");
	}
	else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Set SMS Sim card operation wrong!\n");
		return IHU_FAILURE;
	}
	
	//Step0: 设置短信中心号
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp,(const char *)"AT+CSCA=\"");
	strcat((char*)temp, IHU_GPRS_SMS_CENTER_NUMBER);
	strcat((char*)temp, "\"");
	if(func_gprsmod_send_AT_command(temp, (uint8_t *)"OK", 2) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Set SMS center successful!\n");
	}
	else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Set SMS center failure, CMD = [%s].\n", temp);
		return IHU_FAILURE;		
	}
		
	//Step1: Check SMS
	if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff, "+CMTI:"))
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO:  New SMS! SMS Position: [%s]\n", BSP_STM32_SPS_GPRS_R_Buff);
		func_gprsmod_clear_receive_buffer();
	}

	//Step2: 发送短信
	memset(temp, 0, sizeof(temp));
	strcpy((char*)temp,(const char *)"AT+CMGS=\"");
	strcat((char*)temp, IHU_GPRS_SMS_TARGET_NUMBER);
	strcat((char*)temp, "\"");
	func_gprsmod_send_AT_command(temp,">",2);//发送接收方号码
	strcpy((char*)temp, IHU_VMMW_GPRSMOD_TEST_SMS_CONTENT);
	loc = strlen((const char*)temp);
	temp[loc]='\32';
	temp[loc+1]=0x0D;	//Return
	temp[loc+2]='\0';
	//发送短信内容
	if(func_gprsmod_send_AT_command(temp, (uint8_t *)"OK", 5) == IHU_SUCCESS){
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Send SMS successful!\n");
	}
	else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Send SMS failure!\n");
		return IHU_FAILURE;		
	}

//	//Step3: 查看短信
//	IhuDebugPrint("VMFO: Inquery SMS\n");
//	BSP_STM32_SPS_PRINT_R_Buff[len]='\0';//添加结束符;
//	p=(uint8_t*)strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff, ": ");
//	strcpy((char*)temp,(const char*)"AT+CMGR=");
//	loc=strlen((const char*)temp);
//	strcpy((char*)(&temp[loc]),(const char*)(p+2));
//	loc=strlen((const char*)temp);
//	if(func_gprsmod_send_AT_command(temp,"OK",5)==0)
//	{
//		p=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff+2),"\r\n");
//		p1=(uint8_t*)strstr((const char*)(p+2),"\r\n");
//		loc=p1-p;
//		IhuDebugPrint("VMFO: SMS Content: ");
//		func_gprsmod_print_send_len((char*)p+2,loc);
//	}
//	func_gprsmod_clear_receive_buffer();
	return IHU_SUCCESS;
}

/*******************************************************************************
* 函数名 : func_gprsmod_sim800a_call_test
* 描述   : 拨号测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_call_procedure(uint8_t sub_opt)
{	
	uint8_t temp[50];
	if((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: @@@@ SIM800A CALL TEST @@@@!\n");		
		
	switch(sub_opt)
	{
		//有来电
		case 0:
			if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff, "RING"))
			{
				func_gprsmod_clear_receive_buffer();
				if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: CALL Coming\n");
			}
		break;
			
		//接听	
		case 1:
			if(func_gprsmod_send_AT_command("ATA", (uint8_t *)"OK", 2) == IHU_SUCCESS)
			{
				if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Hand-on successful\n");
			}
			else{
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: SIM800A Call hand-off failure!\n");
				return IHU_FAILURE;		
			}
		break;
		
		//挂断
		case 2:if(func_gprsmod_send_AT_command("ATH", (uint8_t *)"OK", 2) == IHU_SUCCESS)
			{
				if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Hand-off successful\n");
			}else{
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: SIM800A Call hand-on failure!\n");
				return IHU_FAILURE;				
			}
		break;
		
		//拨号
		case 3:
			memset(temp, 0, sizeof(temp));
			strcpy((char*)temp, "ATD");
			strcat((char*)temp, IHU_VMMW_GPRSMOD_TEST_CALLED_NUMBER);
			strcat((char*)temp, ";");
			//temp[strlen(temp)]= (uint8_t)';';
			if(func_gprsmod_send_AT_command(temp, (uint8_t *)"OK", 8) == IHU_SUCCESS)
			{
				if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Called number = %s, Call successful!\n", IHU_VMMW_GPRSMOD_TEST_CALLED_NUMBER);
			}
			else
			{
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: Called failure, Please re-dial!\n");
				return IHU_FAILURE;						
			}
		break;
			
		//发送DTMF
		case 4:
			memset(temp, 0, sizeof(temp));
			strcpy((char*)temp, "AT+VTS=");
			strcat((char*)temp, IHU_VMMW_GPRSMOD_TEST_DTMF_KEY);
			if (func_gprsmod_send_AT_command(temp, (uint8_t *)"OK",3) != IHU_SUCCESS)
			{
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: SIM800A Call, DTMF failure!\n");
				return IHU_FAILURE;		
			}
			if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,"DTMF:"))
			{
				if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Peer press key-down: %s\n", IHU_VMMW_GPRSMOD_TEST_DTMF_KEY);
				func_gprsmod_clear_receive_buffer();
			}
		break;
		
		default: 
			zIhuRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: Not supportted DTMF functions!\n");
			return IHU_FAILURE;
		//break;
	}
	return IHU_SUCCESS;
}


/*******************************************************************************
* 函数名 : func_gprsmod_sim800a_gprs_test
* 描述   : GPRS测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   :  为了保持连接，每空闲隔10秒发送一次心跳
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_gprs_procedure(uint8_t sub_opt)
{	
	uint8_t temp[200];
	IhuDebugPrint("VMFO: @@@@  GPRS TEST  @@@@\n");

	switch(sub_opt)
	{
		//检查下发的数据
		case 0:        
			if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff, "+IPD"))//判断上位机是否有数据下发
			{
				if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New content!\n");
				//p1=(uint8_t*)strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,",");
				//p2=(uint8_t*)strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,":");
				//func_gprsmod_print_send_len((char*)(p2+1),func_gprsmod_convert_str_to_int((p1+1),(p2-p1-1)));
				func_gprsmod_clear_receive_buffer();
			}
		break;
			
		//设置配置
		case 1: 
			memset(temp, 0, sizeof(temp));
			strcpy((char*)&temp, (const char*)"AT+CIPSTART=");
			strcat((char*)&temp, IHU_VMMW_GPRSMOD_STREAM_CHOICE);
			func_gprsmod_send_AT_command("AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 2);	//关闭连接
			func_gprsmod_send_AT_command("AT+CIPSHUT", (uint8_t*)"SHUT OK", 2);		//关闭移动场景
			func_gprsmod_send_AT_command("AT+CGCLASS=\"B\"", (uint8_t*)"OK", 2);//设置GPRS移动台类别为B,支持包交换和数据交换 
			func_gprsmod_send_AT_command("AT+CGDCONT=1,\"IP\",\"CMNET\"", (uint8_t*)"OK",2);//设置PDP上下文,互联网接协议,接入点等信息
			func_gprsmod_send_AT_command("AT+CGATT=1", (uint8_t*)"OK", 2);//附着GPRS业务
			func_gprsmod_send_AT_command("AT+CIPCSGP=1,\"CMNET\"", (uint8_t*)"OK", 2);//设置为GPRS连接模式
			func_gprsmod_send_AT_command("AT+CIPHEAD=1", (uint8_t*)"OK", 2);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
			func_gprsmod_send_AT_command("AT+CIPMUX=0", (uint8_t*)"OK", 2);//设置单路连接
			if(func_gprsmod_send_AT_command(temp, (uint8_t*)"OK", 2 ) == IHU_SUCCESS)//发起连接
			{
				if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: New connecting!\n");
			}else
			{
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: Set GPRS parameter error!\n");
				return IHU_FAILURE;
			}
		break;
			
		//发送内容	
		case 2:
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Begin to send..........!\n");
			if(func_gprsmod_send_AT_command("AT+CIPSEND", (uint8_t*)">", 2) == IHU_SUCCESS)
			{
				memset(temp, 0, sizeof(temp));
				strcpy((char*)temp, IHU_VMMW_GPRSMOD_STEAM_CONTENT);
				if(func_gprsmod_send_AT_command(temp, (uint8_t*)"SEND OK", 8) == IHU_SUCCESS)
				{ 								
					if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Send successful!\n");
				}else
				{
					zIhuRunErrCnt[TASK_ID_VMFO]++;
					IhuErrorPrint("VMFO: GPRS Send failure 1!\n");
					return IHU_FAILURE;
				}
			}
			else
			{
				ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: GPRS Send failure2!\n");
				return IHU_FAILURE;
			}
		break;
			
		//发送结束
		case 3:
			if(func_gprsmod_send_AT_command("AT+CIPSEND", (uint8_t*)">", 2)== IHU_SUCCESS)
			{
				ihu_l1hd_sps_gprs_send_data((uint8_t *)0x00, 1);
				ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1A, 1);//CTRL+Z,结束数据发送,启动一次传输								
				if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO: Heart-beat successful!\n");		 
			}else
			{
				ihu_l1hd_sps_gprs_send_data((uint8_t *)0X1B, 1);//ESC,取消发送
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: GPRS Send failure3!\n");
				return IHU_FAILURE;
			}
		break;
		
		//退出	
		case 4:
			func_gprsmod_send_AT_command("AT+CIPCLOSE=1", (uint8_t*)"CLOSE OK", 5);	//关闭连接
			func_gprsmod_send_AT_command("AT+CIPSHUT", (uint8_t*)"SHUT OK", 5);		//关闭移动场景
			return 1;
	}
	return IHU_SUCCESS;
}

/*******************************************************************************
* 函数名 : func_gprsmod_sim800a_jz_test
* 描述   : 基站定位测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_bs_procedure(void)
{
//  uint8_t *p1, *p3;
	//uint8_t *p2;
	IhuDebugPrint("VMFO: @@@@@@@@@@@@@@@@@@@@@Base Station POSITON TEST@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
	IhuDebugPrint("VMFO: EXIT TEST: EXIT + ENTER\n");
	func_gprsmod_send_AT_command("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",2);
	func_gprsmod_send_AT_command("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",2);
  if(func_gprsmod_send_AT_command("AT+SAPBR=1,1","OK",5))//激活
	{
	
		IhuDebugPrint("VMFO: Open failure, please conform module own this function or not\n");
		return 1;		
	}
	if(func_gprsmod_send_AT_command("AT+SAPBR=2,1","OK",5))
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
			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"EXIT"))//退出
			{
				 func_gprsmod_send_AT_command("AT+SAPBR=0,1","OK",2);//关闭
				 return 0;
			}
			else
			IhuDebugPrint("VMFO: ENTER ERROR\n");
		}
    if(func_gprsmod_send_AT_command("AT+CIPSIM800ALOC=1,1","OK",10)==0)//获取经纬度和时间
		{
        GPRS_UART_TIMER_RECON_Count=1;
//// 			  p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),",");
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
* 函数名 : func_gprsmod_sim800a_tts_test
* 描述   : TTS文本语音测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
OPSTAT ihu_vmmw_gprsmod_tts_procedure(void)
{
  uint16_t len=0;
  uint8_t temp_src[]="Hello, this is a test from BXXH!";
	uint8_t temp[IHU_VMMW_GPRSMOD_TTS_MAX_LEN+15];
	uint8_t loc=0;
	IhuDebugPrint("VMFO: @@@@@@@@@@@@@@@@@@@@@TTS TEXT VOICE TEST@@@@@@@@@@@@@@@@@@@@@@@\n");
	IhuDebugPrint("VMFO: EXIT TEST: EXIT + ENTER\n");
	IhuDebugPrint("VMFO: VOICE Brocasting: Please enter content + Enter\n");
  IhuDebugPrint("VMFO: Attention, support max 100 byte\n");
	if(func_gprsmod_send_AT_command("AT+CTTS=?","OK",2))
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
  func_gprsmod_send_AT_command(temp,"OK",3);
	while(1)
	{
		if(SPS_PRINT_RX_STA&0x8000)
		{
      len=SPS_PRINT_RX_STA&0X3FFF;
			if(len>(IHU_VMMW_GPRSMOD_TTS_MAX_LEN/2))len=(IHU_VMMW_GPRSMOD_TTS_MAX_LEN/2);
      SPS_PRINT_RX_STA=0;
			if((len==4)&&(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"EXIT")))//退出	
			return 0;
			func_gprsmod_send_string("AT+CTTS=2,\"");
			BSP_STM32_SPS_PRINT_R_Buff[len]='\"';
			BSP_STM32_SPS_PRINT_R_Buff[len+1]='\0';
			if(func_gprsmod_send_AT_command(BSP_STM32_SPS_PRINT_R_Buff,"OK",2))
				IhuDebugPrint("VMFO: Wait for previous frame accomplish\n");
		}
	}
}

/*******************************************************************************
*
*
*
*   循环测试业务流程函数
*
*
*
*
*******************************************************************************/

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
//				if(strstr((char*)BSP_STM32_SPS_PRINT_R_Buff,"DIAL"))
//					func_gprsmod_sim800a_call_test();	//电话测试
//				else
//				if(strstr((char*)BSP_STM32_SPS_PRINT_R_Buff,"SMS"))
//					func_gprsmod_sim800a_sms_test();		//短信测试
//				else
//				if(strstr((char*)BSP_STM32_SPS_PRINT_R_Buff,"GPRS"))
//					func_gprsmod_sim800a_gprs_test();	//GPRS测试
//				else
//				if(strstr((char*)BSP_STM32_SPS_PRINT_R_Buff,"LOC"))
//				  func_gprsmod_sim800a_jz_test();		//基站测试
//				else				if(strstr((char*)BSP_STM32_SPS_PRINT_R_Buff,"VOICE"))
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
//	if(func_gprsmod_send_AT_command((uint8_t*)"AT+CGMI",(uint8_t*)"OK",5)==0)//查询制造商名称
//	{ 
//		IhuDebugPrint("VMFO: Maunfacture:");
//		p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff+2),"\r\n");
//		func_gprsmod_print_send_len((char*)BSP_STM32_SPS_GPRS_R_Buff+2,p1-BSP_STM32_SPS_GPRS_R_Buff);
//		func_gprsmod_clear_receive_buffer(); 		
//	} 
//	if(func_gprsmod_send_AT_command("AT+CGMM","OK",5)==0)//查询模块名字
//	{ 
//		IhuDebugPrint("VMFO: Module Type:");
//		p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff+2),"\r\n"); 
//		func_gprsmod_print_send_len((char*)BSP_STM32_SPS_GPRS_R_Buff+2,p1-BSP_STM32_SPS_GPRS_R_Buff);
//		func_gprsmod_clear_receive_buffer();
//	} 
//	if(func_gprsmod_send_AT_command("AT+CGSN","OK",5)==0)//查询产品序列号
//	{ 
//		IhuDebugPrint("VMFO: Product Serial ID:");
//		p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff+2),"\r\n"); 
//		func_gprsmod_print_send_len((char*)BSP_STM32_SPS_GPRS_R_Buff+2,p1-BSP_STM32_SPS_GPRS_R_Buff);
//		func_gprsmod_clear_receive_buffer();		
//	}
//	if(func_gprsmod_send_AT_command("AT+CNUM","+CNUM",2)==0)//查询本机号码
//	{ 
//		uint8_t *p2;
//		IhuDebugPrint("VMFO: Local Number:");
//		p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),"\""); 
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
//		p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),"\""); 
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
//		p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),":");
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
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"EXIT"))return 0;
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"HAND-ON"))mode=1;
//			else
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"HAND-OFF"))mode=2;
//			else
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"CALL"))mode=3;
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
//				if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,"RING"))
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
//						func_gprsmod_print_send_len((char*)BSP_STM32_SPS_PRINT_R_Buff+4,len-4);
//						strcpy((char*)temp,"ATD");
//						for(i=3;i<(len-4+3);i++)
//						temp[i]=BSP_STM32_SPS_PRINT_R_Buff[i+1];
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
//				temp[7]=BSP_STM32_SPS_PRINT_R_Buff[0];
//				temp[8]='\0';
//				func_gprsmod_send_AT_command(temp,"OK",3);
//				}
//				if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,"DTMF:"))
//				{
//          ihu_usleep(10);
//					p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),":");
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
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"EXIT"))return 0;
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"SET"))mode=3;//设置短信中心号
//			else
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"INQUERY"))mode=2;//查看短信
//			else 
//      if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"+"))mode=1;//发送短信
//			else
//			  mode=0;

//		}
//		switch(mode)
//		{
//			case 0:
//        if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,"+CMTI:"))
//				{
//          IhuDebugPrint("VMFO: New SMS\n");
//					IhuDebugPrint("VMFO: SMS Position: ");
//					p=(uint8_t*)strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,",");
//					func_gprsmod_print_send_string((char*)p+1);
//					func_gprsmod_clear_receive_buffer();
//				}
//				break;
//			case 1:
//				IhuDebugPrint("VMFO: Send SMS\n");
//				BSP_STM32_SPS_PRINT_R_Buff[len]='\0';//添加结束符;
//				strcpy((char*)temp,(const char *)"AT+CMGS=\"+86");
//				loc=sizeof("AT+CMGS=\"+86");
//				p=(uint8_t*)strstr((char*)BSP_STM32_SPS_PRINT_R_Buff,(char*)"+");//查找发送内容
//				BSP_STM32_SPS_PRINT_R_Buff[(p-BSP_STM32_SPS_PRINT_R_Buff)]='\0';//添加结束符
//				strcpy((char*)(&temp[loc-1]),(const char*)BSP_STM32_SPS_PRINT_R_Buff);
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
//        BSP_STM32_SPS_PRINT_R_Buff[len]='\0';//添加结束符;
//				p=(uint8_t*)strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff, ": ");
//				strcpy((char*)temp,(const char*)"AT+CMGR=");
//			  loc=strlen((const char*)temp);
//				strcpy((char*)(&temp[loc]),(const char*)(p+2));
//				loc=strlen((const char*)temp);
//			  if(func_gprsmod_send_AT_command(temp,"OK",5)==0)
//				{
//					p=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff+2),"\r\n");
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
//				BSP_STM32_SPS_PRINT_R_Buff[len]='\0';//添加结束符;
//				strcpy((char*)temp,(const char *)"AT+CSCA=");
//			  loc=sizeof("AT+CSCA=");
//				strcpy((char*)(&temp[loc-1]),(const char*)&BSP_STM32_SPS_PRINT_R_Buff[4]);
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
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"EXIT"))mode=4;//退出
//			else
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"SET"))mode=1;//设置配置
//			else
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"SEND"))mode=2;//发送内容
//			else 												mode=0;			
//		}
//		else
//		{
//			if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,"CONNECT OK")&&(flag==1))//连接成功
//			{
//					IhuDebugPrint("VMFO: Connect Successful\n");
//					GPRS_UART_TIMER_RECON_Count=1;
//					flag=2;
//					mode=3;
//					func_gprsmod_clear_receive_buffer();
//			}else
//			if((flag==1)&&((strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,"CLOSED"))&&(GPRS_UART_TIMER_RECON_Count>10)))//连接失败或超时
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
//						if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,"+IPD"))//判断上位机是否有数据下发
//						{
//							IhuDebugPrint("VMFO: New content\n");
//							p1=(uint8_t*)strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,",");
//							p2=(uint8_t*)strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff,":");
//							func_gprsmod_print_send_len((char*)(p2+1),func_gprsmod_convert_str_to_int((p1+1),(p2-p1-1)));
//							IhuDebugPrint("\r\n");
//							func_gprsmod_clear_receive_buffer();
//						}
//			break;
//			case 1: 
//				     if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"TCP")||strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"UDP"))
//						 {
//                strcpy((char*)&temp,(const char*)"AT+CIPSTART=");
//							  memcpy((char*)&temp[12],(const char*)(&BSP_STM32_SPS_PRINT_R_Buff[4]),len-4);
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
//										 BSP_STM32_SPS_PRINT_R_Buff[len]='\32';
//										 BSP_STM32_SPS_PRINT_R_Buff[len+1]='\0';
//										 if(func_gprsmod_send_AT_command(&BSP_STM32_SPS_PRINT_R_Buff[4],"SEND OK",8)==0)
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
//			if(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"EXIT"))//退出
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
// 			  p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),",");
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
//			if(strstr(BSP_STM32_SPS_PRINT_R_Buff,"EXIT"))//退出
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
// 			  p1=(uint8_t*)strstr((const char*)(BSP_STM32_SPS_GPRS_R_Buff),",");
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
//			if((len==4)&&(strstr((const char*)BSP_STM32_SPS_PRINT_R_Buff,"EXIT")))//退出	
//			return 0;
//			func_gprsmod_send_string("AT+CTTS=2,\"");
//			BSP_STM32_SPS_PRINT_R_Buff[len]='\"';
//			BSP_STM32_SPS_PRINT_R_Buff[len+1]='\0';
//			if(func_gprsmod_send_AT_command(BSP_STM32_SPS_PRINT_R_Buff,"OK",2))
//				IhuDebugPrint("VMFO: Wait for previous frame accomplish\n");
//		}
//	}
//}

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
	func_gprsmod_clear_receive_buffer();
	func_gprsmod_send_string("AT+CIPCLOSE=1");	//关闭连接
  ihu_usleep(100);
	func_gprsmod_send_AT_command("AT+CIPSHUT","SHUT OK", 3);		//关闭移动场景
	func_gprsmod_send_AT_command("AT+CGCLASS=\"B\"","OK", 3);//设置GPRS移动台类别为B,支持包交换和数据交换 
	func_gprsmod_send_AT_command("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK", 3);//设置PDP上下文,互联网接协议,接入点等信息
	func_gprsmod_send_AT_command("AT+CGATT=1","OK", 3);//附着GPRS业务
	func_gprsmod_send_AT_command("AT+CIPCSGP=1,\"CMNET\"","OK", 3);//设置为GPRS连接模式
  func_gprsmod_send_AT_command("AT+CIPMUX=0","OK", 3);//设置为单路连接
	func_gprsmod_send_AT_command("AT+CIPHEAD=1","OK", 3);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	func_gprsmod_send_AT_command("AT+CIPMODE=1","OK", 3);//打开透传功能
	func_gprsmod_send_AT_command("AT+CIPCCFG=4,5,200,1","OK", 3);//配置透传模式：单包重发次数:2,间隔1S发送一次,每次发送200的字节
  func_gprsmod_send_AT_command((uint8_t *)GPRS_UART_string, "OK", 5);//建立连接
  
  ihu_usleep(100);                                //等待串口数据接收完毕
  func_gprsmod_clear_receive_buffer();                                    //清串口接收缓存为透传模式准备
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
//p2指向发送数据，p1指向接收数据，这里可以使用BSP_STM32_SPS_GPRS_R_Count来判断
//	if(p2!=p1)   		//说明还有数据未发出
//	{	
//		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET);
//		BSP_STM32_GPRS_UART_SendData(USART_GPRS, *p2);
//		p2++;
//    if(p2>&BSP_STM32_SPS_GPRS_R_Buff[BSP_STM32_SPS_GPRS_REC_MAXLEN])
//			p2=BSP_STM32_SPS_GPRS_R_Buff;
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

//	while(1)
//	{
//		Rec_Server_Data();//接收数据并原样返回	
//	}

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
* 函数名  : USART_PRINT_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 1)、只启动了USART_PRINT中断接收，未启动USART_PRINT中断发送。
*           2)、接收到0x0d 0x0a(回车、"\r\n")代表帧数据接收完成
*
*
* 本函数只是放在这儿，并没有被用上
*******************************************************************************/
//void USART_PRINT_IRQHandler(void)                	
//{
//	uint8_t res;

//	//res=USART_ReceiveData(USART_PRINT);
//	Time_UART_PRINT=0;
//	if((SPS_PRINT_RX_STA&0x8000)==0)//接收未完成
//	{
//		if(SPS_PRINT_RX_STA&0x4000)//接收到了0x0d
//		{
//			if(res!=0x0a)SPS_PRINT_RX_STA=0;//接收错误,重新开始
//			else SPS_PRINT_RX_STA|=0x8000;	//接收完成了 
//		}else //还没收到0X0D
//		{	
//			if(res==0x0d)SPS_PRINT_RX_STA|=0x4000;
//			else
//			{
//				BSP_STM32_SPS_PRINT_R_Buff[SPS_PRINT_RX_STA&0X3FFF]=res;
//				SPS_PRINT_RX_STA++;
//				if(SPS_PRINT_RX_STA>(BSP_STM32_SPS_PRINT_REC_MAXLEN-1))SPS_PRINT_RX_STA=0;//接收数据错误,重新开始接收	  
//			}		 
//		}
//	}  
//} 

/*******************************************************************************
* 函数名  : TIM2_IRQHandler
* 描述    : 定时器2中断断服务函数
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
//void TIM_USART_GPRS_IRQHandler(void)   //IHU_VMMW_GPRSMOD_TIM_CHAN_ID中断
//{
//	//if (TIM_GetITStatus(IHU_VMMW_GPRSMOD_TIM_CHAN_ID, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
//	{
//		//TIM_ClearITPendingBit(IHU_VMMW_GPRSMOD_TIM_CHAN_ID, TIM_IT_Update  );  //清除TIMx更新中断标志	
//		
//		//重连计数器
//		//GPRS_UART_TIMER_RECON_Count==1时启动计数，溢出自动停止
//		if(GPRS_UART_TIMER_RECON_Count>=1)GPRS_UART_TIMER_RECON_Count++;
//		
//		//延时计数器
//		if(GPRS_UART_TIMER_START_Flag) GPRS_UART_TIMER_DELAY_Count++;		

//		//延迟到达后，不再计数，重新启动
//		if(GPRS_UART_TIMER_DELAY_Count > (GPRS_UART_TIMER_WAIT_Duration))
//		{
//			GPRS_UART_TIMER_START_Flag = 0;
//			GPRS_UART_TIMER_DELAY_Count = 0;
//		}
//		
////		//接收IRQ的计数器，由GPRS_UART_IRQ触发
////		SPS_GPRS_TIMER_TRIGGER_Count++;
//		
//		//正在接收状态中
//		if(SPS_PRINT_RX_STA&(~0x8000))
//		{
//			Time_UART_PRINT++;
//			if(Time_UART_PRINT>=200)SPS_PRINT_RX_STA=0;//接收超时，复位接收
//		}
//	}	
//}

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
	for(k=0;k<BSP_STM32_SPS_GPRS_REC_MAXLEN;k++)      //将缓存内容清零
	{
		BSP_STM32_SPS_GPRS_R_Buff[k] = 0x00;
	}
  BSP_STM32_SPS_GPRS_R_Count = 0;               //接收字符串的起始存储位置
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
	int res;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / SPS_UART_RX_MAX_DELAY_DURATION;

	//清理接收缓冲区
	func_gprsmod_clear_receive_buffer();
	ihu_l1hd_sps_gprs_send_data((uint8_t *)cmd, strlen((char*)cmd));
	func_gprsmod_send_LR();	

//	//接收，注意时钟刻度
//	ihu_usleep(wait_time * 1000);
//	BSP_STM32_SPS_GPRS_RcvData_timeout((uint8_t*)BSP_STM32_SPS_GPRS_R_Buff, strlen((char *)ack), 0);
//	if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff, (char*)ack)==NULL) 
//		return IHU_FAILURE;
//	else 
//		return IHU_SUCCESS;
	
	res = IHU_FAILURE;
	while((tickTotal > 0) && (res == IHU_FAILURE))
	{
		ihu_usleep(SPS_UART_RX_MAX_DELAY_DURATION); //这里的周期就是以绝对ms为单位的
		tickTotal--;
		if(strstr((const char*)BSP_STM32_SPS_GPRS_R_Buff, (char*)ack)==NULL)
			 res = IHU_FAILURE;
		else
			 res = IHU_SUCCESS;
	}
	return res;
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
		for(k=0;k<BSP_STM32_SPS_GPRS_REC_MAXLEN;k++)      			
		{
			if(BSP_STM32_SPS_GPRS_R_Buff[k] == ':')
			{
				if((BSP_STM32_SPS_GPRS_R_Buff[k+4] == '1')||(BSP_STM32_SPS_GPRS_R_Buff[k+4] == '5'))   //说明注册成功
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
	func_gprsmod_send_AT_command("ATE0", "OK", 2);								//取消回显		
}



///*******************************************************************************
//* º¯ÊýÃû  : func_gprsmod_print_send_len
//* ÃèÊö    : USART_PRINT·¢ËÍÒ»¶¨³¤¶ÈµÄ×Ö·û
//* ÊäÈë    : *s×Ö·û´®Ö¸Õë£¬len£º×Ö·û³¤¶È
//* Êä³ö    : ÎÞ
//* ·µ»Ø    : ÎÞ 
//* ËµÃ÷    : ÎÞ
//*******************************************************************************/
//void func_gprsmod_print_send_len(char *s,uint8_t len)
//{
//	if(s!=0)
//	while(len)//Checking end char
//	{
//		while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
//		USART_SendData(USART_PRINT ,*s++);//Send current char
//		len--;
//	}
//}	

///*******************************************************************************
//* º¯ÊýÃû  : func_gprsmod_print_data
//* ÃèÊö    : USART_PRINT·¢ËÍÒ»¸ö×Ö½Ú
//* ÊäÈë    : byte Ò»¸ö×Ö½Ú
//* Êä³ö    : ÎÞ
//* ·µ»Ø    : ÎÞ 
//* ËµÃ÷    : ÎÞ
//*******************************************************************************/
//void func_gprsmod_print_data(char byte)
//{
//		while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
//		USART_SendData(USART_PRINT ,byte);//Send current char
//}

//void func_gprsmod_print_send_string(char* s)
//{
//	while(*s)//check end char
//	{
//		while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
//		USART_SendData(USART_PRINT ,*s++);//Send current char
//	}
//}

//void func_gprsmod_data_byte_send(char byte)
//{
//		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET); 
//		USART_SendData(USART_GPRS ,byte);//Send current char
//}


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
  if(strstr((char *)BSP_STM32_SPS_GPRS_R_Buff, a)!=NULL)
	    return 1;
	else
			return 0;
}

