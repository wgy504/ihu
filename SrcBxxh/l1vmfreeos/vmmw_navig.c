/**
 ****************************************************************************************
 *
 * @file mod_rfid.c
 *
 * @brief RFID module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */
 
#include "vmmw_navig.h"

//全局变量，引用外部

//当使用I2C接口时




/**
  * 函数功能: 写数据到MPU6050寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
int8_t func_vmmw_navig_mpu6050_write_reg(uint8_t reg_add,uint8_t reg_dat)
{
  if (ihu_l1hd_i2c_mpu6050_write_data(IHU_VMMW_NAVIG_MPU6050_SLAVE_ADDRESS,reg_add,reg_dat) == IHU_FAILURE)
		return IHU_FAILURE;
	else
		return IHU_SUCCESS;
}

/**
  * 函数功能: 从MPU6050寄存器读取数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void func_vmmw_navig_mpu6050_read_data(uint8_t reg_add,unsigned char *Read,uint8_t num)
{
  ihu_l1hd_i2c_mpu6050_read_buffer(IHU_VMMW_NAVIG_MPU6050_SLAVE_ADDRESS, reg_add, I2C_MEMADD_SIZE_8BIT, Read, num);
}

/**
  * 函数功能: 初始化MPU6050芯片
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
int8_t ihu_vmmw_navig_mpu6050_init(void)
{
	ihu_usleep(200);
	
	func_vmmw_navig_mpu6050_write_reg(IHU_VMMW_NAVIG_MPU6050_RA_PWR_MGMT_1, 0x80);	    //RESET
	ihu_usleep(100);
	
	func_vmmw_navig_mpu6050_write_reg(IHU_VMMW_NAVIG_MPU6050_RA_PWR_MGMT_1, 0x00);	    //解除休眠状态
	func_vmmw_navig_mpu6050_write_reg(IHU_VMMW_NAVIG_MPU6050_RA_SMPLRT_DIV , 0x07);	    //陀螺仪采样率，1KHz
	func_vmmw_navig_mpu6050_write_reg(IHU_VMMW_NAVIG_MPU6050_RA_CONFIG , 0x06);	        //低通滤波器的设置，截止频率是1K，带宽是5K
	func_vmmw_navig_mpu6050_write_reg(IHU_VMMW_NAVIG_MPU6050_RA_ACCEL_CONFIG , 0x01);	  //配置加速度传感器工作在2G模式，不自检 0=>1，让其自检
	func_vmmw_navig_mpu6050_write_reg(IHU_VMMW_NAVIG_MPU6050_RA_GYRO_CONFIG, 0x18);     //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
	
	//需要根据I2C设备是否存在，进而决定是否要返回错误，不然后期的操作会阻塞在I2C设备里面
	if (func_vmmw_navig_mpu6050_write_reg(IHU_VMMW_NAVIG_MPU6050_RA_GYRO_CONFIG, 0x18) == IHU_FAILURE){
		IhuErrorPrint("VMMWNAVIG: Fall Angle sensor (MPU6050) not detected!\n");
		return IHU_FAILURE;
	}
	else
		return IHU_SUCCESS;
}

/**
  * 函数功能: 读取MPU6050的ID
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
int8_t ihu_vmmw_navig_mpu6050_return_id(void)
{
	unsigned char Re = 0;
  
	func_vmmw_navig_mpu6050_read_data(IHU_VMMW_NAVIG_MPU6050_RA_WHO_AM_I,&Re,1);    //读器件地址
	IHU_DEBUG_PRINT_INF("VMMWNAVIG: Read MPU6050 WHO_AM_I = %d\n", Re);
	
	if(Re != IHU_VMMW_NAVIG_MPU6050_ADDRESS)
	{
		return IHU_FAILURE;
	}
	else
	{
		return IHU_SUCCESS;
	}
		
}

/**
  * 函数功能: 读取MPU6050的加速度数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void ihu_vmmw_navig_mpu6050_read_acc(int16_t *accData)
{
    uint8_t buf[6];
    func_vmmw_navig_mpu6050_read_data(IHU_VMMW_NAVIG_MPU6050_ACC_OUT, buf, 6);
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
void ihu_vmmw_navig_mpu6050_read_gyro(int16_t *gyroData)
{
    uint8_t buf[6];
    func_vmmw_navig_mpu6050_read_data(IHU_VMMW_NAVIG_MPU6050_GYRO_OUT,buf,6);
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
void ihu_vmmw_navig_mpu6050_read_temp(int16_t *tempData)
{
	uint8_t buf[2];
    func_vmmw_navig_mpu6050_read_data(IHU_VMMW_NAVIG_MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
    *tempData = (buf[0] << 8) | buf[1];
}

/**
  * 函数功能: 读取MPU6050的温度数据，转化成摄氏度
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void ihu_vmmw_navig_mpu6050_return_temp(int16_t*Temperature)
{
	int16_t temp3;
	uint8_t buf[2];
	
	func_vmmw_navig_mpu6050_read_data(IHU_VMMW_NAVIG_MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
  temp3= (buf[0] << 8) | buf[1];
	*Temperature=(((double) (temp3 + 13200)) / 280)-13;
}

//加速度调用之前，必须使用
//int16_t accData[3];
int8_t func_vmmw_navig_mpu6050_acc_read(int16_t *accData)
{
	if (ihu_vmmw_navig_mpu6050_return_id() == IHU_FAILURE)
		return IHU_FAILURE;
	else{
		ihu_vmmw_navig_mpu6050_read_acc(accData);
		return IHU_SUCCESS;
	}
}

//陀螺仪调用之前，必须使用
//int16_t gyroData[3];
int8_t func_vmmw_navig_mpu6050_gyro_read(int16_t *gyroData)
{
	if (ihu_vmmw_navig_mpu6050_return_id() == IHU_FAILURE)
		return IHU_FAILURE;
	else{
		 ihu_vmmw_navig_mpu6050_read_gyro(gyroData);
		return IHU_SUCCESS;
	}
}

//温度调用之前，必须使用
//int16_t tempData;
int8_t func_vmmw_navig_mpu6050_temp_read(int16_t tempData)
{
	if (ihu_vmmw_navig_mpu6050_return_id() == IHU_FAILURE)
		return IHU_FAILURE;
	else{
		 ihu_vmmw_navig_mpu6050_return_temp(&tempData);
		return IHU_SUCCESS;
	}
}

/*
 *
 *  通过MPU6050测量得出的加速度及陀螺仪数据，计算出倾角
 *
 *
 *
*/
//各坐标轴上静止偏差（重力，角速度）
int16_t offsetAccelX = -195;
int16_t offsetAccelY = 560;
int16_t offsetAccelZ = -169;
int16_t offsetGyroX = 12;
int16_t offsetGyroY = 33;
int16_t offsetGyroZ = 4;
float Angle_accel = 0;
float Angle_gyro = 0;
float Angle_comp = 0;
uint8_t outAngleData[10];

/*---------------------------------------------------------------------------------------------------------*/
/*  重力加速度算出的角度                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
int32_t ihu_vmmw_navig_mpu6050_calc_angle_accel(void)
{
  int32_t value = 0;
	int16_t input[3];
	
	memset(input, 0, sizeof(input));
  if (func_vmmw_navig_mpu6050_acc_read(input) == IHU_FAILURE){
		value = 0;
		Angle_accel = 0;
	}else{
		value = input[0]; //x axis
		if(value > 16384)
			Angle_accel = -(asin(1) * 180 / 3.1415296);
		else
			Angle_accel = -(asin(value * 1.0 / 16384) * 180 / 3.1415296);
	}
   
  return value;
}

/*---------------------------------------------------------------------------------------------------------*/
/*   角速度算出来的角度（积分）                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
int32_t ihu_vmmw_navig_mpu6050_calc_angle_gyro(void)
{
  int32_t value = 0;
	int16_t input[3];
	
	memset(input, 0, sizeof(input));
  if (func_vmmw_navig_mpu6050_gyro_read(input) == IHU_FAILURE){
		value = 0;
	}else{
		value = input[1];  //y axis
		Angle_gyro += (value / 16.384 * 0.01);
	}
 
	return value;
}

/*---------------------------------------------------------------------------------------------------------*/
/*   互补滤波求角度                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
float ihu_vmmw_navig_mpu6050_complement_filter(int32_t simpleGyro)
{
	Angle_comp = 0.98 * (Angle_comp + -simpleGyro / 16.384 * 0.01) + 0.02 * Angle_accel;
 
	return Angle_comp;
}

/*---------------------------------------------------------------------------------------------------------*/
/*   输出角度函数，多次重复计算，消除误差                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
float ihu_vmmw_navig_mpu6050_angle_caculate_by_motion_method(void)
{
	int32_t GyroValue;
	int i = 0;
	
	for (i = 0; i<10; i++){
		ihu_vmmw_navig_mpu6050_calc_angle_accel();
		GyroValue = ihu_vmmw_navig_mpu6050_calc_angle_gyro();
		ihu_vmmw_navig_mpu6050_complement_filter(GyroValue);
		ihu_usleep(10);
	}
	
	return Angle_comp;
}


//得到角度
//x,y,z:x,y,z方向上的重力加速度衡量，不需要单位，直接数值即可
//dir: 要获得的角度，0表示与z轴的夹角，1表示与x轴的夹角，2表示与y轴的夹角
//返回值：角度值，浮点，单位为°度
float ihu_vmmw_navig_mpu6050_angle_caculate_by_acc_xyz(float x, float y, float z, uint8_t dir)
{
	float temp = 0;
	float res = 0;
	switch(dir)
	{
		case 0: //与自然z轴的角度
			temp = sqrt((x*x+y*y))/(z==0?0.01:z);
			res = atan(temp);
			break;
		case 1: //与自然x轴的角度
			temp = x/(sqrt((y*y+z*z))==0?0.01:sqrt((y*y+z*z)));
			res = atan(temp);
			break;
		case 2: //与自然Y轴的角度
			temp = y/(sqrt((x*x+z*z))==0?0.01:sqrt((x*x+z*z)));
			res = atan(temp);
			break;
		default:
			break;
	}
	return res*180/3.14;
}

/*---------------------------------------------------------------------------------------------------------*/
/*   反正切求角度：这里只考虑Z轴数据夹角数据，其它的夹角如果需要可以再行计算                               */
/*---------------------------------------------------------------------------------------------------------*/
#define IHU_IHU_VMMW_NAVIG_I2C_MPU6050_ANGLE_CAC_REP_NBR 100
//返回单位为度
float ihu_wmmw_navig_mpu6050_axis_z_angle_caculate_by_static_method(void)
{
	int16_t input[3];
	int i = 0;
	float output[3];
	float res = 0;

	//循环读取多遍，求算数平均
	memset(output, 0, sizeof(output));
	for (i = 0; i<IHU_IHU_VMMW_NAVIG_I2C_MPU6050_ANGLE_CAC_REP_NBR; i++){
		memset(input, 0, sizeof(input));
		if (func_vmmw_navig_mpu6050_acc_read(input) == IHU_SUCCESS){
			output[0] = output[0] + ((input[0] * 1.0) / IHU_IHU_VMMW_NAVIG_I2C_MPU6050_ANGLE_CAC_REP_NBR);
			output[1] = output[1] + ((input[1] * 1.0) / IHU_IHU_VMMW_NAVIG_I2C_MPU6050_ANGLE_CAC_REP_NBR);
			output[2] = output[2] + ((input[2] * 1.0) / IHU_IHU_VMMW_NAVIG_I2C_MPU6050_ANGLE_CAC_REP_NBR);
			ihu_usleep(10);
		}
	}
	
	//只按照y轴的数据进行表达，求解与z轴，也就是地球重力方向的夹角
	res = ihu_vmmw_navig_mpu6050_angle_caculate_by_acc_xyz(output[0], output[1], output[2], 0);
	IHU_DEBUG_PRINT_INF("VMMWNAVIG: Read MPU6050 Accdata Output = %f, %f, %f, Res=%f Degree\n", output[0], output[1], output[2], res);
	
	return res;
}

/*---------------------------------------------------------------------------------------------------------*/
/*   四元素求角度 偏航角，俯仰角，翻滚角                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define Kp 100.0f                // 比例增益支配率收敛到加速度计/磁强计
#define Ki 0.002f                // 积分增益支配率的陀螺仪偏见的衔接
#define halfT 0.001f             // 采样周期的一半
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;          // 四元数的元素，代表估计方向
float exInt = 0, eyInt = 0, ezInt = 0;        // 按比例缩小积分误差
float Yaw,Pitch,Roll;  //偏航角，俯仰角，翻滚角

void ihu_vmmw_navig_mpu6050_imu_update(float gx, float gy, float gz, float ax, float ay, float az)
{
  float norm;
	float vx, vy, vz;
	float ex, ey, ez;  

	// 测量正常化
	norm = sqrt(ax*ax + ay*ay + az*az);      
	ax = ax / norm;                   //单位化
	ay = ay / norm;
	az = az / norm;      

	// 估计方向的重力
	vx = 2*(q1*q3 - q0*q2);
	vy = 2*(q0*q1 + q2*q3);
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

	// 错误的领域和方向传感器测量参考方向之间的交叉乘积的总和
	ex = (ay*vz - az*vy);
	ey = (az*vx - ax*vz);
	ez = (ax*vy - ay*vx);

	// 积分误差比例积分增益
	exInt = exInt + ex*Ki;
	eyInt = eyInt + ey*Ki;
	ezInt = ezInt + ez*Ki;

	// 调整后的陀螺仪测量
	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;

	// 整合四元数率和正常化
	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  

	// 正常化四元
	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;

	Pitch  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch ,转换为度数
	Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // rollv
	Yaw = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;                //此处没有价值，注掉
}



