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
	
vu8 USART_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN];	//串口GPRS数据接收缓冲区 
vu8 USART_GPRS_R_State=0;					//串口GPRS接收状态
vu16 USART_GPRS_R_Count=0;					//当前接收数据的字节数

vu8 USART_RFID_R_Buff[SPS_RFID_REC_MAXLEN];	//串口RFID数据接收缓冲区 
vu8 USART_RFID_R_State=0;					//串口RFID接收状态
vu16 USART_RFID_R_Count=0;					//当前接收数据的字节数 	  

vu8 UART_BLE_R_Buff[SPS_BLE_REC_MAXLEN];	//串口BLE数据接收缓冲区 
vu8 UART_BLE_R_State=0;					//串口BLE接收状态
vu16 UART_BLE_R_Count=0;					//当前接收数据的字节数 	  

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
	GPIO_Init(GPIOA, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化USART_GPRS_TXD(PA.9)  
 
	GPIO_InitStructure.GPIO_Pin = USART_GPRS_RX_GPIO_PIN;	 			//USART_GPRS_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化USART_GPRS_RXD(PA.10)

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
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//从优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器 
	  
  USART_ITConfig(USART_GPRS, USART_IT_RXNE, ENABLE);			//使能串口1接收中断

  USART_Cmd(USART_GPRS, ENABLE);                    			//使能串口 
	USART_ClearFlag(USART_GPRS, USART_FLAG_TC);					//清除发送完成标志
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : USART_GPRS发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_GPRS_SendData(u8* buff, u16 len)
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

	if(USART_GetITStatus(USART_GPRS, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART_GPRS); //读取接收到的数据(USART_GPRS->DR)
		
		USART_GPRS_R_Buff[USART_GPRS_R_Count++] = Res;
		if(USART_GPRS_R_State == 0)//数据接收未完成
		{
			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
			{
				USART_GPRS_R_State =2;
			}
		}
		else if(USART_GPRS_R_State == 2)
		{
			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
			{
				USART_GPRS_R_State =1;//数据接收完成
			}
			else//接收错误
			{
				USART_GPRS_R_State =0;
				USART_GPRS_R_Count =0;
			}
		}
		if(USART_GPRS_R_Count >= SPS_GPRS_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
		{
			if((USART_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN-2] != 0x0d) || (USART_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN-1] != 0x0a))
			{
				USART_GPRS_R_Count =0;
				USART_GPRS_R_State =0;
			}
		} 		 
	} 
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
	GPIO_Init(GPIOA, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化USART_RFID_TXD(PA.9)  
 
	GPIO_InitStructure.GPIO_Pin = USART_RFID_RX_GPIO_PIN;	 			//USART_RFID_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化USART_RFID_RXD(PA.10)

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
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : USART_RFID发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_RFID_SendData(u8* buff, u16 len)
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
		
		USART_RFID_R_Buff[USART_RFID_R_Count++] = Res;
		if(USART_RFID_R_State == 0)//数据接收未完成
		{
			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
			{
				USART_RFID_R_State =2;
			}
		}
		else if(USART_RFID_R_State == 2)
		{
			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
			{
				USART_RFID_R_State =1;//数据接收完成
			}
			else//接收错误
			{
				USART_RFID_R_State =0;
				USART_RFID_R_Count =0;
			}
		}
		if(USART_RFID_R_Count >= SPS_RFID_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
		{
			if((USART_RFID_R_Buff[SPS_RFID_REC_MAXLEN-2] != 0x0d) || (USART_RFID_R_Buff[SPS_RFID_REC_MAXLEN-1] != 0x0a))
			{
				USART_RFID_R_Count =0;
				USART_RFID_R_State =0;
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
	GPIO_Init(GPIOA, &GPIO_InitStructure);				 //调用库函数中的GPIO初始化函数，初始化UART_BLE_TXD(PA.9)  
 
	GPIO_InitStructure.GPIO_Pin = UART_BLE_RX_GPIO_PIN;	 			//UART_BLE_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	  //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化UART_BLE_RXD(PA.10)

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
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : UART_BLE发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void SPS_BLE_SendData(u8* buff, u16 len)
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
		
		UART_BLE_R_Buff[UART_BLE_R_Count++] = Res;
		if(UART_BLE_R_State == 0)//数据接收未完成
		{
			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
			{
				UART_BLE_R_State =2;
			}
		}
		else if(UART_BLE_R_State == 2)
		{
			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
			{
				UART_BLE_R_State =1;//数据接收完成
			}
			else//接收错误
			{
				UART_BLE_R_State =0;
				UART_BLE_R_Count =0;
			}
		}
		if(UART_BLE_R_Count >= SPS_BLE_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
		{
			if((UART_BLE_R_Buff[SPS_BLE_REC_MAXLEN-2] != 0x0d) || (UART_BLE_R_Buff[SPS_BLE_REC_MAXLEN-1] != 0x0a))
			{
				UART_BLE_R_Count =0;
				UART_BLE_R_State =0;
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
int fputc(int ch, FILE *f)
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
int fgetc(FILE *f)
{
 /* 等待串口1输入数据 */
 while (USART_GetFlagStatus(USART_GPRS, USART_FLAG_RXNE) == RESET);

 return (int)USART_ReceiveData(USART_GPRS);
}	
