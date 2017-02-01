/**
 ****************************************************************************************
 *
 * @file mod_rfid.h
 *
 * @brief RFID module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */

#ifndef L1FREERTOS_MOD_RFID_H_
#define L1FREERTOS_MOD_RFID_H_

#include "stm32f2xx_hal.h"
#include "vmfreeoslayer.h"
#include "bsp_usart.h"
#include "bsp_spi.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

//全局使用的常量定义
#define IHU_VMWM_RFIDMOD_USING_ITF_SPS_RFID 1
#define IHU_VMWM_RFIDMOD_USING_ITF_SPI1 2
#define IHU_VMWM_RFIDMOD_USING_ITF_SPI2 3
#define IHU_VMWM_RFIDMOD_USING_ITF_SET  IHU_VMWM_RFIDMOD_USING_ITF_SPI2  //CCL项目使用的接口为SPI2，BSP中有映射

//定义RFID模块扫描的时间长度，暂时放到4秒，未来将根据实际表现进行合适的调整
#define IHU_VMWM_RFIDMOD_SCAN_RC522_MAX_TIME 4

//向上提供全局统一服务的入口
extern OPSTAT ihu_vmmw_rfidmod_rc522_spi_send_command(uint8_t *command);
extern OPSTAT ihu_vmmw_rfidmod_rc522_spi_read_id(uint8_t *rfidAddr, uint8_t len);

//工作调用流
OPSTAT func_rfidmod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
OPSTAT func_rfidmod_wait_command_fb(uint8_t *ack, uint16_t wait_time);
void func_rfidmod_clear_receive_buffer(void);
void func_rfidmod_send_string(char* s);
#define func_rfidmod_send_LR() func_rfidmod_send_string("\r\n");

//高级定义，简化程序的可读性
#define IHU_ERROR_PRINT_RFIDMOD	zIhuRunErrCnt[TASK_ID_VMFO]++; IhuErrorPrint



/* 宏定义 --------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////
//MF522命令字
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_AUTHENT           0x0E               //验证密钥
#define PCD_RECEIVE           0x08               //接收数据
#define PCD_TRANSMIT          0x04               //发送数据
#define PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define PCD_RESETPHASE        0x0F               //复位
#define PCD_CALCCRC           0x03               //CRC计算

/////////////////////////////////////////////////////////////////////
//Mifare_One卡片命令字
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_AUTHENT1A        0x60               //验证A密钥
#define PICC_AUTHENT1B        0x61               //验证B密钥
#define PICC_READ             0x30               //读块
#define PICC_WRITE            0xA0               //写块
#define PICC_DECREMENT        0xC0               //扣款
#define PICC_INCREMENT        0xC1               //充值
#define PICC_RESTORE          0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define PICC_HALT             0x50               //休眠

/////////////////////////////////////////////////////////////////////
//MF522 FIFO长度定义
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte
#define MAXRLEN  18

/////////////////////////////////////////////////////////////////////
//MF522寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  		        0x3F

/////////////////////////////////////////////////////////////////////
//和MF522通讯时返回的错误代码
/////////////////////////////////////////////////////////////////////
#define 	MI_OK                 0x26
#define 	MI_NOTAGERR           0xcc
#define 	MI_ERR                0xbb

/* 扩展变量 ------------------------------------------------------------------*/
//extern SPI_HandleTypeDef hspiflash;

/* 函数声明 ------------------------------------------------------------------*/
//void MX_SPIFlash_Init(void);


#define          macDummy_Data              0x00

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void   PcdReset             ( void );                       //复位
void   M500PcdConfigISOType ( uint8_t type );                    //工作方式
char   PcdRequest           ( uint8_t req_code, uint8_t * pTagType ); //寻卡
char   PcdAnticoll          ( uint8_t * pSnr);                   //读卡号
char   PcdAuthState         ( uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr );//验证卡片密码
char   PcdRead              ( uint8_t ucAddr, uint8_t * pData );      //读卡
char 	 PcdWrite 						( uint8_t ucAddr, uint8_t * pData );			//写卡
char   PcdSelect            ( uint8_t * pSnr );                  //选卡
char 	 PcdHalt							( void );			//进入休眠状态



#endif /* L1FREERTOS_MOD_RFID_H_ */

