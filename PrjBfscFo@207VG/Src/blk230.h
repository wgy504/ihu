#ifndef _BLK230_H_
#define _BLK230_H_
#include <inttypes.h>

// control bits
#define BLK230_ALARST_BIT          (0x01)
#define BLK230_CW_BIT              (0x02)
#define BLK230_START_BIT           (0x04)
#define BLK230_RUN_BIT             (0x08)

// command list
#define BLK230_CMD_NONE 0
#define BLK230_CMD_STOP 1
#define BLK230_CMD_BRAKE 2
#define BLK230_CMD_CCW 3
#define BLK230_CMD_SPEED 4

// PID parameters
#define BLK230_KP (1)
#define BLK230_KI (0)
#define BLK230_KD (0)

typedef struct blk230_cmd_s
{
	uint32_t cmd:8;
	uint32_t param0:8;
	uint32_t param1:16;
}blk230_cmd_t;

int32_t blk230_init(void);
int32_t blk230_set_dc_speed(uint32_t speed);
int blk230_set_alarm(uint32_t flag);
int blk230_set_stop(uint32_t flag);
int blk230_set_brake(uint32_t flag);
int blk230_set_ccw(uint32_t flag);
int blk230_read_alarm(void);
int blk230_read_speed(void);

#endif // _BLK230_H_
