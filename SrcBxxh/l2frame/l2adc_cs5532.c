#include <inttypes.h>
#include "cmsis_os.h"
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
			
/*转换速率　　　0～9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240*/
uint8_t  CollectionFF =       0x00      ;          //采样频率
/*通道0增益     0～6   0:1  1:2  2:4  3:8  4:16  5:32  6:64*/ 
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
* 名称：MSPI0_WR_Data()
* 功能：向SSI控制器发送数据。
* 入口参数：send_data
* 出口参数：无
* 说明：
****************************************************************************/
uint8_t MSPI0_WR_Data(uint8_t send_data)
{
	 HAL_SPI_Transmit(&hspi3,&send_data,1,1);
   return(0);
}
/****************************************************************************
* 名称：MSPI0_RD_Data()
* 功能：向SSI控制器接收数据。
* 入口参数：NONE
* 出口参数：无
* 说明：
****************************************************************************/
uint8_t MSPI0_RD_Data()
{
  uint8_t rec;
	uint8_t send = 0x00;
	
	HAL_SPI_TransmitReceive(&hspi3, (uint8_t*)&send, (uint8_t *)&rec, 1, 1);

  return(rec);
}
	
/****************************************************************************
* 名称：CS5532Sendcmd()
* 功能：发送命令。
* 入口参数：无
* 出口参数：无
* 说明：
****************************************************************************/

void  CS5532SendCmd(uint8_t cmd)
{
    MSPI0_WR_Data(cmd);
}

/****************************************************************************
* 名称：CS5532SendData()
* 功能：发送数据。
* 入口参数：Data　要发送的数据
* 出口参数：无
* 说明：
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
* 名称：CS5532ReadData()
* 功能：接收数据。
* 入口参数：无
* 出口参数：接收的数据
* 说明：
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
* 名称：WriteOffsetReg()
* 功能：写偏移寄存器。
* 入口参数：ChannelsID　　通道号，不能大于1
* 　　　　　Data　　　　　要写入偏移寄存器的值
* 出口参数：无
* 说明：
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
* 名称：ReadOffsetReg()
* 功能：读偏移寄存器。
* 入口参数：ChannelsID　　通道号，不能大于1
* 出口参数：读出的偏移寄存器值
* 说明：
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
* 名称：WriteGainReg()
* 功能：写增益寄存器。
* 入口参数：ChannelsID　　通道号，不能大于1
* 　　　　　Data　　　　　要写入增益寄存器的值
* 出口参数：无
* 说明：
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
* 名称：ReadGainReg()
* 功能：读增益寄存器。
* 入口参数：ChannelsID　　通道号，不能大于1
* 出口参数：读出的增益寄存器值
* 说明：
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
* 名称：WriteChannelSetupReg()
* 功能：写通道设置寄存器。
* 入口参数：RegID　　设置寄存器号，不能大于3
* 　　　　　Data　　　　　要写入通道设置寄存器的值
* 出口参数：无
* 说明：
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
* 名称：ReadChannelSetupReg()
* 功能：读通道设置寄存器。
* 入口参数：RegID　　设置寄存器号，不能大于3
* 出口参数：读出的通道设置寄存器值
* 说明：
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
* 名称：WriteChannelSetupRegEx()
* 功能：写通道设置寄存器扩展。
* 入口参数：SetupID　　设置寄存器号　0～7
* 　　　　　ChannelID　转换通道号　　0～1
* 　　　　　Gain　　　 增益　　　　　0～6   0:1  1:2  2:4  3:8  4:16  5:32  6:64 
* 　　　　　WordRate　 转换速率　　　0～9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 
* 　　　　　UorB　　　 极性模式　　　0～1   0:单极性　1:双极性    
* 　　　　　TempA0A1　 A0A1寄存器值　0～3
* 出口参数：无
* 说明：
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
* 名称：WriteConfigReg()
* 功能：写配置寄存器。
* 入口参数：
* 　　　　　Data　　　　　要写入配置寄存器的值
* 出口参数：无
* 说明：
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
* 名称：WriteConfigRegEx()
* 功能：写配置寄存器扩展。
* 入口参数：TempIs：　正常输入或短接地　0：正常输入　1　短接地
* 　　　　　TempA0A1：A0A1的寄存器值
* 出口参数：返回是否设置成功。
* 说明： 00 02
****************************************************************************/

uint8_t  WriteConfigRegEx(uint8_t TempIs,uint8_t TempA0A1)
{
    #define OLS   0x00      //A1A2设置源　　　0　通道寄存器　1　配置寄存器
	#define VRS   0x00      //电压参考选择
	
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
* 名称：ReadConfigReg()
* 功能：读通道配置寄存器。
* 入口参数：
* 出口参数：读出的配置寄存器值
* 说明：
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
* 名称：ReadSOD0()
* 功能：等待转换完成。
* 入口参数：无
* 出口参数：无
* 说明：
****************************************************************************/
void ReadSOD0(void)
{
   while(MISO_SDO);
}

/****************************************************************************
* 名称：PerformCalibration()
* 功能：根据通道设置寄存器指针指向的寄存器中的参数进行校准。
* 入口参数：CC  　　1:自偏移校准
　　　　　　　　　　2:自增益校准
　　　　　　　　　　5:系统偏移校准
　　　　　　　　　　6:系统增益校准
　　　　　　CSRP　　通道设置寄存器指针　0~7
* 出口参数：是否成功
* 说明：
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
* 名称：ReadADDataReg()
* 功能：读AD转换的数据寄存器。
* 入口参数：MC  　　0:单次转换
　　　　　　　　　　1:连续转换
　　　　　　CSRP　　通道设置寄存器指针　0~7
* 出口参数：读出的数据寄存器值
* 说明：
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
* 名称：ReadADValue()
* 功能：读AD值。
* 入口参数：ChannelsID:要读ＡＤ的通道号
* 出口参数：读出连续的AD数据值
* 说明：
****************************************************************************/
uint32_t ReadADValue(uint8_t ChannelsID)
{
    uint32_t TempData;
    
    TempData = ReadADDataReg(0x00,ChannelsID);
    TempData = TempData >> 8;
    return TempData;
}
/****************************************************************************
* 名称：StartSeriesConversion()
* 功能：开始连续转换。
* 入口参数：ChannelsID:要转换的ＡＤ通道号
* 出口参数：
* 说明：
****************************************************************************/
uint32_t StartSeriesConversion(uint8_t ChannelsID)
{
    uint32_t TempData;
    
    TempData = ReadADDataReg(0x01,ChannelsID);
	
    return TempData;
}

/****************************************************************************
* 名称：StopSeriesConversion()
* 功能：停止连续转换。
* 入口参数：无
* 出口参数：
* 说明：
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
* 名称：ReadSeriesADValue()
* 功能：读连续转换AD值。
* 入口参数：无
* 出口参数：读出的AD数据值
* 说明：
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
* 名称：CS5532InCmdMode()
* 功能：系统初始化，使ＡＤ转换器CS5532芯片进入等侍命令模式
* 入口参数：无
* 出口参数：无
* 说明：
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
* 名称：CS5532Reset()
* 功能：系统复位。
* 入口参数：无
* 出口参数：无
* 说明：
****************************************************************************/

void  CS5532Reset(void)
{
    WriteConfigReg(0x20000000);
    while(!(ReadConfigReg()&0x10000000));
}

/****************************************************************************
* 名称：InitCS5532()
* 功能：初始化AD转换芯片CS5532的0通道。
* 入口参数：ChannelsID　　通道号，不能大于1
* 出口参数：读出的配置寄存器值
* 说明：
****************************************************************************/
void  InitCS5532(void)
{
  uint8_t  AinPort , i;

   /*cs不使能*/
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

	/*使系统进入命令模式*/
    CS5532InCmdMode();
	     
    CS5532Reset(); 

    WriteConfigRegEx(0x00,0x01);

   /*转换速率　0～9   0:120    1:60    2:30    3:15    4:7.5    5:3840    6:1920    7:960    8:480    9:240 */
		
    //WriteChannelSetupRegEx(0,0,Chan0Gain,CollectionFF,1,0);		//setup1:选择的物理通道1,增益4,双极性,字速率60sps
		WriteChannelSetupRegEx(0,0,zWeightSensorParam.WeightSensorAdcGain,zWeightSensorParam.WeightSensorAdcSampleFreq,1,0);
//    WriteChannelSetupRegEx(1,1,6,CollectionFF,0,1);;
//    WriteChannelSetupRegEx(2,1,0,2,0,1);
//		WriteChannelSetupRegEx(3,1,0,2,0,1);
//    WriteChannelSetupRegEx(4,1,0,2,0,1);
//    WriteChannelSetupRegEx(5,1,0,2,0,1);
//    WriteChannelSetupRegEx(6,1,0,2,0,1);
//    WriteChannelSetupRegEx(7,1,0,2,0,1);


  /* 入口参数：CC  　　1:自偏移校准
　　　　　　　　       2:自增益校准
　　　　　　　　　　   5:系统偏移校准
　　　　　　　　　　   6:系统增益校准
　　　　　　  CSRP　　 通道设置寄存器指针　0~7
  */
    AinPort  = 0x00;

    WriteOffsetReg(AinPort,0x00);        //偏移寄存器1写0
    PerformCalibration(0x02,AinPort);    //setup1:自增益校准
    StartSeriesConversion(AinPort);      //setup1:连续模式
}

/**********************************************************************
***CS5532 ADC初始化
**输入参数：IntEnable: 是否中断使能
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

		/* THIS IS FOR TEST ONLY, NEEDS TO REMOVE AFTER PARAMETERS SAVED IN AWS */
		zWeightSensorParam.WeightSensorAdcSampleFreq = ADC_AMPLIFIER_WORDRATE_120SPS; //0:120sps  1:60sps  2:30sps  3:15sps  4:7.5sps  8:3840sps  9:1920sps  10:960sps  11:480sps  12:240sps
		zWeightSensorParam.WeightSensorAdcGain = ADC_AMPLIFIER_GAIN_16X;			//0:X1  1:X2  2:X4  3:X8  4:X16  5:X32  6:X64
		zWeightSensorParam.WeightSensorAdcBitwidth = SpsGainToBitwidthMapping(pwsp->WeightSensorAdcSampleFreq, pwsp->WeightSensorAdcGain);
		zWeightSensorParam.WeightSensorCalibrationFullWeight = 100000;//0.01g????
		zWeightSensorParam.WeightSensorCalibrationZeroAdcValue = 7293;
		zWeightSensorParam.WeightSensorCalibrationFullAdcValue = 11048;
		
		WeightSensorCalibrationKB(&zWeightSensorParam);
		
		//zWeightSensorParam.WeightSensorInitOrNot = WEIGHT_SENSOR_HAD_INITED;
		
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
int32_t WeightSensorReadCurrent(WeightSensorParamaters_t *pwsp)
{
	int i;
	uint32_t temp = 0;
	int32_t temp2 = 0;
	int32_t temp3 = 0;
	static uint8_t sw = 0;
	
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
	
	if(wsckb.k == 0)
		return 0xFFFFFFFF;
	
	if(0 == temp)
	{
		if(1 == sw)
		{
			temp = 10000;
			sw = 0;
		}
		else
		{
			temp = 20000;
			sw = 1;
		}
	}
	
	temp2 = temp - wsckb.b;
	temp3 = temp2 / wsckb.k;
		
	temp2 = temp-pwsp->WeightSensorCalibrationZeroAdcValue;
	temp3 = (temp2*pwsp->WeightSensorCalibrationFullWeight)/(pwsp->WeightSensorCalibrationFullAdcValue-pwsp->WeightSensorCalibrationZeroAdcValue);
	
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

weight_sensor_cmd_t g_weight_sensor_cmd;

// send a comman to weight sensor
int weight_sensor_send_cmd(uint32_t type)
{
  weight_sensor_cmd_t command;

  command.valid = 1;
  command.type = type;
  command.reserved = 0;
  
  taskENTER_CRITICAL();
  g_weight_sensor_cmd = command;
  taskEXIT_CRITICAL();
  
  return g_weight_sensor_cmd.valid;
}

// weight sensor task receive a command
int weight_sensor_recv_cmd(weight_sensor_cmd_t *command)
{
  taskENTER_CRITICAL();
  *command = g_weight_sensor_cmd;
  g_weight_sensor_cmd.valid = 0;
  taskEXIT_CRITICAL();
  
  return command->valid;
}

weight_sensor_filter_t g_weight_sensor_filter;

#define WS_BETA_DEN   4
#define WS_BETA_NUM1  15
#define WS_BETA_NUM2  10
uint32_t weight_sensor_read_and_filtering(weight_sensor_filter_t *wsf)
{
  uint32_t adc_raw;
  
  if(ReadWheChanOk())
	{
		adc_raw = ReadSeriesADValue();

    wsf->adc_filtered[0] = (((wsf->adc_filtered[0] - adc_raw) * wsf->beta_num[0]) >> WS_BETA_DEN) + adc_raw;
    wsf->adc_filtered[1] = (((wsf->adc_filtered[1] - adc_raw) * wsf->beta_num[1]) >> WS_BETA_DEN) + adc_raw;

    IhuDebugPrint("adc_raw=%d adc_filtered=(%d, %d)\n", adc_raw, wsf->adc_filtered[0], wsf->adc_filtered[1]);
    
    if(abs(wsf->adc_filtered[0] - wsf->adc_filtered[1]) < wsf->stable_thresh)
    {
      return 1;
    }
	}

  return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// weight sensor task
// 1) process the L3BF message
// 2) filter the ADC and report the weight value
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void weight_sensor_task(void const *param)
{
	weight_sensor_cmd_t command;
	int is_started = 0;
	uint32_t last_adc_filtered = 0xFFFF, adc_filtered;
  uint32_t last_adc_tick = 0xFFFF, current_tick, repeat_times = 0;
  msg_struct_l3bfsc_weight_ind_t weight_ind;
  OPSTAT ret;
  WeightSensorParamaters_t *wsparm = (WeightSensorParamaters_t *)param;

  //鍒濆鍖朩eight Sensor ADC
	WeightSensorInit(&zWeightSensorParam);
	IhuDebugPrint("L3BFSC: fsm_bfsc_init: WeightSensorInit()\r\n");

  wsckb.k = 0.089660;
  wsckb.b = 7139;
  
  g_weight_sensor_filter.adc_filtered[0] = 0;
  g_weight_sensor_filter.adc_filtered[1] = 0;
  g_weight_sensor_filter.beta_num[0] = WS_BETA_NUM1;
  g_weight_sensor_filter.beta_num[1] = WS_BETA_NUM2;
  g_weight_sensor_filter.stable_thresh = 10;
  g_weight_sensor_filter.change_thresh = 30;
  
	while(1)
	{		
		/* wait for a new command */
    // process command
		if(weight_sensor_recv_cmd(&command))
    {
      if(command.type == WIGHT_SENSOR_CMD_TYPE_STOP)
        is_started = 0;
      else if(command.type == WIGHT_SENSOR_CMD_TYPE_START)
        is_started = 1;
    }

    if(is_started)
    {
      // read sensor for a stable value
      if(weight_sensor_read_and_filtering(&g_weight_sensor_filter))
      {
        adc_filtered = (g_weight_sensor_filter.adc_filtered[0] + g_weight_sensor_filter.adc_filtered[1]) >> 1;

        if(abs(adc_filtered - last_adc_filtered) > g_weight_sensor_filter.change_thresh)
        {
          last_adc_filtered = adc_filtered;
          last_adc_tick = osKernelSysTick();
          repeat_times = 0;
          
          weight_ind.adc_filtered = adc_filtered;
          weight_ind.repeat_times = repeat_times;
          
					IhuDebugPrint("WS: new weight ind: adc_filtered=%d\n", adc_filtered);
					
          // weight changed, send weight indication to L3BFSC
          ret = ihu_message_send(MSG_ID_L3BFSC_WMC_WEIGHT_IND, TASK_ID_BFSC, TASK_ID_BFSC, \
														&weight_ind, sizeof(msg_struct_l3bfsc_weight_ind_t));

          if (ret == IHU_FAILURE){
      			IhuErrorPrint("WS: Send new weight ind message error!\n");
      		}
        }
        else
        {
          current_tick = osKernelSysTick();
          if(current_tick - last_adc_tick > 2000)
          {
            last_adc_tick = current_tick;
            repeat_times ++;
            
            weight_ind.adc_filtered = adc_filtered;
            weight_ind.repeat_times = repeat_times;
            
						IhuDebugPrint("WS: repeat weight ind: adc_filtered=%d repeat_times=%d\n", adc_filtered, repeat_times);
						
            // weight changed, send weight indication to L3BFSC
            ret = ihu_message_send(MSG_ID_L3BFSC_WMC_WEIGHT_IND, TASK_ID_BFSC, TASK_ID_BFSC, \
  														&weight_ind, sizeof(msg_struct_l3bfsc_weight_ind_t));

            if (ret == IHU_FAILURE){
        			IhuErrorPrint("WS: Send repeat weight ind message error!\n");
        		}
          }
        }
      }
    }

    osDelay(1000);
	}
}

