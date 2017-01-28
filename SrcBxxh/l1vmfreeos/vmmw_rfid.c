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
OPSTAT ihu_vmmw_rfidmod_rc522_spi_send_command(uint8_t *command)
{
	return IHU_SUCCESS;
}

//读取RFID卡的数据，或者说扫描ID
OPSTAT ihu_vmmw_rfidmod_rc522_spi_read_id(uint8_t *rfidAddr, uint8_t len)
{
	int res = 0;
	char cStr[30];
  unsigned char ucArray_ID[4];  //先后存放IC卡的类型和UID(IC卡序列号)
	uint8_t ucStatusReturn;          //返回状态
	//等待的时间长度
	uint32_t tickTotal = IHU_VMWM_RFIDMOD_SCAN_RC522_MAX_TIME * 1000 / IHU_BSP_STM32_SPI_RX_MAX_DELAY;  //4秒
	
	//参数检查
	if ((rfidAddr == NULL) || (len != 4)) IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMMWRFID: invalid input paramter received!\n");
	
	//清空缓冲区
	func_rfidmod_clear_receive_buffer();
	
	//复位RC522外设
	PcdReset();
	
	//设置工作方式	
	M500PcdConfigISOType('A');

	uint8_t * UID;
	uint8_t * KEY;
	uint8_t * Dat;
	*UID = 1;
	*KEY = 0;
	*Dat = 0x01;
	*(Dat+1) = 0x02;
	*(Dat+2) = 0x03;
	*(Dat+3) = 0x04;
	//测试写入
	PcdRequest ( 0x52, ucArray_ID );//寻卡
  PcdAnticoll ( ucArray_ID );//防冲撞
  PcdSelect ( UID );//选定卡
  PcdAuthState ( 0x60, 0x10, KEY, UID );//校验
  PcdWrite ( 0x10, Dat );
  PcdRead ( 0x10, Dat );

	//循环读取
	res = IHU_FAILURE;
	while((tickTotal > 0) && (res == IHU_FAILURE))
	{
		ihu_usleep(IHU_BSP_STM32_SPI_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
		tickTotal--;
    /*寻卡*/
		if ( ( ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ) ) != MI_OK )                                   
			 /*若失败再次寻卡*/
      ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID );		
		IhuDebugPrint("VMMWRFID: Step1 reach.\n");
		if ( ucStatusReturn == MI_OK  )
		{
      /*防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
			IhuDebugPrint("VMMWRFID: Step2 reach.\n");
			if ( PcdAnticoll(ucArray_ID) == MI_OK )  
			{
				memcpy(rfidAddr, ucArray_ID, 4);
				sprintf(cStr, "The Card ID is: %02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3] );							
				IhuDebugPrint("VMMWRFID: [%s]\n", cStr);
				res = IHU_SUCCESS;
				PcdHalt (); 		
      }
		}
	}
	if (res == IHU_FAILURE) IhuDebugPrint("VMMWRFID: Fail to read RFID value!\n");
	return res;	
	
//	//接收看看
//	uint8_t *p1;
//	char s[3];
//	int i = 0;
//	
//	if (func_rfidmod_wait_command_fb("Card Id is:", 4) == IHU_FAILURE) IHU_ERROR_PRINT_TASK(TASK_ID_VMFO, "VMMWRFID: RFID fetch lable failure!\n");
//	
//#if (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI1)
//	p1 = (uint8_t*)strstr((const char*)zIhuBspStm32SpiGeneral1RxBuff, "Card Id is:");
//#elif (IHU_VMWM_RFIDMOD_USING_ITF_SET == IHU_VMWM_RFIDMOD_USING_ITF_SPI2)
//	p1 = (uint8_t*)strstr((const char*)zIhuBspStm32SpiGeneral2RxBuff, "Card Id is:");
//#else
//	#error Un-correct constant definition
//#endif
//	p1 = p1 + sizeof("Card Id is:")-1;
//	for (i = 0; i < len; i++){
//		memset(s, 0, sizeof(s));
//		memcpy((uint8_t*)s, p1+i*2, 2);
//		res = strtoul(s, NULL, 16) & 0xFF;
//		memcpy(rfidAddr+i, &res, 1);
//	}
//	IhuDebugPrint("VMMWRFID: Received RFID value = [%s]\n", p1);
//	
//	return IHU_SUCCESS;
}


/*
 * 函数名：ReadRawRC
 * 描述  ：读RC522寄存器
 * 输入  ：ucAddress，寄存器地址
 * 返回  : 寄存器的当前值
 * 调用  ：内部调用
 */
uint8_t ReadRawRC ( uint8_t ucAddress )
{
	uint8_t ucAddr, ucReturn;
	
	ucAddr = ( ( ucAddress << 1 ) & 0x7E ) | 0x80;
	macRC522_CS_Enable() ;
	SPI_FLASH_SendByte ( ucAddr );
	ucReturn = SPI_FLASH_ReadByte ();
	macRC522_CS_Disable();
	return ucReturn;
}

/*
 * 函数名：WriteRawRC
 * 描述  ：写RC522寄存器
 * 输入  ：ucAddress，寄存器地址
 *         ucValue，写入寄存器的值
 * 返回  : 无
 * 调用  ：内部调用
 */
void WriteRawRC ( uint8_t ucAddress, uint8_t ucValue )
{  
	uint8_t ucAddr;
	
	ucAddr = ( ucAddress << 1 ) & 0x7E;
	macRC522_CS_Enable() ;
	SPI_FLASH_SendByte ( ucAddr );
	SPI_FLASH_SendByte ( ucValue );
	macRC522_CS_Disable();	
}

/*
 * 函数名：SetBitMask
 * 描述  ：对RC522寄存器置位
 * 输入  ：ucReg，寄存器地址
 *         ucMask，置位值
 * 返回  : 无
 * 调用  ：内部调用
 */
void SetBitMask ( uint8_t ucReg, uint8_t ucMask )  
{
    uint8_t ucTemp;
	
    ucTemp = ReadRawRC ( ucReg );
    WriteRawRC ( ucReg, ucTemp | ucMask );         // set bit mask
}

/*
 * 函数名：ClearBitMask
 * 描述  ：对RC522寄存器清位
 * 输入  ：ucReg，寄存器地址
 *         ucMask，清位值
 * 返回  : 无
 * 调用  ：内部调用
 */
void ClearBitMask ( uint8_t ucReg, uint8_t ucMask )  
{
    uint8_t ucTemp;
	
    ucTemp = ReadRawRC ( ucReg );
    WriteRawRC ( ucReg, ucTemp & ( ~ ucMask) );  // clear bit mask
}

/*
 * 函数名：PcdAntennaOn
 * 描述  ：开启天线 
 * 输入  ：无
 * 返回  : 无
 * 调用  ：内部调用
 */
void PcdAntennaOn ( void )
{
    uint8_t uc;
	
    uc = ReadRawRC ( TxControlReg );
    if ( ! ( uc & 0x03 ) )
			SetBitMask(TxControlReg, 0x03);
}


/*
 * 函数名：PcdAntennaOff
 * 描述  ：开启天线 
 * 输入  ：无
 * 返回  : 无
 * 调用  ：内部调用
 */
void PcdAntennaOff ( void )
{
  ClearBitMask ( TxControlReg, 0x03 );
}

/*
 * 函数名：PcdRese
 * 描述  ：复位RC522 
 * 输入  ：无
 * 返回  : 无
 * 调用  ：外部调用
 */
void PcdReset ( void )
{
	macRC522_Reset_Disable();
	HAL_Delay ( 1 );
	macRC522_Reset_Enable();
	HAL_Delay ( 1 );
	macRC522_Reset_Disable();
	HAL_Delay ( 1 );
	WriteRawRC ( CommandReg, 0x0f );
	while ( ReadRawRC ( CommandReg ) & 0x10 );
	HAL_Delay ( 1 );
  WriteRawRC ( ModeReg, 0x3D );            //定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
  WriteRawRC ( TReloadRegL, 30 );          //16位定时器低位    
	WriteRawRC ( TReloadRegH, 0 );			     //16位定时器高位
  WriteRawRC ( TModeReg, 0x8D );				   //定义内部定时器的设置
  WriteRawRC ( TPrescalerReg, 0x3E );			 //设置定时器分频系数
	WriteRawRC ( TxAutoReg, 0x40 );				   //调制发送信号为100%ASK
	PcdAntennaOff();
  HAL_Delay ( 1 );
  PcdAntennaOn();
}

/*
 * 函数名：M500PcdConfigISOType
 * 描述  ：设置RC522的工作方式
 * 输入  ：ucType，工作方式
 * 返回  : 无
 * 调用  ：外部调用
 */
void M500PcdConfigISOType ( uint8_t ucType )
{
	if ( ucType == 'A')                     //ISO14443_A
  {
		ClearBitMask ( Status2Reg, 0x08 );
    WriteRawRC ( ModeReg, 0x3D );//3F
		WriteRawRC ( RxSelReg, 0x86 );//84
		WriteRawRC( RFCfgReg, 0x7F );   //4F
		WriteRawRC( TReloadRegL, 30 );//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		WriteRawRC ( TReloadRegH, 0 );
		WriteRawRC ( TModeReg, 0x8D );
		WriteRawRC ( TPrescalerReg, 0x3E );
		HAL_Delay ( 2 );
		PcdAntennaOn ();//开天线
   }
}

/*
 * 函数名：PcdComMF522
 * 描述  ：通过RC522和ISO14443卡通讯
 * 输入  ：ucCommand，RC522命令字
 *         pInData，通过RC522发送到卡片的数据
 *         ucInLenByte，发送数据的字节长度
 *         pOutData，接收到的卡片返回数据
 *         pOutLenBit，返回数据的位长度
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：内部调用
 */
char PcdComMF522 ( uint8_t ucCommand, uint8_t * pInData, uint8_t ucInLenByte, uint8_t * pOutData, uint32_t * pOutLenBit )		
{
    char cStatus = MI_ERR;
    uint8_t ucIrqEn   = 0x00;
    uint8_t ucWaitFor = 0x00;
    uint8_t ucLastBits;
    uint8_t ucN;
    uint32_t ul;
	
    switch ( ucCommand )
    {
       case PCD_AUTHENT:		//Mifare认证
          ucIrqEn   = 0x12;		//允许错误中断请求ErrIEn  允许空闲中断IdleIEn
          ucWaitFor = 0x10;		//认证寻卡等待时候 查询空闲中断标志位
          break;
       case PCD_TRANSCEIVE:		//接收发送 发送接收
          ucIrqEn   = 0x77;		//允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
          ucWaitFor = 0x30;		//寻卡等待时候 查询接收中断标志位与 空闲中断标志位
          break;
       default:
         break;
    }
    WriteRawRC ( ComIEnReg, ucIrqEn | 0x80 );		//IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反 
    ClearBitMask ( ComIrqReg, 0x80 );			//Set1该位清零时，CommIRqReg的屏蔽位清零
    WriteRawRC ( CommandReg, PCD_IDLE );		//写空闲命令
    SetBitMask ( FIFOLevelReg, 0x80 );			//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
    for ( ul = 0; ul < ucInLenByte; ul ++ )
		  WriteRawRC ( FIFODataReg, pInData [ ul ] );    		//写数据进FIFOdata
    WriteRawRC ( CommandReg, ucCommand );					//写命令
    if ( ucCommand == PCD_TRANSCEIVE )
			SetBitMask(BitFramingReg,0x80);  				//StartSend置位启动数据发送 该位与收发命令使用时才有效
    ul = 1000;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do 														//认证 与寻卡等待时间	
    {
         ucN = ReadRawRC ( ComIrqReg );							//查询事件中断
         ul --;
    } while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//退出条件i=0,定时器中断，与写空闲命令
    ClearBitMask ( BitFramingReg, 0x80 );					//清理允许StartSend位
    if ( ul != 0 )
    {
			if ( ! ( ReadRawRC ( ErrorReg ) & 0x1B ) )			//读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
			{
				cStatus = MI_OK;
				if ( ucN & ucIrqEn & 0x01 )					//是否发生定时器中断
				  cStatus = MI_NOTAGERR;   
				if ( ucCommand == PCD_TRANSCEIVE )
				{
					ucN = ReadRawRC ( FIFOLevelReg );			//读FIFO中保存的字节数
					ucLastBits = ReadRawRC ( ControlReg ) & 0x07;	//最后接收到得字节的有效位数
					if ( ucLastBits )
						* pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
					else
						* pOutLenBit = ucN * 8;   					//最后接收到的字节整个字节有效
					if ( ucN == 0 )		
            ucN = 1;    
					if ( ucN > MAXRLEN )
						ucN = MAXRLEN;   
					for ( ul = 0; ul < ucN; ul ++ )
					  pOutData [ ul ] = ReadRawRC ( FIFODataReg );   
					}
      }
			else
				cStatus = MI_ERR;   
    }
   SetBitMask ( ControlReg, 0x80 );           // stop timer now
   WriteRawRC ( CommandReg, PCD_IDLE ); 
   return cStatus;
}

/*
 * 函数名：PcdRequest
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
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdRequest ( uint8_t ucReq_code, uint8_t * pTagType )
{
   char cStatus;  
	 uint8_t ucComMF522Buf [ MAXRLEN ]; 
   uint32_t ulLen;
	
   ClearBitMask ( Status2Reg, 0x08 );	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
   WriteRawRC ( BitFramingReg, 0x07 );	//	发送的最后一个字节的 七位
   SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号
   ucComMF522Buf [ 0 ] = ucReq_code;		//存入 卡片命令字
   cStatus = PcdComMF522 ( PCD_TRANSCEIVE,	ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//寻卡  
   if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	//寻卡成功返回卡类型 
   {    
       * pTagType = ucComMF522Buf [ 0 ];
       * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
   }
   else
     cStatus = MI_ERR;
   return cStatus;
}

/*
 * 函数名：PcdAnticoll
 * 描述  ：防冲撞
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdAnticoll ( uint8_t * pSnr )
{
    char cStatus;
    uint8_t uc, ucSnr_check = 0;
    uint8_t ucComMF522Buf [ MAXRLEN ]; 
	  uint32_t ulLen;
 
    ClearBitMask ( Status2Reg, 0x08 );		//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
    WriteRawRC ( BitFramingReg, 0x00);		//清理寄存器 停止收发
    ClearBitMask ( CollReg, 0x80 );			  //清ValuesAfterColl所有接收的位在冲突后被清除
    ucComMF522Buf [ 0 ] = PICC_ANTICOLL1;	//卡片防冲突命令
    ucComMF522Buf [ 1 ] = 0x20;
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//与卡片通信
    if ( cStatus == MI_OK)		//通信成功
    {
			for ( uc = 0; uc < 4; uc ++ )
			{
         * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//读出UID
         ucSnr_check ^= ucComMF522Buf [ uc ];
      }
      if ( ucSnr_check != ucComMF522Buf [ uc ] )
				cStatus = MI_ERR;    
    }
    SetBitMask ( CollReg, 0x80 );		
    return cStatus;
}

/*
 * 函数名：CalulateCRC
 * 描述  ：用RC522计算CRC16
 * 输入  ：pIndata，计算CRC16的数组
 *         ucLen，计算CRC16的数组字节长度
 *         pOutData，存放计算结果存放的首地址
 * 返回  : 无
 * 调用  ：内部调用
 */
void CalulateCRC ( uint8_t * pIndata, uint8_t ucLen, uint8_t * pOutData )
{
    uint8_t uc, ucN;
	
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg, PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for ( uc = 0; uc < ucLen; uc ++)
	    WriteRawRC ( FIFODataReg, * ( pIndata + uc ) );   

    WriteRawRC ( CommandReg, PCD_CALCCRC );
    uc = 0xFF;
    do 
    {
        ucN = ReadRawRC ( DivIrqReg );
        uc --;
    } while ( ( uc != 0 ) && ! ( ucN & 0x04 ) );
		
    pOutData [ 0 ] = ReadRawRC ( CRCResultRegL );
    pOutData [ 1 ] = ReadRawRC ( CRCResultRegM );
}

/*
 * 函数名：PcdSelect
 * 描述  ：选定卡片
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdSelect ( uint8_t * pSnr )
{
    char ucN;
    uint8_t uc;
	  uint8_t ucComMF522Buf [ MAXRLEN ]; 
    uint32_t  ulLen;
    
    ucComMF522Buf [ 0 ] = PICC_ANTICOLL1;
    ucComMF522Buf [ 1 ] = 0x70;
    ucComMF522Buf [ 6 ] = 0;
    for ( uc = 0; uc < 4; uc ++ )
    {
    	ucComMF522Buf [ uc + 2 ] = * ( pSnr + uc );
    	ucComMF522Buf [ 6 ] ^= * ( pSnr + uc );
    }
    CalulateCRC ( ucComMF522Buf, 7, & ucComMF522Buf [ 7 ] );
    ClearBitMask ( Status2Reg, 0x08 );
    ucN = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, & ulLen );
    if ( ( ucN == MI_OK ) && ( ulLen == 0x18 ) )
      ucN = MI_OK;  
    else
      ucN = MI_ERR;    
    return ucN;		
}

/*
 * 函数名：PcdAuthState
 * 描述  ：验证卡片密码
 * 输入  ：ucAuth_mode，密码验证模式
 *                     = 0x60，验证A密钥
 *                     = 0x61，验证B密钥
 *         uint8_t ucAddr，块地址
 *         pKey，密码
 *         pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdAuthState ( uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr )
{
    char cStatus;
	  uint8_t uc, ucComMF522Buf [ MAXRLEN ];
    uint32_t ulLen;
    
    ucComMF522Buf [ 0 ] = ucAuth_mode;
    ucComMF522Buf [ 1 ] = ucAddr;
    for ( uc = 0; uc < 6; uc ++ )
	    ucComMF522Buf [ uc + 2 ] = * ( pKey + uc );   
    for ( uc = 0; uc < 6; uc ++ )
	    ucComMF522Buf [ uc + 8 ] = * ( pSnr + uc );   
    cStatus = PcdComMF522 ( PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, & ulLen );
    if ( ( cStatus != MI_OK ) || ( ! ( ReadRawRC ( Status2Reg ) & 0x08 ) ) )
      cStatus = MI_ERR;   
    return cStatus;			
}

/*
 * 函数名：PcdWrite
 * 描述  ：写数据到M1卡一块
 * 输入  ：uint8_t ucAddr，块地址
 *         pData，写入的数据，16字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdWrite ( uint8_t ucAddr, uint8_t * pData )
{
    char cStatus;
	  uint8_t uc, ucComMF522Buf [ MAXRLEN ];
    uint32_t ulLen;
     
    ucComMF522Buf [ 0 ] = PICC_WRITE;
    ucComMF522Buf [ 1 ] = ucAddr;
    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
    if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
      cStatus = MI_ERR;   
    if ( cStatus == MI_OK )
    {
			//memcpy(ucComMF522Buf, pData, 16);
      for ( uc = 0; uc < 16; uc ++ )
			  ucComMF522Buf [ uc ] = * ( pData + uc );  
      CalulateCRC ( ucComMF522Buf, 16, & ucComMF522Buf [ 16 ] );
      cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, & ulLen );
			if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
        cStatus = MI_ERR;   
    } 	
    return cStatus;
}

/*
 * 函数名：PcdRead
 * 描述  ：读取M1卡一块数据
 * 输入  ：uint8_t ucAddr，块地址
 *         pData，读出的数据，16字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdRead ( uint8_t ucAddr, uint8_t * pData )
{
    char cStatus;
	  uint8_t uc, ucComMF522Buf [ MAXRLEN ]; 
    uint32_t ulLen;

    ucComMF522Buf [ 0 ] = PICC_READ;
    ucComMF522Buf [ 1 ] = ucAddr;
    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
    if ( ( cStatus == MI_OK ) && ( ulLen == 0x90 ) )
    {
			for ( uc = 0; uc < 16; uc ++ )
        * ( pData + uc ) = ucComMF522Buf [ uc ];   
    }
    else
      cStatus = MI_ERR;   		
    return cStatus;
}

/*
 * 函数名：PcdHalt
 * 描述  ：命令卡片进入休眠状态
 * 输入  ：无
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdHalt( void )
{
	uint8_t ucComMF522Buf [ MAXRLEN ]; 
	uint32_t  ulLen;
  
  ucComMF522Buf [ 0 ] = PICC_HALT;
  ucComMF522Buf [ 1 ] = 0;
  CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
 	PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
  return MI_OK;
}

void IC_CMT ( uint8_t * UID, uint8_t * KEY, uint8_t RW, uint8_t * Dat )
{
  uint8_t ucArray_ID [ 4 ] = { 0 };//先后存放IC卡的类型和UID(IC卡序列号)
  
  PcdRequest ( 0x52, ucArray_ID );//寻卡
  PcdAnticoll ( ucArray_ID );//防冲撞
  PcdSelect ( UID );//选定卡
  PcdAuthState ( 0x60, 0x10, KEY, UID );//校验
	if ( RW )//读写选择，1是读，0是写
    PcdRead ( 0x10, Dat );
   else 
     PcdWrite ( 0x10, Dat );
   PcdHalt (); 
}





