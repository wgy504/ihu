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
 
#include "mod_hal_gprs.h"

//全局变量，引用外部
extern int8_t 	SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN];		//串口GPRS数据接收缓冲区 
extern int8_t 	GPRS_UART_R_State;												//串口GPRS接收状态
extern int16_t	SPS_GPRS_R_Count;												//当前接收数据的字节数
extern uint8_t 	SPS_GPRS_TIMER_TRIGGER_Count;  					//串口GPRS的时间计时器，貌似没有什么用

//跟时钟/USART相关的本地变量
uint8_t 	GPRS_UART_TIMER_WAIT_Duration=0;        		//传递等待时长
uint16_t 	GPRS_UART_TIMER_DELAY_Count=0;      				//延时变量
uint8_t 	GPRS_UART_TIMER_START_Flag=0;							//定时器0延时启动计数器
uint8_t 	GPRS_UART_TIMER_RECON_Count=0;							//链路重连接的时长计数器，防止超时

/*************  本地变量声明	**************/
uint8_t 	SPS_PRINT_R_Buf[SPS_PRINT_REC_MAXLEN];		//串口1数据缓存区
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

/*******************************************************************************
* 函数名 : GPRS_UART_GSM_test_selection
* 描述   : GSM主测试程序
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void GPRS_UART_GSM_working_procedure_selection(uint8_t option)
{
	uint8_t sim_ready=0;
	while(GPRS_UART_send_AT_command((uint8_t*)"AT",(uint8_t*)"OK", 2))//查询是否应到AT指令
	{
		IhuDebugPrint("VMUO: Not dected module!\n");
		osDelay(400);
		IhuDebugPrint("VMUO: Trying to reconnecting!\n");
		osDelay(400);  
	}
	GPRS_UART_send_AT_command((uint8_t*)"ATE0", (uint8_t*)"OK", 2);//不回显
	GPRS_UART_GSM_module_procedure();
	if(GPRS_UART_GSM_gsm_info_procedure()==0) sim_ready=1;
	
	if ((sim_ready == 1) && (option == 1)) GPRS_UART_GSM_call_procedure();	//电话测试
	else if ((sim_ready == 1) && (option == 2)) GPRS_UART_GSM_sms_procedure();		//短信测试
	else if ((sim_ready == 1) && (option == 3)) GPRS_UART_GSM_gprs_procedure();	//GPRS测试
	else if ((sim_ready == 1) && (option == 4)) GPRS_UART_GSM_bs_procedure();		//基站测试
	else if ((sim_ready == 1) && (option == 5)) GPRS_UART_GSM_tts_procedure();		//语音测试
	else IhuDebugPrint("VMUO: Error selection!\n");
	
	return;
}

/*******************************************************************************
* 函数名 : GPRS_UART_GSM_gsm_info_procedure
* 描述   : GSM/GPRS状态信息检测(信号质量,电池电量,日期时间)
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
uint8_t GPRS_UART_GSM_gsm_info_procedure(void)
{
	char temp[20];
	uint8_t *p1;
	uint8_t *p2;
	uint8_t res=0;
	GPRS_UART_clear_receive_buffer();
	if(GPRS_UART_send_AT_command("AT+CPIN?","OK", 3))
	{
    IhuDebugPrint("VMUO: Please check whether SIM card inserted!\n");
		res|=1<<0;	//查询SIM卡是否在位 
	}
	GPRS_UART_clear_receive_buffer(); 
	
	if(GPRS_UART_send_AT_command("AT+COPS?","OK",3)==0)		//查询运营商名字
	{ 
		p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),"\""); 
		if(p1)//有有效数据
		{
			p2=(uint8_t*)strstr((const char*)(p1+1),"\"");
			strncpy(temp, (char*)p1+1, (p2-p1-1>=sizeof(temp))?sizeof(temp):(p2-p1-1));
			IhuDebugPrint("VMUO: Opertor: %s \n", temp);
		}
	}else res|=1<<1;
	GPRS_UART_clear_receive_buffer();
 
	if(GPRS_UART_send_AT_command("AT+CSQ", "OK", 3)==0)		//查询信号质量
	{ 
		p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),":");
		if(p1)
		{
			p2=(uint8_t*)strstr((const char*)(p1+1),",");
			strncpy(temp, (char*)p1+2, (p2-p1-2>=sizeof(temp))?sizeof(temp):(p2-p1-2));
			IhuDebugPrint("VMUO: Singal quality: %s \n", temp);
		}		
	}else res|=1<<2;

  if(GPRS_UART_send_AT_command("AT+DDET=1","OK",3)==0)
	{
		IhuDebugPrint("VMUO: Module support DTMF audio-decode, support key-down during peer conversation.\n");
	}else
	{
    IhuDebugPrint("VMUO: Module not support DTMF audio-decode, Not support key-down during peer conversation.\n");
	}
	if(GPRS_UART_send_AT_command("AT+CTTS=?","OK",3)==0)
	{
		IhuDebugPrint("VMUO: Support TTS local voice, Support to convert TEXT into voice.\n");
	}
	else
	{
		IhuDebugPrint("VMUO: Module not support TTS local voice.\n");
	}
	if(GPRS_UART_send_AT_command("AT+CIPGSMLOC=?","OK",3)==0)
	{
		IhuDebugPrint("VMUO: Module support base station positioning, able to fetch position information.\n");
	}
	else
	{
		IhuDebugPrint("VMUO: Module not support base station positioning.\n");
	}
	GPRS_UART_clear_receive_buffer();
	
	return res;
}

/*******************************************************************************
* 函数名 : GPRS_UART_GSM_module_procedure
* 描述   : GSM/GPRS主测试控制部分
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
uint8_t GPRS_UART_GSM_module_procedure()
{
	char temp[20];
	int8_t *p1; 
	IhuDebugPrint("VMUO: GSM/GPRS Test Program\n");  
	GPRS_UART_clear_receive_buffer(); 
	if(GPRS_UART_send_AT_command((uint8_t*)"AT+CGMI",(uint8_t*)"OK",5)==0)//查询制造商名称
	{ 
		p1=(int8_t*)strstr((const char*)(SPS_GPRS_R_Buff+2),"\n");
		strncpy(temp, (char*)SPS_GPRS_R_Buff+2, (p1-SPS_GPRS_R_Buff>=sizeof(temp))?sizeof(temp):(p1-SPS_GPRS_R_Buff));
		IhuDebugPrint("VMUO: Maunfacture %s \n", temp);
		GPRS_UART_clear_receive_buffer(); 		
	} 
	if(GPRS_UART_send_AT_command("AT+CGMM","OK",5)==0)//查询模块名字
	{ 
		p1=(int8_t*)strstr((const char*)(SPS_GPRS_R_Buff+2),"\n"); 
		strncpy(temp, (char*)SPS_GPRS_R_Buff+2, (p1-SPS_GPRS_R_Buff>=sizeof(temp))?sizeof(temp):(p1-SPS_GPRS_R_Buff));
		IhuDebugPrint("VMUO: Module Type %s \n", temp);
		GPRS_UART_clear_receive_buffer();
	} 
	if(GPRS_UART_send_AT_command("AT+CGSN","OK",5)==0)//查询产品序列号
	{ 
		p1=(int8_t*)strstr((const char*)(SPS_GPRS_R_Buff+2),"\n"); 
		strncpy(temp, (char*)SPS_GPRS_R_Buff+2, (p1-SPS_GPRS_R_Buff>=sizeof(temp))?sizeof(temp):(p1-SPS_GPRS_R_Buff));
		IhuDebugPrint("VMUO: Product Serial ID %s \n", temp);
		GPRS_UART_clear_receive_buffer();		
	}
	if(GPRS_UART_send_AT_command("AT+CNUM","+CNUM",2)==0)//查询本机号码
	{ 
		int8_t *p2;
		p1=(int8_t*)strstr((const char*)(SPS_GPRS_R_Buff),"\""); 
		p2=(int8_t*)strstr((const char*)(p1+1),"\"");
    p1=(int8_t*)strstr((const char*)(p2+1),"\"");
		strncpy(temp, (char*)p1+1, 11);
		IhuDebugPrint("VMUO: Local Number %s \n", temp);		
		GPRS_UART_clear_receive_buffer();		
	}
	return 1;
}


/*******************************************************************************
* 函数名 : GPRS_UART_GSM_call_test
* 描述   : 拨号测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
uint8_t GPRS_UART_GSM_call_procedure(void)
{	
	uint8_t temp[50];
	uint16_t len=0;
	uint16_t i=0;
	uint8_t mode=0;
	uint8_t flag=0;
  //uint8_t *p1=NULL;
	IhuDebugPrint("VMUO: @@@@@@@@@@@DIAL TEST@@@@@@@@@@@\n");
	IhuDebugPrint("VMUO: DIAL CALL: Input 'CALL xxxx + ENTER'\n");
	IhuDebugPrint("VMUO: HAND-ON CALL: Input 'HAND-ON + ENTER'\n");
  IhuDebugPrint("VMUO: HAND-OFF CALL: Input 'HAND-OFF + ENTER'\n");
	IhuDebugPrint("VMUO: DTMF VOICE: Input 'Single Char + ENTER'\n");
	IhuDebugPrint("VMUO: EXIT TEST: Input 'EXIT + ENTER'\n");
	while(1)
	{
		if(SPS_PRINT_RX_STA&0x8000)
		{
			len=SPS_PRINT_RX_STA&0X3FFF;
			SPS_PRINT_RX_STA=0;
			if(strstr((const char*)SPS_PRINT_R_Buf,"EXIT"))return 0;
			if(strstr((const char*)SPS_PRINT_R_Buf,"HAND-ON"))mode=1;
			else
			if(strstr((const char*)SPS_PRINT_R_Buf,"HAND-OFF"))mode=2;
			else
			if(strstr((const char*)SPS_PRINT_R_Buf,"CALL"))mode=3;
			else
			if(mode==0)mode=0;//无效
			else 
			{
				mode=4;
				flag=1;
			}
		}
		switch(mode)
		{
			case 0://有来电
				if(strstr((const char*)SPS_GPRS_R_Buff,"RING"))
				{
					GPRS_UART_clear_receive_buffer();
					IhuDebugPrint("VMUO: CALL Coming\n");
				}
				break;
			case 1:
						 if(GPRS_UART_send_AT_command("ATA","OK",2)==0)//接听
						 {
							IhuDebugPrint("VMUO: Hand-on successful\n");
						  mode=4; 
						 }
				break;
			case 2:if(GPRS_UART_send_AT_command("ATH","OK",2)==0)//挂断
							IhuDebugPrint("VMUO: Hand-off successful\n");		
						 mode=0;
				break;
			case 3://拨号
						IhuDebugPrint("VMUO:Call:");
						//USART_PRINT_Send_Len((char*)SPS_PRINT_R_Buf+4,len-4);
						strcpy((char*)temp,"ATD");
						for(i=3;i<(len-4+3);i++)
						temp[i]=SPS_PRINT_R_Buf[i+1];
						temp[i++]=';';
						temp[i++]='\0';
						if(GPRS_UART_send_AT_command(temp,"OK",8)==0)
						{
							IhuDebugPrint("VMUO: Call successful\n");
							mode=4;
						}
						else
						{
							IhuDebugPrint("VMUO: Please re-dial\n");
						  mode=0;
						}
			  break;
			case 4://发送DTMF
				if(flag)
				{
				flag=0;
				strcpy((char*)temp,"AT+VTS=");
				temp[7]=SPS_PRINT_R_Buf[0];
				temp[8]='\0';
				GPRS_UART_send_AT_command(temp,"OK",3);
				}
				if(strstr((const char*)SPS_GPRS_R_Buff,"DTMF:"))
				{
          osDelay(10);
//					p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),":");
					IhuDebugPrint("VMUO: Peer press key-down: ");
					//USART_PRINT_Data(*(p1+1));
					IhuDebugPrint("\r\n");	
					GPRS_UART_clear_receive_buffer();
				}
				break;
		}
		
	}
}
/*******************************************************************************
* 函数名 : GPRS_UART_GSM_sms_test
* 描述   : 短信测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
uint8_t GPRS_UART_GSM_sms_procedure(void)
{
	uint8_t temp[50];
	uint8_t loc=0;
	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@@@@@ SMS TEST @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	IhuDebugPrint("VMUO: Set SMS Center: Set +8613010314500, set firstly and then make SMS!\n");
	GPRS_UART_send_AT_command("AT+CMGF=1","OK",2);//设置短信发送模式
  GPRS_UART_send_AT_command("AT+CNMI=3,1,0,0,0","OK",2);//设置短信上报模式，上报位置
  GPRS_UART_send_AT_command("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);		//所有操作在SIM卡中进行
	
	//Step0: 设置短信中心号
	memset(temp, 0, sizeof(temp));
	IhuDebugPrint("VMUO: Set SMS Center Number\n");
	strcpy((char*)temp,(const char *)"AT+CSCA=\"+8613010314500\"");
	if(GPRS_UART_send_AT_command(temp, "OK", 3)==1)
		IhuDebugPrint("VMUO: Set SMS center successful\n");
	else
		IhuDebugPrint("VMUO: Set SMS center failure\n");
	
	//Step1: Check SMS
	if(strstr((const char*)SPS_GPRS_R_Buff, "+CMTI:"))
	{
		IhuDebugPrint("VMUO: New SMS!\n");
		IhuDebugPrint("VMUO: SMS Position: [%s]\n", SPS_GPRS_R_Buff);
		GPRS_UART_clear_receive_buffer();
	}

	//Step2: 发送短信
	memset(temp, 0, sizeof(temp));
	IhuDebugPrint("VMUO: Send SMS\n");
	strcpy((char*)temp,(const char *)"AT+CMGS=\"+8613701629240\"");
	GPRS_UART_send_AT_command(temp,">",2);//发送接收方号码
	strcpy((char*)temp, "This is ZJL test!");
	loc = strlen((const char*)temp);
	temp[loc]='\32';
	temp[loc+1]=0x0D;	//Return
	temp[loc+2]='\0';
	osDelay(200);
	if(GPRS_UART_send_AT_command(temp,"OK", 2)==1)//发送短信内容
		IhuDebugPrint("VMUO: Send SMS successful\n");
	else
		IhuDebugPrint("VMUO: Send SMS failure\n");

//	//Step3: 查看短信
//	IhuDebugPrint("VMUO: Inquery SMS\n");
//	SPS_PRINT_R_Buf[len]='\0';//添加结束符;
//	p=(uint8_t*)strstr((const char*)SPS_PRINT_R_Buf, ": ");
//	strcpy((char*)temp,(const char*)"AT+CMGR=");
//	loc=strlen((const char*)temp);
//	strcpy((char*)(&temp[loc]),(const char*)(p+2));
//	loc=strlen((const char*)temp);
//	if(GPRS_UART_send_AT_command(temp,"OK",5)==0)
//	{
//		p=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff+2),"\r\n");
//		p1=(uint8_t*)strstr((const char*)(p+2),"\r\n");
//		loc=p1-p;
//		IhuDebugPrint("VMUO: SMS Content: ");
//		USART_PRINT_Send_Len((char*)p+2,loc);
//	}
//	GPRS_UART_clear_receive_buffer();
	return 1;
}

/*******************************************************************************
* 函数名 : GPRS_UART_GSM_gprs_test
* 描述   : GPRS测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   :  为了保持连接，每空闲隔10秒发送一次心跳
*******************************************************************************/
uint8_t GPRS_UART_GSM_gprs_procedure(void)
{	
  uint16_t len=0;
	uint8_t mode=0;
	uint8_t temp[200];
	uint8_t flag=0;
//	uint8_t *p1;
//	uint8_t *p2;
	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@GPRS TEST@@@@@@@@@@@@@@@@@@@@@\n");
	IhuDebugPrint("VMUO: Set Parameter: Set\"MODE\",\"IP\", Port+ Enter\n Eg: Set\"TCP\",\"125.89.18.79\",12345\n");
	IhuDebugPrint("VMUO: Send content: Send + Content + Enter \n, Eg: This is a test!\n");
	IhuDebugPrint("VMUO: Exit Test: Exit + Enter\n");
	
	while(1)
	{
		if(SPS_PRINT_RX_STA&0x8000)
		{
			len=SPS_PRINT_RX_STA&0X3FFF;
			if(len>SPS_GPRS_REC_MAXLEN-2)len=SPS_GPRS_REC_MAXLEN-2;
			SPS_PRINT_RX_STA=0;
			if(strstr((const char*)SPS_PRINT_R_Buf,"EXIT"))mode=4;//退出
			else
			if(strstr((const char*)SPS_PRINT_R_Buf,"SET"))mode=1;//设置配置
			else
			if(strstr((const char*)SPS_PRINT_R_Buf,"SEND"))mode=2;//发送内容
			else 												mode=0;			
		}
		else
		{
			if(strstr((const char*)SPS_GPRS_R_Buff,"CONNECT OK")&&(flag==1))//连接成功
			{
					IhuDebugPrint("VMUO: Connect Successful\n");
					GPRS_UART_TIMER_RECON_Count=1;
					flag=2;
					mode=3;
					GPRS_UART_clear_receive_buffer();
			}else
			if((flag==1)&&((strstr((const char*)SPS_GPRS_R_Buff,"CLOSED"))&&(GPRS_UART_TIMER_RECON_Count>10)))//连接失败或超时
			{
					IhuDebugPrint("VMUO: Connect failure\n");
					
				  //while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
					//USART_SendData(USART_PRINT ,GPRS_UART_TIMER_RECON_Count);//发送当前字符
				
					GPRS_UART_TIMER_RECON_Count=0;
					flag=0;
					mode=1; //重新连接
					GPRS_UART_clear_receive_buffer();
			}	
			if((flag==2)&&(GPRS_UART_TIMER_RECON_Count>10))//每10秒一个心跳包
			{
				GPRS_UART_TIMER_RECON_Count=1;
				mode=3;
			}
	  }
    switch(mode)
		{
			case 0:        
						if(strstr((const char*)SPS_GPRS_R_Buff,"+IPD"))//判断上位机是否有数据下发
						{
							IhuDebugPrint("VMUO: New content\n");
//							p1=(uint8_t*)strstr((const char*)SPS_GPRS_R_Buff,",");
//							p2=(uint8_t*)strstr((const char*)SPS_GPRS_R_Buff,":");
							//USART_PRINT_Send_Len((char*)(p2+1),GPRS_UART_change_str_Data((p1+1),(p2-p1-1)));
							IhuDebugPrint("\r\n");
							GPRS_UART_clear_receive_buffer();
						}
			break;
			case 1: 
				     if(strstr((const char*)SPS_PRINT_R_Buf,"TCP")||strstr((const char*)SPS_PRINT_R_Buf,"UDP"))
						 {
                strcpy((char*)&temp,(const char*)"AT+CIPSTART=");
							  memcpy((char*)&temp[12],(const char*)(&SPS_PRINT_R_Buf[4]),len-4);
							  temp[len+12]='\0';
							  GPRS_UART_send_AT_command("AT+CIPCLOSE=1","CLOSE OK",2);	//关闭连接
								GPRS_UART_send_AT_command("AT+CIPSHUT","SHUT OK",2);		//关闭移动场景
 							  GPRS_UART_send_AT_command("AT+CGCLASS=\"B\"","OK",2);//设置GPRS移动台类别为B,支持包交换和数据交换 
								GPRS_UART_send_AT_command("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",2);//设置PDP上下文,互联网接协议,接入点等信息
								GPRS_UART_send_AT_command("AT+CGATT=1","OK",2);//附着GPRS业务
								GPRS_UART_send_AT_command("AT+CIPCSGP=1,\"CMNET\"","OK",2);//设置为GPRS连接模式
								GPRS_UART_send_AT_command("AT+CIPHEAD=1","OK",2);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
                GPRS_UART_send_AT_command("AT+CIPMUX=0","OK",2);//设置单路连接
                if(GPRS_UART_send_AT_command(temp,"OK",2 )==0)//发起连接
								{
                  IhuDebugPrint("VMUO: New connecting\n");
									GPRS_UART_TIMER_RECON_Count=1;
									flag=1;
								}
								else
								{
									IhuDebugPrint("VMUO: Enter error: Please re-enter\n");
									GPRS_UART_TIMER_RECON_Count=0;
									flag=0;
								}
						 }
						 else
						 {
							  IhuDebugPrint("VMUO: Enter error: Please re-enter\n");
						 }
             mode=0;
			break;
			case 2:
              if(flag==2)
							{
                IhuDebugPrint("VMUO: Begin to send..........\n");
								if(GPRS_UART_send_AT_command("AT+CIPSEND",">",2)==0)
								{
										 SPS_PRINT_R_Buf[len]='\32';
										 SPS_PRINT_R_Buf[len+1]='\0';
										 if(GPRS_UART_send_AT_command(&SPS_PRINT_R_Buf[4],"SEND OK",8)==0)
										 { 								
													IhuDebugPrint("VMUO: Send successful\n");
													GPRS_UART_TIMER_RECON_Count=1;
										 }
										 else
											 IhuDebugPrint("VMUO: Send failure\n");
										 
								}else
								{
										 //GPRS_UART_Data_byte_send(0X1B);//ESC,取消发送
                     IhuDebugPrint("VMUO: Send failure\n");
								}
						  }else IhuDebugPrint("VMUO: Not connected\n");
							mode=0;
			break;
			case 3:
						if(GPRS_UART_send_AT_command("AT+CIPSEND",">",2)==0)
						{
							//GPRS_UART_Data_byte_send(0x00);
							//GPRS_UART_Data_byte_send(0X1A);//CTRL+Z,结束数据发送,启动一次传输								
							IhuDebugPrint("VMUO: Heart-beat successful\n");

								 
						}else
						{
								 //GPRS_UART_Data_byte_send(0X1B);//ESC,取消发送
							IhuDebugPrint("VMUO: Heart-beat failure\n");
						}
				mode=0;
			break;
			case 4:
						GPRS_UART_send_AT_command("AT+CIPCLOSE=1","CLOSE OK",5);	//关闭连接
						GPRS_UART_send_AT_command("AT+CIPSHUT","SHUT OK",5);		//关闭移动场景
						return 1;
		}      
	}
}

/*******************************************************************************
* 函数名 : GPRS_UART_GSM_jz_test
* 描述   : 基站定位测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
uint8_t GPRS_UART_GSM_bs_procedure(void)
{
//  uint8_t *p1, *p3;
	//uint8_t *p2;
	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@Base Station POSITON TEST@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
	IhuDebugPrint("VMUO: EXIT TEST: EXIT + ENTER\n");
	GPRS_UART_send_AT_command("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",2);
	GPRS_UART_send_AT_command("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",2);
  if(GPRS_UART_send_AT_command("AT+SAPBR=1,1","OK",5))//激活
	{
	
		IhuDebugPrint("VMUO: Open failure, please conform module own this function or not\n");
		return 1;		
	}
	if(GPRS_UART_send_AT_command("AT+SAPBR=2,1","OK",5))
	{
    IhuDebugPrint("VMUO: Fetch local IP error\n");
		return 2;
	}
  GPRS_UART_TIMER_RECON_Count=1;
	while(1)
	{
    if(SPS_PRINT_RX_STA&0x8000)
		{
			SPS_PRINT_RX_STA=0;
			if(strstr((const char*)SPS_PRINT_R_Buf,"EXIT"))//退出
			{
				 GPRS_UART_send_AT_command("AT+SAPBR=0,1","OK",2);//关闭
				 return 0;
			}
			else
			IhuDebugPrint("VMUO: ENTER ERROR\n");
		}
    if(GPRS_UART_send_AT_command("AT+CIPGSMLOC=1,1","OK",10)==0)//获取经纬度和时间
		{
        GPRS_UART_TIMER_RECON_Count=1;
//// 			  p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),",");
////				p3=(uint8_t*)strstr((const char*)p1,"\r\n");
//				if(p1)//有效数据
//				{	
//					p2=(uint8_t*)strtok((char*)(p1),",");
//          IhuDebugPrint("VMUO: Longitude:");
//					//USART_PRINT_SendString((char *)p2);
//					//USART_PRINT_Data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          //IhuDebugPrint("VMUO: Latitude:");USART_PRINT_SendString((char *)p2);USART_PRINT_Data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          //IhuDebugPrint("VMUO: Date:");USART_PRINT_SendString((char *)p2);USART_PRINT_Data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//					*p3='\0';//插入结束符
//          //IhuDebugPrint("VMUO: Time:");USART_PRINT_SendString((char *)p2);IhuDebugPrint("\r\n");
//				}
		}
		if(GPRS_UART_TIMER_RECON_Count>11)
		{
      GPRS_UART_TIMER_RECON_Count=0;
			//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
			IhuDebugPrint("VMUO: Open failure, please confirm this module own this function\n");
			return 1;
		}
	}
}

/*******************************************************************************
* 函数名 : GPRS_UART_GSM_tts_test
* 描述   : TTS文本语音测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
uint8_t GPRS_UART_GSM_tts_procedure(void)
{
  uint16_t len=0;
  uint8_t temp_src[]="Hello, this is a test from BXXH!";
	uint8_t temp[GPRS_UART_TTS_MAX_len+15];
	uint8_t loc=0;
	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@TTS TEXT VOICE TEST@@@@@@@@@@@@@@@@@@@@@@@\n");
	IhuDebugPrint("VMUO: EXIT TEST: EXIT + ENTER\n");
	IhuDebugPrint("VMUO: VOICE Brocasting: Please enter content + Enter\n");
  IhuDebugPrint("VMUO: Attention, support max 100 byte\n");
	if(GPRS_UART_send_AT_command("AT+CTTS=?","OK",2))
	{
		IhuDebugPrint("VMUO: Error: Please inquery this module own this function or not\n");
		 return 1;
	}
	strcpy((char*)&temp,(const char*)"AT+CTTS=2,\"");
	loc=sizeof("AT+CTTS=2,\"");
  len=strlen((const char*)temp_src);
	strcpy((char*)&temp[loc-1],(const char*)temp_src);
	temp[loc+len-1]='\"';
	temp[loc+len]='\0';
  GPRS_UART_send_AT_command(temp,"OK",3);
	while(1)
	{
		if(SPS_PRINT_RX_STA&0x8000)
		{
      len=SPS_PRINT_RX_STA&0X3FFF;
			if(len>(GPRS_UART_TTS_MAX_len/2))len=(GPRS_UART_TTS_MAX_len/2);
      SPS_PRINT_RX_STA=0;
			if((len==4)&&(strstr((const char*)SPS_PRINT_R_Buf,"EXIT")))//退出	
			return 0;
			GPRS_UART_SendString("AT+CTTS=2,\"");
			SPS_PRINT_R_Buf[len]='\"';
			SPS_PRINT_R_Buf[len+1]='\0';
			if(GPRS_UART_send_AT_command(SPS_PRINT_R_Buf,"OK",2))
				IhuDebugPrint("VMUO: Wait for previous frame accomplish\n");
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
//* 注意   : 串口2负责与GSM模块通信，串口1用于串口调试，可以避免在下载程序时数据
//					 还发送到模块
//*******************************************************************************/
//uint8_t GPRS_UART_gsm_loop_test_main(void)
//{
//	//Print忽略，因为PRINT_USART3已经在BSP_SER中初始化了
//	//USART_PRINT_Init_Config(115200);
//	//SPS_GRPS忽略，因为调用模块本来就会初始发串口
//	GPRS_UART_Init_Config(115200);
//	TIM_USART_GPRS_Init_Config();
//	IhuDebugPrint("VMUO: GSM Module SMS test procedure!\n");//打印信息
//	IhuDebugPrint("VMUO: GSM Module Register network!\n");
//  GPRS_UART_GSM_test_loop(); //GSM测试程序
//	return 1;
//}


///*******************************************************************************
//* 函数名 : GPRS_UART_GSM_test_loop
//* 描述   : GSM主测试程序
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//void GPRS_UART_GSM_test_loop(void)
//{
//	uint8_t sim_ready=0;
//	while(GPRS_UART_send_AT_command((uint8_t*)"AT",(uint8_t*)"OK",5))//查询是否应到AT指令
//	{
//		IhuDebugPrint("VMUO: Not dected module!\n");
//		osDelay(800);
//		IhuDebugPrint("VMUO: Trying to reconnecting!\n");
//		osDelay(400);  
//	} 	 
//	GPRS_UART_send_AT_command((uint8_t*)"ATE0",(uint8_t*)"OK",200);//不回显
//	GPRS_UART_GSM_mtest();
//	if(GPRS_UART_GSM_gsm_test_info()==0)
//	{
//		sim_ready=1;
//		IhuDebugPrint("VMUO: Please select: Chinese+Enter, then re-send\n"); 				    	 
//		IhuDebugPrint("VMUO: DIAL\t"); 				    	 
//		IhuDebugPrint("VMUO: SMS\t");				    	 
//		IhuDebugPrint("VMUO: GPRS\t");
//		IhuDebugPrint("VMUO: LOC\t");
//		IhuDebugPrint("VMUO: VOICE\r\n");
//	}
//	while(1)
//	{
//		if(sim_ready)
//		{
//			if(SPS_PRINT_RX_STA & 0x8000)
//			{
//				SPS_PRINT_RX_STA=0;
//				if(strstr((char*)SPS_PRINT_R_Buf,"DIAL"))
//					GPRS_UART_GSM_call_test();	//电话测试
//				else
//				if(strstr((char*)SPS_PRINT_R_Buf,"SMS"))
//					GPRS_UART_GSM_sms_test();		//短信测试
//				else
//				if(strstr((char*)SPS_PRINT_R_Buf,"GPRS"))
//					GPRS_UART_GSM_gprs_test();	//GPRS测试
//				else
//				if(strstr((char*)SPS_PRINT_R_Buf,"LOC"))
//				  GPRS_UART_GSM_jz_test();		//基站测试
//				else				if(strstr((char*)SPS_PRINT_R_Buf,"VOICE"))
//					GPRS_UART_GSM_tts_test();		//语音测试
//				
//				IhuDebugPrint("VMUO: Please select: Chinese+Enter, then re-send\n"); 				    	 
//				IhuDebugPrint("VMUO: DIAL\t"); 				    	 
//				IhuDebugPrint("VMUO: SMS\t");				    	 
//				IhuDebugPrint("VMUO: GPRS\t");
//				IhuDebugPrint("VMUO: LOC\t");
//				IhuDebugPrint("VMUO: VOICE\r\n");
//			}
//		}			
//	} 	
//}

///*******************************************************************************
//* 函数名 : GPRS_UART_GSM_mtest
//* 描述   : GSM/GPRS主测试控制部分
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//uint8_t GPRS_UART_GSM_mtest()
//{
//	uint8_t *p1; 
//	IhuDebugPrint("VMUO: \nGSM/GPRS Test Program\n");  
//	GPRS_UART_clear_receive_buffer(); 
//	if(GPRS_UART_send_AT_command((uint8_t*)"AT+CGMI",(uint8_t*)"OK",5)==0)//查询制造商名称
//	{ 
//		IhuDebugPrint("VMUO: Maunfacture:");
//		p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff+2),"\r\n");
//		USART_PRINT_Send_Len((char*)SPS_GPRS_R_Buff+2,p1-SPS_GPRS_R_Buff);
//		GPRS_UART_clear_receive_buffer(); 		
//	} 
//	if(GPRS_UART_send_AT_command("AT+CGMM","OK",5)==0)//查询模块名字
//	{ 
//		IhuDebugPrint("VMUO: Module Type:");
//		p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff+2),"\r\n"); 
//		USART_PRINT_Send_Len((char*)SPS_GPRS_R_Buff+2,p1-SPS_GPRS_R_Buff);
//		GPRS_UART_clear_receive_buffer();
//	} 
//	if(GPRS_UART_send_AT_command("AT+CGSN","OK",5)==0)//查询产品序列号
//	{ 
//		IhuDebugPrint("VMUO: Product Serial ID:");
//		p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff+2),"\r\n"); 
//		USART_PRINT_Send_Len((char*)SPS_GPRS_R_Buff+2,p1-SPS_GPRS_R_Buff);
//		GPRS_UART_clear_receive_buffer();		
//	}
//	if(GPRS_UART_send_AT_command("AT+CNUM","+CNUM",2)==0)//查询本机号码
//	{ 
//		uint8_t *p2;
//		IhuDebugPrint("VMUO: Local Number:");
//		p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),"\""); 
//		p2=(uint8_t*)strstr((const char*)(p1+1),"\"");
//    p1=(uint8_t*)strstr((const char*)(p2+1),"\"");
//		USART_PRINT_Send_Len((char*)(p1+1),11);
//		GPRS_UART_clear_receive_buffer();		
//	}
//	IhuDebugPrint("\r\n");
//	return 1;
//}

///*******************************************************************************
//* 函数名 : GPRS_UART_GSM_gsm_test_info
//* 描述   : GSM/GPRS状态信息检测(信号质量,电池电量,日期时间)
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//uint8_t GPRS_UART_GSM_gsm_test_info(void)
//{
//	uint8_t *p1;
//	uint8_t *p2;
//	uint8_t res=0;
//	GPRS_UART_clear_receive_buffer();
//	if(GPRS_UART_send_AT_command("AT+CPIN?","OK",3))
//	{
//    IhuDebugPrint("VMUO: Please check whether SIM card inserted!\n");
//		res|=1<<0;	//查询SIM卡是否在位 
//	}
//	GPRS_UART_clear_receive_buffer(); 
//	
//	if(GPRS_UART_send_AT_command("AT+COPS?","OK",3)==0)		//查询运营商名字
//	{ 
//		p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),"\""); 
//		if(p1)//有有效数据
//		{
//			p2=(uint8_t*)strstr((const char*)(p1+1),"\"");				
//			IhuDebugPrint("VMUO: Opertor:");
//			USART_PRINT_Send_Len((char*)p1+1,p2-p1-1);
//			IhuDebugPrint("\r\n");
//		}
//	}else res|=1<<1;
//	GPRS_UART_clear_receive_buffer();
// 
//	if(GPRS_UART_send_AT_command("AT+CSQ","OK",3)==0)		//查询信号质量
//	{ 
//		p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),":");
//		if(p1)
//		{
//			p2=(uint8_t*)strstr((const char*)(p1+1),",");
//			IhuDebugPrint("VMUO: Singal quality:");
//			USART_PRINT_Send_Len((char*)p1+2,p2-p1-2);
//			IhuDebugPrint("\r\n");
//		}		
//	}else res|=1<<2;

//  if(GPRS_UART_send_AT_command("AT+DDET=1","OK",3)==0)
//	{
//		IhuDebugPrint("VMUO: Module support DTMF audio-decode, support key-down during peer conversation");
//    IhuDebugPrint("\r\n");	
//	}else
//	{
//    IhuDebugPrint("VMUO: Module not support DTMF audio-decode, Not support key-down during peer conversation");
//    IhuDebugPrint("\r\n");	
//	}
//	if(GPRS_UART_send_AT_command("AT+CTTS=?","OK",3)==0)
//	{
//		IhuDebugPrint("VMUO: Support TTS local voice, Support to convert TEXT into voice");
//    IhuDebugPrint("\r\n");	
//	}
//	else
//	{
//		IhuDebugPrint("VMUO: Module not support TTS local voice");
//    IhuDebugPrint("\r\n");			
//	}
//	if(GPRS_UART_send_AT_command("AT+CIPGSMLOC=?","OK",3)==0)
//	{
//		IhuDebugPrint("VMUO: Module support base station positioning, able to fetch position information");
//    IhuDebugPrint("\r\n");	
//	}
//	else
//	{
//		IhuDebugPrint("VMUO: Module not support base station positioning");
//    IhuDebugPrint("\r\n");			
//	}
//	GPRS_UART_clear_receive_buffer();
//	
//	return res;
//}
///*******************************************************************************
//* 函数名 : GPRS_UART_GSM_call_test
//* 描述   : 拨号测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//uint8_t GPRS_UART_GSM_call_test(void)
//{	
//	uint8_t temp[50];
//	uint16_t len=0;
//	uint16_t i=0;
//	uint8_t mode=0;
//	uint8_t flag=0;
//  uint8_t *p1=NULL;
//	IhuDebugPrint("VMUO: @@@@@@@@@@@DIAL TEST@@@@@@@@@@@\n");
//	IhuDebugPrint("VMUO: DIAL CALL: Input 'CALL xxxx + ENTER'\n");
//	IhuDebugPrint("VMUO: HAND-ON CALL: Input 'HAND-ON + ENTER'\n");
//  IhuDebugPrint("VMUO: HAND-OFF CALL: Input 'HAND-OFF + ENTER'\n");
//	IhuDebugPrint("VMUO: DTMF VOICE: Input 'Single Char + ENTER'\n");
//	IhuDebugPrint("VMUO: EXIT TEST: Input 'EXIT + ENTER'\n");
//	while(1)
//	{
//		if(SPS_PRINT_RX_STA&0x8000)
//		{
//			len=SPS_PRINT_RX_STA&0X3FFF;
//			SPS_PRINT_RX_STA=0;
//			if(strstr((const char*)SPS_PRINT_R_Buf,"EXIT"))return 0;
//			if(strstr((const char*)SPS_PRINT_R_Buf,"HAND-ON"))mode=1;
//			else
//			if(strstr((const char*)SPS_PRINT_R_Buf,"HAND-OFF"))mode=2;
//			else
//			if(strstr((const char*)SPS_PRINT_R_Buf,"CALL"))mode=3;
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
//				if(strstr((const char*)SPS_GPRS_R_Buff,"RING"))
//				{
//					GPRS_UART_clear_receive_buffer();
//					IhuDebugPrint("VMUO: CALL Coming\n");
//				}
//				break;
//			case 1:
//						 if(GPRS_UART_send_AT_command("ATA","OK",2)==0)//接听
//						 {
//							IhuDebugPrint("VMUO: Hand-on successful\n");
//						  mode=4; 
//						 }
//				break;
//			case 2:if(GPRS_UART_send_AT_command("ATH","OK",2)==0)//挂断
//							IhuDebugPrint("VMUO: Hand-off successful\n");		
//						 mode=0;
//				break;
//			case 3://拨号
//						IhuDebugPrint("VMUO:Call:");
//						USART_PRINT_Send_Len((char*)SPS_PRINT_R_Buf+4,len-4);
//						strcpy((char*)temp,"ATD");
//						for(i=3;i<(len-4+3);i++)
//						temp[i]=SPS_PRINT_R_Buf[i+1];
//						temp[i++]=';';
//						temp[i++]='\0';
//						if(GPRS_UART_send_AT_command(temp,"OK",8)==0)
//						{
//							IhuDebugPrint("VMUO: Call successful\n");
//							mode=4;
//						}
//						else
//						{
//							IhuDebugPrint("VMUO: Please re-dial\n");
//						  mode=0;
//						}
//			  break;
//			case 4://发送DTMF
//				if(flag)
//				{
//				flag=0;
//				strcpy((char*)temp,"AT+VTS=");
//				temp[7]=SPS_PRINT_R_Buf[0];
//				temp[8]='\0';
//				GPRS_UART_send_AT_command(temp,"OK",3);
//				}
//				if(strstr((const char*)SPS_GPRS_R_Buff,"DTMF:"))
//				{
//          osDelay(10);
//					p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),":");
//					IhuDebugPrint("VMUO: Peer press key-down: ");
//					USART_PRINT_Data(*(p1+1));
//					IhuDebugPrint("\r\n");	
//					GPRS_UART_clear_receive_buffer();
//				}
//				break;
//		}
//		
//	}
//}
///*******************************************************************************
//* 函数名 : GPRS_UART_GSM_sms_test
//* 描述   : 短信测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//uint8_t GPRS_UART_GSM_sms_test(void)
//{
//	uint16_t len=0;
//	uint8_t mode=0;
//	uint8_t temp[50];
//	uint8_t *p,*p1;
//	uint8_t loc=0;
//	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@@@@@ SMS TEST @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
//	IhuDebugPrint("VMUO: Set SMS Center: Set +8613800756500, set firstly and then make SMS!\n");
//	IhuDebugPrint("VMUO: Send SMS: Please enter telephone number + content with ENTER. Eg: 137xxxx+ This is a test!\n");
//  IhuDebugPrint("VMUO: Check SMS: Inquery xx, xx represent the storage position\n");
//	IhuDebugPrint("VMUO: Exit test: Enter 'EXIT + ENTER'\n");
//	GPRS_UART_send_AT_command("AT+CMGF=1","OK",2);//设置短信发送模式
//  GPRS_UART_send_AT_command("AT+CNMI=3,1,0,0,0","OK",2);//设置短信上报模式，上报位置
//  GPRS_UART_send_AT_command("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);		//所有操作在SIM卡中进行
//	while(1)
//	{
//		if(SPS_PRINT_RX_STA&0x8000)
//		{
//			len=SPS_PRINT_RX_STA&0X3FFF;
//			if(len>SPS_GPRS_REC_MAXLEN-2)len=SPS_GPRS_REC_MAXLEN-2;
//			SPS_PRINT_RX_STA=0;
//			if(strstr((const char*)SPS_PRINT_R_Buf,"EXIT"))return 0;
//			if(strstr((const char*)SPS_PRINT_R_Buf,"SET"))mode=3;//设置短信中心号
//			else
//			if(strstr((const char*)SPS_PRINT_R_Buf,"INQUERY"))mode=2;//查看短信
//			else 
//      if(strstr((const char*)SPS_PRINT_R_Buf,"+"))mode=1;//发送短信
//			else
//			  mode=0;

//		}
//		switch(mode)
//		{
//			case 0:
//        if(strstr((const char*)SPS_GPRS_R_Buff,"+CMTI:"))
//				{
//          IhuDebugPrint("VMUO: New SMS\n");
//					IhuDebugPrint("VMUO: SMS Position: ");
//					p=(uint8_t*)strstr((const char*)SPS_GPRS_R_Buff,",");
//					USART_PRINT_SendString((char*)p+1);
//					GPRS_UART_clear_receive_buffer();
//				}
//				break;
//			case 1:
//				IhuDebugPrint("VMUO: Send SMS\n");
//				SPS_PRINT_R_Buf[len]='\0';//添加结束符;
//				strcpy((char*)temp,(const char *)"AT+CMGS=\"+86");
//				loc=sizeof("AT+CMGS=\"+86");
//				p=(uint8_t*)strstr((char*)SPS_PRINT_R_Buf,(char*)"+");//查找发送内容
//				SPS_PRINT_R_Buf[(p-SPS_PRINT_R_Buf)]='\0';//添加结束符
//				strcpy((char*)(&temp[loc-1]),(const char*)SPS_PRINT_R_Buf);
//				loc=strlen((const char*)temp);
//			  temp[loc]='\"';
//				temp[loc+1]='\0';
//				GPRS_UART_send_AT_command(temp,">",2);//发送接收方号码
//        strcpy((char*)&temp,(const char*)(p+1));
//			  loc=strlen((const char*)temp);
//				temp[loc]='\32';
//				temp[loc+1]='\0';
//			  if(GPRS_UART_send_AT_command(temp,"OK",20)==0)//发送短信内容
//					IhuDebugPrint("VMUO: Send SMS successful\n");
//        else
//					IhuDebugPrint("VMUO: Send SMS failure\n");
//        mode=0;
//			break;
//			case 2:
//				IhuDebugPrint("VMUO: Inquery SMS\n");
//        SPS_PRINT_R_Buf[len]='\0';//添加结束符;
//				p=(uint8_t*)strstr((const char*)SPS_PRINT_R_Buf, ": ");
//				strcpy((char*)temp,(const char*)"AT+CMGR=");
//			  loc=strlen((const char*)temp);
//				strcpy((char*)(&temp[loc]),(const char*)(p+2));
//				loc=strlen((const char*)temp);
//			  if(GPRS_UART_send_AT_command(temp,"OK",5)==0)
//				{
//					p=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff+2),"\r\n");
//					p1=(uint8_t*)strstr((const char*)(p+2),"\r\n");
//					loc=p1-p;
//					IhuDebugPrint("VMUO: SMS Content: ");
//					USART_PRINT_Send_Len((char*)p+2,loc);
//				}
//        GPRS_UART_clear_receive_buffer();
//				mode=0;
//				break;
//			case 3:
//				IhuDebugPrint("VMUO: Set SMS Center Number\n");
//				SPS_PRINT_R_Buf[len]='\0';//添加结束符;
//				strcpy((char*)temp,(const char *)"AT+CSCA=");
//			  loc=sizeof("AT+CSCA=");
//				strcpy((char*)(&temp[loc-1]),(const char*)&SPS_PRINT_R_Buf[4]);
//				if(GPRS_UART_send_AT_command(temp,"OK",3)==0)
//					IhuDebugPrint("VMUO: Set SMS center successful\n");
//				else
//					IhuDebugPrint("VMUO: Set SMS center failure\n");
//				mode=0;
//				break;
//		}
//	}
//}

///*******************************************************************************
//* 函数名 : GPRS_UART_GSM_gprs_test
//* 描述   : GPRS测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   :  为了保持连接，每空闲隔10秒发送一次心跳
//*******************************************************************************/
//uint8_t GPRS_UART_GSM_gprs_test(void)
//{	
//  uint16_t len=0;
//	uint8_t mode=0;
//	uint8_t temp[200];
//	uint8_t flag=0;
//	uint8_t *p1,*p2;
//	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@GPRS TEST@@@@@@@@@@@@@@@@@@@@@\n");
//	IhuDebugPrint("VMUO: Set Parameter: Set\"MODE\",\"IP\", Port+ Enter\n Eg: Set\"TCP\",\"125.89.18.79\",12345\n");
//	IhuDebugPrint("VMUO: Send content: Send + Content + Enter \n, Eg: This is a test!\n");
//	IhuDebugPrint("VMUO: Exit Test: Exit + Enter\n");
//	
//	while(1)
//	{
//		if(SPS_PRINT_RX_STA&0x8000)
//		{
//			len=SPS_PRINT_RX_STA&0X3FFF;
//			if(len>SPS_GPRS_REC_MAXLEN-2)len=SPS_GPRS_REC_MAXLEN-2;
//			SPS_PRINT_RX_STA=0;
//			if(strstr((const char*)SPS_PRINT_R_Buf,"EXIT"))mode=4;//退出
//			else
//			if(strstr((const char*)SPS_PRINT_R_Buf,"SET"))mode=1;//设置配置
//			else
//			if(strstr((const char*)SPS_PRINT_R_Buf,"SEND"))mode=2;//发送内容
//			else 												mode=0;			
//		}
//		else
//		{
//			if(strstr((const char*)SPS_GPRS_R_Buff,"CONNECT OK")&&(flag==1))//连接成功
//			{
//					IhuDebugPrint("VMUO: Connect Successful\n");
//					GPRS_UART_TIMER_RECON_Count=1;
//					flag=2;
//					mode=3;
//					GPRS_UART_clear_receive_buffer();
//			}else
//			if((flag==1)&&((strstr((const char*)SPS_GPRS_R_Buff,"CLOSED"))&&(GPRS_UART_TIMER_RECON_Count>10)))//连接失败或超时
//			{
//					IhuDebugPrint("VMUO: Connect failure\n");
//					
//				  while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
//					USART_SendData(USART_PRINT ,GPRS_UART_TIMER_RECON_Count);//发送当前字符
//				
//					GPRS_UART_TIMER_RECON_Count=0;
//					flag=0;
//					mode=1; //重新连接
//					GPRS_UART_clear_receive_buffer();
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
//						if(strstr((const char*)SPS_GPRS_R_Buff,"+IPD"))//判断上位机是否有数据下发
//						{
//							IhuDebugPrint("VMUO: New content\n");
//							p1=(uint8_t*)strstr((const char*)SPS_GPRS_R_Buff,",");
//							p2=(uint8_t*)strstr((const char*)SPS_GPRS_R_Buff,":");
//							USART_PRINT_Send_Len((char*)(p2+1),GPRS_UART_change_str_Data((p1+1),(p2-p1-1)));
//							IhuDebugPrint("\r\n");
//							GPRS_UART_clear_receive_buffer();
//						}
//			break;
//			case 1: 
//				     if(strstr((const char*)SPS_PRINT_R_Buf,"TCP")||strstr((const char*)SPS_PRINT_R_Buf,"UDP"))
//						 {
//                strcpy((char*)&temp,(const char*)"AT+CIPSTART=");
//							  memcpy((char*)&temp[12],(const char*)(&SPS_PRINT_R_Buf[4]),len-4);
//							  temp[len+12]='\0';
//							  GPRS_UART_send_AT_command("AT+CIPCLOSE=1","CLOSE OK",2);	//关闭连接
//								GPRS_UART_send_AT_command("AT+CIPSHUT","SHUT OK",2);		//关闭移动场景
// 							  GPRS_UART_send_AT_command("AT+CGCLASS=\"B\"","OK",2);//设置GPRS移动台类别为B,支持包交换和数据交换 
//								GPRS_UART_send_AT_command("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",2);//设置PDP上下文,互联网接协议,接入点等信息
//								GPRS_UART_send_AT_command("AT+CGATT=1","OK",2);//附着GPRS业务
//								GPRS_UART_send_AT_command("AT+CIPCSGP=1,\"CMNET\"","OK",2);//设置为GPRS连接模式
//								GPRS_UART_send_AT_command("AT+CIPHEAD=1","OK",2);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
//                GPRS_UART_send_AT_command("AT+CIPMUX=0","OK",2);//设置单路连接
//                if(GPRS_UART_send_AT_command(temp,"OK",2 )==0)//发起连接
//								{
//                  IhuDebugPrint("VMUO: New connecting\n");
//									GPRS_UART_TIMER_RECON_Count=1;
//									flag=1;
//								}
//								else
//								{
//									IhuDebugPrint("VMUO: Enter error: Please re-enter\n");
//									GPRS_UART_TIMER_RECON_Count=0;
//									flag=0;
//								}
//						 }
//						 else
//						 {
//							  IhuDebugPrint("VMUO: Enter error: Please re-enter\n");
//						 }
//             mode=0;
//			break;
//			case 2:
//              if(flag==2)
//							{
//                IhuDebugPrint("VMUO: Begin to send..........\n");
//								if(GPRS_UART_send_AT_command("AT+CIPSEND",">",2)==0)
//								{
//										 SPS_PRINT_R_Buf[len]='\32';
//										 SPS_PRINT_R_Buf[len+1]='\0';
//										 if(GPRS_UART_send_AT_command(&SPS_PRINT_R_Buf[4],"SEND OK",8)==0)
//										 { 								
//													IhuDebugPrint("VMUO: Send successful\n");
//													GPRS_UART_TIMER_RECON_Count=1;
//										 }
//										 else
//											 IhuDebugPrint("VMUO: Send failure\n");
//										 
//								}else
//								{
//										 GPRS_UART_Data_byte_send(0X1B);//ESC,取消发送
//                     IhuDebugPrint("VMUO: Send failure\n");
//								}
//						  }else IhuDebugPrint("VMUO: Not connected\n");
//							mode=0;
//			break;
//			case 3:
//						if(GPRS_UART_send_AT_command("AT+CIPSEND",">",2)==0)
//						{
//							GPRS_UART_Data_byte_send(0x00);
//							GPRS_UART_Data_byte_send(0X1A);//CTRL+Z,结束数据发送,启动一次传输								
//							IhuDebugPrint("VMUO: Heart-beat successful\n");

//								 
//						}else
//						{
//								 GPRS_UART_Data_byte_send(0X1B);//ESC,取消发送
//							IhuDebugPrint("VMUO: Heart-beat failure\n");
//						}
//				mode=0;
//			break;
//			case 4:
//						GPRS_UART_send_AT_command("AT+CIPCLOSE=1","CLOSE OK",5);	//关闭连接
//						GPRS_UART_send_AT_command("AT+CIPSHUT","SHUT OK",5);		//关闭移动场景
//						return 1;
//		}      
//	}
//}

///*******************************************************************************
//* 函数名 : GPRS_UART_GSM_jz_test
//* 描述   : 基站定位测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//uint8_t GPRS_UART_GSM_jz_test(void)
//{
//  uint8_t *p1,*p2,*p3;
//	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@Base Station POSITON TEST@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
//	IhuDebugPrint("VMUO: EXIT TEST: EXIT + ENTER\n");
//	GPRS_UART_send_AT_command("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",2);
//	GPRS_UART_send_AT_command("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",2);
//  if(GPRS_UART_send_AT_command("AT+SAPBR=1,1","OK",5))//激活
//	{
//	
//		IhuDebugPrint("VMUO: Open failure, please conform module own this function or not\n");
//		return 1;		
//	}
//	if(GPRS_UART_send_AT_command("AT+SAPBR=2,1","OK",5))
//	{
//    IhuDebugPrint("VMUO: Fetch local IP error\n");
//		return 2;
//	}
//  GPRS_UART_TIMER_RECON_Count=1;
//	while(1)
//	{
//    if(SPS_PRINT_RX_STA&0x8000)
//		{
//			SPS_PRINT_RX_STA=0;
//			if(strstr((const char*)SPS_PRINT_R_Buf,"EXIT"))//退出
//			{
//				 GPRS_UART_send_AT_command("AT+SAPBR=0,1","OK",2);//关闭
//				 return 0;
//			}
//			else
//			IhuDebugPrint("VMUO: ENTER ERROR\n");
//		}
//    if(GPRS_UART_send_AT_command("AT+CIPGSMLOC=1,1","OK",10)==0)//获取经纬度和时间
//		{
//        GPRS_UART_TIMER_RECON_Count=1;
// 			  p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),",");
//				p3=(uint8_t*)strstr((const char*)p1,"\r\n");
//				if(p1)//有效数据
//				{	
//					p2=(uint8_t*)strtok((char*)(p1),",");
//          IhuDebugPrint("VMUO: Longitude:");USART_PRINT_SendString((char *)p2);USART_PRINT_Data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          IhuDebugPrint("VMUO: Latitude:");USART_PRINT_SendString((char *)p2);USART_PRINT_Data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          IhuDebugPrint("VMUO: Date:");USART_PRINT_SendString((char *)p2);USART_PRINT_Data('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//					*p3='\0';//插入结束符
//          IhuDebugPrint("VMUO: Time:");USART_PRINT_SendString((char *)p2);IhuDebugPrint("\r\n");
//				}
//		}
//		if(GPRS_UART_TIMER_RECON_Count>11)
//		{
//      GPRS_UART_TIMER_RECON_Count=0;
//			//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
//			IhuDebugPrint("VMUO: Open failure, please confirm this module own this function\n");
//			return 1;
//		}
//	}
//}

///*******************************************************************************
//* 函数名 : GSM_JZ_test
//* 描述   : 基站定位测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
///*
//uint8_t GPRS_UART_GSM_jz_test(void)
//{
//  uint8_t *p1,*p2,*p3;
//	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@Base Station POSITON TEST@@@@@@@@@@@@@@@@@@@@@@@@@\n");
//	IhuDebugPrint("VMUO: EXIT TEST：EXIT + ENTER\n");
//	GPRS_UART_send_AT_command("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",2);
//	GPRS_UART_send_AT_command("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",2);
//  if(GPRS_UART_send_AT_command("AT+SAPBR=1,1","OK",5))//激活
//	{
//		//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
//		IhuDebugPrint("VMUO: Open failure, please conform module own this function or not\n");
//		return 1;		
//	}
//	if(GPRS_UART_send_AT_command("AT+SAPBR=2,1","OK",5))
//	{
//    IhuDebugPrint("VMUO: Fetch local IP error\n");
//		return 2;
//	}
//  GPRS_UART_TIMER_RECON_Count=1;
//	while(1)
//	{
//    if(SPS_PRINT_RX_STA&0x8000)
//		{
//			SPS_PRINT_RX_STA=0;
//			if(strstr(SPS_PRINT_R_Buf,"EXIT"))//退出
//			{
//				 GPRS_UART_send_AT_command("AT+SAPBR=0,1","OK",2);//关闭
//				 return 0;
//			}
//			else
//			IhuDebugPrint("VMUO: Enter Error\n");
//		}
//    if(GPRS_UART_send_AT_command("AT+CIPGSMLOC=1,1","OK",10)==0)//获取经纬度和时间
//		{
//        GPRS_UART_TIMER_RECON_Count=1;
// 			  p1=(uint8_t*)strstr((const char*)(SPS_GPRS_R_Buff),",");
//				p3=(uint8_t*)strstr(p1,"\r\n");
//				if(p1)//有效数据
//				{	
//					p2=(uint8_t*)strtok((char*)(p1),",");
//          IhuDebugPrint("VMUO: Logiture:");USART_PRINT_SendString(p2);UART1_SendData('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          IhuDebugPrint("VMUO: Latitude:");USART_PRINT_SendString(p2);UART1_SendData('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//          IhuDebugPrint("VMUO: Date:");USART_PRINT_SendString(p2);UART1_SendData('\t');
//					
//					p2=(uint8_t*)strtok(NULL,",");
//					*p3='\0';//插入结束符
//          IhuDebugPrint("VMUO: Time:");USART_PRINT_SendString(p2);IhuDebugPrint("\r\n");
//				}
//		}
//		if(GPRS_UART_TIMER_RECON_Count>11)
//		{
//      GPRS_UART_TIMER_RECON_Count=0;
//			//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
//			IhuDebugPrint("VMUO: Open failure, please confirm this module own this function\n");
//			return 1;
//		}
//	}
//}
//*/

///*******************************************************************************
//* 函数名 : GPRS_UART_GSM_tts_test
//* 描述   : TTS文本语音测试代码
//* 输入   : 
//* 输出   : 
//* 返回   : 
//* 注意   : 
//*******************************************************************************/
//uint8_t GPRS_UART_GSM_tts_test(void)
//{
//  uint16_t len=0;
//  uint8_t temp_src[]="Hello, this is a test from BXXH!";
//	uint8_t temp[GPRS_UART_TTS_MAX_len+15];
//	uint8_t loc=0;
//	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@TTS TEXT VOICE TEST@@@@@@@@@@@@@@@@@@@@@@@\n");
//	IhuDebugPrint("VMUO: EXIT TEST: EXIT + ENTER\n");
//	IhuDebugPrint("VMUO: VOICE Brocasting: Please enter content + Enter\n");
//  IhuDebugPrint("VMUO: Attention, support max 100 byte\n");
//	if(GPRS_UART_send_AT_command("AT+CTTS=?","OK",2))
//	{
//		IhuDebugPrint("VMUO: Error: Please inquery this module own this function or not\n");
//		 return 1;
//	}
//	strcpy((char*)&temp,(const char*)"AT+CTTS=2,\"");
//	loc=sizeof("AT+CTTS=2,\"");
//  len=strlen((const char*)temp_src);
//	strcpy((char*)&temp[loc-1],(const char*)temp_src);
//	temp[loc+len-1]='\"';
//	temp[loc+len]='\0';
//  GPRS_UART_send_AT_command(temp,"OK",3);
//	while(1)
//	{
//		if(SPS_PRINT_RX_STA&0x8000)
//		{
//      len=SPS_PRINT_RX_STA&0X3FFF;
//			if(len>(GPRS_UART_TTS_MAX_len/2))len=(GPRS_UART_TTS_MAX_len/2);
//      SPS_PRINT_RX_STA=0;
//			if((len==4)&&(strstr((const char*)SPS_PRINT_R_Buf,"EXIT")))//退出	
//			return 0;
//			GPRS_UART_SendString("AT+CTTS=2,\"");
//			SPS_PRINT_R_Buf[len]='\"';
//			SPS_PRINT_R_Buf[len+1]='\0';
//			if(GPRS_UART_send_AT_command(SPS_PRINT_R_Buf,"OK",2))
//				IhuDebugPrint("VMUO: Wait for previous frame accomplish\n");
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
void GPRS_UART_connect_server(void)
{
	GPRS_UART_clear_receive_buffer();
	GPRS_UART_SendString("AT+CIPCLOSE=1");	//关闭连接
  osDelay(100);
	GPRS_UART_send_AT_command("AT+CIPSHUT","SHUT OK", 500);		//关闭移动场景
	GPRS_UART_send_AT_command("AT+CGCLASS=\"B\"","OK", 500);//设置GPRS移动台类别为B,支持包交换和数据交换 
	GPRS_UART_send_AT_command("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK", 500);//设置PDP上下文,互联网接协议,接入点等信息
	GPRS_UART_send_AT_command("AT+CGATT=1","OK", 500);//附着GPRS业务
	GPRS_UART_send_AT_command("AT+CIPCSGP=1,\"CMNET\"","OK", 500);//设置为GPRS连接模式
  GPRS_UART_send_AT_command("AT+CIPMUX=0","OK", 500);//设置为单路连接
	GPRS_UART_send_AT_command("AT+CIPHEAD=1","OK", 500);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	GPRS_UART_send_AT_command("AT+CIPMODE=1","OK", 500);//打开透传功能
	GPRS_UART_send_AT_command("AT+CIPCCFG=4,5,200,1","OK", 500);//配置透传模式：单包重发次数:2,间隔1S发送一次,每次发送200的字节
  GPRS_UART_send_AT_command((uint8_t *)GPRS_UART_string, "OK", 2000);//建立连接
  
  osDelay(100);                                //等待串口数据接收完毕
  GPRS_UART_clear_receive_buffer();                                    //清串口接收缓存为透传模式准备
}

/*******************************************************************************
* 函数名 : Rec_Server_Data
* 描述   : 接收服务器数据函数,并原样返回
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void GPRS_UART_Rec_Server_Data(void)
{
//p2指向发送数据，p1指向接收数据，这里可以使用SPS_GPRS_R_Count来判断
//	if(p2!=p1)   		//说明还有数据未发出
//	{	
//		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET);
//		BSP_STM32_GPRS_UART_SendData(USART_GPRS, *p2);
//		p2++;
//    if(p2>&SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN])
//			p2=SPS_GPRS_R_Buff;
//	}
}

//一个完整的数据流程，且不断回送数据
void GPRS_UART_data_connection_and_receive_process(void)
{
	IhuDebugPrint("VMUO: GPRS Module GPRS register network!\n");
	GPRS_UART_SendString("+++");//退出透传模式，避免模块还处于透传模式中
  osDelay(500);  //500ms
	GPRS_UART_Wait_CREG();   //等待模块注册成功
	IhuDebugPrint("VMUO: GPRS Register Success!\n");
	IhuDebugPrint("VMUO: GPRS Module connect server!\n");
	GPRS_UART_Set_ATE0();    //取消回显
	GPRS_UART_connect_server(); //设置GPRS参数
	IhuDebugPrint("VMUO: Connect success!\n");

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
void USART_PRINT_IRQHandler(void)                	
{
	uint8_t res;

	//res=USART_ReceiveData(USART_PRINT);
	Time_UART_PRINT=0;
	if((SPS_PRINT_RX_STA&0x8000)==0)//接收未完成
	{
		if(SPS_PRINT_RX_STA&0x4000)//接收到了0x0d
		{
			if(res!=0x0a)SPS_PRINT_RX_STA=0;//接收错误,重新开始
			else SPS_PRINT_RX_STA|=0x8000;	//接收完成了 
		}else //还没收到0X0D
		{	
			if(res==0x0d)SPS_PRINT_RX_STA|=0x4000;
			else
			{
				SPS_PRINT_R_Buf[SPS_PRINT_RX_STA&0X3FFF]=res;
				SPS_PRINT_RX_STA++;
				if(SPS_PRINT_RX_STA>(SPS_PRINT_REC_MAXLEN-1))SPS_PRINT_RX_STA=0;//接收数据错误,重新开始接收	  
			}		 
		}
	}  
} 

/*******************************************************************************
* 函数名  : TIM2_IRQHandler
* 描述    : 定时器2中断断服务函数
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void TIM_USART_GPRS_IRQHandler(void)   //TIM_USART_GPRS_ID中断
{
	//if (TIM_GetITStatus(TIM_USART_GPRS_ID, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		//TIM_ClearITPendingBit(TIM_USART_GPRS_ID, TIM_IT_Update  );  //清除TIMx更新中断标志	
		
		//重连计数器
		//GPRS_UART_TIMER_RECON_Count==1时启动计数，溢出自动停止
		if(GPRS_UART_TIMER_RECON_Count>=1)GPRS_UART_TIMER_RECON_Count++;
		
		//延时计数器
		if(GPRS_UART_TIMER_START_Flag) GPRS_UART_TIMER_DELAY_Count++;		

		//延迟到达后，不再计数，重新启动
		if(GPRS_UART_TIMER_DELAY_Count > (GPRS_UART_TIMER_WAIT_Duration))
		{
			GPRS_UART_TIMER_START_Flag = 0;
			GPRS_UART_TIMER_DELAY_Count = 0;
		}
		
		//接收IRQ的计数器，由GPRS_UART_IRQ触发
		SPS_GPRS_TIMER_TRIGGER_Count++;
		
		//正在接收状态中
		if(SPS_PRINT_RX_STA&(~0x8000))
		{
			Time_UART_PRINT++;
			if(Time_UART_PRINT>=200)SPS_PRINT_RX_STA=0;//接收超时，复位接收
		}
	}	
}

/*******************************************************************************
* 函数名 : GPRS_UART_clear_receive_buffer
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void GPRS_UART_clear_receive_buffer(void)
{
	uint16_t k;
	for(k=0;k<SPS_GPRS_REC_MAXLEN;k++)      //将缓存内容清零
	{
		SPS_GPRS_R_Buff[k] = 0x00;
	}
  SPS_GPRS_R_Count = 0;               //接收字符串的起始存储位置
}


/*******************************************************************************
* 函数名 : GPRS_UART_send_AT_command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 0:正常  1:错误
* 注意   : 
* uint8_t GPRS_UART_send_AT_command(uint8_t *cmd, uint8_t *ack, uint8_t wait_time)  
* 这里的发送，只有成功返回ACK对应的回复时，才算成功
*******************************************************************************/
uint8_t GPRS_UART_send_AT_command(uint8_t *cmd, uint8_t *ack, uint16_t wait_time) //in Second
{
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	//uint32_t tickTotal = wait_time * 1000 / SPS_UART_RX_MAX_DELAY_DURATION;

	//清理接收缓冲区
	GPRS_UART_clear_receive_buffer();
	BSP_STM32_SPS_GPRS_SendData((uint8_t *)cmd, strlen((char*)cmd));
	GPRS_UART_SendLR();	

	//接收，注意时钟刻度
	BSP_STM32_SPS_GPRS_RcvData_timeout((uint8_t*)SPS_GPRS_R_Buff, strlen((char *)ack), wait_time * 1000);
	if(strstr((const char*)SPS_GPRS_R_Buff, (char*)ack)==NULL) 
		return FAILURE;
	else 
		return SUCCESS;
		
	//等待固定时间
	//osDelay(wait_time); 这里的周期就是以绝对ms为单位的
	
//	while((tickTotal>0) && (res==1))
//	{
//		tickTotal--;
//		BSP_STM32_SPS_GPRS_RcvData((uint8_t*)SPS_GPRS_R_Buff, SPS_GPRS_REC_MAXLEN);
//		if(strstr((const char*)SPS_GPRS_R_Buff, (char*)ack)==NULL)
//			 res=1;
//		else
//		{
//			 res=0;
//		}
//	}
//	if (res == 1) return FAILURE;
//	else return SUCCESS;	
}

/*******************************************************************************
* 函数名  : USART_GPRS_SendString
* 描述    : USART_GPRS发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void GPRS_UART_SendString(char* s)
{
	while(*s != '\0')//检测字符串结束符
	{
		BSP_STM32_SPS_GPRS_SendData((uint8_t *)s++, 1);
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
void GPRS_UART_Wait_CREG(void)
{
	uint8_t i;
	uint8_t k;
	i = 0;
  while(i == 0)        			
	{
		GPRS_UART_clear_receive_buffer();        
		GPRS_UART_SendString("AT+CREG?");   //查询等待模块注册成功
		GPRS_UART_SendLR();
		osDelay(500);  //等待500ms
		for(k=0;k<SPS_GPRS_REC_MAXLEN;k++)      			
		{
			if(SPS_GPRS_R_Buff[k] == ':')
			{
				if((SPS_GPRS_R_Buff[k+4] == '1')||(SPS_GPRS_R_Buff[k+4] == '5'))   //说明注册成功
				{
					i = 1;
					IhuDebugPrint("\r");
					break;
				}
			}
		}
		IhuDebugPrint("VMUO: Under Registering.....");  //注册中。。。
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
void GPRS_UART_Set_ATE0(void)
{
	GPRS_UART_send_AT_command("ATE0", "OK", 500);								//取消回显		
}



///*******************************************************************************
//* º¯ÊýÃû  : USART_PRINT_Send_Len
//* ÃèÊö    : USART_PRINT·¢ËÍÒ»¶¨³¤¶ÈµÄ×Ö·û
//* ÊäÈë    : *s×Ö·û´®Ö¸Õë£¬len£º×Ö·û³¤¶È
//* Êä³ö    : ÎÞ
//* ·µ»Ø    : ÎÞ 
//* ËµÃ÷    : ÎÞ
//*******************************************************************************/
//void USART_PRINT_Send_Len(char *s,uint8_t len)
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
//* º¯ÊýÃû  : USART_PRINT_Data
//* ÃèÊö    : USART_PRINT·¢ËÍÒ»¸ö×Ö½Ú
//* ÊäÈë    : byte Ò»¸ö×Ö½Ú
//* Êä³ö    : ÎÞ
//* ·µ»Ø    : ÎÞ 
//* ËµÃ÷    : ÎÞ
//*******************************************************************************/
//void USART_PRINT_Data(char byte)
//{
//		while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
//		USART_SendData(USART_PRINT ,byte);//Send current char
//}

//void USART_PRINT_SendString(char* s)
//{
//	while(*s)//check end char
//	{
//		while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
//		USART_SendData(USART_PRINT ,*s++);//Send current char
//	}
//}

//void GPRS_UART_Data_byte_send(char byte)
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
* 函数名 : GPRS_UART_change_str_Data
* 描述   : 字符串转整型
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
uint8_t GPRS_UART_change_str_Data(uint8_t *p,uint8_t len)
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
* 函数名 : GPRS_UART_Swap
* 描述   : 交换
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void GPRS_UART_Swap(char *ch1, char *ch2)
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
void GPRS_UART_change_Data_str(int n, char str[])
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
		GPRS_UART_Swap(str+i, str+len-i-1);
	str[len] = '\0';
}

/*******************************************************************************
* 函数名 : GPRS_UART_change_hex_str
* 描述   : 十六进制转换成字符串 如0xAC->'A''C';
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void GPRS_UART_change_hex_str(uint8_t dest[],uint8_t src[],uint8_t len)
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
uint8_t GPRS_UART_Find_char(char *a)
{ 
  if(strstr((char *)SPS_GPRS_R_Buff, a)!=NULL)
	    return 1;
	else
			return 0;
}

