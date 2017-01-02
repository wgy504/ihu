/**
  ******************************************************************************
  * 文件名程: bsp_wdog.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 
  ******************************************************************************
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_wdog.h" 

//从MAIN.x中继承过来的函数
#if (IHU_WATCH_DOG_SET_ENABLE_OR_DISABLE == IHU_WATCH_DOG_ENABLE)
extern IWDG_HandleTypeDef hiwdg;
#endif

//本地全局变量


/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 独立看门狗初始化配置
  * 输入参数: Tout = prv/40 * rlv (s) prv可以是[4,8,16,32,64,128,256]
  *            prv:预分频器值，取值如下：
  *            参数 IWDG_PRESCALER_4: IWDG prescaler set to 4
  *            参数 IWDG_PRESCALER_8: IWDG prescaler set to 8
  *            参数 IWDG_PRESCALER_16: IWDG prescaler set to 16
  *            参数 IWDG_PRESCALER_32: IWDG prescaler set to 32
  *            参数 IWDG_PRESCALER_64: IWDG prescaler set to 64
  *            参数 IWDG_PRESCALER_128: IWDG prescaler set to 128
  *            参数 IWDG_PRESCALER_256: IWDG prescaler set to 256
  *
  *            rlv:预分频器值，取值范围为：0-0XFFF
  * 返 回 值: 无
  * 说    明：函数调用举例：
  *           IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s 超时溢出
  */

// =>2017/1/1研究结果：
// 时钟长度是这么设置的：分频器设置为40KHz，则绝对超时长度 = 分频率/40K*相对超时长度。
// 比如，分频率=128， 相对超时长度= 3125 = 0xC35，则真正的绝对超时长度 = 128/40K*3125 = 10秒
// 如果LSI支持256分频，则最长的长度 = 256/40K * 0xFFF = 26秒
// 想要支持20秒的绝对超时时长，则LSI = 256，相对时长 = 0xC35 (3125)
// 40KHz的LSI分频频率，不可以改变。

int ihu_bsp_stm32_watch_dog_start(void)
{
	/* 
	  开始执行启动任务主函数前使能独立看门狗。
	  设置LSI是128分频，下面函数参数范围0-0xFFF，分别代表最小值3.2ms和最大值13107.2ms
	  下面设置的是10s，如果10s内没有喂狗，系统复位。
	*/
	//Min-max timeout value @32KHz (LSI): ~125us / ~32.7s，所以各个任务的HEART-BEAT周期需要改进到10S以内，并让本周期取20s，以确保至少又一次落在	
	//LSI到底该如何设置，喂狗的定时器能否增长到3分钟，待研究。从目前研究的情形难看，最长只能支持到26秒。
	//这个配置与选择已经被STM32CubeMX完成了，这里不再需要重新配置
	
//  MX_IWDG_Init(IWDG_PRESCALER_256,0xC35);
//  /* 启动独立看门狗 */
	//最新版本的看门狗函数中，启动已经自动完成了，所以只剩下REFRESH就可以了
	//HAL_IWDG_Start(&IHU_BSP_STM32_WATCH_DOG_HANDLER);
	
	return BSP_SUCCESS;
}

//因为系统配置中WATCH DOG没有打开，导致这里编译不通过，暂时注释掉
int ihu_bsp_stm32_watch_dog_refresh(void)
{   
#if (IHU_WATCH_DOG_SET_ENABLE_OR_DISABLE == IHU_WATCH_DOG_ENABLE)
	HAL_IWDG_Refresh(&IHU_BSP_STM32_WATCH_DOG_HANDLER);
#endif
	return BSP_SUCCESS;
}

