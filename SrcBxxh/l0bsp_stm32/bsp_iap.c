/**
  ******************************************************************************
  * 文件名程: bsp_iap.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 
  ******************************************************************************
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_iap.h"
#include "bsp_stmflash.h"


//从MAIN.x中继承过来的函数


//本地全局变量
/*
IAP目前遇到的问题，问题远比理解的多
===================================
1. FLASH使用哪一个？
2. FTP用不用？
3. 还是使用HTTP分段？
4. 要不要文件系统？
5. F103的FLASH访问函数，需要移植到F207
6. FLASH程序的起始地址，为什么是0x8010000? F207是一样的？
7. 内存起始地址，为什么是0x20001000? F207是一样的？为什么要强制将接收缓冲区放在0x20001000?
8. 如何确定FLASH已经被使用了多少空间？哪儿开始是空闲的？
9. 接收到的ELF/BIN文件，如何拷贝到起始地址才算正确的程序结构？没有任何头部矢量场？
10. 直接跳转到新的APP/FLASH PROGRAM地址进行执行，系统如何RESET以后，下次还会从这儿启动，还是又复原了？
11. 如何实现双启动？
12. 如何程序启动不正确，如何判定是由于第二个镜像启动造成的？
13. 官方例子采用bootloader的方式。我们是否可以将这个程序做成固定启动例子，每一次系统启动，都进行检查，然后再执行？是否可以将它做成SHELL，然后
    通过它来执行和选择，到底是否需要升级，是否需要下载升级新的程序？
14. 如果是低功耗版本，设备将进入正常的业务APP进行工作，那程序如何判定需要升级？跟后台相连后将程序下载下来，然后REBOOT自己，激活bootloader的方式？


*/



/* 私有变量 ------------------------------------------------------------------*/
#if defined ( __CC_ARM )  // 使用Keil编译环境

struct STRUCT_IAP_RECIEVE strAppBin  __attribute__((at(0x20001000)))={{0},0};

#elif defined ( __ICCARM__ ) // 使用IAR编译环境

struct STRUCT_IAP_RECIEVE strAppBin;//={{0},0};

#endif


static uint16_t ulBuf_Flash_App[1024];

/* 函数体 --------------------------------------------------------------------*/
void IAP_Write_App_Bin ( uint32_t ulStartAddr, uint8_t * pBin_DataBuf, uint32_t ulBufLength )
{
	uint16_t us, usCtr=0, usTemp;
	uint32_t ulAdd_Write = ulStartAddr;                                //当前写入的地址
	uint8_t * pData = pBin_DataBuf;
	
	for ( us = 0; us < ulBufLength; us += 2 )
	{						    
		usTemp =  ( uint16_t ) pData[1]<<8;
		usTemp += ( uint16_t ) pData[0];	  
		pData += 2;                                                      //偏移2个字节
		ulBuf_Flash_App [ usCtr ++ ] = usTemp;	    
		if ( usCtr == 1024 )
		{
			usCtr = 0;
			STMFLASH_Write ( ulAdd_Write, ulBuf_Flash_App, 1024 );	
			ulAdd_Write += 2048;                                           //偏移2048  16=2*8.所以要乘以2.
		}
	}
	if ( usCtr ) 
    STMFLASH_Write ( ulAdd_Write, ulBuf_Flash_App, usCtr );//将最后的一些内容字节写进去.  
}


#if defined ( __CC_ARM )  // 使用Keil编译环境

__asm void MSR_MSP ( uint32_t ulAddr ) 
{
    MSR MSP, r0 			                   //set Main Stack value
    BX r14
}

#elif defined ( __ICCARM__ ) // 使用IAR编译环境

void MSR_MSP ( uint32_t ulAddr ) 
{
    asm("MSR MSP, r0"); 			                   //set Main Stack value
    asm("BX r14");
}


#endif

//跳转到应用程序段
//ulAddr_App:用户代码起始地址.
void IAP_ExecuteApp ( uint32_t ulAddr_App )
{
	pIapFun_TypeDef pJump2App; 
	
	if ( ( ( * ( __IO uint32_t * ) ulAddr_App ) & 0x2FFE0000 ) == 0x20000000 )	  //检查栈顶地址是否合法.
	{ 
		pJump2App = ( pIapFun_TypeDef ) * ( __IO uint32_t * ) ( ulAddr_App + 4 );	//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP( * ( __IO uint32_t * ) ulAddr_App );					                            //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		pJump2App ();								                                    	//跳转到APP.
	}
}		

/* 
** ===========================================
** Merge from LI Hao C after test, 2017/07/21
** ===========================================
*/
uint32_t AppAddress;
typedef  void (*pFunction)(void);

void IAP_ExecuteApp_fun ( uint32_t ulAddr_App )
{
//taskENTER_CRITICAL();
//taskDISABLE_INTERRUPTS();
#if 1
    pFunction App_Run;
	  AppAddress = *(__IO uint32_t*) (ulAddr_App + 4);

	  App_Run = (pFunction)AppAddress;

	  __set_MSP(*(__IO uint32_t*) ulAddr_App);
	
//	  osDelay(1000);
//	  printf("\n\r Start to Jump to IAP program, IAP addr = %x\n\r", ulAddr_App);
	  App_Run();
#endif
//taskEXIT_CRITICAL();
//taskENABLE_INTERRUPTS();

}

void IAP_write_app_bin_all_in_one(void)
{
	IAP_Write_App_Bin(APP_START_ADDR, strAppBin.ucDataBuf, strAppBin.usLength );
}

void IAP_execute_app_bin_all_in_one(void)
{
	IAP_ExecuteApp(APP_START_ADDR);
}

/*
** ========================================
*/
OPSTAT ihu_bsp_stm32_iap_bootcfg_read_all(SysEngParElementHwBurnPhyIdAddr_t *buf)
{
	return SUCCESS;
}

OPSTAT ihu_bsp_stm32_iap_bootcfg_write_all(SysEngParElementHwBurnPhyIdAddr_t *buf)
{
	return SUCCESS;
}

uint8_t ihu_bsp_stm32_iap_bootcfg_read_boot_flag(void)
{
	return 0;
}

OPSTAT ihu_bsp_stm32_iap_bootcfg_write_boot_flag(uint8_t flag)
{
	return SUCCESS;
}

OPSTAT ihu_bsp_stm32_iap_bootcfg_update_swid (uint16_t relid, uint16_t verid)
{
	return SUCCESS;
}

OPSTAT ihu_bsp_stm32_iap_bootcfg_update_boot_index (uint8_t index)
{
	return SUCCESS;
}

OPSTAT ihu_bsp_stm32_iap_bootcfg_update_boot_load (uint8_t index, uint32_t addr, uint16_t relid, uint16_t verid, uint16_t checksum, uint16_t validflag)
{
	return SUCCESS;
}

bool ihu_bsp_stm32_iap_check_load_max_len(uint32_t len)
{
	return SUCCESS;
}

OPSTAT ihu_bsp_stm32_iap_write_load_segment(uint8_t loadindex, uint16_t segId, uint8_t *buf)
{
	return SUCCESS;
}

OPSTAT ihu_bsp_stm32_iap_switch_to_app(void)
{
	return SUCCESS;
}

OPSTAT ihu_bsp_stm32_iap_switch_to_iap(void)
{
	return SUCCESS;
}

