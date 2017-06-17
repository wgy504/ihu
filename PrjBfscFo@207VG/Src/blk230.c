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
#include "cmsis_os.h"

#include "pca8547a.h"
#include "blk230.h"
//#include "bufprint.h"
#include "vmfreeoslayer.h"


#define bufprint IhuDebugPrint

extern DAC_HandleTypeDef hdac;
#define DACx_CHANNEL DAC_CHANNEL_2

blk230_cmd_t g_motor_cmd;

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

	blk230_set_stop(1);
  blk230_set_brake(0);
  
	status = blk230_set_dc_speed(0);
	if(status != HAL_OK)
	{
		bufprint("error: failed to set motor speed 0\n");
		return status;
	}
	
	return HAL_OK;
}

void blk230_test_task_entry(void const * argument)
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

int blk230_send_cmd(uint32_t start, uint32_t ccw, uint32_t speed, uint32_t duration)
{
  blk230_cmd_t command;

  command.valid = 1;
  command.brake = 0;
  command.ccw = ccw;
  command.stop = !start;
  command.speed = speed;
  command.time2stop = duration;
  
  taskENTER_CRITICAL();
  g_motor_cmd = command;
  taskEXIT_CRITICAL();
  
  return g_motor_cmd.valid;
}

int blk230_recv_cmd(blk230_cmd_t *command)
{
  taskENTER_CRITICAL();
  *command = g_motor_cmd;
  g_motor_cmd.valid = 0;
  taskEXIT_CRITICAL();
  
  return command->valid;
}

int blk230_led_send_cmd(uint32_t led_id, uint8_t led_command)
{
	
}

/*
 * motor task framework, TBD
 */
void blk230_task(void const *param)
{
	blk230_cmd_t command;
	int32_t status;
	int alarm = 0;
	uint32_t now, time2stop = 0;

	uint32_t i = 0;

  PCA8574A_init();
	blk230_init();
	

//	blk230_set_lamp(WMC_LAMP_OUT1, WMC_LAMP_ON);
//	osDelay(1000);
//	
//	blk230_set_lamp(WMC_LAMP_OUT1, WMC_LAMP_OFF);
//	osDelay(1000);


	blk230_set_dc_speed(300);
	blk230_set_stop(0);
	blk230_set_brake(0);
	blk230_set_ccw(0);
	
	blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_OFF);
	blk230_set_lamp(WMC_LAMP_OUT3_YELLOW, WMC_LAMP_OFF);
	osDelay(1000);
	
	blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_ON);
	blk230_set_lamp(WMC_LAMP_OUT3_YELLOW, WMC_LAMP_ON);
	osDelay(1000);

	blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_OFF);
	blk230_set_lamp(WMC_LAMP_OUT3_YELLOW, WMC_LAMP_OFF);

	blk230_set_stop(1);


	while(1)
	{
		status = 0;
		
		/* wait for a new command */
		osDelay(10);
		
		i++;
		if(100 == i)
		{
				ihu_bsp_stm32_led_serv1_f2board_toggle();
				i = 0;			
		}
		
    now = osKernelSysTick();
		if(blk230_recv_cmd(&command))
    {
      status = blk230_set_brake(command.brake);
			OS_ASSERT(status == HAL_OK);
      status = blk230_set_ccw(command.ccw);
			OS_ASSERT(status == HAL_OK);
      status = blk230_set_dc_speed(command.speed);
			OS_ASSERT(status == HAL_OK);
      status = blk230_set_stop(command.stop);
			OS_ASSERT(status == HAL_OK);

      if(command.time2stop > 0)
        time2stop = now + command.time2stop;
      else
        time2stop = 0;
			
			//MYC add to change the LIGHT
			if(1 == command.stop)
			{
					blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_OFF);
			}
			else if(0 == command.stop)
			{
					blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_ON);
			}
			else
			{
			}
			
    }

    // need stop motor?
    if((time2stop != 0) && (now > time2stop))
    {
      status = blk230_set_dc_speed(0);
			OS_ASSERT(status == HAL_OK);
      status = blk230_set_stop(1);
			OS_ASSERT(status == HAL_OK);
      time2stop = 0;
			
			blk230_set_lamp(WMC_LAMP_OUT2_GREEN, WMC_LAMP_OFF);

    }
		
		/* check alarm */
		alarm = blk230_read_alarm();
		if(alarm)
		{
			bufprint("warn: motor alarm! report and reset it...\n", alarm);
      status = blk230_set_stop(1);
			OS_ASSERT(status == HAL_OK);
      status = blk230_set_dc_speed(0);
			OS_ASSERT(status == HAL_OK);
			status = blk230_set_alarm(0);
			OS_ASSERT(status == HAL_OK);
      time2stop = 0;
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

/*
**
**
** #define WMC_LAMP_OUT1   	  				(0x01)
** #define WMC_LAMP_OUT2     	 				(0x02)
** #define WMC_LAMP_OUT3			  	    (0x03)

** #define WMC_LAMP_OFF  		  				(0x00)
** #define WMC_LAMP_ON 	    	 				(0x01)
** 
** blk230_set_lamp(WMC_LAMP_OUT1, WMC_LAMP_ON);
** blk230_set_lamp(WMC_LAMP_OUT1, WMC_LAMP_OFF);
*/

int blk230_set_lamp(uint32_t lamp_id, uint8_t flag)
{
	uint8_t flags;
	uint32_t status;
	uint8_t lamp_mask;
	
	if(WMC_LAMP_OUT1 == lamp_id)
	{
			lamp_mask = BLK230_LAMP_OUT1_BIT;
	}
	else if(WMC_LAMP_OUT2_GREEN == lamp_id)
	{
			lamp_mask = BLK230_LAMP_OUT2_BIT;
	}
	else if (WMC_LAMP_OUT3_YELLOW == lamp_id)
	{
			lamp_mask = BLK230_LAMP_OUT3_BIT;
	}
	else
	{
			//do nothing
	}
	
//	flag = 1; // for ON
//	flag = 0; // for OFF
	
	status = PCA8574A_read(&flags);
	//if(status == 0)
	//{
		flags &= ~lamp_mask;
		if(flag)
			flags |= lamp_mask;
		
		status = PCA8574A_write(flags);
	//}
	
	return status;	
}