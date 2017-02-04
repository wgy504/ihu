#ifndef __BSP_STM32_SOC_FLASH_H
#define __BSP_STM32_SOC_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
#include "stm32f2xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "sysdim.h"
#include "vmfreeoslayer.h"
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#include "commsgccl.h"
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#include "commsgbfsc.h"
#else
	#error Un-correct constant definition
#endif

//不能在这里出现管脚的任何配置和初始化，必须在STM32CubeMX中完成，这里使用STM32CubeMX给出的端口俗名
	

/* 类型定义 ------------------------------------------------------------------*/
/************************** STM32 内部 FLASH 配置 *****************************/
#define STM32_FLASH_SIZE        512  // 所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN        1    // stm32芯片内容FLASH 写入使能(0，禁用;1，使能)

//STM32固定位置的标签存储内容，由烧录器写入
//typedef struct StrStm32F2EquidStorage
//{
//	char  		equLable[20];
//	uint16_t 	equType;
//	uint16_t 	hwId;
//	uint16_t 	swRelId;
//	uint16_t 	swVerId;
//	uint8_t  	swUpgradeFlag;
//	uint8_t 	rsv1;
//	uint8_t 	rsv2;
//	uint8_t 	rsv3;	
//}StrStm32F2EquidStorage_t;

#define IHU_BSP_STM32_F2_FLASH_EQU_STORAGE_ADD_FIX 	0x80001FC0 //根据具体的芯片型号，由uboot确定后再确定
#define IHU_BSP_STM32_F2_FLASH_EQU_STORAGE_SIZE 		IHU_HW_BURN_PHY_ID_BLOCK_LEN      //64BYTE

//本地定义的交换矩阵


//全局函数
//StrStm32F2EquidStorage_t的定义权限被拿到VMLAYER/SYSENGPAR中去了，以简化整个程序的处理过程
extern void ihu_bsp_stm32_f2board_equid_get(uint8_t *equ);

//Local APIs
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		  //读出半字

void STMFLASH_WriteLenByte(uint32_t WriteAddr, uint32_t DataToWrite, uint16_t Len );	      //指定地址开始写入指定长度的数据
uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr, uint16_t Len );					                    	//指定地址开始读取指定长度数据
void STMFLASH_Write( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );		//从指定地址开始写入指定长度的数据
void STMFLASH_Read( uint32_t ReadAddr, uint16_t * pBuffer, uint16_t NumToRead );   	//从指定地址开始读出指定长度的数据
void Test_Write( uint32_t WriteAddr, uint16_t WriteData );                        //测试写入		



#ifdef __cplusplus
}
#endif
#endif /* __BSP_STM32_SOC_FLASH_H */
