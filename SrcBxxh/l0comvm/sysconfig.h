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
 */
//任务模块激活的定义
#define IHU_TASK_PNP_ON 1
#define IHU_TASK_PNP_OFF 0
#define IHU_TASK_PNP_INVALID 0xFF

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

//系统定义的服务器以及本机名称，用于HCU与服务器之间的通信
#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_XML 1
#define IHU_CLOUDXHUI_BH_INTERFACE_STANDARD_ZHB 2

/*
 *
 *   EMCWX项目
 *
 */
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID)
	//DEBUG开关，缺省打开
	#define IHU_TRACE_DEBUG_ON IHU_TRACE_DEBUG_ALL_OFF //利用以上标示位，确定TRACE的级别
	//TRACE开关，缺省打开
	#define IHU_TRACE_MSG_ON IHU_TRACE_MSG_MODE_ALL
	#define IHU_TASK_STATE_VMDA 			IHU_TASK_PNP_ON
	#define IHU_TASK_STATE_TIMER 			IHU_TASK_PNP_ON
	#define IHU_TASK_STATE_ASYLIBRA 		IHU_TASK_PNP_ON
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

/*
 *
 *   EMC14681 EMC项目
 *
 */
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
	#define IHU_COMM_FRONT_ADC 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_I2C 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_PWM 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPS 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPI 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_GPIO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DIDO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_LED 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_ETH 1 //1=ON, 0/OTHERS=OFF
	#define IHU_MAIN_CTRL_EMC68X 1 //1=ON, 0/OTHERS=OFF		
	//定义后台连接网络的接口
	//现在采用这种互斥的方式进行定义，以后需要等待HWINV进行PnP，确保多种接口即插即用，随时切换
	//多种接口之间的优先级关系，则有程序任务自动决定：ETHERNET > WIFI > USB-OTG2 > 3G4G
	//当前版本的做法是，多种接口均启动起来，但如何使用，由CLOUDCONT按照优先级和COMM_BACK_HAWL_CON定义唯一决定
	//Timer setting
	#define IHU_EMC_TIMER_DURATION_PERIOD_READ 600
	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL "http://127.0.0.1/test.php"
	//定义后台CLOUD FTP 的地址
	#define IHU_CLOUDXHUI_FTP_ADDRESS "ftp://121.40.185.177"
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
	
	
/*
 *
 *   数采仪CB项目
 *
 */
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
	#define IHU_COMM_FRONT_ADC 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_I2C 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_PWM 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPS 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPI 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_GPIO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DIDO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_LED 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_ETH 1 //1=ON, 0/OTHERS=OFF
	#define IHU_MAIN_CTRL_SCYCB 1 //1=ON, 0/OTHERS=OFF		
	//定义后台连接网络的接口
	//现在采用这种互斥的方式进行定义，以后需要等待HWINV进行PnP，确保多种接口即插即用，随时切换
	//多种接口之间的优先级关系，则有程序任务自动决定：ETHERNET > WIFI > USB-OTG2 > 3G4G
	//当前版本的做法是，多种接口均启动起来，但如何使用，由CLOUDCONT按照优先级和COMM_BACK_HAWL_CON定义唯一决定
	//Timer setting 
	#define IHU_DIDOCAP_TIMER_DURATION_PERIOD_SCAN 4
	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL "http://127.0.0.1/test.php"
	//定义后台CLOUD FTP 的地址
	#define IHU_CLOUDXHUI_FTP_ADDRESS "ftp://121.40.185.177"
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

/*
 *
 *   流水产线项目主控板
 *
 */
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
	#define IHU_COMM_FRONT_ADC 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_I2C 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_PWM 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPS 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPI 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_GPIO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DIDO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_LED 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_ETH 1 //1=ON, 0/OTHERS=OFF
	#define IHU_MAIN_CTRL_SCYCB 1 //1=ON, 0/OTHERS=OFF		
	//定义后台连接网络的接口
	//现在采用这种互斥的方式进行定义，以后需要等待HWINV进行PnP，确保多种接口即插即用，随时切换
	//多种接口之间的优先级关系，则有程序任务自动决定：ETHERNET > WIFI > USB-OTG2 > 3G4G
	//当前版本的做法是，多种接口均启动起来，但如何使用，由CLOUDCONT按照优先级和COMM_BACK_HAWL_CON定义唯一决定
	//Timer setting
	#define IHU_EMC_TIMER_DURATION_PERIOD_READ 600
	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL "http://127.0.0.1/test.php"
	//定义后台CLOUD FTP 的地址
	#define IHU_CLOUDXHUI_FTP_ADDRESS "ftp://121.40.185.177"
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

/*
 *
 *   流水产线项目，传感器板
 *
 */
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID)
	//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
	#define IHU_COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
	#define IHU_COMM_FRONT_ADC 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_I2C 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_PWM 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPS 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_SPI 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_GPIO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_DIDO 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_LED 1 //1=ON, 0/OTHERS=OFF
	#define IHU_COMM_FRONT_ETH 1 //1=ON, 0/OTHERS=OFF
	#define IHU_MAIN_CTRL_SCYCB 1 //1=ON, 0/OTHERS=OFF		
	//定义后台连接网络的接口
	//现在采用这种互斥的方式进行定义，以后需要等待HWINV进行PnP，确保多种接口即插即用，随时切换
	//多种接口之间的优先级关系，则有程序任务自动决定：ETHERNET > WIFI > USB-OTG2 > 3G4G
	//当前版本的做法是，多种接口均启动起来，但如何使用，由CLOUDCONT按照优先级和COMM_BACK_HAWL_CON定义唯一决定
	//Timer setting
	#define IHU_EMC_TIMER_DURATION_PERIOD_READ 600
	//Series Port config
	#define IHU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
	//定义后台CLOUD连接到服务器
	#define IHU_CLOUDXHUI_HTTP_ADDRESS_LOCAL "http://127.0.0.1/test.php"
	//定义后台CLOUD FTP 的地址
	#define IHU_CLOUDXHUI_FTP_ADDRESS "ftp://121.40.185.177"
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
	
	
	
#else //IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID
#endif //IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID

#endif /* L0COMVM_SYSCONFIG_H_ */
