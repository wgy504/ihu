#ifndef _BLK230_H_
#define _BLK230_H_
#include <inttypes.h>

// control bits
// 7-6-5-4-3-2-1-0
#define BLK230_ALARST_BIT          (0x01)
#define BLK230_CW_BIT              (0x02)
#define BLK230_START_BIT           (0x04)
#define BLK230_RUN_BIT             (0x08)
#define BLK230_LAMP_OUT1_BIT       (0x10)
#define BLK230_LAMP_OUT2_BIT       (0x20)
#define BLK230_LAMP_OUT3_BIT       (0x40)
#define BLK230_CPU_TRG_OUT1_BIT    (0x80)

#define WMC_LAMP_OUT1								(0x01)
#define WMC_LAMP_OUT2_GREEN					(0x02)
#define WMC_LAMP_OUT3_YELLOW				(0x03)

#define WMC_LAMP_ON 	 		  				(0x00)
#define WMC_LAMP_OFF 	    	 				(0x01)

#define LED_COMMNAD_ON										(1)
#define LED_COMMNAD_OFF									  (2)
#define LED_COMMNAD_BINKING_HIGHSPEED		  (3)
#define LED_COMMNAD_BINKING_LOWSPEED			(4)


// PID parameters
#define BLK230_KP (1)
#define BLK230_KI (0)
#define BLK230_KD (0)

typedef struct blk230_cmd_s
{
  uint32_t valid:1;
  uint32_t stop:1;
  uint32_t brake:1;
  uint32_t ccw:1;
  uint32_t speed:12;      // unit rpm
  uint32_t time2stop:16;  // unit ms
}blk230_cmd_t;

//#define LED_COMMNAD_ON									(1)
//#define LED_COMMNAD_OFF									(2)
//#define LED_COMMNAD_BINKING_HIGHSPEED		(3)
//#define LED_COMMNAD_BINKING_LOWSPEED		(4)
typedef struct blk230_led_cmd_s
{
		uint8_t led_command[4];
}blk230_led_cmd_t;

#define LED_BINKING_HIGHSPEED_MS					(250)
#define LED_BINKING_LOWSPEED_MS						(1000)

int32_t blk230_init(void);
int32_t blk230_set_dc_speed(uint32_t speed);
int blk230_set_alarm(uint32_t flag);
int blk230_set_stop(uint32_t flag);
int blk230_set_brake(uint32_t flag);
int blk230_set_ccw(uint32_t flag);
int blk230_set_lamp(uint32_t lamp_id, uint8_t flag);
int blk230_read_alarm(void);
int blk230_read_speed(void);
int blk230_send_cmd(uint32_t start, uint32_t ccw, uint32_t speed, uint32_t duration);
int blk230_recv_cmd(blk230_cmd_t *command);

int blk230_led_send_cmd(uint32_t led_id, uint8_t led_command);
int blk230_led_recv_cmd(uint8_t *command);

void blk230_task(void const *param);

#endif // _BLK230_H_
