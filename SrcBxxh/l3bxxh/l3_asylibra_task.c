/**
 ****************************************************************************************
 *
 * @file l3_asylibra_task.c
 *
 * @brief Weixin Asylibra application task
 *
 * BXXH team
 * Created by ZJL, 20150819
 *
 ****************************************************************************************
 */
#include "l3_asylibra_task.h"
#include "buffer_man.h"
#include "i2c_led.h"

//FSM handler
static int handler_asylibra_disabled_init(ke_msg_id_t const msgid, struct msg_struct_bleapp_vmda_init const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_asylibra_ble_connected(ke_msg_id_t const msgid, struct msg_struct_asylibra_ble_connected const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_asylibra_auth_resp(ke_msg_id_t const msgid, struct msg_struct_asylibra_auth_resp const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_asylibra_init_resp(ke_msg_id_t const msgid, struct msg_struct_asylibra_init_resp const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_sps_server_ble_data_received(ke_msg_id_t const msgid, msg_struct_sps_vmda_ble_data_dl_rcv_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_asylibra_disconnect(ke_msg_id_t const msgid, struct msg_struct_bleapp_vmda_disconnect const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_wxapp_data_req_emc(ke_msg_id_t const msgid, struct msg_struct_wxapp_data_req_emc const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_asylibra_auth_wait_timer(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_wxapp_data_req_time_sync(ke_msg_id_t const msgid, struct msg_struct_wxapp_data_req_time_sync const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_wxapp_equipment_info_sync(ke_msg_id_t const msgid, struct msg_struct_wxapp_equipment_info_sync const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_asylibra_data_resp(ke_msg_id_t const msgid, struct msg_struct_asylibra_data_resp const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_asylibra_send_ble_data_timer(ke_msg_id_t const msgid, struct msg_struct_asylibra_send_ble_data_timer const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);

//Global definition of state machine
const struct ke_msg_handler fsm_asylibra_defualt[] =
{
	{MSG_BLEAPP_VMDA_INIT, 								(ke_msg_func_t)handler_asylibra_disabled_init},
	{MSG_ASYLIBRA_BLE_CONNNECTD, 					(ke_msg_func_t)handler_asylibra_ble_connected},
	{MSG_SPS_VMDA_BLE_DATA_DL_RCV,				(ke_msg_func_t)handler_sps_server_ble_data_received},
	{MSG_ASYLIBRA_AUTH_RESP, 							(ke_msg_func_t)handler_asylibra_auth_resp},
	{MSG_ASYLIBRA_INIT_RESP, 							(ke_msg_func_t)handler_asylibra_init_resp},
	{MSG_BLEAPP_VMDA_DISCONNECT, 					(ke_msg_func_t)handler_asylibra_disconnect},
	{MSG_WXAPP_DATA_REQ_EMC, 							(ke_msg_func_t)handler_wxapp_data_req_emc},	
	{MSG_ASYLIBRA_AUTH_WAIT_TIMER, 				(ke_msg_func_t)handler_asylibra_auth_wait_timer},
	{MSG_WXAPP_DATA_REQ_TIME_SYNC, 				(ke_msg_func_t)handler_wxapp_data_req_time_sync},	
	{MSG_WXAPP_EQUIPMENT_INFO_SYNC, 			(ke_msg_func_t)handler_wxapp_equipment_info_sync},		
	{MSG_ASYLIBRA_DATA_RESP, 							(ke_msg_func_t)handler_asylibra_data_resp},
	{MSG_ASYLIBRA_SEND_BLE_DATA_TIMER,		(ke_msg_func_t)handler_asylibra_send_ble_data_timer},		
};

// Specifies the message handlers that are common to all states.
// Issue here: can not split the FSM into different state based group for message coming, but has to be put together
// otherwise message can not routine to right destination!!!
const struct ke_state_handler handler_asylibra_default = KE_STATE_HANDLER(fsm_asylibra_defualt);  //KE_STATE_HANDLER(fsm_asylibra_defualt) or KE_STATE_HANDLER_NONE 
/// Defines the placeholder for the states of all the task instances.
ke_state_t asylibra_state[ASYLIBRA_IDX_MAX];
/// SPS_SERVER task descriptor
//static const struct ke_task_desc TASK_DESC_ASYLIBRA = {fsm_asylibra_handler, &handler_asylibra_default, asylibra_state, STATE_ASYLIBRA_MAX, ASYLIBRA_IDX_MAX};
static const struct ke_task_desc TASK_DESC_ASYLIBRA = {NULL, &handler_asylibra_default, asylibra_state, STATE_ASYLIBRA_MAX, ASYLIBRA_IDX_MAX};
struct asylibra_env_tag
{
	//connection information
	struct prf_con_info con_info;
	/// Application Task Id
	ke_task_id_t appid;
	/// Connection handle
	uint16_t conhdl;
	/// DB Handle
	uint16_t shdl;
}asylibra_env;
//定义全局变量
//SEQUENCE ID，随着每次传输一次REQ而增加1
uint16_t asy_seqid;

//待发送的数据，做成全局变量
//这里采用内存数据定义，换取复杂的编解码函数处理。未来如果需要完善，这部分必须完善，同时降低内存的占用
uint8_t DataReqEmc[]   = {0xFE,0x01,0x00,0x1C,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x0C,0x20,0x0A,0x00,0x22,0x15,0x08,0x17,0x23,0x01,0x02,0x00,0x01,0x18,0x02,0x27,0x11};
uint16_t zDataReqEmcLength=0;
uint8_t DataReqPm25[]  = {0xFE,0x01,0x00,0x23,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x13,0x25,0x11,0x81,0x10,0x20,0x30,0x40,0x11,0x21,0x31,0x41,0x12,0x22,0x32,0x42,0x00,0x01,0x00,0x01,0x18,0x02,0x27,0x11};
uint16_t zDataReqPm25Length=0;
uint8_t DataReqWindSpeed[]  = {0xFE,0x01,0x00,0x19,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x09,0x26,0x07,0x81,0x00,0x33,0x00,0x01,0x00,0x01,0x18,0x02,0x27,0x11};
uint16_t zDataReqWindSpeedLength=0;
uint8_t DataReqWindDirection[]  = {0xFE,0x01,0x00,0x19,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x09,0x27,0x07,0x81,0x00,0x33,0x00,0x01,0x00,0x01,0x18,0x02,0x27,0x11};
uint16_t zDataReqWindDirectionLength=0;
uint8_t DataReqTemperature[]  = {0xFE,0x01,0x00,0x19,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x09,0x28,0x07,0x81,0x00,0x33,0x00,0x01,0x00,0x01,0x18,0x02,0x27,0x11};
uint16_t zDataReqTemperatureLength=0;
uint8_t DataReqHumidity[]  = {0xFE,0x01,0x00,0x19,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x09,0x29,0x07,0x81,0x00,0x33,0x00,0x01,0x00,0x01,0x18,0x02,0x27,0x11};
uint16_t zDataReqHumidityLength=0;
	
//MYC
uint8_t AuthReqP[] = {0xFE,0x01,0x00,0x1A,0x27,0x11,0x00,0x01,0x0A,0x00,0x18,0x80,0x80,0x04,0x20,0x01,0x28,0x02,0x3A,0x06,0xD0,0x39,0x72,0xA5,0xEF,0x24};
uint8_t InitReq[]   = {0xFE,0x01,0x00,0x13,0x27,0x13,0x00,0x02,0x0A,0x00,0x1A,0x04,0x30,0x37,0x31,0x35,0x12,0x01,0x00};	
	
	
//轮循数据的发送，采用比尔变量来进行轮换控制
bool zDataIncomingFlag[6]; //6种参量：EMC，PM25，风速、风向、温度、湿度，按照顺序排列
//该数据在InitResp中进行初始化，在H5/JSAPI的退出中设置依然有效，断链之后才会无效
struct struct_init_resp_global_data zInitRespGlobalData;	
	
//init
void task_asylibra_init(void)
{
	// Reset the wxapp_receivererometer environment
	memset(&asylibra_env, 0, sizeof(asylibra_env));	
	// Create ASYLIBRA task
	ke_task_create(TASK_ASYLIBRA, &TASK_DESC_ASYLIBRA);
	// Go to IDLE state
	ke_state_set(TASK_ASYLIBRA, STATE_ASYLIBRA_DISABLED);	
	//AirSyncCharPermSet()  是否还需要？TBD
	asy_seqid = 1; //协议规定，REQ-SEQ从1开始
	zInitRespGlobalData.flag = false;
	//初始化轮循的高层数据来临指示
	uint8_t i=0;
	for (i=0; i<6; i++) {zDataIncomingFlag[i] = false;}
	
	/* MYC 2015/10/25 */
	AmtMainLoopGuiMsgBufferInit(&gAmtMainLoopMsgBuffer);
}

//Mainloop entry, give chance to get control rights at anytime
//纯粹充当一个定时器在应用
void task_asylibra_mainloop(void)
{
	
}

//ASYLIBRA task DISABLE state - Init message processing
//It is actually time out processing, after gapc_cmp_evt_handler send out init trigger here.
static int handler_asylibra_disabled_init(ke_msg_id_t const msgid, struct msg_struct_bleapp_vmda_init const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{	
	//等待一段时间后，再发送AUTH消息
	bxxh_timer_set(MSG_ASYLIBRA_AUTH_WAIT_TIMER, TASK_ASYLIBRA, TIMER_ASYLIBRA_AUTH_WAIT);
	//AsylibraTimerStartFlag = true;  //不再采用MAINLOOP方式
	ke_state_set(TASK_ASYLIBRA, STATE_ASYLIBRA_DISABLED);
	return (KE_MSG_CONSUMED);
}


//TASK_ASYLIBRA task BLE CONNECTED state - BLE_CONNECTED message processing
// Processing of Auth send, move here from Main Loop
//第一条AUTH发送消息的处理
static int handler_asylibra_ble_connected(ke_msg_id_t const msgid, struct msg_struct_asylibra_ble_connected const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//完整AUTH生成方式，由于内存严重不够，暂时也不能使用。未来需要打开，形成完整版本
	//简易生成AUTH方式，MAC地址来自NVDS配置项
	//uint8_t AuthReqP[] = {0xFE,0x01,0x00,0x1A,0x27,0x11,0x00,0x01,0x0A,0x00,0x18,0x80,0x80,0x04,0x20,0x01,0x28,0x02,0x3A,0x06,0xD0,0x39,0x72,0xA5,0xEF,0x24};
	uint8_t MacAddress[]={0,0,0,0,0,0};
	uint8_t TmpHigh, TmpLow;
	uint16_t tmp;		
	//Magic
	AuthReqP[0] = ASY_HEADER_MAGICNUMBER;
	AuthReqP[1] = ASY_HEADER_BVERSION;
	//Length
	tmp = sizeof(AuthReqP);
	TmpHigh = (tmp>>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	AuthReqP[2] = TmpHigh;
	AuthReqP[3] = TmpLow;
	//MessageType
	tmp = ECI_req_auth;
	TmpHigh = (tmp>>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	AuthReqP[4] = TmpHigh;
	AuthReqP[5] = TmpLow;	
	//SEQ设置为最新的发送值
	TmpHigh = (asy_seqid>>8) & 0xFF;
	TmpLow = asy_seqid & 0xFF;
	AuthReqP[6] = TmpHigh;
	AuthReqP[7] = TmpLow;
	//设置MAC地址
	nvds_tag_len_t length = 6;
	nvds_get(NVDS_TAG_BD_ADDRESS, &length, &MacAddress[0]);
	int i = 0;	
	for (i=0; i<6; i++)
	{
		AuthReqP[20+i] = MacAddress[5-i];
	}	
	__disable_irq();
    bxxh_data_send_to_ble((uint8_t *)&AuthReqP, sizeof(AuthReqP));
    __enable_irq();
	bxxh_print_console("ASYLIBRA: Send out message AUTH. seqid=");
    
    sprintf(pDebugPrintBuf, "%02X LEN=%d\r\n", asy_seqid, sizeof(AuthReqP));
    bxxh_print_console(pDebugPrintBuf);
	//bxxh_print_console(hexword2string(asy_seqid));	
	
    //bxxh_print_console("\r\n");

	//go to CONNECTED state
	ke_state_set(TASK_ASYLIBRA, STATE_ASYLIBRA_BLE_CONNECTED);
	
	/* ASYLIBRA FULL CONNECTED */
	bxxh_led_on(LED_ID_7);
	
	return (KE_MSG_CONSUMED);
}

// data reveived from SPS_SERVER task, contain all different of L2/L3 payload, to be decoded here
//接收到微信后台消息的总处理函数
static int  handler_sps_server_ble_data_received(ke_msg_id_t const msgid, msg_struct_sps_vmda_ble_data_dl_rcv_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//解码消息内容，不涉及到状态转移。状态转移需要到具体消息处理函数中进行处理。
	GLOBAL_INT_DISABLE();

//	uint8_t MagicNbr = 0, bVer = 0;
	//uint16_t nLegnth = 0;
//	uint16_t nCmdid = 0, nSeq = 0, t1, t2;
//	MagicNbr = param->value[0];
//	bVer = param->value[1];
	//t1 = param->value[2]; t2 = param->value[3];
	//nLegnth = ((t1<<8) & 0xFF00) + (t2 & 0xFF);
//	t1 = param->value[4]; t2 = param->value[5];
//	nCmdid = ((t1<<8) & 0xFF00) + (t2 & 0xFF);
//	t1 = param->value[6]; t2 = param->value[7];
//	nSeq = ((t1<<8) & 0xFF00) + (t2 & 0xFF);
	
	/* MYC 2015/10/25 */
//	sprintf(pDebugPrintBuf, "BLE DATA R, C BufferMan L = %d\r\n", param->length);
//  bxxh_print_console(pDebugPrintBuf);
	AmtMainLoopGuiMsgBufferPut( (uint8_t *)(&param->value[0]), param->length, &gAmtMainLoopMsgBuffer);	
	bxxh_led_blink_once_on_off(LED_ID_7);
	bxxh_led_blink_once_on_off(LED_ID_7);

	GLOBAL_INT_RESTORE();
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////MOVE TO BUFFER MAN APIs ////////////////////////////
	/////////////    START   ////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	
#if 0	
	
	if (MagicNbr != ASY_HEADER_MAGICNUMBER || bVer != ASY_HEADER_BVERSION)
	{
		//收到错误的无法处理的消息，这里只是打印出错信息
		bxxh_print_console("ASYLIBRA: Receive wrong Magic or Ver. Buffer= ");
		int i=0;
		for (i=0; i<10; i++)
		{
				sprintf(pDebugPrintBuf, "%02X ", param->value[i]);
				bxxh_print_console(pDebugPrintBuf);
				//bxxh_print_console(hexbyte2string(param->value[i]));
			
        //bxxh_print_console(" ");
		}
        
        sprintf(pDebugPrintBuf, "Len=%d ", param->length);
        bxxh_print_console(pDebugPrintBuf);
        
		bxxh_print_console(" \r\n");
		return (KE_MSG_CONSUMED);
	}

	if (nCmdid == ECI_resp_auth) //auth_resp
	{
		//涉及到内存COPY的操作，都会导致任务调度不成功，从而丢失消息，应该是堆栈溢出或者其他类似问题，一般来说，需要独立做MEMALLOC，而不是直接使用内存拷贝
		//这里直接采用原始PARAM的内存，反而不会出错，因为这个指针来自于最底层，通过KE_MSG_ALLOC，确保内存地址不出错
		//其它的Param指针对应的操作都不成功，没办法，只能将该数据结构做成跟最低成一样的结构，才可以直接套用
		//首先处理SEQ
		bxxh_print_console("ASYLIBRA: received auth_resp, seqid=");
        
        sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
        bxxh_print_console(pDebugPrintBuf);
		//bxxh_print_console(hexword2string(asy_seqid));	
		
        sprintf(pDebugPrintBuf, "Len=%d ", param->length);
        bxxh_print_console(pDebugPrintBuf);
        
        bxxh_print_console(" \r\n");
		asy_seqid++;
		//再发送消息给处理函数
		bxxh_message_send(MSG_ASYLIBRA_AUTH_RESP, TASK_ASYLIBRA, TASK_ASYLIBRA, (struct msg_struct_asylibra_auth_resp *)param, param->length+8);
	}
	else if (nCmdid == ECI_resp_init) //init_resp
	{
		//首先处理SEQ
		bxxh_print_console("ASYLIBRA: received init_resp, seqid=");
        
        sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
        bxxh_print_console(pDebugPrintBuf);
		//bxxh_print_console(hexword2string(asy_seqid));	
        
        sprintf(pDebugPrintBuf, "Len=%d ", param->length);
        bxxh_print_console(pDebugPrintBuf);
        
		bxxh_print_console(" \r\n");			
		asy_seqid++;
		//再发送消息给处理函数
		bxxh_message_send(MSG_ASYLIBRA_INIT_RESP, TASK_ASYLIBRA, TASK_ASYLIBRA, (struct msg_struct_asylibra_init_resp *)param, param->length+8);
	}
	//这里解析出的数据业务消息，应该直接发给L3-WXAPP处理，但对应的ASYLIBRA-IE部分，需要这里处理
	//在往L3-WXAPP发送数据时，必要的信息将通过消息中的自定义字段带上去
	//简化处理，先！
	else if (nCmdid == ECI_resp_sendData) // data_resp
	{
		//首先处理SEQ
		//先检查收到的RESP是否等于系统中设置的SEQ，如果不等于，说明有问题
		//系统采用最简单的WAIT-STOP机制，意味着上一次的REQ对应的RESP没有收到，则下一次的REQ不能发出，至于由此导致的消息丢失等问题，全部由高层自行处理，链路层不保证		
		bxxh_print_console("ASYLIBRA: received data_resp, seqid=");
        
        sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
        bxxh_print_console(pDebugPrintBuf);
		//bxxh_print_console(hexword2string(asy_seqid));

        sprintf(pDebugPrintBuf, "Len=%d ", param->length);
        bxxh_print_console(pDebugPrintBuf);
        
		bxxh_print_console(" \r\n");
		//这里的SEQID可能不需要增加1，再仔细考究
		asy_seqid++;
		//再发送消息给处理函数
		bxxh_message_send(MSG_ASYLIBRA_DATA_RESP, TASK_ASYLIBRA, TASK_ASYLIBRA, (struct msg_struct_asylibra_data_resp *)param, param->length+8);
	} //nCmdid == 0x224E
	else if (nCmdid == ECI_push_recvData) // data_push
	{
		//先解码，然后将纯数据发送到高层 (其实，这个函数有问题，还是得依靠手工解码先)
		//RecvDataPush *recv = epb_unpack_recv_data_push(param->value, param->length);
		//epb_unpack_recv_data_push_free(recv);		
		//消息头部分，只需要检查一下seq是否等于0，其它的没有特别需要处理的东西
		if (nSeq != 0){
			bxxh_print_console("ASYLIBRA: Receive Data_Push Seq not zero. \r\n");
		}
		//即使SEQ!=0，依然不影响数据的处理，但会送出打印告警
		struct msg_struct_asylibra_data_push m;
		m.UserCmdId = param->value[12];
		m.length = param->value[13]; //长度为2个字节的情形先不考虑
		if (m.length <=ASY_DATA_PUSH_LENGTH_MAX){
			memcpy(&m.data[0], &param->value[14], m.length);
		}else{
			bxxh_print_console("ASYLIBRA: Received wrong length push data! \r\n");
		}
		//增加JSAPI微信界面H5会话操作功能，就是一直看到的18 00 OR 18 02 27 11
		//使用编解码函数将会更加准确，这里使用傻瓜解码方式，可能会出错的，特别是IE字段顺序被WXAPP调整的话
		uint16_t nManufacture = 0;  //Default为厂商自定义数据
		if ((param->value[14 + m.length] == TAG_RecvDataPush_Type) && (param->value[15 + m.length] >0))
		{
			nManufacture = (param->value[16 + m.length] <<8) & 0xFF00 + (param->value[17 + m.length]) & 0xFF;
			if (nManufacture == EDDT_wxDeviceHtmlChatView)
			{
				m.EmDeviceDataType = EDDT_wxDeviceHtmlChatView;
			}
			else 
			{
				m.EmDeviceDataType = EDDT_manufatureSvr;			
			}
		}
		else
		{
			m.EmDeviceDataType = EDDT_manufatureSvr;
		}		
		//将内容体发送到WXAPP进行处理
        sprintf(pDebugPrintBuf, "ASYLIBRA: push_recvData Len=%d ", param->length);
        bxxh_print_console(pDebugPrintBuf);

		bxxh_message_send(MSG_ASYLIBRA_DATA_PUSH, TASK_WXAPP, TASK_ASYLIBRA, &m, sizeof(m));
		//依然在连接态，故而继续保持，确保状态的稳定性
		ke_state_set(TASK_ASYLIBRA, STATE_ASYLIBRA_INIT_OK_DATA_READY);
	}
	else
	{
		//收到错误的无法处理的消息
		bxxh_print_console("ASYLIBRA: Receive un-recoginized message, buffer=");
		int i=0;
		for (i=0; i<10; i++)
		{
			sprintf(pDebugPrintBuf, "%02X ", param->value[i]);
            bxxh_print_console(pDebugPrintBuf);
            //bxxh_print_console(hexbyte2string(param->value[i]));
 
            //bxxh_print_console(" ");
		}
        
        sprintf(pDebugPrintBuf, "Len=%d ", param->length);
        bxxh_print_console(pDebugPrintBuf);
		bxxh_print_console(" \r\n");
	}
   
#endif
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////MOVE TO BUFFER MAN APIs ////////////////////////////
	/////////////    END   ////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	
	return (KE_MSG_CONSUMED);
}


//TASK_ASYLIBRA task AUTH_OK state - auth_resp message processing
static int  handler_asylibra_auth_resp(ke_msg_id_t const msgid, struct msg_struct_asylibra_auth_resp const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//先解码AUTH-RESP消息，并做必要的状态转移

	//发送Init_req, 直接二进制码流， 生成消息结构
	//uint8_t InitReq[]   = {0xFE,0x01,0x00,0x13,0x27,0x13,0x00,0x02,0x0A,0x00,0x1A,0x04,0x30,0x37,0x31,0x35,0x12,0x01,0x7F};
	//uint8_t InitReq[]   = {0xFE,0x01,0x00,0x13,0x27,0x13,0x00,0x02,0x0A,0x00,0x1A,0x04,0x30,0x37,0x31,0x35,0x12,0x01,0x00};
	//MYC 7F -> 00 !!!!!!! //
	uint8_t TmpHigh, TmpLow;
	uint16_t tmp;
	//Magic
	InitReq[0] = ASY_HEADER_MAGICNUMBER;
	InitReq[1] = ASY_HEADER_BVERSION;
	//Length
	tmp = sizeof(InitReq);
	TmpHigh = (tmp>>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	InitReq[2] = TmpHigh;
	InitReq[3] = TmpLow;
	//MessageType
	tmp = ECI_req_init;
	TmpHigh = (tmp>>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	InitReq[4] = TmpHigh;
	InitReq[5] = TmpLow;	
	//SEQ设置为最新的发送值
	TmpHigh = (asy_seqid>>8) & 0xFF;
	TmpLow = asy_seqid & 0xFF;
	InitReq[6] = TmpHigh;
	InitReq[7] = TmpLow;
	//0A 00 => BaseRequest
	//1A 04 30 37 31 35 => Challenge，为了做到足够好，这里应该生成随机数，然后在接收的InitResp中进行对比，以增强安全性
	//12 01 41 => RespFieldFilter
	//enum EmInitRespFieldFilter {
	//EIRFF_userNickName = 0x1;
	//EIRFF_platformType = 0x2;
	//EIRFF_model = 0x4;
	//EIRFF_os = 0x8;
	//EIRFF_time = 0x10;
	//EIRFF_timeZone = 0x20;
	//EIRFF_timeString = 0x40;
	//}	这些领域，全部设置为1，总数值=0x7F，从而所有参量都从InitResp中回送
	InitReq[18] = 0x00;	

	__disable_irq();
	bxxh_data_send_to_ble((uint8_t *)&InitReq, sizeof(InitReq));
	__enable_irq();
	bxxh_print_console("ASYLIBRA: Send out message Init-Req. seqid=");
    
    sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
    bxxh_print_console(pDebugPrintBuf);
    //bxxh_print_console(hexword2string(asy_seqid));	
	
    sprintf(pDebugPrintBuf, "Len=%d, Id=0x%04x ", InitReq[3], (InitReq[4]<<8 | InitReq[5]));
    bxxh_print_console(pDebugPrintBuf);
        
    bxxh_print_console(" \r\n");		
		
	ke_state_set(TASK_ASYLIBRA, STATE_ASYLIBRA_AUTH_OK);

	return (KE_MSG_CONSUMED);
}


//处理进入连接态
static int  handler_asylibra_init_resp(ke_msg_id_t const msgid, struct msg_struct_asylibra_init_resp const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//详细解码过程
//	InitResponse *response = epb_unpack_init_response(param->value, param->length);
//	//无数有想象力的处理可以在这儿完成！！！
//	epb_unpack_init_response_free(response);
	
	//通知并激活L3-WXAPP
	struct msg_struct_asylibra_connect_ready m;
	m.length = sizeof(m);
	bxxh_message_send(MSG_ASYLIBRA_CONNECT_READY, TASK_WXAPP, TASK_ASYLIBRA, &m, m.length);
	ke_state_set(TASK_ASYLIBRA, STATE_ASYLIBRA_INIT_OK_DATA_READY);

	//启动定时器，准备开始轮着发送数据到BLE
	bxxh_timer_set(MSG_ASYLIBRA_SEND_BLE_DATA_TIMER, TASK_ASYLIBRA, TIMER_ASYLIBRA_SEND_DATA_OUT_2S);
	
	zInitRespGlobalData.flag = true;
	//缺少解码uppack函数，这个手工解码过于复杂，暂时空缺，未来慢慢优化
		
	SetDebugTraceOn();
	return (KE_MSG_CONSUMED);
}


//处理拆链过程
static int  handler_asylibra_disconnect(ke_msg_id_t const msgid, struct msg_struct_bleapp_vmda_disconnect const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//推送L3_WXAPP状态的改变，取消可能的定时器
	struct msg_struct_asylibra_connect_ready m;
	m.length = sizeof(m);
	bxxh_message_send(MSG_ASYLIBRA_DISCONNECT_TIGGER_L3, TASK_WXAPP, TASK_ASYLIBRA, &m, m.length);
	ke_state_set(TASK_ASYLIBRA, STATE_ASYLIBRA_DISABLED);
	
	//停止继续轮送数据的定时器
	bxxh_timer_clear(MSG_ASYLIBRA_SEND_BLE_DATA_TIMER, TASK_ASYLIBRA);

	//设置InitResp的状态，以便跟复位H5/JSAPI的控制
	zInitRespGlobalData.flag = false;
    
	/* ASYLIBRA FULL CONNECTED */
	bxxh_led_off(LED_ID_7);
	
	return (KE_MSG_CONSUMED);
}


//处理定时数据处理过程，发送定时EMC给后台
static int  handler_wxapp_data_req_emc(ke_msg_id_t const msgid, struct msg_struct_wxapp_data_req_emc const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//真正发送出去的数据， 全部放在这儿进行处理， DATA_REQ
	//先试试标准的Protobuf编码方式，可惜MALLOC函数的堆栈溢出，这里先割爱了
	/*SendDataRequest *request = (SendDataRequest *)malloc(sizeof(SendDataRequest));
	*/
	//再直接使用建议码流方式，节省内存
	//	uint8_t DataReqEmc[]   = {0xFE,0x01,0x00,0x22,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x14,0x20,0x12,0x00,0x22,0x15,0x08,0x17,0x23,0x01,0x02,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,00,01,0x18,0x00};
	//定时辐射强度(IHU -> Cloud-) FE (Magic Number) 01 (Ver) 00 1E (Length)  27 12 (cmdid/req) 1237 (seq) (变长消息体) 0A 00 (BaseReq) 12 10 (data) 20 (L3控制字) 0E (Len) 00 22 (EMC) YY MM DD HH MM SS (T) 01 02 03 04 05 06 (GPS) 18 00 (Manufacture Data)
	uint8_t AdcHigh, AdcLow, TmpHigh, TmpLow; //TmpH3, TmpH4;
	uint16_t tmp;
	//unsigned int tmp4;
	//Magic
	DataReqEmc[0] = ASY_HEADER_MAGICNUMBER;
	DataReqEmc[1] = ASY_HEADER_BVERSION;
	//Length
	if (param->EmDeviceDataType == EDDT_wxDeviceHtmlChatView)
	{tmp = sizeof(DataReqEmc);}  //IE_Type=18022711, default
	else
	{tmp = sizeof(DataReqEmc)-2;} //IE_Type=1800
	zDataReqEmcLength = tmp;
	TmpHigh = (tmp >>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	DataReqEmc[2] = TmpHigh;
	DataReqEmc[3] = TmpLow;
	//MessageType
	tmp = ECI_req_sendData;
	TmpHigh = (tmp >>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	DataReqEmc[4] = TmpHigh;
	DataReqEmc[5] = TmpLow;	
	//SEQ设置为最新的发送值
	TmpHigh = (asy_seqid >>8) & 0xFF;
	TmpLow = asy_seqid & 0xFF;
	DataReqEmc[6] = TmpHigh;
	DataReqEmc[7] = TmpLow;	
	//User Cmd Id
	DataReqEmc[12] = param->usercmdid;
	DataReqEmc[13] = 0x0A;
	//ADC Value 采用网络大头的方式进行赋值
	uint16_t adcvalue = param->emc_value;
	AdcLow = adcvalue & 0xFF;
	AdcHigh = (adcvalue >> 8) & 0xFF;
	DataReqEmc[14] = AdcHigh;
	DataReqEmc[15] = AdcLow;
	//年月日时分秒的赋值
	DataReqEmc[16] = param->year;
	DataReqEmc[17] = param->month;
	DataReqEmc[18] = param->day;
	DataReqEmc[19] = param->hour;
	DataReqEmc[20] = param->minute;
	DataReqEmc[21] = param->second;
	//GPS，抑制掉，没有多大意义
/*	tmp4 = param->gps_x;
	TmpH4 = (tmp4>>24) & 0xFF;
	TmpH3 = (tmp4>>16) & 0xFF;
	TmpHigh = (tmp4>>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	DataReqEmc[22] = TmpH4;
	DataReqEmc[23] = TmpH3;
	DataReqEmc[24] = TmpHigh;
	DataReqEmc[25] = TmpLow;		
	tmp4 = param->gps_y;
	TmpH4 = (tmp4>>24) & 0xFF;
	TmpH3 = (tmp4>>16) & 0xFF;
	TmpHigh = (tmp4>>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	DataReqEmc[26] = TmpH4;
	DataReqEmc[27] = TmpH3;
	DataReqEmc[28] = TmpHigh;
	DataReqEmc[29] = TmpLow;	
*/	
	//ntimes
	tmp = param->ntimes;
	TmpHigh = (tmp >>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	DataReqEmc[22] = TmpHigh;
	DataReqEmc[23] = TmpLow;
	//EmDeviceDataType的处理
	if (param->EmDeviceDataType == EDDT_wxDeviceHtmlChatView)
	{  //IE_Type=18022711, default
		DataReqEmc[25] = 0x02; //长度
		DataReqEmc[26] = 0x27;
		DataReqEmc[27] = 0x11;
	}
	else
	{  //IE_Type=1800	
		DataReqEmc[25] = 0x00; //长度
	} 

	//发送
	zDataIncomingFlag[0] = true;  //0表示EMC

	//启动定时器，准备开始轮着发送数据到BLE
	//假设上层数据发送很稀疏，不然这个机制会出问题
	bxxh_timer_set(MSG_ASYLIBRA_SEND_BLE_DATA_TIMER, TASK_ASYLIBRA, TIMER_ASYLIBRA_SEND_DATA_OUT_2S);

	//NO state change
	return (KE_MSG_CONSUMED);
}


//处理AUTH WAIT TIMER的问题
//其实完全可以直接在这里处理AUTH发送的问题，但考虑到状态的完整性，还是保留
static int handler_asylibra_auth_wait_timer(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	struct msg_struct_asylibra_ble_connected m;
	m.length = sizeof(m);
	bxxh_message_send(MSG_ASYLIBRA_BLE_CONNNECTD, TASK_ASYLIBRA, TASK_ASYLIBRA, &m, m.length);
    
	return (KE_MSG_CONSUMED);
}

//未来如果发送队列机制处理好了，可以将具体的发送过程并进去，这里单独发送，有微小的冲突风险
//处理MSG_WXAPP_DATA_REQ_TIME_SYNC
static int handler_wxapp_data_req_time_sync(ke_msg_id_t const msgid, struct msg_struct_wxapp_data_req_time_sync const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//直接使用建议码流方式，节省内存
	uint8_t TimeSync[]   = {0xFE,0x01,0x00,0x10,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x02,0xF2,0x00,0x18,0x00};
	uint8_t TmpHigh, TmpLow;
	uint16_t tmp
	;//Magic
	TimeSync[0] = ASY_HEADER_MAGICNUMBER;
	TimeSync[1] = ASY_HEADER_BVERSION;
	//Length
	tmp = sizeof(TimeSync);
	TmpHigh = (tmp >>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	TimeSync[2] = TmpHigh;
	TimeSync[3] = TmpLow;
	//MessageType
	tmp = ECI_req_sendData;
	TmpHigh = (tmp >>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	TimeSync[4] = TmpHigh;
	TimeSync[5] = TmpLow;	
	//SEQ设置为最新的发送值
	TmpHigh = (asy_seqid >>8) & 0xFF;
	TmpLow = asy_seqid & 0xFF;
	TimeSync[6] = TmpHigh;
	TimeSync[7] = TmpLow;
	//发送
	__disable_irq();
	bxxh_data_send_to_ble((uint8_t *)&TimeSync, sizeof(TimeSync));
	__enable_irq();	
	//这个并没有统一防到定时发送中，所以这里需要单独处理SEQID
	asy_seqid++;
	bxxh_print_console("ASYLIBRA: send out time sync message, seqid=");
    
    sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
    bxxh_print_console(pDebugPrintBuf);
	//bxxh_print_console(hexword2string(asy_seqid));	

    sprintf(pDebugPrintBuf, "Len=%d ", param->length);
    bxxh_print_console(pDebugPrintBuf);
	
    bxxh_print_console(" \r\n");

	return (KE_MSG_CONSUMED);
}

//未来如果发送队列机制处理好了，可以将具体的发送过程并进去，这里单独发送，有微小的冲突风险
//处MSG_WXAPP_EQUIPMENT_INFO_SYNC
static int handler_wxapp_equipment_info_sync(ke_msg_id_t const msgid, struct msg_struct_wxapp_equipment_info_sync const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//直接使用建议码流方式，节省内存
	uint8_t EquipInfo[]   = {0xFE,0x01,0x00,0x1C,0x27,0x12,0x12,0x37,0x0A,0x00,0x12,0x0E,0xF0,0x0C,0xD0,0x39,0x72,0xA5,0xEF,0x24,0x01,0x00,0x01,0x01,0x00,0x01, 0x18,0x00};  //  0xF0 0x0C (Len) D0 39 72 A5 EF 24 (MAC) 01 (HW_Type) 00 01 (PEM1) 01 (Release1) 00 01 (SW_Delivery) 

	uint8_t TmpHigh, TmpLow;
	uint16_t tmp
	;//Magic
	EquipInfo[0] = ASY_HEADER_MAGICNUMBER;
	EquipInfo[1] = ASY_HEADER_BVERSION;
	//Length
	tmp = sizeof(EquipInfo);
	TmpHigh = (tmp >>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	EquipInfo[2] = TmpHigh;
	EquipInfo[3] = TmpLow;
	//MessageType
	tmp = ECI_req_sendData;
	TmpHigh = (tmp >>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	EquipInfo[4] = TmpHigh;
	EquipInfo[5] = TmpLow;	
	//SEQ设置为最新的发送值
	TmpHigh = (asy_seqid >>8) & 0xFF;
	TmpLow = asy_seqid & 0xFF;
	EquipInfo[6] = TmpHigh;
	EquipInfo[7] = TmpLow;
	//消息体赋值: MAC
	int i=0;
	for (i=0; i<6; i++)
	{
		EquipInfo[14+i] = param->hw_uuid[i];
	}
	//消息体赋值: HW
	EquipInfo[20] = param->hw_type;
	tmp = param->hw_version;
	TmpHigh = (tmp >>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	EquipInfo[21] = TmpHigh;
	EquipInfo[22] = TmpLow;
	//消息体赋值: SW
	EquipInfo[23] = param->sw_release;
	tmp = param->sw_delivery;
	TmpHigh = (tmp >>8) & 0xFF;
	TmpLow = tmp & 0xFF;
	EquipInfo[24] = TmpHigh;
	EquipInfo[25] = TmpLow;
	//发送
	__disable_irq();
	bxxh_data_send_to_ble((uint8_t *)&EquipInfo, sizeof(EquipInfo));
	__enable_irq();	
	//这个并没有统一防到定时发送中，所以这里需要单独处理SEQID
	asy_seqid++;
	bxxh_print_console("ASYLIBRA: send out equipment info message, seqid=");
    
    sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
    bxxh_print_console(pDebugPrintBuf);
	//bxxh_print_console(hexword2string(asy_seqid));	

    sprintf(pDebugPrintBuf, "Len=%d ", param->length);
    bxxh_print_console(pDebugPrintBuf);
    
	bxxh_print_console(" \r\n");

	return (KE_MSG_CONSUMED);
}

//MSG_ASYLIBRA_DATA_RESP处理过程
static int  handler_asylibra_data_resp(ke_msg_id_t const msgid, struct msg_struct_asylibra_data_resp const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//先解码，暂时属于无效操作

	//No state change
    
	return (KE_MSG_CONSUMED);
}


//TIMER来轮，确保数据一次一次的发送到BLE，而不出现冲突
//简单的算法，比考虑不同传感器之间的公平性，意味着，只要一种传感器发送的速度过快，会导致其它传感器阻塞
static int handler_asylibra_send_ble_data_timer(ke_msg_id_t const msgid, struct msg_struct_asylibra_send_ble_data_timer const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{	
	bool worked = false;
	uint16_t EmcValue = 0;
	uint32_t Pm25 = 0;
	uint16_t WindSpeedValue = 0;
	uint16_t WindDirectionValue = 0;
	uint16_t TemperatureValue = 0;
	uint16_t HumidityValue = 0;
	if (zDataIncomingFlag[0] == true)  //EMC
	{
		__disable_irq();
		bxxh_data_send_to_ble((uint8_t *)&DataReqEmc, zDataReqEmcLength);
		__enable_irq();
		zDataIncomingFlag[0] = false;
		worked = true;
		asy_seqid++;       
		EmcValue = DataReqEmc[14];
		EmcValue = (EmcValue << 8) | DataReqEmc[15];
		//打印输出监控数据
		bxxh_print_console("ASYLIBRA: send out EMC data report, seqid=");  
		sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
		bxxh_print_console(pDebugPrintBuf);
		sprintf(pDebugPrintBuf, "Emc=%d Len=%d ", EmcValue, sizeof(DataReqEmc));
		bxxh_print_console(pDebugPrintBuf);
		bxxh_print_console(" \r\n");			
	}
	else if ((worked == false) && (zDataIncomingFlag[1] == true))  //PM25
	{
		__disable_irq();
		bxxh_data_send_to_ble((uint8_t *)&DataReqPm25, zDataReqPm25Length);
		__enable_irq();	
		zDataIncomingFlag[1] = false;
		asy_seqid++;
		bxxh_print_console("ASYLIBRA: send out PM1025 data report, seqid=");
    //PM2.5
		Pm25 = DataReqPm25[19];
		Pm25 = (Pm25 << 8) | DataReqPm25[20];
		Pm25 = (Pm25 << 8) | DataReqPm25[21];
		Pm25 = (Pm25 << 8) | DataReqPm25[22];
		//打印输出监控数据		
		sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
		bxxh_print_console(pDebugPrintBuf);
		sprintf(pDebugPrintBuf, "Pm25=%d Len=%d ", Pm25, sizeof(DataReqPm25));
		bxxh_print_console(pDebugPrintBuf);        
		bxxh_print_console(" \r\n");	
	}
	else if ((worked == false) && (zDataIncomingFlag[2] == true))  //WindSpeed
	{
		__disable_irq();
		bxxh_data_send_to_ble((uint8_t *)&DataReqWindSpeed, zDataReqWindSpeedLength);
		__enable_irq();
		zDataIncomingFlag[2] = false;
		worked = true;
		asy_seqid++;       
		WindSpeedValue = DataReqWindSpeed[15];
		WindSpeedValue = (WindSpeedValue << 8) | DataReqWindSpeed[16]; 
		//打印输出监控数据
		bxxh_print_console("ASYLIBRA: send out WindSpeed data report, seqid=");  
		sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
		bxxh_print_console(pDebugPrintBuf);
		sprintf(pDebugPrintBuf, "WindSpeed=%d Len=%d ", WindSpeedValue, sizeof(DataReqWindSpeed));
		bxxh_print_console(pDebugPrintBuf);
		bxxh_print_console(" \r\n");			
	}
	else if ((worked == false) && (zDataIncomingFlag[3] == true))  //WindDirection
	{
		__disable_irq();
		bxxh_data_send_to_ble((uint8_t *)&DataReqWindDirection, zDataReqWindDirectionLength);
		__enable_irq();
		zDataIncomingFlag[3] = false;
		worked = true;
		asy_seqid++;       
		WindDirectionValue = DataReqWindDirection[15];
		WindDirectionValue = (WindDirectionValue << 8) | DataReqWindDirection[16]; 
		//打印输出监控数据
		bxxh_print_console("ASYLIBRA: send out WindDirection data report, seqid=");  
		sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
		bxxh_print_console(pDebugPrintBuf);
		sprintf(pDebugPrintBuf, "WindDirection=%d Len=%d ", WindDirectionValue, sizeof(DataReqWindDirection));
		bxxh_print_console(pDebugPrintBuf);
		bxxh_print_console(" \r\n");			
	}	
	else if ((worked == false) && (zDataIncomingFlag[4] == true))  //Temperature
	{
		__disable_irq();
		bxxh_data_send_to_ble((uint8_t *)&DataReqTemperature, zDataReqTemperatureLength);
		__enable_irq();
		zDataIncomingFlag[4] = false;
		worked = true;
		asy_seqid++;       
		TemperatureValue = DataReqTemperature[15];
		TemperatureValue = (WindDirectionValue << 8) | DataReqTemperature[16]; 
		//打印输出监控数据
		bxxh_print_console("ASYLIBRA: send out Temperature data report, seqid=");  
		sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
		bxxh_print_console(pDebugPrintBuf);
		sprintf(pDebugPrintBuf, "Temperature=%d Len=%d ", TemperatureValue, sizeof(DataReqTemperature));
		bxxh_print_console(pDebugPrintBuf);
		bxxh_print_console(" \r\n");			
	}
	else if ((worked == false) && (zDataIncomingFlag[5] == true))  //Humidity
	{
		__disable_irq();
		bxxh_data_send_to_ble((uint8_t *)&DataReqHumidity, zDataReqHumidityLength);
		__enable_irq();
		zDataIncomingFlag[5] = false;
		worked = true;
		asy_seqid++;       
		HumidityValue = DataReqHumidity[15];
		HumidityValue = (HumidityValue << 8) | DataReqHumidity[16]; 
		//打印输出监控数据
		bxxh_print_console("ASYLIBRA: send out Humidity data report, seqid=");  
		sprintf(pDebugPrintBuf, "%02X ", asy_seqid);
		bxxh_print_console(pDebugPrintBuf);
		sprintf(pDebugPrintBuf, "Humidity=%d Len=%d ", HumidityValue, sizeof(DataReqHumidity));
		bxxh_print_console(pDebugPrintBuf);
		bxxh_print_console(" \r\n");			
	}	
	
	//继续启动TIMER
	//为了提高效率，这个轮不是无限的，不然会出现冲突的问题：其他多个传感器任务发来一堆数据，但这整个还未轮完，导致定时器被不断重置
	//判断的方式是：当前连接在线，而且多个传感器中还有数据未发出的数据
	bool IncomingFlag = false;
	uint8_t i=0;	for (i=0; i<6; i++) {IncomingFlag = IncomingFlag || zDataIncomingFlag[i];}
	if ((ke_state_get(TASK_ASYLIBRA) == STATE_ASYLIBRA_INIT_OK_DATA_READY) && (IncomingFlag == true))
	{
		bxxh_timer_set(MSG_ASYLIBRA_SEND_BLE_DATA_TIMER, TASK_ASYLIBRA, TIMER_ASYLIBRA_SEND_DATA_OUT_2S);
	}
	
	//No state change    
	return (KE_MSG_CONSUMED);
}
