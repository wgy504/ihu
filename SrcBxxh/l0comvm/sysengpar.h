/*
 * sysengpar.h
 *
 *  Created on: 2016年1月6日
 *      Author: test
 */

#ifndef L0COMVM_SYSENGPAR_H_
#define L0COMVM_SYSENGPAR_H_

#include "comtype.h"
#include "sysdim.h"

//当前项目
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX "IHU_PRG_EMCWX"
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID 1
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCSPS "IHU_PRG_EMCSPS"
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCSPS_ID 2
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB "IHU_PRG_SCYCB"  //数采仪大板
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID 3
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X "IHU_PRG_EMC68X"  //采用DA14681芯片的微信项目
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X_ID 4
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB "IHU_PRG_PLCCB"  //流水线控制板
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID 5
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB "IHU_PRG_PLCSB"  //流水线传感器板
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID 6
#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX
#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID

//EMWX独特配置项
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCSPS_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID)
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID)
#endif

/*******************************************************************************************
 *                                                                                         *
 *   以下部分的全局定义，来自于HCU代码，还未经过严格的测试验证，未来的修改还需要继续       *
 *                                                                                         *
 *******************************************************************************************/
//基础的通信和硬件部分
typedef struct SysEngParElementComm
{
	UINT8  commBackHawlCon;
}SysEngParElementComm_t;

//后台服务器地址部分
#define SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN 64
#define SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME 12
typedef struct SysEngParElementCloudWx
{
	char  cloudHttpAddLocal[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudHttpAddTest[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudHttpAddSae[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudHttpAddJd[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudHttpAddWechat[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudBhServerName[SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME];
	char  cloudBhHcuName[SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME];
	UINT8 cloudBhItfFrameStd;
}SysEngParElementCloudWx_t;

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
	UINT16 moduleId;
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
typedef struct IhuSysEngParTable
{
	char prjname[SYS_ENG_PAR_PRJ_NAME_LEN];
	//SysEngParElementComm_t comm;
	//SysEngParElementCloudWx_t cloud;
	UINT8 debugMode;
	UINT8 traceMode;
	//SysEngParElementTrace_t traceList;
}IhuSysEngParTable_t;


#endif /* L0COMVM_SYSENGPAR_H_ */
