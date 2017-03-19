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

///*ת�����ʡ�����0��9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240*/
//uint8_t  CollectionFF =       0x01      ;          //����Ƶ��
///*ͨ��0����     0��6   0:1  1:2  2:4  3:8  4:16  5:32  6:64*/ 
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
//* ���ƣ�MSPI0_WR_Data()
//* ���ܣ���SSI�������������ݡ�
//* ��ڲ�����send_data
//* ���ڲ�������
//* ˵����
//****************************************************************************/
//uint8_t MSPI0_WR_Data(uint8_t send_data)
//{ 
//   //SPI_SendData(LPC_SPI,send_data);
//   return(0);
//}
///****************************************************************************
//* ���ƣ�MSPI0_RD_Data()
//* ���ܣ���SSI�������������ݡ�
//* ��ڲ�����NONE
//* ���ڲ�������
//* ˵����
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
//* ���ƣ�CS5532Sendcmd()
//* ���ܣ��������
//* ��ڲ�������
//* ���ڲ�������
//* ˵����
//****************************************************************************/

//void  CS5532SendCmd(uint8_t cmd)
//{
//    MSPI0_WR_Data(cmd);
//}

///****************************************************************************
//* ���ƣ�CS5532SendData()
//* ���ܣ��������ݡ�
//* ��ڲ�����Data��Ҫ���͵�����
//* ���ڲ�������
//* ˵����
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
//* ���ƣ�CS5532ReadData()
//* ���ܣ��������ݡ�
//* ��ڲ�������
//* ���ڲ��������յ�����
//* ˵����
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
//* ���ƣ�WriteOffsetReg()
//* ���ܣ�дƫ�ƼĴ�����
//* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
//* ����������Data����������Ҫд��ƫ�ƼĴ�����ֵ
//* ���ڲ�������
//* ˵����
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
//* ���ƣ�ReadOffsetReg()
//* ���ܣ���ƫ�ƼĴ�����
//* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
//* ���ڲ�����������ƫ�ƼĴ���ֵ
//* ˵����
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
//* ���ƣ�WriteGainReg()
//* ���ܣ�д����Ĵ�����
//* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
//* ����������Data����������Ҫд������Ĵ�����ֵ
//* ���ڲ�������
//* ˵����
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
//* ���ƣ�ReadGainReg()
//* ���ܣ�������Ĵ�����
//* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
//* ���ڲ���������������Ĵ���ֵ
//* ˵����
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
//* ���ƣ�WriteChannelSetupReg()
//* ���ܣ�дͨ�����üĴ�����
//* ��ڲ�����RegID�������üĴ����ţ����ܴ���3
//* ����������Data����������Ҫд��ͨ�����üĴ�����ֵ
//* ���ڲ�������
//* ˵����
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
//* ���ƣ�ReadChannelSetupReg()
//* ���ܣ���ͨ�����üĴ�����
//* ��ڲ�����RegID�������üĴ����ţ����ܴ���3
//* ���ڲ�����������ͨ�����üĴ���ֵ
//* ˵����
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
//* ���ƣ�WriteChannelSetupRegEx()
//* ���ܣ�дͨ�����üĴ�����չ��
//* ��ڲ�����SetupID�������üĴ����š�0��7
//* ����������ChannelID��ת��ͨ���š���0��1
//* ����������Gain������ ���桡��������0��6   0:1  1:2  2:4  3:8  4:16  5:32  6:64 
//* ����������WordRate�� ת�����ʡ�����0��9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 
//* ����������UorB������ ����ģʽ������0��1   0:�����ԡ�1:˫����    
//* ����������TempA0A1�� A0A1�Ĵ���ֵ��0��3
//* ���ڲ�������
//* ˵����
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
//* ���ƣ�WriteConfigReg()
//* ���ܣ�д���üĴ�����
//* ��ڲ�����
//* ����������Data����������Ҫд�����üĴ�����ֵ
//* ���ڲ�������
//* ˵����
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
//* ���ƣ�WriteConfigRegEx()
//* ���ܣ�д���üĴ�����չ��
//* ��ڲ�����TempIs�������������̽ӵء�0���������롡1���̽ӵ�
//* ����������TempA0A1��A0A1�ļĴ���ֵ
//* ���ڲ����������Ƿ����óɹ���
//* ˵���� 00 02
//****************************************************************************/

//uint8_t  WriteConfigRegEx(uint8_t TempIs,uint8_t TempA0A1)
//{
//    #define OLS   0x00      //A1A2����Դ������0��ͨ���Ĵ�����1�����üĴ���
//	#define VRS   0x00      //��ѹ�ο�ѡ��
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
//* ���ƣ�ReadConfigReg()
//* ���ܣ���ͨ�����üĴ�����
//* ��ڲ�����
//* ���ڲ��������������üĴ���ֵ
//* ˵����
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
//* ���ƣ�ReadSOD0()
//* ���ܣ��ȴ�ת����ɡ�
//* ��ڲ�������
//* ���ڲ�������
//* ˵����
//****************************************************************************/
//void ReadSOD0(void)
//{
//   while(MISO_SDO);
//}

///****************************************************************************
//* ���ƣ�PerformCalibration()
//* ���ܣ�����ͨ�����üĴ���ָ��ָ��ļĴ����еĲ�������У׼��
//* ��ڲ�����CC  ����1:��ƫ��У׼
//��������������������2:������У׼
//��������������������5:ϵͳƫ��У׼
//��������������������6:ϵͳ����У׼
//������������CSRP����ͨ�����üĴ���ָ�롡0~7
//* ���ڲ������Ƿ�ɹ�
//* ˵����
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
//* ���ƣ�ReadADDataReg()
//* ���ܣ���ADת�������ݼĴ�����
//* ��ڲ�����MC  ����0:����ת��
//��������������������1:����ת��
//������������CSRP����ͨ�����üĴ���ָ�롡0~7
//* ���ڲ��������������ݼĴ���ֵ
//* ˵����
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
//* ���ƣ�ReadADValue()
//* ���ܣ���ADֵ��
//* ��ڲ�����ChannelsID:Ҫ�����ĵ�ͨ����
//* ���ڲ���������������AD����ֵ
//* ˵����
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
//* ���ƣ�StartSeriesConversion()
//* ���ܣ���ʼ����ת����
//* ��ڲ�����ChannelsID:Ҫת���ģ���ͨ����
//* ���ڲ�����
//* ˵����
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
//* ���ƣ�StopSeriesConversion()
//* ���ܣ�ֹͣ����ת����
//* ��ڲ�������
//* ���ڲ�����
//* ˵����
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
//* ���ƣ�ReadSeriesADValue()
//* ���ܣ�������ת��ADֵ��
//* ��ڲ�������
//* ���ڲ�����������AD����ֵ
//* ˵����
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
//* ���ƣ�CS5532InCmdMode()
//* ���ܣ�ϵͳ��ʼ����ʹ����ת����CS5532оƬ�����������ģʽ
//* ��ڲ�������
//* ���ڲ�������
//* ˵����
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
//* ���ƣ�CS5532Reset()
//* ���ܣ�ϵͳ��λ��
//* ��ڲ�������
//* ���ڲ�������
//* ˵����
//****************************************************************************/

//void  CS5532Reset(void)
//{
//    WriteConfigReg(0x20000000);
//    while(!(ReadConfigReg()&0x10000000));
//}
///****************************************************************************
//*���ƣ�InitCS5532Port()
//*���ܣ�cs5532�˿ڳ�ʼ��
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

////  /*******main clockϵͳʱ�Ӷ˿�����****************/
////	 //set_PinFunc(PINSEL_PORT_1,PINSEL_PIN_18,PINSEL_FUNC_2);
////	PinCfg_PWM.Funcnum = 2;
////	PinCfg_PWM.OpenDrain = 0;
////	PinCfg_PWM.Pinmode = 0;
////	PinCfg_PWM.Portnum = 1;
////	PinCfg_PWM.Pinnum = 18;
////	PINSEL_ConfigPin(&PinCfg_PWM);
////	
////  /*��ʹ��*/
////  PWM_Cmd(LPC_PWM1, DISABLE);
////  PWM_CounterCmd(LPC_PWM1, DISABLE);

////  PWM_ConfigStructInit(PWM_MODE_TIMER, &pwmcfg);
////  PWM_Init(	LPC_PWM1, PWM_MODE_TIMER, &pwmcfg);

//// /*����ƥ��*/
////  matchcfg.	MatchChannel = 0;
////  matchcfg.IntOnMatch    = DISABLE;
////  matchcfg.StopOnMatch   = DISABLE;
////  matchcfg.	ResetOnMatch = ENABLE;

////  PWM_ConfigMatch(LPC_PWM1, &matchcfg);

////   /*����MR0,MR1��ֵ*/
////  mr0val =  25000000Ul/MAINCLOCK;
////  PWM_MatchUpdate(LPC_PWM1,0,mr0val,PWM_MATCH_UPDATE_NOW);
////  PWM_MatchUpdate(LPC_PWM1,1,mr0val/2 ,PWM_MATCH_UPDATE_NOW);

////  /*ͨ��1���ʹ��*/
////  PWM_ChannelCmd(LPC_PWM1,1,ENABLE);

////  PWM_CounterCmd(LPC_PWM1, ENABLE);
////  PWM_Cmd(LPC_PWM1, ENABLE);

////  /************SPI�˿�����***********************/
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
//* ���ƣ�InitCS5532()
//* ���ܣ���ʼ��ADת��оƬCS5532��0ͨ����
//* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
//* ���ڲ��������������üĴ���ֵ
//* ˵����
//****************************************************************************/
//void  InitCS5532(void)
//{
//    uint8_t  AinPort , i;

//		/*cs��ʹ��*/
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

//	/*ʹϵͳ��������ģʽ*/
//    CS5532InCmdMode();
//	     
//    CS5532Reset(); 

//    WriteConfigRegEx(0x00,0x01);

//   /*ת�����ʡ�0��9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 */
//		
//    WriteChannelSetupRegEx(0,0,Chan0Gain,CollectionFF,1,0);		//setup1:ѡ�������ͨ��1,����4,˫����,������60sps
//    WriteChannelSetupRegEx(1,1,6,CollectionFF,0,1);;
//    WriteChannelSetupRegEx(2,1,0,2,0,1);
//		WriteChannelSetupRegEx(3,1,0,2,0,1);
//    WriteChannelSetupRegEx(4,1,0,2,0,1);
//    WriteChannelSetupRegEx(5,1,0,2,0,1);
//    WriteChannelSetupRegEx(6,1,0,2,0,1);
//    WriteChannelSetupRegEx(7,1,0,2,0,1);


//  /* ��ڲ�����CC  ����1:��ƫ��У׼
//����������������       2:������У׼
//��������������������   5:ϵͳƫ��У׼
//��������������������   6:ϵͳ����У׼
//������������  CSRP���� ͨ�����üĴ���ָ�롡0~7
//  */
//    AinPort  = 0x00;

//    WriteOffsetReg(AinPort,0x00);        //ƫ�ƼĴ���1д0
//    PerformCalibration(0x02,AinPort);    //setup1:������У׼
//    StartSeriesConversion(AinPort);      //setup1:����ģʽ
//}
///*******************************************************
//**SD0��������ΪGPIOģʽ
//********************************************************/
//void CS5532_SD0_ConfigGpio(void)
//{
////	 /*p0.17����Ϊ��ͨGPIO*/
////     PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_17,PINSEL_FUNC_0);
////     /*����*/
////     GPIO_SetDir(PINSEL_PORT_0,1UL << PINSEL_PIN_17, 0);
//}
///*******************************************************
//**SDO��������ΪSPIģʽ
//********************************************************/
//void CS5532_SDO_ConfigSpi(void)
//{
////  	PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_17,PINSEL_FUNC_3);
//}

///*
//*���ܣ�SD0�ܽ��жϳ�ʼ������SDO�½����ж�
//*/
//void  CS5532Int_Init(void)
//{

////  /*p0.17����Ϊ��ͨGPIO*/
////   PINSEL_SetPinFunc(PINSEL_PORT_0,PINSEL_PIN_17,PINSEL_FUNC_0);
////  /*����*/
////   GPIO_SetDir(CS5532_SPI_PORT, CS5532_SPI_PIN_MISO, 0);
////   /*p0.17�½����ж�ʹ��*/
////   LPC_GPIOINT->IO0IntEnF   |= CS5532_SPI_PIN_MISO;
////   LPC_GPIOINT->IO0IntClr    = 0xFFFFFFFF;
////   /*�����жϷ������*/
////   NVIC_SetPriority(EINT3_IRQn,	 1);
////   NVIC_EnableIRQ(EINT3_IRQn);
//}


///**********************************************************************
//***CS5532 ADC��ʼ��
//**���������IntEnable: �Ƿ��ж�ʹ��
//***********************************************************************/
//void CS5532Init()
//{
//  InitCS5532Port();
//	InitCS5532();  
//}











