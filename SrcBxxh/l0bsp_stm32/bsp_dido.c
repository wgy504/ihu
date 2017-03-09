/**
  ******************************************************************************
  * 文件名程: bsp_dido.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 基于HAL库的板载LED灯底层驱动实现
  ******************************************************************************
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_dido.h"

//从MAIN.x中继承过来的函数

//本地全局变量



/* 函数体 --------------------------------------------------------------------*/
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
//该传感器的具体读数：以两位数表达的整数
int16_t ihu_bsp_stm32_dido_f2board_dht11_temp_read(void)
{
	StrDht11DataTypeDef_t output;
	memset(&output, 0, sizeof(StrDht11DataTypeDef_t));
	if (func_bsp_stm32_dido_dht11_read_temp_and_humidity(&output) == SUCCESS){
		return (int16_t)(output.temperature*100);
	}else{
		return 0xFFFF;
	}
}

//该传感器的具体读数：以两位数表达的整数
int16_t ihu_bsp_stm32_dido_f2board_dht11_humid_read(void)
{
	StrDht11DataTypeDef_t output;
	memset(&output, 0, sizeof(StrDht11DataTypeDef_t));
	if (func_bsp_stm32_dido_dht11_read_temp_and_humidity(&output) == SUCCESS){
		return (int16_t)(output.humidity*100);
	}else{
		return 0xFFFF;
	}
}

int16_t ihu_bsp_stm32_dido_f2board_fall_read(void)
{
	if (BSP_STM32_DIDO_FALL_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}				

//SW420读取函数：因为这是最为普通的GPIO口，没有任何的特别，所以不再为SW420建立独立的读取过程，而是采用非常通用的
//读取函数
//为了改善稳定性，读取三次，取两次做判定
int16_t ihu_bsp_stm32_dido_f2board_shake_read(void)
{
	int8_t res[3];
	int8_t total = 0;
	
	memset(res, 0, sizeof(res));
	res[0] = ((BSP_STM32_DIDO_SHAKE_READ == GPIO_PIN_RESET)?1:-1);
	ihu_usleep(100);
	res[0] = ((BSP_STM32_DIDO_SHAKE_READ == GPIO_PIN_RESET)?1:-1);
	ihu_usleep(100);
	res[0] = ((BSP_STM32_DIDO_SHAKE_READ == GPIO_PIN_RESET)?1:-1);

	total = res[0] + res[1] + res[2];
	
	if (total > 0)
		return FALSE;
	else
		return TRUE;
}					

int16_t ihu_bsp_stm32_dido_f2board_smoke_read(void)
{
	if (BSP_STM32_DIDO_SMOKE_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}					
								
int16_t ihu_bsp_stm32_dido_f2board_water_read(void)
{
	if (BSP_STM32_DIDO_WATER_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;	
}					
								
int16_t ihu_bsp_stm32_dido_f2board_lock1_di1_trigger_read(void)
{
	if (BSP_STM32_DIDO_LOCK1_DI1_TRIGGER_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;		
}									

int16_t ihu_bsp_stm32_dido_f2board_lock1_di2_tongue_read(void)
{
	if (BSP_STM32_DIDO_LOCK1_DI2_TONGUE_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;			
}

void ihu_bsp_stm32_dido_f2board_lock1_do1_on(void)
{
	BSP_STM32_DIDO_LOCK1_DO1_ON;
}

void ihu_bsp_stm32_dido_f2board_lock1_do1_off(void)
{
	BSP_STM32_DIDO_LOCK1_DO1_OFF;
}

int16_t ihu_bsp_stm32_dido_f2board_door1_restriction_read(void)
{
	if (BSP_STM32_DIDO_DOOR1_RESTRICTION_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}         
            
int16_t ihu_bsp_stm32_dido_f2board_lock2_di1_trigger_read(void)
{
	if (BSP_STM32_DIDO_LOCK2_DI1_TRIGGER_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}									

int16_t ihu_bsp_stm32_dido_f2board_lock2_di2_tongue_read(void)
{
	if (BSP_STM32_DIDO_LOCK2_DI2_TONGUE_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}

void ihu_bsp_stm32_dido_f2board_lock2_do1_on(void)
{
    BSP_STM32_DIDO_LOCK2_DO1_ON;
}

void ihu_bsp_stm32_dido_f2board_lock2_do1_off(void)
{
    BSP_STM32_DIDO_LOCK2_DO1_OFF;
}

int16_t ihu_bsp_stm32_dido_f2board_door2_restriction_read(void)
{
	if (BSP_STM32_DIDO_DOOR2_RESTRICTION_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}   	

int16_t ihu_bsp_stm32_dido_f2board_lock3_di1_trigger_read(void)
{
	if (BSP_STM32_DIDO_LOCK3_DI1_TRIGGER_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}

int16_t ihu_bsp_stm32_dido_f2board_lock3_di2_tongue_read(void)
{
	if (BSP_STM32_DIDO_LOCK3_DI2_TONGUE_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}

void ihu_bsp_stm32_dido_f2board_lock3_do1_on(void)
{
    BSP_STM32_DIDO_LOCK3_DO1_ON;
}

void ihu_bsp_stm32_dido_f2board_lock3_do1_off(void)
{
    BSP_STM32_DIDO_LOCK3_DO1_OFF;
}

int16_t ihu_bsp_stm32_dido_f2board_door3_restriction_read(void)
{
	if (BSP_STM32_DIDO_DOOR3_RESTRICTION_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}   	

int16_t ihu_bsp_stm32_dido_f2board_lock4_di1_trigger_read(void)
{
	if (BSP_STM32_DIDO_LOCK4_DI1_TRIGGER_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;
}									

int16_t ihu_bsp_stm32_dido_f2board_lock4_di2_tongue_read(void)
{
	if (BSP_STM32_DIDO_LOCK4_DI2_TONGUE_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;	
}

void ihu_bsp_stm32_dido_f2board_lock4_do1_on(void)
{
    BSP_STM32_DIDO_LOCK4_DO1_ON;
}

void ihu_bsp_stm32_dido_f2board_lock4_do1_off(void)
{
    BSP_STM32_DIDO_LOCK4_DO1_OFF;
}

int16_t ihu_bsp_stm32_dido_f2board_door4_restriction_read(void)
{
	if (BSP_STM32_DIDO_DOOR4_RESTRICTION_READ == GPIO_PIN_RESET)
		return FALSE;
	else
		return TRUE;		
}   	

void ihu_bsp_stm32_dido_f2board_gprsmod_power_ctrl_on(void)
{
    BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_ON;
}

void ihu_bsp_stm32_dido_f2board_gprsmod_power_ctrl_off(void)
{
    BSP_STM32_DIDO_GPRSMOD_PWR_CTRL_OFF;
}

void ihu_bsp_stm32_dido_f2board_ble_power_ctrl_on(void)
{
    BSP_STM32_DIDO_BLE_PWR_CTRL_ON;
}

void ihu_bsp_stm32_dido_f2board_ble_power_ctrl_off(void)
{
    BSP_STM32_DIDO_BLE_PWR_CTRL_OFF;
}

void ihu_bsp_stm32_dido_f2board_rfid_power_ctrl_on(void)
{
    BSP_STM32_DIDO_RFID_PWR_CTRL_ON;
}

void ihu_bsp_stm32_dido_f2board_rfid_power_ctrl_off(void)
{
    BSP_STM32_DIDO_RFID_PWR_CTRL_OFF;
}

void ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_on(void)
{
    BSP_STM32_DIDO_SENSOR_PWR_CTRL_ON;
}

void ihu_bsp_stm32_dido_f2board_sensor_power_ctrl_off(void)
{
    BSP_STM32_DIDO_SENSOR_PWR_CTRL_OFF;
}      		

void ihu_bsp_stm32_dido_f2board_ble_atcmd_mode_ctrl_on(void)
{
    BSP_STM32_DIDO_BLE_ATCMD_CTRL_ON;
}

void ihu_bsp_stm32_dido_f2board_ble_atcmd_mode_ctrl_off(void)
{
    BSP_STM32_DIDO_BLE_ATCMD_CTRL_OFF;
}







//公共函数

static void DHT11_Delay(uint16_t time)
{
	uint8_t i;

  while(time)
  {    
	  for (i = 0; i < 10; i++)
    {
      
    }
    time--;
  }
}

/**
  * 函数功能: DHT11 初始化函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void ihu_bsp_stm32_dido_f2board_dht11_init ( void )
{
  BSP_STM32_DIDO_DHT11_Dout_GPIO_CLK_ENABLE();
  
	func_bsp_stm32_dido_dht11_mode_out_PP();
	
	BSP_STM32_DIDO_DHT11_Dout_HIGH();  // 拉高GPIO
}

/**
  * 函数功能: 使DHT11-DATA引脚变为上拉输入模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void func_bsp_stm32_dido_dht11_mode_IPU(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.Pin   = BSP_STM32_DIDO_DHT11_GPIO_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(BSP_STM32_DIDO_DHT11_GPIO, &GPIO_InitStruct);
	
}

/**
  * 函数功能: 使DHT11-DATA引脚变为推挽输出模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void func_bsp_stm32_dido_dht11_mode_out_PP(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.Pin = BSP_STM32_DIDO_DHT11_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_STM32_DIDO_DHT11_GPIO, &GPIO_InitStruct); 	 
}

/**
  * 函数功能: 从DHT11读取一个字节，MSB先行
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static uint8_t func_bsp_stm32_dido_dht11_read_byte ( void )
{
	uint8_t i, temp=0;
	

	for(i=0;i<8;i++)    
	{	 
		/*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
		while(BSP_STM32_DIDO_DHT11_Data_IN()==GPIO_PIN_RESET);

		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
		 */
		DHT11_Delay(40); //延时x us 这个延时需要大于数据0持续的时间即可	   	  

		if(BSP_STM32_DIDO_DHT11_Data_IN()==GPIO_PIN_SET)/* x us后仍为高电平表示数据“1” */
		{
			/* 等待数据1的高电平结束 */
			while(BSP_STM32_DIDO_DHT11_Data_IN()==GPIO_PIN_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
		}
		else	 // x us后为低电平表示数据“0”
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
		}
	}
	return temp;
}


/*
 * 
 * 
 */
/**
  * 函数功能: 一次完整的数据传输为40bit，高位先出
  * 输入参数: DHT11_Data:DHT11数据类型
  * 返 回 值: ERROR：  读取出错
  *           SUCCESS：读取成功
  * 说    明：8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
  */
uint8_t func_bsp_stm32_dido_dht11_read_temp_and_humidity(StrDht11DataTypeDef_t *DHT11_Data)
{  
  uint8_t temp;
  uint16_t humi_temp;
  
	/*输出模式*/
	func_bsp_stm32_dido_dht11_mode_out_PP();
	/*主机拉低*/
	BSP_STM32_DIDO_DHT11_Dout_LOW();
	/*延时18ms*/
	ihu_usleep(18);

	/*总线拉高 主机延时30us*/
	BSP_STM32_DIDO_DHT11_Dout_HIGH(); 

	DHT11_Delay(30);   //延时30us

	/*主机设为输入 判断从机响应信号*/ 
	func_bsp_stm32_dido_dht11_mode_IPU();

	/*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/   
	if(BSP_STM32_DIDO_DHT11_Data_IN()==GPIO_PIN_RESET)     
	{
    /*轮询直到从机发出 的80us 低电平 响应信号结束*/  
    while(BSP_STM32_DIDO_DHT11_Data_IN()==GPIO_PIN_RESET);

    /*轮询直到从机发出的 80us 高电平 标置信号结束*/
    while(BSP_STM32_DIDO_DHT11_Data_IN()==GPIO_PIN_SET);

    /*开始接收数据*/   
    DHT11_Data->humi_high8bit= func_bsp_stm32_dido_dht11_read_byte();
    DHT11_Data->humi_low8bit = func_bsp_stm32_dido_dht11_read_byte();
    DHT11_Data->temp_high8bit= func_bsp_stm32_dido_dht11_read_byte();
    DHT11_Data->temp_low8bit = func_bsp_stm32_dido_dht11_read_byte();
    DHT11_Data->check_sum    = func_bsp_stm32_dido_dht11_read_byte();

    /*读取结束，引脚改为输出模式*/
    func_bsp_stm32_dido_dht11_mode_out_PP();
    /*主机拉高*/
    BSP_STM32_DIDO_DHT11_Dout_HIGH();
    
    /* 对数据进行处理 */
    humi_temp=DHT11_Data->humi_high8bit*100+DHT11_Data->humi_low8bit;
    DHT11_Data->humidity =(float)humi_temp/100;
    
    humi_temp=DHT11_Data->temp_high8bit*100+DHT11_Data->temp_low8bit;
    DHT11_Data->temperature=(float)humi_temp/100;    
    
    /*检查读取的数据是否正确*/
    temp = DHT11_Data->humi_high8bit + DHT11_Data->humi_low8bit + 
           DHT11_Data->temp_high8bit+ DHT11_Data->temp_low8bit;
    if(DHT11_Data->check_sum==temp)
    { 
      return SUCCESS;
    }
    else 
      return ERROR;
	}	
	else
		return ERROR;
}


#endif //(IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)






/**
  * 函数功能: 
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DS18B20_Delay(uint16_t time)
{
	uint8_t i;

  while(time)
  {    
	  for (i = 0; i < 10; i++)
    {
      
    }
    time--;
  }
}

/**
  * 函数功能: DS18B20 初始化函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
uint8_t ihu_bsp_stm32_dido_ds18b20_init(void)
{
  BSP_STM32_DIDO_DS18B20_Dout_GPIO_CLK_ENABLE();
  
  func_bsp_stm32_dido_ds18b20_set_mode_out_PP();
	
	BSP_STM32_DIDO_DS18B20_Dout_HIGH();
	
	func_bsp_stm32_dido_ds18b20_reset();
  
  return func_bsp_stm32_dido_ds18b20_presence ();
}


/**
  * 函数功能: 使DS18B20-DATA引脚变为上拉输入模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void func_bsp_stm32_dido_ds18b20_set_mode_IPU(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.Pin   = BSP_STM32_DIDO_DS18B20_Dout_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(BSP_STM32_DIDO_DS18B20_Dout_PORT, &GPIO_InitStruct);
	
}

/**
  * 函数功能: 使DS18B20-DATA引脚变为推挽输出模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void func_bsp_stm32_dido_ds18b20_set_mode_out_PP(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.Pin = BSP_STM32_DIDO_DS18B20_Dout_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_STM32_DIDO_DS18B20_Dout_PORT, &GPIO_InitStruct); 	 
}

/**
  * 函数功能: 主机给从机发送复位脉冲
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void func_bsp_stm32_dido_ds18b20_reset(void)
{
	/* 主机设置为推挽输出 */
	func_bsp_stm32_dido_ds18b20_set_mode_out_PP();
	
	BSP_STM32_DIDO_DS18B20_Dout_LOW();
  
	/* 主机至少产生480us的低电平复位信号 */
	DS18B20_Delay(750);
	
	/* 主机在产生复位信号后，需将总线拉高 */
	BSP_STM32_DIDO_DS18B20_Dout_HIGH();
	
	/*从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲*/
	DS18B20_Delay(15);
}

/**
  * 函数功能: 检测从机给主机返回的存在脉冲
  * 输入参数: 无
  * 返 回 值: 0：成功，1：失败
  * 说    明：无
  */
static uint8_t func_bsp_stm32_dido_ds18b20_presence(void)
{
	uint8_t pulse_time = 0;
	
	/* 主机设置为上拉输入 */
	func_bsp_stm32_dido_ds18b20_set_mode_IPU();
	
	/* 等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号 
	 * 如果存在脉冲没有来则做超时处理，从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
	 */
	while( BSP_STM32_DIDO_DS18B20_Data_IN() && pulse_time<100 )
	{
		pulse_time++;
		DS18B20_Delay(1);
	}	
	/* 经过100us后，存在脉冲都还没有到来*/
	if( pulse_time >=100 )
		return 1;
	else
		pulse_time = 0;
	
	/* 存在脉冲到来，且存在的时间不能超过240us */
	while( !BSP_STM32_DIDO_DS18B20_Data_IN() && pulse_time<240 )
	{
		pulse_time++;
		DS18B20_Delay(1);
	}	
	if( pulse_time >=240 )
		return 1;
	else
		return 0;
}

/**
  * 函数功能: 从DS18B20读取一个bit
  * 输入参数: 无
  * 返 回 值: 读取到的数据
  * 说    明：无
  */
static uint8_t func_bsp_stm32_dido_ds18b20_read_bit(void)
{
	uint8_t dat;
	
	/* 读0和读1的时间至少要大于60us */	
	func_bsp_stm32_dido_ds18b20_set_mode_out_PP();
	/* 读时间的起始：必须由主机产生 >1us <15us 的低电平信号 */
	BSP_STM32_DIDO_DS18B20_Dout_LOW();
	DS18B20_Delay(10);
	
	/* 设置成输入，释放总线，由外部上拉电阻将总线拉高 */
	func_bsp_stm32_dido_ds18b20_set_mode_IPU();
	//Delay_us(2);
	
	if( BSP_STM32_DIDO_DS18B20_Data_IN() == SET )
		dat = 1;
	else
		dat = 0;
	
	/* 这个延时参数请参考时序图 */
	DS18B20_Delay(45);
	
	return dat;
}

/**
  * 函数功能: 从DS18B20读一个字节，低位先行
  * 输入参数: 无
  * 返 回 值: 读到的数据
  * 说    明：无
  */
static uint8_t func_bsp_stm32_dido_ds18b20_read_byte(void)
{
	uint8_t i, j, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		j = func_bsp_stm32_dido_ds18b20_read_bit();		
		dat = (dat) | (j<<i);
	}
	
	return dat;
}

/**
  * 函数功能: 写一个字节到DS18B20，低位先行
  * 输入参数: dat：待写入数据
  * 返 回 值: 无
  * 说    明：无
  */
static void func_bsp_stm32_dido_ds18b20_write_byte(uint8_t dat)
{
	uint8_t i, testb;
	func_bsp_stm32_dido_ds18b20_set_mode_out_PP();
	
	for( i=0; i<8; i++ )
	{
		testb = dat&0x01;
		dat = dat>>1;		
		/* 写0和写1的时间至少要大于60us */
		if (testb)
		{			
			BSP_STM32_DIDO_DS18B20_Dout_LOW();
			/* 1us < 这个延时 < 15us */
			DS18B20_Delay(8);
			
			BSP_STM32_DIDO_DS18B20_Dout_HIGH();
			DS18B20_Delay(58);
		}		
		else
		{			
			BSP_STM32_DIDO_DS18B20_Dout_LOW();
			/* 60us < Tx 0 < 120us */
			DS18B20_Delay(70);
			
			BSP_STM32_DIDO_DS18B20_Dout_HIGH();		
			/* 1us < Trec(恢复时间) < 无穷大*/
			DS18B20_Delay(2);
		}
	}
}

/**
  * 函数功能: 跳过匹配 DS18B20 ROM
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void func_bsp_stm32_dido_ds18b20_skip_rom ( void )
{
	func_bsp_stm32_dido_ds18b20_reset();	   	
	func_bsp_stm32_dido_ds18b20_presence();	 	
	func_bsp_stm32_dido_ds18b20_write_byte(0XCC);		/* 跳过 ROM */	
}

/**
  * 函数功能: 执行匹配 DS18B20 ROM
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void func_bsp_stm32_dido_ds18b20_match_rom ( void )
{
	func_bsp_stm32_dido_ds18b20_reset();	   	
	func_bsp_stm32_dido_ds18b20_presence();	 	
	func_bsp_stm32_dido_ds18b20_write_byte(0X55);		/* 匹配 ROM */	
}


/*
 * 存储的温度是16 位的带符号扩展的二进制补码形式
 * 当工作在12位分辨率时，其中5个符号位，7个整数位，4个小数位
 *
 *         |---------整数----------|-----小数 分辨率 1/(2^4)=0.0625----|
 * 低字节  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----符号位：0->正  1->负-------|-----------整数-----------|
 * 高字节  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * 温度 = 符号位 + 整数 + 小数*0.0625
 */
/**
  * 函数功能: 在跳过匹配 ROM 情况下获取 DS18B20 温度值 
  * 输入参数: 无
  * 返 回 值: 温度值
  * 说    明：无
  */
float func_bsp_stm32_dido_ds18b20_get_temp_skip_rom ( void )
{
	uint8_t tpmsb, tplsb;
	short s_tem;
	float f_tem;
	
	
	func_bsp_stm32_dido_ds18b20_skip_rom ();
	func_bsp_stm32_dido_ds18b20_write_byte(0X44);				/* 开始转换 */
	
	
	func_bsp_stm32_dido_ds18b20_skip_rom ();
  func_bsp_stm32_dido_ds18b20_write_byte(0XBE);				/* 读温度值 */
	
	tplsb = func_bsp_stm32_dido_ds18b20_read_byte();		 
	tpmsb = func_bsp_stm32_dido_ds18b20_read_byte(); 
	
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* 负温度 */
		f_tem = (~s_tem+1) * 0.0625;	
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem; 	
}

/**
  * 函数功能: 在匹配 ROM 情况下获取 DS18B20 温度值 
  * 输入参数: ds18b20_id：用于存放 DS18B20 序列号的数组的首地址
  * 返 回 值: 无
  * 说    明：无
  */
void func_bsp_stm32_dido_ds18b20_read_id ( uint8_t * ds18b20_id )
{
	uint8_t uc;
		
	func_bsp_stm32_dido_ds18b20_write_byte(0x33);       //读取序列号
	
	for ( uc = 0; uc < 8; uc ++ )
	  ds18b20_id [ uc ] = func_bsp_stm32_dido_ds18b20_read_byte();	
}

/**
  * 函数功能: 在匹配 ROM 情况下获取 DS18B20 温度值 
  * 输入参数: ds18b20_id：存放 DS18B20 序列号的数组的首地址
  * 返 回 值: 温度值
  * 说    明：无
  */
float func_bsp_stm32_dido_ds18b20_get_temp_match_rom ( uint8_t * ds18b20_id )
{
	uint8_t tpmsb, tplsb, i;
	short s_tem;
	float f_tem;
	
	
	func_bsp_stm32_dido_ds18b20_match_rom ();            //匹配ROM
	
  for(i=0;i<8;i++)
		func_bsp_stm32_dido_ds18b20_write_byte ( ds18b20_id [ i ] );	
	
	func_bsp_stm32_dido_ds18b20_write_byte(0X44);				/* 开始转换 */

	
	func_bsp_stm32_dido_ds18b20_match_rom ();            //匹配ROM
	
	for(i=0;i<8;i++)
		func_bsp_stm32_dido_ds18b20_write_byte ( ds18b20_id [ i ] );	
	
	func_bsp_stm32_dido_ds18b20_write_byte(0XBE);				/* 读温度值 */
	
	tplsb = func_bsp_stm32_dido_ds18b20_read_byte();		 
	tpmsb = func_bsp_stm32_dido_ds18b20_read_byte(); 
	
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* 负温度 */
		f_tem = (~s_tem+1) * 0.0625;	
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem; 		
}

//读取温度，以2位小数为整数的数据格式
int16_t ihu_bsp_stm32_dido_f2board_ds18b20_temp_read(void)
{
	uint8_t DS18B20ID[8];
	float temperature;
	func_bsp_stm32_dido_ds18b20_get_temp_match_rom(DS18B20ID);
	temperature=func_bsp_stm32_dido_ds18b20_get_temp_match_rom(DS18B20ID);
	return (int16_t)(temperature * 100) & 0xFFFF;	
}




