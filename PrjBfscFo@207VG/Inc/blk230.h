#ifndef _BLK230_H_
#define _BLK230_H_
#include <inttypes.h>

// control bits
#define BLK230_ALARST_BIT          (0x01)
#define BLK230_CW_BIT              (0x02)
#define BLK230_START_BIT           (0x04)
#define BLK230_RUN_BIT             (0x08)

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

int32_t blk230_init(void);
int32_t blk230_set_dc_speed(uint32_t speed);
int blk230_set_alarm(uint32_t flag);
int blk230_set_stop(uint32_t flag);
int blk230_set_brake(uint32_t flag);
int blk230_set_ccw(uint32_t flag);
int blk230_read_alarm(void);
int blk230_read_speed(void);
int blk230_send_cmd(uint32_t start, uint32_t ccw, uint32_t speed, uint32_t duration);
int blk230_recv_cmd(blk230_cmd_t *command);
void blk230_task(void const *param);

#endif // _BLK230_H_
