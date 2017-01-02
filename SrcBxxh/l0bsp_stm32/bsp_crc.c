/**
  ******************************************************************************
  * 文件名程: bsp_crc.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 
  ******************************************************************************
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_crc.h" 

//从MAIN.x中继承过来的函数
#if (IHU_CRC_SET_ENABLE_OR_DISABLE == IHU_HARDWARE_CRC_ENABLE)
extern CRC_HandleTypeDef hcrc;
#endif


//本地全局变量



/* 函数体 --------------------------------------------------------------------*/

uint32_t ihu_bsp_stm32_crc_caculate(uint32_t *inputData, uint16_t len)
{
#if (IHU_CRC_SET_ENABLE_OR_DISABLE == IHU_HARDWARE_CRC_ENABLE)	
	return HAL_CRC_Accumulate(&IHU_BSP_STM32_CRC_HANDLER, (uint32_t *)inputData, len);
#else
	return 0;
#endif
}


