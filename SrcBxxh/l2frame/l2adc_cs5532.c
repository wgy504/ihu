#include <inttypes.h>
#include "l2adc_cs5532.h"
#include "main.h"

typedef struct Structuint32_t
{
		 unsigned char  uint8_t_3;
		 unsigned char  uint8_t_2;
		 unsigned char  uint8_t_1;
		 unsigned char  uint8_t_0;
}uint32_tStruct1;

typedef union Unionuint32_t
{
			uint32_tStruct1         uint32_tStruct;
			unsigned int          uint32_tData;
}uint32_tUnion;
			
/*ת�����ʡ�����0��9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240*/
uint8_t  CollectionFF =       0x00      ;          //����Ƶ��
/*ͨ��0����     0��6   0:1  1:2  2:4  3:8  4:16  5:32  6:64*/ 
uint8_t  Chan0Gain    =       0x05;

//SPI_HandleTypeDef hspi3;


void AD_delay(uint32_t val)
{
//  while(val--)
//  {
//  }
	HAL_Delay(val);
}
/****************************************************************************
* ���ƣ�MSPI0_WR_Data()
* ���ܣ���SSI�������������ݡ�
* ��ڲ�����send_data
* ���ڲ�������
* ˵����
****************************************************************************/
uint8_t MSPI0_WR_Data(uint8_t send_data)
{
	 HAL_SPI_Transmit(&hspi3,&send_data,1,1);
   return(0);
}
/****************************************************************************
* ���ƣ�MSPI0_RD_Data()
* ���ܣ���SSI�������������ݡ�
* ��ڲ�����NONE
* ���ڲ�������
* ˵����
****************************************************************************/
uint8_t MSPI0_RD_Data()
{
  uint8_t rec;
	uint8_t send = 0x00;
	
	HAL_SPI_TransmitReceive(&hspi3, (uint8_t*)&send, (uint8_t *)&rec, 1, 1);

  return(rec);
}
	
/****************************************************************************
* ���ƣ�CS5532Sendcmd()
* ���ܣ��������
* ��ڲ�������
* ���ڲ�������
* ˵����
****************************************************************************/

void  CS5532SendCmd(uint8_t cmd)
{
    MSPI0_WR_Data(cmd);
}

/****************************************************************************
* ���ƣ�CS5532SendData()
* ���ܣ��������ݡ�
* ��ڲ�����Data��Ҫ���͵�����
* ���ڲ�������
* ˵����
****************************************************************************/

void  CS5532SendData(uint32_t Data)
{
       uint32_tUnion DataTemp;
    
       DataTemp.uint32_tData = Data; 
       MSPI0_WR_Data(DataTemp.uint32_tStruct.uint8_t_0);
       MSPI0_WR_Data(DataTemp.uint32_tStruct.uint8_t_1);
       MSPI0_WR_Data(DataTemp.uint32_tStruct.uint8_t_2);
       MSPI0_WR_Data(DataTemp.uint32_tStruct.uint8_t_3);
}


/****************************************************************************
* ���ƣ�CS5532ReadData()
* ���ܣ��������ݡ�
* ��ڲ�������
* ���ڲ��������յ�����
* ˵����
****************************************************************************/

uint32_t  CS5532ReadData(void)
{
    uint32_tUnion DataTemp;
    
    DataTemp.uint32_tStruct.uint8_t_0 = MSPI0_RD_Data();
    DataTemp.uint32_tStruct.uint8_t_1 = MSPI0_RD_Data();
    DataTemp.uint32_tStruct.uint8_t_2 = MSPI0_RD_Data();
    DataTemp.uint32_tStruct.uint8_t_3 = MSPI0_RD_Data();
    
    return DataTemp.uint32_tData;
}

/****************************************************************************
* ���ƣ�WriteOffsetReg()
* ���ܣ�дƫ�ƼĴ�����
* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
* ����������Data����������Ҫд��ƫ�ƼĴ�����ֵ
* ���ڲ�������
* ˵����
****************************************************************************/

uint8_t  WriteOffsetReg(uint8_t ChannelsID,uint32_t Data)
{
    if(ChannelsID > 1)
        return 0;
	 Clr_CS5532();

     CS5532SendCmd((ChannelsID << 4)|0x01);
     CS5532SendData(Data);

	 Set_CS5532();
	 return 1;
}

/****************************************************************************
* ���ƣ�ReadOffsetReg()
* ���ܣ���ƫ�ƼĴ�����
* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
* ���ڲ�����������ƫ�ƼĴ���ֵ
* ˵����
****************************************************************************/

uint32_t  ReadOffsetReg(uint8_t ChannelsID)
{
    uint32_t DataTemp;
    
    if(ChannelsID > 1)
        return 0;
    Clr_CS5532();
		   
    CS5532SendCmd((ChannelsID<<4)|0x09);
    DataTemp = CS5532ReadData();

    Set_CS5532(); 
    return DataTemp;
}

/****************************************************************************
* ���ƣ�WriteGainReg()
* ���ܣ�д����Ĵ�����
* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
* ����������Data����������Ҫд������Ĵ�����ֵ
* ���ڲ�������
* ˵����
****************************************************************************/

uint8_t  WriteGainReg(uint8_t ChannelsID,uint32_t Data)
{
    if(ChannelsID > 1)
        return 0;
	Clr_CS5532();

	CS5532SendCmd((ChannelsID<<4)|0x02);
    CS5532SendData(Data);

	Set_CS5532();
	return 1;
}

/****************************************************************************
* ���ƣ�ReadGainReg()
* ���ܣ�������Ĵ�����
* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
* ���ڲ���������������Ĵ���ֵ
* ˵����
****************************************************************************/

uint32_t  ReadGainReg(uint8_t ChannelsID)
{
    uint32_t DataTemp;
    
    if(ChannelsID > 1)
        return 0;
    Clr_CS5532();
		  
    CS5532SendCmd((ChannelsID<<4)|0x0A);
    DataTemp = CS5532ReadData();

    Set_CS5532();  
    return DataTemp;
}

/****************************************************************************
* ���ƣ�WriteChannelSetupReg()
* ���ܣ�дͨ�����üĴ�����
* ��ڲ�����RegID�������üĴ����ţ����ܴ���3
* ����������Data����������Ҫд��ͨ�����üĴ�����ֵ
* ���ڲ�������
* ˵����
****************************************************************************/

uint8_t  WriteChannelSetupReg(uint8_t RegID,uint32_t Data)
{
    if(RegID > 3)
        return 0;
		
	Clr_CS5532();
		
    CS5532SendCmd((RegID << 4) | 0x05);
    CS5532SendData(Data);

	Set_CS5532();
	return 1;
}

/****************************************************************************
* ���ƣ�ReadChannelSetupReg()
* ���ܣ���ͨ�����üĴ�����
* ��ڲ�����RegID�������üĴ����ţ����ܴ���3
* ���ڲ�����������ͨ�����üĴ���ֵ
* ˵����
****************************************************************************/

uint32_t  ReadChannelSetupReg(uint8_t RegID)
{
    uint32_t DataTemp;
    
    if(RegID > 3)
        return 0;

    Clr_CS5532();

    CS5532SendCmd((RegID<<4)|0x0D);
    DataTemp = CS5532ReadData();
	
	Set_CS5532();
    return DataTemp;
}

/****************************************************************************
* ���ƣ�WriteChannelSetupRegEx()
* ���ܣ�дͨ�����üĴ�����չ��
* ��ڲ�����SetupID�������üĴ����š�0��7
* ����������ChannelID��ת��ͨ���š���0��1
* ����������Gain������ ���桡��������0��6   0:1  1:2  2:4  3:8  4:16  5:32  6:64 
* ����������WordRate�� ת�����ʡ�����0��9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 
* ����������UorB������ ����ģʽ������0��1   0:�����ԡ�1:˫����    
* ����������TempA0A1�� A0A1�Ĵ���ֵ��0��3
* ���ڲ�������
* ˵����
****************************************************************************/
uint8_t  WriteChannelSetupRegEx(uint8_t SetupID,uint8_t ChannelID,uint8_t Gain,uint8_t WordRate,uint8_t UorB,uint8_t TempA0A1)
{
	uint32_t TempData32 = 0x00000000;
	uint32_t TempData;
	uint8_t  TempData8;
	
	if(SetupID > 0x07)
	    return 0;
	if(ChannelID > 0x01)
	    return 0;
	if(Gain > 0x06)
	    return 0;
	if(WordRate > 0x09)
	    return 0;
	if(UorB > 0x01)
	    return 0;
	if(TempA0A1 > 0x03)
	    return 0;

	TempData32 = ReadChannelSetupReg(SetupID/2);
	TempData8 = 16*((SetupID+1)%2);
	
	TempData = 0x00000003;
	TempData32 &= ~(TempData<<(14+TempData8));
	TempData = ChannelID;
	TempData32 |= TempData<<(14+TempData8);
		
	TempData = 0x00000007;
	TempData32 &= ~(TempData<<(11+TempData8));
	TempData = Gain;
	TempData32 |= TempData<<(11+TempData8);
	
	TempData = 0x0000000F;
	TempData32 &= ~(TempData<<(7+TempData8));
	TempData = WordRate;
	if(TempData > 0x04)
	    TempData += 0x03;
	TempData32 |= TempData<<(7+TempData8);
	
	TempData = 0x00000001;
	TempData32 &= ~(TempData<<(6+TempData8));
	TempData = UorB;
	TempData32 |= TempData<<(6+TempData8);
	
	TempData = 0x00000003;
	TempData32 &= ~(TempData<<(4+TempData8));
	TempData = TempA0A1;
	TempData32 |= TempData<<(4+TempData8);
	WriteChannelSetupReg(SetupID/2,TempData32);
	return 1;
}

/****************************************************************************
* ���ƣ�WriteConfigReg()
* ���ܣ�д���üĴ�����
* ��ڲ�����
* ����������Data����������Ҫд�����üĴ�����ֵ
* ���ڲ�������
* ˵����
****************************************************************************/

uint8_t  WriteConfigReg(uint32_t Data)
{
    Clr_CS5532();	
    
    CS5532SendCmd(0x03);
    CS5532SendData(Data);

	Set_CS5532();

    return 1;
}

/****************************************************************************
* ���ƣ�WriteConfigRegEx()
* ���ܣ�д���üĴ�����չ��
* ��ڲ�����TempIs�������������̽ӵء�0���������롡1���̽ӵ�
* ����������TempA0A1��A0A1�ļĴ���ֵ
* ���ڲ����������Ƿ����óɹ���
* ˵���� 00 02
****************************************************************************/

uint8_t  WriteConfigRegEx(uint8_t TempIs,uint8_t TempA0A1)
{
    #define OLS   0x00      //A1A2����Դ������0��ͨ���Ĵ�����1�����üĴ���
	#define VRS   0x00      //��ѹ�ο�ѡ��
	
	uint32_t TempData32 = 0x00000000;
	uint32_t TempData;
	
	if(TempIs > 0x01)
	    return 0;
	if(TempA0A1 > 0x03)
	    return 0;
		
	TempData = 0x00000001;
	TempData32 &= ~(TempData<<27);
	TempData    = TempIs;
	TempData32 |= (TempData<<27);
    
	TempData = 0x00000001;
	TempData32 &= ~(TempData<<25);
	TempData    = VRS;
	TempData32 |= (TempData<<25);
	
	TempData = 0x00000003;
	TempData32 &= ~(TempData<<23);
	TempData = TempA0A1;
	TempData32 |= (TempData<<23);
	
	TempData = 0x00000001;
	TempData32 &= ~(TempData<<22);
	TempData    = OLS;
	TempData32 |= (TempData<<22);

	WriteConfigReg(TempData32);
	return 1;
}

/****************************************************************************
* ���ƣ�ReadConfigReg()
* ���ܣ���ͨ�����üĴ�����
* ��ڲ�����
* ���ڲ��������������üĴ���ֵ
* ˵����
****************************************************************************/

uint32_t  ReadConfigReg(void)
{
    uint32_t DataTemp;
	 
    Clr_CS5532();

    CS5532SendCmd(0x0B);
    DataTemp = CS5532ReadData();

    Set_CS5532();
    return DataTemp;
}




uint8_t ReadWheChanOk(void)
{
  if(MISO_SDO)
  {
    return 0;
  }
  return 1;
}
/****************************************************************************
* ���ƣ�ReadSOD0()
* ���ܣ��ȴ�ת����ɡ�
* ��ڲ�������
* ���ڲ�������
* ˵����
****************************************************************************/
void ReadSOD0(void)
{
   while(MISO_SDO);
}

/****************************************************************************
* ���ƣ�PerformCalibration()
* ���ܣ�����ͨ�����üĴ���ָ��ָ��ļĴ����еĲ�������У׼��
* ��ڲ�����CC  ����1:��ƫ��У׼
��������������������2:������У׼
��������������������5:ϵͳƫ��У׼
��������������������6:ϵͳ����У׼
������������CSRP����ͨ�����üĴ���ָ�롡0~7
* ���ڲ������Ƿ�ɹ�
* ˵����
****************************************************************************/

uint8_t  PerformCalibration(uint8_t CC,uint8_t CSRP)
{
    if(CC >7 )
     return 0;
    if(CSRP >7 )
     return 0;
    Clr_CS5532(); 
	   
    CS5532SendCmd(CC|(CSRP<<3)|0x80);
     ReadSOD0();

	Set_CS5532();
    return 1;
}

/****************************************************************************
* ���ƣ�ReadADDataReg()
* ���ܣ���ADת�������ݼĴ�����
* ��ڲ�����MC  ����0:����ת��
��������������������1:����ת��
������������CSRP����ͨ�����üĴ���ָ�롡0~7
* ���ڲ��������������ݼĴ���ֵ
* ˵����
****************************************************************************/
uint32_t  ReadADDataReg(uint8_t MC,uint8_t CSRP)
{
    uint32_t DataTemp;
    
    if(MC >1 )
        return 0;
    if(CSRP >7 )
        return 0;

    Clr_CS5532();
		
    CS5532SendCmd((MC<<6)|(CSRP<<3)|0x80);

	if(MC != 0x00)
	{
	   return 0;
	}

    ReadSOD0();
    MSPI0_WR_Data(0x00);
    DataTemp = CS5532ReadData();

    Set_CS5532(); 
    return DataTemp;
}

/****************************************************************************
* ���ƣ�ReadADValue()
* ���ܣ���ADֵ��
* ��ڲ�����ChannelsID:Ҫ�����ĵ�ͨ����
* ���ڲ���������������AD����ֵ
* ˵����
****************************************************************************/
uint32_t ReadADValue(uint8_t ChannelsID)
{
    uint32_t TempData;
    
    TempData = ReadADDataReg(0x00,ChannelsID);
    TempData = TempData >> 8;
    return TempData;
}
/****************************************************************************
* ���ƣ�StartSeriesConversion()
* ���ܣ���ʼ����ת����
* ��ڲ�����ChannelsID:Ҫת���ģ���ͨ����
* ���ڲ�����
* ˵����
****************************************************************************/
uint32_t StartSeriesConversion(uint8_t ChannelsID)
{
    uint32_t TempData;
    
    TempData = ReadADDataReg(0x01,ChannelsID);
	
    return TempData;
}

/****************************************************************************
* ���ƣ�StopSeriesConversion()
* ���ܣ�ֹͣ����ת����
* ��ڲ�������
* ���ڲ�����
* ˵����
****************************************************************************/
uint32_t StopSeriesConversion(void)
{
    uint32_t TempData;
    
    Clr_CS5532();
		
    ReadSOD0();
    MSPI0_WR_Data(0xFF);
    TempData = CS5532ReadData();

	Set_CS5532(); 
	TempData = TempData >> 8;
    return TempData;
}

/****************************************************************************
* ���ƣ�ReadSeriesADValue()
* ���ܣ�������ת��ADֵ��
* ��ڲ�������
* ���ڲ�����������AD����ֵ
* ˵����
****************************************************************************/
uint32_t ReadSeriesADValue(void)
{
    uint32_t TempData;


    MSPI0_WR_Data(0x00);
    TempData = CS5532ReadData();
		TempData = TempData >> 8;
		TempData = TempData >> (24 - zWeightSensorParam.WeightSensorAdcBitwidth);

    return TempData;
}              
	            

/****************************************************************************
* ���ƣ�CS5532InCmdMode()
* ���ܣ�ϵͳ��ʼ����ʹ����ת����CS5532оƬ�����������ģʽ
* ��ڲ�������
* ���ڲ�������
* ˵����
****************************************************************************/

void   CS5532InCmdMode(void)
{
    uint8_t TempData;
	Clr_CS5532();

    for(TempData=0;TempData<40;TempData++)
    {
	    MSPI0_WR_Data(0xFF);
    }
	MSPI0_WR_Data(0xfE);

	Set_CS5532();
}


/****************************************************************************
* ���ƣ�CS5532Reset()
* ���ܣ�ϵͳ��λ��
* ��ڲ�������
* ���ڲ�������
* ˵����
****************************************************************************/

void  CS5532Reset(void)
{
    WriteConfigReg(0x20000000);
    while(!(ReadConfigReg()&0x10000000));
}

/****************************************************************************
* ���ƣ�InitCS5532()
* ���ܣ���ʼ��ADת��оƬCS5532��0ͨ����
* ��ڲ�����ChannelsID����ͨ���ţ����ܴ���1
* ���ڲ��������������üĴ���ֵ
* ˵����
****************************************************************************/
void  InitCS5532(void)
{
  uint8_t  AinPort , i;

   /*cs��ʹ��*/
	Set_CS5532();
	for(i=0;i<100;i++)
    { 
	   AD_delay(1);
	}

    Clr_CS5532();
    for(i=0;i<100;i++)
    { 
	   AD_delay(1);
    }

	/*ʹϵͳ��������ģʽ*/
    CS5532InCmdMode();
	     
    CS5532Reset(); 

    WriteConfigRegEx(0x00,0x01);

   /*ת�����ʡ�0��9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 */
		
    //WriteChannelSetupRegEx(0,0,Chan0Gain,CollectionFF,1,0);		//setup1:ѡ�������ͨ��1,����4,˫����,������60sps
		WriteChannelSetupRegEx(0,0,zWeightSensorParam.WeightSensorAdcGain,zWeightSensorParam.WeightSensorAdcSampleFreq,1,0);
//    WriteChannelSetupRegEx(1,1,6,CollectionFF,0,1);;
//    WriteChannelSetupRegEx(2,1,0,2,0,1);
//		WriteChannelSetupRegEx(3,1,0,2,0,1);
//    WriteChannelSetupRegEx(4,1,0,2,0,1);
//    WriteChannelSetupRegEx(5,1,0,2,0,1);
//    WriteChannelSetupRegEx(6,1,0,2,0,1);
//    WriteChannelSetupRegEx(7,1,0,2,0,1);


  /* ��ڲ�����CC  ����1:��ƫ��У׼
����������������       2:������У׼
��������������������   5:ϵͳƫ��У׼
��������������������   6:ϵͳ����У׼
������������  CSRP���� ͨ�����üĴ���ָ�롡0~7
  */
    AinPort  = 0x00;

    WriteOffsetReg(AinPort,0x00);        //ƫ�ƼĴ���1д0
    PerformCalibration(0x02,AinPort);    //setup1:������У׼
    StartSeriesConversion(AinPort);      //setup1:����ģʽ
}

/**********************************************************************
***CS5532 ADC��ʼ��
**���������IntEnable: �Ƿ��ж�ʹ��
***********************************************************************/
void CS5532Init()
{
	InitCS5532();  
}

/* ===================================
** Weight Sensor Parameters and APIs
** ===================================
*/
//???
//-----Freq-----x64-----x32-----x16-----x8-----x4-----x2-----x1
//     7.5       19      20      21     22     22     22     22

uint32_t WeightSensorInit(WeightSensorParamaters_t *pwsp)
{	
		if(NULL == pwsp)
		{
				printf("l2adc_cs5532: WeightSensorInit: NULL == pwsp, return IHU_FAILURE\r\n");
				return 1;
		}
		
		//printf("l2adc_cs5532: WeightSensorInit: pwsp = 0x%08X\r\n", pwsp);
		pwsp->WeightSensorAdcSampleFreq = ADC_AMPLIFIER_WORDRATE_120SPS; //0:120sps  1:60sps  2:30sps  3:15sps  4:7.5sps  8:3840sps  9:1920sps  10:960sps  11:480sps  12:240sps
		pwsp->WeightSensorAdcGain = ADC_AMPLIFIER_GAIN_16X;			//0:X1  1:X2  2:X4  3:X8  4:X16  5:X32  6:X64
		pwsp->WeightSensorAdcBitwidth = SpsGainToBitwidthMapping(pwsp->WeightSensorAdcSampleFreq, pwsp->WeightSensorAdcGain);
		pwsp->WeightSensorCalibrationFullWeight = 100000;//0.01g????
		pwsp->WeightSensorInitOrNot = WEIGHT_SENSOR_HAD_INITED;
		
//		pwsp->WeightSensorAdcSampleFreq = 0;//0:120sps  1:60sps  2:30sps  3:15sps  4:7.5sps  8:3840sps  9:1920sps  10:960sps  11:480sps  12:240sps
//		pwsp->WeightSensorAdcGain = 3;			//0:X1  1:X2  2:X4  3:X8  4:X16  5:X32  6:X64
//		pwsp->WeightSensorAdcBitwidth = 20;
//		pwsp->WeightSensorCalibrationFullWeight = 100000;//0.01g????
//		pwsp->WeightSensorInitOrNot = WEIGHT_SENSOR_HAD_INITED;
	
		CS5532Init();
	
		return 0;
}

/* Reconfig Sensor */
uint32_t weightSensorConfig(WeightSensorParamaters_t *pwsp)
{
	
		if(NULL == pwsp)
		{
				IhuErrorPrint("l2adc_cs5532: weightSensorConfig: NULL == pwsp, return IHU_FAILURE\r\n");
				return 1;
		}

#if 0
    IhuDebugPrint("weightSensorConfig: AdcSampleFreq=%d AdcGain=%d ZeroAdcValue=%d FullAdcValue=%d FullWeight=%d\r\r",
                  pwsp->WeightSensorAdcSampleFreq,
                  pwsp->WeightSensorAdcGain,
                  pwsp->WeightSensorCalibrationZeroAdcValue,
                  pwsp->WeightSensorCalibrationFullAdcValue,
                  pwsp->WeightSensorCalibrationFullWeight);
#endif
    
		zWeightSensorParam.WeightSensorAdcSampleFreq = pwsp->WeightSensorAdcSampleFreq;
		zWeightSensorParam.WeightSensorAdcGain = pwsp->WeightSensorAdcGain;
		zWeightSensorParam.WeightSensorAdcBitwidth = pwsp->WeightSensorAdcBitwidth;
		zWeightSensorParam.WeightSensorAdcValue = pwsp->WeightSensorAdcValue;
		zWeightSensorParam.WeightSensorCalibrationZeroAdcValue = pwsp->WeightSensorCalibrationZeroAdcValue;
		zWeightSensorParam.WeightSensorCalibrationFullAdcValue = pwsp->WeightSensorCalibrationFullAdcValue;
		zWeightSensorParam.WeightSensorCalibrationFullWeight = pwsp->WeightSensorCalibrationFullWeight;
		zWeightSensorParam.WeightSensorStaticZeroValue = pwsp->WeightSensorStaticZeroValue;
		zWeightSensorParam.WeightSensorTailorValue = pwsp->WeightSensorTailorValue;
		zWeightSensorParam.WeightSensorDynamicZeroThreadValue = pwsp->WeightSensorDynamicZeroThreadValue;
		zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs = pwsp->WeightSensorDynamicZeroHysteresisMs;
	
	return 0;
}

//?????
uint32_t WeightSensorCalibrationZero(WeightSensorParamaters_t *pwsp)
{
	int i;
	uint32_t temp = 0;
	
	if(NULL == pwsp)
	{
			IhuErrorPrint("l2adc_cs5532: WeightSensorCalibrationZero: NULL == pwsp, return IHU_FAILURE\r\n");
			return 0;
	}
	
	//printf("l2adc_cs5532: WeightSensorCalibrationZero: pwsp = 0x%08X\r\n", pwsp);
	
	for(i = 0; i < 32; i ++)
	{
		if(ReadWheChanOk())
		{
				temp += ReadSeriesADValue();
		}
		HAL_Delay(20);
	}
	temp = temp >> 5;
	
	pwsp->WeightSensorCalibrationZeroAdcValue = temp;
	
	IhuDebugPrint("l2adc_cs5532: WeightSensorCalibrationZero: ZeroADC=%d FullADC=%d FullWeigth=%d\r\n", 
								pwsp->WeightSensorCalibrationZeroAdcValue, pwsp->WeightSensorCalibrationFullAdcValue, pwsp->WeightSensorCalibrationFullWeight);
	return pwsp->WeightSensorCalibrationZeroAdcValue;
}

//?????
uint32_t WeightSensorCalibrationFull(WeightSensorParamaters_t *pwsp)
{
	int i;
	uint32_t temp = 0;
	
	if(NULL == pwsp)
	{
			IhuErrorPrint("l2adc_cs5532: WeightSensorCalibrationFull: NULL == pwsp, return IHU_FAILURE\r\n");
			return 1;
	}
	
	//printf("l2adc_cs5532: WeightSensorCalibrationFull: pwsp = 0x%08X\r\n", pwsp);
	
	for(i = 0; i < 32; i ++)
	{
		if(ReadWheChanOk())
		{
				temp += ReadSeriesADValue();
		}
		HAL_Delay(20);
	}
	temp = temp >> 5;
	
	pwsp->WeightSensorCalibrationFullAdcValue = temp;
	
	IhuDebugPrint("l2adc_cs5532: WeightSensorCalibrationFull: ZeroADC=%d FullADC=%d FullWeigth=%d\r\n", 
								pwsp->WeightSensorCalibrationZeroAdcValue, pwsp->WeightSensorCalibrationFullAdcValue, pwsp->WeightSensorCalibrationFullWeight);
	
	return pwsp->WeightSensorCalibrationFullAdcValue;
	
}
//??K?B
void WeightSensorCalibrationKB(WeightSensorParamaters_t *pwsp)
{
//	pwsp->WeightSensorCalibrationB = pwsp->WeightSensorCalibrationZeroAdcValue;
//	pwsp->WeightSensorCalibrationK = (double)(pwsp->WeightSensorCalibrationFullAdcValue-pwsp->WeightSensorCalibrationZeroAdcValue)/pwsp->WeightSensorCalibrationFullWeight;
}

//??????
int32_t WeightSensorReadCurrent(WeightSensorParamaters_t *pwsp)
{
	int i;
	uint32_t temp = 0;
	int32_t temp2 = 0;
	int32_t temp3 = 0;
	
	if(NULL == pwsp)
	{
			IhuErrorPrint("l2adc_cs5532: WeightSensorReadCurrent: NULL == pwsp, return 0\r\n");
			return 0;
	}
	
	if(pwsp->WeightSensorCalibrationFullAdcValue <= pwsp->WeightSensorCalibrationZeroAdcValue)
	{
			IhuErrorPrint("l2adc_cs5532: WeightSensorReadCurrent: FullADC %d <= ZeroADC %d, return 0\r\n",
										pwsp->WeightSensorCalibrationFullAdcValue, pwsp->WeightSensorCalibrationZeroAdcValue);
			return 0;
	}
	
	for(i = 0; i < 32; i ++)
	{
		if(ReadWheChanOk())
		{
			temp += ReadSeriesADValue();
		}
		HAL_Delay(10);
	}
	
	temp = temp >> 5;
	
	pwsp->WeightSensorAdcValue = temp;
	
	temp2 = temp-pwsp->WeightSensorCalibrationZeroAdcValue;
	temp3 = (temp2*pwsp->WeightSensorCalibrationFullWeight)/(pwsp->WeightSensorCalibrationFullAdcValue-pwsp->WeightSensorCalibrationZeroAdcValue);
	
	return temp3;
}	

uint32_t SpsGainToBitwidthMappingTable[WORDRATE_INDEX_NUMBER][WORDRATE_INDEX_NUMBER] = { 
/*Gain   														1,		2,		4,		8,		16,		32,		64    */
/*0000 120 Sps 		100 Sps			*/	{	21,		21,		21,		21,		20,		19,		18 },
/*0001 60 Sps 		50 Sps			*/	{	21,		21,		21,		21,		21,		20,		19 },
/*0010 30 Sps 		25 Sps			*/	{	22,		22,		22,		22,		21,		20,		19 },
/*0011 15 Sps 		12.5 Sps		*/	{	22,		22,		22,		22,		22,		21,		20 },
/*0100 7.5 Sps 		6.25 Sps		*/	{	23,		23,		23,		23,		22,		21,		20 },
/*1000 3840 Sps 	3200 Sps		*/	{	13,		13,		13,		13,		13,		13,		13 },
/*1001 1920 Sps 	1600 Sps		*/	{	16,		16,		16,		16,		16,		16,		16 },
/*1010 960 Sps 		800 Sps			*/	{	17,		17,		17,		17,		17,		16,		16 },
/*1011 480 Sps 		400 Sps			*/	{	17,		17,		17,		17,		17,		17,		17 },
/*1100 240 Sps		200 Sps			*/	{	18,		18,		18,		18,		18,		17,		17 }, };

uint32_t SpsGainToBitwidthMapping(uint32_t wordrate_index, uint32_t gain_index)
{
		if (( gain_index >= GAIN_INDEX_NUMBER) || (wordrate_index >= WORDRATE_INDEX_NUMBER) )
		{
				printf("l2adc_cs5532: SpsGainToBitwidthMapping: Invalid parameter, gain_index(%d)>=GAIN_INDEX_NUMBER(%d), or, wordrate_index(%d) >= WORDRATE_INDEX_NUMBER(%d), return 0xFFFFFFFF\r\b", gain_index, GAIN_INDEX_NUMBER, wordrate_index, WORDRATE_INDEX_NUMBER);
				return 0xFFFFFFFF;
		}
		
		printf("l2adc_cs5532: SpsGainToBitwidthMapping: [wordrate_index(%d), gain_index(%d)] => Bitwidth(%d)\r\n", gain_index, wordrate_index, SpsGainToBitwidthMappingTable[gain_index][wordrate_index]);
		return SpsGainToBitwidthMappingTable[wordrate_index][gain_index];
		
}

//int a[5][3]={ {80,75,92}, {61,65,71}, {59,63,70}, {85,87,90}, {76,77,85} };
//*      Gain    ??     0~6   0:1  1:2  2:4  3:8  4:16  5:32  6:64 
//*      WordRate  ????   0~9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 
																																								
//000 Gain = 1, (Input Span = [(VREF+)-(VREF-)]/1*A for unipolar).
//001 Gain = 2, (Input Span = [(VREF+)-(VREF-)]/2*A for unipolar).
//010 Gain = 4, (Input Span = [(VREF+)-(VREF-)]/4*A for unipolar).
//011 Gain = 8, (Input Span = [(VREF+)-(VREF-)]/8*A for unipolar).
//100 Gain = 16, (Input Span = [(VREF+)-(VREF-)]/16*A for unipolar).
//101 Gain = 32, (Input Span = [(VREF+)-(VREF-)]/32*A for unipolar).
//110 Gain = 64, (Input Span = [(VREF+)-(VREF-)]/64*A for unipolar).
																																								
//Bit WR (FRS = 0) WR (FRS = 1)
//0000 120 Sps 		100 Sps
//0001 60 Sps 		50 Sps
//0010 30 Sps 		25 Sps
//0011 15 Sps 		12.5 Sps
//0100 7.5 Sps 		6.25 Sps
//1000 3840 Sps 	3200 Sps
//1001 1920 Sps 	1600 Sps
//1010 960 Sps 		800 Sps
//1011 480 Sps 		400 Sps
//1100 240 Sps		200 Sps
