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

//全局SLEEP时长控制表
UINT32 zIhuSleepCnt[MAX_TASK_NUM_IN_ONE_IHU][MAX_SLEEP_NUM_IN_ONE_TASK];

//全局时间
time_t zIhuSystemTimeUnix = 1444341556;  //2015/8
struct tm zIhuSystemTimeYmd;

//全局公用打印字符串
static INT8 strGlobalPrintChar[IHU_PRINT_CHAR_SIZE];

//请确保，该全局字符串的定义跟Task_Id的顺序保持完全一致，不然后面的显示内容会出现差错
//请服从最长长度TASK_NAME_MAX_LENGTH的定义，不然Debug/Trace打印出的信息也会出错
//全局变量：任务打印命名
//从极致优化内存的角度，这里浪费了一个TASK对应的内存空间（MIN=0)，但它却极大的改善了程序编写的效率，值得浪费！！！
char *zIhuTaskNameList[MAX_TASK_NUM_IN_ONE_IHU] ={
	"MIN", 
	"VMFO",
	"TIMER",
	"ADCLIBRA",
	"SPILEO",
	"I2CARIES",
	"PWMTAURUS",
	"SPSVIRGO",
	"GPIOCANCER",
	"DIDOCAP",
	"LEDPISCES",
	"ETHORION",	
	"EMC68X",
	"MAX"};

//消息ID的定义全局表，方便TRACE函数使用
//请服从MSG_NAME_MAX_LENGTH的最长定义，不然出错
//全局变量：消息打印命名
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

/*******************************************************************************
**
**	全局公用的API函数，不依赖于任何操作系统，只依赖于POSIX支持的标准函数集
**
**********************************************************************************/
//正常打印
void IhuDebugPrint(char *format, ...)
{
	va_list marker;
	char strDebug[IHU_PRINT_CHAR_SIZE-50];	
	
	va_start(marker, format );
	vsprintf(strDebug, format, marker);
	
	sprintf((char *)strGlobalPrintChar, "[DBG: %s, %s] ", __DATE__, __TIME__);
	strcat((char *)strGlobalPrintChar, strDebug);
		
	// The trace is limited to 1000 characters.
	if( (strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-2] != 0) && (strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-1] != 0) )
	{
		strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-3] = '!';
		strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-2] = '\n';
		strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-1] = '\0';
	}

	printf("%s", strGlobalPrintChar);
	strGlobalPrintChar[0] = '\0';

	//Reset variable arguments.
	va_end(marker);	
}

//错误打印
void IhuErrorPrint(char *format, ...)
{
	va_list marker;
	char strDebug[IHU_PRINT_CHAR_SIZE-50];	
	
	va_start(marker, format );
	vsprintf(strDebug, format, marker);
	
	sprintf((char *)strGlobalPrintChar, "[ERR: %s, %s] ", __DATE__, __TIME__);
	strcat((char *)strGlobalPrintChar, strDebug);
		
	// The trace is limited to 1000 characters.
	if( (strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-2] != 0) && (strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-1] != 0) )
	{
		strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-3] = '!';
		strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-2] = '\n';
		strGlobalPrintChar[IHU_PRINT_CHAR_SIZE-1] = '\0';
	}

	printf("%s", strGlobalPrintChar);
	strGlobalPrintChar[0] = '\0';

	//Reset variable arguments.
	va_end(marker);
}

//交换U16的高位低位字节
uint16_t b2l_uint16(uint16_t in)
{
		return ( ((in & 0x00FF) << 8) | (in >> 8) );
}

//API abstract
//通过时钟问题的解决，这个问题终于解决了，原因就是SLEEP和SIGALM公用同一套信号量，导致相互冲突。时钟采用线程方式后，再也没有问题了
void ihu_sleep(UINT32 second)
{
	if (second <= (UINT32)0) second =0;
	//second = sleep(second*1000);
	while (second>0)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			//太多的错误，未来需要再研究这个错误出现的原因，这里留下一点点报告的可行性
				IhuDebugPrint("VMFO: Sleep interrupt by other higher level system call, remaining %d second to be executed\n", second);
		}
		//second = sleep(second);
	}
}

void ihu_usleep(UINT32 usecond)
{
	if (usecond <= 0) usecond =0;
	//usecond = usleep(usecond);
	while (usecond>0)
	{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuErrorPrint("VMFO: uSleep interrupt by other higher level system call, remaining %d usecond to be executed\n", usecond);
		}
		//usecond = usleep(usecond);
	}
}

//INIT the whole system
void ihu_vm_system_init(void)
{
	int i=0;

	//INIT IHU itself
	IhuDebugPrint("VMFO: CURRENT_PRJ=[%s], HW_TYPE=[%d], HW_MODULE=[%d], SW_REL=[%d], SW_DELIVER=[%d].\n", IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT, CURRENT_HW_TYPE, CURRENT_HW_MODULE, CURRENT_SW_RELEASE, CURRENT_SW_DELIVERY);
	IhuDebugPrint("VMFO: BXXH(TM) IHU(c) Application Layer start and initialized, build at %s, %s.\n", __DATE__, __TIME__);

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
	zIhuSysEngPar.timer.emcReqTimer = IHU_EMC_TIMER_DURATION_PERIOD_READ;

	//Series Port config
	zIhuSysEngPar.serialport.SeriesPortForGPS = IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT;

	//后台部分
	strcpy(zIhuSysEngPar.cloud.cloudHttpAddLocal, IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL);

	//for IHU SW FTP
	strcpy(zIhuSysEngPar.cloud.cloudFtpAdd, IHU_CLOUDXHUI_FTP_ADDRESS);

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
	zIhuTaskInfo[TASK_ID_VMFO].pnpState = IHU_TASK_PNP_OFF;
	zIhuTaskInfo[TASK_ID_TIMER].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_ADC == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_ADCLIBRA].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_SPI == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_SPILEO].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_I2C == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_I2CARIES].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_PWM == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_PWMTAURUS].pnpState = IHU_TASK_PNP_ON;
	if (IHU_COMM_FRONT_SPS == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_SPSVIRGO].pnpState = IHU_TASK_PNP_ON;	
	if (IHU_COMM_FRONT_GPIO == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_GPIOCANCER].pnpState = IHU_TASK_PNP_ON;	
	if (IHU_COMM_FRONT_DIDO == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_DIDOCAP].pnpState = IHU_TASK_PNP_ON;	
	if (IHU_COMM_FRONT_LED == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_LEDPISCES].pnpState = IHU_TASK_PNP_ON;		
	if (IHU_COMM_FRONT_ETH == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_ETHORION].pnpState = IHU_TASK_PNP_ON;		
	if (IHU_MAIN_CTRL_EMC68X == IHU_TASK_PNP_ON) zIhuTaskInfo[TASK_ID_EMC68X].pnpState = IHU_TASK_PNP_ON;			
	
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
		return FAILURE;
	}

	strcpy(string, "[");
	if (strlen(zIhuTaskNameList[id])>0){
		strncpy(tmp, zIhuTaskNameList[id], TASK_NAME_MAX_LENGTH-3);
		strcat(string, tmp);
	}else{
		strcat(string, "TASK_ID_XXX");
	}
	strcat(string, "]");
	return SUCCESS;
}

//MsgId transfer to string
//输入的string参数，其内存地址空间和长度预留，是否足够
OPSTAT ihu_msgid_to_string(UINT16 id, char *string)
{
	char tmp[MSG_NAME_MAX_LENGTH-2]="";
	
	if (id <= MSG_ID_COM_MIN || id >= MSG_ID_COM_MAX){
		zIhuRunErrCnt[TASK_ID_VMFO]++;	
		IhuErrorPrint("VMFO: Error Message Id input!\n");
		return FAILURE;
	}

	strcpy(string, "[");
	if (strlen(zIhuMsgNameList[id-MSG_ID_COM_MIN])>0){
		strncpy(tmp, zIhuMsgNameList[id-MSG_ID_COM_MIN], MSG_NAME_MAX_LENGTH-3);
		strcat(string, tmp);
	}else{
		strcat(string, "MSG_ID_XXX");
	}
	strcat(string, "]");

	return SUCCESS;
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
** Return value : SUCCESS OR FAILURE
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
    return SUCCESS;
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
** Return value : SUCCESS OR FAILURE
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
		return FAILURE;
	}
	if((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX))
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The task_ID is invalid.\n");
		return FAILURE;
	}
	if( zIhuFsmTable.pFsmCtrlTable[task_id].taskId != TASK_ID_INVALID )
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: This task_id has been already inited.\n");
		return FAILURE;
	}

	/*
	** Check the first entry. 最初一个FSM表单必须以FSM_STATE_ENTRY开始
	*/
	if( pFsmStateItem[FSM_STATE_ENTRY].state != FSM_STATE_ENTRY )
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The format of the FSM table is invalid.\n");
		return FAILURE;
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
			return FAILURE;
		}
		if( NULL == pFsmStateItem[item].stateFunc)
		{
			zIhuRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: Invalid state function pointer.\n");
			return FAILURE;
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
		return FAILURE;
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
			return FAILURE;
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
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error set FSM State!\n");
	}

    return SUCCESS;
}

OPSTAT FsmRemove(UINT8 task_id)
{
	//入参检查
	if((task_id <= (UINT8)TASK_ID_MIN) || (task_id >= TASK_ID_MAX))
	{
		IhuErrorPrint("VMFO: The task_ID is invalid.\n");
		return FAILURE;
	}

	//设置无效
	zIhuFsmTable.pFsmCtrlTable[task_id].taskId = TASK_ID_INVALID;

    return SUCCESS;
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
** Return value : SUCCESS OR FAILURE
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
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, src_id=%d!!!\n", src_id);
		return FAILURE;
	}
	if (param_len>MAX_IHU_MSG_BODY_LENGTH){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
		return FAILURE;
	}
	if ((msg_id <= MSG_ID_COM_MIN) || (msg_id >= MSG_ID_COM_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on Msg_Id, msg_id=%d!!!\n", msg_id);
		return FAILURE;
	}

	/*
	** Search for the dest_id task state
	*/

	if( zIhuFsmTable.pFsmCtrlTable[dest_id].taskId != dest_id )
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: The destination process does not exist.\n");
		return FAILURE;
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
		return FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_IPT_ON) != FALSE)
	{
		IhuDebugPrint("VMFO: Call state function(0x%x) in state(%d) of task(0x%x) for msg(0x%x).\n", (UINT32)zIhuFsmTable.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc, state, dest_id, mid);
	}

	/*
	** Call the state function.
	*/
	if(zIhuFsmTable.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc != NULL)
	{
		ret = (zIhuFsmTable.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc)(dest_id, src_id, param_ptr, param_len);
		if( FAILURE == ret)
		{
			zIhuRunErrCnt[TASK_ID_VMFO]++;			
			IhuErrorPrint("VMFO: Internal error is found in the state function.\n");
			return FAILURE;
		}
	}
	else
	{
		if(NULL != param_ptr)
		{
			//Free memory, here do nothing.
		}
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Receive invalid msg(%x)[%s] in state(%d) of task(0x%x)[%s].\n", mid, zIhuMsgNameList[mid], state, dest_id, zIhuTaskNameList[dest_id]);
		return FAILURE;
	}

    return SUCCESS;
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
** Return value : SUCCESS OR FAILURE
*******************************************************************************/
OPSTAT FsmProcessingLaunch(void)
{
	OPSTAT ret;
	IhuMsgSruct_t rcv;
	UINT8 task_id = 0; //Get current working task_id
	
	/*
	** Check the task_id
	*/
	task_id = zIhuFsmTable.currentTaskId;
	zIhuFsmTable.currentTaskId = TASK_ID_INVALID;
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
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
		//纯粹是为了消除最后返回RETURN的WARNING告警问题
		if (ret == -3){
			break;
		}
		//留一个rcv函数返回的口子，以便继续执行
		else if (ret == -2){
			continue;
		}
		else if (ret == FAILURE){
			IhuDebugPrint("VMFO: STM kernal receive message error, hold for further action!\n");
			//To be defined here the error case, to recover everything
			ihu_sleep(1); 
		}else{
			ret = FsmRunEngine(rcv.msgType, rcv.dest_id, rcv.src_id, rcv.msgBody, rcv.msgLen);
			if (ret == FAILURE){
				zIhuRunErrCnt[TASK_ID_VMFO]++;	
				IhuErrorPrint("VMFO: Error execute FsmRun state machine!\n");
			}
		}
	}//While(1)
	
	return SUCCESS;
}


//设置状态
OPSTAT FsmSetState(UINT8 task_id, UINT8 newState)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: FsmSetState error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Checking newState range
	if (newState > MAX_STATE_NUM_IN_ONE_TASK){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: FsmSetState error on state, State=%d!!!\n", newState);
		return FAILURE;
	}

	zIhuTaskInfo[task_id].state = newState;
	return SUCCESS;
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
	int msgKey=0, msgQid=0;
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Generate msgKey
	msgKey = task_id + IHU_TASK_QUEUE_ID_START;
	if ((msgKey <= TASK_QUE_ID_MIN) || (msgKey >= TASK_QUE_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, msgKey=%d!!!\n", msgKey);
		return FAILURE;
	}
	//Checking msgQid exiting or not, if YES, just DELETE.
//	msgQid=msgget(msgKey, IPC_EXCL);  /*检查消息队列是否存在*/
	if (msgQid>=0){
		//IhuErrorPrint("VMFO: Existing Queue id error, input Key = %d\n", msgKey);
		//直接使用已有的QUEID，而不是重新创建
		//zIhuTaskInfo[task_id].QueId = msgQid;
		//return SUCCESS;
		//删除消息队列
//		if (msgctl(msgQid, IPC_RMID, NULL) == FAILURE){
//			zIhuRunErrCnt[TASK_ID_VMFO]++;
//			IhuErrorPrint("VMFO: Remove Queue error!\n");
//			return FAILURE;
//		}
		//IhuErrorPrint("VMFO: Already remove Queue Id, input Key = %d\n", msgKey);
	}
	//Re-create the relevant message queue Id
//	msgQid = msgget(msgKey, IPC_CREAT|0666);/*创建消息队列*/
	if(msgQid <0){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Failed to create msg-queue | errno=%d [%s]\n", errno, strerror(errno));
		return FAILURE;
	}
	zIhuTaskInfo[task_id].QueId = msgQid;
	return SUCCESS;
}

//kill message queue
OPSTAT ihu_message_queue_delete(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Not exist
	//特殊情况下，msgqid=0也是可能的，这种情形，需要再仔细考虑
	if (zIhuTaskInfo[task_id].QueId == 0) {return FAILURE;}

	//Delete queue
//	msgctl(zIhuTaskInfo[task_id].QueId, IPC_RMID, NULL); //删除消息队列

	zIhuTaskInfo[task_id].QueId =0;
	return SUCCESS;
}

//查询消息队列
UINT32 ihu_message_queue_inquery(UINT8 task_id)
{
	int msgKey=0, msgQid=0;
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return 0;
	}

	//Generate msgKey
	msgKey = task_id + IHU_TASK_QUEUE_ID_START;
	if ((msgKey <= TASK_QUE_ID_MIN) || (msgKey >= TASK_QUE_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, msgKey=%d!!!\n", msgKey);
		return 0;
	}

//	msgQid=msgget(msgKey, IPC_EXCL);  /*检查消息队列是否存在*/

	return msgQid;
}

//这个过程用在同步不同进程之间的消息队列，确保不同进程之间的任务可以互发消息
//如果队列本来就不存在，就不用同步或者重新填入全局控制表了
OPSTAT ihu_message_queue_resync(void)
{
	UINT8 task_id = 0;
	int msgKey=0, msgQid=0;
	for (task_id = TASK_ID_MIN; task_id < TASK_ID_MAX; task_id++){
		//Generate msgKey
		msgKey = task_id + IHU_TASK_QUEUE_ID_START;
		if ((msgKey <= TASK_QUE_ID_MIN) || (msgKey >= TASK_QUE_ID_MAX)){
			zIhuRunErrCnt[TASK_ID_VMFO]++;
			IhuErrorPrint("VMFO: Error on task_id, msgKey=%d!!!\n", msgKey);
			return FAILURE;
		}
		//Checking msgQid exiting or not
		//这里，不在本进程中的任务模块，只有最为基本的消息队列和Task_ID，其它信息一律不存在
//		msgQid=msgget(msgKey, IPC_EXCL);  /*检查消息队列是否存在*/
		if (msgQid>=0){
			zIhuTaskInfo[task_id].QueId = msgQid;
			zIhuTaskInfo[task_id].TaskId = task_id;
		}
	}//For loop

	return SUCCESS;
}

//清理某一个任务的消息队列
OPSTAT ihu_message_queue_clean(UINT8 dest_id)
{
	//入参检查
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}	
	//清理消息队列
	memset(&zIhuFsmTable.taskQue[dest_id], 0, sizeof(FsmQueueListTable_t));
	
	return SUCCESS;
}


//message send basic processing
//All in parameters
OPSTAT ihu_message_send(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	int ret = 0;
	char s1[TASK_NAME_MAX_LENGTH+2]="", s2[TASK_NAME_MAX_LENGTH+2]="", s3[MSG_NAME_MAX_LENGTH]="";
	IhuMsgSruct_t *msg;
	
	//Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, src_id=%d!!!\n", src_id);
		return FAILURE;
	}
	if (param_len > MAX_IHU_MSG_BODY_LENGTH){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
		return FAILURE;
	}

	//Starting to process message send
	//Not use malloc/free function, as it is a little bit complex to manage, to be checked here the memory conflict
	msg = malloc(sizeof(IhuMsgSruct_t));
	if (msg == NULL){
		IhuErrorPrint("VMFO: Message send allocate memory error, exit!\n");
		return FAILURE;
	}
	//Init to clean this memory area
	//初始化消息内容，是为了稳定可靠安全
	memset(msg, 0, sizeof(IhuMsgSruct_t));
	msg->msgType = msg_id;
	msg->dest_id = dest_id;
	msg->src_id = src_id;
	msg->msgLen = param_len;
	memcpy(&(msg->msgBody[0]), param_ptr, param_len);
	//msg->msgBody是否可以直接当指针？

//	ret = msgsnd(ihu_msgque_inquery(dest_id), msg, (sizeof(IhuMsgSruct_t)-sizeof(long)), IPC_NOWAIT);
//	free(msg);
	if ( ret < 0 ) {
		IhuErrorPrint("VMFO: msgsnd() write msg failed, errno=%d[%s], dest_id = %d [%s]\n",errno,strerror(errno), dest_id, zIhuTaskNameList[dest_id]);
		zIhuTaskInfo[dest_id].QueFullFlag = IHU_TASK_QUEUE_FULL_TRUE;
		return FAILURE;
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

	return SUCCESS;
}


//Reveive message
//dest_id: in
//*msg: out
// ret: 接受消息中的errno=4/EINTR需要单独处理，不然会出现被SIGALRM打断的情形
OPSTAT ihu_message_rcv(UINT8 dest_id, IhuMsgSruct_t *msg)
{
	int ret = 0;
	
	//Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}

	//ret = msgrcv(zIhuTaskInfo[dest_id].QueId, msg, (sizeof(IhuMsgSruct_t) - sizeof(long)), 0, 0);
//	ret = msgrcv(ihu_msgque_inquery(dest_id), msg, (sizeof(IhuMsgSruct_t) - sizeof(long)), 0, 0);
//	if (errno == EINTR) //EINTR, system interrupt call error
//		return EINTR;
	if ( ret < 0 ) {
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: msgrcv() receive msg failed, Qid=%d, msg=%08X, errno=%d[%s]\n", zIhuTaskInfo[dest_id].QueId, msg, errno, strerror(errno));
		return FAILURE;
	}
	return SUCCESS;
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
//	pthread_attr_t          attr;
//	struct  sched_param     sched;

	// The thread is initialized with attributes object default values
//	if(0!= (err = pthread_attr_init(&attr)))
//	{
//		IhuDebugPrint("VMFO: pthread_attr_init() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
//		return FAILURE;
//	}

	// Change the inherit scheduling attribute
	// of the specified thread attribute object,because PTHREAD_INHERIT_SCHED is default value
//	if (0!= (err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)))
//	{
//		IhuDebugPrint("VMFO: pthread_attr_setinheritsched() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
//		return FAILURE;
//	}

	// Set the contention scope attribute of
	// the specified thread attributes object, the thread compete with all threads on the station
//	if (0 != (err = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM)))
//	{
//		IhuDebugPrint("VMFO: pthread_attr_setscope() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
//		return FAILURE;
//	}

	// Set the scheduling policy,SCHED_OTHER is the default
//	if (0 != (err = pthread_attr_setschedpolicy(&attr, SCHED_FIFO)))
//	{
//		IhuDebugPrint("VMFO: pthread_attr_setschedpolicy() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
//		return FAILURE;
//	}

	// Set the priority of the task
//	sched.sched_priority = prio;
//	if (0 != (err = pthread_attr_setschedparam(&attr, &sched)))
//	{
//		IhuErrorPrint("VMFO: pthread_attr_setschedparam() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
//		return FAILURE;
//	}

	// Checking task_id
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id!! err=%d, errno=%d, %s, task_id=%d\n", err, errno, strerror(err), task_id);
		return FAILURE;
	}

	//As FsmProcessLaunch has to transfer task_id in, we will use global variance of zIhuFsmTable.currentTaskId
	//So task_id has to store into there
	//Wonderful mechanism!!!
	if (zIhuFsmTable.currentTaskId != TASK_ID_INVALID){
		ihu_sleep(1);
	}
	if (zIhuFsmTable.currentTaskId != TASK_ID_INVALID){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Task_id not yet read by running process, new task create failure!\n");
		return FAILURE;
	}
	zIhuFsmTable.currentTaskId = task_id;

	// creation of the task
//	err=pthread_create(&(zIhuTaskInfo[task_id].ThrId), &attr, (void *(*)(void*))(task_func), (void*)arg);
	if(err != 0)
	{
		IhuDebugPrint("VMFO: pthread_create() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
		return FAILURE;
	}

	//zIhuTaskInfo[task_id].TaskName to be added in another way, TBD
	zIhuTaskInfo[task_id].TaskId = task_id;
//	zIhuTaskInfo[task_id].processId = getpid(); //进程号存入
	IhuDebugPrint("VMFO: pthread_create() OK ...\n");
	/* ERRORS
	       EAGAIN Insufficient resources to create another thread, or a system-imposed limit on the number of threads was encountered.  The latter case may occur in two ways: the RLIMIT_NPROC soft  resource  limit
	              (set via setrlimit(2)), which limits the number of process for a real user ID, was reached; or the kernel's system-wide limit on the number of threads, /proc/sys/kernel/threads-max, was reached.

	       EINVAL Invalid settings in attr.

	       EPERM  No permission to set the scheduling policy and parameters specified in attr.
	 */
	return SUCCESS;
}

//kill task
//有关进程与线程方面的杀死清理，未来需要再完善
OPSTAT ihu_task_delete(UINT8 task_id)
{
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Not exist
//	if (zIhuTaskInfo[task_id].ThrId ==0){return FAILURE;}

	//只是清理掉FSM中的信息
	FsmRemove(task_id);

	//以下KILL线程会出错，未来待研究验证
	/*
	//SIGKILL用于杀死整个进程, 返回为ESRCH则表示线程不存在
	if (pthread_kill(zIhuTaskInfo[task_id].ThrId, 0) != (EINVAL || ESRCH)) {
		pthread_kill(zIhuTaskInfo[task_id].ThrId, SIGQUIT);
		return SUCCESS;
	}else{
		return FAILURE;
	}
	*/
	return SUCCESS;
}

//聚合创建任务，消息队列，并直接让其开始运行
OPSTAT ihu_task_create_and_run(UINT8 task_id, FsmStateItem_t* pFsmStateItem)
{
	OPSTAT ret = 0;
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Init Fsm table
	ret = FsmAddNew(task_id, pFsmStateItem);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Init state machine FsmAddNew error, taskid = %d.\n", task_id);
		return FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_CRT_ON) != FALSE){
		IhuDebugPrint("VMFO: FsmAddNew Successful, taskId = 0x%x [%s].\n", task_id, zIhuTaskNameList[task_id]);
	}

	//Create Queid
	ret = ihu_message_queue_clean(task_id);
	if (ret == FAILURE)
	{
	zIhuRunErrCnt[TASK_ID_VMFO]++;
	IhuErrorPrint("VMFO: Create queue unsuccessfully, taskId = %d.\n", task_id);
	return FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_CRT_ON) != FALSE){
		IhuDebugPrint("VMFO: Msgque create successful, taskId = 0x%x [%s].\n", task_id, zIhuTaskNameList[task_id]);
	}

	//Create task and make it running for the 1st time
	ret = ihu_task_create(task_id, /*CALLBACK*/ (void *(*)(void *))FsmProcessingLaunch, (void *)NULL, IHU_THREAD_PRIO);
	if (ret == FAILURE)
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Create task un-successfully, taskid = %d.\n", task_id);		
		return FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_CRT_ON) != FALSE){
	  IhuDebugPrint("VMFO: Task create Successful, taskId = 0x%x [%s].\n", task_id, zIhuTaskNameList[task_id]);
	}
	
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_IPT_ON) != FALSE){
		IhuDebugPrint("VMFO: Whole task environment setup successful, taskId = 0x%x [%s].\n", task_id, zIhuTaskNameList[task_id]);
	}
	
	//给该任务设置一个软的状态：省略了
	
	return SUCCESS;
}

//初始化任务并启动
OPSTAT ihu_system_task_init_call(UINT8 task_id, FsmStateItem_t *p)
{
	int ret = 0;
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}
	
	//入参检查
	if (p == NULL){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input wrong FsmStateItem pointer!\n");
		return FAILURE;
	}
	
	//任务控制启动标示检查
	if (zIhuTaskInfo[task_id].pnpState != IHU_TASK_PNP_ON){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: no need create this task [%s]!\n", zIhuTaskNameList[task_id]);	
		return FAILURE;
	}
	
	//打印信息
	if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_CRT_ON) != FALSE){
		IhuDebugPrint("VMFO: Staring to create task [%s] related environments...\n", zIhuTaskNameList[task_id]);
	}

	//任务创建并初始化所有状态机
	ret = ihu_task_create_and_run(task_id, p);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: create task env [%s] un-successfully, program exit.\n", zIhuTaskNameList[task_id]);
		return FAILURE;
	}else{
		if ((zIhuSysEngPar.debugMode & IHU_TRACE_DEBUG_INF_ON) != FALSE){
			IhuDebugPrint("VMFO: create task successfully, taskid=%d[%s].\n", task_id, zIhuTaskNameList[task_id]);
		}
	}
	
	//将该任务执行时对应的状态转移机指针存入，以供以后扫描时使用
	zIhuTaskInfo[task_id].fsmPtr = p;
	
	//正确返回
	return SUCCESS;
}

//创建所有任务
void ihu_task_create_all(void)
{
	//No need create VMFO environments /1
	//if (zIhuTaskInfo[TASK_ID_VMFO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_VMFO, FsmVMFO);	

	//Create task Timer environments /2
	if (zIhuTaskInfo[TASK_ID_TIMER].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_TIMER, FsmTimer);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_TIMER);

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
	if (zIhuTaskInfo[TASK_ID_PWMTAURUS].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_PWMTAURUS, FsmPwmtaurus);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_PWMTAURUS);	

	//Create task SPSVIRGO environments /7
	if (zIhuTaskInfo[TASK_ID_SPSVIRGO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_SPSVIRGO, FsmSpsvirgo);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_SPSVIRGO);	
	
	//Create task GPIOCANCER environments /8
	if (zIhuTaskInfo[TASK_ID_GPIOCANCER].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_GPIOCANCER, FsmGpiocancer);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_GPIOCANCER);	
	
	//Create task DIDOCAP environments /9
	if (zIhuTaskInfo[TASK_ID_DIDOCAP].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_DIDOCAP, FsmDidocap);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_DIDOCAP);
	
	//Create task LEDPISCES environments /10
	if (zIhuTaskInfo[TASK_ID_LEDPISCES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_LEDPISCES, FsmLedpisces);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_LEDPISCES);

	//Create task ETHORION environments /11
	if (zIhuTaskInfo[TASK_ID_ETHORION].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_ETHORION, FsmEthorion);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_ETHORION);
	
	//Create task EMC68X environments /12
	if (zIhuTaskInfo[TASK_ID_EMC68X].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_EMC68X, FsmEmc68x);
	ihu_vm_send_init_msg_to_app_task(TASK_ID_EMC68X);	
	
	IhuDebugPrint("VMFO: Create all task successfully!\n");
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
	return SUCCESS;
}

//Unix Time transfer to YMD time
struct tm ihu_clock_unix_to_ymd(time_t t_unix)
{
		return *localtime(&t_unix);
}


/*******************************************************************************
**
**	没有RTOS的情形
**
**********************************************************************************/
//这里将LaunchEntry和LaunchExecute是因为在VMDA环境中，创造环境和执行不在一起
//具有真正任务的操作系统环境中，没有这个必要了
OPSTAT FsmProcessingLaunchEntryBareRtos(UINT8 task_id)
{
	/*
	** Check the task_id
	*/
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
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
	
	return SUCCESS;
}

OPSTAT FsmProcessingLaunchExecuteBareRtos(UINT8 task_id)
{
	OPSTAT ret;
	IhuMsgSruct_t rcv;

	/*
	** Check the task_id
	*/
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//任务中的消息轮循
	//从当前TASKID对应的消息队列读取消息
	//每一次循环，只从消息队列中读取一个消息并进行执行，这相当于所有任务的优先级都是均等的，不存在优先级高的问题
	//如果消息过多，可能会造成消息队列的溢出，这里没有考虑消息队列的共享问题，未来可以考虑继续优化
	//小喇叭响起来，这里是资源优化集中营！
	memset(&rcv, 0, sizeof(IhuMsgSruct_t));
	ret = ihu_message_rcv(task_id, &rcv);
	if (ret == SUCCESS)
	{
		ret = FsmRunEngine(rcv.msgType, rcv.dest_id, rcv.src_id, rcv.msgBody, rcv.msgLen);
		if (ret == FAILURE)
		{
			zIhuRunErrCnt[TASK_ID_VMFO]++;	
			IhuErrorPrint("VMFO: Error execute FsmRun state machine!\n");
			return FAILURE;
		}
	}
	else
	{
		//失败了，啥都不干，说明读取出错，或者消息队列是空的
		//这意味着只是空转一下
		//真正失败的时候，需要返回FAILURE
		
		//这里是消息队列空，所以返回成功
		return SUCCESS;
	}
	
	return SUCCESS;
}

//Reveive message
//dest_id: in
//*msg: out
OPSTAT ihu_message_rcv_bare_rtos(UINT8 dest_id, IhuMsgSruct_t *msg)
{
	int i=0, j=0;
	bool Flag = FALSE;
	
	//Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}
	
	//循环查找一个任务的消息队列，看看有无有效消息
	for (i=0; i<MAX_QUEUE_NUM_IN_ONE_TASK; i++)
	{
		j = zIhuFsmTable.taskQue[dest_id].queIndex + i;
		j = j%MAX_QUEUE_NUM_IN_ONE_TASK;
		if (zIhuFsmTable.taskQue[dest_id].queList[j].useFlag == TRUE)
		{
			//找到了未消耗的消息
			memcpy(msg, zIhuFsmTable.taskQue[dest_id].queList[j].msgQue, MAX_IHU_MSG_BODY_LENGTH);
			zIhuFsmTable.taskQue[dest_id].queList[j].useFlag = FALSE;
			//将INDEX指向下一个
			j = (j+1) % MAX_QUEUE_NUM_IN_ONE_TASK;
			zIhuFsmTable.taskQue[dest_id].queIndex = j;
			//判断消息的合法性
			if ((msg->dest_id <= TASK_ID_MIN) || (msg->dest_id >= TASK_ID_MAX)){
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: Receive message error on task_id, dest_id=%d!!!\n", msg->dest_id);
				return FAILURE;
			}
			if ((msg->src_id <= TASK_ID_MIN) || (msg->src_id >= TASK_ID_MAX)){
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: Receive message error on src_id, dest_id=%d!!!\n", msg->src_id);
				return FAILURE;
			}
			//不允许定义消息结构中长度为0的消息体，至少需要一个长度域
			if ((msg->msgLen <= 0) || (msg->msgLen > MAX_IHU_MSG_BUF_LENGTH)){
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: Receive message error on length, msgLen=%d!!!\n", msg->msgLen);
				return FAILURE;
			}
			if ((msg->msgType <= MSG_ID_COM_MIN) || (msg->msgType >= MSG_ID_COM_MAX)){
				zIhuRunErrCnt[TASK_ID_VMFO]++;
				IhuErrorPrint("VMFO: Receive message error on msgId, msgType=%d!!!\n", msg->msgType);
				return FAILURE;
			}			
			//退出循环
			Flag = TRUE;
			break;
		}
	}
	//返回错误，并不一定表示真发生了错误，也有可能是没有任何消息供消耗，此时，上层调用者就啥都不做，确保自己安全
	if (Flag == TRUE){	
		return SUCCESS;
	}else{
		return FAILURE;
	}
}

//message send basic processing
//All in parameters
OPSTAT ihu_message_send_bare_rtos(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len)
{
	IhuMsgSruct_t msg;
	int i=0, j=0;
	bool Flag = FALSE;
	char s1[TASK_NAME_MAX_LENGTH+2]="", s2[TASK_NAME_MAX_LENGTH+2]="", s3[MSG_NAME_MAX_LENGTH]="";
	
	//入参检查Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Error on task_id, src_id=%d!!!\n", src_id);
		return FAILURE;
	}
	if (param_len>MAX_IHU_MSG_BODY_LENGTH){
		zIhuRunErrCnt[TASK_ID_VMFO]++;	
		IhuErrorPrint("VMFO: Too large message length than IHU set capability, param_len=%d!!!\n", param_len);
		return FAILURE;
	}
	if ((msg_id <= MSG_ID_COM_MIN) || (msg_id >= MSG_ID_COM_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Receive message error on msgId, msgType=%d!!!\n", msg_id);
		return FAILURE;
	}

	//对来源数据进行初始化，是为了稳定可靠安全	
	memset(&msg, 0, sizeof(IhuMsgSruct_t));
	msg.msgType = msg_id;
	msg.dest_id = dest_id;
	msg.src_id = src_id;
	msg.msgLen = param_len;
	memcpy(&(msg.msgBody[0]), param_ptr, param_len);


	//然后送到目的地，循环查找一个任务的消息队列，看看有无空闲位置
	for (i=0; i<MAX_QUEUE_NUM_IN_ONE_TASK; i++)
	{
		j = zIhuFsmTable.taskQue[dest_id].queIndex + i;
		j = j%MAX_QUEUE_NUM_IN_ONE_TASK;
		if (zIhuFsmTable.taskQue[dest_id].queList[j].useFlag == FALSE)
		{
			//找到了空闲的消息位置
			memcpy(zIhuFsmTable.taskQue[dest_id].queList[j].msgQue, &msg, MAX_IHU_MSG_BODY_LENGTH);
			zIhuFsmTable.taskQue[dest_id].queList[j].useFlag = TRUE;
			//将INDEX指向下一个
			j = (j+1) % MAX_QUEUE_NUM_IN_ONE_TASK;
			zIhuFsmTable.taskQue[dest_id].queIndex = j;
			//退出循环
			Flag = TRUE;
			break;
		}
	}
	//错误，表示没有空闲队列
	if (Flag == FALSE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Message queue full, can not send into task = %d!!!\n", dest_id);
		return FAILURE;
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
				IhuErrorPrint("VMFO: System Engineering Parameter Trace Mode setting error! DebugMode=%d.\n", zIhuSysEngPar.debugMode);				
			}
			break;
	}

	return SUCCESS;
}


//为了对付不存在OS的情形
OPSTAT ihu_system_task_execute_bare_rtos(UINT8 task_id, FsmStateItem_t *p)
{
	int ret = 0;
	char strDebug[IHU_PRINT_CHAR_SIZE];
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		sprintf(strDebug, "VMFO: Input Error on task_id, task_id=%d!!!\n", task_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	
	//入参检查
	if (p == NULL){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Input wrong FsmStateItem pointer!\n");
		return FAILURE;
	}
	
	//任务控制启动标示检查
	if (zIhuTaskInfo[task_id].pnpState != IHU_TASK_PNP_ON){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		sprintf(strDebug, "VMFO: no need execute this task [%s]!\n", zIhuTaskNameList[task_id]);
		IhuErrorPrint(strDebug);	
		return FAILURE;
	}

	//轮询执行任务的消息消耗
	ret = FsmProcessingLaunchExecuteBareRtos(task_id);
	if (ret == FAILURE)
	{
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		sprintf(strDebug, "VMFO: Execute task FSM un-successfully, taskid = %d", task_id);
		IhuErrorPrint(strDebug);		
		return FAILURE;
	}
	
	//正确返回
	return SUCCESS;
}

//轮询所有任务的正常执行
void ihu_task_execute_all_bare_rtos(void)
{
	#ifdef IHU_COMPILE_TASK_VMFO
		//Execute task VMFO FSM /1
		if (zIhuTaskInfo[TASK_ID_VMFO].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_VMFO, FsmVMFO);	
	#endif //IHU_COMPILE_TASK_VMFO
		
	#ifdef IHU_COMPILE_TASK_TIMER
		//Execute task Timer FSM /2
		if (zIhuTaskInfo[TASK_ID_TIMER].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_TIMER, FsmTimer);
	#endif //IHU_COMPILE_TASK_TIMER

//	#ifdef IHU_COMPILE_TASK_ASYLIBRA
//		//Execute task ASYLIBRA FSM /3
//		if (zIhuTaskInfo[TASK_ID_ASYLIBRA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_ASYLIBRA, FsmAsylibra);
//	#endif //IHU_COMPILE_TASK_ASYLIBRA
//		
//	#ifdef IHU_COMPILE_TASK_ADCARIES
//		//Execute task ADCARIES FSM /4
//		if (zIhuTaskInfo[TASK_ID_ADCARIES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_ADCARIES, FsmAdcaries);
//	#endif //IHU_COMPILE_TASK_ADCARIES
//		
//	#ifdef IHU_COMPILE_TASK_EMC
//		//Execute task EMC FSM /5
//		if (zIhuTaskInfo[TASK_ID_EMC].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_EMC, FsmEmc);
//	#endif //IHU_COMPILE_TASK_EMC

	IhuDebugPrint("VMFO: Execute task once, for test!\n");
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

	//wait for ever
	while (1){
		ihu_sleep(60); //可以设置为5秒的定时，甚至更长
		ihu_vm_check_task_que_status_and_action();
	}

	//清理现场环境
	ihu_task_delete_all_and_queue();

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
		return FAILURE;
	}
		
	//发送初始化消息给目标任务，以便初始化所有任务模块
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = ihu_message_send(MSG_ID_COM_INIT, dest_id, TASK_ID_VMFO, &snd, snd.length);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMFO: Send message error, TASK [%s] to TASK[%s]!\n", zIhuTaskNameList[TASK_ID_VMFO], zIhuTaskNameList[dest_id]);
	}
	
	return ret;
}
