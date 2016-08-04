/**
 ****************************************************************************************
 *
 * @file l3_wxapp_task.h
 *
 * @brief Weixin Layer 3 application task
 *
 * BXXH team
 * Created by ZJL, 20150819
 *
 ****************************************************************************************
 */
#include "bxxh_adapt.h"
#include "l1comdef.h"

extern void task_wxapp_init(void);
extern void task_wxapp_mainloop(void);

/// Number of WXAPP Task Instances
#define WXAPP_IDX_MAX                 (1)

/// States of WXAPP task
enum WXAPP_STATE
{
	/// Disabled State
	STATE_WXAPP_DISABLED,
	/// Connectable state
	STATE_WXAPP_OFFLINE,
	/// Security state,
	//STATE_WXAPP_SECURITY,
	/// Connected state
	STATE_WXAPP_CONNECTED,
	/// Number of defined states.
	STATE_WXAPP_MAX
};

extern const struct ke_state_handler fsm_wxapp_state[STATE_WXAPP_MAX];
extern const struct ke_state_handler handler_wxapp_default;
extern ke_state_t wxapp_state[WXAPP_IDX_MAX];

//Internal procedure
#define TIMER_WXAPP_3MIN        18000	//180s，正常情况下需要设置到3分钟一次的定时汇报，或者找根据系统定义，设置为更长
#define TIMER_WXAPP_5SECOND     150	//5s，用于FLASH数据的回传定时轮询

//FLASH data
#define FLASH_DATA_SAMPLE_STORAGE_NBR_MAX 10
typedef struct flash_data_sample_storage1
{
	uint8_t sensortype;  //0x20->EMC, 0x25->PM1025, ??L3UserCmdId
	uint16_t emcvalue;
	time_t timestamp;
	unsigned int pm1d0;
	unsigned int pm2d5;
	unsigned int pm10;
	uint16_t wind_speed;
	uint16_t wind_direction;
	uint16_t temperature;
	uint16_t humidity;
	uint16_t equipid;
}flash_data_sample_storage1_t;
#define DISC_DATA_SAMPLE_ONLINE 1
#define DISC_DATA_SAMPLE_OFFLINE 2


