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
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL
//#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID
#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC
#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID


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
	//MAX_TASK_NUM_IN_ONE_IHU之前放成64个，为了极致优化内存空间，放这么几个足够，这是最小值，基本上没有优化空间了
	//如果不怕麻烦，还可以优化掉MIN=0以及ADCARIES任务模块，但这回造成程序编写的效率以及体系架构非常难看，所以暂时不做
	//理想的情况下，MAX_TASK_NUM_IN_ONE_IHU=8，增加MAX以及AIRKISS模块，可惜内存严重不够
	#define MAX_TASK_NUM_IN_ONE_IHU 6 
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define IHU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 20 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 7  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 30   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_IHU_MSG_BODY_LENGTH 80
	#define IHU_FILE_NAME_LENGTH_MAX 100
	#define MAX_QUEUE_NUM_IN_ONE_TASK 2
	//SLEEP控制表给任务模块可以给出
	#define MAX_SLEEP_NUM_IN_ONE_TASK 2
	#define MAX_SLEEP_COUNTER_UP_LIMITATION 50000
	//MYC add debug print preparation buffer
	#define IHU_PRINT_CHAR_SIZE 200
	//TIMER DIMENSION
	#define MAX_TIMER_NUM_IN_ONE_IHU_1S 8  //Normal situation
	#define MAX_TIMER_NUM_IN_ONE_IHU_10MS 3 //Frame usage
	#define MAX_TIMER_NUM_IN_ONE_IHU_1MS 3  //Real-time usage
	#define MAX_TIMER_SET_DURATION 100000



/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID
 *
 ***********************************************************************************/
//使用了VMFO-FreeRTOS操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID)
	#define MAX_TASK_NUM_IN_ONE_IHU 14
	#define TASK_NAME_MAX_LENGTH 12
  #define IHU_TASK_STACK_SIZE 200 //任务堆栈长度
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	//#define IHU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 20 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 7  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 16   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息参数
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_IHU_MSG_BODY_LENGTH 64  //消息最长长度，这里比较短节省内存，按道理，没有图像和视频的情况下，是足够的
	#define IHU_MSG_BODY_L2FRAME_MAX_LEN MAX_IHU_MSG_BODY_LENGTH - 3
	#define IHU_FILE_NAME_LENGTH_MAX 100
	#define MAX_QUEUE_NUM_IN_ONE_TASK 2
  #define IHU_QUEUE_MAX_SIZE MAX_IHU_MSG_BODY_LENGTH + 6 //should be = MAX_IHU_MSG_BUF_LENGTH
	//SLEEP控制表给任务模块可以给出
	#define MAX_SLEEP_COUNTER_UP_LIMITATION 50000
	#define IHU_PRINT_CHAR_SIZE 128
        #define IHU_PRINT_FILE_LINE_SIZE 60
	#define IHU_PRINT_BUFFER_NUMBER 10
	//#define IHU_PRINT_MUTEX_TIME_OUT_DURATION 100	




/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID
 *
 ***********************************************************************************/
//使用了VMUO-ucos操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID)
	#define MAX_TASK_NUM_IN_ONE_IHU 16
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define IHU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 32 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 9  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 32   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	//ucosiii下的PARTITION必须跟256/128/64/32对其，不然会导致出错
	#define MAX_IHU_MSG_BODY_LENGTH 58  //ARM Cortex-M3的能力没有想象的那么强大，所以需要将MessageLength大大降低
	#define IHU_MSG_BODY_L2FRAME_MAX_LEN MAX_IHU_MSG_BODY_LENGTH - 3
	#define IHU_FILE_NAME_LENGTH_MAX 100
	#define MAX_QUEUE_NUM_IN_ONE_TASK 2
	#define IHU_MAX_PARTITION_TOTAL 5
	//SLEEP控制表给任务模块可以给出
//	#define MAX_SLEEP_NUM_IN_ONE_TASK 2
//	#define MAX_SLEEP_COUNTER_UP_LIMITATION 50000
	#define IHU_PRINT_CHAR_SIZE 128



/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID
 *
 ***********************************************************************************/
//使用了VMUO-ucos操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID)
	#define MAX_TASK_NUM_IN_ONE_IHU 16
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define IHU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 32 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 9  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 32   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_IHU_MSG_BODY_LENGTH 58
	#define IHU_MSG_BODY_L2FRAME_MAX_LEN MAX_IHU_MSG_BODY_LENGTH - 3
	#define IHU_FILE_NAME_LENGTH_MAX 100
	#define MAX_QUEUE_NUM_IN_ONE_TASK 2
	#define IHU_MAX_PARTITION_TOTAL 5
	//SLEEP控制表给任务模块可以给出
//	#define MAX_SLEEP_NUM_IN_ONE_TASK 2
//	#define MAX_SLEEP_COUNTER_UP_LIMITATION 50000
	#define IHU_PRINT_CHAR_SIZE 1000




/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID
 *
 ***********************************************************************************/
//使用了VMUO-ucos操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID)
	#define MAX_TASK_NUM_IN_ONE_IHU 16
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define IHU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 32 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 9  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 32   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_IHU_MSG_BODY_LENGTH 58
	#define IHU_MSG_BODY_L2FRAME_MAX_LEN MAX_IHU_MSG_BODY_LENGTH - 3
	#define IHU_FILE_NAME_LENGTH_MAX 100
	#define MAX_QUEUE_NUM_IN_ONE_TASK 2
	#define IHU_MAX_PARTITION_TOTAL 5
	//SLEEP控制表给任务模块可以给出
//	#define MAX_SLEEP_NUM_IN_ONE_TASK 2
//	#define MAX_SLEEP_COUNTER_UP_LIMITATION 50000
	#define IHU_PRINT_CHAR_SIZE 1000




/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID
 *
 ***********************************************************************************/
//使用了VMFO-FreeRTOS操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
	#define MAX_TASK_NUM_IN_ONE_IHU 11
	#define TASK_NAME_MAX_LENGTH 12
  #define IHU_TASK_STACK_SIZE 600 //任务堆栈长度，非常重要，将影响HardFault陷入崩溃的产生
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	//#define IHU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 40 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 9  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 42   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息参数
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_IHU_MSG_BODY_LENGTH 560  //消息最长长度，这里比较短节省内存，按道理，没有图像和视频的情况下，是足够的。加上6B的头以后最好4字节对其。
	#define IHU_MSG_BODY_L2FRAME_MAX_LEN MAX_IHU_MSG_BODY_LENGTH - 4
	#define IHU_FILE_NAME_LENGTH_MAX 64
	#define MAX_QUEUE_NUM_IN_ONE_TASK 2
  #define IHU_QUEUE_MAX_SIZE MAX_IHU_MSG_BODY_LENGTH + 24 //由于涉及到4B对其，消息长度必须是头部4B对其，消息体4B对其！！！
	//SLEEP控制表给任务模块可以给出
	#define MAX_SLEEP_COUNTER_UP_LIMITATION 50000
	#define IHU_PRINT_CHAR_SIZE 300 // IHU_QUEUE_MAX_SIZE + 300  //打印的缓冲区最好放大点，这个区域的内容如果太大，超过了系统堆栈可能分配的长度，将会使得它覆盖其它变量，危险！
	#define IHU_PRINT_FILE_LINE_SIZE 60
	#define IHU_PRINT_BUFFER_NUMBER 1  //太长的打印缓冲区，会造成覆盖其它区域
	#define IHU_PRINT_MUTEX_TIME_OUT_DURATION 100



/***********************************************************************************
 *
 * IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID
 *
 ***********************************************************************************/
//使用了VMFO-FreeRTOS操作系统
#elif (IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_BFSC_ID)
	#define MAX_TASK_NUM_IN_ONE_IHU 10
	#define TASK_NAME_MAX_LENGTH 12
  #define IHU_TASK_STACK_SIZE 400 //任务堆栈长度
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	//#define IHU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 52 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 9  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 38   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息参数
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_IHU_MSG_BODY_LENGTH 205  //消息最长长度，这里比较短节省内存，按道理，没有图像和视频的情况下，是足够的
	#define IHU_MSG_BODY_L2FRAME_MAX_LEN MAX_IHU_MSG_BODY_LENGTH - 3
	#define IHU_FILE_NAME_LENGTH_MAX 100
	#define MAX_QUEUE_NUM_IN_ONE_TASK 2
  #define IHU_QUEUE_MAX_SIZE MAX_IHU_MSG_BODY_LENGTH + 24 //由于涉及到4B对其，消息长度必须是头部4B对其，消息体4B对其！！！
	//SLEEP控制表给任务模块可以给出
	#define MAX_SLEEP_COUNTER_UP_LIMITATION 50000
	#define IHU_PRINT_CHAR_SIZE 300 // IHU_QUEUE_MAX_SIZE + 300  //打印的缓冲区最好放大点
	#define IHU_PRINT_FILE_LINE_SIZE 60
	#define IHU_PRINT_BUFFER_NUMBER 1  //太长的打印缓冲区，是否会造成覆盖其它区域
	#define IHU_PRINT_MUTEX_TIME_OUT_DURATION 100
	
	
#else
	#error Un-correct constant definition
#endif

#endif /* L0COMVM_SYSDIM_H_ */
