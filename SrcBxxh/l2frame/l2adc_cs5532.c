///**
// ****************************************************************************************
// *
// * @file l2adc_cs5532.c
// *
// * @brief L2 ADC API for CS5532/34-BS from Cirrus Logic,
// *
// * BXXH team
// * Created by MYC, 20170206
// *
// ****************************************************************************************
// */

////#include   "includes.h"
//#include   "l2adc_cs5532.h"
//#include		"comtype.h"


//typedef struct StructUint32
//      {
//       unsigned char  uint8_3;
//       unsigned char  uint8_2;
//       unsigned char  uint8_1;
//       unsigned char  uint8_0;
//      }Uint32Struct1;

//typedef union UnionUint32
//      {
//        Uint32Struct1         Uint32Struct;
//        unsigned int          Uint32Data;
//      }Uint32Union;

///*转换速率　　　0～9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240*/
//uint8_t  CollectionFF =       0x01      ;          //采样频率
///*通道0增益     0～6   0:1  1:2  2:4  3:8  4:16  5:32  6:64*/ 
//// uint8_t  Chan0Gain    =       0x02;
//uint8_t  Chan0Gain    =       0x04;


//#define       CS5532_SPI_PORT           0
//#define       CS5532_SPI_PIN_SCLK       (1UL << 15)
//#define       CS5532_SPI_PIN_MISO       (1UL << 17)
//#define       CS5532_SPI_PIN_MOSI       (1UL << 18)
//#define       CS5532_SPI_PIN_CS         (1UL << 16)
//			
//#define       FALSE         (0)
//#define       TRUE         	(1)
//			


//#define MISO_SDO        (GPIO_ReadValue(0)  & CS5532_SPI_PIN_MISO)
////#define Clr_CS5532()    GPIO_ClearValue(CS5532_SPI_PORT,CS5532_SPI_PIN_CS)
//#define Clr_CS5532()		HAL_GPIO_WritePin(CUBEMX_PIN_F2_ADC_CS_GPIO_Port, CUBEMX_PIN_F2_ADC_CS_Pin, GPIO_PIN_RESET);
////#define Set_CS5532()    GPIO_SetValue(CS5532_SPI_PORT,CS5532_SPI_PIN_CS)
//#define Set_CS5532()		HAL_GPIO_WritePin(CUBEMX_PIN_F2_ADC_CS_GPIO_Port, CUBEMX_PIN_F2_ADC_CS_Pin, GPIO_PIN_SET);

//void AD_delay(uint32_t val)
//{
//  while(val--)
//  {
//  }
//}

///****************************************************************************
//* 名称：MSPI0_WR_Data()
//* 功能：向SSI控制器发送数据。
//* 入口参数：send_data
//* 出口参数：无
//* 说明：
//****************************************************************************/
//uint8_t MSPI0_WR_Data(uint8_t send_data)
//{ 
//   //SPI_SendData(LPC_SPI,send_data);
//   return(0);
//}
///****************************************************************************
//* 名称：MSPI0_RD_Data()
//* 功能：向SSI控制器接收数据。
//* 入口参数：NONE
//* 出口参数：无
//* 说明：
//****************************************************************************/
//uint8_t MSPI0_RD_Data()
//{
//  uint8_t rec;

//  //SPI_SendData(LPC_SPI,0x00);
//  //rec = SPI_ReceiveData(LPC_SPI);

//  return(rec);
//}

///*
//	
///****************************************************************************
//* 名称：CS5532Sendcmd()
//* 功能：发送命令。
//* 入口参数：无
//* 出口参数：无
//* 说明：
//****************************************************************************/

//void  CS5532SendCmd(uint8_t cmd)
//{
//    MSPI0_WR_Data(cmd);
//}

///****************************************************************************
//* 名称：CS5532SendData()
//* 功能：发送数据。
//* 入口参数：Data　要发送的数据
//* 出口参数：无
//* 说明：
//****************************************************************************/

//void  CS5532SendData(uint32_t Data)
//{
//       Uint32Union DataTemp;
//    
//       DataTemp.Uint32Data = Data; 
//       MSPI0_WR_Data(DataTemp.Uint32Struct.uint8_0);
//       MSPI0_WR_Data(DataTemp.Uint32Struct.uint8_1);
//       MSPI0_WR_Data(DataTemp.Uint32Struct.uint8_2);
//       MSPI0_WR_Data(DataTemp.Uint32Struct.uint8_3);
//}


///****************************************************************************
//* 名称：CS5532ReadData()
//* 功能：接收数据。
//* 入口参数：无
//* 出口参数：接收的数据
//* 说明：
//****************************************************************************/

//uint32_t  CS5532ReadData(void)
//{
//    Uint32Union DataTemp;
//    
//    DataTemp.Uint32Struct.uint8_0 = MSPI0_RD_Data();
//    DataTemp.Uint32Struct.uint8_1 = MSPI0_RD_Data();
//    DataTemp.Uint32Struct.uint8_2 = MSPI0_RD_Data();
//    DataTemp.Uint32Struct.uint8_3 = MSPI0_RD_Data();
//    
//    return DataTemp.Uint32Data;
//}

///****************************************************************************
//* 名称：WriteOffsetReg()
//* 功能：写偏移寄存器。
//* 入口参数：ChannelsID　　通道号，不能大于1
//* 　　　　　Data　　　　　要写入偏移寄存器的值
//* 出口参数：无
//* 说明：
//****************************************************************************/

//uint8_t  WriteOffsetReg(uint8_t ChannelsID,uint32_t Data)
//{
//    if(ChannelsID > 1)
//        return FALSE;
//	 Clr_CS5532();

//     CS5532SendCmd((ChannelsID << 4)|0x01);
//     CS5532SendData(Data);

//	 Set_CS5532();
//	 return TRUE;
//}

///****************************************************************************
//* 名称：ReadOffsetReg()
//* 功能：读偏移寄存器。
//* 入口参数：ChannelsID　　通道号，不能大于1
//* 出口参数：读出的偏移寄存器值
//* 说明：
//****************************************************************************/

//uint32_t  ReadOffsetReg(uint8_t ChannelsID)
//{
//    uint32_t DataTemp;
//    
//    if(ChannelsID > 1)
//        return 0;
//    Clr_CS5532();
//		   
//    CS5532SendCmd((ChannelsID<<4)|0x09);
//    DataTemp = CS5532ReadData();

//    Set_CS5532(); 
//    return DataTemp;
//}

///****************************************************************************
//* 名称：WriteGainReg()
//* 功能：写增益寄存器。
//* 入口参数：ChannelsID　　通道号，不能大于1
//* 　　　　　Data　　　　　要写入增益寄存器的值
//* 出口参数：无
//* 说明：
//****************************************************************************/

//uint8_t  WriteGainReg(uint8_t ChannelsID,uint32_t Data)
//{
//    if(ChannelsID > 1)
//        return FALSE;
//	Clr_CS5532();

//	CS5532SendCmd((ChannelsID<<4)|0x02);
//    CS5532SendData(Data);

//	Set_CS5532();
//	return TRUE;
//}

///****************************************************************************
//* 名称：ReadGainReg()
//* 功能：读增益寄存器。
//* 入口参数：ChannelsID　　通道号，不能大于1
//* 出口参数：读出的增益寄存器值
//* 说明：
//****************************************************************************/

//uint32_t  ReadGainReg(uint8_t ChannelsID)
//{
//    uint32_t DataTemp;
//    
//    if(ChannelsID > 1)
//        return 0;
//    Clr_CS5532();
//		  
//    CS5532SendCmd((ChannelsID<<4)|0x0A);
//    DataTemp = CS5532ReadData();

//    Set_CS5532();  
//    return DataTemp;
//}

///****************************************************************************
//* 名称：WriteChannelSetupReg()
//* 功能：写通道设置寄存器。
//* 入口参数：RegID　　设置寄存器号，不能大于3
//* 　　　　　Data　　　　　要写入通道设置寄存器的值
//* 出口参数：无
//* 说明：
//****************************************************************************/

//uint8_t  WriteChannelSetupReg(uint8_t RegID,uint32_t Data)
//{
//    if(RegID > 3)
//        return FALSE;
//		
//	Clr_CS5532();
//		
//    CS5532SendCmd((RegID << 4) | 0x05);
//    CS5532SendData(Data);

//	Set_CS5532();
//	return TRUE;
//}

///****************************************************************************
//* 名称：ReadChannelSetupReg()
//* 功能：读通道设置寄存器。
//* 入口参数：RegID　　设置寄存器号，不能大于3
//* 出口参数：读出的通道设置寄存器值
//* 说明：
//****************************************************************************/

//uint32_t  ReadChannelSetupReg(uint8_t RegID)
//{
//    uint32_t DataTemp;
//    
//    if(RegID > 3)
//        return 0;

//    Clr_CS5532();

//    CS5532SendCmd((RegID<<4)|0x0D);
//    DataTemp = CS5532ReadData();
//	
//	Set_CS5532();
//    return DataTemp;
//}

///****************************************************************************
//* 名称：WriteChannelSetupRegEx()
//* 功能：写通道设置寄存器扩展。
//* 入口参数：SetupID　　设置寄存器号　0～7
//* 　　　　　ChannelID　转换通道号　　0～1
//* 　　　　　Gain　　　 增益　　　　　0～6   0:1  1:2  2:4  3:8  4:16  5:32  6:64 
//* 　　　　　WordRate　 转换速率　　　0～9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 
//* 　　　　　UorB　　　 极性模式　　　0～1   0:单极性　1:双极性    
//* 　　　　　TempA0A1　 A0A1寄存器值　0～3
//* 出口参数：无
//* 说明：
//****************************************************************************/
//uint8_t  WriteChannelSetupRegEx(uint8_t SetupID,uint8_t ChannelID,uint8_t Gain,uint8_t WordRate,uint8_t UorB,uint8_t TempA0A1)
//{
//	uint32_t TempData32 = 0x00000000;
//	uint32_t TempData;
//	uint8_t  TempData8;
//	
//	if(SetupID > 0x07)
//	    return FALSE;
//	if(ChannelID > 0x01)
//	    return FALSE;
//	if(Gain > 0x06)
//	    return FALSE;
//	if(WordRate > 0x09)
//	    return FALSE;
//	if(UorB > 0x01)
//	    return FALSE;
//	if(TempA0A1 > 0x03)
//	    return FALSE;

//	TempData32 = ReadChannelSetupReg(SetupID/2);
//	TempData8 = 16*((SetupID+1)%2);
//	
//	TempData = 0x00000003;
//	TempData32 &= ~(TempData<<(14+TempData8));
//	TempData = ChannelID;
//	TempData32 |= TempData<<(14+TempData8);
//		
//	TempData = 0x00000007;
//	TempData32 &= ~(TempData<<(11+TempData8));
//	TempData = Gain;
//	TempData32 |= TempData<<(11+TempData8);
//	
//	TempData = 0x0000000F;
//	TempData32 &= ~(TempData<<(7+TempData8));
//	TempData = WordRate;
//	if(TempData > 0x04)
//	    TempData += 0x03;
//	TempData32 |= TempData<<(7+TempData8);
//	
//	TempData = 0x00000001;
//	TempData32 &= ~(TempData<<(6+TempData8));
//	TempData = UorB;
//	TempData32 |= TempData<<(6+TempData8);
//	
//	TempData = 0x00000003;
//	TempData32 &= ~(TempData<<(4+TempData8));
//	TempData = TempA0A1;
//	TempData32 |= TempData<<(4+TempData8);
//	WriteChannelSetupReg(SetupID/2,TempData32);
//	return TRUE;
//}

///****************************************************************************
//* 名称：WriteConfigReg()
//* 功能：写配置寄存器。
//* 入口参数：
//* 　　　　　Data　　　　　要写入配置寄存器的值
//* 出口参数：无
//* 说明：
//****************************************************************************/

//uint8_t  WriteConfigReg(uint32_t Data)
//{
//    Clr_CS5532();	
//    
//    CS5532SendCmd(0x03);
//    CS5532SendData(Data);

//	Set_CS5532();

//    return TRUE;
//}

///****************************************************************************
//* 名称：WriteConfigRegEx()
//* 功能：写配置寄存器扩展。
//* 入口参数：TempIs：　正常输入或短接地　0：正常输入　1　短接地
//* 　　　　　TempA0A1：A0A1的寄存器值
//* 出口参数：返回是否设置成功。
//* 说明： 00 02
//****************************************************************************/

//uint8_t  WriteConfigRegEx(uint8_t TempIs,uint8_t TempA0A1)
//{
//    #define OLS   0x00      //A1A2设置源　　　0　通道寄存器　1　配置寄存器
//	#define VRS   0x00      //电压参考选择
//	
//	uint32_t TempData32 = 0x00000000;
//	uint32_t TempData;
//	
//	if(TempIs > 0x01)
//	    return FALSE;
//	if(TempA0A1 > 0x03)
//	    return FALSE;
//		
//	TempData = 0x00000001;
//	TempData32 &= ~(TempData<<27);
//	TempData    = TempIs;
//	TempData32 |= (TempData<<27);
//    
//	TempData = 0x00000001;
//	TempData32 &= ~(TempData<<25);
//	TempData    = VRS;
//	TempData32 |= (TempData<<25);
//	
//	TempData = 0x00000003;
//	TempData32 &= ~(TempData<<23);
//	TempData = TempA0A1;
//	TempData32 |= (TempData<<23);
//	
//	TempData = 0x00000001;
//	TempData32 &= ~(TempData<<22);
//	TempData    = OLS;
//	TempData32 |= (TempData<<22);

//	WriteConfigReg(TempData32);
//	return TRUE;
//}

///****************************************************************************
//* 名称：ReadConfigReg()
//* 功能：读通道配置寄存器。
//* 入口参数：
//* 出口参数：读出的配置寄存器值
//* 说明：
//****************************************************************************/

//uint32_t  ReadConfigReg(void)
//{
//    uint32_t DataTemp;
//	 
//    Clr_CS5532();

//    CS5532SendCmd(0x0B);
//    DataTemp = CS5532ReadData();

//    Set_CS5532();
//    return DataTemp;
//}




//uint8_t ReadWheChanOk(void)
//{
//  if(MISO_SDO)
//  {
//    return FALSE;
//  }
//  return TRUE;
//}
///****************************************************************************
//* 名称：ReadSOD0()
//* 功能：等待转换完成。
//* 入口参数：无
//* 出口参数：无
//* 说明：
//****************************************************************************/
//void ReadSOD0(void)
//{
//   while(MISO_SDO);
//}

///****************************************************************************
//* 名称：PerformCalibration()
//* 功能：根据通道设置寄存器指针指向的寄存器中的参数进行校准。
//* 入口参数：CC  　　1:自偏移校准
//　　　　　　　　　　2:自增益校准
//　　　　　　　　　　5:系统偏移校准
//　　　　　　　　　　6:系统增益校准
//　　　　　　CSRP　　通道设置寄存器指针　0~7
//* 出口参数：是否成功
//* 说明：
//****************************************************************************/

//uint8_t  PerformCalibration(uint8_t CC,uint8_t CSRP)
//{
//    if(CC >7 )
//     return FALSE;
//    if(CSRP >7 )
//     return FALSE;
//    Clr_CS5532(); 
//	   
//    CS5532SendCmd(CC|(CSRP<<3)|0x80);
//     ReadSOD0();

//	Set_CS5532();
//    return TRUE;
//}

///****************************************************************************
//* 名称：ReadADDataReg()
//* 功能：读AD转换的数据寄存器。
//* 入口参数：MC  　　0:单次转换
//　　　　　　　　　　1:连续转换
//　　　　　　CSRP　　通道设置寄存器指针　0~7
//* 出口参数：读出的数据寄存器值
//* 说明：
//****************************************************************************/
//uint32_t  ReadADDataReg(uint8_t MC,uint8_t CSRP)
//{
//    uint32_t DataTemp;
//    
//    if(MC >1 )
//        return FALSE;
//    if(CSRP >7 )
//        return FALSE;

//    Clr_CS5532();
//		
//    CS5532SendCmd((MC<<6)|(CSRP<<3)|0x80);

//	if(MC != 0x00)
//	{
//	   return FALSE;
//	}

//    ReadSOD0();
//    MSPI0_WR_Data(0x00);
//    DataTemp = CS5532ReadData();

//    Set_CS5532(); 
//    return DataTemp;
//}

///****************************************************************************
//* 名称：ReadADValue()
//* 功能：读AD值。
//* 入口参数：ChannelsID:要读ＡＤ的通道号
//* 出口参数：读出连续的AD数据值
//* 说明：
//****************************************************************************/
//uint32_t ReadADValue(uint8_t ChannelsID)
//{
//    uint32_t TempData;
//    
//    TempData = ReadADDataReg(0x00,ChannelsID);
//    TempData = TempData >> 8;
//    return TempData;
//}
///****************************************************************************
//* 名称：StartSeriesConversion()
//* 功能：开始连续转换。
//* 入口参数：ChannelsID:要转换的ＡＤ通道号
//* 出口参数：
//* 说明：
//****************************************************************************/
//uint32_t StartSeriesConversion(uint8_t ChannelsID)
//{
//    uint32_t TempData;
//    
//    TempData = ReadADDataReg(0x01,ChannelsID);
//	
//    return TempData;
//}

///****************************************************************************
//* 名称：StopSeriesConversion()
//* 功能：停止连续转换。
//* 入口参数：无
//* 出口参数：
//* 说明：
//****************************************************************************/
//uint32_t StopSeriesConversion(void)
//{
//    uint32_t TempData;
//    
//    Clr_CS5532();
//		
//    ReadSOD0();
//    MSPI0_WR_Data(0xFF);
//    TempData = CS5532ReadData();

//	Set_CS5532(); 
//	TempData = TempData >> 8;
//    return TempData;
//}

///****************************************************************************
//* 名称：ReadSeriesADValue()
//* 功能：读连续转换AD值。
//* 入口参数：无
//* 出口参数：读出的AD数据值
//* 说明：
//****************************************************************************/
//uint32_t ReadSeriesADValue(void)
//{
//    uint32_t TempData;


//    MSPI0_WR_Data(0x00);
//    TempData = CS5532ReadData();
//	TempData = TempData >> 8;

//    return TempData;
//}              
//	            

///****************************************************************************
//* 名称：CS5532InCmdMode()
//* 功能：系统初始化，使ＡＤ转换器CS5532芯片进入等侍命令模式
//* 入口参数：无
//* 出口参数：无
//* 说明：
//****************************************************************************/

//void   CS5532InCmdMode(void)
//{
//    uint8_t TempData;
//	Clr_CS5532();

//    for(TempData=0;TempData<40;TempData++)
//    {
//	    MSPI0_WR_Data(0xFF);
//    }
//	MSPI0_WR_Data(0xfE);

//	Set_CS5532();
//}


///****************************************************************************
//* 名称：CS5532Reset()
//* 功能：系统复位。
//* 入口参数：无
//* 出口参数：无
//* 说明：
//****************************************************************************/

//void  CS5532Reset(void)
//{
//    WriteConfigReg(0x20000000);
//    while(!(ReadConfigReg()&0x10000000));
//}
///****************************************************************************
//*名称：InitCS5532Port()
//*功能：cs5532端口初始化
//*****************************************************************************/
//void InitCS5532Port()
//{
////  #define  MAINCLOCK                4500000UL

////	PINSEL_CFG_Type PinCfg_PWM;
////	PINSEL_CFG_Type PinCfg_SPI;
////	PINSEL_CFG_Type PinCfg_GPIO;
////  PWM_TIMERCFG_Type pwmcfg;
////  PWM_MATCHCFG_Type matchcfg;
////  SPI_CFG_Type  spicfg;
////  uint32_t        mr0val;

////  /*******main clock系统时钟端口配置****************/
////	 //set_PinFunc(PINSEL_PORT_1,PINSEL_PIN_18,PINSEL_FUNC_2);
////	PinCfg_PWM.Funcnum = 2;
////	PinCfg_PWM.OpenDrain = 0;
////	PinCfg_PWM.Pinmode = 0;
////	PinCfg_PWM.Portnum = 1;
////	PinCfg_PWM.Pinnum = 18;
////	PINSEL_ConfigPin(&PinCfg_PWM);
////	
////  /*不使能*/
////  PWM_Cmd(LPC_PWM1, DISABLE);
////  PWM_CounterCmd(LPC_PWM1, DISABLE);

////  PWM_ConfigStructInit(PWM_MODE_TIMER, &pwmcfg);
////  PWM_Init(	LPC_PWM1, PWM_MODE_TIMER, &pwmcfg);

//// /*设置匹配*/
////  matchcfg.	MatchChannel = 0;
////  matchcfg.IntOnMatch    = DISABLE;
////  matchcfg.StopOnMatch   = DISABLE;
////  matchcfg.	ResetOnMatch = ENABLE;

////  PWM_ConfigMatch(LPC_PWM1, &matchcfg);

////   /*设置MR0,MR1的值*/
////  mr0val =  25000000Ul/MAINCLOCK;
////  PWM_MatchUpdate(LPC_PWM1,0,mr0val,PWM_MATCH_UPDATE_NOW);
////  PWM_MatchUpdate(LPC_PWM1,1,mr0val/2 ,PWM_MATCH_UPDATE_NOW);

////  /*通道1输出使能*/
////  PWM_ChannelCmd(LPC_PWM1,1,ENABLE);

////  PWM_CounterCmd(LPC_PWM1, ENABLE);
////  PWM_Cmd(LPC_PWM1, ENABLE);

////  /************SPI端口配置***********************/
////  /*sclk,miso,mosi*/
//////  PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_15,PINSEL_FUNC_3);
//////  PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_17,PINSEL_FUNC_3);
//////  PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_18,PINSEL_FUNC_3);	
////	PinCfg_SPI.Funcnum = 3;
////	PinCfg_SPI.OpenDrain = 0;
////	PinCfg_SPI.Pinmode = 0;
////	PinCfg_SPI.Portnum = 0;
////	PinCfg_SPI.Pinnum = 15;
////	PINSEL_ConfigPin(&PinCfg_SPI);
////	PinCfg_SPI.Pinnum = 17;
////	PINSEL_ConfigPin(&PinCfg_SPI);
////	PinCfg_SPI.Pinnum = 18;
////	PINSEL_ConfigPin(&PinCfg_SPI);


////  /*cs is the GPIO*/
//////  PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_16,PINSEL_FUNC_0);
////	PinCfg_GPIO.Funcnum = 0;
////	PinCfg_GPIO.OpenDrain = 0;
////	PinCfg_GPIO.Pinmode = 0;
////	PinCfg_GPIO.Portnum = 0;
////	PinCfg_GPIO.Pinnum = 16;
////	PINSEL_ConfigPin(&PinCfg_GPIO);
////	
////  GPIO_SetDir(CS5532_SPI_PORT, CS5532_SPI_PIN_CS, 1);
////  GPIO_SetValue(CS5532_SPI_PORT, CS5532_SPI_PIN_CS);

////  SPI_ConfigStructInit(&spicfg);

////  SPI_Init(LPC_SPI, &spicfg);

//}
///****************************************************************************
//* 名称：InitCS5532()
//* 功能：初始化AD转换芯片CS5532的0通道。
//* 入口参数：ChannelsID　　通道号，不能大于1
//* 出口参数：读出的配置寄存器值
//* 说明：
//****************************************************************************/
//void  InitCS5532(void)
//{
//    uint8_t  AinPort , i;

//		/*cs不使能*/
//		Set_CS5532();
//		for(i=0;i<100;i++)
//    { 
//				AD_delay(200);
//		}

//    Clr_CS5532();
//    for(i=0;i<100;i++)
//    { 
//	   AD_delay(200);
//    }

//	/*使系统进入命令模式*/
//    CS5532InCmdMode();
//	     
//    CS5532Reset(); 

//    WriteConfigRegEx(0x00,0x01);

//   /*转换速率　0～9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 */
//		
//    WriteChannelSetupRegEx(0,0,Chan0Gain,CollectionFF,1,0);		//setup1:选择的物理通道1,增益4,双极性,字速率60sps
//    WriteChannelSetupRegEx(1,1,6,CollectionFF,0,1);;
//    WriteChannelSetupRegEx(2,1,0,2,0,1);
//		WriteChannelSetupRegEx(3,1,0,2,0,1);
//    WriteChannelSetupRegEx(4,1,0,2,0,1);
//    WriteChannelSetupRegEx(5,1,0,2,0,1);
//    WriteChannelSetupRegEx(6,1,0,2,0,1);
//    WriteChannelSetupRegEx(7,1,0,2,0,1);


//  /* 入口参数：CC  　　1:自偏移校准
//　　　　　　　　       2:自增益校准
//　　　　　　　　　　   5:系统偏移校准
//　　　　　　　　　　   6:系统增益校准
//　　　　　　  CSRP　　 通道设置寄存器指针　0~7
//  */
//    AinPort  = 0x00;

//    WriteOffsetReg(AinPort,0x00);        //偏移寄存器1写0
//    PerformCalibration(0x02,AinPort);    //setup1:自增益校准
//    StartSeriesConversion(AinPort);      //setup1:连续模式
//}
///*******************************************************
//**SD0引脚配置为GPIO模式
//********************************************************/
//void CS5532_SD0_ConfigGpio(void)
//{
////	 /*p0.17配置为普通GPIO*/
////     PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_17,PINSEL_FUNC_0);
////     /*输入*/
////     GPIO_SetDir(PINSEL_PORT_0,1UL << PINSEL_PIN_17, 0);
//}
///*******************************************************
//**SDO引脚配置为SPI模式
//********************************************************/
//void CS5532_SDO_ConfigSpi(void)
//{
////  	PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_17,PINSEL_FUNC_3);
//}

///*
//*功能：SD0管教中断初始化，在SDO下降沿中断
//*/
//void  CS5532Int_Init(void)
//{

////  /*p0.17配置为普通GPIO*/
////   PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_17,PINSEL_FUNC_0);
////  /*输入*/
////   GPIO_SetDir(CS5532_SPI_PORT, CS5532_SPI_PIN_MISO, 0);
////   /*p0.17下降沿中断使能*/
////   LPC_GPIOINT->IO0IntEnF   |= CS5532_SPI_PIN_MISO;
////   LPC_GPIOINT->IO0IntClr    = 0xFFFFFFFF;
////   /*设置中断服务程序*/
////   NVIC_SetPriority(EINT3_IRQn,	 1);
////   NVIC_EnableIRQ(EINT3_IRQn);
//}


///**********************************************************************
//***CS5532 ADC初始化
//**输入参数：IntEnable: 是否中断使能
//***********************************************************************/
//void CS5532Init()
//{
//  InitCS5532Port();
//	InitCS5532();  
//}











