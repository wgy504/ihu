/**
 ****************************************************************************************
 *
 * @file l3_asylibra_task.h
 *
 * @brief Weixin Asylibra application task
 *
 * BXXH team
 * Created by ZJL, 20150819
 *
 ****************************************************************************************
 */

#include "bxxh_adapt.h"
#include "l1comdef.h"

extern void task_asylibra_init(void);
extern void task_asylibra_mainloop(void);

/// Number of ASYLIBRA Task Instances
#define ASYLIBRA_IDX_MAX                 (1)

/// States of ASYLIBRA task
//MYC added for AirSync Protocal
enum ASYLIBRA_STATE
{
	STATE_ASYLIBRA_DISABLED,  //Used
	//STATE_ASYLIBRA_BLE_INIT,
	STATE_ASYLIBRA_BLE_CONNECTED,  //Used
	//STATE_ASYLIBRA_WAIT_AUTHRESP,
	STATE_ASYLIBRA_AUTH_OK,  //Used
	//STATE_ASYLIBRA_WAIT_INITRESP,
	STATE_ASYLIBRA_INIT_OK_DATA_READY, //Used
	STATE_ASYLIBRA_MAX,
};

//用于存放InitResp中全局数据表而设立的结构
struct struct_init_resp_global_data
{
	bool flag;
	uint32_t user_id_high;              //2=微信用户高32位
	uint32_t user_id_low;               //3=微信用户低32位
	uint32_t challeange_answer;         //4=手机回复的挑战值，为设备生成字节的CRC32
	uint8_t init_scence;                //5=微信连上设备时，处于什么情景
	uint32_t auto_sync_max_duration_second;  //6=自动同步持续多长时间，微信就会关闭。 0xffffffff表示无限长时间
	char user_nick_name[15];            //11=微信用户昵称
	uint16_t platform_type;             //12=手机平台
	char model[15];                     //13=手机硬件型号 
	char os[15];                        //14=手机OS版本
	int32_t time;                       //15=手机当前时间
	int32_t time_zone;                  //16=手机当前时区
	char time_string[15];               //17=手机当前时间，格式为201402281005285，2014(Y)02(M)28(D)10(H)05(M)28(S)5(W)
};

//to be understood
extern const struct ke_state_handler fsm_asylibra_state[STATE_ASYLIBRA_MAX];
extern const struct ke_state_handler handler_asylibra_default;
extern ke_state_t asylibra_state[ASYLIBRA_IDX_MAX];

//internal procedure
//void ReadAdcSendWeChat(uint16_t adcvalue);
#define TIMER_ASYLIBRA_AUTH_WAIT         100	//1s, chang to 3S for test stability. 
#define TIMER_ASYLIBRA_SEND_DATA_OUT_2S  50	//原来2S轮一次，查看是否有数据未发送出去， 任务中的数据综合量，不能比这个频率高，为了JSAPI，优化到500ms水平
#define ASY_HEADER_MAGICNUMBER  0xFE
#define ASY_HEADER_BVERSION  0x01
