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
typedef struct SysEngParElementTimUnit
{
	UINT8 id;
	char name[IHU_SYSDIM_TIMERID_NAME_LEN_MAX];
	UINT8 gradunarity;
	UINT32 dur;
}SysEngParElementTimUnit_t;
typedef struct SysEngParElementSensor
{
	SysEngParElementTimUnit_t array[IHU_SYSDIM_TIMERID_NBR_MAX];
}SysEngParElementSensorTimer_t;

//Series port config
#define IHU_SYSENG_PAR_ELEMENT_SERIESPORT_LEN 20
typedef struct SysEngParElementSeriesPort
{
	UINT32 SeriesPortForGPS;
}SysEngParElementSeriesPort_t;

//local SW storage address
#define IHU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN 64
typedef struct SysEngParElementIhuSwDownload
{
	char  ihuSwDownloadDir[IHU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
	char  ihuSwActiveDir[IHU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
	char  ihuSwBackupDir[IHU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN];

}SysEngParElementIhuSwDownload_t;

//后台服务器地址部分
#define IHU_SYSENG_PAR_ELEMENT_CLOUDXHUI_LEN 64
#define IHU_SYSENG_PAR_ELEMENT_CLOUDXHUI_NAME 12
typedef struct SysEngParElementCloudXhui
{
	char  cloudHttpAddLocal[IHU_SYSENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	char  cloudBhServerAddr[IHU_SYSENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	char  cloudBhServerName[IHU_SYSENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	char  cloudBhServerPort[IHU_SYSENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	char  cloudBhFtpSvrAddr[IHU_SYSENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	UINT8 cloudBhItfFrameStd;
}SysEngParElementCloudXhui_t;

//TRACE采用黑白名单的方式
//关系表单，采用INDEX索引的方式，引导到相应的配置表中，进行详细定义
typedef struct SysEngParElementTraceModule
{
	UINT8 moduleId;
	char   moduleName[IHU_SYSDIM_TASK_NAME_LEN_MAX];
	UINT8  moduleCtrFlag;
	UINT8  moduleToAllow;
	UINT8  moduleToRestrict;
	UINT8  moduleFromAllow;
	UINT8  moduleFromRestrict;
}SysEngParElementTraceModule_t;
typedef struct SysEngParElementTraceMsg
{
	UINT16 msgId;
	char   msgName[IHU_SYSDIM_MSGID_NAME_LEN_MAX];
	UINT8  msgCtrFlag;
	UINT8  msgAllow;
	UINT8  msgRestrict;
}SysEngParElementTraceMsg_t;
typedef struct SysEngParElementTrace
{
	SysEngParElementTraceModule_t mod[IHU_SYSDIM_TASK_NBR_MAX];
	SysEngParElementTraceMsg_t msg[IHU_SYSDIM_MSGID_NBR_MAX];
}SysEngParElementTrace_t;

//通用硬件固定位置的标签存储内容，由工厂生产过程中烧录器写入
//该结构必然跟各个物理器件的定义一致，不然会导致读取失败
#pragma pack (1) //强制1字节对齐
typedef struct SysEngParElementHwBurnPhyIdAddr
{
	char  	equLable[20];
	UINT16 	hwType;
	UINT16 	hwPemId;
	UINT16 	swRelId;
	UINT16 	swVerId;
	UINT8  	swUpgradeFlag;
	UINT8 	swUpgPollId;
	UINT8 	bootIndex;
	UINT8 	bootAreaMax;   //32
	UINT32  facLoadAddr; 
	UINT16	facLoadSwRel;
	UINT16	facLoadSwVer;	
	UINT16 	facLoadCheckSum;
	UINT16  facLoadValid;
	UINT32	spare2;
	UINT32  bootLoad1Addr;
	UINT16 	bootLoad1RelId;
	UINT16 	bootLoad1VerId;
	UINT16 	bootLoad1CheckSum;
	UINT16  bootLoad1Valid;
	UINT32	spare3;   //32
	UINT32  bootLoad2Addr;
	UINT16 	bootLoad2RelId;
	UINT16 	bootLoad2VerId;
	UINT16 	bootLoad2CheckSum;
	UINT16  bootLoad2Valid;
	UINT32	spare4;
	UINT32  bootLoad3Addr;
	UINT16 	bootLoad3RelId;
	UINT16 	bootLoad3VerId;
	UINT16 	bootLoad3CheckSum;
	UINT16  bootLoad3Valid;
	UINT32  spare5;       //32
	UINT8   cipherKey[16];
	UINT8 	rsv[16];   //32
}SysEngParElementHwBurnPhyIdAddr_t;
#pragma pack () //取消字节对其
#define IHU_SYSENG_HBB_BLOCK_LEN sizeof(SysEngParElementHwBurnPhyIdAddr_t)

//工程参数总控制表
#define SYS_ENG_PAR_PRJ_NAME_LEN 20
//EMCWX的符号空间太小，必须限制，为了编程方便，暂时不区分，未来再说
//#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID)
//	typedef struct IhuSysEngParTab
//	{
//		char prjname[SYS_ENG_PAR_PRJ_NAME_LEN];
//		UINT8 debugMode;
//		UINT8 traceMode;
//	}IhuSysEngParTab_t;
//#else
//#endif	
typedef struct IhuSysEngParTab
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
}IhuSysEngParTab_t;	

#endif /* L0COMVM_SYSENGPAREMCWX_H_ */
