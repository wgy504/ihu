/*
 * sysengparemcwx.h
 *
 *  Created on: 2016年1月6日
 *      Author: test
 */

#ifndef L0COMVM_SYSENGPAREMCWX_H_
#define L0COMVM_SYSENGPAREMCWX_H_

#include "comtype.h"
#include "sysdim.h"

/*******************************************************************************************
 *                                                                                         *
 *   以下部分的全局定义，来自于IHU代码，还未经过严格的测试验证，未来的修改还需要继续       *
 *                                                                                         *
 *******************************************************************************************/
//基础的通信和硬件部分
typedef struct SysEngParElementComm
{
	UINT8  commBackHawlCon;
}SysEngParElementComm_t;

//Timer setting for sensor
typedef struct SysEngParElementSensor
{
	INT32 vmfoPeriodScanTimer;	
	INT32 adclibraPeriodScanTimer;
	INT32 didocapPeriodScanTimer;
	INT32 ethorionPeriodScanTimer;
	INT32 gpiocancerPeriodScanTimer;
	INT32 canvelaPeriodScanTimer;
	INT32 i2cariesPeriodScanTimer;
	INT32 ledpiscesPeriodScanTimer;
	INT32 ledpiscesGalowagScanTimer;
	INT32 pwmtaurusPeriodScanTimer;
	INT32 spileoPeriodScanTimer;
	INT32 spsvirgoPeriodScanTimer;
	INT32 dcmiarisPeriodScanTimer;	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCSPS_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID)
	INT32 scycbPeriodScanTimer;
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	INT32 cclPeriodScanTimer;	
	INT32 cclEventReportPeriodScanTimer;		
	INT32 cclLockWorkActiveScanTimer;
	INT32 cclLockWorkWaitForOpenScanTimer;
	INT32 cclDidoTriggerPeriodScanTimer;
	INT32 cclDidoWorkingPeriodScanTimer;
	INT32 cclSpsWorkingPeriodScanTimer;
	INT32 cclI2cWorkingPeriodScanTimer;
	INT32 cclDcmiWorkingPeriodScanTimer;
	INT32 cclAdcWorkingPeriodScanTimer;
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	INT32 bfscPeriodScanTimer;
	INT32 bfscAdclibraScanTimer;
	INT32 bfscL3bfscWaitWeightTimer;
	INT32 bfscL3bfscRolloutTimer;
	INT32 bfscL3bfscGiveupTimer;
#else
	#error Un-correct constant definition
#endif
}SysEngParElementSensorTimer_t;

//Series port config
#define SYS_ENG_PAR_ELEMENT_SERIESPORT_LEN 20
typedef struct SysEngParElementSeriesPort
{
	UINT32 SeriesPortForGPS;
}SysEngParElementSeriesPort_t;

//local SW storage address
#define SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN 64
typedef struct SysEngParElementIhuSwDownload
{
	char  ihuSwDownloadDir[SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
	char  ihuSwActiveDir[SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
	char  ihuSwBackupDir[SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN];

}SysEngParElementIhuSwDownload_t;

//后台服务器地址部分
#define SYS_ENG_PAR_ELEMENT_CLOUDXHUI_LEN 64
#define SYS_ENG_PAR_ELEMENT_CLOUDXHUI_NAME 12
typedef struct SysEngParElementCloudXhui
{
	char  cloudHttpAddLocal[SYS_ENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	char  cloudBhServerAddr[SYS_ENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	char  cloudBhServerName[SYS_ENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	char  cloudBhFtpSvrAddr[SYS_ENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	UINT8 cloudBhItfFrameStd;
}SysEngParElementCloudXhui_t;

//TRACE采用黑白名单的方式
//关系表单，采用INDEX索引的方式，引导到相应的配置表中，进行详细定义
typedef struct SysEngParElementTraceModule
{
	UINT8 moduleId;
	char   moduleName[TASK_NAME_MAX_LENGTH];
	UINT8  moduleCtrFlag;
	UINT8  moduleToAllow;
	UINT8  moduleToRestrict;
	UINT8  moduleFromAllow;
	UINT8  moduleFromRestrict;
}SysEngParElementTraceModule_t;
typedef struct SysEngParElementTraceMsg
{
	UINT16 msgId;
	char   msgName[MSG_NAME_MAX_LENGTH];
	UINT8  msgCtrFlag;
	UINT8  msgAllow;
	UINT8  msgRestrict;
}SysEngParElementTraceMsg_t;
typedef struct SysEngParElementTrace
{
	SysEngParElementTraceModule_t mod[MAX_TASK_NUM_IN_ONE_IHU];
	SysEngParElementTraceMsg_t msg[MAX_MSGID_NUM_IN_ONE_TASK];
}SysEngParElementTrace_t;

//通用硬件固定位置的标签存储内容，由工厂生产过程中烧录器写入
//该结构必然跟各个物理器件的定义一致，不然会导致读取失败
typedef struct SysEngParElementHwBurnPhyIdAddr
{
	char  	equLable[20];
	UINT16 	hwType;
	UINT16 	hwPemId;
	UINT16 	swRelId;
	UINT16 	swVerId;
	UINT8  	swUpgradeFlag;
	UINT8 	rsv1;
	UINT8 	rsv2;
	UINT8 	rsv3;
}SysEngParElementHwBurnPhyIdAddr_t;

//工程参数总控制表
#define SYS_ENG_PAR_PRJ_NAME_LEN 20
//EMCWX的符号空间太小，必须限制
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID)
	typedef struct IhuSysEngParTable
	{
		char prjname[SYS_ENG_PAR_PRJ_NAME_LEN];
		UINT8 debugMode;
		UINT8 traceMode;
	}IhuSysEngParTable_t;
#else
	typedef struct IhuSysEngParTable
	{
		char prjname[SYS_ENG_PAR_PRJ_NAME_LEN];
		SysEngParElementComm_t comm;
		SysEngParElementSensorTimer_t timer;
		SysEngParElementSeriesPort_t serialport;
		SysEngParElementCloudXhui_t cloud;
		SysEngParElementIhuSwDownload_t swDownload;
		UINT8 debugMode;
		UINT8 traceMode;
		SysEngParElementTrace_t traceList;
		SysEngParElementHwBurnPhyIdAddr_t hwBurnId;
	}IhuSysEngParTable_t;	
#endif	

#endif /* L0COMVM_SYSENGPAREMCWX_H_ */
