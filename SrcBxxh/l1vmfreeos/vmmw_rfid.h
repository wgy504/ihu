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
#define IHU_VMWM_RFIDMOD_SCAN_MF522_MAX_TIME 4

//向上提供全局统一服务的入口
extern OPSTAT ihu_vmmw_rfidmod_mf522_spi_send_command(uint8_t *command);
extern OPSTAT ihu_vmmw_rfidmod_mf522_spi_read_id(uint8_t *rfidAddr, uint8_t len);
extern OPSTAT ihu_vmmw_rfidmod_nrf24l01_spi_read_id(uint8_t *rfidAddr, uint8_t len);

//工作调用流
OPSTAT func_rfidmod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, UINT16 wait_time);  //秒级！！！
OPSTAT func_rfidmod_wait_command_fb(uint8_t *ack, uint16_t wait_time);
void func_rfidmod_clear_receive_buffer(void);
void func_rfidmod_send_string(char* s);
#define func_rfidmod_send_LR() func_rfidmod_send_string("\r\n");

//高级定义，简化程序的可读性
#define IHU_ERROR_PRINT_RFIDMOD(arg...)	do{zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;  IhuErrorPrint(##arg);  return IHU_FAILURE; }while(0)


/*
 *
 *  MF522模块
 *
 *
 *
*/

/////////////////////////////////////////////////////////////////////
//MF522命令字
/////////////////////////////////////////////////////////////////////
#define IHU_VMWM_RFIDMOD_MF522_PCD_IDLE              0x00               //取消当前命令
#define IHU_VMWM_RFIDMOD_MF522_PCD_AUTHENT           0x0E               //验证密钥
#define IHU_VMWM_RFIDMOD_MF522_PCD_RECEIVE           0x08               //接收数据
#define IHU_VMWM_RFIDMOD_MF522_PCD_TRANSMIT          0x04               //发送数据
#define IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define IHU_VMWM_RFIDMOD_MF522_PCD_RESETPHASE        0x0F               //复位
#define IHU_VMWM_RFIDMOD_MF522_PCD_CALCCRC           0x03               //CRC计算

/////////////////////////////////////////////////////////////////////
//Mifare_One卡片命令字
/////////////////////////////////////////////////////////////////////
#define IHU_VMWM_RFIDMOD_MF522_PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define IHU_VMWM_RFIDMOD_MF522_PICC_REQALL           0x52               //寻天线区内全部卡
#define IHU_VMWM_RFIDMOD_MF522_PICC_ANTICOLL1        0x93               //防冲撞
#define IHU_VMWM_RFIDMOD_MF522_PICC_ANTICOLL2        0x95               //防冲撞
#define IHU_VMWM_RFIDMOD_MF522_PICC_AUTHENT1A        0x60               //验证A密钥
#define IHU_VMWM_RFIDMOD_MF522_PICC_AUTHENT1B        0x61               //验证B密钥
#define IHU_VMWM_RFIDMOD_MF522_PICC_READ             0x30               //读块
#define IHU_VMWM_RFIDMOD_MF522_PICC_WRITE            0xA0               //写块
#define IHU_VMWM_RFIDMOD_MF522_PICC_DECREMENT        0xC0               //扣款
#define IHU_VMWM_RFIDMOD_MF522_PICC_INCREMENT        0xC1               //充值
#define IHU_VMWM_RFIDMOD_MF522_PICC_RESTORE          0xC2               //调块数据到缓冲区
#define IHU_VMWM_RFIDMOD_MF522_PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define IHU_VMWM_RFIDMOD_MF522_PICC_HALT             0x50               //休眠

/////////////////////////////////////////////////////////////////////
//MF522 FIFO长度定义
/////////////////////////////////////////////////////////////////////
#define IHU_VMWM_RFIDMOD_MF522_DEF_FIFO_LENGTH       64                 //FIFO size=64byte
#define IHU_VMWM_RFIDMOD_MF522_MAXRLEN  18

/////////////////////////////////////////////////////////////////////
//MF522寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     IHU_VMWM_RFIDMOD_MF522_RFU00                 0x00    
#define     IHU_VMWM_RFIDMOD_MF522_CommandReg            0x01    
#define     IHU_VMWM_RFIDMOD_MF522_ComIEnReg             0x02    
#define     IHU_VMWM_RFIDMOD_MF522_DivlEnReg             0x03    
#define     IHU_VMWM_RFIDMOD_MF522_ComIrqReg             0x04    
#define     IHU_VMWM_RFIDMOD_MF522_DivIrqReg             0x05
#define     IHU_VMWM_RFIDMOD_MF522_ErrorReg              0x06    
#define     IHU_VMWM_RFIDMOD_MF522_Status1Reg            0x07    
#define     IHU_VMWM_RFIDMOD_MF522_Status2Reg            0x08    
#define     IHU_VMWM_RFIDMOD_MF522_FIFODataReg           0x09
#define     IHU_VMWM_RFIDMOD_MF522_FIFOLevelReg          0x0A
#define     IHU_VMWM_RFIDMOD_MF522_WaterLevelReg         0x0B
#define     IHU_VMWM_RFIDMOD_MF522_ControlReg            0x0C
#define     IHU_VMWM_RFIDMOD_MF522_BitFramingReg         0x0D
#define     IHU_VMWM_RFIDMOD_MF522_CollReg               0x0E
#define     IHU_VMWM_RFIDMOD_MF522_RFU0F                 0x0F
// PAGE 1     
#define     IHU_VMWM_RFIDMOD_MF522_RFU10                 0x10
#define     IHU_VMWM_RFIDMOD_MF522_ModeReg               0x11
#define     IHU_VMWM_RFIDMOD_MF522_TxModeReg             0x12
#define     IHU_VMWM_RFIDMOD_MF522_RxModeReg             0x13
#define     IHU_VMWM_RFIDMOD_MF522_TxControlReg          0x14
#define     IHU_VMWM_RFIDMOD_MF522_TxAutoReg             0x15
#define     IHU_VMWM_RFIDMOD_MF522_TxSelReg              0x16
#define     IHU_VMWM_RFIDMOD_MF522_RxSelReg              0x17
#define     IHU_VMWM_RFIDMOD_MF522_RxThresholdReg        0x18
#define     IHU_VMWM_RFIDMOD_MF522_DemodReg              0x19
#define     IHU_VMWM_RFIDMOD_MF522_RFU1A                 0x1A
#define     IHU_VMWM_RFIDMOD_MF522_RFU1B                 0x1B
#define     IHU_VMWM_RFIDMOD_MF522_MifareReg             0x1C
#define     IHU_VMWM_RFIDMOD_MF522_RFU1D                 0x1D
#define     IHU_VMWM_RFIDMOD_MF522_RFU1E                 0x1E
#define     IHU_VMWM_RFIDMOD_MF522_SerialSpeedReg        0x1F
// PAGE 2    
#define     IHU_VMWM_RFIDMOD_MF522_RFU20                 0x20  
#define     IHU_VMWM_RFIDMOD_MF522_CRCResultRegM         0x21
#define     IHU_VMWM_RFIDMOD_MF522_CRCResultRegL         0x22
#define     IHU_VMWM_RFIDMOD_MF522_RFU23                 0x23
#define     IHU_VMWM_RFIDMOD_MF522_ModWidthReg           0x24
#define     IHU_VMWM_RFIDMOD_MF522_RFU25                 0x25
#define     IHU_VMWM_RFIDMOD_MF522_RFCfgReg              0x26
#define     IHU_VMWM_RFIDMOD_MF522_GsNReg                0x27
#define     IHU_VMWM_RFIDMOD_MF522_CWGsCfgReg            0x28
#define     IHU_VMWM_RFIDMOD_MF522_ModGsCfgReg           0x29
#define     IHU_VMWM_RFIDMOD_MF522_TModeReg              0x2A
#define     IHU_VMWM_RFIDMOD_MF522_TPrescalerReg         0x2B
#define     IHU_VMWM_RFIDMOD_MF522_TReloadRegH           0x2C
#define     IHU_VMWM_RFIDMOD_MF522_TReloadRegL           0x2D
#define     IHU_VMWM_RFIDMOD_MF522_TCounterValueRegH     0x2E
#define     IHU_VMWM_RFIDMOD_MF522_TCounterValueRegL     0x2F
// PAGE 3      
#define     IHU_VMWM_RFIDMOD_MF522_RFU30                 0x30
#define     IHU_VMWM_RFIDMOD_MF522_TestSel1Reg           0x31
#define     IHU_VMWM_RFIDMOD_MF522_TestSel2Reg           0x32
#define     IHU_VMWM_RFIDMOD_MF522_TestPinEnReg          0x33
#define     IHU_VMWM_RFIDMOD_MF522_TestPinValueReg       0x34
#define     IHU_VMWM_RFIDMOD_MF522_TestBusReg            0x35
#define     IHU_VMWM_RFIDMOD_MF522_AutoTestReg           0x36
#define     IHU_VMWM_RFIDMOD_MF522_VersionReg            0x37
#define     IHU_VMWM_RFIDMOD_MF522_AnalogTestReg         0x38
#define     IHU_VMWM_RFIDMOD_MF522_TestDAC1Reg           0x39  
#define     IHU_VMWM_RFIDMOD_MF522_TestDAC2Reg           0x3A   
#define     IHU_VMWM_RFIDMOD_MF522_TestADCReg            0x3B   
#define     IHU_VMWM_RFIDMOD_MF522_RFU3C                 0x3C   
#define     IHU_VMWM_RFIDMOD_MF522_RFU3D                 0x3D   
#define     IHU_VMWM_RFIDMOD_MF522_RFU3E                 0x3E   
#define     IHU_VMWM_RFIDMOD_MF522_RFU3F		  		        0x3F

/////////////////////////////////////////////////////////////////////
//和MF522通讯时返回的错误代码
/////////////////////////////////////////////////////////////////////
#define 	IHU_VMWM_RFIDMOD_MF522_MI_OK                 0x26
#define 	IHU_VMWM_RFIDMOD_MF522_MI_NOTAGERR           0xcc
#define 	IHU_VMWM_RFIDMOD_MF522_MI_ERR                0xbb
#define   IHU_VMWM_RFIDMOD_MF522_MAC_DUMMY_DATA              0x00

/* 函数声明 ------------------------------------------------------------------*/
void    func_rfidmod_mf522_PcdReset             ( void );                       //复位
void    func_rfidmod_mf522_M500PcdConfigISOType ( uint8_t type );                    //工作方式
char    func_rfidmod_mf522_PcdRequest           ( uint8_t req_code, uint8_t * pTagType ); //寻卡
char    func_rfidmod_mf522_PcdAnticoll          ( uint8_t * pSnr);                   //读卡号
char    func_rfidmod_mf522_PcdAuthState         ( uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr );//验证卡片密码
char    func_rfidmod_mf522_PcdRead              ( uint8_t ucAddr, uint8_t * pData );      //读卡
char 	  func_rfidmod_mf522_PcdWrite 						( uint8_t ucAddr, uint8_t * pData );			//写卡
char    func_rfidmod_mf522_PcdSelect            ( uint8_t * pSnr );                  //选卡
char 	  func_rfidmod_mf522_PcdHalt							( void );			//进入休眠状态
void    func_rfidmod_mf522_IC_CMT ( uint8_t * UID, uint8_t * KEY, uint8_t RW, uint8_t * Dat );
uint8_t func_rfidmod_mf522_ReadRawRC ( uint8_t ucAddress );
void    func_rfidmod_mf522_WriteRawRC ( uint8_t ucAddress, uint8_t ucValue);
void    func_rfidmod_mf522_SetBitMask ( uint8_t ucReg, uint8_t ucMask );
void    func_rfidmod_mf522_ClearBitMask ( uint8_t ucReg, uint8_t ucMask );
void    func_rfidmod_mf522_PcdAntennaOn ( void);
void    func_rfidmod_mf522_PcdAntennaOff ( void );
char    func_rfidmod_mf522_PcdComMF522 ( uint8_t ucCommand, uint8_t * pInData, uint8_t ucInLenByte, uint8_t * pOutData, uint32_t * pOutLenBit );
void    func_rfidmod_mf522_CalulateCRC ( uint8_t * pIndata, uint8_t ucLen, uint8_t * pOutData );

/*
 *
 *  NRF24L01模块
 *
 *
 *
*/

// NRF24L01发送接收数据宽度定义
#define IHU_VMWM_RFIDMOD_NRF24L01_TX_ADR_WIDTH                                  5   	//5字节的地址宽度
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_ADR_WIDTH                                  5   	//5字节的地址宽度
#define IHU_VMWM_RFIDMOD_NRF24L01_TX_PLOAD_WIDTH                                32  	//32字节的用户数据宽度
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_PLOAD_WIDTH                                32  	//32字节的用户数据宽度

//NRF24L01寄存器操作命令
#define IHU_VMWM_RFIDMOD_NRF24L01_NRF_READ_REG    0x00  //读配置寄存器,低5位为寄存器地址
#define IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG   0x20  //写配置寄存器,低5位为寄存器地址
#define IHU_VMWM_RFIDMOD_NRF24L01_RD_RX_PLOAD     0x61  //读RX有效数据,1~32字节
#define IHU_VMWM_RFIDMOD_NRF24L01_WR_TX_PLOAD     0xA0  //写TX有效数据,1~32字节
#define IHU_VMWM_RFIDMOD_NRF24L01_FLUSH_TX        0xE1  //清除TX FIFO寄存器.发射模式下用
#define IHU_VMWM_RFIDMOD_NRF24L01_FLUSH_RX        0xE2  //清除RX FIFO寄存器.接收模式下用
#define IHU_VMWM_RFIDMOD_NRF24L01_REUSE_TX_PL     0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define IHU_VMWM_RFIDMOD_NRF24L01_NOP             0xFF  //空操作,可以用来读状态寄存器	 
//SPI(NRF24L01)寄存器地址
#define IHU_VMWM_RFIDMOD_NRF24L01_CONFIG          0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
                              //bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define IHU_VMWM_RFIDMOD_NRF24L01_EN_AA           0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define IHU_VMWM_RFIDMOD_NRF24L01_EN_RXADDR       0x02  //接收地址允许,bit0~5,对应通道0~5
#define IHU_VMWM_RFIDMOD_NRF24L01_SETUP_AW        0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define IHU_VMWM_RFIDMOD_NRF24L01_SETUP_RETR      0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define IHU_VMWM_RFIDMOD_NRF24L01_RF_CH           0x05  //RF通道,bit6:0,工作通道频率;
#define IHU_VMWM_RFIDMOD_NRF24L01_RF_SETUP        0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define IHU_VMWM_RFIDMOD_NRF24L01_STATUS          0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                              //bit5:数据发送完成中断;bit6:接收数据中断;
#define IHU_VMWM_RFIDMOD_NRF24L01_MAX_TX  		    0x10  //达到最大发送次数中断
#define IHU_VMWM_RFIDMOD_NRF24L01_TX_OK   		    0x20  //TX发送完成中断
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_OK   		    0x40  //接收到数据中断

#define IHU_VMWM_RFIDMOD_NRF24L01_OBSERVE_TX      0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define IHU_VMWM_RFIDMOD_NRF24L01_CD              0x09  //载波检测寄存器,bit0,载波检测;
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P0      0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P1      0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P2      0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P1[39:8]相等;
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P3      0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P1[39:8]相等;
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P4      0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P1[39:8]相等;
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P5      0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P1[39:8]相等;
#define IHU_VMWM_RFIDMOD_NRF24L01_TX_ADDR         0x10  //发送地址(低字节在前),ShockBurstTM模式下,IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P0与此地址相等
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_PW_P0        0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_PW_P1        0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_PW_P2        0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_PW_P3        0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_PW_P4        0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define IHU_VMWM_RFIDMOD_NRF24L01_RX_PW_P5        0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define IHU_VMWM_RFIDMOD_NRF24L01_NRF_FIFO_STATUS 0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                              //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;

//NRF24L01 Local API - 函数声明
void    func_rfidmod_nrf24l01_rx_mode(void);					//配置为接收模式
void    func_rfidmod_nrf24l01_tx_mode(void);					//配置为发送模式
uint8_t func_rfidmod_nrf24l01_write_buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);//写数据区
uint8_t func_rfidmod_nrf24l01_read_buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);	//读数据区		  
uint8_t func_rfidmod_nrf24l01_read_reg(uint8_t reg);					//读寄存器
uint8_t func_rfidmod_nrf24l01_write_reg(uint8_t reg, uint8_t value);		//写寄存器
int8_t  func_rfidmod_nrf24l01_check(void);						//检查24L01是否存在
int8_t func_rfidmod_nrf24l01_tx_package(uint8_t *txbuf);				//发送一个包的数据
int8_t func_rfidmod_nrf24l01_rx_package(uint8_t *rxbuf);				//接收一个包的数据

#endif /* L1FREERTOS_MOD_RFID_H_ */

