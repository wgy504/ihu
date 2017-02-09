/*
 * sysconf.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L0COMVM_SYSCONFIG_H_
#define L0COMVM_SYSCONFIG_H_

#include "sysversion.h"
#include "sysdim.h"

/*
 *
 *   全局性工程参数控制表，可以被中途修改，更可以事先更新好，重启系统让其起到作用
 *   这里的配置项，只是作为缺省参数，一旦工程参数出错或者不可用，这里的缺省参数将会被起作用
 *   这里的参数起到出厂参数的作用，随着工厂版本一起到达现场
 *
 *   强烈注意的要点：
 *   1. 批量生产制作工厂包(Factory Load)时，需要将IHU_HARDWARE_MASSIVE_PRODUTION_SET开关打开，以便从烧录区域读取设备标签和各种硬件信息
 *
 *
 *
 */
//任务模块激活的定义
#define IHU_TASK_PNP_NULL 0
#define IHU_TASK_PNP_ON 1
#define IHU_TASK_PNP_OFF 2
#define IHU_TASK_PNP_INVALID 0xFF

//队列的状态定义
#define IHU_TASK_QUEUE_FULL_NULL 0
#define IHU_TASK_QUEUE_FULL_FALSE 1
#define IHU_TASK_QUEUE_FULL_TRUE 2
#define IHU_TASK_QUEUE_FULL_INVALID 0xFF

//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
#define IHU_COMM_HW_BOARD_ON 1
#define IHU_COMM_HW_BOARD_OFF 0

//DEBUG开关，缺省打开
#define IHU_TRACE_DEBUG_ALL_OFF 0 //全关
#define IHU_TRACE_DEBUG_ALL_ON 0xFF //全开
#define IHU_TRACE_DEBUG_INF_ON 1  //信息
#define IHU_TRACE_DEBUG_NOR_ON 2  //普通级
#define IHU_TRACE_DEBUG_IPT_ON 4  //重要级
#define IHU_TRACE_DEBUG_CRT_ON 8  //关键级
#define IHU_TRACE_DEBUG_FAT_ON 16 //致命级

//TRACE开关，缺省打开
#define IHU_TRACE_MSG_MODE_OFF 0 //全关
#define IHU_TRACE_MSG_MODE_INVALID 0xFF //无效
#define IHU_TRACE_MSG_MODE_ALL 1 //放开所有的TRACE模块和消息，该模式将忽略模块和消息的设置
#define IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT 2  //除了所有的TIME_OUT消息
#define IHU_TRACE_MSG_MODE_ALL_BUT_HEART_BEAT 3 //除了所有的HEART_BEAT消息
#define IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT 4  //除了所有的TIME_OUT消息
#define IHU_TRACE_MSG_MODE_MOUDLE_TO_ALLOW 10  //只通过模块号过滤消息
#define IHU_TRACE_MSG_MODE_MOUDLE_TO_RESTRICT 11  //只通过模块号过滤消息
#define IHU_TRACE_MSG_MODE_MOUDLE_FROM_ALLOW 12  //只通过模块号过滤消息
#define IHU_TRACE_MSG_MODE_MOUDLE_FROM_RESTRICT 13  //只通过模块号过滤消息
#define IHU_TRACE_MSG_MODE_MOUDLE_DOUBLE_ALLOW 14  //只通过模块号过滤消息
#define IHU_TRACE_MSG_MODE_MOUDLE_DOUBLE_RESTRICT 15  //只通过模块号过滤消息
#define IHU_TRACE_MSG_MODE_MSGID_ALLOW 20  //只通过模块号过滤消息
#define IHU_TRACE_MSG_MODE_MSGID_RESTRICT 21  //只通过模块号过滤消息
#define IHU_TRACE_MSG_MODE_COMBINE_TO_ALLOW 30  //通过模块和消息枚举
#define IHU_TRACE_MSG_MODE_COMBINE_TO_RESTRICT 31  //通过模块和消息枚举
#define IHU_TRACE_MSG_MODE_COMBINE_FROM_ALLOW 32  //通过模块和消息枚举
#define IHU_TRACE_MSG_MODE_COMBINE_FROM_RESTRICT 33  //通过模块和消息枚举
#define IHU_TRACE_MSG_MODE_COMBINE_DOUBLE_ALLOW 34  //通过模块和消息枚举
#define IHU_TRACE_MSG_MODE_COMBINE_DOUBLE_RESTRICT 35  //通过模块和消息枚举

//系统定义的服务器以及本机名称，用于IHU与后台服务器之间的通信
#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_NULL 						0
#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_XML 						1
#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_ZHB 						2  //中环保
#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_HUITP_L2FRAME 	3
#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_HUITP_XML 			4
#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_HUITP_JASON 		5
#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_INVALID 				0xFF

//定义硬件是否进入批量生产阶段，从而决定硬件标签读入的方式
#define IHU_HARDWARE_MASSIVE_PRODUTION_YES 1
#define IHU_HARDWARE_MASSIVE_PRODUTION_NO 2

//定义固件是否支持升级
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_NONE 0
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_NO 1
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_YES_STABLE 2
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_YES_TRAIL 3
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_YES_PATCH 4
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_YES_INVALID 0xFF

//定义软件升级方式
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_NULL 0
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_UART_DIRECT 1
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_UART_GPRS 2
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_CANITF 3
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_UDISK 4
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_SD_CARD 5
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_USBNET 6
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_ETH_DIRECT 7
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_SPI_DIRECT 8
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_SPI_ETH 9
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_WIRELESS 10
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_ACTIVE_CONNECT 11
#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_INVALID 0xFF

 /***********************************************************************************
 *
 * EMCWX项目 IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID
 *
 ***********************************************************************************/
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID)
	//DEBUG开关，缺省打开
	#define IHU_TRACE_DEBUG_ON IHU_TRACE_DEBUG_ALL_OFF //利用以上标示位，确定TRACE的级别
	//TRACE开关，缺省打开
	#define IHU_TRACE_MSG_ON IHU_TRACE_MSG_MODE_ALL
	#define IHU_TASK_STATE_VMDA 			IHU_TASK_PNP_ON
	#define IHU_TASK_STATE_TIMER 			IHU_TASK_PNP_ON
	#define IHU_TASK_STATE_ASYLIBRA 	IHU_TASK_PNP_ON
	#define IHU_TASK_STATE_AIRKISS 		IHU_TASK_PNP_OFF
	#define IHU_TASK_STATE_ADCARIES 	IHU_TASK_PNP_ON
	#define IHU_TASK_STATE_EMC 				IHU_TASK_PNP_ON
	//为了缺乏内存时的编译，做下列选项
	#define IHU_COMPILE_TASK_VMDASHELL  IHU_TASK_PNP_ON
	//#undef IHU_COMPILE_TASK_VMDASHELL
	#define IHU_COMPILE_TASK_TIMER    IHU_TASK_PNP_ON
	//#undef IHU_COMPILE_TASK_TIMER
	#define IHU_COMPILE_TASK_ASYLIBRA  IHU_TASK_PNP_ON
	//#undef IHU_COMPILE_TASK_ASYLIBRA
	#define IHU_COMPILE_TASK_AIRKISS  IHU_TASK_PNP_ON
	//#undef IHU_COMPILE_TASK_AIRKISS
	#define IHU_COMPILE_TASK_ADCARIES IHU_TASK_PNP_ON
	//#undef IHU_COMPILE_TASK_ADCARIES
	#define IHU_COMPILE_TASK_EMC      IHU_TASK_PNP_ON
	//#undef IHU_COMPILE_TASK_EMC
	//控制是否存储本地硬盘/内存的选项
	#define IHU_MEM_SENSOR_SAVE_FLAG_YES 1
	#define IHU_MEM_SENSOR_SAVE_FLAG_NO 0 //and Others
	#define IHU_MEM_SENSOR_SAVE_FLAG IHU_MEM_SENSOR_SAVE_FLAG_YES
	#define IHU_DISC_SENSOR_SAVE_FLAG_YES 1
	#define IHU_DISC_SENSOR_SAVE_FLAG_NO 0 //and Others
	#define IHU_DISC_SENSOR_SAVE_FLAG IHU_DISC_SENSOR_SAVE_FLAG_YES
	//最少保留多久的数据，做成安全的全局变量，并不能随意通过工程参数改小
	//部分保留数据可以改的更小，可以放在工参里另行定义
	#define IHU_DATA_SAVE_DURATION_MIN_IN_DAYS 60
	#define IHU_DATA_SAVE_DURATION_IN_DAYS 90
	//月份的计算按照（[天数-1]/30）+1进行，还是很精妙的，这样就不用单独设置月份数据
	//月份的意义是，定时扫描，确保多少个月以内，文件数据必须保留，因为文件数据目前是按照时间月份进行存储的。使用天数进行计算，容易出现错误
	//#define IHU_DATA_SAVE_DURATION_IN_MONTHS 3

	//CURRENT HW PRODUCT CATELOG
	#define IHU_HARDWARE_PRODUCT_CAT_TYPE 0x1101 // HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E1_EMC_58_01 0x1101


 /***********************************************************************************
 *
 * 数采仪CB项目 IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID
 *
 ***********************************************************************************/
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
	
	//ucos下的VMUO还未改进VM FuncHandler机制，待简化这部分
	#define IHU_COMM_FRONT_ADC 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_I2C 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_PWM 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPS 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPI 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_GPIO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DIDO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_LED 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_ETH 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DCMI 0 //1=ON, 0/OTHERS=OFF	
	#define IHU_MAIN_CTRL_SCYCB 1 //1=ON, 0/OTHERS=OFF		
	//定义后台连接网络的接口
	//现在采用这种互斥的方式进行定义，以后需要等待HWINV进行PnP，确保多种接口即插即用，随时切换
	//多种接口之间的优先级关系，则有程序任务自动决定：ETHERNET > WIFI > USB-OTG2 > 3G4G
	//当前版本的做法是，多种接口均启动起来，但如何使用，由CLOUDCONT按照优先级和COMM_BACK_HAWL_CON定义唯一决定
	//Timer setting：心跳协议必须设置在20s以内，不然不能满足
	#define IHU_VMFO_TIMER_DURATION_PERIOD_SCAN 300		
	#define IHU_ADCLIBRA_TIMER_DURATION_PERIOD_SCAN 300
	#define IHU_DIDOCAP_TIMER_DURATION_PERIOD_SCAN 300
	#define IHU_ETHORION_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_GPIOCANCER_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_I2CARIERS_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_LEDPISCES_TIMER_DURATION_PERIOD_SCAN 3
	#define IHU_LEDPISCES_TIMER_GALOWAG_DURATION_PERIOD_SCAN 1
	#define IHU_PWMTAURUS_TIMER_DURATION_PERIOD_SCAN 300
	#define IHU_SPILEO_TIMER_DURATION_PERIOD_SCAN 400
	#define IHU_SPSVIRGO_TIMER_DURATION_PERIOD_SCAN 5
	#define IHU_DCMIARIS_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_SCYCB_TIMER_DURATION_PERIOD_SCAN 400
	
	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL "http://127.0.0.1/test.php"
	//local SW storage address
	#define  IHU_SW_DOWNLOAD_DIR_DEFAULT "/home/pi/ihu_sw_download/"
	#define  IHU_SW_ACTIVE_DIR_DEFAULT "/home/pi/ihu_sw_active/"
	#define  IHU_SW_BACKUP_DIR_DEFAULT "/home/pi/ihu_sw_backup/"
	//系统定义的服务器以及本机名称，用于与服务器之间的通信
	#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_XML	
	//DEBUG开关，缺省打开
	#define IHU_TRACE_DEBUG_ON IHU_TRACE_DEBUG_ALL_ON //利用以上标示位，确定TRACE的级别
	//TRACE开关，缺省打开
	#define IHU_TRACE_MSG_ON IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT

	//模块启动的时间长度
	#define IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION  10  //in micro-second

	//CURRENT HW PRODUCT CATELOG
	#define IHU_HARDWARE_PRODUCT_CAT_TYPE 0x0301 // HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_SCY_WATER_01 0x0301

	//定义是否进入批量生产，从而确定硬件标识从哪里读取
	#define IHU_HARDWARE_MASSIVE_PRODUTION_SET IHU_HARDWARE_MASSIVE_PRODUTION_NO

	//定义固件是否自动升级
	#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_SET IHU_HARDWARE_BURN_ID_FW_UPGRADE_NO

/***********************************************************************************
 *
 * 流水产线项目主控板 IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID
 *
 ***********************************************************************************/
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
	
	//ucos下的VMUO还未改进VM FuncHandler机制，待简化这部分
	#define IHU_COMM_FRONT_ADC 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_I2C 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_PWM 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPS 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPI 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_GPIO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DIDO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_LED 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_ETH 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DCMI 0 //1=ON, 0/OTHERS=OFF	
	#define IHU_MAIN_CTRL_SCYCB 1 //1=ON, 0/OTHERS=OFF		
	//定义后台连接网络的接口
	//现在采用这种互斥的方式进行定义，以后需要等待HWINV进行PnP，确保多种接口即插即用，随时切换
	//多种接口之间的优先级关系，则有程序任务自动决定：ETHERNET > WIFI > USB-OTG2 > 3G4G
	//当前版本的做法是，多种接口均启动起来，但如何使用，由CLOUDCONT按照优先级和COMM_BACK_HAWL_CON定义唯一决定
	//Timer setting：心跳协议必须设置在20s以内，不然不能满足
	#define IHU_VMFO_TIMER_DURATION_PERIOD_SCAN 300		
	#define IHU_ADCLIBRA_TIMER_DURATION_PERIOD_SCAN 300
	#define IHU_DIDOCAP_TIMER_DURATION_PERIOD_SCAN 300
	#define IHU_ETHORION_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_CANVELA_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_I2CARIERS_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_LEDPISCES_TIMER_DURATION_PERIOD_SCAN 4
	#define IHU_LEDPISCES_TIMER_GALOWAG_DURATION_PERIOD_SCAN 1
	#define IHU_PWMTAURUS_TIMER_DURATION_PERIOD_SCAN 300
	#define IHU_SPILEO_TIMER_DURATION_PERIOD_SCAN 400
	#define IHU_SPSVIRGO_TIMER_DURATION_PERIOD_SCAN 5	
	#define IHU_DCMIARIS_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_EMC_TIMER_DURATION_PERIOD_READ 600
	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL "http://127.0.0.1/test.php"
	//local SW storage address
	#define  IHU_SW_DOWNLOAD_DIR_DEFAULT "/home/pi/ihu_sw_download/"
	#define  IHU_SW_ACTIVE_DIR_DEFAULT "/home/pi/ihu_sw_active/"
	#define  IHU_SW_BACKUP_DIR_DEFAULT "/home/pi/ihu_sw_backup/"
	//系统定义的服务器以及本机名称，用于与服务器之间的通信
	#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_XML	
	//DEBUG开关，缺省打开
	#define IHU_TRACE_DEBUG_ON IHU_TRACE_DEBUG_ALL_ON //利用以上标示位，确定TRACE的级别
	//TRACE开关，缺省打开
	#define IHU_TRACE_MSG_ON IHU_TRACE_MSG_MODE_ALL

	//模块启动的时间长度
	#define IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION  10  //in micro-second

	//CURRENT HW PRODUCT CATELOG
	#define IHU_HARDWARE_PRODUCT_CAT_TYPE 0x0311 // HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_PLCCB_CB_01 0x0311
	
	//定义是否进入批量生产，从而确定硬件标识从哪里读取
	#define IHU_HARDWARE_MASSIVE_PRODUTION_SET IHU_HARDWARE_MASSIVE_PRODUTION_NO

	//定义固件是否自动升级
	#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_SET IHU_HARDWARE_BURN_ID_FW_UPGRADE_NO

 /***********************************************************************************
 *
 * 流水产线项目传感器板 IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID
 *
 ***********************************************************************************/
 #elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
	
	//ucos下的VMUO还未改进VM FuncHandler机制，待简化这部分
	#define IHU_COMM_FRONT_ADC 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_I2C 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_PWM 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPS 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPI 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_GPIO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DIDO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_LED 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_ETH 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DCMI 0 //1=ON, 0/OTHERS=OFF	
	#define IHU_MAIN_CTRL_SCYCB 1 //1=ON, 0/OTHERS=OFF		
	//定义后台连接网络的接口
	//现在采用这种互斥的方式进行定义，以后需要等待HWINV进行PnP，确保多种接口即插即用，随时切换
	//多种接口之间的优先级关系，则有程序任务自动决定：ETHERNET > WIFI > USB-OTG2 > 3G4G
	//当前版本的做法是，多种接口均启动起来，但如何使用，由CLOUDCONT按照优先级和COMM_BACK_HAWL_CON定义唯一决定
	//Timer setting：心跳协议必须设置在20s以内，不然不能满足
	#define IHU_VMFO_TIMER_DURATION_PERIOD_SCAN 300		
	#define IHU_ADCLIBRA_TIMER_DURATION_PERIOD_SCAN 300
	#define IHU_DIDOCAP_TIMER_DURATION_PERIOD_SCAN 300
	#define IHU_ETHORION_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_CANVELA_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_I2CARIERS_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_LEDPISCES_TIMER_DURATION_PERIOD_SCAN 4
	#define IHU_LEDPISCES_TIMER_GALOWAG_DURATION_PERIOD_SCAN 1
	#define IHU_PWMTAURUS_TIMER_DURATION_PERIOD_SCAN 300
	#define IHU_SPILEO_TIMER_DURATION_PERIOD_SCAN 400
	#define IHU_SPSVIRGO_TIMER_DURATION_PERIOD_SCAN 5
	#define IHU_DCMIARIS_TIMER_DURATION_PERIOD_SCAN 200
	#define IHU_EMC_TIMER_DURATION_PERIOD_READ 600
	
	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL "http://127.0.0.1/test.php"
	//local SW storage address
	#define  IHU_SW_DOWNLOAD_DIR_DEFAULT "/home/pi/ihu_sw_download/"
	#define  IHU_SW_ACTIVE_DIR_DEFAULT "/home/pi/ihu_sw_active/"
	#define  IHU_SW_BACKUP_DIR_DEFAULT "/home/pi/ihu_sw_backup/"
	//系统定义的服务器以及本机名称，用于与服务器之间的通信
	#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_XML	
	//DEBUG开关，缺省打开
	#define IHU_TRACE_DEBUG_ON IHU_TRACE_DEBUG_ALL_ON //利用以上标示位，确定TRACE的级别
	//TRACE开关，缺省打开
	#define IHU_TRACE_MSG_ON IHU_TRACE_MSG_MODE_ALL

	//模块启动的时间长度
	#define IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION  10  //in micro-second

	//CURRENT HW PRODUCT CATELOG
	#define IHU_HARDWARE_PRODUCT_CAT_TYPE 0x0321 // HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_PLCCB_SB_01  0x0321

	//定义是否进入批量生产，从而确定硬件标识从哪里读取
	#define IHU_HARDWARE_MASSIVE_PRODUTION_SET IHU_HARDWARE_MASSIVE_PRODUTION_NO

	//定义固件是否自动升级
	#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_SET IHU_HARDWARE_BURN_ID_FW_UPGRADE_NO


 /***********************************************************************************
 *
 * EMC14681 EMC项目 IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID
 *
 ***********************************************************************************/
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
	
	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL "http://127.0.0.1/test.php"
	//local SW storage address
	#define  IHU_SW_DOWNLOAD_DIR_DEFAULT "/home/pi/ihu_sw_download/"
	#define  IHU_SW_ACTIVE_DIR_DEFAULT "/home/pi/ihu_sw_active/"
	#define  IHU_SW_BACKUP_DIR_DEFAULT "/home/pi/ihu_sw_backup/"
	//系统定义的服务器以及本机名称，用于与服务器之间的通信
	#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_XML	
	//DEBUG开关，缺省打开
	#define IHU_TRACE_DEBUG_ON IHU_TRACE_DEBUG_ALL_ON //利用以上标示位，确定TRACE的级别
	//TRACE开关，缺省打开
	#define IHU_TRACE_MSG_ON IHU_TRACE_MSG_MODE_ALL
	
	//模块启动的时间长度
	#define IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION  10  //in micro-second

	//CURRENT HW PRODUCT CATELOG
	#define IHU_HARDWARE_PRODUCT_CAT_TYPE 0x1111 // HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E1_EMC_68_01 0x1111
	
	//定义是否进入批量生产，从而确定硬件标识从哪里读取
	#define IHU_HARDWARE_MASSIVE_PRODUTION_SET IHU_HARDWARE_MASSIVE_PRODUTION_NO
	
	//定义固件是否自动升级
	#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_SET IHU_HARDWARE_BURN_ID_FW_UPGRADE_NO
	
	//启动区地址指示
	#define IHU_BOOT_IAP_START_ADDRESS 0x80000000
	#define IHU_BOOT_IAP_MAX_LEN  0x20000  //128KB
	#define IHU_BOOT_FAC_LOAD_START_ADDRESS 0x80020000
	#define IHU_BOOT_FAC_LOAD_MAX_LEN  0x40000  //256KB
	#define IHU_BOOT_BLOCK1_LOAD_START_ADDRESS 0x80060000
	#define IHU_BOOT_BLOCK1_LOAD_MAX_LEN  0x40000  //256KB
	#define IHU_BOOT_BLOCK2_LOAD_START_ADDRESS 0x800A0000
	#define IHU_BOOT_BLOCK2_LOAD_MAX_LEN  0x40000  //256KB
	#define IHU_BOOT_FLASH_END_START	0x800FFFFF	
	

 /***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID
 *
 ***********************************************************************************/
 #elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID

	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL 						"http://127.0.0.1/test.php"
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_BH_SERVER_ADDR 	"http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php"
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_BH_SERVER_NAME 	"XHZN_HCU"
	#define IHU_CLOUDXHUI_HCU_NAME_SELF 								"HCU_G514_FHYS_SH001"
	#define IHU_CLOUDXHUI_TCP_SERVER_NAME_ADDR 					"http://www.hkrob.com/"
	#define IHU_CLOUDXHUI_TCP_SERVER_IP_ADDR 						"121.40.118.33"
	#define IHU_CLOUDXHUI_TCP_SERVER_PORT 							"9501"
	#define IHU_CLOUDXHUI_FTP_BH_SERVER_ADDR 						"http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php"
	#define IHU_CLOUDXHUI_FTP_BH_SERVER_USER_NAME 			"test1234"
	#define IHU_CLOUDXHUI_FTP_BH_SERVER_USER_PSWD 			"123456"
	#define IHU_CLOUDXHUI_FTP_BH_SERVER_FILE_NAME 			"ihu_new_pkg.bin"
	#define IHU_CLOUDXHUI_FTP_BH_SERVER_FILE_PATH 			"/"
	
	//local SW storage address
	#define  IHU_SW_DOWNLOAD_DIR_DEFAULT "/home/pi/ihu_sw_download/"
	#define  IHU_SW_ACTIVE_DIR_DEFAULT "/home/pi/ihu_sw_active/"
	#define  IHU_SW_BACKUP_DIR_DEFAULT "/home/pi/ihu_sw_backup/"
	//系统定义的服务器以及本机名称，用于与服务器之间的通信
	#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_XML
	//DEBUG开关，缺省打开
	#define IHU_TRACE_DEBUG_ON IHU_TRACE_DEBUG_NOR_ON + IHU_TRACE_DEBUG_IPT_ON + IHU_TRACE_DEBUG_CRT_ON + IHU_TRACE_DEBUG_FAT_ON  //利用以上标示位，确定TRACE的级别 IHU_TRACE_DEBUG_INF_ON +
	//TRACE开关，缺省打开
	#define IHU_TRACE_MSG_ON  IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT //IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT  // IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT //IHU_TRACE_MSG_MODE_ALL // 
	
	#define IHU_VMMW_GPRSMOD_SMS_CENTER_NUMBER  "+8613010314500"  //短信中心号码
	#define IHU_VMMW_GPRSMOD_SMS_TARGET_NUMBER  "+8613701629240"  //将短信发给谁，暂时没用上，目前短信是交给了后台进行的处理
	
	//模块启动的时间长度
	#define IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION  10  //in micro-second
	
	//CURRENT HW PRODUCT CATELOG
	#define IHU_HARDWARE_PRODUCT_CAT_TYPE 0x0514 // HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5104 0x0514

	//定义产品型号，以及本产品所支持的最大传感器套数
	#if (IHU_HARDWARE_PRODUCT_CAT_TYPE == 0x0501)
		#define IHU_CCL_SENSOR_LOCK_NUMBER_MAX 1
	#elif (IHU_HARDWARE_PRODUCT_CAT_TYPE == 0x0502)
		#define IHU_CCL_SENSOR_LOCK_NUMBER_MAX 2
	#elif (IHU_HARDWARE_PRODUCT_CAT_TYPE == 0x0512)
		#define IHU_CCL_SENSOR_LOCK_NUMBER_MAX 2
	#elif (IHU_HARDWARE_PRODUCT_CAT_TYPE == 0x0514)
		#define IHU_CCL_SENSOR_LOCK_NUMBER_MAX 4
	#elif (IHU_HARDWARE_PRODUCT_CAT_TYPE == 0x0524)
		#define IHU_CCL_SENSOR_LOCK_NUMBER_MAX 4
	#else
	#endif

	//定义是否进入批量生产，从而确定硬件标识从哪里读取
	#define IHU_HARDWARE_MASSIVE_PRODUTION_SET IHU_HARDWARE_MASSIVE_PRODUTION_NO
	//定义固件是否自动升级
	#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_SET IHU_HARDWARE_BURN_ID_FW_UPGRADE_NO
	
	//启动区地址指示
	#define IHU_BOOT_IAP_START_ADDRESS 0x80000000
	#define IHU_BOOT_IAP_MAX_LEN  0x20000  //128KB
	#define IHU_BOOT_FAC_LOAD_START_ADDRESS 0x80020000
	#define IHU_BOOT_FAC_LOAD_MAX_LEN  0x40000  //256KB
	#define IHU_BOOT_BLOCK1_LOAD_START_ADDRESS 0x80060000
	#define IHU_BOOT_BLOCK1_LOAD_MAX_LEN  0x40000  //256KB
	#define IHU_BOOT_BLOCK2_LOAD_START_ADDRESS 0x800A0000
	#define IHU_BOOT_BLOCK2_LOAD_MAX_LEN  0x40000  //256KB
	#define IHU_BOOT_FLASH_END_START	0x800FFFFF

	
/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID
 *
 ***********************************************************************************/
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
	
	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL 						"http://127.0.0.1/test.php"
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_BH_SERVER_ADDR 	"http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php"
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_BH_SERVER_NAME 	"XHZN_HCU"
	#define IHU_CLOUDXHUI_HCU_NAME_SELF 								"HCU_G881_BFSC_SH001"
	#define IHU_CLOUDXHUI_FTP_BH_SERVER_ADDR 						"http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php"
	
	//local SW storage address
	#define  IHU_SW_DOWNLOAD_DIR_DEFAULT "/home/pi/ihu_sw_download/"
	#define  IHU_SW_ACTIVE_DIR_DEFAULT "/home/pi/ihu_sw_active/"
	#define  IHU_SW_BACKUP_DIR_DEFAULT "/home/pi/ihu_sw_backup/"
	//系统定义的服务器以及本机名称，用于与服务器之间的通信
	#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_XML
	//DEBUG开关，缺省打开
	#define IHU_TRACE_DEBUG_ON IHU_TRACE_DEBUG_NOR_ON + IHU_TRACE_DEBUG_IPT_ON + IHU_TRACE_DEBUG_CRT_ON + IHU_TRACE_DEBUG_FAT_ON  //利用以上标示位，确定TRACE的级别 IHU_TRACE_DEBUG_INF_ON + 

	//TRACE开关，缺省打开
	#define IHU_TRACE_MSG_ON  IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT  //IHU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT  //IHU_TRACE_MSG_MODE_ALL   

	//模块启动的时间长度
	#define IHU_MODULE_START_DISTRIBUTION_DELAY_DURATION  10  //in micro-second

	//CURRENT HW PRODUCT CATELOG
	#define IHU_HARDWARE_PRODUCT_CAT_TYPE 0x0881 // HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_IHU_01 0x0881

	//定义是否进入批量生产，从而确定硬件标识从哪里读取
	#define IHU_HARDWARE_MASSIVE_PRODUTION_SET IHU_HARDWARE_MASSIVE_PRODUTION_NO

	//定义固件是否自动升级
	#define IHU_HARDWARE_BURN_ID_FW_UPGRADE_SET IHU_HARDWARE_BURN_ID_FW_UPGRADE_NO

	//启动区地址指示
	#define IHU_BOOT_IAP_START_ADDRESS 0x80000000
	#define IHU_BOOT_IAP_MAX_LEN  0x20000  //128KB
	#define IHU_BOOT_FAC_LOAD_START_ADDRESS 0x80020000
	#define IHU_BOOT_FAC_LOAD_MAX_LEN  0x40000  //256KB
	#define IHU_BOOT_BLOCK1_LOAD_START_ADDRESS 0x80060000
	#define IHU_BOOT_BLOCK1_LOAD_MAX_LEN  0x40000  //256KB
	#define IHU_BOOT_BLOCK2_LOAD_START_ADDRESS 0x800A0000
	#define IHU_BOOT_BLOCK2_LOAD_MAX_LEN  0x40000  //256KB
	#define IHU_BOOT_FLASH_END_START	0x800FFFFF
	
	
//==============================================================================================================================
#else //IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID
	#error Un-correct constant definition
#endif //IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID

#endif /* L0COMVM_SYSCONFIG_H_ */
