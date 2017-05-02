/* Motor control for BLH230K BLDC:
 * 1) STM32 PIN PB10 and PB11 via I2C chip PCA8547A.
 * 2) PCA8547A pin 0 
 * 3) speed setting: external DC, speed = DACx_OUT * 625 (r/min)
 *    PA5 as DAC_OUT2
 *    DAC Vref+ is set as 3.3V? Vdda is set to 3.3V?
 *    DACx_OUT=Vref*DORx/4095
 * 4) feedback signal
 *    speed: PA0, alarm: PA7
 */
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_dac.h"

#include "pca8547a.h"
#include "blk230.h"
//#include "bufprint.h"
#include "vmfreeoslayer.h"

#define bufprint IhuDebugPrint

extern DAC_HandleTypeDef hdac;
#define DACx_CHANNEL DAC_CHANNEL_2


int32_t blk230_init()
{
	int32_t status;
	
	/* assume PAC8547A init is done ... */
	
	status = HAL_DAC_Start(&hdac, DACx_CHANNEL);
	if(status!= HAL_OK)
	{
		bufprint("error: failed to start DAC\n");
		return status;
	}
	
	status = blk230_set_dc_speed(0);
	if(status != HAL_OK)
	{
		bufprint("error: failed to set motor speed 0\n");
		return status;
	}
	
	blk230_set_stop(1);
	
	return HAL_OK;
}

void blk230_test_task(void const * argument)
{
	uint32_t testLoop = 0;
	
	PCA8574A_init();
	blk230_init();
	
	while(1)
	{
		osDelay(1000);
		
		uint32_t rpm = 0;
		/* 0~7: speed = 500, run, cw
		* 8~15: speed = 2000, run, cw
		* 16~23: speed = 0, stop
		* 24~31: speed = 1000, run, ccw
		*/
		if((testLoop % 16) == 0)
		{
			rpm=100;
			bufprint("start with speed %d rpm\n", rpm);
			blk230_set_dc_speed(rpm);
			blk230_set_stop(0);
			blk230_set_brake(0);
			blk230_set_ccw(0);
		}
		else if((testLoop % 16) == 4)
		{
			rpm=1000;
			bufprint("start with speed %d rpm\n", rpm);
			blk230_set_dc_speed(rpm);
			blk230_set_stop(0);
			blk230_set_brake(0);
			blk230_set_ccw(0);
		}
		else if((testLoop % 16) == 8)
		{
			rpm=0;
			bufprint("start with speed %d rpm\n", rpm);
			blk230_set_dc_speed(rpm);
			blk230_set_stop(1);
			blk230_set_brake(0);
			blk230_set_ccw(0);
		}
		else if((testLoop % 16) == 10)
		{
			rpm=200;
			bufprint("start with speed %d rpm\n", rpm);
			blk230_set_dc_speed(rpm);
			blk230_set_stop(0);
			blk230_set_brake(0);
			blk230_set_ccw(1);
		}
		else if((testLoop % 16) == 14)
		{
			rpm=0;
			bufprint("start with speed %d rpm\n", rpm);
			blk230_set_dc_speed(rpm);
			blk230_set_stop(1);
			blk230_set_brake(0);
			blk230_set_ccw(1);
		}
		
		testLoop ++;
	}
}

/*
 * motor task framework, TBD
 */
void blk230_task(void *param)
{
	blk230_cmd_t command;
	int32_t status;
	int alarm = 0;
	int target_speed = 0;
	
	while(1)
	{
		command.cmd = BLK230_CMD_NONE;
		status = 0;
		
		/* TBD: wait for command from upper */
		
		
		/* process each command */
		switch(command.cmd)
		{
			case BLK230_CMD_STOP:
				status = blk230_set_stop(command.param0);
				break;
			
			case BLK230_CMD_BRAKE:
				status = blk230_set_brake(command.param0);
				break;
			
			case BLK230_CMD_CCW:
				status = blk230_set_ccw(command.param0);
				break;
			
			case BLK230_CMD_SPEED:
				target_speed = command.param1;
				status = blk230_set_dc_speed(target_speed);
				break;
		}
		
		/* check the status */
		if(status != 0)
		{
			bufprint("warn: status=%d command=0x%x\n", status, *(uint32_t *)(&command));
		}
		
		/* check alarm */
		alarm = blk230_read_alarm();
		if(alarm)
		{
			bufprint("warn: motor alarm! report and reset it...\n", alarm);
			blk230_set_alarm(0);
		}
	}
}

/* speed: r/min
 */
int32_t blk230_set_dc_speed(uint32_t speed)
{
	int32_t status;
	uint32_t DORx;
	
	/* speed = DACx_OUT * 625 (r/min)
	   DACx_OUT=Vref*DORx/4095 (voltage)
	   ==> DORx = speed*4095/625/Vref
	 */
	DORx = (speed*4095*1000)/625/33;
	DORx = DORx / 100;
	
	status = HAL_DAC_SetValue(&hdac, DACx_CHANNEL, DAC_ALIGN_12B_R, DORx);
	if(status != HAL_OK)
		return status;
	
	return HAL_OK;
}

// TBD
int blk230_read_alarm(void)
{
	return 0;
}

// TBD
int blk230_read_speed(void)
{
	return 0;
}

// reset alarm
int blk230_set_alarm(uint32_t flag)
{
	uint8_t flags;
	uint32_t status;
	
	status = PCA8574A_read(&flags);
	if(status == 0)
	{
		flags &= ~BLK230_ALARST_BIT;
		if(flag)
			flags |= BLK230_ALARST_BIT;
		
		status = PCA8574A_write(flags);
	}
	
	return status;
}

// start or stop
int blk230_set_stop(uint32_t flag)
{
	uint8_t flags;
	uint32_t status;
	
	status = PCA8574A_read(&flags);
	if(status == 0)
	{
		flags &= ~BLK230_START_BIT;
		if(flag)
			flags |= BLK230_START_BIT;
		
		status = PCA8574A_write(flags);
	}
	
	return status;
}

// run or brake
int blk230_set_brake(uint32_t flag)
{
	uint8_t flags;
	uint32_t status;
	
	status = PCA8574A_read(&flags);
	if(status == 0)
	{
		flags &= ~BLK230_RUN_BIT;
		if(flag)
			flags |= BLK230_RUN_BIT;
		
		status = PCA8574A_write(flags);
	}
	
	return status;
}

// counterwise or not
int blk230_set_ccw(uint32_t flag)
{
	uint8_t flags;
	uint32_t status;
	
	status = PCA8574A_read(&flags);
	if(status == 0)
	{
		flags &= ~BLK230_CW_BIT;
		if(flag)
			flags |= BLK230_CW_BIT;
		
		status = PCA8574A_write(flags);
	}
	
	return status;
}

