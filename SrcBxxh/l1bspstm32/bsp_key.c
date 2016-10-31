/**
  ******************************************************************************
  * 文件名程: bsp_key.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 板载独立按键底层驱动函数
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_key.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/***************************************用户自定义函数**************************/
/**
  * 函数功能: 板载按键IO引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_key.h
  *           文件相关宏定义就可以方便修改引脚。
  */
void KEY_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 使能(开启)KEY引脚对应IO端口时钟 */  
  KEY1_RCC_CLK_ENABLE();
  KEY2_RCC_CLK_ENABLE();
  
  /* 配置KEY1 GPIO:输入下拉模式 */
  GPIO_InitStruct.Pin = KEY1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KEY1_GPIO, &GPIO_InitStruct);  
  
  /* 配置KEY2 GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = KEY2_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY2_GPIO, &GPIO_InitStruct);

}

/**
  * 函数功能: 按键1状态获取函数.
  * 输入参数: 无
  * 返 回 值: 如果按键处于被按下状态的电平，返回KEY_ON。
  *           如果按键处于弹起状态的电平，返回KEY_OFF。
  * 说    明：无
  */
uint8_t GetPinStateOfKey1(void)
{
	if(HAL_GPIO_ReadPin(KEY1_GPIO,KEY1_GPIO_PIN)==KEY1_DOWNSTATE)
	{
			return KEY_ON;
	}
	else
	{
			return KEY_OFF;
	}
}

/**
  * 函数功能: 按键2状态获取函数.
  * 输入参数: 无
  * 返 回 值: 如果按键处于被按下状态的电平，返回KEY_ON。
  *           如果按键处于弹起状态的电平，返回KEY_OFF。
  * 说    明：无
  */
uint8_t GetPinStateOfKey2(void)
{
	if(HAL_GPIO_ReadPin(KEY2_GPIO,KEY2_GPIO_PIN)==KEY2_DOWNSTATE)
	{
			return KEY_ON;
	}
	else
	{
			return KEY_OFF;
	}
}

/***************************************用户自定义函数结束***********************************/



/*
************************************************************************************************************************
*                              消抖延时函数
*
* 函数描述：用户如果没有定时调用，即没有定义KEY_FIXED_PERIOD，就需要重新根据CPU重写此函数。
*
*     参数：xms        根据不同按键消抖的时间，一般是20ms
*
*   返回值：无
*
*     注意：消抖延时
*
************************************************************************************************************************
*/
#ifndef  KEY_FIXED_PERIOD
void key_delay1ms(uint8_t xms)
{
    
}
#endif


/*					以下内容不需要更改，直接使用即可								*/
/*
************************************************************************************************************************
*                              按键创建函数
*
* 函数描述：使用按键前需要创建按键。
*
*     参数：p_Key        指向按键控制变量的指针
*           p_CallBack   用来获取按键电平的回调函数指针，因为不同CPU电平获取的方式不一样，这部分需要用户自己编写
*                        伪代码如下：
*
*                          uint8_t GetPinStateOfKeyXX(void)
*                          {
*                            如果管脚的电平是按键按下时的电平
*                            返回1
*
*                            如果管脚的电平是按键弹起时的电平
*                            返回0
*                          }
*
*   返回值：如果按键处于被按下状态的电平，返回1。
*           如果按键处于弹起状态的电平，返回0。
*
************************************************************************************************************************
*/
void KeyCreate(KEY *p_Key,KEY_CALLBACK_PTR p_CallBack)
{
  p_Key->GetStatePtr=p_CallBack;
  p_Key->Times=0;
  p_Key->State=KEY_UP;
  p_Key->StateChange=NOT_CHANGED;
#ifdef KEY_FIXED_PERIOD
	p_Key->Time_ms=0;          //消抖时间初始化为0
#endif
  
#if USER_DATA_EN>0             
	p_Key->User_Data=0;		           //用户变量，可由用户任意使用
#endif
  
}

/*
************************************************************************************************************************
*                              按键状态更新函数
*
* 函数描述：按键驱动函数，只适合STM32单片机库开发，其他类型的单片机可以自行修改。
*
*     参数：p_Key        指向按键控制变量的指针
*
*   返回值：无
*
*     注意：调用频率需要大于20Hz
*
************************************************************************************************************************
*/
void Key_RefreshState(KEY* p_Key)
{
  switch(p_Key->State)
  {
    case KEY_UP:
    {
      if((*(p_Key->GetStatePtr))())//执行回调函数判断按键管脚状态
      {
#ifdef  KEY_FIXED_PERIOD   
        p_Key->Time_ms = 0;
        p_Key->State = KEY_DOWN_WOBBLE;//进行消抖延时
#else
        p_Key->State = KEY_DOWN_WOBBLE;
        key_delay1ms(KEY_WOBBLE_TIME);				
        if((*(p_Key->GetStatePtr))())
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_DOWN;
        }
#endif
      }
    }break;
    
    #ifdef  KEY_FIXED_PERIOD
    case KEY_DOWN_WOBBLE:
    {
      p_Key->Time_ms += KEY_FIXED_PERIOD;
      if( p_Key->Time_ms >=KEY_WOBBLE_TIME )
      {
        if((*(p_Key->GetStatePtr))())
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_DOWN;
        }
        else
        {
          p_Key->State = KEY_UP;
        }
      }
    }break;
    #endif

    case KEY_DOWN:
    {
      if( (*(p_Key->GetStatePtr))() == 0 )
      {
#ifdef  KEY_FIXED_PERIOD
        p_Key->Time_ms = 0;
        p_Key->State = KEY_UP_WOBBLE;//进行消抖延时
#else
        key_delay1ms(KEY_WOBBLE_TIME);
        if( (*(p_Key->GetStatePtr))() == 0 )
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_UP;
          p_Key->Times++;
          if( p_Key->Times > 250)
            p_Key->Times = 250;//最多允许按下250次没处理
        }
#endif
      }
    }break;

#ifdef  KEY_FIXED_PERIOD
    case KEY_UP_WOBBLE:
    {
      p_Key->Time_ms += KEY_FIXED_PERIOD;
      if( p_Key->Time_ms >= KEY_WOBBLE_TIME )
      {
        if( (*(p_Key->GetStatePtr))()==0)
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_UP;
          p_Key->Times++;
          if( p_Key->Times > 250)
            p_Key->Times = 250;//最多允许按下250次没处理
        }
        else
        {
          p_Key->State = KEY_DOWN;
        }
      }
    }break;
#endif
  }
}

/*
************************************************************************************************************************
*                              按键被按下次数访问函数
*
* 函数描述：对按键被按下次数的访问
*
*     参数：p_Key        指向按键控制变量的指针
*           option	     选项，由于按键操作一般只用到一下两种操作，所以其他的就没有包括进来
*                        KEY_ACCESS_READ         读取按键被按下的次数
*                        KEY_ACCESS_WRITE_CLEAR  清零按键被按下的次数
*   返回值：->times的值。
*
*     注意：访问过程没有用信号量保护，如果在同一个任务的上下调用Key_AccessTimes和Key_RefreshState
*           是没有什么关系的，而在系统中如果两个函数在两个任务当中就要加以临界段保护了，不过两个函数
*           一般都要在一个任务中使用，因为每次调用Key_RefreshState，Times就有可能被更新，跟在后面
*           查询最好
*
************************************************************************************************************************
*/

uint8_t Key_AccessTimes(KEY* p_Key,ACCESS_TYPE opt)
{
	uint8_t times_temp;
	
	if((opt&KEY_ACCESS_WRITE_CLEAR) == KEY_ACCESS_WRITE_CLEAR)
	{
			p_Key->Times = 0;					
	}
		
	times_temp = p_Key->Times;
	
	return times_temp;
}

/*
************************************************************************************************************************
*                              按键状态访问函数
*
* 函数描述：对按键被按下状态的访问
*
*     参数：p_Key        指向按键控制变量的指针

*   返回值：->State的值。
*
*     注意：1.访问过程没有用信号量保护，如果在同一个任务的上下调用Key_AccessTimes和Key_RefreshState
*           是没有什么关系的，而在系统中如果两个函数在两个任务当中就要加以临界段保护了，不过两个函数
*           一般都要在一个任务中使用，因为每次调用Key_RefreshState，Times就有可能被更新，跟在后面
*           查询最好
************************************************************************************************************************
*/
uint8_t Key_AccessState(KEY* p_Key,KEY_STATE *p_State)
{
  uint8_t StateChange=p_Key->StateChange;
  //读取状态
  *p_State = p_Key->State;
  //读取后更新状态是否被改变
  p_Key->StateChange=NOT_CHANGED;
  //返回状态是否改变
  return StateChange;
}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
