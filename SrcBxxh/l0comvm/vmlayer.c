/*
 * vmlayer.c
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#include "vmlayer.h"
#include "i2c_led.h"

/*
 *
 *   全局变量
 *
 */

//全局变量：任务和队列
IhuTaskTag_t zIhuTaskInfo[MAX_TASK_NUM_IN_ONE_IHU];

//全局变量：记录所有任务模块工作差错的次数，以便适当处理
UINT32 zIhuRunErrCnt[MAX_TASK_NUM_IN_ONE_IHU];

//全局变量：
IhuHwInvInfoTag_t zIhuHwInvInfo;

//任务状态机FSM全局控制表，占用内存的大户！！！
FsmTable_t zIhuFsmTable;

//全局工程参数表
IhuSysEngParTable_t zIhuSysEngPar; //全局工程参数控制表

//全局SLEEP时长控制表
UINT32 zIhuSleepCnt[MAX_TASK_NUM_IN_ONE_IHU][MAX_SLEEP_NUM_IN_ONE_TASK];

//全局时间
time_t zIhuSystemTimeUnix = 1444341556;  //2015/8
struct tm zIhuSystemTimeYmd;

//请确保，该全局字符串的定义跟Task_Id的顺序保持完全一致，不然后面的显示内容会出现差错
//请服从最长长度TASK_NAME_MAX_LENGTH的定义，不然Debug/Trace打印出的信息也会出错
//全局变量：任务打印命名
//从极致优化内存的角度，这里浪费了一个TASK对应的内存空间（MIN=0)，但它却极大的改善了程序编写的效率，值得浪费！！！
char *zIhuTaskNameList[MAX_TASK_NUM_IN_ONE_IHU] ={
	"MIN",
	"VMDA",
	"TIMER",
	"ASYLIBRA",
	//"AKSLEO",
	"ADCARIES",
	"EMC"};
	//,"MAX"};

//消息ID的定义全局表，方便TRACE函数使用
//请服从MSG_NAME_MAX_LENGTH的最长定义，不然出错
//全局变量：消息打印命名
char *zIhuMsgNameList[MAX_MSGID_NUM_IN_ONE_TASK] ={
		"MSG_ID_COM_MIN",
		"MSG_ID_COM_INIT",
		"MSG_ID_COM_INIT_FB",
		"MSG_ID_COM_RESTART",
		"MSG_ID_COM_TIME_OUT",
		"MSG_ID_XXX_NULL"
};

//char strDebug1[BX_PRINT_SZ];
bool flagInitMsgSend = FALSE;
/*
 *
 *   过程API函数
 *
 */

//VM系统初始化过程API
void bxxh_vm_init_hook(void)
{
	char strDebug[BX_PRINT_SZ];
	
	//纯粹启动信息的展示
	IhuDebugPrint("IHU-VM: VM Shell start and initialization!");
	sprintf(strDebug, "IHU-VM: Current HW-Type=%d, HW-VER=%d, SW-REL=%d, SW-DELIVERY=%d.", IHU_EMCWX_CURRENT_HW_TYPE, IHU_EMCWX_CURRENT_HW_PEM, IHU_EMCWX_CURRENT_SW_RELEASE, IHU_EMCWX_CURRENT_SW_DELIVERY);
	IhuDebugPrint(strDebug);
		
	//NVDS MAC ADRESS的展示
	UINT8 MacAdd[]={0,0,0,0,0,0};
	ihu_mac_add_get(MacAdd, sizeof(MacAdd));
	sprintf(strDebug, "IHU-VM: MAC Address = [%02X %02X %02X %02X %02X %02X].", MacAdd[0], MacAdd[1], MacAdd[2], MacAdd[3], MacAdd[4], MacAdd[5]);
	IhuDebugPrint(strDebug);

	//初始化工程参量控制表
	memset(&zIhuSysEngPar, 0, sizeof(IhuSysEngParTable_t));
	zIhuSysEngPar.debugMode = TRACE_DEBUG_ON; //取缺省DEBUG设置
	zIhuSysEngPar.traceMode = TRACE_MSG_ON;
	
	//init VMDA SHELL Task
	bxxh_task_vmda_init();
	
	//初始化全局变量TASK_ID/QUE_ID/TASK_STAT
	memset(&(zIhuTaskInfo[0].TaskId), 0, sizeof(zIhuTaskInfo)*(TASK_ID_MAX-TASK_ID_MIN+1));
	int i=0;
	for (i=TASK_ID_MIN; i<TASK_ID_MAX; i++){
		zIhuTaskInfo[i].TaskId = i;
		strcpy(zIhuTaskInfo[i].TaskName, zIhuTaskNameList[i]);
	}
	zIhuTaskInfo[TASK_ID_VMDA].pnpState = IHU_TASK_STATE_VMDA;
	zIhuTaskInfo[TASK_ID_TIMER].pnpState = IHU_TASK_STATE_TIMER;
	zIhuTaskInfo[TASK_ID_ASYLIBRA].pnpState = IHU_TASK_STATE_ASYLIBRA;
	zIhuTaskInfo[TASK_ID_ADCARIES].pnpState = IHU_TASK_STATE_ADCARIES;
	zIhuTaskInfo[TASK_ID_EMC].pnpState = IHU_TASK_STATE_EMC;	
	
	//初始化记录各个任务模块差错的参量
	memset(zIhuRunErrCnt, 0, sizeof(UINT32)*(TASK_ID_MAX-TASK_ID_MIN+1));

	//初始化HwInv参量
	memset(&zIhuHwInvInfo, 0, sizeof(IhuHwInvInfoTag_t));
	
	//初始化SLEEP控制表
	memset(zIhuSleepCnt, 0, sizeof(UINT32)*MAX_TASK_NUM_IN_ONE_IHU*MAX_SLEEP_NUM_IN_ONE_TASK);
	
	//初始化全局时间
	zIhuSystemTimeUnix = 1444341556; //2015.Aug, where we starting from!!!
	
	//TrACE INIT
	//TraceInit();

	//Init FSM
	FsmInit();

	//创建并初始化所有任务，这是调用vmda_shell中提供的控制API来进行这个初始化
	ihu_task_create_all();
	
	//然后，初始化完成，系统可以进入正常的循环中
	return;
}

//系统任务每次都要调用的轮询过程
void bxxh_vm_task_entry_hook(void)
{
	//要给VMDA SHELL一个MAIN ENTRY的入口，有些逆层次结构，但无伤大雅
	bxxh_task_vmda_mainloop();
	
	//实现等待一会儿
	if (flagInitMsgSend == FALSE)
	{
		if (ihu_sleep(VM_SLEEP_INIT_1SECOND_CNT, TASK_ID_VMDA, VM_SLEEP_INIT_1SECOND_SEED) == FAILURE)
		{
			return;
		}
		else{
			//使用bxxh_message_send方法，发送消息给VMDA，让VMDA-SHELL起到总控的目的，初始化所有其它任务模块
			bxxh_init_msg_to_vmda();			
			flagInitMsgSend = TRUE;
		}
	}

	//直接调用VM状态机的轮循，该状态机中，轮询出错，并不引起特别重要的后果，只是打印错误，记录出错数量，并正常返回
	if (ihu_sleep(VM_SLEEP_INIT_1SECOND_CNT, TASK_ID_VMDA, VM_SLEEP_INIT_1SECOND_SEED) == SUCCESS){
		ihu_task_execute_all();
	}
	return;

}


//正常打印，print string to console port (UART2)
void IhuDebugPrint(char *p)
{
	char strDebugPrintBuf[BX_PRINT_SZ]; //用于打印
	if (strlen(p) < BX_PRINT_SZ-50){
		//sprintf(strDebugPrintBuf, "DBG: %s, %s, %d, %s\r\n", __DATE__, __TIME__, __LINE__, p);
		sprintf(strDebugPrintBuf, "DBG: %s, %s, %s\r\n", __DATE__, __TIME__, p);
		arch_printf(strDebugPrintBuf);		
	}
	else{
		strncpy(strDebugPrintBuf, p, BX_PRINT_SZ-1);
		arch_printf(strDebugPrintBuf);
		arch_printf("\r\n");
	}
}

//错误打印，print string to console port (UART2)
void IhuErrorPrint(char *p)
{
	char strDebugPrintBuf[BX_PRINT_SZ]; //用于打印
	if (strlen(p) < BX_PRINT_SZ-50){
		//sprintf(strDebugPrintBuf, "ERR: %s, %s, %d, %s\r\n", __DATE__, __TIME__, __LINE__, p);
		sprintf(strDebugPrintBuf, "ERR: %s, %s, %s\r\n", __DATE__, __TIME__, p);
		arch_printf(strDebugPrintBuf);		
	}
	else{
		strncpy(strDebugPrintBuf, p, BX_PRINT_SZ-1);
		arch_printf(strDebugPrintBuf);
		arch_printf("\r\n");		
	}
}


//纯粹为了方便，以便自己的任务调用
void bxxh_timer_set(ke_msg_id_t const timerid, ke_task_id_t const taskid, uint16_t delay)
{
	app_timer_set(timerid, taskid, delay);
}

//纯粹为了方便，以便自己的任务调用
void bxxh_timer_clear(ke_msg_id_t const timerid, ke_task_id_t const taskid)
{
	ke_timer_clear(timerid, taskid);
}

void app_uart_push_zjl(unsigned char *p, int n)
{
	return;
}

//数据送往BLE模块
void bxxh_data_send_to_ble(unsigned char *p, int n)
{
	app_uart_push_zjl(p, n);
	bxxh_led_blink_once_on_off(LED_ID_7);
	bxxh_led_blink_once_on_off(LED_ID_7);
}

void app_ble_push(unsigned char *p, int n)
{
	return;
}

//数据送往UART1模块
void bxxh_data_send_to_uart(unsigned char *p, int n)
{
	app_ble_push(p, n);
	bxxh_led_blink_once_on_off(LED_ID_6);
	bxxh_led_blink_once_on_off(LED_ID_6);
}

//采样EMC数据
int bxxh_emc_sample(void)
{
	return adc_get_sample();
}

//Only used for PEM1, to be updated for PEM2
void bxxh_led_flair(void)
{
	if (zIhuHwInvInfo.led_on_off == true){
#ifdef IHU_EMCWX_CFG_HW_PEM2_0
		bxxh_led_blink_once_on_off(LED_ID_6);
		bxxh_led_blink_once_on_off(LED_ID_6);
#else
		GPIO_SetActive(BEEP_PORT,BEEP_PIN);
#endif
		
		zIhuHwInvInfo.led_on_off = false;
	}else{
		
#ifdef IHU_EMCWX_CFG_HW_PEM2_0
		bxxh_led_blink_once_on_off(LED_ID_6);
		bxxh_led_blink_once_on_off(LED_ID_6);
#else
		GPIO_SetInactive(BEEP_PORT,BEEP_PIN);
#endif		
		zIhuHwInvInfo.led_on_off = true;
	}
}

uint16_t b2l_uint16(uint16_t in)
{
		return ( ((in & 0x00FF) << 8) | (in >> 8) );
}

//message send
//param_len indicate the message total length = structure + value length
void bxxh_message_send(ke_msg_id_t const msg_id, ke_task_id_t const dest_id, ke_task_id_t const src_id, void const *param_ptr, uint16_t const param_len)
{
	char strDebug[BX_PRINT_SZ];
	struct param_str *msg = ke_msg_alloc(msg_id, dest_id, src_id, param_len);
	
	if(NULL == msg)
	{
		sprintf(strDebug, "IHU-VM: bxxh_message_send, ke_msg_alloc return with NULL msg_id = %d", msg_id);
		IhuDebugPrint(strDebug);
		return;
	}	
	memcpy(msg, param_ptr, param_len);

	//TIME_OUT消息要小心
#if (TRACE_MSG_ON)
	sprintf(strDebug, "IHU-VM: MSGID=0x%02x, [0x%02x]<-[0x%02x], LEN=%d", msg_id, dest_id, src_id, param_len);
	IhuDebugPrint(strDebug);
#endif //TRACE_MSG_ON

	ke_msg_send(msg);
}

//初始化生成消息，发送给VMDA，以便发送给所有任务模块
void bxxh_init_msg_to_vmda(void)
{
	int ret = 0;
	char strDebug[BX_PRINT_SZ];
	
	if (zIhuTaskInfo[TASK_ID_VMDA].pnpState != IHU_TASK_PNP_ON){
		return;
	}
	
	//纯粹是发送INIT MESSAGE给VMDA模块，以便VMDA分别触发其它所有模块的初始化
	msg_struct_com_init_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = ihu_message_send(MSG_ID_COM_INIT, TASK_ID_VMDA, TASK_ID_VMDA, &snd, snd.length);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Send message error, TASK [%s] to TASK[%s]!", zIhuTaskNameList[TASK_ID_VMDA], zIhuTaskNameList[TASK_ID_VMDA]);
		IhuErrorPrint(strDebug);
	}
	
	return;
}

OPSTAT fsm_com_do_nothing(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT8 param_len)
{
	return SUCCESS;
}

/*******************************************************************************
**
**	任务和队列相关的API
**
**------------------------------------------------------------------------------*/
OPSTAT FsmSetState(UINT8 task_id, UINT8 newState)
{
	char strDebug[BX_PRINT_SZ];	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: FsmSetState error on task_id, task_id=%d!!!", task_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	//Checking newState range
	if (newState > MAX_STATE_NUM_IN_ONE_TASK){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: FsmSetState error on state, State=%d!!!", newState);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	zIhuTaskInfo[task_id].state = newState;
	return SUCCESS;
}

UINT8  FsmGetState(UINT8 task_id)
{
	char strDebug[BX_PRINT_SZ];	
	/*
	** Check the task_id
	*/
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on task_id, task_id=%d!!!", task_id);
		IhuErrorPrint(strDebug);
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
	char strDebug[BX_PRINT_SZ];
	IhuDebugPrint("IHU-VM: >>Start init FSM.");
	zIhuFsmTable.numOfFsmCtrlTable = 0;
	for(i=0; i<MAX_TASK_NUM_IN_ONE_IHU; i++)
	{
		zIhuFsmTable.pFsmCtrlTable[i].taskId = TASK_ID_INVALID;
		zIhuFsmTable.pFsmCtrlTable[i].numOfFsmArrayElement = 0;
	}
	zIhuFsmTable.currentTaskId = TASK_ID_INVALID;

	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		sprintf(strDebug, "IHU-VM: Maxium (%d) tasks/modules supported.", MAX_TASK_NUM_IN_ONE_IHU);
		IhuDebugPrint(strDebug);
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
	char strDebug[BX_PRINT_SZ];

	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
		sprintf(strDebug, "IHU-VM: >>Register new FSM. TaskId:(%d), pFsm(0x%x).", task_id, (UINT32)pFsmStateItem);
		IhuDebugPrint(strDebug);
	}
	/*
	** Check the array of the state machine.
	*/
	if( pFsmStateItem == NULL)
	{
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("IHU-VM: Can not init FSM machine.");
		return FAILURE;
	}
	if((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX))
	{
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("IHU-VM: The task_ID is invalid.");
		return FAILURE;
	}
	if( zIhuFsmTable.pFsmCtrlTable[task_id].taskId != TASK_ID_INVALID )
	{
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("IHU-VM: This task_id has been already inited.");
		return FAILURE;
	}

	/*
	** Check the first entry. 最初一个FSM表单必须以FSM_STATE_ENTRY开始
	*/
	if( pFsmStateItem[FSM_STATE_ENTRY].state != FSM_STATE_ENTRY )
	{
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("IHU-VM: The format of the FSM table is invalid.");
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
			zIhuRunErrCnt[TASK_ID_VMDA]++;
			IhuErrorPrint("IHU-VM: Invalid FSM machine.");
			return FAILURE;
		}
		if( NULL == pFsmStateItem[item].stateFunc)
		{
			zIhuRunErrCnt[TASK_ID_VMDA]++;
			IhuErrorPrint("IHU-VM: Invalid state function pointer.");
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
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("IHU-VM: Invalid FSM machine -- Can not find the end of the FSM.");
		return FAILURE;
	}
	zIhuFsmTable.pFsmCtrlTable[task_id].numOfFsmArrayElement = item-1; //有效STATE-MSG条目数，不包括START/END两条

	/*
	**  Insert this new fsm item into fsm table.
	**  The position in the fsm table is based on the task_id.
	*/
	zIhuFsmTable.numOfFsmCtrlTable++;

	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE)
	{
		sprintf(strDebug, "IHU-VM: FsmAddNew: task_id = 0x%x [%s], src_id= %x, dest_id= %X.", task_id, zIhuTaskNameList[task_id], 0, 0);
		IhuDebugPrint(strDebug);
		sprintf(strDebug, "IHU-VM: After add this one, Total (%d) FSM in the table.", zIhuFsmTable.numOfFsmCtrlTable);
		IhuDebugPrint(strDebug);
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
			zIhuRunErrCnt[TASK_ID_VMDA]++;
			sprintf(strDebug, "IHU-VM: The State number > %d.", MAX_STATE_NUM_IN_ONE_TASK);
			IhuErrorPrint(strDebug);
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
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("IHU-VM: Error set FSM State!");
	}

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
OPSTAT FsmRunEngine(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT8 param_len)
{
	UINT8  state;
	OPSTAT ret;
	UINT32 mid;
	char strDebug[BX_PRINT_SZ];
	/*
	** Check the task_id, message_id and par_length
	*/
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on task_id, dest_id=%d!!!", dest_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on task_id, src_id=%d!!!", src_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if (param_len>MAX_IHU_MSG_BODY_LENGTH){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Too large message length than HCU set capability, param_len=%d!!!", param_len);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if ((msg_id <= MSG_ID_COM_MIN) || (msg_id >= MSG_ID_COM_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on Msg_Id, msg_id=%d!!!", msg_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	/*
	** Search for the dest_id task state
	*/

	if( zIhuFsmTable.pFsmCtrlTable[dest_id].taskId != dest_id )
	{
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("IHU-VM: The destination process does not exist.");
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
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: The state(%d) or msgId(0x%x) of task(0x%x) is error.", 	state, mid, dest_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_IPT_ON) != FALSE)
	{
		sprintf(strDebug, "IHU-VM: Call state function(0x%x) in state(%d) of task(0x%x) for msg(0x%x).", (UINT32)zIhuFsmTable.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc, state, dest_id, mid);
		IhuDebugPrint(strDebug);
	}

	/*
	** Call the state function.
	*/
	if(zIhuFsmTable.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc != NULL)
	{
		ret = (zIhuFsmTable.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc)(dest_id, src_id, param_ptr, param_len);
		if( FAILURE == ret)
		{
			zIhuRunErrCnt[TASK_ID_VMDA]++;			
			IhuErrorPrint("IHU-VM: Internal error is found in the state function.");
			return FAILURE;
		}
	}
	else
	{
		if(NULL != param_ptr)
		{
			//Free memory, here do nothing.
		}
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Receive invalid msg(%x)[%s] in state(%d) of task(0x%x)[%s].", mid, zIhuMsgNameList[mid], state, dest_id, zIhuTaskNameList[dest_id]);
		IhuErrorPrint(strDebug);
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
OPSTAT FsmProcessingLaunchEntry(UINT8 task_id)
{
	char strDebug[BX_PRINT_SZ];

	/*
	** Check the task_id
	*/
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on task_id, task_id=%d!!!", task_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	/*
	** Run each task entry API, give each task a chance to init any specific information
	*/
	if (zIhuFsmTable.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc != NULL){
		(zIhuFsmTable.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc)(task_id, 0, NULL, 0);
	}else{
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Task (ID=%d) get no init entry fetched!", task_id);
		IhuErrorPrint(strDebug);
	}
	
	return SUCCESS;
}

OPSTAT FsmProcessingLaunchExecute(UINT8 task_id)
{
	OPSTAT ret;
	IhuMsgSruct_t rcv;
	char strDebug[BX_PRINT_SZ];

	/*
	** Check the task_id
	*/
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on task_id, task_id=%d!!!", task_id);
		IhuErrorPrint(strDebug);
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
			zIhuRunErrCnt[TASK_ID_VMDA]++;	
			IhuErrorPrint("IHU-VM: Error execute FsmRun state machine!");
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
OPSTAT ihu_message_rcv(UINT8 dest_id, IhuMsgSruct_t *msg)
{
	char strDebug[BX_PRINT_SZ];	
	//Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on task_id, dest_id=%d!!!", dest_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	//循环查找一个任务的消息队列，看看有无有效消息
	int i=0, j=0;
	bool Flag = FALSE;
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
				zIhuRunErrCnt[TASK_ID_VMDA]++;
				sprintf(strDebug, "IHU-VM: Receive message error on task_id, dest_id=%d!!!", msg->dest_id);
				IhuErrorPrint(strDebug);
				return FAILURE;
			}
			if ((msg->src_id <= TASK_ID_MIN) || (msg->src_id >= TASK_ID_MAX)){
				zIhuRunErrCnt[TASK_ID_VMDA]++;
				sprintf(strDebug, "IHU-VM: Receive message error on src_id, dest_id=%d!!!", msg->src_id);
				IhuErrorPrint(strDebug);
				return FAILURE;
			}
			//不允许定义消息结构中长度为0的消息体，至少需要一个长度域
			if ((msg->msgLen <= 0) || (msg->msgLen > MAX_IHU_MSG_BUF_LENGTH)){
				zIhuRunErrCnt[TASK_ID_VMDA]++;
				sprintf(strDebug, "IHU-VM: Receive message error on length, msgLen=%d!!!", msg->msgLen);
				IhuErrorPrint(strDebug);
				return FAILURE;
			}
			if ((msg->msgType <= MSG_ID_COM_MIN) || (msg->msgType >= MSG_ID_COM_MAX)){
				zIhuRunErrCnt[TASK_ID_VMDA]++;
				sprintf(strDebug, "IHU-VM: Receive message error on msgId, msgType=%d!!!", msg->msgType);
				IhuErrorPrint(strDebug);
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
OPSTAT ihu_message_send(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT8 param_len)
{
	IhuMsgSruct_t msg;
	char strDebug[BX_PRINT_SZ];	
	
	//入参检查Checking task_id range
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on task_id, dest_id=%d!!!", dest_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if ((src_id <= TASK_ID_MIN) || (src_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on task_id, src_id=%d!!!", src_id);		
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if (param_len>MAX_IHU_MSG_BODY_LENGTH){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Too large message length than IHU set capability, param_len=%d!!!", param_len);			
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if ((msg_id <= MSG_ID_COM_MIN) || (msg_id >= MSG_ID_COM_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Receive message error on msgId, msgType=%d!!!", msg_id);
		IhuErrorPrint(strDebug);
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
	int i=0, j=0;
	bool Flag = FALSE;
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
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Message queue full, can not send into task = %d!!!", dest_id);	
		IhuErrorPrint(strDebug);
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
	char s1[TASK_NAME_MAX_LENGTH+2]="", s2[TASK_NAME_MAX_LENGTH+2]="", s3[MSG_NAME_MAX_LENGTH]="";
	switch (zIhuSysEngPar.traceMode)
	{
		case TRACE_MSG_MODE_OFF:
			break;

		case TRACE_MSG_MODE_INVALID:
			break;

		case TRACE_MSG_MODE_ALL:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			IhuDebugPrint(strDebug);
			break;

		case TRACE_MSG_MODE_ALL_BUT_TIME_OUT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if (msg_id != MSG_ID_COM_TIME_OUT){
				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
				IhuDebugPrint(strDebug);
			}
			break;

//		case TRACE_MSG_MODE_ALL_BUT_HEART_BEAT:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);
//			}
//			break;

//		case TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((msg_id != MSG_ID_COM_TIME_OUT) && (msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);
//			}
//			break;

//		case TRACE_MSG_MODE_MOUDLE_TO_ALLOW:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);
//			}
//			break;

//		case TRACE_MSG_MODE_MOUDLE_TO_RESTRICT:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict!= TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);
//			}
//			break;
//
//		case TRACE_MSG_MODE_MOUDLE_FROM_ALLOW:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);
//			}
//			break;
//
//		case TRACE_MSG_MODE_MOUDLE_FROM_RESTRICT:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict!= TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_MOUDLE_DOUBLE_ALLOW:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)
//					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_MOUDLE_DOUBLE_RESTRICT:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleToRestrict != TRUE)
//					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_MSGID_ALLOW:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_MSGID_RESTRICT:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_COMBINE_TO_ALLOW:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
//					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_COMBINE_TO_RESTRICT:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
//					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_COMBINE_FROM_ALLOW:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
//					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_COMBINE_FROM_RESTRICT:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
//					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_COMBINE_DOUBLE_ALLOW:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)
//					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)
//					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

//		case TRACE_MSG_MODE_COMBINE_DOUBLE_RESTRICT:
//			taskid_to_string(dest_id, s1);
//			taskid_to_string(src_id, s2);
//			msgid_to_string(msg_id, s3);
//			if ((zIhuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zIhuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)
//					&& (zIhuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zIhuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)
//					&& (zIhuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zIhuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)){
//				sprintf(strDebug, "MSGTRC: MSGID=%02X%s,DID=%02X%s,SID=%02X%s,LEN=%d\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
//				IhuDebugPrint(strDebug);				
//			}
//			break;

		default:
			if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
				zIhuRunErrCnt[TASK_ID_VMDA]++;				
				sprintf(strDebug, "IHU-VM: System Engineering Parameter Trace Mode setting error! DebugMode=%d.", zIhuSysEngPar.debugMode);
				IhuErrorPrint(strDebug);				
			}
			break;
	}

	return SUCCESS;
}

//TaskId transfer to string
//待调试的函数，是否需要使用动态内存
//正确的做法也许应该使用zHcuTaskInfo[id].TaskName这个信息来表达
OPSTAT taskid_to_string(UINT8 id, char *string)
{
	if ((id<=TASK_ID_MIN) || (id>=TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;	
		IhuErrorPrint("IHU-VM: Error task Id input!");
		return FAILURE;
	}
	char tmp[TASK_NAME_MAX_LENGTH-2]="";
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
OPSTAT msgid_to_string(UINT16 id, char *string)
{
	if (id <= MSG_ID_COM_MIN || id >= MSG_ID_COM_MAX){
		zIhuRunErrCnt[TASK_ID_VMDA]++;	
		IhuErrorPrint("IHU-VM: Error Message Id input!");
		return FAILURE;
	}
	char tmp[MSG_NAME_MAX_LENGTH-2]="";
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


//清理某一个任务的消息队列
OPSTAT ihu_message_queue_clean(UINT8 dest_id)
{
	char strDebug[BX_PRINT_SZ];
	//入参检查
	if ((dest_id <= TASK_ID_MIN) || (dest_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Error on task_id, dest_id=%d!!!", dest_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}	
	//清理消息队列
	memset(&zIhuFsmTable.taskQue[dest_id], 0, sizeof(FsmQueueListTable_t));
	
	return SUCCESS;
}

//聚合创建任务，消息队列，并直接让其开始运行
OPSTAT ihu_task_create_and_run(UINT8 task_id, FsmStateItem_t* pFsmStateItem)
{
	OPSTAT ret = 0;
	char strDebug[BX_PRINT_SZ];
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Input Error on task_id, task_id=%d!!!", task_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	//Init Fsm table
	ret = FsmAddNew(task_id, pFsmStateItem);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Init state machine FsmAddNew error, taskid = %d.", task_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_CRT_ON) != FALSE){
		sprintf(strDebug, "IHU-VM: FsmAddNew Successful, taskId = 0x%x [%s].", task_id, zIhuTaskNameList[task_id]);
		IhuDebugPrint(strDebug);
	}

	//Create Queid
	ret = ihu_message_queue_clean(task_id);
	if (ret == FAILURE)
	{
	zIhuRunErrCnt[TASK_ID_VMDA]++;
	sprintf(strDebug, "IHU-VM: Create queue unsuccessfully, taskId = %d.", task_id);
	IhuErrorPrint(strDebug);
	return FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_CRT_ON) != FALSE){
		sprintf(strDebug, "IHU-VM: Msgque create successful, taskId = 0x%x [%s].", task_id, zIhuTaskNameList[task_id]);
		IhuDebugPrint(strDebug);
	}

	//Create task and make it running for the 1st time
	ret = FsmProcessingLaunchEntry(task_id);
	if (ret == FAILURE)
	{
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Create task un-successfully, taskid = %d", task_id);
		IhuErrorPrint(strDebug);		
		return FAILURE;
	}
	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_CRT_ON) != FALSE){
		sprintf(strDebug, "IHU-VM: Task create Successful, taskId = 0x%x [%s].", task_id, zIhuTaskNameList[task_id]);
	  IhuDebugPrint(strDebug);
	}
	return SUCCESS;
}

OPSTAT ihu_system_task_init_call(UINT8 task_id, FsmStateItem_t *p)
{
	int ret = 0;
	char strDebug[BX_PRINT_SZ];
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Input Error on task_id, task_id=%d!!!", task_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	
	//入参检查
	if (p == NULL){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("IHU-VM: Input wrong FsmStateItem pointer!");
		return FAILURE;
	}
	
	//任务控制启动标示检查
	if (zIhuTaskInfo[task_id].pnpState != IHU_TASK_PNP_ON){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: no need create this task [%s]!", zIhuTaskNameList[task_id]);
		IhuErrorPrint(strDebug);	
		return FAILURE;
	}
	
	//打印信息
	if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_CRT_ON) != FALSE){
		sprintf(strDebug, "IHU-VM: Staring to create task [%s] related environments...", zIhuTaskNameList[task_id]);
		IhuDebugPrint(strDebug);
	}

	//任务创建并初始化所有状态机
	ret = ihu_task_create_and_run(task_id, p);
	if (ret == FAILURE){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: create task env [%s] un-successfully, program exit.", zIhuTaskNameList[task_id]);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}else{
		if ((zIhuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
			sprintf(strDebug, "IHU-VM: create task successfully, taskid=%d[%s].", task_id, zIhuTaskNameList[task_id]);
			IhuDebugPrint(strDebug);
		}
	}
	
	//正确返回
	return SUCCESS;
}

OPSTAT ihu_system_task_execute(UINT8 task_id, FsmStateItem_t *p)
{
	int ret = 0;
	char strDebug[BX_PRINT_SZ];
	
	//Checking task_id range
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Input Error on task_id, task_id=%d!!!", task_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	
	//入参检查
	if (p == NULL){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		IhuErrorPrint("IHU-VM: Input wrong FsmStateItem pointer!");
		return FAILURE;
	}
	
	//任务控制启动标示检查
	if (zIhuTaskInfo[task_id].pnpState != IHU_TASK_PNP_ON){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: no need execute this task [%s]!", zIhuTaskNameList[task_id]);
		IhuErrorPrint(strDebug);	
		return FAILURE;
	}

	//轮询执行任务的消息消耗
	ret = FsmProcessingLaunchExecute(task_id);
	if (ret == FAILURE)
	{
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Execute task FSM un-successfully, taskid = %d", task_id);
		IhuErrorPrint(strDebug);		
		return FAILURE;
	}
	
	//正确返回
	return SUCCESS;
}

//创建所有任务
void ihu_task_create_all(void)
{
#ifdef IHU_COMPILE_TASK_VMDA
	//Create task VMDA environments /1
	if (zIhuTaskInfo[TASK_ID_VMDA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_VMDA, FsmVmda);	
#endif //IHU_COMPILE_TASK_VMDA

#ifdef IHU_COMPILE_TASK_TIMER
	//Create task Timer environments /2
	if (zIhuTaskInfo[TASK_ID_TIMER].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_TIMER, FsmTimer);
#endif //IHU_COMPILE_TASK_TIMER

#ifdef IHU_COMPILE_TASK_ASYLIBRA
	//Create task ASYLIBRA environments /3
	if (zIhuTaskInfo[TASK_ID_ASYLIBRA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_ASYLIBRA, FsmAsylibra);
#endif //IHU_COMPILE_TASK_ASYLIBRA
	
#ifdef IHU_COMPILE_TASK_ADCARIES
	//Create task ADCARIES environments /4
	if (zIhuTaskInfo[TASK_ID_ADCARIES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_ADCARIES, FsmAdcaries);
#endif //IHU_COMPILE_TASK_ADCARIES
	
#ifdef IHU_COMPILE_TASK_EMC
	//Create task EMC environments /5
	if (zIhuTaskInfo[TASK_ID_EMC].pnpState == IHU_TASK_PNP_ON) ihu_system_task_init_call(TASK_ID_EMC, FsmEmc);
#endif //IHU_COMPILE_TASK_EMC
	
	IhuDebugPrint("VMDA: Create all task successfully!");
}

//轮询所有任务的正常执行
void ihu_task_execute_all(void)
{
#ifdef IHU_COMPILE_TASK_VMDA
	//Execute task VMDA FSM /1
	if (zIhuTaskInfo[TASK_ID_VMDA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_VMDA, FsmVmda);	
#endif //IHU_COMPILE_TASK_VMDA
	
#ifdef IHU_COMPILE_TASK_TIMER
	//Execute task Timer FSM /2
	if (zIhuTaskInfo[TASK_ID_TIMER].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_TIMER, FsmTimer);
#endif //IHU_COMPILE_TASK_TIMER

#ifdef IHU_COMPILE_TASK_ASYLIBRA
	//Execute task ASYLIBRA FSM /3
	if (zIhuTaskInfo[TASK_ID_ASYLIBRA].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_ASYLIBRA, FsmAsylibra);
#endif //IHU_COMPILE_TASK_ASYLIBRA
	
#ifdef IHU_COMPILE_TASK_ADCARIES
	//Execute task ADCARIES FSM /4
	if (zIhuTaskInfo[TASK_ID_ADCARIES].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_ADCARIES, FsmAdcaries);
#endif //IHU_COMPILE_TASK_ADCARIES
	
#ifdef IHU_COMPILE_TASK_EMC
	//Execute task EMC FSM /5
	if (zIhuTaskInfo[TASK_ID_EMC].pnpState == IHU_TASK_PNP_ON) ihu_system_task_execute(TASK_ID_EMC, FsmEmc);
#endif //IHU_COMPILE_TASK_EMC

	IhuDebugPrint("VMDA: Execute task once, for test!");
}

//休眠多少轮时间长度，靠系统轮询次数来计数
//实际上形成了一种周期休眠机制，由于它必须由轮训重入才能获得SUCCESS执行结果，从而得到控制权，所以小心使用该机制进行消息消耗性处理
//它更容易当做周期性定时器来使用，而不是真正意义上的SLEEP从而挂起程序
OPSTAT ihu_sleep(UINT32 cntDuration, UINT8 task_id, UINT8 seed)
{
	char strDebug[BX_PRINT_SZ];
	
	//入参检查
	if ((task_id <= TASK_ID_MIN) || (task_id >= TASK_ID_MAX)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Input Error on task_id, task_id=%d!!!", task_id);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if ((cntDuration <= 0) || (cntDuration > MAX_SLEEP_COUNTER_UP_LIMITATION)){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Input Error on cntDuration, cntDuration=%d!!!", cntDuration);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}
	if (seed >= MAX_SLEEP_NUM_IN_ONE_TASK){
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		sprintf(strDebug, "IHU-VM: Input Error on seed, seed=%d!!!", seed);
		IhuErrorPrint(strDebug);
		return FAILURE;
	}

	//计数
	zIhuSleepCnt[task_id][seed]++;
	if (zIhuSleepCnt[task_id][seed] >=cntDuration){
		zIhuSleepCnt[task_id][seed] = 0;
		return SUCCESS;
	}else{
		return FAILURE;
	}
}

//从系统底层获取MAC地址
//该地址返回将按照正常的顺序进行，对于DA14580需要逆取
OPSTAT ihu_mac_add_get(UINT8 *mac, UINT8 len)
{
	//入参检查
	if (len !=6){
		IhuErrorPrint("VMDA: Func ihu_mac_add_get input parameter len must be 6!");
		zIhuRunErrCnt[TASK_ID_VMDA]++;
		return FAILURE;
	}
	
	//底层获取MAC地址
	UINT8 tmpMac[] = {0,0,0,0,0,0};
	nvds_get(NVDS_TAG_BD_ADDRESS, &len, tmpMac);
	int i = 0;	
	for (i=0; i<6; i++)
	{
		*(mac+i) = tmpMac[5-i];
	}
	
	return SUCCESS;
}

//Unix Time transfer to YMD time
struct tm ihu_clock_unix_to_ymd(time_t t_unix)
{
	return *localtime(&t_unix);
}

//EMC ADC Read
UINT16 ihu_emc_adc_read(void)
{
	return bxxh_emc_sample();
}

//简单的函数映射
OPSTAT get_mac_addr(UINT8* mac)
{
	return ihu_mac_add_get(mac, 6);
}

