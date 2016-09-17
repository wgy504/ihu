/**
 ****************************************************************************************
 *
 * @file bxxh_adapt.c
 *
 * @brief Adaptation of SPS project
 *
 * BXXH team
 * Created by ZJL, 20150819
 *
 ****************************************************************************************
 */
 
#include "bxxh_adapt.h"


//MYC add debug print preparation buffer
char pDebugPrintBuf[BX_PRINT_SZ];

bool IsDebugTraceOn = true;
	
void SetDebugTraceOnOff(bool debugFlag)
{
		if(true == debugFlag)
				IsDebugTraceOn = true;
		else
				IsDebugTraceOn = false;
}

void SetDebugTraceOn(void)
{
		IsDebugTraceOn = true;
}
void SetDebugTraceOff(void)
{
		IsDebugTraceOn = false;
}

//init each task
void bxxh_init(void)
{
	//init bxxxh itself
  //uint8_t p[64];
	SetDebugTraceOn();
  sprintf(pDebugPrintBuf, "\r\nBXXH: Build: %s, %s\r\n", __DATE__, __TIME__);
	//bxxh_print_console(("%s, %s", __DATE__, __TIME__));
	bxxh_print_console(pDebugPrintBuf);
	bxxh_print_console("BXXH: Application Layer start, print console initlizied!\r\n");
    
  sprintf(pDebugPrintBuf, "BXXH: Current Hardware Type = %d", CURRENT_HW_TYPE);
	bxxh_print_console(pDebugPrintBuf);
  sprintf(pDebugPrintBuf, ", Hardware Version = %d", CURRENT_HW_PEM);
	bxxh_print_console(pDebugPrintBuf);
  sprintf(pDebugPrintBuf, ", Software Release = %d", CURRENT_SW_RELEASE);
	bxxh_print_console(pDebugPrintBuf);    
  sprintf(pDebugPrintBuf, ", Software Delivery = %d", CURRENT_SW_DELIVERY);
	bxxh_print_console(pDebugPrintBuf);    

	uint8_t MacAddress[]={0,0,0,0,0,0};
	nvds_tag_len_t length = 6;
	nvds_get(NVDS_TAG_BD_ADDRESS, &length, &MacAddress[0]);
  sprintf(pDebugPrintBuf, ", MAC Address = %02X %02X %02X %02X %02X %02X\r\n", MacAddress[5], MacAddress[4], MacAddress[3], MacAddress[2], MacAddress[1], MacAddress[0]);
  bxxh_print_console(pDebugPrintBuf);
  
	//Close Trace to ensure no crash because of crash MYC //
	SetDebugTraceOff();
//    bxxh_print_console(", MAC Address = ");
//	int i = 0;	
//	for (i=0; i<6; i++)
//	{
//		bxxh_print_console(hexbyte2string(MacAddress[5-i]));
//	}	
//	bxxh_print_console(".\r\n");

	
	//init test task
	//init wxapp task
	task_wxapp_init();
	
	//init asylibra task
	task_asylibra_init();
}


//give a chance for every task the entry point
void bxxh_task_entry_hook1(void)
{
	//bxxh mainloop entry
	
	//asylibra mainloop entry
	task_wxapp_mainloop();
	
	//asylibra mainloop entry
	task_asylibra_mainloop();
}

//print string to console port (UART2)
void bxxh_print_console(char *p)
{
	if(true == IsDebugTraceOn)
	{
			arch_printf(p);
	}
}



//本地子函数
void msgid_print(uint16_t id)
{
	switch(id)
	{
		case MSG_ASYLIBRA_INIT:
			bxxh_print_console("[MSG_ASYLIBRA_INIT]");
			break;
		case MSG_ASYLIBRA_BLE_CONNNECTD:
			bxxh_print_console("[MSG_ASYLIBRA_BLE_CONNNECTD]");
			break;		
		case MSG_ASYLIBRA_AUTH_REQ:
			bxxh_print_console("[MSG_ASYLIBRA_AUTH_REQ]");
			break;		
		case MSG_ASYLIBRA_AUTH_RESP:
			bxxh_print_console("[MSG_ASYLIBRA_AUTH_RESP]");
			break;		
		case MSG_ASYLIBRA_INIT_REQ:
			bxxh_print_console("[MSG_ASYLIBRA_INIT_REQ]");
			break;	
		case MSG_ASYLIBRA_INIT_RESP:
			bxxh_print_console("[MSG_ASYLIBRA_INIT_RESP]");
			break;	
		case MSG_ASYLIBRA_CONNECT_READY:
			bxxh_print_console("[MSG_ASYLIBRA_CONNECT_READY]");
			break;			
		case MSG_ASYLIBRA_DATA_PUSH:
			bxxh_print_console("[MSG_ASYLIBRA_DATA_PUSH]");
			break;		
		case MSG_ASYLIBRA_DATA_REQ:
			bxxh_print_console("[MSG_ASYLIBRA_DATA_REQ]");
			break;	
		case MSG_ASYLIBRA_DATA_RESP:
			bxxh_print_console("[MSG_ASYLIBRA_DATA_RESP]");
			break;			
		case MSG_ASYLIBRA_AUTH_WAIT_TIMER:
			bxxh_print_console("[MSG_ASYLIBRA_AUTH_WAIT_TIMER]");
			break;				
		case MSG_ASYLIBRA_DISCONNECT_TIGGER_L3:
			bxxh_print_console("[MSG_ASYLIBRA_DISCONNECT_TIGGER_L3]");
			break;				
		case MSG_ASYLIBRA_SEND_BLE_DATA_TIMER:
			bxxh_print_console("[MSG_ASYLIBRA_SEND_BLE_DATA_TIMER]");
			break;					
		case MSG_WXAPP_3MIN_TIMEOUT:
			bxxh_print_console("[MSG_WXAPP_3MIN_TIMEOUT]");
			break;	
		case MSG_WXAPP_5SECOND_TIMEOUT:
			bxxh_print_console("[MSG_WXAPP_5SECOND_TIMEOUT]");
			break;	
		case MSG_WXAPP_DATA_REQ_EMC:
			bxxh_print_console("[MSG_WXAPP_DATA_REQ_EMC]");
			break;	
		case MSG_WXAPP_PERIOD_REPORT_EMC_TRIGGER:
			bxxh_print_console("[MSG_WXAPP_PERIOD_REPORT_EMC_TRIGGER]");
			break;	
		case MSG_WXAPP_DATA_REQ_TIME_SYNC:
			bxxh_print_console("[MSG_WXAPP_DATA_REQ_TIME_SYNC]");
			break;							
		case MSG_SPS_VMDA_BLE_DATA_DL_RCV:
			bxxh_print_console("[MSG_SPS_VMDA_BLE_DATA_RCV]");
			break;	
		case MSG_BLEAPP_VMDA_DISCONNECT:
			bxxh_print_console("[MSG_BLEAPP_VMDA_DISCONNECT]");
			break;	
		case MSG_BLEAPP_VMDA_INIT:
			bxxh_print_console("[MSG_BLEAPP_VMDA_INIT]");
			break;	
		case MSG_APP_SERVER_UART1_DATA_RECEIVED:
			bxxh_print_console("[MSG_APP_SERVER_UART1_DATA_RECEIVED]");
			break;			
		
		default:
			bxxh_print_console("[MSG_XXXX]");
			break;
	}
}


//本地子函数
void taskid_print(uint8_t id)
{
	switch(id)
	{
		case TASK_ASYLIBRA:
			bxxh_print_console("[TASK_ASYLIBRA]");
			break;
		case TASK_WXAPP:
			bxxh_print_console("[TASK_WXAPP]");
			break;
		case TASK_APP:
			bxxh_print_console("[TASK_APP]");
		  break;
//		case TASK_SPS_SERVER:
//			bxxh_print_console("[TASK_SPS_SERVER]");
//			break;		
		default:
			bxxh_print_console("[TASK_XXXX]");
			break;
	}
}


char *hexbyte2string(uint8_t inp)
{
	char *output = "    ";
	uint8_t input, i, tmp;
	input = inp;
	for (i=0; i<2; i++)
	{
		tmp = (input>>((1-i)*4)) & 0x0F;
		switch(tmp)
		{
			case 0:
				output[i] = '0';
				break;
			case 1:
				output[i] = '1';
				break;
			case 2:
				output[i] = '2';
				break;
			case 3:
				output[i] = '3';
				break;
			case 4:
				output[i] = '4';
				break;
			case 5:
				output[i] = '5';
				break;			
			case 6:
				output[i] = '6';
				break;
			case 7:
				output[i] = '7';
				break;
			case 8:
				output[i] = '8';
				break;
			case 9:
				output[i] = '9';
				break;
			case 0xA:
				output[i] = 'A';
				break;
			case 0xB:
				output[i] = 'B';
				break;
			case 0xC:
				output[i] = 'C';
				break;
			case 0xD:
				output[i] = 'D';
				break;
			case 0xE:
				output[i] = 'E';
				break;
			case 0xF:
				output[i] = 'F';
				break;
			default:
				break;
		}		
	}
	return output;
}

char *hexword2string(uint16_t inp)
{
	char *output = "    ";
	uint8_t i, tmp;
	uint16_t input;
	input = inp;
	for (i=0; i<4; i++)
	{
		tmp = (input>>((3-i)*4)) & 0x0F;
		switch(tmp)
		{
			case 0:
				output[i] = '0';
				break;
			case 1:
				output[i] = '1';
				break;
			case 2:
				output[i] = '2';
				break;
			case 3:
				output[i] = '3';
				break;
			case 4:
				output[i] = '4';
				break;
			case 5:
				output[i] = '5';
				break;			
			case 6:
				output[i] = '6';
				break;
			case 7:
				output[i] = '7';
				break;
			case 8:
				output[i] = '8';
				break;
			case 9:
				output[i] = '9';
				break;
			case 0xA:
				output[i] = 'A';
				break;
			case 0xB:
				output[i] = 'B';
				break;
			case 0xC:
				output[i] = 'C';
				break;
			case 0xD:
				output[i] = 'D';
				break;
			case 0xE:
				output[i] = 'E';
				break;
			case 0xF:
				output[i] = 'F';
				break;
			default:
				break;
		}		
	}
	return output;
}

char *hexint2string(unsigned int inp)
{
	char *output = "    ";
	uint8_t i, tmp;
	unsigned int input;
	input = inp;
	for (i=0; i<8; i++)
	{
		tmp = (input>>((7-i)*4)) & 0x0F;
		switch(tmp)
		{
			case 0:
				output[i] = '0';
				break;
			case 1:
				output[i] = '1';
				break;
			case 2:
				output[i] = '2';
				break;
			case 3:
				output[i] = '3';
				break;
			case 4:
				output[i] = '4';
				break;
			case 5:
				output[i] = '5';
				break;			
			case 6:
				output[i] = '6';
				break;
			case 7:
				output[i] = '7';
				break;
			case 8:
				output[i] = '8';
				break;
			case 9:
				output[i] = '9';
				break;
			case 0xA:
				output[i] = 'A';
				break;
			case 0xB:
				output[i] = 'B';
				break;
			case 0xC:
				output[i] = 'C';
				break;
			case 0xD:
				output[i] = 'D';
				break;
			case 0xE:
				output[i] = 'E';
				break;
			case 0xF:
				output[i] = 'F';
				break;
			default:
				break;
		}		
	}
	return output;
}



