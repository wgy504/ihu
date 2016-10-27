/**
 ****************************************************************************************
 *
 * @file gprs.c
 *
 * @brief GPRS module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */
 
#include "gprs.h"

extern vu8 SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN];	//串口GPRS数据接收缓冲区 
extern vu8 SPS_GPRS_R_State;					//串口GPRS接收状态
extern vu16 SPS_GPRS_R_Count;					//当前接收数据的字节数
const char *SPS_GPRS_string = "AT+CIPSTART=\"TCP\",\"14.125.48.205\",9015";//IP登录服务器,需要根据自己的IP和端口做修改


/*******************************************************************************
* 函数名 : main 
* 描述   : 主函数
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 串口2负责与GSM模块通信，串口1用于串口调试，可以避免在下载程序时数据
					 还发送到模块
*******************************************************************************/
int GPRS_control_gsm_test(void)
{
	//SysTick
	//SysTick_Init_Config();
	//LED
	//GPIO_Config();

	//Print
	//USART1_Init_Config(115200);
	//SPS_GRPS
	//USART_GPRS_Init_Config(115200);
	Timer2_Init_Config();
	IhuDebugPrint("VMUO: GSM Module SMS test procedure!\n");//打印信息
	IhuDebugPrint("VMUO: GSM Module Register network!\n");
  GSM_test(); //GSM测试程序
	while(1)
	{
	}
}
/*******************************************************************************
* 函数名  : USART1_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 1)、只启动了USART1中断接收，未启动USART1中断发送。
*           2)、接收到0x0d 0x0a(回车、"\r\n")代表帧数据接收完成
*******************************************************************************/
void USART1_IRQHandler(void)                	
{
		u8 res;

		res=USART_ReceiveData(USART1);
		Time_UART1=0;
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					Uart1_Buf[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(Buf1_Max-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  

} 
/*******************************************************************************
* 函数名  : USART_GPRS_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 
*******************************************************************************/
void USART_GPRS_IRQHandler(void)                	
{
			u8 Res=0;
      Time_UART2=0;
			Res= USART_ReceiveData(USART_GPRS);		//将接收到的字符串存到缓存中
			Uart2_Buf[First_Int]= Res; 
			First_Int++;                			//缓存指针向后移动
			if(First_Int > Buf2_Max)       		//如果缓存满,将缓存指针指向缓存的首地址
			{
				First_Int = 0;
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
void TIM2_IRQHandler(void)   //TIM3中断
{
	static u8 flag =1;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 

		if(flag)
		{
			//LED4_ON(); 
			flag=0;
		}
		else
		{
			//LED4_OFF(); 
			flag=1;
		}
		if(Time_miao>=1)Time_miao++;//Time_miao==1时启动计数，溢出自动停止
		if(Timer0_start)
		Times++;
		if(Times > (shijian))
		{
			Timer0_start = 0;
			Times = 0;
		}
		Time_UART2++;
		if(USART_RX_STA&(~0x8000))//正在接收状态中
		{
			Time_UART1++;
			if(Time_UART1>=200)USART_RX_STA=0;//接收超时，复位接收
		}
	}	
}

/*******************************************************************************
* 函数名 : CLR_Buf2
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void CLR_Buf2(void)
{
	u16 k;
	for(k=0;k<Buf2_Max;k++)      //将缓存内容清零
	{
		Uart2_Buf[k] = 0x00;
	}
    First_Int = 0;              //接收字符串的起始存储位置
}
/*******************************************************************************
* 函数名 : GSM_send_cmd
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 0:正常  1:错误
* 注意   : 
*******************************************************************************/
u8 GSM_send_cmd(u8 *cmd,u8 *ack,u8 wait_time)         
{
	u8 res=1;
	//u8 *c;
	//c = cmd;										//保存字符串地址到c
	CLR_Buf2(); 
	for (; *cmd!='\0';cmd++)
	{
		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET);
	  USART_SendData(USART_GPRS,*cmd);
	}
	SPS_GPRS_SendLR();	
	if(wait_time==0)return res;
	Times = 0;
	shijian = wait_time;
	Timer0_start = 1;
	while(Timer0_start&res)                    
	{
		if(strstr((const char*)Uart2_Buf,(char*)ack)==NULL)
			 res=1;
		else
		{
			 res=0;
		}

	}
	return res;
}
/*******************************************************************************
* 函数名 : GSM_test
* 描述   : GSM主测试程序
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void GSM_test(void)
{
	u8 sim_ready=0;
	while(GSM_send_cmd((u8*)"AT",(u8*)"OK",5))//查询是否应到AT指令
	{
		IhuDebugPrint("VMUO: Not dected module!\n");
		ihu_usleep(800);
		IhuDebugPrint("VMUO: Trying to reconnecting!\n");
		ihu_usleep(400);  
	} 	 
	GSM_send_cmd((u8*)"ATE0",(u8*)"OK",200);//不回显
	GSM_mtest();
	if(GSM_gsminfo()==0)
	{
		sim_ready=1;
		IhuDebugPrint("VMUO: Please select: Chinese+Enter, then re-send\n"); 				    	 
		IhuDebugPrint("VMUO: DIAL\t"); 				    	 
		IhuDebugPrint("VMUO: SMS\t");				    	 
		IhuDebugPrint("VMUO: GPRS\t");
		IhuDebugPrint("VMUO: LOC\t");
		IhuDebugPrint("VMUO: VOICE\r\n");
	}
	while(1)
	{
		if(sim_ready)
		{
			if(USART_RX_STA&0x8000)
			{
				USART_RX_STA=0;
				if(strstr((char*)Uart1_Buf,"DIAL"))
					GSM_call_test();	//电话测试
				else
				if(strstr((char*)Uart1_Buf,"SMS"))
					GSM_sms_test();		//短信测试
				else
				if(strstr((char*)Uart1_Buf,"GPRS"))
					GSM_gprs_test();	//GPRS测试
				else
				if(strstr((char*)Uart1_Buf,"LOC"))
				  GSM_jz_test();		//基站测试
				else				if(strstr((char*)Uart1_Buf,"VOICE"))
					GSM_tts_test();		//语音测试
				
				IhuDebugPrint("VMUO: Please select: Chinese+Enter, then re-send\n"); 				    	 
				IhuDebugPrint("VMUO: DIAL\t"); 				    	 
				IhuDebugPrint("VMUO: SMS\t");				    	 
				IhuDebugPrint("VMUO: GPRS\t");
				IhuDebugPrint("VMUO: LOC\t");
				IhuDebugPrint("VMUO: VOICE\r\n");
			}
			
		}			
	} 	
}
/*******************************************************************************
* 函数名 : GSM_mtest
* 描述   : GSM/GPRS主测试控制部分
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void GSM_mtest()
{
	u8 *p1; 
	IhuDebugPrint("VMUO: \nGSM/GPRS Test Program\n");  
	CLR_Buf2(); 
	if(GSM_send_cmd((u8*)"AT+CGMI",(u8*)"OK",5)==0)//查询制造商名称
	{ 
		IhuDebugPrint("VMUO: Maunfacture:");
		p1=(u8*)strstr((const char*)(Uart2_Buf+2),"\r\n");
		USART_PRINT_Send_Len((char*)Uart2_Buf+2,p1-Uart2_Buf);
		CLR_Buf2(); 		
	} 
	if(GSM_send_cmd("AT+CGMM","OK",5)==0)//查询模块名字
	{ 
		IhuDebugPrint("VMUO: Module Type:");
		p1=(u8*)strstr((const char*)(Uart2_Buf+2),"\r\n"); 
		USART_PRINT_Send_Len((char*)Uart2_Buf+2,p1-Uart2_Buf);
		CLR_Buf2();
	} 
	if(GSM_send_cmd("AT+CGSN","OK",5)==0)//查询产品序列号
	{ 
		IhuDebugPrint("VMUO: Product Serial ID:");
		p1=(u8*)strstr((const char*)(Uart2_Buf+2),"\r\n"); 
		USART_PRINT_Send_Len((char*)Uart2_Buf+2,p1-Uart2_Buf);
		CLR_Buf2();		
	}
	if(GSM_send_cmd("AT+CNUM","+CNUM",2)==0)//查询本机号码
	{ 
		u8 *p2;
		IhuDebugPrint("VMUO: Local Number:");
		p1=(u8*)strstr((const char*)(Uart2_Buf),"\""); 
		p2=(u8*)strstr((const char*)(p1+1),"\"");
    p1=(u8*)strstr((const char*)(p2+1),"\"");
		USART_PRINT_Send_Len((char*)(p1+1),11);
		CLR_Buf2();		
	}
	IhuDebugPrint("\r\n");
}

/*******************************************************************************
* 函数名 : GSM_gsminfo_gsminfo
* 描述   : GSM/GPRS状态信息检测(信号质量,电池电量,日期时间)
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 GSM_gsminfo()
{
	u8 *p1;
	u8 *p2;
	u8 res=0;
	CLR_Buf2();
	if(GSM_send_cmd("AT+CPIN?","OK",3))
	{
    IhuDebugPrint("VMUO: Please check whether SIM card inserted!\n");
		res|=1<<0;	//查询SIM卡是否在位 
	}
	CLR_Buf2(); 
	
	if(GSM_send_cmd("AT+COPS?","OK",3)==0)		//查询运营商名字
	{ 
		p1=(u8*)strstr((const char*)(Uart2_Buf),"\""); 
		if(p1)//有有效数据
		{
			p2=(u8*)strstr((const char*)(p1+1),"\"");				
			IhuDebugPrint("VMUO: Opertor:");
			USART_PRINT_Send_Len((char*)p1+1,p2-p1-1);
			IhuDebugPrint("\r\n");
		}
	}else res|=1<<1;
	CLR_Buf2();
 
	if(GSM_send_cmd("AT+CSQ","OK",3)==0)		//查询信号质量
	{ 
		p1=(u8*)strstr((const char*)(Uart2_Buf),":");
		if(p1)
		{
			p2=(u8*)strstr((const char*)(p1+1),",");
			IhuDebugPrint("VMUO: Singal quality:");
			USART_PRINT_Send_Len((char*)p1+2,p2-p1-2);
			IhuDebugPrint("\r\n");
		}		
	}else res|=1<<2;

  if(GSM_send_cmd("AT+DDET=1","OK",3)==0)
	{
		IhuDebugPrint("VMUO: Module support DTMF audio-decode, support key-down during peer conversation");
    IhuDebugPrint("\r\n");	
	}else
	{
    IhuDebugPrint("VMUO: Module not support DTMF audio-decode, Not support key-down during peer conversation");
    IhuDebugPrint("\r\n");	
	}
	if(GSM_send_cmd("AT+CTTS=?","OK",3)==0)
	{
		IhuDebugPrint("VMUO: Support TTS local voice, Support to convert TEXT into voice");
    IhuDebugPrint("\r\n");	
	}
	else
	{
		IhuDebugPrint("VMUO: Module not support TTS local voice");
    IhuDebugPrint("\r\n");			
	}
	if(GSM_send_cmd("AT+CIPGSMLOC=?","OK",3)==0)
	{
		IhuDebugPrint("VMUO: Module support base station positioning, able to fetch position information");
    IhuDebugPrint("\r\n");	
	}
	else
	{
		IhuDebugPrint("VMUO: Module not support base station positioning");
    IhuDebugPrint("\r\n");			
	}
	CLR_Buf2();
	
	return res;
}
/*******************************************************************************
* 函数名 : GSM_call_test
* 描述   : 拨号测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 GSM_call_test()
{	
	u8 temp[50];
	u16 len=0;
	u16 i=0;
	u8 mode=0;
	u8 flag=0;
  u8 *p1=NULL;
	IhuDebugPrint("VMUO: @@@@@@@@@@@DIAL TEST@@@@@@@@@@@\n");
	IhuDebugPrint("VMUO: DIAL CALL: Input 'CALL xxxx + ENTER'\n");
	IhuDebugPrint("VMUO: HAND-ON CALL: Input 'HAND-ON + ENTER'\n");
  IhuDebugPrint("VMUO: HAND-OFF CALL: Input 'HAND-OFF + ENTER'\n");
	IhuDebugPrint("VMUO: DTMF VOICE: Input 'Single Char + ENTER'\n");
	IhuDebugPrint("VMUO: EXIT TEST: Input 'EXIT + ENTER'\n");
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0X3FFF;
			USART_RX_STA=0;
			if(strstr((const char*)Uart1_Buf,"EXIT"))return 0;
			if(strstr((const char*)Uart1_Buf,"HAND-ON"))mode=1;
			else
			if(strstr((const char*)Uart1_Buf,"HAND-OFF"))mode=2;
			else
			if(strstr((const char*)Uart1_Buf,"CALL"))mode=3;
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
				if(strstr((const char*)Uart2_Buf,"RING"))
				{
					CLR_Buf2();
					IhuDebugPrint("VMUO: CALL Coming\n");
				}
				break;
			case 1:
						 if(GSM_send_cmd("ATA","OK",2)==0)//接听
						 {
							IhuDebugPrint("VMUO: Hand-on successful\n");
						  mode=4; 
						 }
				break;
			case 2:if(GSM_send_cmd("ATH","OK",2)==0)//挂断
							IhuDebugPrint("VMUO: Hand-off successful\n");		
						 mode=0;
				break;
			case 3://拨号
						IhuDebugPrint("VMUO:Call:");
						USART_PRINT_Send_Len((char*)Uart1_Buf+4,len-4);
						strcpy((char*)temp,"ATD");
						for(i=3;i<(len-4+3);i++)
						temp[i]=Uart1_Buf[i+1];
						temp[i++]=';';
						temp[i++]='\0';
						if(GSM_send_cmd(temp,"OK",8)==0)
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
				temp[7]=Uart1_Buf[0];
				temp[8]='\0';
				GSM_send_cmd(temp,"OK",3);
				}
				if(strstr((const char*)Uart2_Buf,"DTMF:"))
				{
          ihu_usleep(10);
					p1=(u8*)strstr((const char*)(Uart2_Buf),":");
					IhuDebugPrint("VMUO: Peer press key-down: ");
					USART_PRINT_Data(*(p1+1));
					IhuDebugPrint("\r\n");	
					CLR_Buf2();
				}
				break;
		}
		
	}
}
/*******************************************************************************
* 函数名 : GSM_sms_test
* 描述   : 短信测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 GSM_sms_test()
{
	u16 len=0;
	u8 mode=0;
	u8 temp[50];
	u8 *p,*p1;
	u8 loc=0;
	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@@@@@ SMS TEST @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	IhuDebugPrint("VMUO: Set SMS Center: Set +8613800756500, set firstly and then make SMS!\n");
	IhuDebugPrint("VMUO: Send SMS: Please enter telephone number + content with ENTER. Eg: 137xxxx+ This is a test!\n");
  IhuDebugPrint("VMUO: Check SMS: Inquery xx, xx represent the storage position\n");
	IhuDebugPrint("VMUO: Exit test: Enter 'EXIT + ENTER'\n");
	GSM_send_cmd("AT+CMGF=1","OK",2);//设置短信发送模式
  GSM_send_cmd("AT+CNMI=3,1,0,0,0","OK",2);//设置短信上报模式，上报位置
  GSM_send_cmd("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);		//所有操作在SIM卡中进行
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0X3FFF;
			if(len>Buf2_Max-2)len=Buf2_Max-2;
			USART_RX_STA=0;
			if(strstr((const char*)Uart1_Buf,"EXIT"))return 0;
			if(strstr((const char*)Uart1_Buf,"SET"))mode=3;//设置短信中心号
			else
			if(strstr((const char*)Uart1_Buf,"INQUERY"))mode=2;//查看短信
			else 
      if(strstr((const char*)Uart1_Buf,"+"))mode=1;//发送短信
			else
			  mode=0;

		}
		switch(mode)
		{
			case 0:
        if(strstr((const char*)Uart2_Buf,"+CMTI:"))
				{
          IhuDebugPrint("VMUO: New SMS\n");
					IhuDebugPrint("VMUO: SMS Position: ");
					p=(u8*)strstr((const char*)Uart2_Buf,",");
					USART_PRINT_SendString((char*)p+1);
					CLR_Buf2();
				}
				break;
			case 1:
				IhuDebugPrint("VMUO: Send SMS\n");
				Uart1_Buf[len]='\0';//添加结束符;
				strcpy((char*)temp,(const char *)"AT+CMGS=\"+86");
				loc=sizeof("AT+CMGS=\"+86");
				p=(u8*)strstr((char*)Uart1_Buf,(char*)"+");//查找发送内容
				Uart1_Buf[(p-Uart1_Buf)]='\0';//添加结束符
				strcpy((char*)(&temp[loc-1]),(const char*)Uart1_Buf);
				loc=strlen((const char*)temp);
			  temp[loc]='\"';
				temp[loc+1]='\0';
				GSM_send_cmd(temp,">",2);//发送接收方号码
        strcpy((char*)&temp,(const char*)(p+1));
			  loc=strlen((const char*)temp);
				temp[loc]='\32';
				temp[loc+1]='\0';
			  if(GSM_send_cmd(temp,"OK",20)==0)//发送短信内容
					IhuDebugPrint("VMUO: Send SMS successful\n");
        else
					IhuDebugPrint("VMUO: Send SMS failure\n");
        mode=0;
			break;
			case 2:
				IhuDebugPrint("VMUO: Inquery SMS\n");
        Uart1_Buf[len]='\0';//添加结束符;
				p=(u8*)strstr((const char*)Uart1_Buf, ": ");
				strcpy((char*)temp,(const char*)"AT+CMGR=");
			  loc=strlen((const char*)temp);
				strcpy((char*)(&temp[loc]),(const char*)(p+2));
				loc=strlen((const char*)temp);
			  if(GSM_send_cmd(temp,"OK",5)==0)
				{
					p=(u8*)strstr((const char*)(Uart2_Buf+2),"\r\n");
					p1=(u8*)strstr((const char*)(p+2),"\r\n");
					loc=p1-p;
					IhuDebugPrint("VMUO: SMS Content: ");
					USART_PRINT_Send_Len((char*)p+2,loc);
				}
        CLR_Buf2();
				mode=0;
				break;
			case 3:
				IhuDebugPrint("VMUO: Set SMS Center Number\n");
				Uart1_Buf[len]='\0';//添加结束符;
				strcpy((char*)temp,(const char *)"AT+CSCA=");
			  loc=sizeof("AT+CSCA=");
				strcpy((char*)(&temp[loc-1]),(const char*)&Uart1_Buf[4]);
				if(GSM_send_cmd(temp,"OK",3)==0)
					IhuDebugPrint("VMUO: Set SMS center successful\n");
				else
					IhuDebugPrint("VMUO: Set SMS center failure\n");
				mode=0;
				break;
		}
	}
}

/*******************************************************************************
* 函数名 : GSM_gprs_test
* 描述   : GPRS测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   :  为了保持连接，每空闲隔10秒发送一次心跳
*******************************************************************************/
u8 GSM_gprs_test()
{	
  u16 len=0;
	u8 mode=0;
	u8 temp[200];
	u8 flag=0;
	u8 *p1,*p2;
	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@GPRS TEST@@@@@@@@@@@@@@@@@@@@@\n");
	IhuDebugPrint("VMUO: Set Parameter: Set\"MODE\",\"IP\", Port+ Enter\n Eg: Set\"TCP\",\"125.89.18.79\",12345\n");
	IhuDebugPrint("VMUO: Send content: Send + Content + Enter \n, Eg: This is a test!\n");
	IhuDebugPrint("VMUO: Exit Test: Exit + Enter\n");
	
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0X3FFF;
			if(len>Buf2_Max-2)len=Buf2_Max-2;
			USART_RX_STA=0;
			if(strstr((const char*)Uart1_Buf,"EXIT"))mode=4;//退出
			else
			if(strstr((const char*)Uart1_Buf,"SET"))mode=1;//设置配置
			else
			if(strstr((const char*)Uart1_Buf,"SEND"))mode=2;//发送内容
			else 												mode=0;			
		}
		else
		{
			if(strstr((const char*)Uart2_Buf,"CONNECT OK")&&(flag==1))//连接成功
			{
					IhuDebugPrint("VMUO: Connect Successful\n");
					Time_miao=1;
					flag=2;
					mode=3;
					CLR_Buf2();
			}else
			if((flag==1)&&((strstr((const char*)Uart2_Buf,"CLOSED"))&&(Time_miao>10)))//连接失败或超时
			{
					IhuDebugPrint("VMUO: Connect failure\n");
					
				  while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
					USART_SendData(USART1 ,Time_miao);//发送当前字符
				
					Time_miao=0;
					flag=0;
					mode=1; //重新连接
					CLR_Buf2();
			}	
			if((flag==2)&&(Time_miao>10))//每10秒一个心跳包
			{
				Time_miao=1;
				mode=3;
			}
	  }
    switch(mode)
		{
			case 0:        
						if(strstr((const char*)Uart2_Buf,"+IPD"))//判断上位机是否有数据下发
						{
							IhuDebugPrint("VMUO: New content\n");
							p1=(u8*)strstr((const char*)Uart2_Buf,",");
							p2=(u8*)strstr((const char*)Uart2_Buf,":");
							USART_PRINT_Send_Len((char*)(p2+1),change_str_Data((p1+1),(p2-p1-1)));
							IhuDebugPrint("\r\n");
							CLR_Buf2();
						}
			break;
			case 1: 
				     if(strstr((const char*)Uart1_Buf,"TCP")||strstr((const char*)Uart1_Buf,"UDP"))
						 {
                strcpy((char*)&temp,(const char*)"AT+CIPSTART=");
							  memcpy((char*)&temp[12],(const char*)(&Uart1_Buf[4]),len-4);
							  temp[len+12]='\0';
							  GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",2);	//关闭连接
								GSM_send_cmd("AT+CIPSHUT","SHUT OK",2);		//关闭移动场景
 							  GSM_send_cmd("AT+CGCLASS=\"B\"","OK",2);//设置GPRS移动台类别为B,支持包交换和数据交换 
								GSM_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",2);//设置PDP上下文,互联网接协议,接入点等信息
								GSM_send_cmd("AT+CGATT=1","OK",2);//附着GPRS业务
								GSM_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",2);//设置为GPRS连接模式
								GSM_send_cmd("AT+CIPHEAD=1","OK",2);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
                GSM_send_cmd("AT+CIPMUX=0","OK",2);//设置单路连接
                if(GSM_send_cmd(temp,"OK",2 )==0)//发起连接
								{
                  IhuDebugPrint("VMUO: New connecting\n");
									Time_miao=1;
									flag=1;
								}
								else
								{
									IhuDebugPrint("VMUO: Enter error: Please re-enter\n");
									Time_miao=0;
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
								if(GSM_send_cmd("AT+CIPSEND",">",2)==0)
								{
										 Uart1_Buf[len]='\32';
										 Uart1_Buf[len+1]='\0';
										 if(GSM_send_cmd(&Uart1_Buf[4],"SEND OK",8)==0)
										 { 								
													IhuDebugPrint("VMUO: Send successful\n");
													Time_miao=1;
										 }
										 else
											 IhuDebugPrint("VMUO: Send failure\n");
										 
								}else
								{
										 SPS_GPRS_Data(0X1B);//ESC,取消发送
                     IhuDebugPrint("VMUO: Send failure\n");
								}
						  }else IhuDebugPrint("VMUO: Not connected\n");
							mode=0;
			break;
			case 3:
						if(GSM_send_cmd("AT+CIPSEND",">",2)==0)
						{
							SPS_GPRS_Data(0x00);
							SPS_GPRS_Data(0X1A);//CTRL+Z,结束数据发送,启动一次传输								
							IhuDebugPrint("VMUO: Heart-beat successful\n");

								 
						}else
						{
								 SPS_GPRS_Data(0X1B);//ESC,取消发送
							IhuDebugPrint("VMUO: Heart-beat failure\n");
						}
				mode=0;
			break;
			case 4:
						GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",5);	//关闭连接
						GSM_send_cmd("AT+CIPSHUT","SHUT OK",5);		//关闭移动场景
						return 1;
		}      
	}
}
/*******************************************************************************
* 函数名 : change_str_Data
* 描述   : 字符串转整型
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 change_str_Data(u8 *p,u8 len)
{
  u8 i=0;
	u8 value=0;
	for(i=0;i<len;i++)
	{
    value=value*10;
		value+=(*(p++)-'0');
	}
	return value;
}
/*******************************************************************************
* 函数名 : Swap
* 描述   : 交换
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Swap(char *ch1, char *ch2)
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
void change_Data_str(int n, char str[])
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
		Swap(str+i, str+len-i-1);
	str[len] = '\0';
}
/*******************************************************************************
* 函数名 : GSM_JZ_test
* 描述   : 基站定位测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
/*
u8 GSM_jz_test()
{
  u8 *p1,*p2,*p3;
	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@Base Station POSITON TEST@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	IhuDebugPrint("VMUO: EXIT TEST：EXIT + ENTER\n");
	GSM_send_cmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",2);
	GSM_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",2);
  if(GSM_send_cmd("AT+SAPBR=1,1","OK",5))//激活
	{
		//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
		IhuDebugPrint("VMUO: Open failure, please conform module own this function or not\n");
		return 1;		
	}
	if(GSM_send_cmd("AT+SAPBR=2,1","OK",5))
	{
    IhuDebugPrint("VMUO: Fetch local IP error\n");
		return 2;
	}
  Time_miao=1;
	while(1)
	{
    if(USART_RX_STA&0x8000)
		{
			USART_RX_STA=0;
			if(strstr(Uart1_Buf,"EXIT"))//退出
			{
				 GSM_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
				 return 0;
			}
			else
			IhuDebugPrint("VMUO: Enter Error\n");
		}
    if(GSM_send_cmd("AT+CIPGSMLOC=1,1","OK",10)==0)//获取经纬度和时间
		{
        Time_miao=1;
 			  p1=(u8*)strstr((const char*)(Uart2_Buf),",");
				p3=(u8*)strstr(p1,"\r\n");
				if(p1)//有效数据
				{	
					p2=(u8*)strtok((char*)(p1),",");
          IhuDebugPrint("VMUO: Logiture:");USART_PRINT_SendString(p2);UART1_SendData('\t');
					
					p2=(u8*)strtok(NULL,",");
          IhuDebugPrint("VMUO: Latitude:");USART_PRINT_SendString(p2);UART1_SendData('\t');
					
					p2=(u8*)strtok(NULL,",");
          IhuDebugPrint("VMUO: Date:");USART_PRINT_SendString(p2);UART1_SendData('\t');
					
					p2=(u8*)strtok(NULL,",");
					*p3='\0';//插入结束符
          IhuDebugPrint("VMUO: Time:");USART_PRINT_SendString(p2);IhuDebugPrint("\r\n");
				}
		}
		if(Time_miao>11)
		{
      Time_miao=0;
			//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
			IhuDebugPrint("VMUO: Open failure, please confirm this module own this function\n");
			return 1;
		}
	}
}
*/

/*******************************************************************************
* 函数名 : change_hex_str
* 描述   : 十六进制转换成字符串 如0xAC->'A''C';
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void change_hex_str(u8 dest[],u8 src[],u8 len)
{
    u8 i=0;
    u8 temp_h;
    u8 temp_l;
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
* 函数名 : GSM_jz_test
* 描述   : 基站定位测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 GSM_jz_test()
{
  u8 *p1,*p2,*p3;
	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@Base Station POSITON TEST@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
	IhuDebugPrint("VMUO: EXIT TEST: EXIT + ENTER\n");
	GSM_send_cmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",2);
	GSM_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",2);
  if(GSM_send_cmd("AT+SAPBR=1,1","OK",5))//激活
	{
	
		IhuDebugPrint("VMUO: Open failure, please conform module own this function or not\n");
		return 1;		
	}
	if(GSM_send_cmd("AT+SAPBR=2,1","OK",5))
	{
    IhuDebugPrint("VMUO: Fetch local IP error\n");
		return 2;
	}
  Time_miao=1;
	while(1)
	{
    if(USART_RX_STA&0x8000)
		{
			USART_RX_STA=0;
			if(strstr((const char*)Uart1_Buf,"EXIT"))//退出
			{
				 GSM_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
				 return 0;
			}
			else
			IhuDebugPrint("VMUO: ENTER ERROR\n");
		}
    if(GSM_send_cmd("AT+CIPGSMLOC=1,1","OK",10)==0)//获取经纬度和时间
		{
        Time_miao=1;
 			  p1=(u8*)strstr((const char*)(Uart2_Buf),",");
				p3=(u8*)strstr((const char*)p1,"\r\n");
				if(p1)//有效数据
				{	
					p2=(u8*)strtok((char*)(p1),",");
          IhuDebugPrint("VMUO: Longitude:");USART_PRINT_SendString((char *)p2);USART_PRINT_Data('\t');
					
					p2=(u8*)strtok(NULL,",");
          IhuDebugPrint("VMUO: Latitude:");USART_PRINT_SendString((char *)p2);USART_PRINT_Data('\t');
					
					p2=(u8*)strtok(NULL,",");
          IhuDebugPrint("VMUO: Date:");USART_PRINT_SendString((char *)p2);USART_PRINT_Data('\t');
					
					p2=(u8*)strtok(NULL,",");
					*p3='\0';//插入结束符
          IhuDebugPrint("VMUO: Time:");USART_PRINT_SendString((char *)p2);IhuDebugPrint("\r\n");
				}
		}
		if(Time_miao>11)
		{
      Time_miao=0;
			//sim900a_send_cmd("AT+SAPBR=0,1","OK",2);//关闭
			IhuDebugPrint("VMUO: Open failure, please confirm this module own this function\n");
			return 1;
		}
	}
}
#define MAXlen 200 //定义最多播放的字节数
/*******************************************************************************
* 函数名 : GSM_tts_test
* 描述   : TTS文本语音测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 GSM_tts_test()
{
  u16 len=0;
  u8 temp_src[]="Hello, this is a test from BXXH!";
	u8 temp[MAXlen+15];
	u8 loc=0;
	IhuDebugPrint("VMUO: @@@@@@@@@@@@@@@@@@@@@TTS TEXT VOICE TEST@@@@@@@@@@@@@@@@@@@@@@@\n");
	IhuDebugPrint("VMUO: EXIT TEST: EXIT + ENTER\n");
	IhuDebugPrint("VMUO: VOICE Brocasting: Please enter content + Enter\n");
  IhuDebugPrint("VMUO: Attention, support max 100 byte\n");
	if(GSM_send_cmd("AT+CTTS=?","OK",2))
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
  GSM_send_cmd(temp,"OK",3);
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
      len=USART_RX_STA&0X3FFF;
			if(len>(MAXlen/2))len=(MAXlen/2);
      USART_RX_STA=0;
			if((len==4)&&(strstr((const char*)Uart1_Buf,"EXIT")))//退出	
			return 0;
			SPS_GPRS_SendString("AT+CTTS=2,\"");
			Uart1_Buf[len]='\"';
			Uart1_Buf[len+1]='\0';
			if(GSM_send_cmd(Uart1_Buf,"OK",2))
				IhuDebugPrint("VMUO: Wait for previous frame accomplish\n");
		}
	}
}

/*******************************************************************************
* 函数名 : CLR_Buf2
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void SPS_GPRS_clear_receive_buffer(void)
{
	u16 k;
	for(k=0;k<SPS_GPRS_REC_MAXLEN;k++)      //将缓存内容清零
	{
		SPS_GPRS_R_Buff[k] = 0x00;
	}
  SPS_GPRS_R_Count = 0;               //接收字符串的起始存储位置
}

/*******************************************************************************
* 函数名 : Find
* 描述   : 判断缓存中是否含有指定的字符串
* 输入   : 
* 输出   : 
* 返回   : unsigned char:1 找到指定字符，0 未找到指定字符 
* 注意   : 
*******************************************************************************/
u8 SPS_GPRS_Find_char(char *a)
{ 
  if(strstr((char *)SPS_GPRS_R_Buff, a)!=NULL)
	    return 1;
	else
			return 0;
}

/*******************************************************************************
* 函数名 : GSM_send_cmd
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：ms)
* 输出   : 
* 返回   : 0:正常  1:错误
* 注意   : 
*******************************************************************************/
u8 SPS_GPRS_send_AT_command(char *cmd, char *ack, u16 wait_time)   //in ms      
{
	u8 res=1;
	SPS_GPRS_clear_receive_buffer(); 
	for (; *cmd!='\0';cmd++)
	{
		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET);
	  USART_SendData(USART_GPRS,*cmd);
	}
	SPS_GPRS_SendLR();	
	ihu_sleep(wait_time);
	if(strstr((const char*)SPS_GPRS_R_Buff,(char*)ack)==NULL)
		 res=1;
	else res=0;

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
void SPS_GPRS_SendString(char* s)
{
	while(*s != '\0')//检测字符串结束符
	{
		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET); 
		USART_SendData(USART_GPRS ,*s++);//发送当前字符
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
void SPS_GPRS_Wait_CREG(void)
{
	u8 i;
	u8 k;
	i = 0;
  while(i == 0)        			
	{
		SPS_GPRS_clear_receive_buffer();        
		SPS_GPRS_SendString("AT+CREG?");   //查询等待模块注册成功
		SPS_GPRS_SendLR();
		ihu_sleep(500);  //等待500ms
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
void SPS_GPRS_Set_ATE0(void)
{
	SPS_GPRS_send_AT_command("ATE0", "OK", 500);								//取消回显		
}

/*******************************************************************************
* 函数名 : Connect_Server
* 描述   : GPRS连接服务器函数
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void SPS_GPRS_connect_server(void)
{
	SPS_GPRS_clear_receive_buffer();
	SPS_GPRS_SendString("AT+CIPCLOSE=1");	//关闭连接
  ihu_sleep(100);
	SPS_GPRS_send_AT_command("AT+CIPSHUT","SHUT OK", 500);		//关闭移动场景
	SPS_GPRS_send_AT_command("AT+CGCLASS=\"B\"","OK", 500);//设置GPRS移动台类别为B,支持包交换和数据交换 
	SPS_GPRS_send_AT_command("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK", 500);//设置PDP上下文,互联网接协议,接入点等信息
	SPS_GPRS_send_AT_command("AT+CGATT=1","OK", 500);//附着GPRS业务
	SPS_GPRS_send_AT_command("AT+CIPCSGP=1,\"CMNET\"","OK", 500);//设置为GPRS连接模式
  SPS_GPRS_send_AT_command("AT+CIPMUX=0","OK", 500);//设置为单路连接
	SPS_GPRS_send_AT_command("AT+CIPHEAD=1","OK", 500);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	SPS_GPRS_send_AT_command("AT+CIPMODE=1","OK", 500);//打开透传功能
	SPS_GPRS_send_AT_command("AT+CIPCCFG=4,5,200,1","OK", 500);//配置透传模式：单包重发次数:2,间隔1S发送一次,每次发送200的字节
  SPS_GPRS_send_AT_command((char *)SPS_GPRS_string, "OK", 2000);//建立连接
  
  ihu_sleep(100);                                //等待串口数据接收完毕
  SPS_GPRS_clear_receive_buffer();                                    //清串口接收缓存为透传模式准备
}

/*******************************************************************************
* 函数名 : Rec_Server_Data
* 描述   : 接收服务器数据函数,并原样返回
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void SPS_GPRS_Rec_Server_Data(void)
{
//p2指向发送数据，p1指向接收数据，这里可以使用SPS_GPRS_R_Count来判断
//	if(p2!=p1)   		//说明还有数据未发出
//	{	
//		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET);
//		SPS_GPRS_SendData(USART_GPRS, *p2);
//		p2++;
//    if(p2>&SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN])
//			p2=SPS_GPRS_R_Buff;
//	}
}

//一个完整的数据流程，且不断回送数据
void SPS_GPRS_data_connection_and_receive_process(void)
{
	IhuDebugPrint("VMUO: GPRS Module GPRS register network!\n");
	SPS_GPRS_SendString("+++");//退出透传模式，避免模块还处于透传模式中
  ihu_usleep(500);  //500ms
	SPS_GPRS_Wait_CREG();   //等待模块注册成功
	IhuDebugPrint("VMUO: GPRS Register Success!\n");
	IhuDebugPrint("VMUO: GPRS Module connect server!\n");
	SPS_GPRS_Set_ATE0();    //取消回显
	SPS_GPRS_connect_server(); //设置GPRS参数
	IhuDebugPrint("VMUO: Connect success!\n");

//	while(1)
//	{
//		Rec_Server_Data();//接收数据并原样返回	
//	}

}

/*******************************************************************************
* º¯ÊýÃû  : USART_PRINT_Send_Len
* ÃèÊö    : USART1·¢ËÍÒ»¶¨³¤¶ÈµÄ×Ö·û
* ÊäÈë    : *s×Ö·û´®Ö¸Õë£¬len£º×Ö·û³¤¶È
* Êä³ö    : ÎÞ
* ·µ»Ø    : ÎÞ 
* ËµÃ÷    : ÎÞ
*******************************************************************************/
void USART_PRINT_Send_Len(char *s,u8 len)
{
	if(s!=0)
	while(len)//Checking end char
	{
		while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
		USART_SendData(USART_PRINT ,*s++);//Send current char
		len--;
	}
}	

/*******************************************************************************
* º¯ÊýÃû  : USART_PRINT_Data
* ÃèÊö    : USART1·¢ËÍÒ»¸ö×Ö½Ú
* ÊäÈë    : byte Ò»¸ö×Ö½Ú
* Êä³ö    : ÎÞ
* ·µ»Ø    : ÎÞ 
* ËµÃ÷    : ÎÞ
*******************************************************************************/
void USART_PRINT_Data(char byte)
{
		while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
		USART_SendData(USART_PRINT ,byte);//Send current char
}

void USART_PRINT_SendString(char* s)
{
	while(*s)//check end char
	{
		while(USART_GetFlagStatus(USART_PRINT, USART_FLAG_TC)==RESET); 
		USART_SendData(USART_PRINT ,*s++);//Send current char
	}
}

void SPS_GPRS_Data(char byte)
{
		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET); 
		USART_SendData(USART_GPRS ,byte);//Send current char
}


