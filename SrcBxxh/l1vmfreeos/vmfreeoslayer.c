/*
 * vmfreeoslayer.c
 *
 *  Created on: 2016年9月18日
 *      Author: test
 */

#include "vmfreeoslayer.h"

/**********************************************************************************
 *
 *   =====VM的使用方式=====
 *   1. 定义全局变量、任务模块、消息结构
 *   2. 完善系统DIMENSION，工程参数定义
 *   3. 调用硬件初始化  ihu_vm_system_init();
 *   4. 调用任务创建 ihu_task_create_all(void);
 *   5. 进入while(1)循环，并sleep状态，每1分钟醒来一次，检查各个任务的状态以及messageque，如果非正常，则重新启动该任务
 *   6. 象征性的清理环境，调用 ihu_task_delete_all_and_queue();
 *   7. 结束
 *
 **********************************************************************************/


/**********************************************************************************
 *
 *   VMLAYER全局变量
 *
 **********************************************************************************/

//全局变量：任务和队列
IhuTaskTag_t zIhuTaskInfo[MAX_TASK_NUM_IN_ONE_IHU];

//全局变量：记录所有任务模块工作差错的次数，以便适当处理
UINT32 zIhuRunErrCnt[MAX_TASK_NUM_IN_ONE_IHU];
IhuGlobalCounter_t zIhuGlobalCounter;  //定义全局计数器COUNTER

//任务状态机FSM全局控制表，占用内存的大户！！！
FsmTable_t zIhuFsmTable;

//全局工程参数表
IhuSysEngParTable_t zIhuSysEngPar; //全局工程参数控制表

//全局时间
time_t zIhuSystemTimeUnix = 1444341556;  //2015/8
struct tm zIhuSystemTimeYmd;

//全局公用打印字符串
IhuPrintBufferChar_t zIhuPrintBufferChar[IHU_PRINT_BUFFER_NUMBER];
//static char strGlobalPrintChar[IHU_PRINT_BUFFER_NUMBER][IHU_PRINT_CHAR_SIZE];
//static char zStrIhuPrintFileLine[IHU_PRINT_BUFFER_NUMBER][IHU_PRINT_FILE_LINE_SIZE];
unsigned int globalPrintIndex = 0;
OS_MUTEX zIhuPrintMutex;

//请确保，该全局字符串的定义跟Task_Id的顺序保持完全一致，不然后面的显示内容会出现差错
//请服从最长长度TASK_NAME_MAX_LENGTH的定义，不然Debug/Trace打印出的信息也会出错
//全局变量：任务打印命名
//从极致优化内存的角度，这里浪费了一个TASK对应的内存空间（MIN=0)，但它却极大的改善了程序编写的效率，值得浪费！！！
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
  char *zIhuTaskNameList[MAX_TASK_NUM_IN_ONE_IHU] ={
    "MIN",
    "VMFO",
    "TIMER",
  //	"ADCLIBRA",
  //	"SPILEO",
  //	"I2CARIES",
  //	"PWMTAURUS",
  //	"SPSVIRGO",
  //	"CANVELA",
  //	"DIDOCAP",
  //	"LEDPISCES",
  //	"ETHORION",
    "EMC68X",
    "MAX"};
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
  char *zIhuTaskNameList[MAX_TASK_NUM_IN_ONE_IHU] ={
    "MIN",
    "VMFO",
    "TIMER",
		"ADCLIBRA",
		//"SPILEO",  //该项目中抑制了该任务模块
		"I2CARIES",
		//"PWMTAURUS",  //该项目中抑制了该任务模块
		"SPSVIRGO",
		//"CANVELA",  //该项目中抑制了该任务模块
		"DIDOCAP",
		"LEDPISCES",
		//"ETHORION", //该项目中抑制了该任务模块
		"DCMIARIS",
    "CCL",
    "MAX"};
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
  char *zIhuTaskNameList[MAX_TASK_NUM_IN_ONE_IHU] ={
    "MIN",
    "VMFO",
    "TIMER",
		"ADCLIBRA",
		"SPILEO",
		"I2CARIES",
		//"PWMTAURUS", //该项目中抑制了该任务模块
		//"SPSVIRGO", //该项目中抑制了该任务模块
		"CANVELA",
		//"DIDOCAP", //该项目中抑制了该任务模块
		"LEDPISCES",
		//"ETHORION", //该项目中抑制了该任务模块
    "BFSC",
    "MAX"};
#else
#endif

//消息ID的定义全局表，方便TRACE函数使用
//请服从MSG_NAME_MAX_LENGTH的最长定义，不然出错
//全局变量：消息打印命名
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
char *zIhuMsgNameList[MAX_MSGID_NUM_IN_ONE_TASK] ={
	"MSG_ID_COM_MIN",
	"MSG_ID_COM_INIT",
	"MSG_ID_COM_INIT_FB",
	"MSG_ID_COM_RESTART",
	"MSG_ID_COM_TIME_OUT",
	"MSG_ID_COM_HEART_BEAT",
	"MSG_ID_COM_HEART_BEAT_FB",
	"MSG_ID_XXX_NULL"
};
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
char *zIhuMsgNameList[MAX_MSGID_NUM_IN_ONE_TASK] ={
	//START FLAG
	"MSG_ID_COM_MIN", //Starting point
	
	//正常的消息ID
	"MSG_ID_COM_INIT",
	"MSG_ID_COM_INIT_FB",
	"MSG_ID_COM_RESTART",  //L1->重新初始化上下文数据
	"MSG_ID_COM_TIME_OUT",
	"MSG_ID_COM_STOP",
	"MSG_ID_COM_HEART_BEAT",
	"MSG_ID_COM_HEART_BEAT_FB",
	
	//VMFO
	"MSG_ID_VMFO_TIMER_1S_PERIOD",
	
	//ADC
	
	//LED

	//DIDO
	"MSG_ID_DIDO_CCL_SENSOR_STATUS_RESP",
	"MSG_ID_DIDO_CCL_EVENT_LOCK_TRIGGER",
	"MSG_ID_DIDO_CCL_EVENT_FAULT_TRIGGER",
	"MSG_ID_DIDO_CCL_EVENT_STATUS_UPDATE",
	"MSG_ID_DIDO_CCL_DOOR_OPEN_EVENT",
	"MSG_ID_DIDO_CCL_LOCK_C_DOOR_C_EVENT",

	//SPS
	"MSG_ID_SPS_L2FRAME_SEND",
	"MSG_ID_SPS_L2FRAME_RCV",
	"MSG_ID_SPS_CCL_CLOUD_FB",
	"MSG_ID_SPS_CCL_SENSOR_STATUS_RESP",
	"MSG_ID_SPS_CCL_EVENT_REPORT_CFM",
	"MSG_ID_SPS_CCL_FAULT_REPORT_CFM",
	"MSG_ID_SPS_CCL_CLOSE_REPORT_CFM",	
	
	//SPI
	"MSG_ID_SPI_L2FRAME_SEND",
	"MSG_ID_SPI_L2FRAME_RCV",
		
	//I2C
	"MSG_ID_I2C_L2FRAME_SEND",
	"MSG_ID_I2C_L2FRAME_RCV",
	"MSG_ID_I2C_CCL_SENSOR_STATUS_RESP",
	
	//CAN
	"MSG_ID_CAN_L2FRAME_SEND",
	"MSG_ID_CAN_L2FRAME_RCV",

	//DCMI
	"MSG_ID_DCMI_CCL_SENSOR_STATUS_RESP",
	
	//CCL
	"MSG_ID_CCL_SPS_OPEN_AUTH_INQ",  	   //后台查询
	"MSG_ID_CCL_COM_SENSOR_STATUS_REQ",
	"MSG_ID_CCL_COM_CTRL_CMD",
	"MSG_ID_CCL_SPS_EVENT_REPORT_SEND",  //周期汇报
	"MSG_ID_CCL_SPS_FAULT_REPORT_SEND",  //故障汇报
	"MSG_ID_CCL_SPS_CLOSE_REPORT_SEND",  //正常一次开关报告

	//END FLAG
	"MSG_ID_COM_MAX" //Ending point
};
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
char *zIhuMsgNameList[MAX_MSGID_NUM_IN_ONE_TASK] ={
	//START FLAG
	"MSG_ID_COM_MIN", //Starting point
	
	//正常的消息ID
	"MSG_ID_COM_INIT",
	"MSG_ID_COM_INIT_FB",
	"MSG_ID_COM_RESTART",  //L1->重新初始化上下文数据
	"MSG_ID_COM_TIME_OUT",
	"MSG_ID_COM_STOP",
	"MSG_ID_COM_HEART_BEAT",
	"MSG_ID_COM_HEART_BEAT_FB",	

	//VMFO
	"MSG_ID_VMFO_TIMER_1S_PERIOD",
	
	//ADC
	"MSG_ID_ADC_NEW_MATERIAL_WS",
	"MSG_ID_ADC_MATERIAL_DROP",
	"MSG_ID_ADC_L3BFSC_MEAS_CMD_RESP",
	
	//LED
	
	//UART
	"MSG_ID_SPS_L2FRAME_SEND",
	"MSG_ID_SPS_L2FRAME_RCV",
	
	//SPI
	"MSG_ID_SPI_L2FRAME_SEND",
	"MSG_ID_SPI_L2FRAME_RCV",
		
	//I2C
	"MSG_ID_I2C_L2FRAME_SEND",
	"MSG_ID_I2C_L2FRAME_RCV",
	"MSG_ID_I2C_L3BFSC_MOTO_CMD_RESP",
	
	//CAN
	"MSG_ID_CAN_L2FRAME_SEND",
	"MSG_ID_CAN_L2FRAME_RCV",
	"MSG_ID_CAN_L3BFSC_CMD_CTRL",
	"MSG_ID_CAN_L3BFSC_INIT_REQ",
	"MSG_ID_CAN_L3BFSC_ROLL_OUT_REQ",
	"MSG_ID_CAN_L3BFSC_GIVE_UP_REQ",
	"MSG_ID_CAN_ADC_WS_MAN_SET_ZERO",

	//L3BFSC
	"MSG_ID_L3BFSC_CAN_INIT_RESP",
	"MSG_ID_L3BFSC_CAN_NEW_WS_EVENT",
	"MSG_ID_L3BFSC_CAN_ROLL_OUT_RESP",
	"MSG_ID_L3BFSC_CAN_GIVE_UP_RESP",
	"MSG_ID_L3BFSC_CAN_ERROR_STATUS_REPORT",
	"MSG_ID_L3BFSC_ADC_WS_CMD_CTRL",
	"MSG_ID_L3BFSC_I2C_MOTO_CMD_CTRL",
	"MSG_ID_L3BFSC_CAN_CMD_RESP",
	
	//END FLAG
	"MSG_ID_COM_MAX"
};
#else
	#error Un-correct constant definition
#endif

/*******************************************************************************
**
**	全局公用的API函数，不依赖于任何操作系统，只依赖于POSIX支持的标准函数集
**
**  已经使用了线程安全的方法，采用的是MUTEX互锁机制，确保不同打印之间调用的不相关性和单入性
**
**********************************************************************************/
//正常打印
void IhuDebugPrintFo(UINT8 index, char *format, ...)
{
	va_list marker;
	char strDebug[IHU_PRINT_CHAR_SIZE];
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	char *ptrPrintBuffer;
#endif
	//UINT8 index=0;

	//index = globalPrintIndex;
	memset(zIhuPrintBufferChar[index].PrintBuffer, 0, IHU_PRINT_CHAR_SIZE);
	
	va_start(marker, format );
	vsnprintf(strDebug, IHU_PRINT_CHAR_SIZE-1, format, marker);
	va_end(marker);	

	//试图将实时时间打印出来，可惜没成功
////在CCL项目中，已经打开了RTC，所以可以调用RTC函数输出真实的时间，其它项目可以类似干，如果没有打开，则依然可以继续使用编译时间
//#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
//	char ss[20];
//	sprintf(zIhuPrintBufferChar[index].PrintBuffer, "%s, [DBG: ", zIhuPrintBufferChar[index].PrintHeader);
//	memset(ss, 0, sizeof(ss));
//	ihu_l1hd_rtc_get_current_ymdhms_time(ss);
//	strcat(zIhuPrintBufferChar[index].PrintBuffer, ss);
//	strcat(zIhuPrintBufferChar[index].PrintBuffer, "] ");
//#endif
	sprintf(zIhuPrintBufferChar[index].PrintBuffer, "%s, [DBG: %s, %s] ", zIhuPrintBufferChar[index].PrintHeader, __DATE__, __TIME__);
	strncat(zIhuPrintBufferChar[index].PrintBuffer, strDebug, IHU_PRINT_CHAR_SIZE - strlen(zIhuPrintBufferChar[index].PrintBuffer) - 1);

	// The trace is limited to 128 characters as defined at SYSDIM.H
	if( (zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-2] != 0) && (zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-1] != 0) )
	{
		zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-3] = '!';
		zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-2] = '\n';
		zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-1] = '\0';
	}
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	printf("%s", ptrPrintBuffer);
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	/* 互斥信号量 */
	if (OS_MUTEX_GET(zIhuPrintMutex, IHU_PRINT_MUTEX_TIME_OUT_DURATION) != OS_MUTEX_TAKEN){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		return;
	}
	ihu_l1hd_sps_print_send_data(zIhuPrintBufferChar[index].PrintBuffer);
	OS_MUTEX_PUT(zIhuPrintMutex);
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	if (OS_MUTEX_GET(zIhuPrintMutex, IHU_PRINT_MUTEX_TIME_OUT_DURATION) != OS_MUTEX_TAKEN){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		return;
	}
	ihu_l1hd_sps_print_send_data(zIhuPrintBufferChar[index].PrintBuffer);
	OS_MUTEX_PUT(zIhuPrintMutex);	
#else
	#error Un-correct constant definition
#endif
	
	zIhuPrintBufferChar[index].PrintBuffer[0] = '\0';
}

//错误打印
void IhuErrorPrintFo(UINT8 index, char *format, ...)
{
	va_list marker;
	char strDebug[IHU_PRINT_CHAR_SIZE];
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
  char *ptrPrintBuffer;
#endif
	//UINT8 index=0;

	va_start(marker, format );
	vsnprintf(strDebug, IHU_PRINT_CHAR_SIZE-1, format, marker);
	va_end(marker);	
	
	//index = globalPrintIndex;
	memset(zIhuPrintBufferChar[index].PrintBuffer, 0, IHU_PRINT_CHAR_SIZE);

	//试图将实时时间打印出来，可惜没成功
//#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
//	char ss[20];
//	sprintf(zIhuPrintBufferChar[index].PrintBuffer, "%s, [ERR: ", zIhuPrintBufferChar[index].PrintHeader);
//	memset(ss, 0, sizeof(ss));
//	ihu_l1hd_rtc_get_current_ymdhms_time(ss);
//	strcat(zIhuPrintBufferChar[index].PrintBuffer, ss);
//	strcat(zIhuPrintBufferChar[index].PrintBuffer, "] ");
//#endif		
	sprintf(zIhuPrintBufferChar[index].PrintBuffer, "%s, [ERR: %s, %s] ", zIhuPrintBufferChar[index].PrintHeader, __DATE__, __TIME__);
	strncat(zIhuPrintBufferChar[index].PrintBuffer, strDebug, IHU_PRINT_CHAR_SIZE - strlen(zIhuPrintBufferChar[index].PrintBuffer) - 1);

	// The trace is limited to 128 characters as defined at SYSDIM.H
	if( (zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-2] != 0) && (zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-1] != 0) )
	{
		zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-3] = '!';
		zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-2] = '\n';
		zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-1] = '\0';
	}
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	printf("%s", ptrPrintBuffer);
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)	
	if (OS_MUTEX_GET(zIhuPrintMutex, IHU_PRINT_MUTEX_TIME_OUT_DURATION) != OS_MUTEX_TAKEN){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		return;
	}
	ihu_l1hd_sps_print_send_data(zIhuPrintBufferChar[index].PrintBuffer);
	OS_MUTEX_PUT(zIhuPrintMutex);
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)	
	if (OS_MUTEX_GET(zIhuPrintMutex, IHU_PRINT_MUTEX_TIME_OUT_DURATION) != OS_MUTEX_TAKEN){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		return;
	}
	ihu_l1hd_sps_print_send_data(zIhuPrintBufferChar[index].PrintBuffer);
	OS_MUTEX_PUT(zIhuPrintMutex);		
#else
	#error Un-correct constant definition
#endif
	
	zIhuPrintBufferChar[index].PrintBuffer[0] = '\0';
}

//独特的ID打印
UINT8 IhuDebugPrintId(char *file, int line)
{
	char strLine[9];
	UINT8 index=0;
//	char *ptrPrintBuffer;
//	
//	ptrPrintBuffer = strGlobalPrintChar[globalPrintIndex++];
//	if (globalPrintIndex >= IHU_PRINT_BUFFER_NUMBER)
//		globalPrintIndex = 0;
//	memset(ptrPrintBuffer, 0, IHU_PRINT_CHAR_SIZE);	
//	snprintf(ptrPrintBuffer, 50, "[%s]", file);
//	sprintf(strLine, "[%6d]", line);
//	strcat(ptrPrintBuffer, strLine);

//#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
//	printf("%s", strGlobalPrintChar);
//#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)		
//	if (OS_MUTEX_GET(zIhuPrintMutex, IHU_PRINT_MUTEX_TIME_OUT_DURATION) != OS_MUTEX_TAKEN){
//		zIhuRunErrCnt[TASK_ID_VMFO]++;
//		return;
//	}
//	BSP_STM32_sps_print_data_send(ptrPrintBuffer);
//	OS_MUTEX_PUT(zIhuPrintMutex);	
//#else
//#endif
	globalPrintIndex++;
	if (globalPrintIndex >= IHU_PRINT_BUFFER_NUMBER) globalPrintIndex = 0;
	index = globalPrintIndex;
	memset(zIhuPrintBufferChar[index].PrintHeader, 0, IHU_PRINT_FILE_LINE_SIZE);
	
//	snprintf(zIhuPrintBufferChar[index].PrintHeader, 49, "[%s", file);
//	if (strlen(zIhuPrintBufferChar[index].PrintHeader)<49){
//		for (i=0; i<49-strlen(zIhuPrintBufferChar[index].PrintHeader); i++) strcat(zIhuPrintBufferChar[index].PrintHeader, " ");
//	}
//	strcat(zIhuPrintBufferChar[index].PrintHeader, "]");

	snprintf(zIhuPrintBufferChar[index].PrintHeader, 50, "[%s]", file);
	sprintf(strLine, "[%6d]", line);
	strcat(zIhuPrintBufferChar[index].PrintHeader, strLine);
	return index;
}

void IhuDebugPrintFoEmc68x(char *format, ...)
{
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
        va_list marker;
        char strDebug[IHU_PRINT_CHAR_SIZE];

        char *ptrPrintBuffer;
        UINT8 index=0;

        //index = globalPrintIndex;
        memset(zIhuPrintBufferChar[index].PrintBuffer, 0, IHU_PRINT_CHAR_SIZE);

        va_start(marker, format );
        vsnprintf(strDebug, IHU_PRINT_CHAR_SIZE-1, format, marker);
        va_end(marker);

        sprintf(zIhuPrintBufferChar[index].PrintBuffer, "%s, [DBG: %s, %s] ", zIhuPrintBufferChar[index].PrintHeader, __DATE__, __TIME__);
        strncat(zIhuPrintBufferChar[index].PrintBuffer, strDebug, IHU_PRINT_CHAR_SIZE - strlen(zIhuPrintBufferChar[index].PrintBuffer) - 1);

        // The trace is limited to 128 characters as defined at SYSDIM.H
        if( (zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-2] != 0) && (zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-1] != 0) )
        {
                zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-3] = '!';
                zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-2] = '\n';
                zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-1] = '\0';
        }

        printf("%s", ptrPrintBuffer);
        zIhuPrintBufferChar[index].PrintBuffer[0] = '\0';
#endif

}

//错误打印
void IhuErrorPrintFoEmc68x(char *format, ...)
{
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
        va_list marker;
        char strDebug[IHU_PRINT_CHAR_SIZE];
        char *ptrPrintBuffer;
        UINT8 index=0;

        va_start(marker, format );
        vsnprintf(strDebug, IHU_PRINT_CHAR_SIZE-1, format, marker);
        va_end(marker);

        //index = globalPrintIndex;
        memset(zIhuPrintBufferChar[index].PrintBuffer, 0, IHU_PRINT_CHAR_SIZE);

        sprintf(zIhuPrintBufferChar[index].PrintBuffer, "%s, [ERR: %s, %s] ", zIhuPrintBufferChar[index].PrintHeader, __DATE__, __TIME__);
        strncat(zIhuPrintBufferChar[index].PrintBuffer, strDebug, IHU_PRINT_CHAR_SIZE - strlen(zIhuPrintBufferChar[index].PrintBuffer) - 1);

        // The trace is limited to 128 characters as defined at SYSDIM.H
        if( (zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-2] != 0) && (zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-1] != 0) )
        {
                zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-3] = '!';
                zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-2] = '\n';
                zIhuPrintBufferChar[index].PrintBuffer[IHU_PRINT_CHAR_SIZE-1] = '\0';
        }

        printf("%s", ptrPrintBuffer);
        zIhuPrintBufferChar[index].PrintBuffer[0] = '\0';
#endif

}



//交换U16的高位低位字节
uint16_t b2l_uint16(uint16_t in)
{
		return ( ((in & 0x00FF) << 8) | (in >> 8) );
}

//API abstract
//这里采用系统提供的时钟函数
void ihu_sleep(UINT32 second)
{
	if (second <= (UINT32)0) second =0;
	if (second >= MAX_SLEEP_COUNTER_UP_LIMITATION) second = MAX_SLEEP_COUNTER_UP_LIMITATION;
	second = second*1000;
	OS_DELAY_MS(second);
}

void ihu_usleep(UINT32 usecond)
{
	if (usecond <= 0) usecond =0;
	if (usecond >= MAX_SLEEP_COUNTER_UP_LIMITATION) usecond = MAX_SLEEP_COUNTER_UP_LIMITATION;
	OS_DELAY_MS(usecond);
}

//INIT the whole system
void ihu_vm_system_init(void)
{	
	int i=0;
	
	//初始化打印缓冲区
	globalPrintIndex = 0;
	memset(&(zIhuPrintBufferChar[0].localIndex), 0, sizeof(IhuPrintBufferChar_t));
	//采用OS_MUTEX_CREATE居然不成功，怪哉。。。
	//OS_MUTEX_CREATE(zIhuPrintMutex);
	zIhuPrintMutex = xSemaphoreCreateRecursiveMutex();
	if (zIhuPrintMutex == NULL){
		IhuDebugPrint("VMFO: Initialize VMFO failure, not continue any more...!\n");
		return;
	}

	//INIT IHU itself
	IhuDebugPrint("VMFO: Compiled HW_ID Info: CURRENT_PRJ=[%s], PRODUCT_CAT=[0x%x], HW_TYPE=[%d], SW_REL=[%d], SW_DELIVER=[%d].\n", IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT, \
		IHU_HARDWARE_PRODUCT_CAT_TYPE, IHU_CURRENT_HW_TYPE, IHU_CURRENT_SW_RELEASE, IHU_CURRENT_SW_DELIVERY);
	IhuDebugPrint("VMFO: BXXH(TM) IHU(c) Application Layer start and initialized, build at %s, %s.\n", __DATE__, __TIME__);

	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)	
	//初始化全局变量TASK_ID/QUE_ID/TASK_STAT
	memset(&(zIhuTaskInfo[0].TaskId), 0, sizeof(zIhuTaskInfo)*(TASK_ID_MAX-TASK_ID_MIN+1));

	for (i=TASK_ID_MIN; i<TASK_ID_MAX; i++){
		zIhuTaskInfo[i].TaskId = i;
		strcpy(zIhuTaskInfo[i].TaskName, zIhuTaskNameList[i]);
	}
	memset(zIhuRunErrCnt, 0, sizeof(UINT32)*(TASK_ID_MAX-TASK_ID_MIN+1));
	
	//Init Fsm
	FsmInit();
	
	//初始化全局工参配置信息，这里给出了大部分用到的参数的初始化结构，以便未来可以更加方便的添加完善
	//后台通信部分
	zIhuSysEngPar.comm.commBackHawlCon = IHU_COMM_BACK_HAWL_CON;

	//Sensor timer
	//zIhuSysEngPar.timer.emcReqTimer = IHU_EMC_TIMER_DURATION_PERIOD_READ;

	//Series Port config
	zIhuSysEngPar.serialport.SeriesPortForGPS = IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT;

	//后台部分
	strcpy(zIhuSysEngPar.cloud.cloudHttpAddLocal, IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL);

	//for IHU SW FTP

	//local SW storage address
	strcpy(zIhuSysEngPar.swDownload.ihuSwDownloadDir, IHU_SW_DOWNLOAD_DIR_DEFAULT);
	strcpy(zIhuSysEngPar.swDownload.ihuSwActiveDir, IHU_SW_ACTIVE_DIR_DEFAULT);
	strcpy(zIhuSysEngPar.swDownload.ihuSwBackupDir, IHU_SW_BACKUP_DIR_DEFAULT);

	//DEBUG部分
	zIhuSysEngPar.debugMode = IHU_TRACE_DEBUG_ON;
	
	//TRACE部分
	zIhuSysEngPar.traceMode = IHU_TRACE_MSG_ON;
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE){
		IhuDebugPrint("VMFO: Set basic engineering data correctly from SYSTEM DEFAULT parameters!\n");
	}
	
	//后台协议接口
	zIhuSysEngPar.cloud.cloudBhItfFrameStd = IHU_CLOUDXHUI_BH_INTERFACE_STANDARD;
	
	//打开所有模块和消息的TRACE属性
	memset(&(zIhuSysEngPar.traceList), 0, sizeof(SysEngParElementTrace_t));
	for (i=TASK_ID_MIN; i<TASK_ID_MAX; i++){
		zIhuSysEngPar.traceList.mod[i].moduleId = i;
		zIhuSysEngPar.traceList.mod[i].moduleFromAllow = TRUE;
		zIhuSysEngPar.traceList.mod[i].moduleToAllow = TRUE;		
	}
	for (i=MSG_ID_COM_MIN; i<MSG_ID_COM_MAX; i++){
		zIhuSysEngPar.traceList.msg[i].msgId = i;
		zIhuSysEngPar.traceList.msg[i].msgAllow = TRUE;
	}

	//处理启动模块
	for (i=0; i<MAX_TASK_NUM_IN_ONE_IHU; i++){
		zIhuTaskInfo[i].pnpState = IHU_TASK_PNP_INVALID;
	}
	//肯定需要启动的任务模块
	zIhuTaskInfo[TASK_ID_VMFO].pnpState = IHU_TASK_PNP_ON;
	zIhuTaskInfo[TASK_ID_TIMER].pnpState = IHU_TASK_PNP_ON;
//	if (IHU_COMM_FRONT_ADC == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_ADCLIBRA].pnpState = IHU_TASK_PNP_ON;
//	if (IHU_COMM_FRONT_SPI == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_SPILEO].pnpState = IHU_TASK_PNP_ON;
//	if (IHU_COMM_FRONT_I2C == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_I2CARIES].pnpState = IHU_TASK_PNP_ON;
//	if (IHU_COMM_FRONT_PWM == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_PWMTAURUS].pnpState = IHU_TASK_PNP_ON;
//	if (IHU_COMM_FRONT_SPS == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_SPSVIRGO].pnpState = IHU_TASK_PNP_ON;
//	if (IHU_COMM_FRONT_GPIO == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_CANVELA].pnpState = IHU_TASK_PNP_ON;
//	if (IHU_COMM_FRONT_DIDO == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_DIDOCAP].pnpState = IHU_TASK_PNP_ON;
//	if (IHU_COMM_FRONT_LED == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_LEDPISCES].pnpState = IHU_TASK_PNP_ON;
//	if (IHU_COMM_FRONT_ETH == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_ETHORION].pnpState = IHU_TASK_PNP_ON;
	if (IHU_MAIN_CTRL_EMC68X == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_EMC68X].pnpState = IHU_TASK_PNP_ON;

	
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	//初始化全局变量TASK_ID/QUE_ID/TASK_STAT
	memset(&(zIhuTaskInfo[0].TaskId), 0, sizeof(zIhuTaskInfo)*(TASK_ID_MAX-TASK_ID_MIN+1));

	for (i=TASK_ID_MIN; i<TASK_ID_MAX; i++){
		zIhuTaskInfo[i].TaskId = i;
		strcpy(zIhuTaskInfo[i].TaskName, zIhuTaskNameList[i]);
	}
	memset(zIhuRunErrCnt, 0, sizeof(UINT32)*(TASK_ID_MAX-TASK_ID_MIN+1));

	//Init Fsm
	FsmInit();
	
	//初始化全局工参配置信息，这里给出了大部分用到的参数的初始化结构，以便未来可以更加方便的添加完善
	//后台通信部分
	zIhuSysEngPar.comm.commBackHawlCon = IHU_COMM_BACK_HAWL_CON;

	//Sensor timer
	zIhuSysEngPar.timer.vmfoPeriodScanTimer = IHU_VMFO_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.adclibraPeriodScanTimer = IHU_ADCLIBRA_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.didocapPeriodScanTimer = IHU_DIDOCAP_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.ethorionPeriodScanTimer = IHU_ETHORION_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.canvelaPeriodScanTimer = IHU_CANVELA_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.i2cariesPeriodScanTimer = IHU_I2CARIERS_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.ledpiscesPeriodScanTimer = IHU_LEDPISCES_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.ledpiscesGalowagScanTimer = IHU_LEDPISCES_TIMER_GALOWAG_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.pwmtaurusPeriodScanTimer = IHU_PWMTAURUS_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.spileoPeriodScanTimer = IHU_SPILEO_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.spsvirgoPeriodScanTimer = IHU_SPSVIRGO_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.dcmiarisPeriodScanTimer = IHU_DCMIARIS_TIMER_DURATION_PERIOD_SCAN;	
	zIhuSysEngPar.timer.cclPeriodScanTimer = IHU_CCL_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.cclEventReportPeriodScanTimer = IHU_CCL_TIMER_DURATION_EVENT_REPORT_PEROID_SCAN;
	zIhuSysEngPar.timer.cclLockWorkActiveScanTimer = IHU_CCL_TIMER_DURATION_LOCK_ACTIVE;
	zIhuSysEngPar.timer.cclLockWorkWaitForOpenScanTimer = IHU_CCL_TIMER_DURATION_LOCK_WORK_WAIT_TO_OPEN;	
	zIhuSysEngPar.timer.cclDidoTriggerPeriodScanTimer = IHU_CCL_TIMER_DURATION_DIDO_TRIGGER_PERIOD_SCAN;
	zIhuSysEngPar.timer.cclDidoWorkingPeriodScanTimer = IHU_CCL_TIMER_DURATION_DIDO_WORKING_PERIOD_SCAN;
	zIhuSysEngPar.timer.cclSpsWorkingPeriodScanTimer = IHU_CCL_TIMER_DURATION_SPS_WORKING_PERIOD_SCAN;
	zIhuSysEngPar.timer.cclI2cWorkingPeriodScanTimer = IHU_CCL_TIMER_DURATION_I2C_WORKING_PERIOD_SCAN;
	zIhuSysEngPar.timer.cclDcmiWorkingPeriodScanTimer = IHU_CCL_TIMER_DURATION_DCMI_WORKING_PERIOD_SCAN;
	zIhuSysEngPar.timer.cclAdcWorkingPeriodScanTimer = IHU_CCL_TIMER_DURATION_ADC_WORKING_PERIOD_SCAN;

	//Series Port config
	zIhuSysEngPar.serialport.SeriesPortForGPS = IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT;

	//后台部分
	strcpy(zIhuSysEngPar.cloud.cloudHttpAddLocal, IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL);
	strcpy(zIhuSysEngPar.cloud.cloudBhServerAddr, IHU_CLOUDXHUI_HTTP_ADDRESS_BH_SERVER_ADDR);
	strcpy(zIhuSysEngPar.cloud.cloudBhServerName, IHU_CLOUDXHUI_HTTP_ADDRESS_BH_SERVER_NAME);
	strcpy(zIhuSysEngPar.cloud.cloudBhFtpSvrAddr, IHU_CLOUDXHUI_FTP_BH_SERVER_ADDR);	
	strcpy(zIhuSysEngPar.cloud.cloudBhIhuName, IHU_CLOUDXHUI_HCU_NAME_SELF);
	
	//local SW storage address
	strcpy(zIhuSysEngPar.swDownload.ihuSwDownloadDir, IHU_SW_DOWNLOAD_DIR_DEFAULT);
	strcpy(zIhuSysEngPar.swDownload.ihuSwActiveDir, IHU_SW_ACTIVE_DIR_DEFAULT);
	strcpy(zIhuSysEngPar.swDownload.ihuSwBackupDir, IHU_SW_BACKUP_DIR_DEFAULT);

	//DEBUG部分
	zIhuSysEngPar.debugMode = IHU_TRACE_DEBUG_ON;
	
	//TRACE部分
	zIhuSysEngPar.traceMode = IHU_TRACE_MSG_ON;
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE){
		IhuDebugPrint("VMFO: Set basic engineering data correctly from SYSTEM DEFAULT parameters!\n");
	}
	
	//后台协议接口
	zIhuSysEngPar.cloud.cloudBhItfFrameStd = IHU_CLOUDXHUI_BH_INTERFACE_STANDARD;
	
	//打开所有模块和消息的TRACE属性
	memset(&(zIhuSysEngPar.traceList), 0, sizeof(SysEngParElementTrace_t));
	for (i=TASK_ID_MIN; i<TASK_ID_MAX; i++){
		zIhuSysEngPar.traceList.mod[i].moduleId = i;
		zIhuSysEngPar.traceList.mod[i].moduleFromAllow = TRUE;
		zIhuSysEngPar.traceList.mod[i].moduleToAllow = TRUE;		
	}
	for (i=MSG_ID_COM_MIN; i<MSG_ID_COM_MAX; i++){
		zIhuSysEngPar.traceList.msg[i].msgId = i;
		zIhuSysEngPar.traceList.msg[i].msgAllow = TRUE;
	}
	
	//硬件烧录区域，系统唯一标识部分，后面程序中访问到这些系统参数都必须从这个地方读取
	ihu_l1hd_f2board_equid_get(&(zIhuSysEngPar.hwBurnId));
	//由于硬件部分并没有真正起作用，所以暂时需要从系统定义区重复写入，一旦批量生产这部分可以去掉
	if (IHU_HARDWARE_MASSIVE_PRODUTION_SET == IHU_HARDWARE_MASSIVE_PRODUTION_NO){
		strncpy(zIhuSysEngPar.hwBurnId.equLable, IHU_CLOUDXHUI_HCU_NAME_SELF, (sizeof(IHU_CLOUDXHUI_HCU_NAME_SELF)<sizeof(zIhuSysEngPar.hwBurnId.equLable))?(sizeof(IHU_CLOUDXHUI_HCU_NAME_SELF)):(sizeof(zIhuSysEngPar.hwBurnId.equLable)));
		zIhuSysEngPar.hwBurnId.hwType = IHU_HARDWARE_PRODUCT_CAT_TYPE;
		zIhuSysEngPar.hwBurnId.hwPemId = IHU_CURRENT_HW_TYPE; //PEM小型号
		zIhuSysEngPar.hwBurnId.swRelId = IHU_CURRENT_SW_RELEASE;
		zIhuSysEngPar.hwBurnId.swVerId = IHU_CURRENT_SW_DELIVERY;
		zIhuSysEngPar.hwBurnId.swUpgradeFlag = IHU_HARDWARE_BURN_ID_FW_UPGRADE_SET;
	}

	//处理启动模块
	for (i=0; i<MAX_TASK_NUM_IN_ONE_IHU; i++){
		zIhuTaskInfo[i].pnpState = IHU_TASK_PNP_INVALID;
	}
	//肯定需要启动的任务模块
	zIhuTaskInfo[TASK_ID_VMFO].pnpState = IHU_TASK_PNP_ON;
	zIhuTaskInfo[TASK_ID_TIMER].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_ADC == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_ADCLIBRA].pnpState = IHU_TASK_PNP_ON;
	//if (IHU_COMM_FRONT_SPI == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_SPILEO].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_I2C == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_I2CARIES].pnpState = IHU_TASK_PNP_ON;
	//if (IHU_COMM_FRONT_PWM == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_PWMTAURUS].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_SPS == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_SPSVIRGO].pnpState = IHU_TASK_PNP_ON;
	//if (IHU_COMM_FRONT_GPIO == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_CANVELA].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_DIDO == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_DIDOCAP].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_LED == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_LEDPISCES].pnpState = IHU_TASK_PNP_ON;
	//if (IHU_COMM_FRONT_ETH == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_ETHORION].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_DCMI == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_DCMIARIS].pnpState = IHU_TASK_PNP_ON;
	if (IHU_MAIN_CTRL_CCL == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_CCL].pnpState = IHU_TASK_PNP_ON;			
	
	//初始化之后的系统标识信息
	IhuDebugPrint("VMFO: Initialized Hardware Burn Physical Id/Address: CURRENT_PRJ=[%s], HW_LABLE=[%s], PRODUCT_CAT=[0x%x], HW_TYPE=[0x%x], SW_RELEASE_VER=[%d.%d], FW_UPGRADE_FLAG=[%d].\n", \
		IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT, \
		zIhuSysEngPar.hwBurnId.equLable, \
		zIhuSysEngPar.hwBurnId.hwType, \
		zIhuSysEngPar.hwBurnId.hwPemId, \
		zIhuSysEngPar.hwBurnId.swRelId, \
		zIhuSysEngPar.hwBurnId.swVerId, \
		zIhuSysEngPar.hwBurnId.swUpgradeFlag);
	
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	//初始化全局变量TASK_ID/QUE_ID/TASK_STAT
	memset(&(zIhuTaskInfo[0].TaskId), 0, sizeof(zIhuTaskInfo)*(TASK_ID_MAX-TASK_ID_MIN+1));

	for (i=TASK_ID_MIN; i<TASK_ID_MAX; i++){
		zIhuTaskInfo[i].TaskId = i;
		strcpy(zIhuTaskInfo[i].TaskName, zIhuTaskNameList[i]);
	}
	memset(zIhuRunErrCnt, 0, sizeof(UINT32)*(TASK_ID_MAX-TASK_ID_MIN+1));

	//Init Fsm
	FsmInit();
	
	//初始化全局工参配置信息，这里给出了大部分用到的参数的初始化结构，以便未来可以更加方便的添加完善
	//后台通信部分
	zIhuSysEngPar.comm.commBackHawlCon = IHU_COMM_BACK_HAWL_CON;

	//Sensor timer
	zIhuSysEngPar.timer.vmfoPeriodScanTimer = IHU_VMFO_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.adclibraPeriodScanTimer = IHU_ADCLIBRA_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.didocapPeriodScanTimer = IHU_DIDOCAP_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.ethorionPeriodScanTimer = IHU_ETHORION_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.canvelaPeriodScanTimer = IHU_CANVELA_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.i2cariesPeriodScanTimer = IHU_I2CARIERS_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.ledpiscesPeriodScanTimer = IHU_LEDPISCES_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.ledpiscesGalowagScanTimer = IHU_LEDPISCES_TIMER_GALOWAG_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.pwmtaurusPeriodScanTimer = IHU_PWMTAURUS_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.spileoPeriodScanTimer = IHU_SPILEO_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.spsvirgoPeriodScanTimer = IHU_SPSVIRGO_TIMER_DURATION_PERIOD_SCAN;
	zIhuSysEngPar.timer.dcmiarisPeriodScanTimer = IHU_DCMIARIS_TIMER_DURATION_PERIOD_SCAN;	
	zIhuSysEngPar.timer.bfscPeriodScanTimer = IHU_BFSC_TIMER_DURATION_PERIOD_SCAN;
	//这里才是真正有意义的定时器定义
	zIhuSysEngPar.timer.bfscAdclibraScanTimer = IHU_BFSC_ADCLIBRA_SCAN_DURATION;
	zIhuSysEngPar.timer.bfscL3bfscWaitWeightTimer = IHU_BFSC_L3BFSC_WAIT_WEIGHT_DURATION;
	zIhuSysEngPar.timer.bfscL3bfscRolloutTimer = IHU_BFSC_L3BFSC_ROLL_OUT_DURATION;
	zIhuSysEngPar.timer.bfscL3bfscGiveupTimer = IHU_BFSC_L3BFSC_GIVE_UP_DURATION;	
	
	//Series Port config
	zIhuSysEngPar.serialport.SeriesPortForGPS = IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT;

	//后台部分
	strcpy(zIhuSysEngPar.cloud.cloudHttpAddLocal, IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL);

	//for IHU SW FTP

	//local SW storage address
	strcpy(zIhuSysEngPar.swDownload.ihuSwDownloadDir, IHU_SW_DOWNLOAD_DIR_DEFAULT);
	strcpy(zIhuSysEngPar.swDownload.ihuSwActiveDir, IHU_SW_ACTIVE_DIR_DEFAULT);
	strcpy(zIhuSysEngPar.swDownload.ihuSwBackupDir, IHU_SW_BACKUP_DIR_DEFAULT);

	//DEBUG部分
	zIhuSysEngPar.debugMode = IHU_TRACE_DEBUG_ON;
	
	//TRACE部分
	zIhuSysEngPar.traceMode = IHU_TRACE_MSG_ON;
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE){
		IhuDebugPrint("VMFO: Set basic engineering data correctly from SYSTEM DEFAULT parameters!\n");
	}
	
	//后台协议接口
	zIhuSysEngPar.cloud.cloudBhItfFrameStd = IHU_CLOUDXHUI_BH_INTERFACE_STANDARD;
	
	//打开所有模块和消息的TRACE属性
	memset(&(zIhuSysEngPar.traceList), 0, sizeof(SysEngParElementTrace_t));
	for (i=TASK_ID_MIN; i<TASK_ID_MAX; i++){
		zIhuSysEngPar.traceList.mod[i].moduleId = i;
		zIhuSysEngPar.traceList.mod[i].moduleFromAllow = TRUE;
		zIhuSysEngPar.traceList.mod[i].moduleToAllow = TRUE;		
	}
	for (i=MSG_ID_COM_MIN; i<MSG_ID_COM_MAX; i++){
		zIhuSysEngPar.traceList.msg[i].msgId = i;
		zIhuSysEngPar.traceList.msg[i].msgAllow = TRUE;
	}

	//硬件烧录区域，系统唯一标识部分，后面程序中访问到这些系统参数都必须从这个地方读取
	ihu_l1hd_f2board_equid_get(&(zIhuSysEngPar.hwBurnId));
	//由于硬件部分并没有真正起作用，所以暂时需要从系统定义区重复写入，一旦批量生产这部分可以去掉
	if (IHU_HARDWARE_MASSIVE_PRODUTION_SET == IHU_HARDWARE_MASSIVE_PRODUTION_NO){
		strncpy(zIhuSysEngPar.hwBurnId.equLable, IHU_CLOUDXHUI_HCU_NAME_SELF, (sizeof(IHU_CLOUDXHUI_HCU_NAME_SELF)<sizeof(zIhuSysEngPar.hwBurnId.equLable))?(sizeof(IHU_CLOUDXHUI_HCU_NAME_SELF)):(sizeof(zIhuSysEngPar.hwBurnId.equLable)));
		zIhuSysEngPar.hwBurnId.hwType = IHU_HARDWARE_PRODUCT_CAT_TYPE;
		zIhuSysEngPar.hwBurnId.hwPemId = IHU_CURRENT_HW_TYPE; //PEM小型号
		zIhuSysEngPar.hwBurnId.swRelId = IHU_CURRENT_SW_RELEASE;
		zIhuSysEngPar.hwBurnId.swVerId = IHU_CURRENT_SW_DELIVERY;
		zIhuSysEngPar.hwBurnId.swUpgradeFlag = IHU_HARDWARE_BURN_ID_FW_UPGRADE_SET;
	}	

	//处理启动模块
	for (i=0; i<MAX_TASK_NUM_IN_ONE_IHU; i++){
		zIhuTaskInfo[i].pnpState = IHU_TASK_PNP_INVALID;
	}
	//肯定需要启动的任务模块
	zIhuTaskInfo[TASK_ID_VMFO].pnpState = IHU_TASK_PNP_ON;
	zIhuTaskInfo[TASK_ID_TIMER].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_ADC == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_ADCLIBRA].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_SPI == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_SPILEO].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_I2C == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_I2CARIES].pnpState = IHU_TASK_PNP_ON;
	//if (IHU_COMM_FRONT_PWM == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_PWMTAURUS].pnpState = IHU_TASK_PNP_ON;
	//if (IHU_COMM_FRONT_SPS == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_SPSVIRGO].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_GPIO == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_CANVELA].pnpState = IHU_TASK_PNP_ON;
	//if (IHU_COMM_FRONT_DIDO == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_DIDOCAP].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_LED == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_LEDPISCES].pnpState = IHU_TASK_PNP_ON;
	//if (IHU_COMM_FRONT_ETH == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_ETHORION].pnpState = IHU_TASK_PNP_ON;
	//if (IHU_COMM_FRONT_DCMI == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_DCMIARIS].pnpState = IHU_TASK_PNP_ON;
	if (IHU_MAIN_CTRL_BFSC == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_BFSC].pnpState = IHU_TASK_PNP_ON;

	//初始化之后的系统标识信息
	IhuDebugPrint("VMFO: Initialized Hardware Burn Physical Id/Address: CURRENT_PRJ=[%s], HW_LABLE=[%s], PRODUCT_CAT=[0x%x], HW_TYPE=[0x%x], SW_RELEASE_VER=[%d.%d], FW_UPGRADE_FLAG=[%d].\n", \
		IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT, \
		zIhuSysEngPar.hwBurnId.equLable, \
		zIhuSysEngPar.hwBurnId.hwType, \
		zIhuSysEngPar.hwBurnId.hwPemId, \
		zIhuSysEngPar.hwBurnId.swRelId, \
		zIhuSysEngPar.hwBurnId.swVerId, \
		zIhuSysEngPar.hwBurnId.swUpgradeFlag);

#else
	#error Un-correct constant definition
#endif	
	
	return;
}

//TaskId transfer to string
//待调试的函数，是否需要使用动态内存
//正确的做法也许应该使用zIhuTaskInfo[id].TaskName这个信息来表达
OPSTAT ihu_taskid_to_string(UINT8 id, char *string)
{
	char tmp[TASK_NAME_MAX_LENGTH-2]="";
	
	if ((id<=TASK_ID_MIN) || (id>=TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;	
		IhuErrorPrint("VMFO: Error task Id input!\n");
		return IHU_FAILURE;
	}

	strcpy(string, "[");
	if (strlen(zIhuTaskNameList[id])>0){
		strncpy(tmp, zIhuTaskNameList[id], TASK_NAME_MAX_LENGTH-3);
		strcat(string, tmp);
	}else{
		strcat(string, "TASK_ID_XXX");
	}
	strcat(string, "]");
	return IHU_SUCCESS;
}

//MsgId transfer to string
//输入的string参数，其内存地址空间和长度预留，是否足够
OPSTAT ihu_msgid_to_string(UINT16 id, char *string)
{
	char tmp[MSG_NAME_MAX_LENGTH-2]="";
	
	if (id <= MSG_ID_COM_MIN || id >= MSG_ID_COM_MAX){
		zIhuRunErrCnt[TASK_ID_VMFO]++;	
		IhuErrorPrint("VMFO: Error Message Id input!\n");
		return IHU_FAILURE;
	}

	strcpy(string, "[");
	if (strlen(zIhuMsgNameList[id-MSG_ID_COM_MIN])>0){
		strncpy(tmp, zIhuMsgNameList[id-MSG_ID_COM_MIN], MSG_NAME_MAX_LENGTH-3);
		strcat(string, tmp);
	}else{
		strcat(string, "MSG_ID_XXX");
	}
	strcat(string, "]");

	return IHU_SUCCESS;
}


/*******************************************************************************
**
**	全局公共的状态机机制，任务和队列相关的API, 不依赖于任何操作系统
**
**********************************************************************************/

/*******************************************************************************
**	状态机API
**
**	METHOD :FsmInit
**------------------------------------------------------------------------------
** Description :
**    Initialize the module of the fsm.
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 none
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : IHU_SUCCESS OR IHU_FAILURE
*******************************************************************************/
OPSTAT FsmInit(void)
{
	UINT32 i;
	IhuDebugPrint("VMFO: >>Start init FSM.\n");
	zIhuFsmTable.numOfFsmCtrlTable = 0;
	for(i=0; i<MAX_TASK_NUM_IN_ONE_IHU; i++)
	{
		zIhuFsmTable.pFsmCtrlTable[i].taskId = TASK_ID_INVALID;
		zIhuFsmTable.pFsmCtrlTable[i].numOfFsmArrayElement = 0;
	}
	zIhuFsmTable.currentTaskId = TASK_ID_INVALID;

	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_FAT_ON) != FALSE){
		IhuDebugPrint("VMFO: Maxium (%d) tasks/modules supported.\n", MAX_TASK_NUM_IN_ONE_IHU);
	}
    return IHU_SUCCESS;
}

/*******************************************************************************
** METHOD : FsmAddNew
**------------------------------------------------------------------------------
** Description :
**    fill the FSM table information of entity
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 Taskid : entity identification
**			 pFsmStateItem: pointer to message table of entity
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : IHU_SUCCESS OR IHU_FAILURE
*******************************************************************************/
OPSTAT FsmAddNew(UINT8 task_id, FsmStateItem_t* pFsmStateItem)
{
	OPSTAT ret;
	UINT32 state;
	UINT32 msgid;
	UINT32 item, itemNo, i, j;

	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE){
		IhuDebugPrint("VMFO: >>Register new FSM. TaskId:(%d), pFsm(0x%x).\n", task_id, (UINT32)pFsmStateItem);
	}
	/*
	** Check the array of the state machine.
	*/
	if( pFsmStateItem == NULL)
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Can not init FSM machine.\n");
		return IHU_FAILURE;
	}
	if((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX))
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The task_ID is invalid.\n");
		return IHU_FAILURE;
	}
	if( zIhuFsmTable.pFsmCtrlTable[task_id].taskId != TASK_ID_INVALID )
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: This task_id has been already inited.\n");
		return IHU_FAILURE;
	}

	/*
	** Check the first entry. 最初一个FSM表单必须以FSM_STATE_ENTRY开始
	*/
	if( pFsmStateItem[FSM_STATE_ENTRY].state != FSM_STATE_ENTRY )
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The format of the FSM table is invalid.\n");
		return IHU_FAILURE;
	}

    /*
    ** Check each item of the state entry. 最后一个FSM表单使用FSM_STATE_END结束
    */
	for(item=1; item < MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK; item++)
	{
		if( FSM_STATE_END == pFsmStateItem[item].state )
		{
			break;
		}
		if( FSM_STATE_ENTRY == pFsmStateItem[item].state )
		{
			zIhuRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: Invalid FSM machine.\n");
			return IHU_FAILURE;
		}
		if( NULL == pFsmStateItem[item].stateFunc)
		{
			zIhuRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: Invalid state function pointer.\n");
			return IHU_FAILURE;
		}
	}

	/*
	** Check the number of the item.
	** If we can not find the ENDING flag of the state machine after more than
	** MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK entry, raise fatal alarm.
	*/
	if( MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK == item)
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Invalid FSM machine -- Can not find the end of the FSM.\n");
		return IHU_FAILURE;
	}
	zIhuFsmTable.pFsmCtrlTable[task_id].numOfFsmArrayElement = item-1; //有效STATE-MSG条目数，不包括START/END两条

	/*
	**  Insert this new fsm item into fsm table.
	**  The position in the fsm table is based on the task_id.
	*/
	zIhuFsmTable.numOfFsmCtrlTable++;

	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE)
	{
		IhuDebugPrint("VMFO: FsmAddNew: task_id = 0x%x [%s], src_id= %x, dest_id= %X.\n", task_id, zIhuTaskNameList[task_id], 0, 0);
		IhuDebugPrint("VMFO: After add this one, Total (%d) FSM in the table.\n", zIhuFsmTable.numOfFsmCtrlTable);
	}

	/*
	** Save the state machine info.
	*/
	zIhuFsmTable.pFsmCtrlTable[task_id].taskId = task_id;
	for(i=0; i<MAX_STATE_NUM_IN_ONE_TASK; i++)
	{
		for(j=0; j<MAX_MSGID_NUM_IN_ONE_TASK; j++)
		{
			zIhuFsmTable.pFsmCtrlTable[task_id].pFsmArray[i][j].stateFunc = NULL;
		}
	}

	//检查Msg_id的合法性，不能超过FF，这里很有技巧，全靠消息ID不超过FF来设定，小心消息过多的情况
	//内部消息也不能使用任意定义的数值，因为这里将采用MsgId进行索引，以提高效率
	for(itemNo=0; itemNo < item; itemNo++)
	{
		state = pFsmStateItem[itemNo].state;
		if(state >= MAX_STATE_NUM_IN_ONE_TASK)
		{
			zIhuRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: The State number > %d.\n", MAX_STATE_NUM_IN_ONE_TASK);
			return IHU_FAILURE;
		}
		msgid = pFsmStateItem[itemNo].msg_id & MASK_MSGID_NUM_IN_ONE_TASK;
		zIhuFsmTable.pFsmCtrlTable[task_id].pFsmArray[state][msgid].stateFunc = pFsmStateItem[itemNo].stateFunc;
	}

	/*
	** Start the task entry init api/function
	*/
	//Input message id = 0, param_ptr = NULL, param_len = 0.
    /*
    ** call the entry
    ** 这一块代码不能复用，因为这个任务和被执行任务并不再同一个空间之内，它只能被目标任务的入口函数进行执行
    ** 该函数API必须在每个任务中进行表达，否则一定会出错， 该代码需要移动到任务创建成功后的入口函数中进行调用
    */
	//(zIhuFsmTable.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_COM_MIN].stateFunc)(task_id, 0, NULL, 0);

	/*
	** Set the state to init state(FSM_STATE_IDLE).
	** 一旦任务创建，自动转移到IDLE状态，因而每个任务独特的初始化不能处理状态，也不能被FsmRunEngine调度，而必须被任务创建的入口函数所执行
	*/
	ret = FsmSetState(task_id, FSM_STATE_IDLE);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error set FSM State!\n");
	}

    return IHU_SUCCESS;
}

OPSTAT FsmRemove(UINT8 task_id)
{
	//入参检查
	if((task_id <= (UINT8)TASK_ID_MIN) || (task_id >= TASK_ID_MAX))
	{
		IhuErrorPrint("VMFO: The task_ID is invalid.\n");
		return IHU_FAILURE;
	}

	//设置无效
	zIhuFsmTable.pFsmCtrlTable[task_id].taskId = TASK_ID_INVALID;

    return IHU_SUCCESS;
}

/*******************************************************************************
** METHOD : FsmRunEngine
**------------------------------------------------------------------------------
** Description :
**    supply action function entry of task that receive current Event in current state
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 src_id: identification of task that send out current message
** 		     dest_id: identification of task that receive current message
** 		     commandId: identification of current message
** 		     param_ptr: pointer to current message structure
** 		     param_len: message length
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : IHU_SUCCESS OR IHU_FAILURE
*******************************************************************************/
OPSTAT FsmRunEngine(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	UINT8  state;
	OPSTAT ret;
	UINT32 mid;
	/*
	** Check the task_id, message_id and par_length
	*/
	
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, src_id=[0x%x], dest_id=[0x%x], msgid=[0x%x][%s]!!!\n", src_id, dest_id, msg_id, zIhuMsgNameList[msg_id]);
		return IHU_FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, src_id=[0x%x], dest_id=[0x%x], msgid=[0x%x][%s]!!!\n", src_id, dest_id, msg_id, zIhuMsgNameList[msg_id]);
		return IHU_FAILURE;
	}
	if (param_len>MAX_IHU_MSG_BODY_LENGTH){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
		return IHU_FAILURE;
	}
	if ((msg_id <= MSG_ID_COM_MIN) || (msg_id >= MSG_ID_COM_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on Msg_Id, msg_id=%d!!!\n", msg_id);
		return IHU_FAILURE;
	}

	/*
	** Search for the dest_id task state
	*/

	if( zIhuFsmTable.pFsmCtrlTable[dest_id].taskId != dest_id )
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The destination process does not exist.\n");
		return IHU_FAILURE;
	}

	/*
	** Get current FSM state
	*/
	state = FsmGetState(dest_id);
	mid = msg_id & MASK_MSGID_NUM_IN_ONE_TASK;

	//check the state and messageId of task
	if((state >= MAX_STATE_NUM_IN_ONE_TASK)||(mid >= MAX_MSGID_NUM_IN_ONE_TASK))
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The state(%d) or msgId(0x%x) of task(0x%x) is error.\n", state, mid, dest_id);
		return IHU_FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE)
	{
		//消息ID的顺序跟commsgxxx.h的中的定义可能不完全一样，导致这里直接POP出消息名字会出错，注意！
		IhuDebugPrint("VMFO: Call state function(0x%x) in state(%d) of dest task(0x%x[%s]) for msg(0x%x[%s]), src=(0x%x[%s]).\n", (UINT32)zIhuFsmTable.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc, state, dest_id, zIhuTaskNameList[dest_id], mid, zIhuMsgNameList[mid-1], src_id, zIhuTaskNameList[src_id]);
	}

	/*
	** Call the state function.
	*/
	if(zIhuFsmTable.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc != NULL)
	{
		ret = (zIhuFsmTable.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc)(dest_id, src_id, param_ptr, param_len);
		if( IHU_FAILURE == ret)
		{
			zIhuRunErrCnt[TASK_ID_VMFO]++;			
			IhuErrorPrint("VMFO: Internal error is found in the state function.\n");
			return IHU_FAILURE;
		}
	}
	else
	{
		if(NULL != param_ptr)
		{
			//Free memory, here do nothing.
		}
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		//消息ID的顺序跟commsgxxx.h的中的定义可能不完全一样，导致这里直接POP出消息名字会出错，注意！		
		IhuErrorPrint("VMFO: Receive invalid msg(%x)[%s] in state(%d) of task(0x%x)[%s] from task(0x%x)[%s].\n", mid, zIhuMsgNameList[mid-1], state, dest_id, zIhuTaskNameList[dest_id], src_id, zIhuTaskNameList[src_id]);
		return IHU_FAILURE;
	}

    return IHU_SUCCESS;
}

/*******************************************************************************
** METHOD : FsmProcessingLaunch
**------------------------------------------------------------------------------
** Description :
**    check and handle message loop as each task entry
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 none
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : IHU_SUCCESS OR IHU_FAILURE
*******************************************************************************/
OPSTAT FsmProcessingLaunch(void *task)
{
	OPSTAT ret;
	IhuMsgSruct_t rcv;
	UINT8 task_id = 0; //Get current working task_id
	
	/*
	** Check the task_id
	*/
	//这里的currentTaskId通过currentTaskId是传不过来的，必须采用arg参数传递
	//task_id = zIhuFsmTable.currentTaskId;
	//zIhuFsmTable.currentTaskId = TASK_ID_INVALID;
	task_id = (UINT8)*(char *)task;

	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	/*
	** Run each task entry API, give each task a chance to init any specific information
	*/
	if (zIhuFsmTable.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc != NULL){
		(zIhuFsmTable.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc)(task_id, 0, NULL, 0);
	}else{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Task (ID=%d) get no init entry fetched!\n", task_id);
	}
	
	//任务中的消息轮循
	//从当前TASKID对应的消息队列读取消息
	//每一次循环，只从消息队列中读取一个消息并进行执行，这相当于所有任务的优先级都是均等的，不存在优先级高的问题
	//如果消息过多，可能会造成消息队列的溢出，这里没有考虑消息队列的共享问题，未来可以考虑继续优化
	//小喇叭响起来，这里是资源优化集中营！
		//Loop for ever
	while (1){
		memset(&rcv, 0, sizeof(IhuMsgSruct_t));
		ret = ihu_message_rcv(task_id, &rcv);
		//以下方式是为了调查出现无效接收消息时的方式，就是扎到出现问题消息的特征，进而帮助分析产生无效消息的来源
//		if (task_id == TASK_ID_SPSVIRGO) IhuDebugPrint("VMFO: Received message at task_id=%d [%s], &rcv=[0x%x]\n", task_id, zIhuTaskNameList[task_id], &rcv);
//		if ((rcv.src_id <=TASK_ID_MIN) || (rcv.src_id >= TASK_ID_MAX) || (rcv.dest_id <=TASK_ID_MIN) || (rcv.dest_id >= TASK_ID_MAX)){
//			IhuErrorPrint("VMFO: TEST msg_id=[0x%x], &msg_id=[0x%x]\n", rcv.msgType, &(rcv.msgType));
//			IhuErrorPrint("VMFO: TEST dest_id=[0x%x], &dest_id=[0x%x]\n", rcv.dest_id, &(rcv.dest_id));
//			IhuErrorPrint("VMFO: TEST src_id=[0x%x], &src_id=[0x%x]\n", rcv.src_id, &(rcv.src_id));
//			IhuErrorPrint("VMFO: TEST msg_len=[0x%x], &msg_len=[0x%x]\n", rcv.msgLen, &(rcv.msgLen));
//		}
		
		//纯粹是为了消除最后返回RETURN的WARNING告警问题
		if (ret == -3){
			break;
		}
		//留一个rcv函数返回的口子，以便继续执行
		else if (ret == -2){
			continue;
		}
		else if (ret == IHU_FAILURE){
			IhuDebugPrint("VMFO: STM kernal receive message error, hold for further action!\n");
			//To be defined here the error case, to recover everything
			ihu_sleep(1); 
		}else{
			ret = FsmRunEngine(rcv.msgType, rcv.dest_id, rcv.src_id, rcv.msgBody, rcv.msgLen);
			if (ret == IHU_FAILURE){
				zIhuRunErrCnt[TASK_ID_VMFO]++;	
				IhuErrorPrint("VMFO: Error execute FsmRun state machine!\n");
			}
		}
	}//While(1)
	
	return IHU_SUCCESS;
}


//设置状态
OPSTAT FsmSetState(UINT8 task_id, UINT8 newState)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: FsmSetState error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//Checking newState range
	if (newState > MAX_STATE_NUM_IN_ONE_TASK){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: FsmSetState error on state, State=%d!!!\n", newState);
		return IHU_FAILURE;
	}

	zIhuTaskInfo[task_id].state = newState;
	return IHU_SUCCESS;
}

//获取状态
UINT8 FsmGetState(UINT8 task_id)
{
	/*
	** Check the task_id
	*/
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return FSM_STATE_INVALID;
	}
	/*
	** Check the state info
	*/
	if (zIhuTaskInfo[task_id].state <= MAX_STATE_NUM_IN_ONE_TASK){
		return zIhuTaskInfo[task_id].state;
	}else{
		return FSM_STATE_INVALID;
	}
}

/**********************************************************************************
 *
 *   IHU VMLAYER全局函数过程
 *   依赖于FreeRTOS，面对底层SDK所定义的过程API函数
 *
 **********************************************************************************/
/**************************************************************************************
 * 创建任务队列，以便用于任务之间消息通信, 每个任务创建唯一一个队列，所以使用TASKID作为索引
 * void  Ihu_message_queue_create( UINT task_id)
 ***************************************************************************************/
OPSTAT ihu_message_queue_create(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//OS_QUEUE_CREATE(queue, item_size, max_items)
	OS_QUEUE_CREATE(zIhuTaskInfo[task_id].QueId, IHU_QUEUE_MAX_SIZE, MAX_QUEUE_NUM_IN_ONE_TASK);
	if(zIhuTaskInfo[task_id].QueId == NULL){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Failed to create msg-queue | errno=%d [%s]\n", errno, strerror(errno));
		return IHU_FAILURE;
	}

	return IHU_SUCCESS;
}

//kill message queue
OPSTAT ihu_message_queue_delete(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//Not exist
	if (zIhuTaskInfo[task_id].QueId == NULL) {
    zIhuRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Null msg-queue and no need delete | errno=%d [%s]\n", errno, strerror(errno));
	  return IHU_FAILURE;
	}

	//Delete queue
	OS_QUEUE_DELETE(zIhuTaskInfo[task_id].QueId);
	zIhuTaskInfo[task_id].QueId = NULL;

	return IHU_SUCCESS;
}

//查询消息队列
QueueHandle_t ihu_message_queue_inquery(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return 0;
	}

	return zIhuTaskInfo[task_id].QueId;
}

//这个过程用在同步不同进程之间的消息队列，确保不同进程之间的任务可以互发消息
//如果队列本来就不存在，就不用同步或者重新填入全局控制表了
OPSTAT ihu_message_queue_resync(void)
{
  //此函数在VMFO中没有实质意义

	return IHU_SUCCESS;
}

//清理某一个任务的消息队列
OPSTAT ihu_message_queue_clean(UINT8 dest_id)
{
	//入参检查
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return IHU_FAILURE;
	}

	//先删除消息队列
  if (zIhuTaskInfo[dest_id].QueId == NULL) {
    zIhuRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Null msg-queue and no need clean | errno=%d [%s]\n", errno, strerror(errno));
    return IHU_FAILURE;
  }
	OS_QUEUE_DELETE(zIhuTaskInfo[dest_id].QueId);

	//再建立消息队列
	OS_QUEUE_CREATE(zIhuTaskInfo[dest_id].QueId, IHU_QUEUE_MAX_SIZE, MAX_QUEUE_NUM_IN_ONE_TASK);
  if (zIhuTaskInfo[dest_id].QueId == NULL) {
    zIhuRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: Not successfully re-build the msg queue | errno=%d [%s]\n", errno, strerror(errno));
    return IHU_FAILURE;
  }

	return IHU_SUCCESS;
}

//message send basic processing
//All in parameters
OPSTAT ihu_message_send(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	//int ret = 0;
	char s1[TASK_NAME_MAX_LENGTH+2]="", s2[TASK_NAME_MAX_LENGTH+2]="", s3[MSG_NAME_MAX_LENGTH]="";
	IhuMsgSruct_t msg;
	
	//Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return IHU_FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, src_id=%d!!!\n", src_id);
		return IHU_FAILURE;
	}
	if (param_len > MAX_IHU_MSG_BODY_LENGTH){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
		return IHU_FAILURE;
	}

	//Init to clean this memory area
  //初始化消息内容，是为了稳定可靠安全
  memset(&msg, 0, sizeof(IhuMsgSruct_t));
  msg.msgType = msg_id;
  msg.dest_id = dest_id;
  msg.src_id = src_id;
  msg.msgLen = param_len;
  memcpy(&(msg.msgBody[0]), param_ptr, param_len);

  //正式发送QUEUE
  if (OS_QUEUE_PUT(zIhuTaskInfo[dest_id].QueId, &msg, 0) != OS_QUEUE_OK){
    zIhuRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: msgsnd() write msg failed, errno=%d[%s], dest_id = %d [%s]\n",errno,strerror(errno), dest_id, zIhuTaskNameList[dest_id]);
    //zIhuTaskInfo[dest_id].QueFullFlag = IHU_TASK_QUEUE_FULL_TRUE;
    return IHU_FAILURE;
  }

	/*
	 *  Message Trace processing
	 *  注意字符串长度，太小会出现内存飞掉的情形，MessageTrace的数据配置来源于数据库，层次比这个还要高，虽然有点怪怪的，但不影响总体架构
	 *  最开始采用IFDEF的形式，后期完善后改为更为直接的代码方式
	 *  软件开发中，DEBUG和TRACE是两种最常用/最有用的调试模式，比单步还有用，这不仅是因为熟手不需要单步执行，而且也是因为多线程多进程单步执行环境制造的复杂性
	 *
	 *  有关MESSAGE TRACE的效率，是一个要注意的问题，当系统负载不高时，打开所有的TRACE是合适的，但一旦部署实际系统，TRACE需要减少到最低程度，这是原则，实际需要
	 *  维护人员根据情况灵活把我
	 *
	 *  本TRACE功能，提供了多种工作模式
	 *
	 */
	switch (zIhuSysEngPar.traceMode)
	{
		case IHU_TRACE_MSG_MODE_OFF:
			break;

		case IHU_TRACE_MSG_MODE_INVALID:
			break;

		case IHU_TRACE_MSG_MODE_ALL:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			break;

		case IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if (msg_id != MSG_ID_COM_TIME_OUT){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_ALL_BUT_HEART_BEAT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((msg_id != MSG_ID_COM_TIME_OUT) && (msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_MOUDLE_TO_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_MOUDLE_TO_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict!= TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_MOUDLE_FROM_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_MOUDLE_FROM_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict!= TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_MOUDLE_DOUBLE_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_MOUDLE_DOUBLE_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_MSGID_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_MSGID_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_COMBINE_TO_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_COMBINE_TO_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_COMBINE_FROM_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_COMBINE_FROM_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_COMBINE_DOUBLE_ALLOW:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case IHU_TRACE_MSG_MODE_COMBINE_DOUBLE_RESTRICT:
			ihu_taskid_to_string(dest_id, s1);
			ihu_taskid_to_string(src_id, s2);
			ihu_msgid_to_string(msg_id, s3);
			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)
					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
				IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		default:
			if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE){
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: System Engineering Parameter Trace Mode setting error! DebugMode=%d\n", zIhuSysEngPar.debugMode);
			}
			break;
	}

	return IHU_SUCCESS;
}


//Reveive message
//dest_id: in
//*msg: out
// ret: 接受消息中的errno=4/EINTR需要单独处理，不然会出现被SIGALRM打断的情形
OPSTAT ihu_message_rcv(UINT8 dest_id, IhuMsgSruct_t *msg)
{
	//Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return IHU_FAILURE;
	}

	//未来调试时，需要注意，这里的BUFFER长度是否合理。原先在LINUX系统中，需要扣掉头部
	//ret = msgrcv(zIhuTaskInfo[dest_id].QueId, msg, (sizeof(IhuMsgSruct_t) - sizeof(long)), 0, 0);
	if (OS_QUEUE_GET(zIhuTaskInfo[dest_id].QueId, msg, OS_QUEUE_FOREVER) != OS_QUEUE_OK){
    zIhuRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMFO: msgrcv() receive msg failed, Qid=%d, msg=%08X, errno=%d[%s]\n", zIhuTaskInfo[dest_id].QueId, msg, errno, strerror(errno));
    //防止某个任务在接受消息时，既不阻塞，也接受出错，所以休眠100ms，保持其他任务的正常运行
    ihu_usleep(100);
    return IHU_FAILURE;
	}

	return IHU_SUCCESS;
}


/**************************************************************************************
 * 创建任务，使用的是线程模拟，各个任务之间可以共享内存
 * To Create a thread
 * void  ihu_task_create( UINT8 task_id,          		out: pointer to zIhuTaskId[task_id]
 *                   void *(*task_func)(void *),        in: task's entry point
 *                   void *arg,                         in: parameter of the task's function
 *                   int prio )                         in: priority of the task*
 ***************************************************************************************/
OPSTAT ihu_task_create(UINT8 task_id, void *(*task_func)(void *), void *arg, int prio)
{
	int                     err;
	OS_BASE_TYPE ret;

	// Checking task_id
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id!! err=%d, errno=%d, %s, task_id=%d\n", err, errno, strerror(err), task_id);
		return IHU_FAILURE;
	}

	//As FsmProcessLaunch has to transfer task_id in, we will use global variance of zIhuFsmTable.currentTaskId
	//So task_id has to store into there
	//Wonderful mechanism!!!
	//在FreeRTOS下，任务创建好了之后，并没有立即启动，而是要等待osKernelStart()执行之后才能启动调度，所以这个机制对FreeRTOS并不适合
//	if (zIhuFsmTable.currentTaskId != TASK_ID_INVALID){
//		ihu_sleep(1);
//	}
//	if (zIhuFsmTable.currentTaskId != TASK_ID_INVALID){
//		zIhuRunErrCnt[TASK_ID_VMFO]++;
//		IhuErrorPrint("VMFO: Task_id not yet ready by running process, new task create failure!\n");
//		return IHU_FAILURE;
//	}
//	zIhuFsmTable.currentTaskId = task_id;
//void (*TaskFunction_t)( void * )
	
	// creation of the task
  ret = OS_TASK_CREATE(
        (const char *)zIhuTaskNameList[task_id],  /* The text name assigned to the task, for debug only; not used by the kernel. */
        //(void *(*)(void *))(task_func),         /* The System Initialization task. */
        (TaskFunction_t)(task_func),
        arg,                                      /* The parameter passed to the task. */
        IHU_TASK_STACK_SIZE * OS_STACK_WORD_SIZE, /* The number of bytes to allocate to the stack of the task. */
        prio,                                     /* The priority assigned to the task. */
        zIhuTaskInfo[task_id].TaskHandle);        /* The task handle */
	if(ret != OS_TASK_CREATE_SUCCESS)
	{
	  zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuDebugPrint("VMFO: Create task nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
		return IHU_FAILURE;
	}

	//存储现场记录, zIhuTaskInfo[task_id].TaskName to be added in another way, TBD
	zIhuTaskInfo[task_id].TaskId = task_id;
	IhuDebugPrint("VMFO: pthread_create() OK ...\n");

	//返回
	return IHU_SUCCESS;
}

//kill task
OPSTAT ihu_task_delete(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//只是清理掉FSM中的信息
	FsmRemove(task_id);

  //删除任务
	if ((zIhuTaskInfo[task_id].TaskHandle == NULL) || (zIhuTaskInfo[task_id].TaskHandle == 0)){
    zIhuRunErrCnt[TASK_ID_VMFO]++;
    IhuErrorPrint("VMUO: No exist or right task to be deleted! \n");
    return IHU_FAILURE;
	}
	OS_TASK_DELETE(zIhuTaskInfo[task_id].TaskHandle);

	//返回
	return IHU_SUCCESS;
}

//聚合创建任务，消息队列，并直接让其开始运行
OPSTAT ihu_task_create_and_run(UINT8 task_id, FsmStateItem_t* pFsmStateItem)
{
	OPSTAT ret = 0;
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}

	//Init Fsm table
	ret = FsmAddNew(task_id, pFsmStateItem);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Init state machine FsmAddNew error, taskid = %d.\n", task_id);
		return IHU_FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_CRT_ON) != FALSE){
		IhuDebugPrint("VMFO: FsmAddNew Successful, taskId = 0x%x [%s].\n", task_id, zIhuTaskNameList[task_id]);
	}

	//Create Queid
	ret = ihu_message_queue_create(task_id);
	if (ret == IHU_FAILURE)
	{
	zIhuRunErrCnt[TASK_ID_VMFO]++;
	IhuErrorPrint("VMFO: Create queue unsuccessfully, taskId = %d.\n", task_id);
	return IHU_FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_CRT_ON) != FALSE){
		IhuDebugPrint("VMFO: Msgque create successful, taskId = 0x%x [%s].\n", task_id, zIhuTaskNameList[task_id]);
	}

	//Create task and make it running for the 1st time
	//让VMFO保持更高一级的优先级，以便VMFO及时处理守护程序的职责
	if (task_id == TASK_ID_VMFO) ret = ihu_task_create(task_id, /*CALLBACK*/(void *(*)(void *))FsmProcessingLaunch, (void *)&(zIhuTaskInfo[task_id].TaskId), IHU_THREAD_PRIO+1);
	else ret = ihu_task_create(task_id, /*CALLBACK*/(void *(*)(void *))FsmProcessingLaunch, (void *)&(zIhuTaskInfo[task_id].TaskId), IHU_THREAD_PRIO);
	if (ret == IHU_FAILURE)
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Create task un-successfully, taskid = %d.\n", task_id);		
		return IHU_FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_CRT_ON) != FALSE){
	  IhuDebugPrint("VMFO: Task create Successful, taskId = 0x%x [%s].\n", task_id, zIhuTaskNameList[task_id]);
	}
	
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_IPT_ON) != FALSE){
		IhuDebugPrint("VMFO: Whole task environment setup successful, taskId = 0x%x [%s].\n", task_id, zIhuTaskNameList[task_id]);
	}
	
	//给该任务设置一个软的状态：省略了
	
	return IHU_SUCCESS;
}

//初始化任务并启动
OPSTAT ihu_system_task_init_call(UINT8 task_id, FsmStateItem_t *p)
{
	int ret = 0;
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input Error on task_id, task_id=%d!!!\n", task_id);
		return IHU_FAILURE;
	}
	
	//入参检查
	if (p == NULL){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input wrong FsmStateItem pointer!\n");
		return IHU_FAILURE;
	}
	
	//任务控制启动标示检查
	if (zIhuTaskInfo[task_id].pnpState != IHU_TASK_PNP_ON){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: no need create this task [%s]!\n", zIhuTaskNameList[task_id]);	
		return IHU_FAILURE;
	}
	
	//打印信息
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_CRT_ON) != FALSE){
		IhuDebugPrint("VMFO: Staring to create task [%s] related environments...\n", zIhuTaskNameList[task_id]);
	}

	//任务创建并初始化所有状态机
	ret = ihu_task_create_and_run(task_id, p);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: create task env [%s] un-successfully, program exit.\n", zIhuTaskNameList[task_id]);
		return IHU_FAILURE;
	}else{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: create task successfully, taskid=%d[%s].\n", task_id, zIhuTaskNameList[task_id]);
		}
	}
	
	//将该任务执行时对应的状态转移机指针存入，以供以后扫描时使用
	zIhuTaskInfo[task_id].fsmPtr = p;
	
	//正确返回
	return IHU_SUCCESS;
}

//创建所有任务
void ihu_task_create_all(void)
{
	//Create VMFO environments /1
	if (zIhuTaskInfo[TASK_ID_VMFO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_VMFO, FsmVmfo);	
	ihu_vm_send_init_msg_to_app_task(TASK_ID_VMFO);

	//Create task Timer environments /2
	if (zIhuTaskInfo[TASK_ID_TIMER].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_TIMER, FsmTimer);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_TIMER);	

	//未来考虑使用handler初始化所有的状态机入口，从而不再需要这个ifdef的方式
	
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	//Create task ADCLIBRA environments /3
//	if (zIhuTaskInfo[TASK_ID_ADCLIBRA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_ADCLIBRA, FsmAdclibra);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_ADCLIBRA);
//
//	//Create task SPILEO environments /4
//	if (zIhuTaskInfo[TASK_ID_SPILEO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_SPILEO, FsmSpileo);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_SPILEO);
//
//	//Create task I2CARIES environments /5
//	if (zIhuTaskInfo[TASK_ID_I2CARIES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_I2CARIES, FsmI2caries);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_I2CARIES);
//
//	//Create task PWMTAURUS environments /6
//	if (zIhuTaskInfo[TASK_ID_PWMTAURUS].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_PWMTAURUS, FsmPwmtaurus);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_PWMTAURUS);
//
//	//Create task SPSVIRGO environments /7
//	if (zIhuTaskInfo[TASK_ID_SPSVIRGO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_SPSVIRGO, FsmSpsvirgo);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_SPSVIRGO);
//
//	//Create task CANVELA environments /8
//	if (zIhuTaskInfo[TASK_ID_CANVELA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_CANVELA, FsmCanvela);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_CANVELA);
//
//	//Create task DIDOCAP environments /9
//	if (zIhuTaskInfo[TASK_ID_DIDOCAP].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_DIDOCAP, FsmDidocap);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_DIDOCAP);
//
//	//Create task LEDPISCES environments /10
//	if (zIhuTaskInfo[TASK_ID_LEDPISCES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_LEDPISCES, FsmLedpisces);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_LEDPISCES);
//
//	//Create task ETHORION environments /11
//	if (zIhuTaskInfo[TASK_ID_ETHORION].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_ETHORION, FsmEthorion);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_ETHORION);
	
	//Create task EMC68X environments /12
	if (zIhuTaskInfo[TASK_ID_EMC68X].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_EMC68X, FsmEmc68x);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_EMC68X);
	
	IhuDebugPrint("VMFO: Create all task successfully!\n");	
	
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	//Create task ADCLIBRA environments /3
	if (zIhuTaskInfo[TASK_ID_ADCLIBRA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_ADCLIBRA, FsmAdclibra);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_ADCLIBRA);

	//Create task SPILEO environments /4
//	if (zIhuTaskInfo[TASK_ID_SPILEO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_SPILEO, FsmSpileo);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_SPILEO);

	//Create task I2CARIES environments /5
	if (zIhuTaskInfo[TASK_ID_I2CARIES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_I2CARIES, FsmI2caries);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_I2CARIES);

	//Create task PWMTAURUS environments /6
//	if (zIhuTaskInfo[TASK_ID_PWMTAURUS].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_PWMTAURUS, FsmPwmtaurus);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_PWMTAURUS);

	//Create task SPSVIRGO environments /7
	if (zIhuTaskInfo[TASK_ID_SPSVIRGO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_SPSVIRGO, FsmSpsvirgo);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_SPSVIRGO);

	//Create task CANVELA environments /8
//	if (zIhuTaskInfo[TASK_ID_CANVELA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_CANVELA, FsmCanvela);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_CANVELA);

	//Create task DIDOCAP environments /9
	if (zIhuTaskInfo[TASK_ID_DIDOCAP].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_DIDOCAP, FsmDidocap);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_DIDOCAP);

	//Create task LEDPISCES environments /10
	if (zIhuTaskInfo[TASK_ID_LEDPISCES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_LEDPISCES, FsmLedpisces);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_LEDPISCES);

	//Create task ETHORION environments /11
//	if (zIhuTaskInfo[TASK_ID_ETHORION].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_ETHORION, FsmEthorion);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_ETHORION);

	//Create task DCMIARIS environments /11.5
	if (zIhuTaskInfo[TASK_ID_DCMIARIS].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_DCMIARIS, FsmDcmiaris);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_DCMIARIS);

	//Create task EMC68X environments /12
	if (zIhuTaskInfo[TASK_ID_CCL].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_CCL, FsmCcl);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_CCL);
	
	IhuDebugPrint("VMFO: Create all task successfully!\n");

#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)

	//Create task ADCLIBRA environments /3
	if (zIhuTaskInfo[TASK_ID_ADCLIBRA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_ADCLIBRA, FsmAdclibra);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_ADCLIBRA);

	//Create task SPILEO environments /4
	if (zIhuTaskInfo[TASK_ID_SPILEO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_SPILEO, FsmSpileo);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_SPILEO);

	//Create task I2CARIES environments /5
	if (zIhuTaskInfo[TASK_ID_I2CARIES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_I2CARIES, FsmI2caries);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_I2CARIES);

	//Create task PWMTAURUS environments /6
//	if (zIhuTaskInfo[TASK_ID_PWMTAURUS].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_PWMTAURUS, FsmPwmtaurus);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_PWMTAURUS);

	//Create task SPSVIRGO environments /7
//	if (zIhuTaskInfo[TASK_ID_SPSVIRGO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_SPSVIRGO, FsmSpsvirgo);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_SPSVIRGO);

	//Create task CANVELA environments /8
	if (zIhuTaskInfo[TASK_ID_CANVELA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_CANVELA, FsmCanvela);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_CANVELA);

	//Create task DIDOCAP environments /9
//	if (zIhuTaskInfo[TASK_ID_DIDOCAP].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_DIDOCAP, FsmDidocap);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_DIDOCAP);

	//Create task LEDPISCES environments /10
	if (zIhuTaskInfo[TASK_ID_LEDPISCES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_LEDPISCES, FsmLedpisces);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_LEDPISCES);

	//Create task ETHORION environments /11
//	if (zIhuTaskInfo[TASK_ID_ETHORION].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_ETHORION, FsmEthorion);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_ETHORION);
	
	//Create task DCMIARIS environments /11.5
//	if (zIhuTaskInfo[TASK_ID_DCMIARIS].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_DCMIARIS, FsmDcmiaris);
//	ihu_vm_send_init_msg_to_app_task(TASK_ID_DCMIARIS);
	
	//Create task EMC68X environments /12
	if (zIhuTaskInfo[TASK_ID_BFSC].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_BFSC, FsmBfsc);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_BFSC);
	
	IhuDebugPrint("VMFO: Create all task successfully!\n");
#else
	#error Un-correct constant definition
#endif
}

void ihu_task_delete_all_and_queue(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	IhuDebugPrint("VMFO: Clean all running task and message queue for next time debug!\n");
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		ihu_task_delete(task_id);
		ihu_message_queue_delete(task_id);
	}
	return;
}

//公共API，不做任何事情
OPSTAT fsm_com_do_nothing(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
{
	return IHU_SUCCESS;
}

//Unix Time transfer to YMD time
struct tm ihu_clock_unix_to_ymd(time_t t_unix)
{
		return *localtime(&t_unix);
}


/*******************************************************************************
**
**  没有RTOS的情形
**
**********************************************************************************/
////这里将LaunchEntry和LaunchExecute是因为在VMDA环境中，创造环境和执行不在一起
////具有真正任务的操作系统环境中，没有这个必要了
//OPSTAT FsmProcessingLaunchEntryBareRtos(UINT8 task_id)
//{
//  /*
//  ** Check the task_id
//  */
//  if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
//    return IHU_FAILURE;
//  }

//  /*
//  ** Run each task entry API, give each task a chance to init any specific information
//  */
//  if (zIhuFsmTable.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc != NULL){
//    (zIhuFsmTable.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc)(task_id, 0, NULL, 0);
//  }else{
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Task (ID=%d) get no init entry fetched!\n", task_id);
//  }

//  return IHU_SUCCESS;
//}

//OPSTAT FsmProcessingLaunchExecuteBareRtos(UINT8 task_id)
//{
//  OPSTAT ret;
//  IhuMsgSruct_t rcv;

//  /*
//  ** Check the task_id
//  */
//  if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
//    return IHU_FAILURE;
//  }

//  //任务中的消息轮循
//  //从当前TASKID对应的消息队列读取消息
//  //每一次循环，只从消息队列中读取一个消息并进行执行，这相当于所有任务的优先级都是均等的，不存在优先级高的问题
//  //如果消息过多，可能会造成消息队列的溢出，这里没有考虑消息队列的共享问题，未来可以考虑继续优化
//  //小喇叭响起来，这里是资源优化集中营！
//  memset(&rcv, 0, sizeof(IhuMsgSruct_t));
//  ret = ihu_message_rcv(task_id, &rcv);
//  if (ret == IHU_SUCCESS)
//  {
//    ret = FsmRunEngine(rcv.msgType, rcv.dest_id, rcv.src_id, rcv.msgBody, rcv.msgLen);
//    if (ret == IHU_FAILURE)
//    {
//      zIhuRunErrCnt[TASK_ID_VMFO]++;
//      IhuErrorPrint("VMFO: Error execute FsmRun state machine!\n");
//      return IHU_FAILURE;
//    }
//  }
//  else
//  {
//    //失败了，啥都不干，说明读取出错，或者消息队列是空的
//    //这意味着只是空转一下
//    //真正失败的时候，需要返回FAILURE

//    //这里是消息队列空，所以返回成功
//    return IHU_SUCCESS;
//  }

//  return IHU_SUCCESS;
//}

////Reveive message
////dest_id: in
////*msg: out
//OPSTAT ihu_message_rcv_bare_rtos(UINT8 dest_id, IhuMsgSruct_t *msg)
//{
//  int i=0, j=0;
//  bool Flag = FALSE;

//  //Checking task_id range
//  if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
//    return IHU_FAILURE;
//  }

//  //循环查找一个任务的消息队列，看看有无有效消息
//  for (i=0; i<MAX_QUEUE_NUM_IN_ONE_TASK; i++)
//  {
//    j = zIhuFsmTable.taskQue[dest_id].queIndex + i;
//    j = j%MAX_QUEUE_NUM_IN_ONE_TASK;
//    if (zIhuFsmTable.taskQue[dest_id].queList[j].useFlag == TRUE)
//    {
//      //找到了未消耗的消息
//      memcpy(msg, zIhuFsmTable.taskQue[dest_id].queList[j].msgQue, MAX_IHU_MSG_BODY_LENGTH);
//      zIhuFsmTable.taskQue[dest_id].queList[j].useFlag = FALSE;
//      //将INDEX指向下一个
//      j = (j+1) % MAX_QUEUE_NUM_IN_ONE_TASK;
//      zIhuFsmTable.taskQue[dest_id].queIndex = j;
//      //判断消息的合法性
//      if ((msg->dest_id <= TASK_ID_MIN) || (msg->dest_id >= TASK_ID_MAX)){
//        zIhuRunErrCnt[TASK_ID_VMFO]++;
//        IhuErrorPrint("VMFO: Receive message error on task_id, dest_id=%d!!!\n", msg->dest_id);
//        return IHU_FAILURE;
//      }
//      if ((msg->src_id <= TASK_ID_MIN) || (msg->src_id >= TASK_ID_MAX)){
//        zIhuRunErrCnt[TASK_ID_VMFO]++;
//        IhuErrorPrint("VMFO: Receive message error on src_id, dest_id=%d!!!\n", msg->src_id);
//        return IHU_FAILURE;
//      }
//      //不允许定义消息结构中长度为0的消息体，至少需要一个长度域
//      if ((msg->msgLen <= 0) || (msg->msgLen > MAX_IHU_MSG_BUF_LENGTH)){
//        zIhuRunErrCnt[TASK_ID_VMFO]++;
//        IhuErrorPrint("VMFO: Receive message error on length, msgLen=%d!!!\n", msg->msgLen);
//        return IHU_FAILURE;
//      }
//      if ((msg->msgType <= MSG_ID_COM_MIN) || (msg->msgType >= MSG_ID_COM_MAX)){
//        zIhuRunErrCnt[TASK_ID_VMFO]++;
//        IhuErrorPrint("VMFO: Receive message error on msgId, msgType=%d!!!\n", msg->msgType);
//        return IHU_FAILURE;
//      }
//      //退出循环
//      Flag = TRUE;
//      break;
//    }
//  }
//  //返回错误，并不一定表示真发生了错误，也有可能是没有任何消息供消耗，此时，上层调用者就啥都不做，确保自己安全
//  if (Flag == TRUE){
//    return IHU_SUCCESS;
//  }else{
//    return IHU_FAILURE;
//  }
//}

////message send basic processing
////All in parameters
//OPSTAT ihu_message_send_bare_rtos(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
//{
//  IhuMsgSruct_t msg;
//  int i=0, j=0;
//  bool Flag = FALSE;
//  char s1[TASK_NAME_MAX_LENGTH+2]="", s2[TASK_NAME_MAX_LENGTH+2]="", s3[MSG_NAME_MAX_LENGTH]="";

//  //入参检查Checking task_id range
//  if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
//    return IHU_FAILURE;
//  }
//  if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Error on task_id, src_id=%d!!!\n", src_id);
//    return IHU_FAILURE;
//  }
//  if (param_len>MAX_IHU_MSG_BODY_LENGTH){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
//    return IHU_FAILURE;
//  }
//  if ((msg_id <= MSG_ID_COM_MIN) || (msg_id >= MSG_ID_COM_MAX)){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Receive message error on msgId, msgType=%d!!!\n", msg_id);
//    return IHU_FAILURE;
//  }

//  //对来源数据进行初始化，是为了稳定可靠安全
//  memset(&msg, 0, sizeof(IhuMsgSruct_t));
//  msg.msgType = msg_id;
//  msg.dest_id = dest_id;
//  msg.src_id = src_id;
//  msg.msgLen = param_len;
//  memcpy(&(msg.msgBody[0]), param_ptr, param_len);


//  //然后送到目的地，循环查找一个任务的消息队列，看看有无空闲位置
//  for (i=0; i<MAX_QUEUE_NUM_IN_ONE_TASK; i++)
//  {
//    j = zIhuFsmTable.taskQue[dest_id].queIndex + i;
//    j = j%MAX_QUEUE_NUM_IN_ONE_TASK;
//    if (zIhuFsmTable.taskQue[dest_id].queList[j].useFlag == FALSE)
//    {
//      //找到了空闲的消息位置
//      memcpy(zIhuFsmTable.taskQue[dest_id].queList[j].msgQue, &msg, MAX_IHU_MSG_BODY_LENGTH);
//      zIhuFsmTable.taskQue[dest_id].queList[j].useFlag = TRUE;
//      //将INDEX指向下一个
//      j = (j+1) % MAX_QUEUE_NUM_IN_ONE_TASK;
//      zIhuFsmTable.taskQue[dest_id].queIndex = j;
//      //退出循环
//      Flag = TRUE;
//      break;
//    }
//  }
//  //错误，表示没有空闲队列
//  if (Flag == FALSE){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Message queue full, can not send into task = %d!!!\n", dest_id);
//    return IHU_FAILURE;
//  }

//  /*
//   *  Message Trace processing
//   *  注意字符串长度，太小会出现内存飞掉的情形，MessageTrace的数据配置来源于数据库，层次比这个还要高，虽然有点怪怪的，但不影响总体架构
//   *  最开始采用IFDEF的形式，后期完善后改为更为直接的代码方式
//   *  软件开发中，DEBUG和TRACE是两种最常用/最有用的调试模式，比单步还有用，这不仅是因为熟手不需要单步执行，而且也是因为多线程多进程单步执行环境制造的复杂性
//   *
//   *  有关MESSAGE TRACE的效率，是一个要注意的问题，当系统负载不高时，打开所有的TRACE是合适的，但一旦部署实际系统，TRACE需要减少到最低程度，这是原则，实际需要
//   *  维护人员根据情况灵活把我
//   *
//   *  本TRACE功能，提供了多种工作模式
//   *
//   */
//  switch (zIhuSysEngPar.traceMode)
//  {
//    case IHU_TRACE_MSG_MODE_OFF:
//      break;

//    case IHU_TRACE_MSG_MODE_INVALID:
//      break;

//    case IHU_TRACE_MSG_MODE_ALL:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      break;

//    case IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if (msg_id != MSG_ID_COM_TIME_OUT){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_ALL_BUT_HEART_BEAT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((msg_id != MSG_ID_COM_TIME_OUT) && (msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_MOUDLE_TO_ALLOW:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_MOUDLE_TO_RESTRICT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict!= TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_MOUDLE_FROM_ALLOW:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_MOUDLE_FROM_RESTRICT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict!= TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_MOUDLE_DOUBLE_ALLOW:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)
//          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_MOUDLE_DOUBLE_RESTRICT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict != TRUE)
//          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_MSGID_ALLOW:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_MSGID_RESTRICT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_COMBINE_TO_ALLOW:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
//          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_COMBINE_TO_RESTRICT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
//          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_COMBINE_FROM_ALLOW:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
//          && (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_COMBINE_FROM_RESTRICT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
//          && (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_COMBINE_DOUBLE_ALLOW:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
//          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)
//          && (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    case IHU_TRACE_MSG_MODE_COMBINE_DOUBLE_RESTRICT:
//      ihu_taskid_to_string(dest_id, s1);
//      ihu_taskid_to_string(src_id, s2);
//      ihu_msgid_to_string(msg_id, s3);
//      if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
//          && (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)
//          && (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
//        IhuDebugPrint("MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//      }
//      break;

//    default:
//      if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_NOR_ON) != FALSE){
//        zIhuRunErrCnt[TASK_ID_VMFO]++;
//        IhuErrorPrint("VMFO: System Engineering Parameter Trace Mode setting error! DebugMode=%d.\n", zIhuSysEngPar.debugMode);
//      }
//      break;
//  }

//  return IHU_SUCCESS;
//}


////为了对付不存在OS的情形
//OPSTAT ihu_system_task_execute_bare_rtos(UINT8 task_id, FsmStateItem_t *p)
//{
//  int ret = 0;
//  char strDebug[IHU_PRINT_CHAR_SIZE];

//  //Checking task_id range
//  if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    sprintf(strDebug, "VMFO: Input Error on task_id, task_id=%d!!!\n", task_id);
//    IhuErrorPrint(strDebug);
//    return IHU_FAILURE;
//  }

//  //入参检查
//  if (p == NULL){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    IhuErrorPrint("VMFO: Input wrong FsmStateItem pointer!\n");
//    return IHU_FAILURE;
//  }

//  //任务控制启动标示检查
//  if (zIhuTaskInfo[task_id].pnpState != IHU_TASK_PNP_ON){
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    sprintf(strDebug, "VMFO: no need execute this task [%s]!\n", zIhuTaskNameList[task_id]);
//    IhuErrorPrint(strDebug);
//    return IHU_FAILURE;
//  }

//  //轮询执行任务的消息消耗
//  ret = FsmProcessingLaunchExecuteBareRtos(task_id);
//  if (ret == IHU_FAILURE)
//  {
//    zIhuRunErrCnt[TASK_ID_VMFO]++;
//    sprintf(strDebug, "VMFO: Execute task FSM un-successfully, taskid = %d", task_id);
//    IhuErrorPrint(strDebug);
//    return IHU_FAILURE;
//  }

//  //正确返回
//  return IHU_SUCCESS;
//}

////轮询所有任务的正常执行
//void ihu_task_execute_all_bare_rtos(void)
//{
//  #ifdef IHU_COMPILE_TASK_VMFO
//    //Execute task VMFO FSM /1
//    if (zIhuTaskInfo[TASK_ID_VMFO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_VMFO, FsmVMFO);
//  #endif //IHU_COMPILE_TASK_VMFO

//  #ifdef IHU_COMPILE_TASK_TIMER
//    //Execute task Timer FSM /2
//    if (zIhuTaskInfo[TASK_ID_TIMER].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_TIMER, FsmTimer);
//  #endif //IHU_COMPILE_TASK_TIMER

////  #ifdef IHU_COMPILE_TASK_ASYLIBRA
////    //Execute task ASYLIBRA FSM /3
////    if (zIhuTaskInfo[TASK_ID_ASYLIBRA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_ASYLIBRA, FsmAsylibra);
////  #endif //IHU_COMPILE_TASK_ASYLIBRA
////
////  #ifdef IHU_COMPILE_TASK_ADCARIES
////    //Execute task ADCARIES FSM /4
////    if (zIhuTaskInfo[TASK_ID_ADCARIES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_ADCARIES, FsmAdcaries);
////  #endif //IHU_COMPILE_TASK_ADCARIES
////
////  #ifdef IHU_COMPILE_TASK_EMC
////    //Execute task EMC FSM /5
////    if (zIhuTaskInfo[TASK_ID_EMC].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_EMC, FsmEmc);
////  #endif //IHU_COMPILE_TASK_EMC

//  IhuDebugPrint("VMFO: Execute task once, for test!\n");
//}

//安装ISR服务程序，目前暂时找不到合适的ucosIII对应的方式，待定
//采用ISR方式后，硬件的轮询将变得不重要，都采用ISR方式，将更加干净、高效，完全是最正常的方式
OPSTAT ihu_isr_install(UINT8 priority, void *my_routine)
{

        return IHU_SUCCESS;
}

/**********************************************************************************
 *
 *   MAIN函数入口，供应上层使用
 *
 **********************************************************************************/
int ihu_vm_main(void) 
{
	//硬件初始化
	ihu_vm_system_init();

	//从数据库或者系统缺省配置中，读取系统配置的工程参数
	//省略

	//启动所有任务
  ihu_task_create_all();

	//wait for ever，由于FreeRTOS需要vTaskStartScheduler()以后才能执行，所以这里不能阻塞
//	while (1){
//		ihu_sleep(60); //可以设置为5秒的定时，甚至更长
//		ihu_vm_check_task_que_status_and_action();
//	}

	//清理现场环境，永远到达不了，清掉以消除COMPILE WARNING
	//ihu_task_delete_all_and_queue();

	//永远到达不了，清掉以消除COMPILE WARNING
	return EXIT_SUCCESS;
}

//TBD
void ihu_vm_check_task_que_status_and_action(void)
{
	return;
}

OPSTAT ihu_vm_send_init_msg_to_app_task(UINT8 dest_id)
{
	int ret = 0;
	msg_struct_com_init_t snd;
	
	//入参检查
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return IHU_FAILURE;
	}
		
	//发送初始化消息给目标任务，以便初始化所有任务模块
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = ihu_message_send(MSG_ID_COM_INIT, dest_id, TASK_ID_VMFO, &snd, snd.length);
	if (ret == IHU_FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_VMFO], zIhuTaskNameList[dest_id]);
	}
	
	return ret;
}

