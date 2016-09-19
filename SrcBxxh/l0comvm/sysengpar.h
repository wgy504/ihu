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
	INT32 emcReqTimer;
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
	char  cloudBhServerName[SYS_ENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	char  cloudBhIhuName[SYS_ENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	char  cloudFtpAdd[SYS_ENG_PAR_ELEMENT_CLOUDXHUI_LEN];
	UINT8 cloudBhItfFrameStd;
}SysEngParElementCloudXhui_t;

//Debug采用完全的等级方式，并通过按位操作，详细定义在sysconfig.h中，不应该重复定义，放在这里只是为了参考方便
/*
#define TRACE_DEBUG_ALL_OFF 0 //全关
#define TRACE_DEBUG_INF_ON 1  //全开
#define TRACE_DEBUG_NOR_ON 2  //普通级
#define TRACE_DEBUG_IPT_ON 4  //重要级
#define TRACE_DEBUG_CRT_ON 8  //关键级
#define TRACE_DEBUG_FAT_ON 16 //致命级
*/

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

//工程参数总控制表
#define SYS_ENG_PAR_PRJ_NAME_LEN 20
//EMCWX的符号空间太小，必须限制
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID)
	typedef struct IhuSysEngParTable
	{
		char prjname[SYS_ENG_PAR_PRJ_NAME_LEN];
		//SysEngParElementComm_t comm;
		//SysEngParElementSensorTimer_t timer;
		//SysEngParElementSeriesPort_t serialport;
		//SysEngParElementCloudXhui_t cloud;
		//SysEngParElementIhuSwDownload_t swDownload;
		UINT8 debugMode;
		UINT8 traceMode;
		//SysEngParElementTrace_t traceList;
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
	}IhuSysEngParTable_t;	
#endif	

#endif /* L0COMVM_SYSENGPAREMCWX_H_ */
