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
#include "l1hd_usart.h"	  
	
vu8 SPS_GPRS_R_Buff[SPS_GPRS_REC_MAXLEN];			//串口GPRS数据接收缓冲区 
vu8 SPS_GPRS_R_State=0;												//串口GPRS接收状态
vu16 SPS_GPRS_R_Count=0;											//当前接收数据的字节数
u8 SPS_GPRS_TIMER_TRIGGER_Count=0;  					//串口GPRS的时间计时器，跟TIM2相互关联，从而使得GPRS串口在接收IRQ处理程序中可以设置时间超时定时器

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
	SPS_GPRS_TIMER_TRIGGER_Count=0;
	//要不要设计成这样的：中断进来后立即禁止再次重入，等待接收R_BUFFER被处理好了后再行ENABLE该中断？
	//还是我们有更好的方式，比如直接采用USART_IT_RXNE比特位，这个比特位一旦拉高后，不会再次进来了？
	//当然还有一种方式来规避这个问题，就是接收到了数据后立即发送给任务模块，这个接收就继续了。
	//由于状态机控制的复杂性，如果不是要做正常的双向通信机制，我们这里将采用与TIMER配合的轮询方式来工作
	//USART_ITConfig(USART_GPRS, USART_IT_RXNE, DISABLE);
	
	// 原始代码中，使用了USART_IT_RXNE进行判定，而非USART_FLAG_RXNE，后续需要仔细确定，到底该如何？
	if(USART_GetITStatus(USART_GPRS, USART_FLAG_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(USART_GPRS, USART_IT_RXNE);
		Res = USART_ReceiveData(USART_GPRS); //读取接收到的数据(USART_GPRS->DR)
		
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
* 函数名  : SPS_GPRS_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 
* 本函数是原始函数，放在这里，只是为了对比而已
*******************************************************************************/
//void SPS_GPRS_IRQHandler(void)                	
//{
//			u8 Res=0;
//      SPS_GPRS_TIMER_TRIGGER_Count=0;
//			Res= USART_ReceiveData(USART_GPRS);		//将接收到的字符串存到缓存中
//			SPS_GPRS_R_Buff[SPS_GPRS_R_Count]= Res; 
//			SPS_GPRS_R_Count++;                			//缓存指针向后移动
//			if(SPS_GPRS_R_Count > SPS_GPRS_REC_MAXLEN)       		//如果缓存满,将缓存指针指向缓存的首地址
//			{
//				SPS_GPRS_R_Count = 0;
//			}
//} 	

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





