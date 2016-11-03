/**
  ******************************************************************************
  * 文件名程: bsp_led.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 基于HAL库的板载LED灯底层驱动实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/**
  * 很多初学者可能会疑惑bsp_led.c和bsp_led.h这两个文件哪里来的？？是不是官方下载
  * 的？？实际上，这两个文件是要求我们自己创建的，内容也是我们自己敲代码的。
  * 这个已经属于应用层次的代码ST官方是不提供的，需要我们根据自己的开发板硬件来
  * 编写我们的程序，所以需要我们自己新建文件编写。
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_led.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 板载LED灯IO引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_led.h
  *           文件相关宏定义就可以方便修改引脚。
  */
void LED_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 使能(开启)LED引脚对应IO端口时钟 */  
  LED1_RCC_CLK_ENABLE();
  LED2_RCC_CLK_ENABLE();
  LED3_RCC_CLK_ENABLE();
  
  /* 配置LED1引脚输出电压 */
  HAL_GPIO_WritePin(LED1_GPIO, LED1_GPIO_PIN, GPIO_PIN_RESET);

  /* 配置LED2引脚输出电压 */
  HAL_GPIO_WritePin(LED2_GPIO, LED2_GPIO_PIN, GPIO_PIN_RESET);
  /* 配置LED3引脚输出电压 */
  HAL_GPIO_WritePin(LED2_GPIO, LED3_GPIO_PIN, GPIO_PIN_RESET);
  
  /* 设定LED1对应引脚IO编号 */
  GPIO_InitStruct.Pin = LED1_GPIO_PIN;
  /* 设定LED1对应引脚IO为输出模式 */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  /* 设定LED1对应引脚IO操作速度 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  /* 初始化LED1对应引脚IO */
  HAL_GPIO_Init(LED1_GPIO, &GPIO_InitStruct);
  
  /* 设定LED2对应引脚IO编号 */
  GPIO_InitStruct.Pin = LED2_GPIO_PIN;
  /* 设定LED2对应引脚IO为输出模式 */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  /* 设定LED2对应引脚IO操作速度 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  /* 初始化LED2对应引脚IO */
  HAL_GPIO_Init(LED2_GPIO, &GPIO_InitStruct);
  
  /* 设定LED3对应引脚IO编号 */
  GPIO_InitStruct.Pin = LED3_GPIO_PIN;
  /* 设定LED3对应引脚IO为输出模式 */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  /* 设定LED3对应引脚IO操作速度 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  /* 初始化LED3对应引脚IO */
  HAL_GPIO_Init(LED3_GPIO, &GPIO_InitStruct);
  
}

/**
  * 函数功能: 设置板载LED灯的状态
  * 输入参数: LEDx:其中x可甚至为(1,2,3)用来选择对应的LED灯
  * 输入参数：state:设置LED灯的输出状态。
  *             可选值：LED_OFF：LED灯灭；
  *                     LED_ON： LED灯亮。
  *                     LED_TOGGLE：反转LED
  * 返 回 值: 无
  * 说    明：该函数使用类似标准库函数的编程方法，方便理解标准库函数编程思想。
  */
void LEDx_StateSet(uint8_t LEDx,LEDState_TypeDef state)
{
  /* 检查输入参数是否合法 */
  assert_param(IS_LED_TYPEDEF(LEDx));
  assert_param(IS_LED_STATE(state));
  
  /* 判断设置的LED灯状态，如果设置为LED灯灭 */
  if(state==LED_OFF)
  {
    if(LEDx & LED1)            
      LED1_OFF;/* LED1灭 */
    
    if(LEDx & LED2)
      LED2_OFF;/* LED2灭 */
   
    if(LEDx & LED3)
      LED3_OFF;/* LED3灭 */    
  }
  else if(state==LED_ON) /* 设置LED灯为亮 */
  {
    if(LEDx & LED1)
      LED1_ON;/* LED1亮 */
    
    if(LEDx & LED2)
      LED2_ON;/* LED2亮 */
    
    if(LEDx & LED3)
      LED3_ON;/* LED3亮 */ 
  }
  else
  {
    if(LEDx & LED1)
      LED1_TOGGLE;/* 设置引脚输出反转 */
    
    if(LEDx & LED2)
      LED2_TOGGLE;/* 设置引脚输出反转 */
    
    if(LEDx & LED3)
      LED3_TOGGLE;/* 设置引脚输出反转 */ 
  }
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:          led_init
* Description:       ST-M3-LITE-205RG精简开发板有1个引脚连接了LED发光二极管，
                     GPIOC.2--LED1，该函数的作用是对其各引脚进行初始化。
* Input:             none
* Output:            none
* Return:            none
* Created by:        alvan
* Created date:      2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
//void LED_F205_board_init(void)
//{ 	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	LED_F205RG_BOARD_CLK_ENABLE();    /* 使能端口PORTC时钟                                  */  	 
//	GPIO_InitStructure.GPIO_Pin    = LED_F205_BOARD_PIN;               
//	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
//	GPIO_Init(LED_F205_BOARD_PORT, &GPIO_InitStructure);                /* 配置引脚GPIOC.2为推挽输出,IO最大速度50M            */
//	GPIO_SetBits(LED_F205_BOARD_PORT, LED_F205_BOARD_PIN);   
//	
//	# if 0    /* IO 测试   */


//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
//								 RCC_AHB1Periph_GPIOB |
//								 RCC_AHB1Periph_GPIOC | 
//								 RCC_AHB1Periph_GPIOD , ENABLE);    /* 使能端口时钟                                     */  	 

//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
//														GPIO_Pin_1  |
//														GPIO_Pin_2  |
//														GPIO_Pin_3  |
//														GPIO_Pin_4  |
//														GPIO_Pin_5  |
//														GPIO_Pin_6  |
//														GPIO_Pin_7  |
//														GPIO_Pin_8  |
//														GPIO_Pin_9  |
//														GPIO_Pin_10 |
//														GPIO_Pin_11 |
//														GPIO_Pin_12;																		
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);                   /* 配置引脚GPIOA.0~GPIOA.12为推挽输出,IO最大速度50M  */

//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
//														GPIO_Pin_1  |
//														GPIO_Pin_2  |
//														GPIO_Pin_5  |
//														GPIO_Pin_6  |
//														GPIO_Pin_7  |
//														GPIO_Pin_8  |
//														GPIO_Pin_9  |
//														GPIO_Pin_10 |
//														GPIO_Pin_11 | 
//														GPIO_Pin_12 |
//														GPIO_Pin_13 |
//														GPIO_Pin_14 |
//														GPIO_Pin_15;																		
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);                   	

//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
//														GPIO_Pin_1  |
//														GPIO_Pin_2  |
//														GPIO_Pin_3  |																		
//														GPIO_Pin_4  |
//														GPIO_Pin_5  |
//														GPIO_Pin_6  |
//														GPIO_Pin_7  |
//														GPIO_Pin_8  |
//														GPIO_Pin_9  |
//														GPIO_Pin_10 |
//														GPIO_Pin_11 | 
//														GPIO_Pin_12 |
//														GPIO_Pin_13;																		
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);   

//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2; 
//																														
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);	

//	GPIOA->ODR &= 0xE000;                       
//	GPIOB->ODR &= 0x0018;
//	GPIOC->ODR &= 0xC000;                      
//	GPIOD->ODR &= 0xFFFB;                       /* 对应的IO输出低                                     */

//	GPIOA->ODR |= ~0xE000;                       
//	GPIOB->ODR |= ~0x0018;
//	GPIOC->ODR |= ~0xC000;                      
//	GPIOD->ODR |= ~0xFFFB;                      /* 对应的IO输出高                                     */		

//	#endif    
//}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_on
* Description:      LED全亮
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void LED_F205_board_on(void)
{
    LED_F205RG_board_on();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_off
* Description:      LED全灭
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   LED_F205_board_off (void)
{
    LED_F205RG_board_off();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_negation
* Description:      LED取反闪烁
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   LED_F205_board_negation (void)
{
    LED_F205RG_board_negation();
}

//void LED_F205_timer_ind_init(void)
//{ 	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	LED_F205RG_TIMER_IND_CLK_ENABLE();    															/* 使能端口PORTC时钟                                  */  	 
//	GPIO_InitStructure.GPIO_Pin    = LED_F205_TIMER_IND_PIN;               
//	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
//	GPIO_Init(LED_F205_TIMER_IND_PORT, &GPIO_InitStructure);                /* 配置引脚GPIOC.2为推挽输出,IO最大速度50M            */
//	GPIO_SetBits(LED_F205_TIMER_IND_PORT, LED_F205_TIMER_IND_PIN);   
//	
//	# if 0    /* IO 测试   */


//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
//								 RCC_AHB1Periph_GPIOB |
//								 RCC_AHB1Periph_GPIOC | 
//								 RCC_AHB1Periph_GPIOD , ENABLE);    /* 使能端口时钟                                     */  	 

//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
//														GPIO_Pin_1  |
//														GPIO_Pin_2  |
//														GPIO_Pin_3  |
//														GPIO_Pin_4  |
//														GPIO_Pin_5  |
//														GPIO_Pin_6  |
//														GPIO_Pin_7  |
//														GPIO_Pin_8  |
//														GPIO_Pin_9  |
//														GPIO_Pin_10 |
//														GPIO_Pin_11 |
//														GPIO_Pin_12;																		
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);                   /* 配置引脚GPIOA.0~GPIOA.12为推挽输出,IO最大速度50M  */

//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
//														GPIO_Pin_1  |
//														GPIO_Pin_2  |
//														GPIO_Pin_5  |
//														GPIO_Pin_6  |
//														GPIO_Pin_7  |
//														GPIO_Pin_8  |
//														GPIO_Pin_9  |
//														GPIO_Pin_10 |
//														GPIO_Pin_11 | 
//														GPIO_Pin_12 |
//														GPIO_Pin_13 |
//														GPIO_Pin_14 |
//														GPIO_Pin_15;																		
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);                   	

//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |
//														GPIO_Pin_1  |
//														GPIO_Pin_2  |
//														GPIO_Pin_3  |																		
//														GPIO_Pin_4  |
//														GPIO_Pin_5  |
//														GPIO_Pin_6  |
//														GPIO_Pin_7  |
//														GPIO_Pin_8  |
//														GPIO_Pin_9  |
//														GPIO_Pin_10 |
//														GPIO_Pin_11 | 
//														GPIO_Pin_12 |
//														GPIO_Pin_13;																		
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);   

//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2; 
//																														
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);	

//	GPIOA->ODR &= 0xE000;                       
//	GPIOB->ODR &= 0x0018;
//	GPIOC->ODR &= 0xC000;                      
//	GPIOD->ODR &= 0xFFFB;                       /* 对应的IO输出低                                     */

//	GPIOA->ODR |= ~0xE000;                       
//	GPIOB->ODR |= ~0x0018;
//	GPIOC->ODR |= ~0xC000;                      
//	GPIOD->ODR |= ~0xFFFB;                      /* 对应的IO输出高                                     */		

//	#endif    
//}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_on
* Description:      LED全亮
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void  LED_F205_timer_ind_on (void)
{
    LED_F205RG_timer_ind_on();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_off
* Description:      LED全灭
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   LED_F205_timer_ind_off (void)
{
    LED_F205RG_timer_ind_off();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         led_all_negation
* Description:      LED取反闪烁
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan 
* Created date:     2014-7-29
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/

void   LED_F205_timer_ind_negation (void)
{
	LED_F205RG_timer_ind_negation();
}


void LED_F205_onoff(GPIO_TypeDef* port, uint16_t pin, uint8_t onoff)
{
	LED_F205RG_onoff(port, pin, onoff);
}


