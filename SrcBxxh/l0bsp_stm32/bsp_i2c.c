/**
  ******************************************************************************
  * 文件名程: bsp_i2c.c 
  * 作    者: BXXH
  * 版    本: V1.0
  * 编写日期: 2016-10-04
  * 功    能: 板载调试串口底层驱动程序：默认使用I2C1
  ******************************************************************************
  * 
  ******************************************************************************
*/

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_i2c.h"

//从MAIN.x中继承过来的函数
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
extern I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;  //MAIN中未定义，这里重新定义是为了复用
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
extern I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;  //MAIN中未定义，这里重新定义是为了复用
#else
	#error Un-correct constant definition
#endif
extern uint8_t zIhuI2cRxBuffer[2];

//本地全局变量
int8_t zIhuBspStm32I2c1RxBuff[IHU_BSP_STM32_I2C1_REC_MAX_LEN];		//数据接收缓冲区 
int8_t zIhuBspStm32I2c1RxState=0;																	//接收状态
int16_t zIhuBspStm32I2c1RxCount=0;																//当前接收数据的字节数
int16_t zIhuBspStm32I2c1RxLen=0;
int8_t zIhuBspStm32I2c2RxBuff[IHU_BSP_STM32_I2C2_REC_MAX_LEN];		//数据接收缓冲区 
int8_t zIhuBspStm32I2c2RxState=0;																	//接收状态
int16_t zIhuBspStm32I2c2RxCount=0;																//当前接收数据的字节数 	  
int16_t zIhuBspStm32I2c2RxLen=0;

/* 函数体 --------------------------------------------------------------------*/

int ihu_bsp_stm32_i2c_slave_hw_init(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_I2C1_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32I2c1RxBuff[k] = 0x00;
	}
  zIhuBspStm32I2c1RxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32I2c1RxState = 0;
	zIhuBspStm32I2c1RxLen = 0;

	for(k=0;k<IHU_BSP_STM32_I2C2_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32I2c2RxBuff[k] = 0x00;
	}
  zIhuBspStm32I2c2RxCount = 0;               //接收字符串的起始存储位置
	zIhuBspStm32I2c2RxState = 0;
	zIhuBspStm32I2c2RxLen = 0;	
	
	return BSP_SUCCESS;
}

/*******************************************************************************
* 函数名  : I2C_SendData
* 描述    : I2C_IAU发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_i2c_iau_send_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_I2C_Slave_Transmit(&IHU_BSP_STM32_I2C_IAU_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_I2C_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_i2c_iau_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_I2C_Slave_Receive(&IHU_BSP_STM32_I2C_IAU_HANDLER, buff, len, IHU_BSP_STM32_I2C_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}



/*******************************************************************************
* 函数名  : I2C_SendData
* 描述    : I2C_SPARE1发送数据缓冲区数据
* 输入    : *buff：数据缓冲区指针，len：发送数据长度
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
int ihu_bsp_stm32_i2c_spare1_send_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_I2C_Slave_Transmit(&IHU_BSP_STM32_I2C_SPARE1_HANDLER, (uint8_t *)buff, len, IHU_BSP_STM32_I2C_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;		
}

int ihu_bsp_stm32_i2c_spare1_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_I2C_Slave_Receive(&IHU_BSP_STM32_I2C_SPARE1_HANDLER, buff, len, IHU_BSP_STM32_I2C_RX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

/**
  * 串口接口完成回调函数的处理
  * 为什么需要重新执行HAL_I2C_Receive_IT，
	* 接收只使用IT模式，因为这么模式比较方便
  */
void HAL_I2C_RxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
#if (BSP_STM32_I2C_WORK_MODE_CHOICE == BSP_STM32_I2C_WORK_MODE_IAU)
	uint8_t res = 0;
	msg_struct_i2caries_l2frame_rcv_t snd;
	//处于SLAVE MODE
  if(I2cHandle==&IHU_BSP_STM32_I2C_IAU_HANDLER)
  {
		res = zIhuI2cRxBuffer[IHU_BSP_STM32_I2C_IAU_HANDLER_ID-1];
		zIhuBspStm32I2c1RxBuff[zIhuBspStm32I2c1RxCount++] = res;
		if (zIhuBspStm32I2c1RxCount >= IHU_BSP_STM32_I2C1_REC_MAX_LEN)
			zIhuBspStm32I2c1RxCount = 0;

		//为了IDLE状态下提高效率，直接分解为IDLE和ELSE
		if (zIhuBspStm32I2c1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE)
		{
			//只有满足这么苛刻的条件，才算找到了帧头
			if ((res == IHU_HUITP_L2FRAME_STD_RX_START_FLAG_CHAR) && (zIhuBspStm32I2c1RxCount == 1))
			zIhuBspStm32I2c1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_START;
		}
		else
		{
			//收到CHECKSUM
			if((zIhuBspStm32I2c1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_START) && (zIhuBspStm32I2c1RxCount == 2))
			{
				zIhuBspStm32I2c1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM;
			}
			//收到长度高位
			else if((zIhuBspStm32I2c1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_CKSM) && (zIhuBspStm32I2c1RxCount == 3))
			{
				zIhuBspStm32I2c1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN;
			}
			//收到长度低位
			else if((zIhuBspStm32I2c1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_HEADER_LEN) && (zIhuBspStm32I2c1RxCount == 4))
			{
				zIhuBspStm32I2c1RxLen = ((zIhuBspStm32I2c1RxBuff[2] <<8) + zIhuBspStm32I2c1RxBuff[3]);
				//CHECKSUM及判定
				if ((zIhuBspStm32I2c1RxBuff[1] == (zIhuBspStm32I2c1RxBuff[0] ^ zIhuBspStm32I2c1RxBuff[2]^zIhuBspStm32I2c1RxBuff[3])) &&\
					(zIhuBspStm32I2c1RxLen < IHU_BSP_STM32_I2C1_REC_MAX_LEN-4))
				zIhuBspStm32I2c1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_BODY;
			}
			//收到BODY位
			else if((zIhuBspStm32I2c1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (zIhuBspStm32I2c1RxLen > 1))
			{
				zIhuBspStm32I2c1RxLen--;
			}
			//收到BODY最后一位
			else if((zIhuBspStm32I2c1RxState == IHU_HUITP_L2FRAME_STD_RX_STATE_BODY) && (zIhuBspStm32I2c1RxLen == 1))
			{
				zIhuBspStm32I2c1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				zIhuBspStm32I2c1RxLen = 0;
				zIhuBspStm32I2c1RxCount = 0;
				//发送数据到上层I2CARIES模块
				memset(&snd, 0, sizeof(msg_struct_i2caries_l2frame_rcv_t));
				memcpy(snd.data, &zIhuBspStm32I2c1RxBuff[4], ((zIhuBspStm32I2c1RxBuff[2]<<8)+zIhuBspStm32I2c1RxBuff[3]));
				snd.length = sizeof(msg_struct_i2caries_l2frame_rcv_t);				
				ihu_message_send(MSG_ID_I2C_L2FRAME_RCV, TASK_ID_I2CARIES, TASK_ID_VMFO, &snd, snd.length);				
			}
			//差错情况
			else{
				zIhuBspStm32I2c1RxState = IHU_HUITP_L2FRAME_STD_RX_STATE_IDLE;
				zIhuBspStm32I2c1RxLen = 0;
				zIhuBspStm32I2c1RxCount = 0;
			}
		}
		//重新设置中断
		HAL_I2C_Slave_Receive_IT(&IHU_BSP_STM32_I2C_IAU_HANDLER, &zIhuI2cRxBuffer[IHU_BSP_STM32_I2C_IAU_HANDLER_ID-1], 1);
  }
#elif (BSP_STM32_I2C_WORK_MODE_CHOICE == BSP_STM32_I2C_WORK_MODE_MPU6050)
	uint8_t res = 0;
	msg_struct_i2caries_l2frame_rcv_t snd;	
	//处于MASTER MODE
  if(I2cHandle==&IHU_BSP_STM32_I2C_MPU6050_HANDLER)
  {
		res = zIhuI2cRxBuffer[IHU_BSP_STM32_I2C_MPU6050_HANDLER_ID-1];
		zIhuBspStm32I2c1RxBuff[zIhuBspStm32I2c1RxCount++] = res;
		//固定长度到达，直接发送到上层，或者以后变成结果，送到固定的某个结构中，供给上层使用polling mode读取
		if (zIhuBspStm32I2c1RxCount >= IHU_BSP_STM32_I2C_MPU6050_FIX_FRAME_LEN){
			zIhuBspStm32I2c1RxLen = 0;
			zIhuBspStm32I2c1RxCount = 0;
			//发送数据到上层I2CARIES模块
			memset(&snd, 0, sizeof(msg_struct_i2caries_l2frame_rcv_t));
			memcpy(snd.data, &zIhuBspStm32I2c1RxBuff[0], IHU_BSP_STM32_I2C_MPU6050_FIX_FRAME_LEN);
			snd.length = sizeof(msg_struct_i2caries_l2frame_rcv_t);				
			ihu_message_send(MSG_ID_I2C_L2FRAME_RCV, TASK_ID_I2CARIES, TASK_ID_VMFO, &snd, snd.length);
		}
		//差错情况
		else{
			zIhuBspStm32I2c1RxLen = 0;
			zIhuBspStm32I2c1RxCount = 0;
		}
		//重新设置中断
		HAL_I2C_Master_Receive_IT(&IHU_BSP_STM32_I2C_MPU6050_HANDLER, (uint16_t)IHU_BSP_STM32_I2C_MPU6050_SENSOR_SLAVE_ADDRESS, &zIhuI2cRxBuffer[IHU_BSP_STM32_I2C_MPU6050_HANDLER_ID-1], 1);
  }	
#elif (BSP_STM32_I2C_WORK_MODE_CHOICE == BSP_STM32_I2C_WORK_MODE_CCL_SENSOR)
	uint8_t res = 0;
	msg_struct_i2caries_l2frame_rcv_t snd;	
	//处于MASTER MODE
  if(I2cHandle==&IHU_BSP_STM32_I2C_CCL_SENSOR_HANDLER)
  {
		res = zIhuI2cRxBuffer[IHU_BSP_STM32_I2C_CCL_SENSOR_HANDLER_ID-1];
		zIhuBspStm32I2c1RxBuff[zIhuBspStm32I2c1RxCount++] = res;
		//固定长度到达，直接发送到上层，或者以后变成结果，送到固定的某个结构中，供给上层使用polling mode读取
		if (zIhuBspStm32I2c1RxCount >= IHU_BSP_STM32_I2C_CCL_FIX_FRAME_LEN){
			zIhuBspStm32I2c1RxLen = 0;
			zIhuBspStm32I2c1RxCount = 0;
			//发送数据到上层I2CARIES模块
			memset(&snd, 0, sizeof(msg_struct_i2caries_l2frame_rcv_t));
			memcpy(snd.data, &zIhuBspStm32I2c1RxBuff[0], IHU_BSP_STM32_I2C_CCL_FIX_FRAME_LEN);
			snd.length = sizeof(msg_struct_i2caries_l2frame_rcv_t);				
			ihu_message_send(MSG_ID_I2C_L2FRAME_RCV, TASK_ID_I2CARIES, TASK_ID_VMFO, &snd, snd.length);				
		}
		//差错情况
		else{
			zIhuBspStm32I2c1RxLen = 0;
			zIhuBspStm32I2c1RxCount = 0;
		}
		//重新设置中断
		//HAL_I2C_Master_Receive_IT(&hi2c1, (uint16_t)IHU_BSP_STM32_I2C_CCL_SENSOR_SLAVE_ADDRESS, &zIhuI2cRxBuffer[0], 1);	
		HAL_I2C_Master_Receive_IT(&IHU_BSP_STM32_I2C_CCL_SENSOR_HANDLER, (uint16_t)IHU_BSP_STM32_I2C_CCL_SENSOR_SLAVE_ADDRESS, &zIhuI2cRxBuffer[IHU_BSP_STM32_I2C_CCL_SENSOR_HANDLER_ID-1], 1);
  }
#endif
  else if(I2cHandle==&IHU_BSP_STM32_I2C_SPARE1_HANDLER)
  {
		zIhuBspStm32I2c2RxBuff[zIhuBspStm32I2c2RxCount] = zIhuI2cRxBuffer[IHU_BSP_STM32_I2C_SPARE1_HANDLER_ID-1];
		zIhuBspStm32I2c2RxCount++;
		if (zIhuBspStm32I2c2RxCount >= IHU_BSP_STM32_I2C2_REC_MAX_LEN)
			zIhuBspStm32I2c2RxCount = 0;
		HAL_I2C_Slave_Receive_IT(&IHU_BSP_STM32_I2C_SPARE1_HANDLER, &zIhuI2cRxBuffer[IHU_BSP_STM32_I2C_SPARE1_HANDLER_ID-1], 1);
  }
}


/**
  * 函数功能: I2C通信错误处理函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 一般在I2C通信超时时调用该函数
  */
static void I2C_BSP_STM32_MPU6050_Error (void)
{
  /* 反初始化I2C通信总线 */
  HAL_I2C_DeInit(&IHU_BSP_STM32_I2C_MPU6050_HANDLER);
  
  /* 重新初始化I2C通信总线*/
  //MX_I2C1_Init();
	//考虑到这里需要重新初始化，不再真正执行重新初始化
}

/**
  * 函数功能: 通过I2C写入一个值到指定寄存器内
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  *           Value：值
  * 返 回 值: 无
  * 说    明: 无
  */
void func_bsp_stm32_i2c_mpu6050_write_data(uint16_t Addr, uint8_t Reg, uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&IHU_BSP_STM32_I2C_MPU6050_HANDLER, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, IHU_BSP_STM32_I2C_TIMEOUT_TX_MAX);
  
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_BSP_STM32_MPU6050_Error();
  }
}

/**
  * 函数功能: 通过I2C写入一段数据到指定寄存器内
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  *           RegSize：寄存器尺寸(8位或者16位)
  *           pBuffer：缓冲区指针
  *           Length：缓冲区长度
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 在循环调用是需加一定延时时间
  */
HAL_StatusTypeDef func_bsp_stm32_i2c_mpu6050_write_buffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&IHU_BSP_STM32_I2C_MPU6050_HANDLER, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, IHU_BSP_STM32_I2C_TIMEOUT_TX_MAX); 

  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_BSP_STM32_MPU6050_Error();
  }        
  return status;
}


/**
  * 函数功能: 通过I2C读取一个指定寄存器内容
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  * 返 回 值: uint8_t：寄存器内容
  * 说    明: 无
  */
uint8_t func_bsp_stm32_i2c_mpu6050_read_data(uint16_t Addr, uint8_t Reg)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;
  
  status = HAL_I2C_Mem_Read(&IHU_BSP_STM32_I2C_MPU6050_HANDLER, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, IHU_BSP_STM32_I2C_TIMEOUT_TX_MAX);
 
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_BSP_STM32_MPU6050_Error();
  
  }
  return value;
}

/**
  * 函数功能: 通过I2C读取一段寄存器内容存放到指定的缓冲区内
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  *           RegSize：寄存器尺寸(8位或者16位)
  *           pBuffer：缓冲区指针
  *           Length：缓冲区长度
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 无
  */
HAL_StatusTypeDef func_bsp_stm32_i2c_mpu6050_read_buffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(&IHU_BSP_STM32_I2C_MPU6050_HANDLER, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, IHU_BSP_STM32_I2C_TIMEOUT_TX_MAX);
  
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_BSP_STM32_MPU6050_Error();
  }        
  return status;
}

/**
  * 函数功能: 检测I2C设备是否处于准备好可以通信状态
  * 输入参数: DevAddress：I2C设备地址
  *           Trials：尝试测试次数
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 无
  */
HAL_StatusTypeDef func_bsp_stm32_i2c_mpu6050_is_device_ready(uint16_t DevAddress, uint32_t Trials)
{ 
  return (HAL_I2C_IsDeviceReady(&IHU_BSP_STM32_I2C_MPU6050_HANDLER, DevAddress, Trials, IHU_BSP_STM32_I2C_TIMEOUT_TX_MAX));
}

/**
  * 函数功能: 写数据到MPU6050寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void BSP_STM32_MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
  func_bsp_stm32_i2c_mpu6050_write_data(BSP_STM32_MPU6050_SLAVE_ADDRESS,reg_add,reg_dat);
}

/**
  * 函数功能: 从MPU6050寄存器读取数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void BSP_STM32_MPU6050_ReadData(uint8_t reg_add,unsigned char *Read,uint8_t num)
{
  func_bsp_stm32_i2c_mpu6050_read_buffer(BSP_STM32_MPU6050_SLAVE_ADDRESS,reg_add,I2C_MEMADD_SIZE_8BIT,Read,num);
}

/**
  * 函数功能: 初始化MPU6050芯片
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void ihu_bsp_stm32_i2c_mpu6050_init(void)
{
  int i=0,j=0;
  //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      ;
    }
  }
	BSP_STM32_MPU6050_WriteReg(BSP_STM32_MPU6050_RA_PWR_MGMT_1, 0x00);	    //解除休眠状态
	BSP_STM32_MPU6050_WriteReg(BSP_STM32_MPU6050_RA_SMPLRT_DIV , 0x07);	    //陀螺仪采样率，1KHz
	BSP_STM32_MPU6050_WriteReg(BSP_STM32_MPU6050_RA_CONFIG , 0x06);	        //低通滤波器的设置，截止频率是1K，带宽是5K
	BSP_STM32_MPU6050_WriteReg(BSP_STM32_MPU6050_RA_ACCEL_CONFIG , 0x00);	  //配置加速度传感器工作在2G模式，不自检
	BSP_STM32_MPU6050_WriteReg(BSP_STM32_MPU6050_RA_GYRO_CONFIG, 0x18);     //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
}

/**
  * 函数功能: 读取MPU6050的ID
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
uint8_t func_bsp_stm32_i2c_mpu6050_return_id(void)
{
	unsigned char Re = 0;
    BSP_STM32_MPU6050_ReadData(BSP_STM32_MPU6050_RA_WHO_AM_I,&Re,1);    //读器件地址
	if(Re != 0x68)
	{
		return 0;
	}
	else
	{
		printf("MPU6050 ID = %d\r\n",Re);
		return 1;
	}
		
}

/**
  * 函数功能: 读取MPU6050的加速度数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void func_bsp_stm32_i2c_mpu6050_read_acc(int16_t *accData)
{
    uint8_t buf[6];
    BSP_STM32_MPU6050_ReadData(BSP_STM32_MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
  * 函数功能: 读取MPU6050的角速度数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void func_bsp_stm32_i2c_mpu6050_read_gyro(int16_t *gyroData)
{
    uint8_t buf[6];
    BSP_STM32_MPU6050_ReadData(BSP_STM32_MPU6050_GYRO_OUT,buf,6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}

/**
  * 函数功能: 读取MPU6050的原始温度数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void func_bsp_stm32_i2c_mpu6050_read_temp(int16_t *tempData)
{
	uint8_t buf[2];
    BSP_STM32_MPU6050_ReadData(BSP_STM32_MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
    *tempData = (buf[0] << 8) | buf[1];
}

/**
  * 函数功能: 读取MPU6050的温度数据，转化成摄氏度
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void func_bsp_stm32_i2c_mpu6050_return_temp(int16_t*Temperature)
{
	int16_t temp3;
	uint8_t buf[2];
	
	BSP_STM32_MPU6050_ReadData(BSP_STM32_MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
  temp3= (buf[0] << 8) | buf[1];
	*Temperature=(((double) (temp3 + 13200)) / 280)-13;
}

//加速度调用之前，必须使用
//int16_t accData[3];
int8_t ihu_bsp_stm32_i2c_mpu6050_acc_read(int16_t *accData)
{
	if (func_bsp_stm32_i2c_mpu6050_return_id() == 0)
		return IHU_FAILURE;
	else{
		func_bsp_stm32_i2c_mpu6050_read_acc(accData);
		return IHU_SUCCESS;
	}
}

//陀螺仪调用之前，必须使用
//int16_t gyroData[3];
int8_t ihu_bsp_stm32_i2c_mpu6050_gyro_read(int16_t *gyroData)
{
	if (func_bsp_stm32_i2c_mpu6050_return_id() == 0)
		return IHU_FAILURE;
	else{
		 func_bsp_stm32_i2c_mpu6050_read_gyro(gyroData);
		return IHU_SUCCESS;
	}
}

//温度调用之前，必须使用
//int16_t tempData;
int8_t ihu_bsp_stm32_i2c_mpu6050_temp_read(int16_t tempData)
{
	if (func_bsp_stm32_i2c_mpu6050_return_id() == 0)
		return IHU_FAILURE;
	else{
		 func_bsp_stm32_i2c_mpu6050_return_temp(&tempData);
		return IHU_SUCCESS;
	}
}

//CCL SENSOR：给传感器发送数据和控制命令
int ihu_bsp_stm32_i2c_ccl_send_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_I2C_Master_Transmit(&IHU_BSP_STM32_I2C_CCL_SENSOR_HANDLER, (uint16_t)IHU_BSP_STM32_I2C_CCL_SENSOR_SLAVE_ADDRESS, (uint8_t *)buff, len, IHU_BSP_STM32_I2C_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}

//CCL SENSOR: 从传感器中读取数据
int ihu_bsp_stm32_i2c_ccl_sensor_rcv_data(uint8_t* buff, uint16_t len)
{    
	if (HAL_I2C_Master_Receive(&IHU_BSP_STM32_I2C_CCL_SENSOR_HANDLER, (uint16_t)IHU_BSP_STM32_I2C_CCL_SENSOR_SLAVE_ADDRESS, (uint8_t *)buff, len, IHU_BSP_STM32_I2C_TX_MAX_DELAY_DURATION) == HAL_OK)
		return BSP_SUCCESS;
	else
		return BSP_FAILURE;
}




