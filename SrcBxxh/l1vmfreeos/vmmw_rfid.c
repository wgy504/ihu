/**
 ****************************************************************************************
 *
 * @file mod_rfid.c
 *
 * @brief RFID module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */
 
#include "vmmw_rfid.h"

//全局变量，引用外部
//当使用串口时
extern int8_t 	zIhuBspStm32SpsRfidRxBuff[IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN];			//串口RFID数据接收缓冲区 
extern int16_t	zIhuBspStm32SpsRfidRxCount;																					//当前接收数据的字节数 	  
//当使用SPI接口时
#if (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI1)
	extern int8_t 	zIhuBspStm32SpiGeneral1RxBuff[IHU_BSP_STM32_SPI1_GENERAL_REC_MAX_LEN];				//SPI数据接收缓冲区 
	extern int16_t 	zIhuBspStm32SpiGeneral1RxCount;																							//当前接收数据的字节数 	  
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI2)
	extern int8_t 	zIhuBspStm32SpiGeneral2RxBuff[IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN];				//SPI数据接收缓冲区 
	extern int16_t 	zIhuBspStm32SpiGeneral2RxCount;																							//当前接收数据的字节数
#endif

/*******************************************************************************
* 函数名 : func_rfidmod_uart_send_AT_command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 1:正常  -1:错误
* 注意   : 
* OPSTAT func_rfidmod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, uint8_t wait_time)  
* 这里的发送，只有成功返回ACK对应的回复时，才算成功
*******************************************************************************/
OPSTAT func_rfidmod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, uint16_t wait_time) //in Second
{
	int res;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
	func_rfidmod_clear_receive_buffer();
	ihu_l1hd_sps_rfid_send_data((uint8_t *)cmd, strlen((char*)cmd));
	func_rfidmod_send_LR();	
	
	res = IHU_FAILURE;
	while((tickTotal > 0) && (res == IHU_FAILURE))
	{
		ihu_usleep(IHU_BSP_STM32_SPS_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
		tickTotal--;
		if(strstr((const char*)zIhuBspStm32SpsRfidRxBuff, (char*)ack)==NULL)
			 res = IHU_FAILURE;
		else
			 res = IHU_SUCCESS;
	}
	return res;
}

//in Second
//使用#if的方式，是因为SPI可能根本不存在，但USART是强迫存在的VMMW模块，不得已，不然可以使用if-else-end
OPSTAT func_rfidmod_wait_command_fb(uint8_t *ack, uint16_t wait_time)
{
	int ret = 0;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
	ret = IHU_FAILURE;

	while((tickTotal > 0) && (ret == IHU_FAILURE))
	{
		ihu_usleep(IHU_BSP_STM32_SPS_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
		tickTotal--;

#if (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPS_RFID)
		if(strstr((const char*)zIhuBspStm32SpsRfidRxBuff, (char*)ack)==NULL)
			 ret = IHU_FAILURE;
		else
			 ret = IHU_SUCCESS;
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI1)
		if(strstr((const char*)zIhuBspStm32SpiGeneral1RxBuff, (char*)ack)==NULL)
			 ret = IHU_FAILURE;
		else
			 ret = IHU_SUCCESS;
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI2)
		if(strstr((const char*)zIhuBspStm32SpiGeneral2RxBuff, (char*)ack)==NULL)
			 ret = IHU_FAILURE;
		else
			 ret = IHU_SUCCESS;
#else
	#error Un-correct constant definition
#endif
	}
	return ret;
}


/*******************************************************************************
* 函数名 : func_rfidmod_clear_receive_buffer
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
//使用#if的方式，是因为SPI可能根本不存在，但USART是强迫存在的VMMW模块，不得已，不然可以使用if-else-end
void func_rfidmod_clear_receive_buffer(void)
{
	uint16_t k;
#if (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPS_RFID)
	for(k=0;k<IHU_BSP_STM32_SPS_RFID_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsRfidRxBuff[k] = 0x00;
	}
	zIhuBspStm32SpsRfidRxCount = 0;               //接收字符串的起始存储位置
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI1)
	for(k=0;k<IHU_BSP_STM32_SPI1_GENERAL_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpiGeneral1RxBuff[k] = 0x00;
	}
	zIhuBspStm32SpiGeneral1RxCount = 0;               //接收字符串的起始存储位置
#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI2)
	for(k=0;k<IHU_BSP_STM32_SPI2_GENERAL_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpiGeneral2RxBuff[k] = 0x00;
	}
	zIhuBspStm32SpiGeneral2RxCount = 0;               //接收字符串的起始存储位置
#else
	#error Un-correct constant definition
#endif
}

/*******************************************************************************
* 函数名  : USART_RFID_SendString
* 描述    : USART_RFID发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void func_rfidmod_send_string(char* s)
{
	while(*s != '\0')//检测字符串结束符
	{
		ihu_l1hd_sps_rfid_send_data((uint8_t *)s++, 1);
	}
}


//发送命令
OPSTAT ihu_vmmw_rfidmod_mf522_spi_send_command(uint8_t *command)
{
	return IHU_SUCCESS;
}

//读取MF522 RFID卡的数据，或者说扫描ID
OPSTAT ihu_vmmw_rfidmod_mf522_spi_read_id(uint8_t *rfidAddr, uint8_t len)
{
	uint8_t CT[2] = {0,0};//卡片类型
	uint8_t SN1[4] = {0,0,0,0};//卡片号码
	uint8_t assWd[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //验证码
	uint8_t RFRead[16]; //存放RFID 
	uint8_t RFWrite[16]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
	uint8_t card_1[4]={0xc3,0x6b,0x97,0xf4};    //异形卡
	uint8_t card_2[4]={0x46,0x5f,0x9a,0xb5};    //白卡
	uint8_t s=0x08;

	//等待的时间长度
	//uint32_t tickTotal = IHU_VMWM_RFIDMOD_SCAN_MF522_MAX_TIME * 1000 / IHU_BSP_STM32_SPI_RX_MAX_DELAY;  //4秒
	
	//参数检查：RFID固定长度为16
	if ((rfidAddr == NULL) || (len != 16)) IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMMWRFID: invalid input paramter received!\n");
	
	//清空缓冲区
	func_rfidmod_clear_receive_buffer();
	
	//复位MF522外设：需要探测是否成功，如果不成功，则直接返回，并输出错误，从而加快整个程序的执行过程
	func_rfidmod_mf522_PcdReset();
	
  // read version
	IHU_DEBUG_PRINT_INF("VMMWRFID: VersionReg is 0x%02x\n", func_rfidmod_mf522_ReadRawRC(IHU_VMWM_RFIDMOD_MF522_VersionReg));
	IHU_DEBUG_PRINT_INF("VMMWRFID: ErrorReg is 0x%02x\n", func_rfidmod_mf522_ReadRawRC(IHU_VMWM_RFIDMOD_MF522_ErrorReg));
	IHU_DEBUG_PRINT_INF("VMMWRFID: Status1Reg is 0x%02x\n", func_rfidmod_mf522_ReadRawRC(IHU_VMWM_RFIDMOD_MF522_Status1Reg));
	IHU_DEBUG_PRINT_INF("VMMWRFID: Status2Reg is 0x%02x\n", func_rfidmod_mf522_ReadRawRC(IHU_VMWM_RFIDMOD_MF522_Status2Reg));
	
	//设置工作方式	
	func_rfidmod_mf522_M500PcdConfigISOType('A');

	//寻卡测试
	if (func_rfidmod_mf522_PcdRequest(IHU_VMWM_RFIDMOD_MF522_PICC_REQALL, CT) != IHU_VMWM_RFIDMOD_MF522_MI_OK)
		IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMMWRFID: RFID device (MFRC522) can not find card!\n");
	
	//防冲撞,至此，SN1中保存了卡号
	if (func_rfidmod_mf522_PcdAnticoll(SN1) == IHU_VMWM_RFIDMOD_MF522_MI_OK){
		IHU_DEBUG_PRINT_INF("VMMWRFID: Card type = [0x%x, 0x%x], Card number = [0x%x, 0x%x, 0x%x, 0x%x].\n", CT[0], CT[1], SN1[0], SN1[1], SN1[2], SN1[3]);
		if(!memcmp(SN1, card_1, 4))
			IHU_DEBUG_PRINT_FAT("VMMWRFID: Other card find!\n");
		else if(!memcmp(SN1, card_2, 4)) 
			IHU_DEBUG_PRINT_FAT("VMMWRFID: White card find!\n");
	}
	else{
		IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMMWRFID: RDID device (MFRC522) can not pass anticoll procedure!\n");
	}
	
	//选卡测试	
	if (func_rfidmod_mf522_PcdSelect(SN1) != IHU_VMWM_RFIDMOD_MF522_MI_OK)
		IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMMWRFID: RDID device (MFRC522) can not pass select card procedure!\n");
	
	//验证A密匙
	if (func_rfidmod_mf522_PcdAuthState(0x60,0x09,assWd,SN1)!= IHU_VMWM_RFIDMOD_MF522_MI_OK)
		IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMMWRFID: RDID device (MFRC522) can not pass authtication procedure!\n");
	
	//写卡实验
	if (func_rfidmod_mf522_PcdWrite(s, RFWrite)!= IHU_VMWM_RFIDMOD_MF522_MI_OK)
		IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMMWRFID: RDID device (MFRC522) can not pass write procedure!\n");

	//读卡实验
	if (func_rfidmod_mf522_PcdRead(s, RFRead) == IHU_VMWM_RFIDMOD_MF522_MI_OK){
		memcpy(rfidAddr, RFRead, len);
	}else{
		IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMMWRFID: RDID device (MFRC522) can not pass read procedure!\n");	
	}

	//命令卡片进入休眠状态
	func_rfidmod_mf522_PcdHalt();

	return IHU_SUCCESS;
	
//	//循环读取
//	res = IHU_FAILURE;
//	while((tickTotal > 0) && (res == IHU_FAILURE))
//	{
//		ihu_usleep(IHU_BSP_STM32_SPI_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
//		tickTotal--;
//    /*寻卡*/
//		if ( ( ucStatusReturn = func_rfidmod_mf522_PcdRequest ( IHU_VMWM_RFIDMOD_MF522_PICC_REQALL, ucArray_ID ) ) != IHU_VMWM_RFIDMOD_MF522_MI_OK )                                   
//			 /*若失败再次寻卡*/
//      ucStatusReturn = func_rfidmod_mf522_PcdRequest ( IHU_VMWM_RFIDMOD_MF522_PICC_REQALL, ucArray_ID );		
//		IhuDebugPrint("VMMWRFID: Step1 reach.\n");
//		if ( ucStatusReturn == IHU_VMWM_RFIDMOD_MF522_MI_OK  )
//		{
//      /*防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
//			IhuDebugPrint("VMMWRFID: Step2 reach.\n");
//			if ( func_rfidmod_mf522_PcdAnticoll(ucArray_ID) == IHU_VMWM_RFIDMOD_MF522_MI_OK )  
//			{
//				memcpy(rfidAddr, ucArray_ID, 4);
//				sprintf(cStr, "The Card ID is: %02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3] );							
//				IhuDebugPrint("VMMWRFID: [%s]\n", cStr);
//				res = IHU_SUCCESS;
//				func_rfidmod_mf522_PcdHalt (); 		
//      }
//		}
//	}
//	if (res == IHU_FAILURE) IhuDebugPrint("VMMWRFID: Fail to read RFID value!\n");
//	return res;	
	
}


/*
 * 函数名：func_rfidmod_mf522_ReadRawRC
 * 描述  ：读MF522寄存器
 * 输入  ：ucAddress，寄存器地址
 * 返回  : 寄存器的当前值
 * 调用  ：内部调用
 */
uint8_t func_rfidmod_mf522_ReadRawRC ( uint8_t ucAddress )
{
	uint8_t ucAddr=0, ucReturn=0xEE;
	
	ucAddr = ( ( ucAddress << 1 ) & 0x7E ) | 0x80;
	ihu_l1hd_spi_mf522_cs_enable() ;
	ihu_l1hd_spi_flash_send_byte ( ucAddr );
	ucReturn = ihu_l1hd_spi_flash_read_byte ();
	ihu_l1hd_spi_mf522_cs_disable();
	return ucReturn;
}

/*
 * 函数名：func_rfidmod_mf522_WriteRawRC
 * 描述  ：写MF522寄存器
 * 输入  ：ucAddress，寄存器地址
 *         ucValue，写入寄存器的值
 * 返回  : 无
 * 调用  ：内部调用
 */
void func_rfidmod_mf522_WriteRawRC ( uint8_t ucAddress, uint8_t ucValue )
{  
	uint8_t ucAddr=0;
	
	ucAddr = ( ucAddress << 1 ) & 0x7E;
	ihu_l1hd_spi_mf522_cs_enable() ;
	ihu_l1hd_spi_flash_send_byte ( ucAddr );
	ihu_l1hd_spi_flash_send_byte ( ucValue );
	ihu_l1hd_spi_mf522_cs_disable();	
}

/*
 * 函数名：func_rfidmod_mf522_SetBitMask
 * 描述  ：对MF522寄存器置位
 * 输入  ：ucReg，寄存器地址
 *         ucMask，置位值
 * 返回  : 无
 * 调用  ：内部调用
 */
void func_rfidmod_mf522_SetBitMask ( uint8_t ucReg, uint8_t ucMask )  
{
    uint8_t ucTemp;
	
    ucTemp = func_rfidmod_mf522_ReadRawRC ( ucReg );
    func_rfidmod_mf522_WriteRawRC ( ucReg, ucTemp | ucMask );         // set bit mask
}

/*
 * 函数名：func_rfidmod_mf522_ClearBitMask
 * 描述  ：对MF522寄存器清位
 * 输入  ：ucReg，寄存器地址
 *         ucMask，清位值
 * 返回  : 无
 * 调用  ：内部调用
 */
void func_rfidmod_mf522_ClearBitMask ( uint8_t ucReg, uint8_t ucMask )  
{
    uint8_t ucTemp;
	
    ucTemp = func_rfidmod_mf522_ReadRawRC ( ucReg );
    func_rfidmod_mf522_WriteRawRC ( ucReg, ucTemp & ( ~ ucMask) );  // clear bit mask
}

/*
 * 函数名：func_rfidmod_mf522_PcdAntennaOn
 * 描述  ：开启天线 
 * 输入  ：无
 * 返回  : 无
 * 调用  ：内部调用
 */
void func_rfidmod_mf522_PcdAntennaOn ( void )
{
    uint8_t uc;
	
    uc = func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_TxControlReg );
    if ( ! ( uc & 0x03 ) )
			func_rfidmod_mf522_SetBitMask(IHU_VMWM_RFIDMOD_MF522_TxControlReg, 0x03);
}


/*
 * 函数名：func_rfidmod_mf522_PcdAntennaOff
 * 描述  ：开启天线 
 * 输入  ：无
 * 返回  : 无
 * 调用  ：内部调用
 */
void func_rfidmod_mf522_PcdAntennaOff ( void )
{
  func_rfidmod_mf522_ClearBitMask ( IHU_VMWM_RFIDMOD_MF522_TxControlReg, 0x03 );
}

/*
 * 函数名：PcdRese
 * 描述  ：复位MF522 
 * 输入  ：无
 * 返回  : 无
 * 调用  ：外部调用
 */
void func_rfidmod_mf522_PcdReset ( void )
{
	ihu_l1hd_spi_mf522_reset_disable();
	ihu_sleep ( 1 );
	ihu_l1hd_spi_mf522_reset_enable();
	ihu_sleep ( 1 );
	ihu_l1hd_spi_mf522_reset_disable();
	ihu_sleep ( 1 );
	func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_CommandReg, IHU_VMWM_RFIDMOD_MF522_PCD_RESETPHASE );
	while ( func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_CommandReg ) & 0x10 );
	ihu_sleep ( 1 );
  func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_ModeReg, 0x3D );            //定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
  func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_TReloadRegL, 30 );          //16位定时器低位    
	func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_TReloadRegH, 0 );			     //16位定时器高位
  func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_TModeReg, 0x8D );				   //定义内部定时器的设置
  func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_TPrescalerReg, 0x3E );			 //设置定时器分频系数
	func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_TxAutoReg, 0x40 );				   //调制发送信号为100%ASK
	func_rfidmod_mf522_PcdAntennaOff();
  ihu_sleep ( 1 );
  func_rfidmod_mf522_PcdAntennaOn();
}

/*
 * 函数名：func_rfidmod_mf522_M500PcdConfigISOType
 * 描述  ：设置MF522的工作方式
 * 输入  ：ucType，工作方式
 * 返回  : 无
 * 调用  ：外部调用
 */
void func_rfidmod_mf522_M500PcdConfigISOType ( uint8_t ucType )
{
	if ( ucType == 'A')                     //ISO14443_A
  {
		func_rfidmod_mf522_ClearBitMask ( IHU_VMWM_RFIDMOD_MF522_Status2Reg, 0x08 );
    func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_ModeReg, 0x3D );//3F
		func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_RxSelReg, 0x86 );//84
		func_rfidmod_mf522_WriteRawRC( IHU_VMWM_RFIDMOD_MF522_RFCfgReg, 0x7F );   //4F
		func_rfidmod_mf522_WriteRawRC( IHU_VMWM_RFIDMOD_MF522_TReloadRegL, 30 );//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_TReloadRegH, 0 );
		func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_TModeReg, 0x8D );
		func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_TPrescalerReg, 0x3E );
		ihu_usleep( 20 );
		func_rfidmod_mf522_PcdAntennaOn ();//开天线
   }
}

/*
 * 函数名：func_rfidmod_mf522_PcdComMF522
 * 描述  ：通过MF522和ISO14443卡通讯
 * 输入  ：ucCommand，MF522命令字
 *         pInData，通过MF522发送到卡片的数据
 *         ucInLenByte，发送数据的字节长度
 *         pOutData，接收到的卡片返回数据
 *         pOutLenBit，返回数据的位长度
 * 返回  : 状态值
 *         = IHU_VMWM_RFIDMOD_MF522_MI_OK，成功
 * 调用  ：内部调用
 */
char func_rfidmod_mf522_PcdComMF522 ( uint8_t ucCommand, uint8_t * pInData, uint8_t ucInLenByte, uint8_t * pOutData, uint32_t * pOutLenBit )		
{
    char cStatus = IHU_VMWM_RFIDMOD_MF522_MI_ERR;
    uint8_t ucIrqEn   = 0x00;
    uint8_t ucWaitFor = 0x00;
    uint8_t ucLastBits;
    uint8_t ucN;
    uint32_t ul;
	
    switch ( ucCommand )
    {
       case IHU_VMWM_RFIDMOD_MF522_PCD_AUTHENT:		//Mifare认证
          ucIrqEn   = 0x12;		//允许错误中断请求ErrIEn  允许空闲中断IdleIEn
          ucWaitFor = 0x10;		//认证寻卡等待时候 查询空闲中断标志位
          break;
       case IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE:		//接收发送 发送接收
          ucIrqEn   = 0x77;		//允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
          ucWaitFor = 0x30;		//寻卡等待时候 查询接收中断标志位与 空闲中断标志位
          break;
       default:
         break;
    }
    func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_ComIEnReg, ucIrqEn | 0x80 );		//IRqInv置位管脚IRQ与IHU_VMWM_RFIDMOD_MF522_Status1Reg的IRq位的值相反 
    func_rfidmod_mf522_ClearBitMask ( IHU_VMWM_RFIDMOD_MF522_ComIrqReg, 0x80 );			//Set1该位清零时，CommIRqReg的屏蔽位清零
    func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_CommandReg, IHU_VMWM_RFIDMOD_MF522_PCD_IDLE );		//写空闲命令
    func_rfidmod_mf522_SetBitMask ( IHU_VMWM_RFIDMOD_MF522_FIFOLevelReg, 0x80 );			//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
    for ( ul = 0; ul < ucInLenByte; ul ++ )
		  func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_FIFODataReg, pInData [ ul ] );    		//写数据进FIFOdata
    func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_CommandReg, ucCommand );					//写命令
    if ( ucCommand == IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE )
			func_rfidmod_mf522_SetBitMask(IHU_VMWM_RFIDMOD_MF522_BitFramingReg,0x80);  				//StartSend置位启动数据发送 该位与收发命令使用时才有效
    ul = 1000;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do 														//认证 与寻卡等待时间	
    {
         ucN = func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_ComIrqReg );							//查询事件中断
         ul --;
    } while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//退出条件i=0,定时器中断，与写空闲命令
    func_rfidmod_mf522_ClearBitMask ( IHU_VMWM_RFIDMOD_MF522_BitFramingReg, 0x80 );					//清理允许StartSend位
    if ( ul != 0 )
    {
			if ( ! ( func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_ErrorReg ) & 0x1B ) )			//读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
			{
				cStatus = IHU_VMWM_RFIDMOD_MF522_MI_OK;
				if ( ucN & ucIrqEn & 0x01 )					//是否发生定时器中断
				  cStatus = IHU_VMWM_RFIDMOD_MF522_MI_NOTAGERR;   
				if ( ucCommand == IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE )
				{
					ucN = func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_FIFOLevelReg );			//读FIFO中保存的字节数
					ucLastBits = func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_ControlReg ) & 0x07;	//最后接收到得字节的有效位数
					if ( ucLastBits )
						* pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
					else
						* pOutLenBit = ucN * 8;   					//最后接收到的字节整个字节有效
					if ( ucN == 0 )		
            ucN = 1;    
					if ( ucN > IHU_VMWM_RFIDMOD_MF522_MAXRLEN )
						ucN = IHU_VMWM_RFIDMOD_MF522_MAXRLEN;   
					for ( ul = 0; ul < ucN; ul ++ )
					  pOutData [ ul ] = func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_FIFODataReg );   
					}
      }
			else
				cStatus = IHU_VMWM_RFIDMOD_MF522_MI_ERR;   
    }
   func_rfidmod_mf522_SetBitMask ( IHU_VMWM_RFIDMOD_MF522_ControlReg, 0x80 );           // stop timer now
   func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_CommandReg, IHU_VMWM_RFIDMOD_MF522_PCD_IDLE ); 
   return cStatus;
}

/*
 * 函数名：func_rfidmod_mf522_PcdRequest
 * 描述  ：寻卡
 * 输入  ：ucReq_code，寻卡方式
 *                     = 0x52，寻感应区内所有符合14443A标准的卡
 *                     = 0x26，寻未进入休眠状态的卡
 *         pTagType，卡片类型代码
 *                   = 0x4400，Mifare_UltraLight
 *                   = 0x0400，Mifare_One(S50)
 *                   = 0x0200，Mifare_One(S70)
 *                   = 0x0800，Mifare_Pro(X))
 *                   = 0x4403，Mifare_DESFire
 * 返回  : 状态值
 *         = IHU_VMWM_RFIDMOD_MF522_MI_OK，成功
 * 调用  ：外部调用
 */
char func_rfidmod_mf522_PcdRequest ( uint8_t ucReq_code, uint8_t * pTagType )
{
   char cStatus;  
	 uint8_t ucComMF522Buf [ IHU_VMWM_RFIDMOD_MF522_MAXRLEN ]; 
   uint32_t ulLen;
	
   func_rfidmod_mf522_ClearBitMask ( IHU_VMWM_RFIDMOD_MF522_Status2Reg, 0x08 );	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
   func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_BitFramingReg, 0x07 );	//	发送的最后一个字节的 七位
   func_rfidmod_mf522_SetBitMask ( IHU_VMWM_RFIDMOD_MF522_TxControlReg, 0x03 );	//TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号
   ucComMF522Buf [ 0 ] = ucReq_code;		//存入 卡片命令字
   cStatus = func_rfidmod_mf522_PcdComMF522 ( IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE,	ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//寻卡  
   if ( ( cStatus == IHU_VMWM_RFIDMOD_MF522_MI_OK ) && ( ulLen == 0x10 ) )	//寻卡成功返回卡类型 
   {    
       * pTagType = ucComMF522Buf [ 0 ];
       * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
   }
   else
     cStatus = IHU_VMWM_RFIDMOD_MF522_MI_ERR;
   return cStatus;
}

/*
 * 函数名：func_rfidmod_mf522_PcdAnticoll
 * 描述  ：防冲撞
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = IHU_VMWM_RFIDMOD_MF522_MI_OK，成功
 * 调用  ：外部调用
 */
char func_rfidmod_mf522_PcdAnticoll ( uint8_t * pSnr )
{
    char cStatus;
    uint8_t uc, ucSnr_check = 0;
    uint8_t ucComMF522Buf [ IHU_VMWM_RFIDMOD_MF522_MAXRLEN ]; 
	  uint32_t ulLen;
 
    func_rfidmod_mf522_ClearBitMask ( IHU_VMWM_RFIDMOD_MF522_Status2Reg, 0x08 );		//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
    func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_BitFramingReg, 0x00);		//清理寄存器 停止收发
    func_rfidmod_mf522_ClearBitMask ( IHU_VMWM_RFIDMOD_MF522_CollReg, 0x80 );			  //清ValuesAfterColl所有接收的位在冲突后被清除
    ucComMF522Buf [ 0 ] = IHU_VMWM_RFIDMOD_MF522_PICC_ANTICOLL1;	//卡片防冲突命令
    ucComMF522Buf [ 1 ] = 0x20;
    cStatus = func_rfidmod_mf522_PcdComMF522 ( IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//与卡片通信
    if ( cStatus == IHU_VMWM_RFIDMOD_MF522_MI_OK)		//通信成功
    {
			for ( uc = 0; uc < 4; uc ++ )
			{
         * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//读出UID
         ucSnr_check ^= ucComMF522Buf [ uc ];
      }
      if ( ucSnr_check != ucComMF522Buf [ uc ] )
				cStatus = IHU_VMWM_RFIDMOD_MF522_MI_ERR;    
    }
    func_rfidmod_mf522_SetBitMask ( IHU_VMWM_RFIDMOD_MF522_CollReg, 0x80 );		
    return cStatus;
}

/*
 * 函数名：func_rfidmod_mf522_CalulateCRC
 * 描述  ：用MF522计算CRC16
 * 输入  ：pIndata，计算CRC16的数组
 *         ucLen，计算CRC16的数组字节长度
 *         pOutData，存放计算结果存放的首地址
 * 返回  : 无
 * 调用  ：内部调用
 */
void func_rfidmod_mf522_CalulateCRC ( uint8_t * pIndata, uint8_t ucLen, uint8_t * pOutData )
{
    uint8_t uc, ucN;
	
    func_rfidmod_mf522_ClearBitMask(IHU_VMWM_RFIDMOD_MF522_DivIrqReg,0x04);
    func_rfidmod_mf522_WriteRawRC(IHU_VMWM_RFIDMOD_MF522_CommandReg, IHU_VMWM_RFIDMOD_MF522_PCD_IDLE);
    func_rfidmod_mf522_SetBitMask(IHU_VMWM_RFIDMOD_MF522_FIFOLevelReg,0x80);
    for ( uc = 0; uc < ucLen; uc ++)
	    func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_FIFODataReg, * ( pIndata + uc ) );   

    func_rfidmod_mf522_WriteRawRC ( IHU_VMWM_RFIDMOD_MF522_CommandReg, IHU_VMWM_RFIDMOD_MF522_PCD_CALCCRC );
    uc = 0xFF;
    do 
    {
        ucN = func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_DivIrqReg );
        uc --;
    } while ( ( uc != 0 ) && ! ( ucN & 0x04 ) );
		
    pOutData [ 0 ] = func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_CRCResultRegL );
    pOutData [ 1 ] = func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_CRCResultRegM );
}

/*
 * 函数名：func_rfidmod_mf522_PcdSelect
 * 描述  ：选定卡片
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = IHU_VMWM_RFIDMOD_MF522_MI_OK，成功
 * 调用  ：外部调用
 */
char func_rfidmod_mf522_PcdSelect ( uint8_t * pSnr )
{
    char ucN;
    uint8_t uc;
	  uint8_t ucComMF522Buf [ IHU_VMWM_RFIDMOD_MF522_MAXRLEN ]; 
    uint32_t  ulLen;
    
    ucComMF522Buf [ 0 ] = IHU_VMWM_RFIDMOD_MF522_PICC_ANTICOLL1;
    ucComMF522Buf [ 1 ] = 0x70;
    ucComMF522Buf [ 6 ] = 0;
    for ( uc = 0; uc < 4; uc ++ )
    {
    	ucComMF522Buf [ uc + 2 ] = * ( pSnr + uc );
    	ucComMF522Buf [ 6 ] ^= * ( pSnr + uc );
    }
    func_rfidmod_mf522_CalulateCRC ( ucComMF522Buf, 7, & ucComMF522Buf [ 7 ] );
    func_rfidmod_mf522_ClearBitMask ( IHU_VMWM_RFIDMOD_MF522_Status2Reg, 0x08 );
    ucN = func_rfidmod_mf522_PcdComMF522 ( IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, & ulLen );
    if ( ( ucN == IHU_VMWM_RFIDMOD_MF522_MI_OK ) && ( ulLen == 0x18 ) )
      ucN = IHU_VMWM_RFIDMOD_MF522_MI_OK;  
    else
      ucN = IHU_VMWM_RFIDMOD_MF522_MI_ERR;    
    return ucN;		
}

/*
 * 函数名：func_rfidmod_mf522_PcdAuthState
 * 描述  ：验证卡片密码
 * 输入  ：ucAuth_mode，密码验证模式
 *                     = 0x60，验证A密钥
 *                     = 0x61，验证B密钥
 *         uint8_t ucAddr，块地址
 *         pKey，密码
 *         pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = IHU_VMWM_RFIDMOD_MF522_MI_OK，成功
 * 调用  ：外部调用
 */
char func_rfidmod_mf522_PcdAuthState ( uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr )
{
    char cStatus;
	  uint8_t uc, ucComMF522Buf [ IHU_VMWM_RFIDMOD_MF522_MAXRLEN ];
    uint32_t ulLen;
    
    ucComMF522Buf [ 0 ] = ucAuth_mode;
    ucComMF522Buf [ 1 ] = ucAddr;
    for ( uc = 0; uc < 6; uc ++ )
	    ucComMF522Buf [ uc + 2 ] = * ( pKey + uc );   
    for ( uc = 0; uc < 6; uc ++ )
	    ucComMF522Buf [ uc + 8 ] = * ( pSnr + uc );   
    cStatus = func_rfidmod_mf522_PcdComMF522 ( IHU_VMWM_RFIDMOD_MF522_PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, & ulLen );
    if ( ( cStatus != IHU_VMWM_RFIDMOD_MF522_MI_OK ) || ( ! ( func_rfidmod_mf522_ReadRawRC ( IHU_VMWM_RFIDMOD_MF522_Status2Reg ) & 0x08 ) ) )
      cStatus = IHU_VMWM_RFIDMOD_MF522_MI_ERR;   
    return cStatus;			
}

/*
 * 函数名：func_rfidmod_mf522_PcdWrite
 * 描述  ：写数据到M1卡一块
 * 输入  ：uint8_t ucAddr，块地址
 *         pData，写入的数据，16字节
 * 返回  : 状态值
 *         = IHU_VMWM_RFIDMOD_MF522_MI_OK，成功
 * 调用  ：外部调用
 */
char func_rfidmod_mf522_PcdWrite ( uint8_t ucAddr, uint8_t * pData )
{
    char cStatus;
	  uint8_t uc, ucComMF522Buf [ IHU_VMWM_RFIDMOD_MF522_MAXRLEN ];
    uint32_t ulLen;
     
    ucComMF522Buf [ 0 ] = IHU_VMWM_RFIDMOD_MF522_PICC_WRITE;
    ucComMF522Buf [ 1 ] = ucAddr;
    func_rfidmod_mf522_CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
    cStatus = func_rfidmod_mf522_PcdComMF522 ( IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
    if ( ( cStatus != IHU_VMWM_RFIDMOD_MF522_MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
      cStatus = IHU_VMWM_RFIDMOD_MF522_MI_ERR;   
    if ( cStatus == IHU_VMWM_RFIDMOD_MF522_MI_OK )
    {
			//memcpy(ucComMF522Buf, pData, 16);
      for ( uc = 0; uc < 16; uc ++ )
			  ucComMF522Buf [ uc ] = * ( pData + uc );  
      func_rfidmod_mf522_CalulateCRC ( ucComMF522Buf, 16, & ucComMF522Buf [ 16 ] );
      cStatus = func_rfidmod_mf522_PcdComMF522 ( IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, & ulLen );
			if ( ( cStatus != IHU_VMWM_RFIDMOD_MF522_MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
        cStatus = IHU_VMWM_RFIDMOD_MF522_MI_ERR;   
    } 	
    return cStatus;
}

/*
 * 函数名：func_rfidmod_mf522_PcdRead
 * 描述  ：读取M1卡一块数据
 * 输入  ：uint8_t ucAddr，块地址
 *         pData，读出的数据，16字节
 * 返回  : 状态值
 *         = IHU_VMWM_RFIDMOD_MF522_MI_OK，成功
 * 调用  ：外部调用
 */
char func_rfidmod_mf522_PcdRead ( uint8_t ucAddr, uint8_t * pData )
{
    char cStatus;
	  uint8_t uc, ucComMF522Buf [ IHU_VMWM_RFIDMOD_MF522_MAXRLEN ]; 
    uint32_t ulLen;

    ucComMF522Buf [ 0 ] = IHU_VMWM_RFIDMOD_MF522_PICC_READ;
    ucComMF522Buf [ 1 ] = ucAddr;
    func_rfidmod_mf522_CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
    cStatus = func_rfidmod_mf522_PcdComMF522 ( IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
    if ( ( cStatus == IHU_VMWM_RFIDMOD_MF522_MI_OK ) && ( ulLen == 0x90 ) )
    {
			for ( uc = 0; uc < 16; uc ++ )
        * ( pData + uc ) = ucComMF522Buf [ uc ];   
    }
    else
      cStatus = IHU_VMWM_RFIDMOD_MF522_MI_ERR;   		
    return cStatus;
}

/*
 * 函数名：func_rfidmod_mf522_PcdHalt
 * 描述  ：命令卡片进入休眠状态
 * 输入  ：无
 * 返回  : 状态值
 *         = IHU_VMWM_RFIDMOD_MF522_MI_OK，成功
 * 调用  ：外部调用
 */
char func_rfidmod_mf522_PcdHalt( void )
{
	uint8_t ucComMF522Buf [ IHU_VMWM_RFIDMOD_MF522_MAXRLEN ]; 
	uint32_t  ulLen;
  
  ucComMF522Buf [ 0 ] = IHU_VMWM_RFIDMOD_MF522_PICC_HALT;
  ucComMF522Buf [ 1 ] = 0;
  func_rfidmod_mf522_CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
 	func_rfidmod_mf522_PcdComMF522 ( IHU_VMWM_RFIDMOD_MF522_PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
  return IHU_VMWM_RFIDMOD_MF522_MI_OK;
}

void func_rfidmod_mf522_IC_CMT ( uint8_t * UID, uint8_t * KEY, uint8_t RW, uint8_t * Dat )
{
  uint8_t ucArray_ID [ 4 ] = { 0 };//先后存放IC卡的类型和UID(IC卡序列号)
  
  func_rfidmod_mf522_PcdRequest ( 0x52, ucArray_ID );//寻卡
  func_rfidmod_mf522_PcdAnticoll ( ucArray_ID );//防冲撞
  func_rfidmod_mf522_PcdSelect ( UID );//选定卡
  func_rfidmod_mf522_PcdAuthState ( 0x60, 0x10, KEY, UID );//校验
	if ( RW )//读写选择，1是读，0是写
    func_rfidmod_mf522_PcdRead ( 0x10, Dat );
   else 
     func_rfidmod_mf522_PcdWrite ( 0x10, Dat );
   func_rfidmod_mf522_PcdHalt (); 
}



/*
 *
 * NRF24L01的中间件操作过程
 *
 *
 *
*/

const uint8_t TX_ADDRESS[IHU_VMWM_RFIDMOD_NRF24L01_TX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01}; //发送地址
const uint8_t RX_ADDRESS[IHU_VMWM_RFIDMOD_NRF24L01_RX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01};

/**
  * 函数功能: SPI写寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:指定寄存器地址
  *           
  */ 
uint8_t func_rfidmod_nrf24l01_write_reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
  ihu_l1hd_spi_nrf24l01_cs_enable();                 //使能SPI传输
  ihu_usleep(10);	
  status =ihu_l1hd_spi_nrf24l01_read_write_byte(reg);//发送寄存器号 
  ihu_usleep(10);	
  ihu_l1hd_spi_nrf24l01_read_write_byte(value);      //写入寄存器的值
  ihu_usleep(10);	
  ihu_l1hd_spi_nrf24l01_cs_disable();                 //禁止SPI传输	   
  ihu_usleep(10);	
  return(status);       			//返回状态值
}

/**
  * 函数功能: 读取SPI寄存器值
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:要读的寄存器
  *           
  */ 
uint8_t func_rfidmod_nrf24l01_read_reg(uint8_t reg)
{
	uint8_t reg_val;	    
 	ihu_l1hd_spi_nrf24l01_cs_enable();          //使能SPI传输
  ihu_usleep(10);	
  ihu_l1hd_spi_nrf24l01_read_write_byte(reg);   //发送寄存器号
  ihu_usleep(10);	
  reg_val=ihu_l1hd_spi_nrf24l01_read_write_byte(0XFF);//读取寄存器内容
  ihu_usleep(10);	
  ihu_l1hd_spi_nrf24l01_cs_disable();          //禁止SPI传输		    
  ihu_usleep(10);	
  return(reg_val);           //返回状态值
}	


/**
  * 函数功能: 在指定位置读出指定长度的数据
  * 输入参数: 无
  * 返 回 值: 此次读到的状态寄存器值 
  * 说    明：无
  *           
  */ 
uint8_t func_rfidmod_nrf24l01_read_buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status,uint8_t_ctr;	   
  
  ihu_l1hd_spi_nrf24l01_cs_enable();           //使能SPI传输
	ihu_usleep(10);
  status=ihu_l1hd_spi_nrf24l01_read_write_byte(reg);//发送寄存器值(位置),并读取状态值
	ihu_usleep(10);
 	for(uint8_t_ctr=0;uint8_t_ctr<len;uint8_t_ctr++)
  {
    pBuf[uint8_t_ctr]=ihu_l1hd_spi_nrf24l01_read_write_byte(0XFF);//读出数据
		ihu_usleep(10);
  }
  ihu_l1hd_spi_nrf24l01_cs_disable();       //关闭SPI传输
	ihu_usleep(10);
  return status;        //返回读到的状态值
}

/**
  * 函数功能: 在指定位置写指定长度的数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:寄存器(位置)  *pBuf:数据指针  len:数据长度
  *           
  */ 
uint8_t func_rfidmod_nrf24l01_write_buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,uint8_t_ctr;	    
 	ihu_l1hd_spi_nrf24l01_cs_enable();          //使能SPI传输
	ihu_usleep(10);
  status = ihu_l1hd_spi_nrf24l01_read_write_byte(reg);//发送寄存器值(位置),并读取状态值
	ihu_usleep(10);
  for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
  {
    ihu_l1hd_spi_nrf24l01_read_write_byte(*pBuf++); //写入数据
		ihu_usleep(10);
  }
  ihu_l1hd_spi_nrf24l01_cs_disable();       //关闭SPI传输
	ihu_usleep(10);
  return status;          //返回读到的状态值
}				   

/**
  * 函数功能: 检测24L01是否存在
  * 输入参数: 无
  * 返 回 值: 0，成功;1，失败
  * 说    明：无          
  */ 
int8_t func_rfidmod_nrf24l01_check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
	func_rfidmod_nrf24l01_write_buf(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_TX_ADDR,buf,5);//写入5个字节的地址.	
	func_rfidmod_nrf24l01_read_buf(IHU_VMWM_RFIDMOD_NRF24L01_TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return IHU_FAILURE;//检测24L01错误	
	return IHU_SUCCESS;		 //检测到24L01
}	 	 

/**
  * 函数功能: 启动NRF24L01发送一次数据
  * 输入参数: 无
  * 返 回 值: 发送完成状况
  * 说    明：txbuf:待发送数据首地址
  *           
  */ 
int8_t func_rfidmod_nrf24l01_tx_package(uint8_t *txbuf)
{
	uint8_t sta;
	ihu_l1hd_spi_nrf24l01_ce_low();
  func_rfidmod_nrf24l01_write_buf(IHU_VMWM_RFIDMOD_NRF24L01_WR_TX_PLOAD,txbuf,IHU_VMWM_RFIDMOD_NRF24L01_TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	ihu_l1hd_spi_nrf24l01_ce_high();//启动发送	 
  
	while(ihu_l1hd_spi_nrf24l01_irq_read()!=0);//等待发送完成
  
	sta=func_rfidmod_nrf24l01_read_reg(IHU_VMWM_RFIDMOD_NRF24L01_STATUS);  //读取状态寄存器的值	   
	func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&IHU_VMWM_RFIDMOD_NRF24L01_MAX_TX)//达到最大重发次数
	{
		func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return IHU_VMWM_RFIDMOD_NRF24L01_MAX_TX; 
	}
	if(sta&IHU_VMWM_RFIDMOD_NRF24L01_TX_OK)//发送完成
	{
		return IHU_VMWM_RFIDMOD_NRF24L01_TX_OK;
	}
	return 0xFF;//其他原因发送失败
}

/**
  * 函数功能:启动NRF24L01接收一次数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */
int8_t func_rfidmod_nrf24l01_rx_package(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
	sta=func_rfidmod_nrf24l01_read_reg(IHU_VMWM_RFIDMOD_NRF24L01_STATUS);  //读取状态寄存器的值    	 
	ihu_usleep(10);
	func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_STATUS,sta); //清除TX_DS或MAX_RT中断标志
	ihu_usleep(10);
	if(sta&IHU_VMWM_RFIDMOD_NRF24L01_RX_OK)//接收到数据
	{
		func_rfidmod_nrf24l01_read_buf(IHU_VMWM_RFIDMOD_NRF24L01_RD_RX_PLOAD,rxbuf,IHU_VMWM_RFIDMOD_NRF24L01_RX_PLOAD_WIDTH);//读取数据
		ihu_usleep(10);
		func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_FLUSH_RX,0xff);//清除RX FIFO寄存器 
		ihu_usleep(10);
		return IHU_SUCCESS; 
	}
	return IHU_FAILURE;//没收到任何数据
}

/**
  * 函数功能: 该函数初始化NRF24L01到RX模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */ 
void func_rfidmod_nrf24l01_rx_mode(void)
{
	ihu_l1hd_spi_nrf24l01_ce_low();
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_CONFIG, 0x0F);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC 
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_EN_AA,0x01);    //使能通道0的自动应答    
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_EN_RXADDR,0x01);//使能通道0的接收地址  	 
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_RF_CH,40);	     //设置RF通信频率		  
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_RX_PW_P0,IHU_VMWM_RFIDMOD_NRF24L01_RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_buf(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P0,(uint8_t*)RX_ADDRESS,IHU_VMWM_RFIDMOD_NRF24L01_RX_ADR_WIDTH);//写RX节点地址
	ihu_usleep(10);
  ihu_l1hd_spi_nrf24l01_ce_high(); //CE为高,进入接收模式 
  ihu_usleep(10);
}

/**
  * 函数功能: 该函数初始化NRF24L01到TX模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */ 
void func_rfidmod_nrf24l01_tx_mode(void)
{														 
	ihu_l1hd_spi_nrf24l01_ce_low();	    
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_buf(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_TX_ADDR,(uint8_t*)TX_ADDRESS,IHU_VMWM_RFIDMOD_NRF24L01_TX_ADR_WIDTH);//写TX节点地址 
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_buf(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_RX_ADDR_P0,(uint8_t*)RX_ADDRESS,IHU_VMWM_RFIDMOD_NRF24L01_RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_EN_AA,0x01);     //使能通道0的自动应答    
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_EN_RXADDR,0x01); //使能通道0的接收地址  
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_SETUP_RETR,0xff);//设置自动重发间隔时间:4000us + 86us;最大自动重发次数:15次
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_RF_CH,40);       //设置RF通道为40
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	ihu_usleep(10);
  func_rfidmod_nrf24l01_write_reg(IHU_VMWM_RFIDMOD_NRF24L01_NRF_WRITE_REG+IHU_VMWM_RFIDMOD_NRF24L01_CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	ihu_usleep(10);
	ihu_l1hd_spi_nrf24l01_ce_high();//CE为高,10us后启动发送
  ihu_usleep(10);
}
   

//NRF24L01模块的读取ID
OPSTAT ihu_vmmw_rfidmod_nrf24l01_spi_read_id(uint8_t *rfidAddr, uint8_t len)
{
	int res;
	
	uint32_t tickTotal = 3;
	
	//先判定设备是否存在
	res = IHU_FAILURE;
	//是不是这个模块的工作，必须先拉低CE信号？待完善
	//ihu_l1hd_spi_nrf24l01_ce_low();
	while((tickTotal > 0) && (res == IHU_FAILURE))
	{
		ihu_sleep(1); //这里的周期就是以绝对s为单位的
		tickTotal--;
		if(func_rfidmod_nrf24l01_check() == IHU_FAILURE)
			 res = IHU_FAILURE;
		else
			 res = IHU_SUCCESS;
	}
	if (res == IHU_FAILURE) IHU_ERROR_PRINT_RFIDMOD("VMMWRFID: RFID sensor (NRF24L01) in RX mode, 1st time run and not detected!\n");

	//再进入干活：这里先放3秒，太长的话，整个系统就太肉
	tickTotal = 3;
	res = IHU_FAILURE;
	while((tickTotal > 0) && (res == IHU_FAILURE))
	{
		ihu_usleep(10); //这里的周期就是以绝对s为单位的
		tickTotal--;
		func_rfidmod_nrf24l01_rx_mode();
		ihu_usleep(100);
		if(func_rfidmod_nrf24l01_rx_package(rfidAddr)==IHU_FAILURE)
			 res = IHU_FAILURE;
		else
			 res = IHU_SUCCESS;
	}	
  if(res == IHU_FAILURE) IHU_ERROR_PRINT_RFIDMOD("VMMWRFID: RFID sensor (NRF24L01) in RX mode, RFID is runnning but can not receive peer package!\n");
	
	return IHU_SUCCESS;
}








