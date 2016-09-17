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


#endif /* L0COMVM_COMTYPE_H_ */
