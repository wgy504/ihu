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

int16_t ihu_bsp_stm32_dido_f2board_shake_read(void)
{
    return BSP_STM32_DIDO_SHAKE_READ;
}					

int16_t ihu_bsp_stm32_dido_f2board_smoke_read(void)
{
    return BSP_STM32_DIDO_SMOKE_READ;
}					
								
int16_t ihu_bsp_stm32_dido_f2board_water_read(void)
{
    return BSP_STM32_DIDO_WATER_READ;
}					
								
int16_t ihu_bsp_stm32_dido_f2board_lock1_di1_trigger_read(void)
{
    return BSP_STM32_DIDO_LOCK1_DI1_TRIGGER_READ;
}									

int16_t ihu_bsp_stm32_dido_f2board_lock1_di2_tongue_read(void)
{
    return BSP_STM32_DIDO_LOCK1_DI2_TONGUE_READ;
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
    return BSP_STM32_DIDO_DOOR1_RESTRICTION_READ;
}         
            
int16_t ihu_bsp_stm32_dido_f2board_lock2_di1_trigger_read(void)
{
    return BSP_STM32_DIDO_LOCK2_DI1_TRIGGER_READ;
}									

int16_t ihu_bsp_stm32_dido_f2board_lock2_di2_tongue_read(void)
{
    return BSP_STM32_DIDO_LOCK2_DI2_TONGUE_READ;
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
    return BSP_STM32_DIDO_DOOR2_RESTRICTION_READ;
}   	

int16_t ihu_bsp_stm32_dido_f2board_lock3_di1_trigger_read(void)
{
    return BSP_STM32_DIDO_LOCK3_DI1_TRIGGER_READ;
}

int16_t ihu_bsp_stm32_dido_f2board_lock3_di2_tongue_read(void)
{
    return BSP_STM32_DIDO_LOCK3_DI2_TONGUE_READ;
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
    return BSP_STM32_DIDO_DOOR3_RESTRICTION_READ;
}   	

int16_t ihu_bsp_stm32_dido_f2board_lock4_di1_trigger_read(void)
{
    return BSP_STM32_DIDO_LOCK4_DI1_TRIGGER_READ;
}									

int16_t ihu_bsp_stm32_dido_f2board_lock4_di2_tongue_read(void)
{
    return BSP_STM32_DIDO_LOCK4_DI2_TONGUE_READ;
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
    return BSP_STM32_DIDO_DOOR4_RESTRICTION_READ;
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










