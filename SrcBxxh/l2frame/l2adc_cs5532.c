#include <inttypes.h>
#include "cmsis_os.h"
#include "l2adc_cs5532.h"
#include "l2adclibra.h"
#include "l3bfsc.h"
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
  while(val--)
  {
  }
	//HAL_Delay(val);
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
		//TempData = TempData >> (24 - zWeightSensorParam.WeightSensorAdcBitwidth);

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
			 AD_delay(200);
		}

    Clr_CS5532();
    for(i=0;i<100;i++)
    { 
	   AD_delay(200);
    }

		/*ʹϵͳ��������ģʽ*/
		taskENTER_CRITICAL();
		__disable_irq(); /* PROTECTION SPI READ/WRITE */
    CS5532InCmdMode();
		__enable_irq();
		taskEXIT_CRITICAL();

	     
    CS5532Reset(); 

		taskENTER_CRITICAL();
		__disable_irq(); /* PROTECTION SPI READ/WRITE */
    WriteConfigRegEx(0x00,0x01);
		__enable_irq();
		taskEXIT_CRITICAL();

   /*ת�����ʡ�0��9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 */
		
    //WriteChannelSetupRegEx(0,0,Chan0Gain,CollectionFF,1,0);		//setup1:ѡ�������ͨ��1,����4,˫����,������60sps
		IhuDebugPrint("InitCS5532: zWeightSensorParam.WeightSensorAdcGain=%d, zWeightSensorParam.WeightSensorAdcSampleFreq=%d\r\n", zWeightSensorParam.WeightSensorAdcGain, zWeightSensorParam.WeightSensorAdcSampleFreq);
		
		taskENTER_CRITICAL();
		__disable_irq(); /* PROTECTION SPI READ/WRITE */
		WriteChannelSetupRegEx(0,0,zWeightSensorParam.WeightSensorAdcGain,zWeightSensorParam.WeightSensorAdcSampleFreq,1,0);
		__enable_irq();
		taskEXIT_CRITICAL();
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
		taskENTER_CRITICAL();
		__disable_irq();   /* PROTECTION SPI READ/WRITE */
    WriteOffsetReg(AinPort,0x00);        //ƫ�ƼĴ���1д0
    PerformCalibration(0x02,AinPort);    //setup1:������У׼
    StartSeriesConversion(AinPort);      //setup1:����ģʽ
		__enable_irq();
		taskEXIT_CRITICAL();
}

/**********************************************************************
***CS5532 ADC��ʼ��
**���������IntEnable: �Ƿ��ж�ʹ��
***********************************************************************/
void CS5532Init()
{
//		taskENTER_CRITICAL();
//		__disable_irq();
		InitCS5532();  
//		__enable_irq();
//		taskEXIT_CRITICAL();
		
}

/* ===================================
** Weight Sensor Parameters and APIs
** ===================================
*/
//???
//-----Freq-----x64-----x32-----x16-----x8-----x4-----x2-----x1
//     7.5       19      20      21     22     22     22     22

extern weight_sensor_filter_t g_weight_sensor_filter;

uint32_t WeightSensorDefaultCalibrationValue(WeightSensorParamaters_t *pwsp)
{
		uint32_t wmc_id = 0;
		if(NULL == pwsp)
		{
				IhuDebugPrint("l2adc_cs5532: WeightSensorInit: NULL == pwsp, return IHU_FAILURE\r\n");
				return 1;
		}
//		0					1000
//1		0x0673f4	0x078e24
//2		0x06b526	0x07d0b5
//3		0x068e14	0x07a480
//4		0x06bedb	0x07d660
//5		0x067e60	0x079862
//6		0x06902c	0x07a9e5
//7		0x06b320	0x07cf4b
//8		0x070c3c	0x08329f
//9		0x069605	0x07aee0
//10	0x06bcc1	0x07d996

extern WmcInventory_t										zWmcInvenory;
		wmc_id = GetWmcId();
		//switch(zWmcInvenory.wmc_id.wmc_id)
		switch(wmc_id)
		{
				case 0:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x0673f4;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x078e24;
						pwsp->WeightSensorCalibrationFullWeight = 100000;

						break;
				
				case 1:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x0673f4;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x078e24;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
				
				case 2:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x06b526;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x07d0b5;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
				
				case 3:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x068e14;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x07a480;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
				
				case 4:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x06bedb;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x07d660;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
				
				case 5:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x067e60;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x079862;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
				
				case 6:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x06902c;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x07a9e5;
						pwsp->WeightSensorCalibrationFullWeight = 100000;	
						break;
				
				case 7:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x06b320;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x07cf4b;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
				
				case 8:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x070c3c;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x08329f;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
				
				case 9:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x069605;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x07aee0;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
				
				case 10:
						pwsp->WeightSensorCalibrationZeroAdcValue = 0x06bcc1;
						pwsp->WeightSensorCalibrationFullAdcValue = 0x07d996;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
				
				default:
						pwsp->WeightSensorCalibrationZeroAdcValue = 31560;
						pwsp->WeightSensorCalibrationFullAdcValue = 81512;
						pwsp->WeightSensorCalibrationFullWeight = 100000;		
						break;
		}
		
		IhuDebugPrint("WeightSensorDefaultCalibrationValue, ZeroAdc=%d(%x), FullAdc=%d(%x), FullWeight=%d, WmcId=%d\n", \
													pwsp->WeightSensorCalibrationZeroAdcValue, pwsp->WeightSensorCalibrationZeroAdcValue, \
													pwsp->WeightSensorCalibrationFullAdcValue, pwsp->WeightSensorCalibrationFullAdcValue, \
													pwsp->WeightSensorCalibrationFullWeight, zWmcInvenory.wmc_id.wmc_id);
		
		WeightSensorCalibrationKB(pwsp);
		return 0;
}
	
	

uint32_t WeightSensorInit(WeightSensorParamaters_t *pwsp)
{	
		if(NULL == pwsp)
		{
				IhuDebugPrint("l2adc_cs5532: WeightSensorInit: NULL == pwsp, return IHU_FAILURE\r\n");
				return 1;
		}
		
		pwsp->WeightSensorLoadDetectionTimeMs = WEIGHT_SENSOR_LOAD_DETECTION_TIME_MS;
		pwsp->WeightSensorLoadThread = WEIGHT_SENSOR_LOAD_THREDSHOLD;
		pwsp->WeightSensorEmptyDetectionTimeMs = WEIGHT_SENSOR_EMPTY_DETECTION_TIME_MS;		
		pwsp->WeightSensorEmptyThread = WEIGHT_SENSOR_EMPTY_DETECTION_THREDSHOLD;
		pwsp->RemainDetectionTimeSec = 2;
		pwsp->MaxAllowedWeight = 1000000;
//		pwsp->WeightSensorCalibrationZeroAdcValue = 31560;
//		pwsp->WeightSensorCalibrationFullAdcValue = 81512;
//		pwsp->WeightSensorCalibrationFullWeight = 100000;
		pwsp->WeightSensorStaticZeroValue = WEIGHT_SENSOR_STATIC_ZERO_VALUE;
		pwsp->WeightSensorTailorValue = WEIGHT_SENSOR_TAILOR_VALUE;
		pwsp->WeightSensorDynamicZeroThreadValue = WEIGHT_SENSOR_DYNAMIC_ZERO_THREDSHOLD_VALUE;
		pwsp->WeightSensorDynamicZeroHysteresisMs = WEIGHT_SENSOR_DYNAMIC_ZERO_HYSTERESIS_MS;
		
		//printf("l2adc_cs5532: WeightSensorInit: pwsp = 0x%08X\r\n", pwsp);
		pwsp->WeightSensorAdcSampleFreq = ADC_AMPLIFIER_WORDRATE_15SPS; //0:120sps  1:60sps  2:30sps  3:15sps  4:7.5sps  8:3840sps  9:1920sps  10:960sps  11:480sps  12:240sps
		pwsp->WeightSensorAdcGain = ADC_AMPLIFIER_GAIN_64X;			//0:X1  1:X2  2:X4  3:X8  4:X16  5:X32  6:X64
		pwsp->WeightSensorAdcBitwidth = SpsGainToBitwidthMapping(pwsp->WeightSensorAdcSampleFreq, pwsp->WeightSensorAdcGain);
		pwsp->WeightSensorInitOrNot = WEIGHT_SENSOR_HAD_INITED;
	
		zWeightSensorParam.WeightSensorFilterCoeff[0] = 0;//WS_BETA_NUM1;
		zWeightSensorParam.WeightSensorFilterCoeff[1] = 0;//WS_BETA_NUM2;
		zWeightSensorParam.WeightSensorFilterCoeff[2] = 12;
		zWeightSensorParam.WeightSensorFilterCoeff[3] = 30;

		g_weight_sensor_filter.adc_filtered[0] = 0;
		g_weight_sensor_filter.adc_filtered[1] = 0;
		g_weight_sensor_filter.beta_num[0] = zWeightSensorParam.WeightSensorFilterCoeff[0]; // WS_BETA_NUM1;
		g_weight_sensor_filter.beta_num[1] = zWeightSensorParam.WeightSensorFilterCoeff[1]; // WS_BETA_NUM2;
		g_weight_sensor_filter.stable_thresh = zWeightSensorParam.WeightSensorFilterCoeff[2]; // 12;    // ~2g
		g_weight_sensor_filter.change_thresh = zWeightSensorParam.WeightSensorFilterCoeff[3]; // 30;    // ~5g
		
		zWeightSensorParam.WeightSensorOutputValue[0] = 1; // 1 => Generate randam value */
		zWeightSensorParam.WeightSensorOutputValue[1] = 1; // 1 => Generate randam value */
		
//		pwsp->WeightSensorAdcSampleFreq = 0;//0:120sps  1:60sps  2:30sps  3:15sps  4:7.5sps  8:3840sps  9:1920sps  10:960sps  11:480sps  12:240sps
//		pwsp->WeightSensorAdcGain = 3;			//0:X1  1:X2  2:X4  3:X8  4:X16  5:X32  6:X64
//		pwsp->WeightSensorAdcBitwidth = 20;
//		pwsp->WeightSensorCalibrationFullWeight = 100000;//0.01g????
//		pwsp->WeightSensorInitOrNot = WEIGHT_SENSOR_HAD_INITED;
		WeightSensorDefaultCalibrationValue(pwsp);
		WeightSensorCalibrationKB(pwsp);
		
		CS5532Init();
	
		return 0;
}

/*
** GLOBAL VARIBLE FOR WEIGHT CALIBRATION
*/
WeightSensorCalirationKB_t wsckb;

void WeightSensorCalibrationKB(WeightSensorParamaters_t *pwsp)
{

	wsckb.b = pwsp->WeightSensorCalibrationZeroAdcValue;
	
	if(pwsp->WeightSensorCalibrationFullWeight == 0)
		return;

	wsckb.k = (double)(pwsp->WeightSensorCalibrationFullAdcValue-pwsp->WeightSensorCalibrationZeroAdcValue)/pwsp->WeightSensorCalibrationFullWeight;
}

/* Reconfig Sensor */
uint32_t weightSensorConfig(WeightSensorParamaters_t *pwsp)
{
	
		if(NULL == pwsp)
		{
				IhuErrorPrint("l2adc_cs5532: weightSensorConfig: NULL == pwsp, return IHU_FAILURE\r\n");
				return 1;
		}

#if 0 //// DONT KNOW THE REASON WHY ADD THIS 
    IhuDebugPrint("weightSensorConfig: AdcSampleFreq=%d AdcGain=%d ZeroAdcValue=%d FullAdcValue=%d FullWeight=%d\r\r",
                  pwsp->WeightSensorAdcSampleFreq,
                  pwsp->WeightSensorAdcGain,
                  pwsp->WeightSensorCalibrationZeroAdcValue,
                  pwsp->WeightSensorCalibrationFullAdcValue,
                  pwsp->WeightSensorCalibrationFullWeight);
#endif
    
		zWeightSensorParam.WeightSensorLoadDetectionTimeMs = pwsp->WeightSensorLoadDetectionTimeMs;
		zWeightSensorParam.WeightSensorLoadThread = pwsp->WeightSensorLoadThread;
		zWeightSensorParam.WeightSensorEmptyDetectionTimeMs = pwsp->WeightSensorEmptyDetectionTimeMs;		
		zWeightSensorParam.WeightSensorEmptyThread = pwsp->WeightSensorEmptyThread;
		zWeightSensorParam.WeightSensorPickupThread = pwsp->WeightSensorPickupThread;
		zWeightSensorParam.WeightSensorPickupDetectionTimeMs = pwsp->WeightSensorPickupDetectionTimeMs;
		zWeightSensorParam.StardardReadyTimeMs = pwsp->StardardReadyTimeMs;		
		zWeightSensorParam.MaxAllowedWeight = pwsp->MaxAllowedWeight;
    zWeightSensorParam.RemainDetectionTimeSec = pwsp->RemainDetectionTimeSec;

		IhuDebugPrint("zWeightSensorParam.WeightSensorLoadDetectionTimeMs=%d\n", zWeightSensorParam.WeightSensorLoadDetectionTimeMs);
		IhuDebugPrint("zWeightSensorParam.WeightSensorLoadThread=%d\n", zWeightSensorParam.WeightSensorLoadThread);
		IhuDebugPrint("zWeightSensorParam.WeightSensorEmptyDetectionTimeMs=%d\n", zWeightSensorParam.WeightSensorEmptyDetectionTimeMs);
		IhuDebugPrint("zWeightSensorParam.WeightSensorEmptyThread=%d\n", zWeightSensorParam.WeightSensorEmptyThread);

		IhuDebugPrint("zWeightSensorParam.WeightSensorPickupThread=%d\n", zWeightSensorParam.WeightSensorPickupThread);
		IhuDebugPrint("zWeightSensorParam.WeightSensorPickupDetectionTimeMs=%d\n", zWeightSensorParam.WeightSensorPickupDetectionTimeMs);
		IhuDebugPrint("zWeightSensorParam.StardardReadyTimeMs=%d\n", zWeightSensorParam.StardardReadyTimeMs);
		IhuDebugPrint("zWeightSensorParam.MaxAllowedWeight=%d\n", zWeightSensorParam.MaxAllowedWeight);
		IhuDebugPrint("zWeightSensorParam.RemainDetectionTimeSec=%d\n", zWeightSensorParam.RemainDetectionTimeSec);
		
		if(zWeightSensorParam.RemainDetectionTimeSec < 2)
		{
				IhuDebugPrint("zWeightSensorParam.RemainDetectionTimeSec=%d, < 2, force set to 2 sec\n", zWeightSensorParam.RemainDetectionTimeSec);
				zWeightSensorParam.RemainDetectionTimeSec = 2;
		}
		else
		{
				IhuDebugPrint("zWeightSensorParam.RemainDetectionTimeSec=%d\n", zWeightSensorParam.RemainDetectionTimeSec);
		}
		

		//zWeightSensorParam.WeightSensorInitOrNot = pwsp->WeightSensorInitOrNot;
		zWeightSensorParam.WeightSensorAdcSampleFreq = pwsp->WeightSensorAdcSampleFreq;
		zWeightSensorParam.WeightSensorAdcGain = pwsp->WeightSensorAdcGain;
		zWeightSensorParam.WeightSensorAdcBitwidth = pwsp->WeightSensorAdcBitwidth;
		//zWeightSensorParam.WeightSensorAdcValue = pwsp->WeightSensorAdcValue;
		zWeightSensorParam.WeightSensorCalibrationZeroAdcValue = pwsp->WeightSensorCalibrationZeroAdcValue;
		zWeightSensorParam.WeightSensorCalibrationFullAdcValue = pwsp->WeightSensorCalibrationFullAdcValue;
		zWeightSensorParam.WeightSensorCalibrationFullWeight = pwsp->WeightSensorCalibrationFullWeight;
		zWeightSensorParam.WeightSensorStaticZeroValue = pwsp->WeightSensorStaticZeroValue;
		zWeightSensorParam.WeightSensorTailorValue = pwsp->WeightSensorTailorValue;
		zWeightSensorParam.WeightSensorDynamicZeroThreadValue = pwsp->WeightSensorDynamicZeroThreadValue;
		zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs = pwsp->WeightSensorDynamicZeroHysteresisMs;

		IhuDebugPrint("zWeightSensorParam.WeightSensorAdcSampleFreq=%d\n", zWeightSensorParam.WeightSensorAdcSampleFreq);
		IhuDebugPrint("zWeightSensorParam.WeightSensorAdcGain=%d\n", zWeightSensorParam.WeightSensorAdcGain);
		IhuDebugPrint("zWeightSensorParam.WeightSensorAdcBitwidth=%d\n", zWeightSensorParam.WeightSensorAdcBitwidth);
		IhuDebugPrint("zWeightSensorParam.WeightSensorCalibrationZeroAdcValue=%d\n", zWeightSensorParam.WeightSensorCalibrationZeroAdcValue);
		IhuDebugPrint("zWeightSensorParam.WeightSensorCalibrationFullAdcValue=%d\n", zWeightSensorParam.WeightSensorCalibrationFullAdcValue);
		
		IhuDebugPrint("zWeightSensorParam.WeightSensorCalibrationFullWeight=%d\n", zWeightSensorParam.WeightSensorCalibrationFullWeight);
		IhuDebugPrint("zWeightSensorParam.WeightSensorStaticZeroValue=%d\n", zWeightSensorParam.WeightSensorStaticZeroValue);
		IhuDebugPrint("zWeightSensorParam.WeightSensorTailorValue=%d\n", zWeightSensorParam.WeightSensorTailorValue);
		IhuDebugPrint("zWeightSensorParam.WeightSensorDynamicZeroThreadValue=%d\n", zWeightSensorParam.WeightSensorDynamicZeroThreadValue);
		IhuDebugPrint("zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs=%d\n", zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs);

		zWeightSensorParam.WeightSensorFilterCoeff[0] = pwsp->WeightSensorFilterCoeff[0];
		zWeightSensorParam.WeightSensorFilterCoeff[1] = pwsp->WeightSensorFilterCoeff[1];
		zWeightSensorParam.WeightSensorFilterCoeff[2] = pwsp->WeightSensorFilterCoeff[2];
		zWeightSensorParam.WeightSensorFilterCoeff[3] = pwsp->WeightSensorFilterCoeff[3];
		zWeightSensorParam.WeightSensorOutputValue[0] = 1; //pwsp->WeightSensorOutputValue[0]; /* 1: IHU, FIXED AS IT IS DIFFICULT FOR FACTORY/JIANLIN TO REMEMBER */
		zWeightSensorParam.WeightSensorOutputValue[1] = 0; //pwsp->WeightSensorOutputValue[1]; /* 0: Read ADC, FIXED AS IT IS DIFFICULT FOR FACTORY/JIANLIN TO REMEMBER */
		zWeightSensorParam.WeightSensorOutputValue[2] = pwsp->WeightSensorOutputValue[2];
		zWeightSensorParam.WeightSensorOutputValue[3] = pwsp->WeightSensorOutputValue[3];

		IhuDebugPrint("zWeightSensorParam.WeightSensorFilterCoeff[0]=%d\n", zWeightSensorParam.WeightSensorFilterCoeff[0]);
		IhuDebugPrint("zWeightSensorParam.WeightSensorFilterCoeff[1]=%d\n", zWeightSensorParam.WeightSensorFilterCoeff[1]);
		IhuDebugPrint("zWeightSensorParam.WeightSensorFilterCoeff[2]=%d\n", zWeightSensorParam.WeightSensorFilterCoeff[2]);
		IhuDebugPrint("zWeightSensorParam.WeightSensorFilterCoeff[3]=%d\n", zWeightSensorParam.WeightSensorFilterCoeff[3]);
		if( (0==zWeightSensorParam.WeightSensorFilterCoeff[0]) && (0==zWeightSensorParam.WeightSensorFilterCoeff[1]) )
		{
				IhuDebugPrint("zWeightSensorParam.WeightSensorFilterCoeff[0,1]=%d, MOVING EVERAGE MODE CONFIGURE FROM HCU!\n", zWeightSensorParam.WeightSensorFilterCoeff[0]);
		}
		else
		{
				IhuDebugPrint("zWeightSensorParam.WeightSensorFilterCoeff[0,1]=%d, FILTERING MODE CONFIGURE FROM HCU!\n", zWeightSensorParam.WeightSensorFilterCoeff[0]);			
		}
		
		IhuDebugPrint("zWeightSensorParam.WeightSensorOutputValue[0]=%d\n", zWeightSensorParam.WeightSensorOutputValue[0]);
		IhuDebugPrint("zWeightSensorParam.WeightSensorOutputValue[1]=%d\n", zWeightSensorParam.WeightSensorOutputValue[1]);
		IhuDebugPrint("zWeightSensorParam.WeightSensorOutputValue[2]=%d\n", zWeightSensorParam.WeightSensorOutputValue[2]);
		IhuDebugPrint("zWeightSensorParam.WeightSensorOutputValue[3]=%d\n", zWeightSensorParam.WeightSensorOutputValue[3]);
		
		/* THESE IS ONLY FOR TEST */
		/* 1 == zWeightSensorParam.WeightSensorOutputValue[0], LOAD DEFAULT PARAMETER, = 0 Follow SetConfigReq */
		/* 1 == zWeightSensorParam.WeightSensorOutputValue[1], Auto Generate Value, without reading ADC */
    /* 2 == */		
		if(1 == zWeightSensorParam.WeightSensorOutputValue[0])
		{
				IhuDebugPrint("======================================================================================\n");
				IhuDebugPrint("zWeightSensorParam.WeightSensorOutputValue[0]=%d, LOAD DEFAULT PARAMETER FOR SENSOR!!!\n", zWeightSensorParam.WeightSensorOutputValue[0]);

				g_weight_sensor_filter.adc_filtered[0] = 0;
				g_weight_sensor_filter.adc_filtered[1] = 0;
				g_weight_sensor_filter.beta_num[0] = 0;//zWeightSensorParam.WeightSensorFilterCoeff[0]; // WS_BETA_NUM1;
				g_weight_sensor_filter.beta_num[1] = 0;//zWeightSensorParam.WeightSensorFilterCoeff[1]; // WS_BETA_NUM2;
				g_weight_sensor_filter.stable_thresh = 12;    // ~2g
				g_weight_sensor_filter.change_thresh = 30;    // ~5g
				IhuDebugPrint("g_weight_sensor_filter.beta_num[0]=%d\n", g_weight_sensor_filter.beta_num[0]);
				IhuDebugPrint("g_weight_sensor_filter.beta_num[1]=%d\n", g_weight_sensor_filter.beta_num[1]);
				IhuDebugPrint("g_weight_sensor_filter.stable_thresh=%d\n", g_weight_sensor_filter.stable_thresh);
				IhuDebugPrint("g_weight_sensor_filter.change_thresh=%d\n", g_weight_sensor_filter.change_thresh);
				if( (0==g_weight_sensor_filter.beta_num[0]) && (0==g_weight_sensor_filter.beta_num[1]) )
				{
						IhuDebugPrint("g_weight_sensor_filter.beta_num[0,1]=%d, MOVING EVERAGE MODE FROM DEFAULT PARAMETER!!!\n", g_weight_sensor_filter.beta_num[0]);
				}
				else
				{
						IhuDebugPrint("g_weight_sensor_filter.beta_num[0,1]=%d, FILTERING MODE FROM DEFAULT PARAMETER!!!\n", g_weight_sensor_filter.beta_num[0]);
				}
			
				/* THIS IS FOR TEST ONLY, NEEDS TO REMOVE AFTER PARAMETERS SAVED IN AWS */
				zWeightSensorParam.WeightSensorLoadDetectionTimeMs = WEIGHT_SENSOR_LOAD_DETECTION_TIME_MS;
				zWeightSensorParam.WeightSensorLoadThread = WEIGHT_SENSOR_LOAD_THREDSHOLD;
				zWeightSensorParam.WeightSensorEmptyDetectionTimeMs = WEIGHT_SENSOR_EMPTY_DETECTION_TIME_MS;		
				zWeightSensorParam.WeightSensorEmptyThread = WEIGHT_SENSOR_EMPTY_DETECTION_THREDSHOLD;
				zWeightSensorParam.RemainDetectionTimeSec = 10;
				zWeightSensorParam.MaxAllowedWeight = 1000000;

				zWeightSensorParam.WeightSensorStaticZeroValue = WEIGHT_SENSOR_STATIC_ZERO_VALUE;
				zWeightSensorParam.WeightSensorTailorValue = WEIGHT_SENSOR_TAILOR_VALUE;
				zWeightSensorParam.WeightSensorDynamicZeroThreadValue = WEIGHT_SENSOR_DYNAMIC_ZERO_THREDSHOLD_VALUE;
				zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs = WEIGHT_SENSOR_DYNAMIC_ZERO_HYSTERESIS_MS;
				IhuDebugPrint("zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs=%d\n", zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs);

				zWeightSensorParam.WeightSensorAdcSampleFreq = ADC_AMPLIFIER_WORDRATE_15SPS; //0:120sps  1:60sps  2:30sps  3:15sps  4:7.5sps  8:3840sps  9:1920sps  10:960sps  11:480sps  12:240sps
				zWeightSensorParam.WeightSensorAdcGain = ADC_AMPLIFIER_GAIN_64X;			//0:X1  1:X2  2:X4  3:X8  4:X16  5:X32  6:X64
				zWeightSensorParam.WeightSensorAdcBitwidth = SpsGainToBitwidthMapping(zWeightSensorParam.WeightSensorAdcSampleFreq, zWeightSensorParam.WeightSensorAdcGain);

				IhuDebugPrint("zWeightSensorParam.WeightSensorLoadDetectionTimeMs=%d\n", zWeightSensorParam.WeightSensorLoadDetectionTimeMs);
				IhuDebugPrint("zWeightSensorParam.WeightSensorLoadThread=%d\n", zWeightSensorParam.WeightSensorLoadThread);
				IhuDebugPrint("zWeightSensorParam.WeightSensorEmptyDetectionTimeMs=%d\n", zWeightSensorParam.WeightSensorEmptyDetectionTimeMs);
				IhuDebugPrint("zWeightSensorParam.WeightSensorEmptyThread=%d\n", zWeightSensorParam.WeightSensorEmptyThread);

				IhuDebugPrint("zWeightSensorParam.WeightSensorPickupThread=%d\n", zWeightSensorParam.WeightSensorPickupThread);
				IhuDebugPrint("zWeightSensorParam.WeightSensorPickupDetectionTimeMs=%d\n", zWeightSensorParam.WeightSensorPickupDetectionTimeMs);
				IhuDebugPrint("zWeightSensorParam.StardardReadyTimeMs=%d\n", zWeightSensorParam.StardardReadyTimeMs);
				IhuDebugPrint("zWeightSensorParam.MaxAllowedWeight=%d\n", zWeightSensorParam.MaxAllowedWeight);
				IhuDebugPrint("zWeightSensorParam.RemainDetectionTimeSec=%d\n", zWeightSensorParam.RemainDetectionTimeSec);

				IhuDebugPrint("zWeightSensorParam.WeightSensorAdcSampleFreq=%d\n", zWeightSensorParam.WeightSensorAdcSampleFreq);
				IhuDebugPrint("zWeightSensorParam.WeightSensorAdcGain=%d\n", zWeightSensorParam.WeightSensorAdcGain);
				IhuDebugPrint("zWeightSensorParam.WeightSensorAdcBitwidth=%d\n", zWeightSensorParam.WeightSensorAdcBitwidth);
				IhuDebugPrint("zWeightSensorParam.WeightSensorCalibrationZeroAdcValue=%d\n", zWeightSensorParam.WeightSensorCalibrationZeroAdcValue);
				IhuDebugPrint("zWeightSensorParam.WeightSensorCalibrationFullAdcValue=%d\n", zWeightSensorParam.WeightSensorCalibrationFullAdcValue);
				
				IhuDebugPrint("zWeightSensorParam.WeightSensorCalibrationFullWeight=%d\n", zWeightSensorParam.WeightSensorCalibrationFullWeight);
				IhuDebugPrint("zWeightSensorParam.WeightSensorStaticZeroValue=%d\n", zWeightSensorParam.WeightSensorStaticZeroValue);
				IhuDebugPrint("zWeightSensorParam.WeightSensorTailorValue=%d\n", zWeightSensorParam.WeightSensorTailorValue);
				IhuDebugPrint("zWeightSensorParam.WeightSensorDynamicZeroThreadValue=%d\n", zWeightSensorParam.WeightSensorDynamicZeroThreadValue);
				IhuDebugPrint("zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs=%d\n", zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs);
		
				/* THESE LOAD THE DEFAULT PARAMETERS FOR CALIBRATION */
				IhuDebugPrint("WeightSensorDefaultCalibrationValue(&zWeightSensorParam);\n", zWeightSensorParam.WeightSensorDynamicZeroHysteresisMs);
				WeightSensorDefaultCalibrationValue(&zWeightSensorParam);
				IhuDebugPrint("======================================================================================\n");
		}
		else
		{
				IhuDebugPrint("zWeightSensorParam.WeightSensorOutputValue[0]=%d, GET HCU PARAMETER FOR SENSOR!!!\n", zWeightSensorParam.WeightSensorOutputValue[0]);
		}
		
		if(1 == zWeightSensorParam.WeightSensorOutputValue[1])
		{
				IhuDebugPrint("1 == zWeightSensorParam.WeightSensorOutputValue[1], ADC Simu Mode [100000-400000], NOT Real Value\n");
		}
		else
		{
				IhuDebugPrint("0 == zWeightSensorParam.WeightSensorOutputValue[1], ADC Normal Mode, Real Value !!!\n");
		}
		
		IhuDebugPrint("WeightSensorCalibrationKB(&zWeightSensorParam)\n");
		WeightSensorCalibrationKB(&zWeightSensorParam);
		
		IhuDebugPrint("InitWeightAdcBfscLocalParam(&zWeightSensorParam)\n");
		InitWeightAdcBfscLocalParam(&zWeightSensorParam);
		
		IhuDebugPrint("CS5532Init(), ReInit CS5532 ADC for Weight Sendor\n");
		CS5532Init();
		
		//zWeightSensorParam.WeightSensorInitOrNot = WEIGHT_SENSOR_HAD_INITED;
		
	//		  g_weight_sensor_filter.adc_filtered[0] = 0;
	//			g_weight_sensor_filter.adc_filtered[1] = 0;
	//			g_weight_sensor_filter.beta_num[0] = zWeightSensorParam.WeightSensorFilterCoeff[0]; // WS_BETA_NUM1;
	//			g_weight_sensor_filter.beta_num[1] = zWeightSensorParam.WeightSensorFilterCoeff[1]; // WS_BETA_NUM2;
	//			g_weight_sensor_filter.stable_thresh = zWeightSensorParam.WeightSensorFilterCoeff[2]; // 12;    // ~2g
	//			g_weight_sensor_filter.change_thresh = zWeightSensorParam.WeightSensorFilterCoeff[3]; // 30;    // ~5g
		
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
		taskENTER_CRITICAL();
		__disable_irq(); /* THIS IS VERY IMPORTANT, OR, THE SPI WILL BE INTERRUPT and THE ADC VALUE WILL BE JUMP UP/DOWN */
		if(ReadWheChanOk())
		{
				temp += ReadSeriesADValue();
		}
		__enable_irq();
		taskEXIT_CRITICAL();
		HAL_Delay(20);
	}
	temp = temp >> 5;
	
	pwsp->WeightSensorCalibrationZeroAdcValue = temp;
	
	WeightSensorCalibrationKB(pwsp);
	
  IhuDebugPrint("l2adc_cs5532: WeightSensorCalibrationZero: ZeroADC=%d FullADC=%d FullWeigth=%d k=%f b=%d\r\n", 
								pwsp->WeightSensorCalibrationZeroAdcValue, pwsp->WeightSensorCalibrationFullAdcValue, pwsp->WeightSensorCalibrationFullWeight, wsckb.k, wsckb.b);

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
		taskENTER_CRITICAL();
		__disable_irq(); /* THIS IS VERY IMPORTANT, OR, THE SPI WILL BE INTERRUPT and THE ADC VALUE WILL BE JUMP UP/DOWN */
		if(ReadWheChanOk())
		{
				temp += ReadSeriesADValue();
		}
		__enable_irq();
		taskEXIT_CRITICAL();
		HAL_Delay(20);
	}
	temp = temp >> 5;
	
	pwsp->WeightSensorCalibrationFullAdcValue = temp;
	
  WeightSensorCalibrationKB(pwsp);

  IhuDebugPrint("l2adc_cs5532: WeightSensorCalibrationFull: ZeroADC=%d FullADC=%d FullWeigth=%d k=%f b=%d\r\n", 
								pwsp->WeightSensorCalibrationZeroAdcValue, pwsp->WeightSensorCalibrationFullAdcValue, pwsp->WeightSensorCalibrationFullWeight, wsckb.k, wsckb.b);
  
	return pwsp->WeightSensorCalibrationFullAdcValue;
	
}
////??K?B
//void WeightSensorCalibrationKB(WeightSensorParamaters_t *pwsp)
//{
////	pwsp->WeightSensorCalibrationB = pwsp->WeightSensorCalibrationZeroAdcValue;
////	pwsp->WeightSensorCalibrationK = (double)(pwsp->WeightSensorCalibrationFullAdcValue-pwsp->WeightSensorCalibrationZeroAdcValue)/pwsp->WeightSensorCalibrationFullWeight;
//}

//??????
uint32_t WeightSensorReadInstantAdc()
{
		int i;
		uint32_t temp = 0;
		
		for(i = 0; i < 16; i ++)
		{
				taskENTER_CRITICAL();
				__disable_irq(); /* THIS IS VERY IMPORTANT, OR, THE SPI WILL BE INTERRUPT and THE ADC VALUE WILL BE JUMP UP/DOWN */
				if(ReadWheChanOk())
				{
					temp += ReadSeriesADValue();
				}
				__enable_irq();
				taskEXIT_CRITICAL();
				HAL_Delay(200);
		}
		
		temp = temp >> 4;
		return temp;
}


int32_t WeightSensorReadCurrent(WeightSensorParamaters_t *pwsp)
{
	int i;
	uint32_t temp = 0;
	int32_t temp2 = 0;
	int32_t temp3 = 0;
	//static uint8_t sw = 0;
	
	static int test_sample_count = 0;
	static int32_t test_sample_value = 0;
	
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
	
	taskENTER_CRITICAL();
	__disable_irq();  /* THIS IS VERY IMPORTANT, OR, THE SPI WILL BE INTERRUPT and THE ADC VALUE WILL BE JUMP UP/DOWN */
	for(i = 0; i < 1; i ++)
	{
		if(ReadWheChanOk())
		{
			temp += ReadSeriesADValue();
		}
		//HAL_Delay(10);
	}
	__enable_irq();	
	taskEXIT_CRITICAL();
	
	temp = temp >> 0;
	
	pwsp->WeightSensorAdcValue = temp;
	
	if(wsckb.k == 0)
		return 0xFFFFFFFF;
	
	temp2 = temp - wsckb.b;
	temp3 = temp2 / wsckb.k;
	
//	IhuDebugPrint("l2adc_cs5532: WeightSensorReadCurrent: temp=%d, temp2=%d, ZeroAdc=%d, FullAdc=%d, FullWeight=%d, k=%f, b=%d, temp3=%d\r\n",
//								temp, temp2, pwsp->WeightSensorCalibrationZeroAdcValue, 
//								pwsp->WeightSensorCalibrationFullAdcValue, pwsp->WeightSensorCalibrationFullWeight, 
//	              wsckb.k, wsckb.b, temp3);
//	printf("l2adc_cs5532: WeightSensorReadCurrent: temp=%d, temp2=%d, ZeroAdc=%d, FullAdc=%d, FullWeight=%d, k=%f, b=%d, temp3=%d\r\n",
//								temp, temp2, pwsp->WeightSensorCalibrationZeroAdcValue, 
//								pwsp->WeightSensorCalibrationFullAdcValue, pwsp->WeightSensorCalibrationFullWeight, 
//	              wsckb.k, wsckb.b, temp3);
  //temp = 10000 + (rand() % 5000) - 2500;
	//return temp; ///THIS IS ONLY FOR TEST

	/* 0 == zWeightSensorParam.WeightSensorOutputValue[1] ==> THIS IS NORMAL CAEE */
	if(0 == zWeightSensorParam.WeightSensorOutputValue[1])
	{
			return temp3;
	}
	/* Test Code Skip All ADC, but Generate ramdom ADC/Weight */
	else if(1 == zWeightSensorParam.WeightSensorOutputValue[1])
	{
extern BfscWmcState_t										zBfscWmcState;			
			if( HUITP_IEID_SUI_BFSC_COMINETYPE_NULL == zBfscWmcState.last_combin_type.WeightCombineType )
			{
					if(0 == test_sample_count)
					{
							test_sample_value = 10000 + (rand() % 30000);  /* 100g ~ 400g */
							test_sample_count = 1;
					}
					return test_sample_value + ((rand() % (WEIGHT_SENSOR_LOAD_THREDSHOLD-2)) - ((WEIGHT_SENSOR_LOAD_THREDSHOLD>>1) - 1) );
					
			}
			else /* HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT */
			{
					test_sample_value = 10000 + (rand() % 30000);  /* Next random data, 100g ~ 400g */
					return ((rand() % WEIGHT_SENSOR_LOAD_THREDSHOLD) - (WEIGHT_SENSOR_LOAD_THREDSHOLD>>1));  // SO THAT the COMBIN_OUT MESSAGE WILL BE SENT OUT //
			}		
	}
	
	else if(2 == zWeightSensorParam.WeightSensorOutputValue[1])
	{
extern BfscWmcState_t										zBfscWmcState;			
//			if( HUITP_IEID_SUI_BFSC_COMINETYPE_NULL == zBfscWmcState.last_combin_type.WeightCombineType )
//			{
//					if(0 == test_sample_count)
//					{
//							test_sample_value = 10000 + (rand() % 30000);  /* 100g ~ 400g */
//							test_sample_count = 1;
//					}
//					return test_sample_value + ((rand() % (WEIGHT_SENSOR_LOAD_THREDSHOLD-2)) - ((WEIGHT_SENSOR_LOAD_THREDSHOLD>>1) - 1) );
//					
//			}
//			else /* HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT */
			{
					test_sample_value = 10000 + (rand() % 30000);  /* Next random data, 100g ~ 400g */
					return ((rand() % WEIGHT_SENSOR_LOAD_THREDSHOLD) - (WEIGHT_SENSOR_LOAD_THREDSHOLD>>1));  // SO THAT the COMBIN_OUT MESSAGE WILL BE SENT OUT //
			}		
	}
	
	return 0;
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
				IhuDebugPrint("l2adc_cs5532: SpsGainToBitwidthMapping: Invalid parameter, gain_index(%d)>=GAIN_INDEX_NUMBER(%d), or, wordrate_index(%d) >= WORDRATE_INDEX_NUMBER(%d), return 0xFFFFFFFF\r\b", gain_index, GAIN_INDEX_NUMBER, wordrate_index, WORDRATE_INDEX_NUMBER);
				return 0xFFFFFFFF;
		}
		
		IhuDebugPrint("l2adc_cs5532: SpsGainToBitwidthMapping: [wordrate_index(%d), gain_index(%d)] => Bitwidth(%d)\r\n", wordrate_index, gain_index, SpsGainToBitwidthMappingTable[wordrate_index][gain_index]);
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//weight_sensor_cmd_t g_weight_sensor_cmd;

//int weight_sensor_map_adc_to_weight(uint32_t adc_value)
//{
//  int den, weight;
//  
//  den = (zWeightSensorParam.WeightSensorCalibrationFullAdcValue - zWeightSensorParam.WeightSensorCalibrationZeroAdcValue);
//  if(den <= 0)
//    den = 6000;  // set a default value
//  
//  weight = (adc_value-zWeightSensorParam.WeightSensorCalibrationZeroAdcValue);
//  if(weight < 0)
//    weight = 0;
//  weight = weight*zWeightSensorParam.WeightSensorCalibrationFullWeight/den;
//	
//  weight = 10000 + (rand() % 10000);

//  return weight;
//}

//// send a comman to weight sensor
//int weight_sensor_send_cmd(uint32_t type)
//{
//  weight_sensor_cmd_t command;

//  command.valid = 1;
//  command.type = type;
//  command.reserved = 0;
//  
//  taskENTER_CRITICAL();
//  g_weight_sensor_cmd = command;
//  taskEXIT_CRITICAL();
//  
//  return g_weight_sensor_cmd.valid;
//}

//// weight sensor task receive a command
//int weight_sensor_recv_cmd(weight_sensor_cmd_t *command)
//{
//  taskENTER_CRITICAL();
//  *command = g_weight_sensor_cmd;
//  g_weight_sensor_cmd.valid = 0;
//  taskEXIT_CRITICAL();
//  
//  return command->valid;
//}

//weight_sensor_filter_t g_weight_sensor_filter;

//#define WS_BETA_DEN   4  // DEN=1<<4
//#define WS_BETA_NUM1  15
//#define WS_BETA_NUM2  10
//uint32_t weight_sensor_read_and_filtering(weight_sensor_filter_t *wsf)
//{
//  int i=0, adc_raw=0;
//  int temp;

//  do
//  {
//    if(ReadWheChanOk())
//    {
//      temp = ReadSeriesADValue();
//      // skip the abnormal adc value
//      if((temp > zWeightSensorParam.WeightSensorCalibrationZeroAdcValue/2) && (temp < zWeightSensorParam.WeightSensorCalibrationFullAdcValue*15))
//      {
//        adc_raw += temp;
//        i++;
//      }
//      //IhuDebugPrint("adc_raw=%d\n", temp);
//    }

//    osDelay(8);
//  }while(i<4);

//  adc_raw = adc_raw >> 2;
//  
//  temp = ((wsf->adc_filtered[0] - adc_raw) * wsf->beta_num[0]);
//  wsf->adc_filtered[0] = (temp >> WS_BETA_DEN) + adc_raw;
//  temp = ((wsf->adc_filtered[1] - adc_raw) * wsf->beta_num[1]);
//  wsf->adc_filtered[1] = (temp >> WS_BETA_DEN) + adc_raw;
//  //wsf->adc_filtered[0] = (((wsf->adc_filtered[0] - adc_raw) * wsf->beta_num[0]) >> WS_BETA_DEN) + adc_raw;
//  //wsf->adc_filtered[1] = (((wsf->adc_filtered[1] - adc_raw) * wsf->beta_num[1]) >> WS_BETA_DEN) + adc_raw;

//  //IhuDebugPrint("adc_raw=%d adc_filtered=(%d, %d)\n", adc_raw, wsf->adc_filtered[0], wsf->adc_filtered[1]);
//  
//  if(abs(wsf->adc_filtered[0] - wsf->adc_filtered[1]) < wsf->stable_thresh)
//  {
//    return 1;
//  }

//  return 0;
//}

//#define NOTEST
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// weight sensor task
//// 1) process the L3BF message
//// 2) filter the ADC and report the weight value
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void weight_sensor_task(void const *param)
//{
//	weight_sensor_cmd_t command;
//  #ifdef NOTEST
//	int is_started = 0;
//  #else
//  int is_started = 1;
//  #endif
//	uint32_t last_adc_filtered = 0xFFFF, adc_filtered;
//  uint32_t last_adc_tick = 0xFFFF, current_tick, repeat_times = 0;
//  msg_struct_l3bfsc_weight_ind_t weight_ind;
//  OPSTAT ret;
//  WeightSensorParamaters_t *wsparm = (WeightSensorParamaters_t *)param;

//  // give control to other tasks
//  osDelay(20);
//  
//  //初始化Weight Sensor ADC
//	WeightSensorInit(&zWeightSensorParam);
//	IhuDebugPrint("L3BFSC: fsm_bfsc_init: WeightSensorInit()\r\n");

//  zWeightSensorParam.WeightSensorCalibrationZeroAdcValue = 7125;
//  zWeightSensorParam.WeightSensorCalibrationFullAdcValue = 13375;
//  zWeightSensorParam.WeightSensorCalibrationFullWeight = 100000;
//  
//  g_weight_sensor_filter.adc_filtered[0] = 0;
//  g_weight_sensor_filter.adc_filtered[1] = 0;
//  g_weight_sensor_filter.beta_num[0] = WS_BETA_NUM1;
//  g_weight_sensor_filter.beta_num[1] = WS_BETA_NUM2;
//  g_weight_sensor_filter.stable_thresh = 12;    // ~2g
//  g_weight_sensor_filter.change_thresh = 30;    // ~5g

//	while(1)
//	{		
//		/* wait for a new command */
//    // process command
//		if(weight_sensor_recv_cmd(&command))
//    {
//      if(command.type == WIGHT_SENSOR_CMD_TYPE_STOP)
//        is_started = 0;
//      else if(command.type == WIGHT_SENSOR_CMD_TYPE_START)
//        is_started = 1;
//    }

//    if(is_started)
//    {
//      // read sensor for a stable value
//      if(weight_sensor_read_and_filtering(&g_weight_sensor_filter))
//      {
//        adc_filtered = (g_weight_sensor_filter.adc_filtered[0] + g_weight_sensor_filter.adc_filtered[1]) >> 1;

//        if(abs(adc_filtered - last_adc_filtered) > g_weight_sensor_filter.change_thresh)
//        {
//          last_adc_filtered = adc_filtered;
//          last_adc_tick = osKernelSysTick();
//          repeat_times = 0;
//          
//          weight_ind.adc_filtered = adc_filtered;
//          weight_ind.repeat_times = repeat_times;
//          
//					IhuDebugPrint("tick%d: WS: new weight ind: adc_filtered=%d weight=%d\n", last_adc_tick, adc_filtered, weight_sensor_map_adc_to_weight(adc_filtered));
//					#ifdef NOTEST
//          // weight changed, send weight indication to L3BFSC
//          ret = ihu_message_send(MSG_ID_L3BFSC_WMC_WEIGHT_IND, TASK_ID_BFSC, TASK_ID_BFSC, \
//														&weight_ind, sizeof(msg_struct_l3bfsc_weight_ind_t));

//          if (ret == IHU_FAILURE){
//      			IhuErrorPrint("WS: Send new weight ind message error!\n");
//      		}
//          #endif
//        }
//        else
//        {
//          current_tick = osKernelSysTick();
//          if(current_tick - last_adc_tick > 2000)
//          {
//            last_adc_tick = current_tick;
//            repeat_times ++;
//            
//            weight_ind.adc_filtered = adc_filtered;
//            weight_ind.repeat_times = repeat_times;
//            
//						IhuDebugPrint("tick%d: WS: repeat weight ind: adc_filtered=%d weight=%d repeat_times=%d\n", last_adc_tick, adc_filtered, weight_sensor_map_adc_to_weight(adc_filtered), repeat_times);

//            #ifdef NOTEST
//            // weight changed, send weight indication to L3BFSC
//            ret = ihu_message_send(MSG_ID_L3BFSC_WMC_WEIGHT_IND, TASK_ID_BFSC, TASK_ID_BFSC, \
//  														&weight_ind, sizeof(msg_struct_l3bfsc_weight_ind_t));

//            if (ret == IHU_FAILURE){
//        			IhuErrorPrint("WS: Send repeat weight ind message error!\n");
//        		}
//            #endif
//          }
//        }
//      }
//    }
//    else
//      osDelay(10);
//	}
//}

