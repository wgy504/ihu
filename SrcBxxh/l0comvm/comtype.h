/*
 * comtype.h
 *
 *  Created on: 2016年1月6日
 *      Author: test
 */

#ifndef L0COMVM_COMTYPE_H_
#define L0COMVM_COMTYPE_H_

/*1. 公共结构名定义
** Date types used in this project.
*/
typedef unsigned char   		UINT8;
typedef signed char     		INT8;
typedef char            		CHAR;           //only for character array
typedef unsigned short  		UINT16;
typedef signed short    		INT16;
typedef unsigned int    		UINT32;
typedef	signed int      		INT32;  	
typedef unsigned long int 	UINT64;
typedef signed long int 		INT64;
typedef unsigned long   		ULONG;
typedef signed long     		SLONG;
typedef UINT8           		BOOLEAN;          //defined for RP
typedef void            		VOID;
//typedef uint16_t 						ke_task_id_t;
//typedef uint16_t 						ke_msg_id_t;
//typedef uint16_t 						ke_state_t;

typedef INT32         	OPSTAT;
#define SUCCESS        	0
#define FAILURE        	-1
#define true   					1
#define false						0
#define TRUE   					1
#define FALSE						0
	
/*
** Definition of invalid values used in the project.
*/
#define  	IHU_INVALID_U32					0xffffffff
#define  	IHU_INVALID_U16					0xffff
#define  	IHU_INVALID_U8					0xff
#define 	IHU_LOGIC_TRUE					0x1
#define 	IHU_LOGIC_FALSE					0x0
#define 	IHU_INVALID_PARAMETER			0xffffffff
#define 	IHU_MEASURE_RECEIVED			0x0
#define 	IHU_COMMAND_RECEIVED			0x1
#define 	IHU_MEASURE_COMMAND_RECEIVED	0x2

//当前项目
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX "IHU_PRG_EMCWX"
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID 1
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCSPS "IHU_PRG_EMCSPS"
#define IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCSPS_ID 2
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB "IHU_PRG_SCYCB"  //数采仪大板
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_SCYCB_ID 3
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X "IHU_PRG_EMC68X"  //采用DA14681芯片的微信项目
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X_ID 4
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB "IHU_PRG_PLCCB"  //流水线控制板
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCCB_ID 5
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB "IHU_PRG_PLCSB"  //流水线传感器板
#define IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_PLCSB_ID 6
#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX
#define IHU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMCWX_ID

#define IHU_WORKING_VM_VMDA_USAGE_ID 1
#define IHU_WORKING_VM_VMFREEOS_USAGE_ID 2
#define IHU_WORKING_VM_VMUCOS_USAGE_ID 3
#define IHU_WORKING_VM_VMHUIOS_USAGE_ID 4
#define IHU_WORKING_VM_VMLITEOS_USAGE_ID 5
#define IHU_WORKING_VM_CURRENT_USAGE_ID IHU_WORKING_VM_VMDA_USAGE_ID

#endif /* L0COMVM_COMTYPE_H_ */
