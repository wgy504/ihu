/*
 * sysdim.h
 *
 *  Created on: 2016年1月12日
 *      Author: test
 */

#ifndef L0COMVM_SYSDIM_H_
#define L0COMVM_SYSDIM_H_

//系统任务级
//MAX_TASK_NUM_IN_ONE_IHU之前放成64个，为了极致优化内存空间，放这么几个足够，这是最小值，基本上没有优化空间了
//如果不怕麻烦，还可以优化掉MIN=0以及ADCARIES任务模块，但这回造成程序编写的效率以及体系架构非常难看，所以暂时不做
//理想的情况下，MAX_TASK_NUM_IN_ONE_IHU=8，增加MAX以及AIRKISS模块，可惜内存严重不够
#define MAX_TASK_NUM_IN_ONE_IHU 6 
#define TASK_NAME_MAX_LENGTH 12

//系统消息级
//maxmum state number and msg number in one task, no instance concept
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
#define BX_PRINT_SZ 200

//时钟定义的DIMENSION
#define MAX_TIMER_NUM_IN_ONE_IHU_1S 8  //Normal situation
#define MAX_TIMER_NUM_IN_ONE_IHU_10MS 3 //Frame usage
#define MAX_TIMER_NUM_IN_ONE_IHU_1MS 3  //Real-time usage
#define MAX_TIMER_SET_DURATION 100000

#endif /* L0COMVM_SYSDIM_H_ */
