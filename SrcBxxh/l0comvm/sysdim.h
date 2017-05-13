/*
 * sysdim.h
 *
 *  Created on: 2016年1月12日
 *      Author: test
 */

#ifndef L0COMVM_SYSDIM_H_
#define L0COMVM_SYSDIM_H_

/**********************************************************************************
 *
 *   =====项目配置使用方式=====
 * 
 *   0. 建立一个新的KEIL项目，包括源文件及目录结构
 *   1. 修改sysdim.h，并配置相应的性能参数
 *   2. 确定要使用哪一种操作系统对应的VM，选择VMDA, VMFO, VMUO, VMLO, VMHO，分别对应FreeRTOS, ucosIII, LiteOS, HuiOS
 *   3. 换过VM之后，生成相应项目的commsgxxx.h, l1comdefxxx.h, l1timerxxx.h，这些在不同的项目环境下是不一样的
 *   4. 更新sysconfig.h, 配置相应的工程参数
 *   5. 修改vmxxxlayer.h，修改包含文件以及关系
 *   6. 修改l2frame中的公共模块包含关系
 *   7. 新建l3xxx.h/c, 修改l3app中的包含关系，建立其完整的文件体系
 *   8. 回归编译老旧工程项目，确保不损坏之前的项目
 *
 **********************************************************************************/


//项目定义
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX "IHU_PRG_EMCWX"			//EMC电磁辐射
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID 1
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCSPS "IHU_PRG_EMCSPS" 		//用来测试透传串口的官方例程，暂时不用
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCSPS_ID 2
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB "IHU_PRG_SCYCB"  	//数采仪大板
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID 3
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X "IHU_PRG_EMC68X"  	//采用DA14681芯片的微信项目
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID 4
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB "IHU_PRG_PLCCB"  	//流水线控制板
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID 5
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB "IHU_PRG_PLCSB"		//流水线传感器板
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID 6
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL "IHU_PRG_CCL"  			//云控锁
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID 7
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC "IHU_PRG_BFSC"  		//波峰组合秤
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID 8


//KEIL5选择项目
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID
#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL
#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID


//Eclipse选择项目
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID


//下面两个任务暂时不用，留待称重机项目使用
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID


/***********************************************************************************
 *
 * 为每一个项目，定义独立的性能配置数据
 *
 ***********************************************************************************/
 
 /***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID
 *
 ***********************************************************************************/
//使用了VMDA-BLE独立裸操作系统
#if (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID)
	//为不同的项目配置的系统性能
	//系统任务级
	//IHU_SYSDIM_TASK_NBR_MAX之前放成64个，为了极致优化内存空间，放这么几个足够，这是最小值，基本上没有优化空间了
	//如果不怕麻烦，还可以优化掉MIN=0以及ADCARIES任务模块，但这回造成程序编写的效率以及体系架构非常难看，所以暂时不做
	//理想的情况下，IHU_SYSDIM_TASK_NBR_MAX=8，增加MAX以及AIRKISS模块，可惜内存严重不够
	#define IHU_SYSDIM_TASK_NBR_MAX 6 
	#define IHU_SYSDIM_TASK_NAME_LEN_MAX 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define IHU_TASK_QUEUE_ID_START 1024
	#define IHU_SYSDIM_MSGID_NBR_MAX 20 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define IHU_SYSDIM_MSGID_MASK_SET 255 //消息号段在同一个任务中必须连续到这个范围内
	#define IHU_SYSDIM_TASK_STATE_NBR_MAX 7  //一个任务之中最多定义的状态数量
	#define IHU_SYSDIM_FSM_ENTRY_NBR_MAX 30   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define IHU_SYSDIM_MSGID_NAME_LEN_MAX 70
	#define IHU_SYSDIM_MSG_BODY_LEN_MAX 80
	#define IHU_SYSDIM_FILE_NAME_LEN_MAX 100
	#define IHU_SYSDIM_MSG_QUEUE_NBR_MAX 2
	//SLEEP控制表给任务模块可以给出
	#define MAX_SLEEP_NUM_IN_ONE_TASK 2
	#define IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX 50000
	//MYC add debug print preparation buffer
	#define IHU_SYSDIM_PRINT_CHAR_SIZE_MAX 200
	//TIMER DIMENSION
	//最大的定时器数量
	#define IHU_SYSDIM_TIMERID_NBR_MAX 40
	#define IHU_SYSDIM_TIMERID_NAME_LEN_MAX 40
	#define IHU_SYSDIM_TIMERID_NBR_MAX_1S 8  //Normal situation
	#define IHU_SYSDIM_TIMERID_NBR_MAX_10MS 3 //Frame usage
	#define IHU_SYSDIM_TIMERID_NBR_MAX_1MS 3  //Real-time usage
	#define MAX_TIMER_SET_DURATION 100000


/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID
 *
 ***********************************************************************************/
//使用了VMFO-FreeRTOS操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	#define IHU_SYSDIM_TASK_NBR_MAX 14
	#define IHU_SYSDIM_TASK_NAME_LEN_MAX 12
  #define IHU_SYSDIM_TASK_STACK_SIZE_MAX 200 //任务堆栈长度
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	//#define IHU_TASK_QUEUE_ID_START 1024
	#define IHU_SYSDIM_MSGID_NBR_MAX 20 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define IHU_SYSDIM_MSGID_MASK_SET 255 //消息号段在同一个任务中必须连续到这个范围内
	#define IHU_SYSDIM_TASK_STATE_NBR_MAX 7  //一个任务之中最多定义的状态数量
	#define IHU_SYSDIM_FSM_ENTRY_NBR_MAX 16   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息参数
	#define IHU_SYSDIM_MSGID_NAME_LEN_MAX 70
	#define IHU_SYSDIM_MSG_BODY_LEN_MAX 64  //消息最长长度，这里比较短节省内存，按道理，没有图像和视频的情况下，是足够的
	#define IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX IHU_SYSDIM_MSG_BODY_LEN_MAX - 3
	#define IHU_SYSDIM_FILE_NAME_LEN_MAX 100
	#define IHU_SYSDIM_MSG_QUEUE_NBR_MAX 2
  #define IHU_SYSDIM_MSG_QUEUE_SIZE_MAX IHU_SYSDIM_MSG_BODY_LEN_MAX + 6 //should be = IHU_SYSMSG_COM_BODY_LEN_MAX
	//SLEEP控制表给任务模块可以给出
	#define IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX 50000
	#define IHU_SYSDIM_PRINT_CHAR_SIZE_MAX 128
        #define IHU_SYSDIM_PRINT_FILE_LINE_NBR_MAX 60
	#define IHU_SYSDIM_PRINT_BUF_NBR_MAX 10
	//#define IHU_SYSDIM_PRINT_MUTEX_TIME_OUT_DUR 100	
	//最大的定时器数量
	#define IHU_SYSDIM_TIMERID_NBR_MAX 40
	#define IHU_SYSDIM_TIMERID_NAME_LEN_MAX 40




/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID
 *
 ***********************************************************************************/
//使用了VMUO-ucos操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID)
	#define IHU_SYSDIM_TASK_NBR_MAX 16
	#define IHU_SYSDIM_TASK_NAME_LEN_MAX 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define IHU_TASK_QUEUE_ID_START 1024
	#define IHU_SYSDIM_MSGID_NBR_MAX 32 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define IHU_SYSDIM_MSGID_MASK_SET 255 //消息号段在同一个任务中必须连续到这个范围内
	#define IHU_SYSDIM_TASK_STATE_NBR_MAX 9  //一个任务之中最多定义的状态数量
	#define IHU_SYSDIM_FSM_ENTRY_NBR_MAX 32   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define IHU_SYSDIM_MSGID_NAME_LEN_MAX 70
	//ucosiii下的PARTITION必须跟256/128/64/32对其，不然会导致出错
	#define IHU_SYSDIM_MSG_BODY_LEN_MAX 58  //ARM Cortex-M3的能力没有想象的那么强大，所以需要将MessageLength大大降低
	#define IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX IHU_SYSDIM_MSG_BODY_LEN_MAX - 3
	#define IHU_SYSDIM_FILE_NAME_LEN_MAX 100
	#define IHU_SYSDIM_MSG_QUEUE_NBR_MAX 2
	#define IHU_SYSDIM_PARTITION_MAX 5
	//SLEEP控制表给任务模块可以给出
//	#define MAX_SLEEP_NUM_IN_ONE_TASK 2
//	#define IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX 50000
	#define IHU_SYSDIM_PRINT_CHAR_SIZE_MAX 128
	//最大的定时器数量
	#define IHU_SYSDIM_TIMERID_NBR_MAX 40
	#define IHU_SYSDIM_TIMERID_NAME_LEN_MAX 40



/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID
 *
 ***********************************************************************************/
//使用了VMUO-ucos操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID)
	#define IHU_SYSDIM_TASK_NBR_MAX 16
	#define IHU_SYSDIM_TASK_NAME_LEN_MAX 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define IHU_TASK_QUEUE_ID_START 1024
	#define IHU_SYSDIM_MSGID_NBR_MAX 32 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define IHU_SYSDIM_MSGID_MASK_SET 255 //消息号段在同一个任务中必须连续到这个范围内
	#define IHU_SYSDIM_TASK_STATE_NBR_MAX 9  //一个任务之中最多定义的状态数量
	#define IHU_SYSDIM_FSM_ENTRY_NBR_MAX 32   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define IHU_SYSDIM_MSGID_NAME_LEN_MAX 70
	#define IHU_SYSDIM_MSG_BODY_LEN_MAX 58
	#define IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX IHU_SYSDIM_MSG_BODY_LEN_MAX - 3
	#define IHU_SYSDIM_FILE_NAME_LEN_MAX 100
	#define IHU_SYSDIM_MSG_QUEUE_NBR_MAX 2
	#define IHU_SYSDIM_PARTITION_MAX 5
	//SLEEP控制表给任务模块可以给出
//	#define MAX_SLEEP_NUM_IN_ONE_TASK 2
//	#define IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX 50000
	#define IHU_SYSDIM_PRINT_CHAR_SIZE_MAX 1000
	//最大的定时器数量
	#define IHU_SYSDIM_TIMERID_NBR_MAX 40
	#define IHU_SYSDIM_TIMERID_NAME_LEN_MAX 40




/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID
 *
 ***********************************************************************************/
//使用了VMUO-ucos操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID)
	#define IHU_SYSDIM_TASK_NBR_MAX 16
	#define IHU_SYSDIM_TASK_NAME_LEN_MAX 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define IHU_TASK_QUEUE_ID_START 1024
	#define IHU_SYSDIM_MSGID_NBR_MAX 32 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define IHU_SYSDIM_MSGID_MASK_SET 255 //消息号段在同一个任务中必须连续到这个范围内
	#define IHU_SYSDIM_TASK_STATE_NBR_MAX 9  //一个任务之中最多定义的状态数量
	#define IHU_SYSDIM_FSM_ENTRY_NBR_MAX 32   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define IHU_SYSDIM_MSGID_NAME_LEN_MAX 70
	#define IHU_SYSDIM_MSG_BODY_LEN_MAX 58
	#define IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX IHU_SYSDIM_MSG_BODY_LEN_MAX - 3
	#define IHU_SYSDIM_FILE_NAME_LEN_MAX 100
	#define IHU_SYSDIM_MSG_QUEUE_NBR_MAX 2
	#define IHU_SYSDIM_PARTITION_MAX 5
	//SLEEP控制表给任务模块可以给出
//	#define MAX_SLEEP_NUM_IN_ONE_TASK 2
//	#define IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX 50000
	#define IHU_SYSDIM_PRINT_CHAR_SIZE_MAX 1000
	//最大的定时器数量
	#define IHU_SYSDIM_TIMERID_NBR_MAX 40
	#define IHU_SYSDIM_TIMERID_NAME_LEN_MAX 40




/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID
 *
 ***********************************************************************************/
//使用了VMFO-FreeRTOS操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#define IHU_SYSDIM_TASK_NBR_MAX 11
	#define IHU_SYSDIM_TASK_NAME_LEN_MAX 12
  #define IHU_SYSDIM_TASK_STACK_SIZE_MAX 800 //任务堆栈长度，非常重要，将影响HardFault陷入崩溃的产生
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	//#define IHU_TASK_QUEUE_ID_START 1024
	#define IHU_SYSDIM_MSGID_NBR_MAX 43 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define IHU_SYSDIM_MSGID_MASK_SET 255 //消息号段在同一个任务中必须连续到这个范围内
	#define IHU_SYSDIM_TASK_STATE_NBR_MAX 10  //一个任务之中最多定义的状态数量
	#define IHU_SYSDIM_FSM_ENTRY_NBR_MAX 42   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息参数
	#define IHU_SYSDIM_MSGID_NAME_LEN_MAX 70
	#define IHU_SYSDIM_MSG_BODY_LEN_MAX 560  //消息最长长度，这里比较短节省内存，按道理，没有图像和视频的情况下，是足够的。加上6B的头以后最好4字节对其。
	#define IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX IHU_SYSDIM_MSG_BODY_LEN_MAX - 4
	#define IHU_SYSDIM_FILE_NAME_LEN_MAX 64
	#define IHU_SYSDIM_MSG_QUEUE_NBR_MAX 2
  #define IHU_SYSDIM_MSG_QUEUE_SIZE_MAX IHU_SYSDIM_MSG_BODY_LEN_MAX + 24 //由于涉及到4B对其，消息长度必须是头部4B对其，消息体4B对其！！！
	//SLEEP控制表给任务模块可以给出
	#define IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX 50000
	#define IHU_SYSDIM_PRINT_CHAR_SIZE_MAX 300 // IHU_SYSDIM_MSG_QUEUE_SIZE_MAX + 300  //打印的缓冲区最好放大点，这个区域的内容如果太大，超过了系统堆栈可能分配的长度，将会使得它覆盖其它变量，危险！
	#define IHU_SYSDIM_PRINT_FILE_LINE_NBR_MAX 60
	#define IHU_SYSDIM_PRINT_BUF_NBR_MAX 1  //太长的打印缓冲区，会造成覆盖其它区域
	#define IHU_SYSDIM_PRINT_MUTEX_TIME_OUT_DUR 100
	//最大的定时器数量
	#define IHU_SYSDIM_TIMERID_NBR_MAX 40
	#define IHU_SYSDIM_TIMERID_NAME_LEN_MAX 40



/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID
 *
 ***********************************************************************************/
//使用了VMFO-FreeRTOS操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#define IHU_SYSDIM_TASK_NBR_MAX 10
	#define IHU_SYSDIM_TASK_NAME_LEN_MAX 12
  #define IHU_SYSDIM_TASK_STACK_SIZE_MAX 600 //任务堆栈长度
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	//#define IHU_TASK_QUEUE_ID_START 1024
	#define IHU_SYSDIM_MSGID_NBR_MAX 52 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define IHU_SYSDIM_MSGID_MASK_SET 255 //消息号段在同一个任务中必须连续到这个范围内
	#define IHU_SYSDIM_TASK_STATE_NBR_MAX 16  //一个任务之中最多定义的状态数量
	#define IHU_SYSDIM_FSM_ENTRY_NBR_MAX 50   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息参数
	#define IHU_SYSDIM_MSGID_NAME_LEN_MAX 70
	#define IHU_SYSDIM_MSG_BODY_LEN_MAX 252  //消息最长长度，这里比较短节省内存，按道理，没有图像和视频的情况下，是足够的 //MYC from 205 to 252
	#define IHU_SYSDIM_L2FRAME_MSG_BODY_LEN_MAX IHU_SYSDIM_MSG_BODY_LEN_MAX - 3
	#define IHU_SYSDIM_FILE_NAME_LEN_MAX 100
	#define IHU_SYSDIM_MSG_QUEUE_NBR_MAX 2
  #define IHU_SYSDIM_MSG_QUEUE_SIZE_MAX 	IHU_SYSDIM_MSG_BODY_LEN_MAX + 24 //由于涉及到4B对其，消息长度必须是头部4B对其，消息体4B对其！！！
	//SLEEP控制表给任务模块可以给出
	#define IHU_SYSDIM_SLEEP_UP_LIMIT_CNT_MAX 50000
	#define IHU_SYSDIM_PRINT_CHAR_SIZE_MAX 300 // IHU_SYSDIM_MSG_QUEUE_SIZE_MAX + 300  //打印的缓冲区最好放大点
	#define IHU_SYSDIM_PRINT_FILE_LINE_NBR_MAX 60
	#define IHU_SYSDIM_PRINT_BUF_NBR_MAX 1  //太长的打印缓冲区，是否会造成覆盖其它区域
	#define IHU_SYSDIM_PRINT_MUTEX_TIME_OUT_DUR 100
	//最大的定时器数量
	#define IHU_SYSDIM_TIMERID_NBR_MAX 40
	#define IHU_SYSDIM_TIMERID_NAME_LEN_MAX 40

	
#else
	#error Un-correct constant definition
#endif

#endif /* L0COMVM_SYSDIM_H_ */
