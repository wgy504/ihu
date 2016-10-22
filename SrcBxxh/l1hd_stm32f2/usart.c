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
	
vu8 Usart1_R_Buff[USART1_REC_MAXLEN];	//串口1数据接收缓冲区 
vu8 Usart1_R_State=0;					//串口1接收状态
vu16 Usart1_R_Count=0;					//当前接收数据的字节数 	  

/*******************************************************************************
* 函数名  : USART1_Init_Config
* 描述    : USART1初始化配置
* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void USART1_Init_Config(u32 bound)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	/*使能USART1和GPIOA外设时钟*/  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_AHB1Periph_GPIOC, ENABLE);	

	/*复位串口1*/
 	USART_DeInit(USART1);  
	
	/*USART1_GPIO初始化设置*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//USART1_TXD(PA.9)     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//调用库函数中的GPIO初始化函数，初始化USART1_TXD(PA.9)  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//USART1_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化USART1_RXD(PA.10)


  /*USART1 初始化设置*/
	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
	USART_Init(USART1, &USART_InitStructure);										//初始化串口1

  /*Usart1 NVIC配置*/
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//从优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器 
	  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//使能串口1接收中断

  USART_Cmd(USART1, ENABLE);                    			//使能串口 
	USART_ClearFlag(USART1, USART_FLAG_TC);					//清除发送完成标志
}


/*******************************************************************************
* 函数名  : USART_SendData
* 描述    : USART1发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void USART1_SendData(u8* buff, u16 len)
{    
	u16 i;

	for(i=0; i<len; i++)  
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
	    USART_SendData(USART1 ,buff[i]);   
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
	u8 Res=0;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1); //读取接收到的数据(USART1->DR)
		
		Usart1_R_Buff[Usart1_R_Count++] = Res;
		if(Usart1_R_State == 0)//数据接收未完成
		{
			if(Res == 0x0d)//接收到0x0d,下一个字节接收到0x0a则接收完成
			{
				Usart1_R_State =2;
			}
		}
		else if(Usart1_R_State == 2)
		{
			if(Res == 0x0a)//上一个字节接收到0x0d,这个字节接收到oxoa则接收完成
			{
				Usart1_R_State =1;//数据接收完成
			}
			else//接收错误
			{
				Usart1_R_State =0;
				Usart1_R_Count =0;
			}
		}
		if(Usart1_R_Count >= USART1_REC_MAXLEN)//接收数据长度走出接收数据缓冲区
		{
			if((Usart1_R_Buff[USART1_REC_MAXLEN-2] != 0x0d) || (Usart1_R_Buff[USART1_REC_MAXLEN-1] != 0x0a))
			{
				Usart1_R_Count =0;
				Usart1_R_State =0;
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
 USART_SendData(USART1, (uint8_t) ch);

 /* 循环直到发送完成 */
 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

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
 while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

 return (int)USART_ReceiveData(USART1);
}	
