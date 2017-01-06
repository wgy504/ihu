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
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
extern CRC_HandleTypeDef hcrc;
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
extern CRC_HandleTypeDef hcrc;
#endif


//本地全局变量



/* 函数体 --------------------------------------------------------------------*/

uint32_t ihu_bsp_stm32_crc_caculate(uint32_t *inputData, uint16_t len)
{
	return HAL_CRC_Accumulate(&IHU_BSP_STM32_CRC_HANDLER, (uint32_t *)inputData, len);
}


