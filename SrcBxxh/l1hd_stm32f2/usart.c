/**********************************************************************************
 * 文件名  ：usart.c
 * 描述    ：usart1 应用函数库          
 * 实验平台：NiRen_STM32min板
 * 硬件连接：TXD(PB9)  -> 外部串口RXD     
 *           RXD(PB10) -> 外部串口TXD      
 *           GND	   -> 外部串口GND 
 * 库版本  ：ST_v3.5
**********************************************************************************/

#include "stm32f2xx.h"
#include "usart.h"	  
	
vu8 SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN];	//串口GPRS数据接收缓冲区 
vu8 SPS_GPRS_R_State=0;					//串口GPRS接收状态
vu16 SPS_GPRS_R_Count=0;					//当前接收数据的字节数
const char *SPS_GPRS_string = "AT+CIPSTART=\"TCP\",\"14.125.48.205\",9015";//IP登录服务器,需要根据自己的IP和端口做修改

vu8 SPS_RFID_R_Buff[SPS_RFID_REC_MAXLEN];	//串口RFID数据接收缓冲区 
vu8 SPS_RFID_R_State=0;					//串口RFID接收状态
vu16 SPS_RFID_R_Count=0;					//当前接收数据的字节数 	  

vu8 SPS_BLE_R_Buff[SPS_BLE_REC_MAXLEN];	//串口BLE数据接收缓冲区 
vu8 SPS_BLE_R_State=0;					//串口BLE接收状态
vu16 SPS_BLE_R_Count=0;					//当前接收数据的字节数 	  

vu8 SPS_SPARE1_R_Buff[SPS_SPARE1_REC_MAXLEN];	//串口SPARE1数据接收缓冲区 
vu8 SPS_SPARE1_R_State=0;					//串口SPARE1接收状态
vu16 SPS_SPARE1_R_Count=0;					//当前接收数据的字节数 	  


/*******************************************************************************
* 函数名  : USART_GPRS_Init_Config
* 描述    : USART_GPRS初始化配置
* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_GPRS_Init_Config(u32 bound)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	/*使能USART_GPRS和GPIO外设时钟*/  
	USART_GPRS_TX_ENABLE();
	USART_GPRS_RX_ENABLE();
	USART_GPRS_CLK_ENABLE();
	
	/*复位串口GPRS*/
 	USART_DeInit(USART_GPRS);  
	
	/*USART_GPRS_GPIO初始化设置*/ 
	GPIO_PinAFConfig(USART_GPRS_TX_GPIO_PORT, USART_GPRS_TX_SOURCE, USART_GPRS_TX_AF);
	GPIO_PinAFConfig(USART_GPRS_RX_GPIO_PORT, USART_GPRS_RX_SOURCE, USART_GPRS_RX_AF);
	GPIO_InitStructure.GPIO_Pin = USART_GPRS_TX_GPIO_PIN;			//USART_GPRS_TXD(PA.9)     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
	GPIO_Init(USART_GPRS_TX_GPIO_PORT, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化USART_GPRS_TXD(PA.9)  
 
	GPIO_InitStructure.GPIO_Pin = USART_GPRS_RX_GPIO_PIN;	 			//USART_GPRS_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
	GPIO_Init(USART_GPRS_RX_GPIO_PORT, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化USART_GPRS_RXD(PA.10)

  /*USART_GPRS 初始化设置*/
	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
	USART_Init(USART_GPRS, &USART_InitStructure);										//初始化串口1

  /*USART_GPRS NVIC配置*/
  NVIC_InitStructure.NVIC_IRQChannel = USART_GPRS_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//从优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器 
	  
  USART_ITConfig(USART_GPRS, USART_IT_RXNE, ENABLE);			//使能串口1接收中断 Receive Data register not empty interrupt 
  USART_Cmd(USART_GPRS, ENABLE);                    			//使能串口 
	USART_ClearFlag(USART_GPRS, USART_FLAG_TC);							//清除发送完成标志 Transmission complete interrupt
	
	//挂载中断
	BSP_IntVectSet(USART_GPRS_INT_VECTOR, SPS_GPRS_IRQHandler);
  BSP_IntEn(USART_GPRS_INT_VECTOR);
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : USART_GPRS发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_GPRS_SendData(vs8* buff, u16 len)
{    
	u16 i;

	for(i=0; i<len; i++)  
	{
		while(USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC)==RESET); 
	    USART_SendData(USART_GPRS ,buff[i]);   
	}
}

/*******************************************************************************
* 函数名  : USART_GPRS_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 1)、只启动了USART_GPRS中断接收，未启动USART_GPRS中断发送。
*           2)、接收到0x0d 0x0a(回车、"\r\n")代表帧数据接收完成
*******************************************************************************/
void SPS_GPRS_IRQHandler(void)                	
{
	u8 Res=0;
	
	//要不要设计成这样的：中断进来后立即禁止再次重入，等待接收R_BUFFER被处理好了后再行ENABLE该中断？
	//还是我们有更好的方式，比如直接采用USART_IT_RXNE比特位，这个比特位一旦拉高后，不会再次进来了？
	//当然还有一种方式来规避这个问题，就是接收到了数据后立即发送给任务模块，这个接收就继续了。
	//由于状态机控制的复杂性，如果不是要做正常的双向通信机制，我们这里将采用与TIMER配合的轮询方式来工作
	//USART_ITConfig(USART_GPRS, USART_IT_RXNE, DISABLE);
	
	// 原始代码中，使用了USART_IT_RXNE进行判定，而非USART_FLAG_RXNE，后续需要仔细确定，到底该如何？
	if(USART_GetITStatus(USART_GPRS, USART_FLAG_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(USART_GPRS, USART_IT_RXNE);
		Res =USART_ReceiveData(USART_GPRS); //读取接收到的数据(USART_GPRS->DR)
		
		SPS_GPRS_R_Buff[SPS_GPRS_R_Count++] = Res;
		if(SPS_GPRS_R_State == 0)//数据接收未完成
		{
			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
			{
				SPS_GPRS_R_State =2;
			}
		}
		else if(SPS_GPRS_R_State == 2)
		{
			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到0xoa则接收完成
			{
				SPS_GPRS_R_State =1;//数据接收完成
			}
			else//接收错误
			{
				SPS_GPRS_R_State =0;
				SPS_GPRS_R_Count =0;
			}
		}
		if(SPS_GPRS_R_Count >= SPS_GPRS_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
		{
			if((SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN-2] != 0x0d) || (SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN-1] != 0x0a))
			{
				SPS_GPRS_R_Count =0;
				SPS_GPRS_R_State =0;
			}
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
* 函数名  : USART_RFID_Init_Config
* 描述    : USART_RFID初始化配置
* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_RFID_Init_Config(u32 bound)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	/*使能USART_RFID和GPIO外设时钟*/  
	USART_RFID_TX_ENABLE();
	USART_RFID_RX_ENABLE();
	USART_RFID_CLK_ENABLE();
	
	/*复位串口RFID*/
 	USART_DeInit(USART_RFID);  
	
	/*USART_RFID_GPIO初始化设置*/ 
	GPIO_PinAFConfig(USART_RFID_TX_GPIO_PORT, USART_RFID_TX_SOURCE, USART_RFID_TX_AF);
	GPIO_PinAFConfig(USART_RFID_RX_GPIO_PORT, USART_RFID_RX_SOURCE, USART_RFID_RX_AF);
	GPIO_InitStructure.GPIO_Pin = USART_RFID_TX_GPIO_PIN;			//USART_RFID_TXD(PA.9)     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
	GPIO_Init(USART_RFID_TX_GPIO_PORT, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化USART_RFID_TXD(PA.9)  
 
	GPIO_InitStructure.GPIO_Pin = USART_RFID_RX_GPIO_PIN;	 			//USART_RFID_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
	GPIO_Init(USART_RFID_RX_GPIO_PORT, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化USART_RFID_RXD(PA.10)

  /*USART_RFID 初始化设置*/
	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
	USART_Init(USART_RFID, &USART_InitStructure);										//初始化串口1

  /*USART_RFID NVIC配置*/
  NVIC_InitStructure.NVIC_IRQChannel = USART_RFID_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//从优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器 
	  
  USART_ITConfig(USART_RFID, USART_IT_RXNE, ENABLE);			//使能串口1接收中断
  USART_Cmd(USART_RFID, ENABLE);                    			//使能串口 
	USART_ClearFlag(USART_RFID, USART_FLAG_TC);					//清除发送完成标志
	
	//挂载中断
	BSP_IntVectSet(USART_RFID_INT_VECTOR, SPS_RFID_IRQHandler);
  BSP_IntEn(USART_RFID_INT_VECTOR);
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : USART_RFID发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_RFID_SendData(vs8* buff, u16 len)
{    
	u16 i;

	for(i=0; i<len; i++)  
	{
		while(USART_GetFlagStatus(USART_RFID, USART_FLAG_TC)==RESET); 
	    USART_SendData(USART_RFID ,buff[i]);   
	}
}

/*******************************************************************************
* 函数名  : USART_RFID_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 1)、只启动了USART_RFID中断接收，未启动USART_RFID中断发送。
*           2)、接收到0x0d 0x0a(回车、"\r\n")代表帧数据接收完成
*******************************************************************************/
void SPS_RFID_IRQHandler(void)                	
{
	u8 Res=0;

	if(USART_GetITStatus(USART_RFID, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART_RFID); //读取接收到的数据(USART_RFID->DR)
		
		SPS_RFID_R_Buff[SPS_RFID_R_Count++] = Res;
		if(SPS_RFID_R_State == 0)//数据接收未完成
		{
			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
			{
				SPS_RFID_R_State =2;
			}
		}
		else if(SPS_RFID_R_State == 2)
		{
			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
			{
				SPS_RFID_R_State =1;//数据接收完成
			}
			else//接收错误
			{
				SPS_RFID_R_State =0;
				SPS_RFID_R_Count =0;
			}
		}
		if(SPS_RFID_R_Count >= SPS_RFID_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
		{
			if((SPS_RFID_R_Buff[SPS_RFID_REC_MAXLEN-2] != 0x0d) || (SPS_RFID_R_Buff[SPS_RFID_REC_MAXLEN-1] != 0x0a))
			{
				SPS_RFID_R_Count =0;
				SPS_RFID_R_State =0;
			}
		} 		 
	} 
} 	

/*******************************************************************************
* 函数名  : UART_BLE_Init_Config
* 描述    : UART_BLE初始化配置
* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_BLE_Init_Config(u32 bound)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	/*使能UART_BLE和GPIO外设时钟*/  
	UART_BLE_TX_ENABLE();
	UART_BLE_RX_ENABLE();
	UART_BLE_CLK_ENABLE();
	
	/*复位串口BLE*/
 	USART_DeInit(UART_BLE);  
	
	/*UART_BLE_GPIO初始化设置*/ 
	GPIO_PinAFConfig(UART_BLE_TX_GPIO_PORT, UART_BLE_TX_SOURCE, UART_BLE_TX_AF);
	GPIO_PinAFConfig(UART_BLE_RX_GPIO_PORT, UART_BLE_RX_SOURCE, UART_BLE_RX_AF);
	GPIO_InitStructure.GPIO_Pin = UART_BLE_TX_GPIO_PIN;			//UART_BLE_TXD(PA.9)     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
	GPIO_Init(UART_BLE_TX_GPIO_PORT, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化UART_BLE_TXD(PA.9)  
 
	GPIO_InitStructure.GPIO_Pin = UART_BLE_RX_GPIO_PIN;	 			//UART_BLE_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
	GPIO_Init(UART_BLE_RX_GPIO_PORT, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化UART_BLE_RXD(PA.10)

  /*UART_BLE 初始化设置*/
	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
	USART_Init(UART_BLE, &USART_InitStructure);										//初始化串口1

  /*UART_BLE NVIC配置*/
  NVIC_InitStructure.NVIC_IRQChannel = UART_BLE_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//从优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器 
	  
  USART_ITConfig(UART_BLE, USART_IT_RXNE, ENABLE);			//使能串口1接收中断
  USART_Cmd(UART_BLE, ENABLE);                    			//使能串口 
	USART_ClearFlag(UART_BLE, USART_FLAG_TC);					//清除发送完成标志
	
	//挂载中断
	BSP_IntVectSet(UART_BLE_INT_VECTOR, SPS_BLE_IRQHandler);
  BSP_IntEn(UART_BLE_INT_VECTOR);
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : UART_BLE发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_BLE_SendData(vs8* buff, u16 len)
{    
	u16 i;

	for(i=0; i<len; i++)  
	{
		while(USART_GetFlagStatus(UART_BLE, USART_FLAG_TC)==RESET); 
	    USART_SendData(UART_BLE ,buff[i]);   
	}
}

/*******************************************************************************
* 函数名  : UART_BLE_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 1)、只启动了UART_BLE中断接收，未启动UART_BLE中断发送。
*           2)、接收到0x0d 0x0a(回车、"\r\n")代表帧数据接收完成
*******************************************************************************/
void SPS_BLE_IRQHandler(void)                	
{
	u8 Res=0;

	if(USART_GetITStatus(UART_BLE, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(UART_BLE); //读取接收到的数据(UART_BLE->DR)
		
		SPS_BLE_R_Buff[SPS_BLE_R_Count++] = Res;
		if(SPS_BLE_R_State == 0)//数据接收未完成
		{
			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
			{
				SPS_BLE_R_State =2;
			}
		}
		else if(SPS_BLE_R_State == 2)
		{
			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
			{
				SPS_BLE_R_State =1;//数据接收完成
			}
			else//接收错误
			{
				SPS_BLE_R_State =0;
				SPS_BLE_R_Count =0;
			}
		}
		if(SPS_BLE_R_Count >= SPS_BLE_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
		{
			if((SPS_BLE_R_Buff[SPS_BLE_REC_MAXLEN-2] != 0x0d) || (SPS_BLE_R_Buff[SPS_BLE_REC_MAXLEN-1] != 0x0a))
			{
				SPS_BLE_R_Count =0;
				SPS_BLE_R_State =0;
			}
		} 		 
	} 
}

/*******************************************************************************
* 函数名  : UART_SPARE1_Init_Config
* 描述    : UART_SPARE1初始化配置
* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_SPARE1_Init_Config(u32 bound)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	/*使能UART_SPARE1和GPIO外设时钟*/  
	UART_SPARE1_TX_ENABLE();
	UART_SPARE1_RX_ENABLE();
	UART_SPARE1_CLK_ENABLE();
	
	/*复位串口SPARE1*/
 	USART_DeInit(UART_SPARE1);  
	
	/*UART_SPARE1_GPIO初始化设置*/ 
	GPIO_PinAFConfig(UART_SPARE1_TX_GPIO_PORT, UART_SPARE1_TX_SOURCE, UART_SPARE1_TX_AF);
	GPIO_PinAFConfig(UART_SPARE1_RX_GPIO_PORT, UART_SPARE1_RX_SOURCE, UART_SPARE1_RX_AF);
	GPIO_InitStructure.GPIO_Pin = UART_SPARE1_TX_GPIO_PIN;			//UART_SPARE1_TXD(PA.9)     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
	GPIO_Init(UART_SPARE1_TX_GPIO_PORT, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化UART_SPARE1_TXD(PA.9)  
 
	GPIO_InitStructure.GPIO_Pin = UART_SPARE1_RX_GPIO_PIN;	 			//UART_SPARE1_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
	GPIO_Init(UART_SPARE1_RX_GPIO_PORT, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化UART_SPARE1_RXD(PA.10)

  /*UART_SPARE1 初始化设置*/
	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
	USART_Init(UART_SPARE1, &USART_InitStructure);										//初始化串口1

  /*UART_SPARE1 NVIC配置*/
  NVIC_InitStructure.NVIC_IRQChannel = UART_SPARE1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//从优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器 
	  
  USART_ITConfig(UART_SPARE1, USART_IT_RXNE, ENABLE);			//使能串口1接收中断
  USART_Cmd(UART_SPARE1, ENABLE);                    			//使能串口 
	USART_ClearFlag(UART_SPARE1, USART_FLAG_TC);					//清除发送完成标志

	//挂载中断
	BSP_IntVectSet(UART_SPARE1_INT_VECTOR, SPS_SPARE1_IRQHandler);
  BSP_IntEn(UART_SPARE1_INT_VECTOR);
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : UART_SPARE1发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_SPARE1_SendData(vs8* buff, u16 len)
{    
	u16 i;

	for(i=0; i<len; i++)  
	{
		while(USART_GetFlagStatus(UART_SPARE1, USART_FLAG_TC)==RESET); 
	    USART_SendData(UART_SPARE1 ,buff[i]);   
	}
}

/*******************************************************************************
* 函数名  : UART_SPARE1_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 1)、只启动了UART_SPARE1中断接收，未启动UART_SPARE1中断发送。
*           2)、接收到0x0d 0x0a(回车、"\r\n")代表帧数据接收完成
*******************************************************************************/
void SPS_SPARE1_IRQHandler(void)                	
{
	u8 Res=0;

	if(USART_GetITStatus(UART_SPARE1, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(UART_SPARE1); //读取接收到的数据(UART_SPARE1->DR)
		
		SPS_SPARE1_R_Buff[SPS_SPARE1_R_Count++] = Res;
		if(SPS_SPARE1_R_State == 0)//数据接收未完成
		{
			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
			{
				SPS_SPARE1_R_State =2;
			}
		}
		else if(SPS_SPARE1_R_State == 2)
		{
			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
			{
				SPS_SPARE1_R_State =1;//数据接收完成
			}
			else//接收错误
			{
				SPS_SPARE1_R_State =0;
				SPS_SPARE1_R_Count =0;
			}
		}
		if(SPS_SPARE1_R_Count >= SPS_SPARE1_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
		{
			if((SPS_SPARE1_R_Buff[SPS_SPARE1_REC_MAXLEN-2] != 0x0d) || (SPS_SPARE1_R_Buff[SPS_SPARE1_REC_MAXLEN-1] != 0x0a))
			{
				SPS_SPARE1_R_Count =0;
				SPS_SPARE1_R_State =0;
			}
		} 		 
	} 
}

/***********************************************************************/
/*************************----- 串口重定向 -----************************/
/***********************************************************************/
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 

/*******************************************************************************
* 函数名  : fputc
* 描述    : 重定义putc函数
* 输入    : 无
* 返回    : 无 
* 说明    : 重定义putc函数，这样可以使用printf函数从串口1打印输出
*******************************************************************************/
int sps_fputc(int ch, FILE *f)
{
 /* e.g. 给USART写一个字符 */
 USART_SendData(USART_GPRS, (uint8_t) ch);

 /* 循环直到发送完成 */
 while (USART_GetFlagStatus(USART_GPRS, USART_FLAG_TC) == RESET);

 return ch;
}

/*******************************************************************************
* 函数名  : fgetc
* 描述    : 重定义getc函数
* 输入    : 无
* 返回    : 无 
* 说明    : 重定义getc函数，这样可以使用scanff函数从串口1输入数据
*******************************************************************************/
int sps_fgetc(FILE *f)
{
 /* 等待串口1输入数据 */
 while (USART_GetFlagStatus(USART_GPRS, USART_FLAG_RXNE) == RESET);

 return (int)USART_ReceiveData(USART_GPRS);
}	





