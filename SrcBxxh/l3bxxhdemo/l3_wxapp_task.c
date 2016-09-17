/**
 ****************************************************************************************
 *
 * @file l3_wxapp_task.c
 *
 * @brief Weixin Layer 3 application task
 *
 * BXXH team
 * Created by ZJL, 20150819
 *
 ****************************************************************************************
 */

#include "l3_wxapp_task.h"


//Global definition of state machine
/*
const struct ke_msg_handler fsm_wxapp_disabled[] =
{
    {MSG_WXAPP_INIT, (ke_msg_func_t)handler_wxapp_disabled_init},
};
const struct ke_msg_handler fsm_wxapp_connected[] =
{
    {MSG_WXAPP_TEST1, (ke_msg_func_t)handler_wxapp_connected_test1},
		{MSG_WXAPP_TEST2, (ke_msg_func_t)handler_wxapp_connected_test2},
};
const struct ke_state_handler fsm_wxapp_state[STATE_WXAPP_MAX] =
{
	[STATE_WXAPP_DISABLED] = KE_STATE_HANDLER(fsm_wxapp_disabled),
	[STATE_WXAPP_CONNECTED] = KE_STATE_HANDLER(fsm_wxapp_connected),
};
*/

static int handler_wxapp_disabled_init(ke_msg_id_t const msgid, struct msg_struct_asylibra_connect_ready const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_wxapp_3min_timtout(ke_msg_id_t const msgid, struct msg_struct_3min_timtout const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_wxapp_5second_timtout(ke_msg_id_t const msgid, struct msg_struct_5second_timtout const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_wxapp_data_push(ke_msg_id_t const msgid, struct msg_struct_asylibra_data_push const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_wxapp_period_report_emc(ke_msg_id_t const msgid, struct msg_struct_period_report_emc_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
static int handler_wxapp_disconnect_trigger_l3(ke_msg_id_t const msgid, struct msg_struct_asylibra_disconnect_tigger_l3 const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);

const struct ke_msg_handler fsm_wxapp_defualt[] =
{
	{MSG_ASYLIBRA_CONNECT_READY, 						(ke_msg_func_t)handler_wxapp_disabled_init},
	{MSG_WXAPP_3MIN_TIMEOUT, 								(ke_msg_func_t)handler_wxapp_3min_timtout},
	{MSG_WXAPP_5SECOND_TIMEOUT, 						(ke_msg_func_t)handler_wxapp_5second_timtout},
	{MSG_ASYLIBRA_DATA_PUSH, 								(ke_msg_func_t)handler_wxapp_data_push},
	{MSG_WXAPP_PERIOD_REPORT_EMC_TRIGGER, 	(ke_msg_func_t)handler_wxapp_period_report_emc},
	{MSG_ASYLIBRA_DISCONNECT_TIGGER_L3, 			(ke_msg_func_t)handler_wxapp_disconnect_trigger_l3},
};
/// Specifies the message handlers that are common to all states.
const struct ke_state_handler handler_wxapp_default = KE_STATE_HANDLER(fsm_wxapp_defualt); //KE_STATE_HANDLER_NONE;
/// Defines the placeholder for the states of all the task instances.
ke_state_t wxapp_state[WXAPP_IDX_MAX];
/// WXAPP task descriptor
//Not yeat solved FSM and incoming message group issue
//static const struct ke_task_desc TASK_DESC_WXAPP = {fsm_wxapp_state, &handler_wxapp_default, wxapp_state, STATE_WXAPP_MAX, WXAPP_IDX_MAX};
static const struct ke_task_desc TASK_DESC_WXAPP = {NULL, &handler_wxapp_default, wxapp_state, STATE_WXAPP_MAX, WXAPP_IDX_MAX};
struct wxapp_env_tag
{
	//connection information
	struct prf_con_info con_info;
	/// Application Task Id
	ke_task_id_t appid;
	/// Connection handle
	uint16_t conhdl;
	/// DB Handle
	uint16_t shdl;
}wxapp_env;

//全局时间
time_t z_system_time_unix = 1444341556;  //2015/8
struct tm *z_system_time_ymd;
//全局FLASH数据存储，桶形算法
//将所有的采样数据都存到这儿，然后由WXAPP统一送回到后台，其它任务模块只负责采集并存储数据，并不负责单独发送数据，
//以减少数据发送的冲突
//这意味着，桶形数据缓冲区，必须设计成各类数据都能适应，同时，发送不同数据格式到后台，也能处理
flash_data_sample_storage1_t z_flash_data_sample[FLASH_DATA_SAMPLE_STORAGE_NBR_MAX];
uint8_t z_flash_data_sample_write;  //存储的起点
uint8_t z_flash_data_sample_read; //读取的起点
uint8_t z_flash_data_sample_nbr;  //存入有效多少记录

//Init
void task_wxapp_init(void)
{
	// Reset the wxapp_receivererometer environment
	memset(&wxapp_env, 0, sizeof(wxapp_env));
	
	// Create WXAPP task
	ke_task_create(TASK_WXAPP, &TASK_DESC_WXAPP);

	// Go to IDLE state
	ke_state_set(TASK_WXAPP, STATE_WXAPP_DISABLED);
	//初始化Unix时钟标签到固定值
	z_system_time_unix = 1444341556; //2015/8
	z_system_time_ymd = localtime(&z_system_time_unix);
	//初始化采样存储数据的指针
	z_flash_data_sample_write = 0;
	z_flash_data_sample_read = 0;
	z_flash_data_sample_nbr = 0;
}

//Mainloop entry, give chance to get control rights at anytime
//int testcnttotal = 1000;
//int testcnt = 0;
void task_wxapp_mainloop(void)
{
	//这里可以放置一些及时工作的钩子函数
	/*char data[] = "It is a test";
	testcnt++;
	if (testcnt >= testcnttotal)
	{
		//bxxh_data_send_to_uart((unsigned char*)data, sizeof(data));
		//uart_write("My test!\r\n", 20, NULL);
		//uart_finish_transfers();
		//bxxh_print_console("Pure Test");
		bxxh_print_console("TASK_MODBUS state = ");
		bxxh_print_console(hexbyte2string(ke_state_get(TASK_MODBUS)));	
		bxxh_print_console(" \r\n");
		bxxh_print_console("TASK_WXAPP state = ");
		bxxh_print_console(hexbyte2string(ke_state_get(TASK_WXAPP)));	
		bxxh_print_console(" \r\n");
		bxxh_print_console("TASK_ASYLIBRA state = ");
		bxxh_print_console(hexbyte2string(ke_state_get(TASK_ASYLIBRA)));	
		bxxh_print_console(" \r\n");
		testcnt = 0;
	}*/
}


//WXAPP task DISABLE state - Init message processing
//初始化L3任务，并进行相应的干活
static int handler_wxapp_disabled_init(ke_msg_id_t const msgid, struct msg_struct_asylibra_connect_ready const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//先从后台获取最新时间，并更新下位机的时间
	struct msg_struct_wxapp_data_req_time_sync m;
	m.length = sizeof(m);
	bxxh_message_send(MSG_WXAPP_DATA_REQ_TIME_SYNC, TASK_ASYLIBRA, TASK_WXAPP, &m, m.length);

	//第一次上电初始化后，启动定时器
	if (ke_state_get(TASK_WXAPP) == STATE_WXAPP_DISABLED)
	{
		//L3-WXAPP正式进入CONNECT状态后，ADC定时读取数据便开始了
		bxxh_timer_set(MSG_WXAPP_3MIN_TIMEOUT, TASK_WXAPP, TIMER_WXAPP_3MIN);
	}
	//重现BLE上线
	else if (ke_state_get(TASK_WXAPP) == STATE_WXAPP_OFFLINE)
	{
		//将FLASH存储的历史数据都发到后台
		//妥善处理nTimes的参数问题，以表征时间的问题
		//启动定时器，将数据发送后台。每次发送之前，必须确认连接是否存在
		if (z_flash_data_sample_nbr > 0)
		{
			bxxh_timer_set(MSG_WXAPP_5SECOND_TIMEOUT, TASK_WXAPP, TIMER_WXAPP_5SECOND);
		}
	}
	
	ke_state_set(TASK_WXAPP, STATE_WXAPP_CONNECTED);    
	return (KE_MSG_CONSUMED);
}


//3分定时处理机制
static int handler_wxapp_3min_timtout(ke_msg_id_t const msgid, struct msg_struct_3min_timtout const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//首先更新时钟逻辑
	z_system_time_unix = z_system_time_unix + TIMER_WXAPP_3MIN/100;
	z_system_time_ymd = localtime(&z_system_time_unix);

	//再发送Trigger给ADC-EMC读取数据
	struct msg_struct_period_report_emc_trigger m;
	m.length = sizeof(m);
	bxxh_message_send(MSG_WXAPP_PERIOD_REPORT_EMC_TRIGGER, TASK_WXAPP, TASK_WXAPP, &m, m.length);	

	//最后启动定时器，以便循环读取
	bxxh_timer_set(MSG_WXAPP_3MIN_TIMEOUT, TASK_WXAPP, TIMER_WXAPP_3MIN);
	//不改变状态
    
	return (KE_MSG_CONSUMED);
}


//5S FLASH历史记录的处理，发送到后台
static int handler_wxapp_5second_timtout(ke_msg_id_t const msgid, struct msg_struct_5second_timtout const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//发送数据给ASYLIBRA进行DATA_REQ发送
	if (ke_state_get(TASK_WXAPP) == STATE_WXAPP_CONNECTED)
	{
		if (z_flash_data_sample[z_flash_data_sample_read].sensortype == L3CI_emc_indicator)
		{
			struct msg_struct_wxapp_data_req_emc m;
			m.usercmdid = L3CI_emc_indicator;
			m.emc_value = z_flash_data_sample[z_flash_data_sample_read].emcvalue;
			m.length = sizeof(m);
			struct tm *t1 = localtime(&(z_flash_data_sample[z_flash_data_sample_read].timestamp));		
			m.year = (t1->tm_year+1900-2000) & 0xFF;
			m.month = t1->tm_mon+1;
			m.day = t1->tm_mday;
			m.hour = t1->tm_hour;
			m.minute = t1->tm_min;
			m.second = t1->tm_sec;
			m.gps_x = 0;
			m.gps_y = 0;
			m.ntimes = z_flash_data_sample_nbr;
			m.EmDeviceDataType = EDDT_manufatureSvr; //存储的数据，肯定不会是H5界面来的
			bxxh_message_send(MSG_WXAPP_DATA_REQ_EMC, TASK_ASYLIBRA, TASK_WXAPP, &m, m.length);
		}
		else
		{
			bxxh_print_console("WXAPP: Unexpeceted Flash Record storage. \r\n");
		}
		//处理剩下的数据指针问题
		z_flash_data_sample_nbr--;
		z_flash_data_sample_read++;
		z_flash_data_sample_read = z_flash_data_sample_read % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;		
	}
	//判断连接状态，以及剩余数据，是否需要继续启动定时器
	if ((z_flash_data_sample_nbr > 0) && (ke_state_get(TASK_WXAPP) == STATE_WXAPP_CONNECTED))
	{
		bxxh_timer_set(MSG_WXAPP_5SECOND_TIMEOUT, TASK_WXAPP, TIMER_WXAPP_5SECOND);
	}

	return (KE_MSG_CONSUMED);
}

//DATA_PUSH消息处理过程，纯粹的业务逻辑处理过程
static int  handler_wxapp_data_push(ke_msg_id_t const msgid, struct msg_struct_asylibra_data_push const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	uint8_t UserCmid = param->UserCmdId;
	
	//时钟同步信息
	if (UserCmid == L3CI_time_sync) 
	{
		//时钟同步信息的消息结构，参考《设计要点》中的定义
		time_t t4=0, t3=0, t2=0, t1=0;
		t4 = param->data[0];
		t3 = param->data[1];
		t2 = param->data[2];
		t1 = param->data[3];
		t4 = t4<<24;
		t3 = t3<<16;
		t2 = t2<<8;
		z_system_time_unix = t4+t3+t2+t1;		
		z_system_time_ymd = localtime(&z_system_time_unix);
	}//UserCmid == L3CI_time_sync
	
	
	//设备基本信息
	if (UserCmid == L3CI_equipment_info) 
	{
		//设备基本信息的消息结构，参考《设计要点》中的定义
		//这里不处理任何细节参数部分，未来需要进一步增强
		uint8_t MacAddress[]={0,0,0,0,0,0};
		nvds_tag_len_t length = 6;
		nvds_get(NVDS_TAG_BD_ADDRESS, &length, &MacAddress[0]);
		struct msg_struct_wxapp_equipment_info_sync m;
		m.length = sizeof(m);
		m.hw_type = CURRENT_HW_TYPE;
		m.hw_version = CURRENT_HW_PEM;
		m.sw_release = CURRENT_SW_RELEASE;
		m.sw_delivery = CURRENT_SW_DELIVERY;
		int i = 0;	
		for (i=0; i<6; i++)
		{
			m.hw_uuid[i] = MacAddress[5-i];
		}		
		bxxh_message_send(MSG_WXAPP_EQUIPMENT_INFO_SYNC, TASK_ASYLIBRA, TASK_WXAPP, &m, m.length);
		
	}//UserCmid == L3CI_equipment_info
		
	//电磁辐射瞬时读取
	else if (UserCmid == L3CI_emc_indicator) 
	{
		//固定格式的消息结构，参考《设计要点》中的定义
		struct msg_struct_wxapp_data_req_emc m;
		m.usercmdid = L3CI_emc_indicator;
		m.emc_value = (uint16_t)bxxh_emc_sample();
		m.length = sizeof(m);
		m.year = (z_system_time_ymd->tm_year+1900-2000) & 0xFF;
		m.month = z_system_time_ymd->tm_mon+1;
		m.day = z_system_time_ymd->tm_mday;
		m.hour = z_system_time_ymd->tm_hour;
		m.minute = z_system_time_ymd->tm_min;
		m.second = z_system_time_ymd->tm_sec;
		m.gps_x = 210;
		m.gps_y = 211;
		m.ntimes = 0;
		m.EmDeviceDataType = param->EmDeviceDataType;
        
        sprintf(pDebugPrintBuf, "EMC(I)=%d\r\n", m.emc_value);
        bxxh_print_console(pDebugPrintBuf);
 
		bxxh_message_send(MSG_WXAPP_DATA_REQ_EMC, TASK_ASYLIBRA, TASK_WXAPP, &m, m.length);
		//bxxh_led_flair();
	}//UserCmid == L3CI_emc_indicator

	//不明就里的情况
	else{
		
		sprintf(pDebugPrintBuf, "WXAPP: Receive undefined user command(0x%04X)\r\n", UserCmid);
    bxxh_print_console(pDebugPrintBuf);
	}

	ke_state_set(TASK_WXAPP, STATE_WXAPP_CONNECTED);    
	return (KE_MSG_CONSUMED);
}


//
static int handler_wxapp_period_report_emc(ke_msg_id_t const msgid, struct msg_struct_period_report_emc_trigger *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{

	//准备定时给服务器后台发送汇报数据
	//定时到达，还要处理上行链路是否可用，以便存储数据在本地
	//尝试着将有用的ADC数据发送给ASYLIBRA，然后在ASYLIBRA中组包，并发送出去
	uint16_t EmcResult = (uint16_t)bxxh_emc_sample();
    
    sprintf(pDebugPrintBuf, "EMC(P)=%d\r\n", EmcResult);
    bxxh_print_console(pDebugPrintBuf);
    
	if (ke_state_get(TASK_WXAPP) == STATE_WXAPP_OFFLINE)
	{
		//存储到FLASH，贯穿写入数据
		z_flash_data_sample[z_flash_data_sample_write].sensortype = L3CI_emc_indicator;
		z_flash_data_sample[z_flash_data_sample_write].emcvalue = EmcResult;
		z_flash_data_sample[z_flash_data_sample_write].timestamp = z_system_time_unix;
		z_flash_data_sample_write++;
		z_flash_data_sample_write = z_flash_data_sample_write % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
		//判断是否满，从而回写
		if (z_flash_data_sample_nbr >= FLASH_DATA_SAMPLE_STORAGE_NBR_MAX)
		{
			z_flash_data_sample_nbr = FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
			z_flash_data_sample_read++;
			z_flash_data_sample_read = z_flash_data_sample_read % FLASH_DATA_SAMPLE_STORAGE_NBR_MAX;
		}else{
			z_flash_data_sample_nbr++;
		}	
	}
	else if (ke_state_get(TASK_WXAPP) == STATE_WXAPP_CONNECTED)
	{
		struct msg_struct_wxapp_data_req_emc m;
		m.usercmdid = L3CI_emc_indicator;
		m.emc_value = EmcResult;
		m.length = sizeof(m);
		m.year = (z_system_time_ymd->tm_year+1900-2000) & 0xFF;
		m.month = z_system_time_ymd->tm_mon+1;
		m.day = z_system_time_ymd->tm_mday;
		m.hour = z_system_time_ymd->tm_hour;
		m.minute = z_system_time_ymd->tm_min;
		m.second = z_system_time_ymd->tm_sec;
		m.gps_x = 110;
		m.gps_y = 111;
		m.ntimes = 1;
		m.EmDeviceDataType = EDDT_manufatureSvr; //周期报告，肯定不会是从WX界面H5来的。
		bxxh_message_send(MSG_WXAPP_DATA_REQ_EMC, TASK_ASYLIBRA, TASK_WXAPP, &m, m.length);						
		//MedMsgProcess();
		//bxxh_led_flair();
	}
	//不改变状态
    
	return (KE_MSG_CONSUMED);
	//定时机制是否与SLEEP MODE相互冲突，需要再考虑再研究
}

static int handler_wxapp_disconnect_trigger_l3(ke_msg_id_t const msgid, struct msg_struct_asylibra_disconnect_tigger_l3 const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	//计数器停止，FLASH数据不能接着回传了
	bxxh_timer_clear(MSG_WXAPP_5SECOND_TIMEOUT, TASK_WXAPP);

	
	//设置到初始状态
	ke_state_set(TASK_WXAPP, STATE_WXAPP_OFFLINE);
    
	return (KE_MSG_CONSUMED);
}
