/*
*	WeChat mpbledemo2 
*
*	author by anqiren  2014/12/02  V1.0bat
*
**/
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "rwble_config.h"
#include "app_wechat.h"
#include "wechat_aes.h"
#include "epb_MmBp.h"
#include "ble_wechat_util.h"
#include "md5.h"
#include "crc32.h"
#include "mpbledemo2.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "arch_console.h"
#include "wkupct_quadec.h" 
#include "ke_mem.h"
#include "ke_task.h"
#include "wechat_task.h"
#include "vmlayer.h"

mpbledemo2_info m_info = {CMD_NULL, {NULL, 0}};

#if defined EAM_md5AndNoEnrypt || EAM_md5AndAesEnrypt
	uint8_t md5_type_and_id[16];
#endif

uint8_t challeange[CHALLENAGE_LENGTH] = {0x11,0x22,0x33,0x44}; //为了方便这里填了一组确定的数组，使用过程中请自行生成随机数

mpbledemo2_state mpbledemo2Sta = {false, false, false, false, false, false,0,0,0};
const uint8_t key[16] = DEVICE_KEY
uint8_t session_key[32] = {0};
data_handler *m_mpbledemo2_handler = NULL;
uint8_t isLightOn __attribute__((section("retention_mem_area0"), zero_init));;

static void mpbledemo2_handleCmdFromServer(BleDemo2CmdID cmd, uint8_t *ptrData, uint32_t lengthInByte);

/**@brief   Function for the light initialization.
 *
 * @details Initializes all lights used by this application.
 */

int32_t mpbledemo2_get_md5(void)
{
	int32_t error_code = 0;
	#if defined EAM_md5AndNoEnrypt || EAM_md5AndAesEnrypt
	char device_type[] = DEVICE_TYPE;
	char device_id[] = DEVICE_ID;
	char argv[sizeof(DEVICE_TYPE) + sizeof(DEVICE_ID) - 1];
	memcpy(argv,device_type,sizeof(DEVICE_TYPE));
/*when add the DEVICE_ID to DEVICE_TYPE, the offset shuld -1 to overwrite '\0'  at the end of DEVICE_TYPE */
	memcpy(argv + sizeof(DEVICE_TYPE)-1,device_id,sizeof(DEVICE_ID));
#ifdef CATCH_LOG	
	arch_printf ( "\r\nDEVICE_TYPE and DEVICE_ID:%s\r\n",argv);
#endif
	error_code = md5(argv, md5_type_and_id);
#ifdef CATCH_LOG
	arch_printf ( "\r\nMD5:");
	for ( uint8_t i = 0; i < 16; i++ )
	arch_printf ( " %02x", md5_type_and_id[i] );
	
#endif
#endif
	return error_code;
}

void mpbledemo2_reset()
{
	mpbledemo2Sta.auth_send = false;
	mpbledemo2Sta.auth_state = false;
	mpbledemo2Sta.indication_state = false;
	mpbledemo2Sta.init_send = false;
	mpbledemo2Sta.init_state = false;
	mpbledemo2Sta.send_data_seq = 0;
	mpbledemo2Sta.push_data_seq = 0;
	mpbledemo2Sta.seq = 0;
}
    
static void gpio_button_enable(void);
int32_t mpbledemo2_button_handler(uint8_t pin_no);
static void gpio_button_handler(void)
{
    mpbledemo2_button_handler(MPBLEDEMO2_BUTTON_1);
    gpio_button_enable();
}

static void gpio_button_enable(void)
{   
   wkupct_register_callback(gpio_button_handler);
   wkupct_enable_irq(0x200, 0x200, 1, 0x14); // P1_1, polarity low , 1 event, debouncing time = 20ms
}


int32_t mpbledemo2_init(void)
{
	mpbledemo2_reset();
	return (mpbledemo2_get_md5());
}

int32_t mpbledemo2_init_peripheral_func(void)
{
#if USE_PUSH_BUTTON
    RESERVE_GPIO( PUSH_BUTTON, GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, PID_GPIO);   
#endif // USE_PUSH_BUTTON
    RESERVE_GPIO( GREEN_LED, GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, PID_GPIO);  
    
#if USE_PUSH_BUTTON
    GPIO_ConfigurePin( GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, INPUT_PULLUP, PID_GPIO, false ); // Push Button 
#endif // USE_PUSH_BUTTON
    GPIO_ConfigurePin( GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, OUTPUT, PID_GPIO, isLightOn ); //Alert LED
    
    gpio_button_enable();    
    
    return 0;
}    

void mpbledemo2_indication_state(bool isEnable)
{
    mpbledemo2Sta.indication_state = isEnable;
    if (false == isEnable)
    {
        mpbledemo2Sta.auth_send = false;
    }
}

/**@brief Function for handling button pull events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
int32_t mpbledemo2_button_handler(uint8_t pin_no)
{
    // check whether the authority and init procedures have already been completed successfully
    if (!(mpbledemo2Sta.auth_state) || !(mpbledemo2Sta.init_state))
    {
        return -1;
    }
    
    if (!ble_wechat_is_last_data_sent_out())
    {
        return -1;
    }
    
    if (FirstNotificationBit || SecondNotificationBit)
    {
        return -1;
    }
		
		
    int32_t retValue = 0;
    
	switch (pin_no)
    {
        case MPBLEDEMO2_BUTTON_1:
		{
            struct wechat_send_data_req * req = KE_MSG_ALLOC(WECHAT_SEND_DATA_TO_MASTER, TASK_APP,
                                                TASK_APP, wechat_send_data_req);
            req->dataLen = sizeof(SEND_HELLO_WECHAT);
            req->pDataBuf = SEND_HELLO_WECHAT;
            ke_msg_send(req);
            FirstNotificationBit = 1;
            SecondNotificationBit = 1;
            break;
        }
        default:
            break;
    }
    
	return retValue;
}

int32_t device_auth()
{	
	if (m_mpbledemo2_handler == NULL) {
			m_mpbledemo2_handler = get_handler_by_type(PRODUCT_TYPE_MPBLEDEMO2);
		}
		uint8_t *data = NULL;
		uint32_t len = 0;
		
		ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_AUTH);	
		m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
		if(data == NULL)
		{
			return errorCodeProduce;
		}
        
		//sent data
		
	#ifdef CATCH_LOG
		arch_printf("\r\n auth send! ");
	#endif
		#ifdef CATCH_LOG
				arch_printf("\r\n##send data: ");
				uint8_t *d = data;
				for(uint8_t i=0;i<len;++i){
				arch_printf(" %x",d[i]);}
				BpFixHead *fix_head = (BpFixHead *)data;
				arch_printf("\r\n CMDID: %d", ntohs(fix_head->nCmdId));
				arch_printf("\r\n len: %d", ntohs(fix_head->nLength ));
				arch_printf("\r\n Seq: %d", ntohs(fix_head->nSeq));
			#endif
				ble_wechat_indicate_data(data, len);
		return 0;
	}
	

int32_t	device_init()
	{
		uint8_t *data = NULL;
		uint32_t len = 0;
		ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_INIT);
		m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
		if(data == NULL)
		{
			return errorCodeProduce;
		}
        
        
		//sent data
		
		ble_wechat_indicate_data(data, len);
	#ifdef CATCH_LOG
		arch_printf("\r\n init send! ");
	#endif
		#ifdef CATCH_LOG
				arch_printf("\r\n##send data: ");
				uint8_t *d = data;
				for(uint8_t i=0;i<len;++i){
				arch_printf(" %x",d[i]);}
				BpFixHead *fix_head = (BpFixHead *)data;
				arch_printf("\r\n CMDID: %d", ntohs(fix_head->nCmdId));
				arch_printf("\r\n len: %d", ntohs(fix_head->nLength ));
				arch_printf("\r\n Seq: %d", ntohs(fix_head->nSeq));
			#endif
		return 0;
}

void mpbledemo2_main_process()
{
	int error_code = 0;
	if((mpbledemo2Sta.indication_state) && (!mpbledemo2Sta.auth_state) && (!mpbledemo2Sta.auth_send) )
	{
		error_code = device_auth();
        if (0 == error_code)
        {
            mpbledemo2Sta.auth_send = true;	
        }
	}
	if((mpbledemo2Sta.auth_state) && (!mpbledemo2Sta.init_state) && (!mpbledemo2Sta.init_send))
	{
		error_code = device_init();
        if (0 == error_code)
        {
            mpbledemo2Sta.init_send = true;            
        }
	}
}

void mpbledemo2_data_free_func(uint8_t *data, uint32_t len)
{
	if(data)
    {
		ke_free(data);
		data = NULL;
    }
}

//从DEVICE发送命令到后台
void mpbledemo2_data_produce_func(void *args, uint8_t **r_data, uint32_t *r_len)
{	
		static uint16_t bleDemoHeadLen = sizeof(BlueDemoHead);
		mpbledemo2_info *info = (mpbledemo2_info *)args;
		BaseRequest basReq = {NULL};
		static uint8_t fix_head_len = sizeof(BpFixHead);
		BpFixHead fix_head = {0xFE, 1, 0, htons(ECI_req_auth), 0};
		mpbledemo2Sta.seq++;
		switch (info->cmd)
		{
		case CMD_AUTH:
			{
			#if defined EAM_md5AndAesEnrypt
				uint8_t deviceid[] = DEVICE_ID;
				static uint32_t seq = 0x00000001;//
				uint32_t ran = 0x11223344;//为了方便起见这里放了一个固定值做为随机数，在使用时请自行生成随机数。
				ran = t_htonl(ran);
				seq = t_htonl(seq);
				uint8_t id_len = strlen(DEVICE_ID);
				uint8_t* data = ke_malloc(id_len+8, KE_MEM_NON_RETENTION);
				if(!data){arch_printf("\r\nNot enough memory!");return;}
				memcpy(data,deviceid,id_len);
				memcpy(data+id_len,(uint8_t*)&ran,4);
				memcpy(data+id_len+4,(uint8_t*)&seq,4);
				uint32_t crc = crc32(0, data, id_len+8);
				crc = t_htonl(crc);
				memset(data,0x00,id_len+8);
				memcpy(data,(uint8_t*)&ran,4);
				memcpy(data+4,(uint8_t*)&seq,4);
				memcpy(data+8,(uint8_t*)&crc,4);	
				uint8_t CipherText[16];
				AES_Init(key);
				AES_Encrypt_PKCS7 (data, CipherText, 12, key);
				if(data){ke_free(data);data = NULL;}
				AuthRequest authReq = {&basReq, true,{md5_type_and_id, MD5_TYPE_AND_ID_LENGTH}, 
                PROTO_VERSION, AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, true ,
                {CipherText, CIPHER_TEXT_LENGTH}, false, {NULL, 0}, false, {NULL, 0}, false, 
                {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
				seq++;
			#endif
				
			#if defined EAM_macNoEncrypt
				static uint8_t mac_address[MAC_ADDRESS_LENGTH];
				ihu_get_mac_addr(mac_address);
				AuthRequest authReq = {&basReq, false,{NULL, 0}, PROTO_VERSION, AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, false,{NULL, 0}, true, {mac_address, MAC_ADDRESS_LENGTH}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
			#endif
				
			#if defined EAM_md5AndNoEnrypt
				AuthRequest authReq = {&basReq, true,{md5_type_and_id, MD5_TYPE_AND_ID_LENGTH}, PROTO_VERSION, (EmAuthMethod)AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, false ,{NULL, 0}, false, {NULL, 0}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
			#endif
				*r_len = epb_auth_request_pack_size(&authReq) + fix_head_len;
				*r_data = (uint8_t *)ke_malloc(*r_len, KE_MEM_NON_RETENTION);
				if(!(*r_data)){arch_printf("\r\nNot enough memory!");return;}
				if(epb_pack_auth_request(&authReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{
					*r_data = NULL;
					return;
				}
				fix_head.nCmdId = htons(ECI_req_auth);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(mpbledemo2Sta.seq);
				memcpy(*r_data, &fix_head, fix_head_len);                
				return ;
		}
		case CMD_INIT:
			{
				//has challeange
				InitRequest initReq = {&basReq,false, {NULL, 0},true, {challeange, CHALLENAGE_LENGTH}};
				*r_len = epb_init_request_pack_size(&initReq) + fix_head_len;
			#if defined EAM_md5AndAesEnrypt
				uint8_t length = *r_len;				
				uint8_t *p = ke_malloc(AES_get_length( *r_len-fix_head_len), KE_MEM_NON_RETENTION);
				if(!p){arch_printf("\r\nNot enough memory!");return;}
				*r_len = AES_get_length( *r_len-fix_head_len)+fix_head_len;
			#endif
			//pack data
				*r_data = (uint8_t *)ke_malloc(*r_len, KE_MEM_NON_RETENTION);
				if(!(*r_data)){arch_printf("\r\nNot enough memory!");return;}
				if(epb_pack_init_request(&initReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{*r_data = NULL;return;}
				//encrypt body
			#if defined EAM_md5AndAesEnrypt
				AES_Init(session_key);
				AES_Encrypt_PKCS7(*r_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度
				memcpy(*r_data + fix_head_len, p, *r_len-fix_head_len);
				if(p)ke_free(p);
			#endif
				fix_head.nCmdId = htons(ECI_req_init);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(mpbledemo2Sta.seq);
				memcpy(*r_data, &fix_head, fix_head_len);
				return  ;
		}
		case CMD_SENDDAT_TIME_SYNC:
		case CMD_SENDDAT_EQU_INFO:
		case CMD_SENDDAT_EMC_REPORT:
		case CMD_SENDDAT:
			{
				#ifdef CATCH_LOG
					arch_printf("\r\n msg to send : %s",(uint8_t*)info->send_msg.str);
				#endif
				BlueDemoHead  *bleDemoHead = (BlueDemoHead*)ke_malloc(bleDemoHeadLen+info->send_msg.len, KE_MEM_NON_RETENTION);
				if (!bleDemoHead)
				{
					arch_printf("\r\nNo enough memory!");
					return;
				}
        // header of sent data
				bleDemoHead->m_magicCode[0] = MPBLEDEMO2_MAGICCODE_H;
				bleDemoHead->m_magicCode[1] = MPBLEDEMO2_MAGICCODE_L;
				bleDemoHead->m_version      = htons(MPBLEDEMO2_VERSION);
				bleDemoHead->m_totalLength  = htons(bleDemoHeadLen + info->send_msg.len);
				if (info->cmd == CMD_SENDDAT_TIME_SYNC) bleDemoHead->m_cmdid = htons(readTimeSyncReq);
				else if (info->cmd == CMD_SENDDAT_EQU_INFO) bleDemoHead->m_cmdid = htons(readEquInfoReq);
				else if (info->cmd == CMD_SENDDAT_EMC_REPORT) bleDemoHead->m_cmdid = htons(readEmcDataResp);
				else	bleDemoHead->m_cmdid = htons(sendTextReq);
				bleDemoHead->m_seq          = htons(mpbledemo2Sta.seq);
				bleDemoHead->m_errorCode    = 0;	
                
				/*connect body and head.*/
				/*turn to uint8_t* befort offset.*/
				memcpy((uint8_t*)bleDemoHead + bleDemoHeadLen, info->send_msg.str, info->send_msg.len);		
                
				SendDataRequest sendDatReq = 
				{
					&basReq, 
					{ (uint8_t*) bleDemoHead, (bleDemoHeadLen + info->send_msg.len)},  // define the data content wrapped in epb
					false,      // no type, the data is directly sent to vendor server
					(EmDeviceDataType)NULL
				};
				*r_len = epb_send_data_request_pack_size(&sendDatReq) + fix_head_len;
                    
				#if defined EAM_md5AndAesEnrypt
					uint16_t length = *r_len;
					uint8_t *p = ke_malloc(AES_get_length( *r_len-fix_head_len), KE_MEM_NON_RETENTION);
					if(!p){arch_printf("\r\nNot enough memory!");return;}
					*r_len = AES_get_length( *r_len-fix_head_len)+fix_head_len;
				#endif
                
				*r_data = (uint8_t *)ke_malloc(*r_len, KE_MEM_NON_RETENTION);
				if(!(*r_data)){arch_printf("\r\nNot enough memory!");return;}
				if(epb_pack_send_data_request(&sendDatReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{
					*r_data = NULL;
					#if defined EAM_md5AndAesEnrypt
						if(p){ke_free(p);
						p = NULL;}
					#endif
					arch_printf("\r\nepb_pack_send_data_request error!");
					return;
				}
                
				#if defined EAM_md5AndAesEnrypt
					//encrypt body
					AES_Init(session_key);
					AES_Encrypt_PKCS7(*r_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度
					memcpy(*r_data + fix_head_len, p, *r_len-fix_head_len);
					if(p){ke_free(p); p = NULL;}
				#endif
                
        // header of epb
				fix_head.nCmdId = htons(ECI_req_sendData);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(mpbledemo2Sta.seq);
                
				memcpy(*r_data, &fix_head, fix_head_len);
				if(bleDemoHead){ke_free(bleDemoHead);bleDemoHead = NULL;}
                
				#ifdef CATCH_LOG
					arch_printf("\r\n##send data: ");
					uint8_t *d = *r_data;
					for(uint8_t i=0;i<*r_len;++i){
					arch_printf(" %x",d[i]);}
					BpFixHead *fix_head = (BpFixHead *)*r_data;
					arch_printf("\r\n CMDID: %d",ntohs(fix_head->nCmdId));
					arch_printf("\r\n len: %d", ntohs(fix_head->nLength ));
					arch_printf("\r\n Seq: %d", ntohs(fix_head->nSeq));
				#endif
                
				// increase sequence by 1
        mpbledemo2Sta.send_data_seq++;				
				return ;
		}
	}	
}

//从后台发送命令到DEVICE
int mpbledemo2_data_consume_func(uint8_t *data, uint32_t len)
{
		BpFixHead *fix_head = (BpFixHead *)data;
		uint8_t fix_head_len = sizeof(BpFixHead);
		#ifdef CATCH_LOG
			arch_printf("\r\n##Received data: ");
			uint8_t *d = data;
			for(uint8_t i=0;i<len;++i){
			arch_printf(" %x",d[i]);}
			arch_printf("\r\n CMDID: %d", ntohs(fix_head->nCmdId));
			arch_printf("\r\n len: %d", ntohs(fix_head->nLength));
			arch_printf("\r\n Seq: %d",ntohs(fix_head->nSeq));
		#endif
			
		switch(ntohs(fix_head->nCmdId))
		{
			case ECI_none:
				{
				}
				break;
				
			case ECI_resp_auth:
				{
					AuthResponse* authResp;
					authResp = epb_unpack_auth_response(data+fix_head_len,len-fix_head_len);
					#ifdef CATCH_LOG
						arch_printf("\r\n@@Received 'authResp'\r\n");
					#endif
						if(!authResp){return errorCodeUnpackAuthResp;}
					#ifdef CATCH_LOG
						arch_printf("\r\n unpack 'authResp' success!\r\n");
					#endif
					if(authResp->base_response)
					{
						if(authResp->base_response->err_code == 0)
						{
							mpbledemo2Sta.auth_state = true;

						}
						else
						{
						#ifdef CATCH_LOG
							arch_printf("\r\n error code:%d",authResp->base_response->err_code);
						#endif
							#ifdef CATCH_LOG
							if(authResp->base_response->has_err_msg)
							{
								arch_printf("\r\n base_response error msg:%s",authResp->base_response->err_msg.str);	
							}
							#endif
                            int32_t returnedErrCode = authResp->base_response->err_code;
							epb_unpack_auth_response_free(authResp);
							return returnedErrCode;
						}
					}
					#if defined EAM_md5AndAesEnrypt// get sessionkey
						if(authResp->aes_session_key.len)
						{
						#ifdef CATCH_LOG
							arch_printf("\r\nsession_key:");
						#endif
							AES_Init(key);
							AES_Decrypt(session_key,authResp->aes_session_key.data,authResp->aes_session_key.len,key);
							#ifdef CATCH_LOG
							for(uint8_t i = 0;i<16;i++)
							{
								arch_printf(" 0x%02x",session_key[i]);	
							}
							#endif
						}
					#endif
					epb_unpack_auth_response_free(authResp);
				}
				break;
				
			case ECI_resp_sendData:
				{
					#ifdef CATCH_LOG
						arch_printf("\r\n@@Received 'sendDataResp'\r\n");
					#endif
					#if defined EAM_md5AndAesEnrypt		
						uint32_t length = len- fix_head_len;//加密后数据长度
						uint8_t *p = ke_malloc (length, KE_MEM_NON_RETENTION);
						if(!p){arch_printf("\r\nNot enough memory!"); if(data)ke_free(data);data = NULL; return 0;}
						AES_Init(session_key);
						//解密数据
						AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
						
						uint8_t temp;
						temp = p[length - 1];//算出填充长度
						len = len - temp;//取加密前数据总长度
						memcpy(data + fix_head_len, p ,length -temp);//把明文放回
						if(p){ke_free(p);p = NULL;}
					#endif	
					SendDataResponse *sendDataResp;
					sendDataResp = epb_unpack_send_data_response(data+fix_head_len,len-fix_head_len);
					if (!sendDataResp)
					{
						return errorCodeUnpackSendDataResp;
					}
					#ifdef CATCH_LOG
						BlueDemoHead *bledemohead = (BlueDemoHead*)sendDataResp->data.data;
						if ((sendDataResp->data.len < 12) ||(bledemohead->m_magicCode[0] != MPBLEDEMO2_MAGICCODE_H) || (bledemohead->m_magicCode[1] != MPBLEDEMO2_MAGICCODE_H))
						{
							arch_printf("\r\n received msg: no message content! \r\n");
						}									
						else if(ntohs(bledemohead->m_cmdid) == sendTextResp)
						{
							arch_printf("\r\n received msg: %s\r\n",sendDataResp->data.data+sizeof(BlueDemoHead));
						}
					#endif
					if(sendDataResp->base_response->err_code)
					{
						epb_unpack_send_data_response_free(sendDataResp);
						return sendDataResp->base_response->err_code;
					}
					epb_unpack_send_data_response_free(sendDataResp);
				}
				break;
			
			case ECI_resp_init:
				{
					FirstNotificationBit = 0;
					SecondNotificationBit = 0; 
					#ifdef CATCH_LOG
						arch_printf("\r\n@@Received 'initResp'\r\n");
					#endif
					#if defined EAM_md5AndAesEnrypt		
						uint32_t length = len- fix_head_len;//加密后数据长度
						uint8_t *p = ke_malloc (length, KE_MEM_NON_RETENTION);
						if(!p){arch_printf("\r\nNot enough memory!");if(data)ke_free(data);data = NULL; return 0;}
						AES_Init(session_key);
						//解密数据
						AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
						
						uint8_t temp;
						temp = p[length - 1];//算出填充长度
						len = len - temp;//取加密前数据总长度
						memcpy(data + fix_head_len, p ,length -temp);//把明文放回
						if(p){ke_free(p);p = NULL;}
					#endif		
					InitResponse *initResp = epb_unpack_init_response(data+fix_head_len, len-fix_head_len);
					if(!initResp)
					{
						return errorCodeUnpackInitResp;
					}
					#ifdef CATCH_LOG
						arch_printf("\r\n unpack 'initResp' success!");
					#endif	
					if(initResp->base_response)
					{
						if(initResp->base_response->err_code == 0)
						{
							if(initResp->has_challeange_answer)
							{
								if(crc32(0,challeange,CHALLENAGE_LENGTH) == initResp->challeange_answer)
								{
									mpbledemo2Sta.init_state = true;
								}
							}
							else mpbledemo2Sta.init_state = true;
							mpbledemo2Sta.wechats_switch_state = true;
						}
						else
						{
						#ifdef CATCH_LOG
							arch_printf("\r\n error code:%d",initResp->base_response->err_code);
						#endif	
							if(initResp->base_response->has_err_msg)
							{
							#ifdef CATCH_LOG
								arch_printf("\r\n base_response error msg:%s",initResp->base_response->err_msg.str);
							#endif	
							}
							epb_unpack_init_response_free(initResp);
							return initResp->base_response->err_code;
						}
					}
					epb_unpack_init_response_free(initResp);
					
					//这里可以增加时钟的启动，从而实现链路建立以后，自动上报数据
					vmda1458x_timer_set(WECHAT_PERIOD_REPORT_TIME_OUT, TASK_WECHAT, BLEDEMO2_TIMER_PERIOD_REPORT);
					//闪灯
					vmda1458x_led_blink_once_off_on(LED_ID_6);
				}
				break;
			
			case ECI_push_recvData:
				{
					#if defined EAM_md5AndAesEnrypt
						uint32_t length = len- fix_head_len;//加密后数据长度
						uint8_t *p = ke_malloc (length, KE_MEM_NON_RETENTION);
						if(!p){arch_printf("\r\nNot enough memory!");if(data)ke_free(data); data =NULL; return 0;}
						AES_Init(session_key);
						//解密数据
						AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
						
						uint8_t temp;
						temp = p[length - 1];//算出填充长度
						len = len - temp;//取加密前数据总长度
						memcpy(data + fix_head_len, p ,length -temp);//把明文放回
						if(p){ke_free(p);p = NULL;}
					#endif
					RecvDataPush *recvDatPush;
					recvDatPush = epb_unpack_recv_data_push(data+fix_head_len, len-fix_head_len);
					#ifdef CATCH_LOG
						arch_printf("\r\n@@Received 'recvDataPush'\r\n");
					#endif
					if(!recvDatPush)
					{
						return errorCodeUnpackRecvDataPush;
					}
					#ifdef CATCH_LOG
						arch_printf("\r\n unpack the 'recvDataPush' successfully! \r\n");
						if(recvDatPush->base_push == NULL)
						{
							arch_printf("\r\n recvDatPush->base_push is NULL! \r\n");
						}
						else 
						{
							arch_printf("\r\n recvDatPush->base_push is not NULL! \r\n");
						}
						arch_printf("\r\n recvDatPush->data.len: %x \r\n",recvDatPush->data.len);
						arch_printf("\r\n recvDatPush->data.data:  \r\n");
						const uint8_t *d = recvDatPush->data.data;
						for(uint8_t i=0;i<recvDatPush->data.len;++i){
						arch_printf(" %x",d[i]);}
						if(recvDatPush->has_type)
						{
							arch_printf("\r\n recvDatPush has type! \r\n");
							arch_printf("\r\n type: %d\r\n",recvDatPush->type);
						}
					#endif	
					BlueDemoHead *bledemohead = (BlueDemoHead*)recvDatPush->data.data;
					#ifdef CATCH_LOG
						arch_printf("\r\n magicCode: %x",bledemohead->m_magicCode[0]);
						arch_printf(" %x",bledemohead->m_magicCode[1]);
						arch_printf("\r\n version: %x",ntohs(bledemohead->m_version));
						arch_printf("\r\n totalLength: %x",ntohs(bledemohead->m_totalLength));
						arch_printf("\r\n cmdid: %x",ntohs(bledemohead->m_cmdid ));
						arch_printf("\r\n errorCode: %x",ntohs(bledemohead->m_errorCode));
					#endif	 
					// ble demo command handler
					mpbledemo2_handleCmdFromServer((BleDemo2CmdID)ntohs(bledemohead->m_cmdid), ((uint8_t *)recvDatPush->data.data) + \
										sizeof(BlueDemoHead), ntohs(bledemohead->m_totalLength) - sizeof(BlueDemoHead));  
					epb_unpack_recv_data_push_free(recvDatPush);
					mpbledemo2Sta.push_data_seq++;
				}
				break;
				
			case ECI_push_switchView:
				{
					mpbledemo2Sta.wechats_switch_state = !mpbledemo2Sta.wechats_switch_state;
					#ifdef CATCH_LOG
						arch_printf("\r\n@@Received 'switchViewPush'\r\n");
					#endif
					#if defined EAM_md5AndAesEnrypt		
						uint32_t length = len- fix_head_len;//加密后数据长度
						uint8_t *p = ke_malloc (length, KE_MEM_NON_RETENTION);
						if(!p){arch_printf("\r\nNot enough memory!");if(data)ke_free(data);data = NULL; return 0;}
						AES_Init(session_key);
						//解密数据
						AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
						
						uint8_t temp;
						temp = p[length - 1];//算出填充长度
						len = len - temp;//取加密前数据总长度
						memcpy(data + fix_head_len, p ,length -temp);//把明文放回
						if(p){ke_free(p);p = NULL;}
					#endif		
					SwitchViewPush *swichViewPush;
					swichViewPush = epb_unpack_switch_view_push(data+fix_head_len,len-fix_head_len);
					if(!swichViewPush)
					{
						return errorCodeUnpackSwitchViewPush;
					}
					epb_unpack_switch_view_push_free(swichViewPush);
				}
				break;
			
			case ECI_push_switchBackgroud:
				{
					#ifdef CATCH_LOG
						arch_printf("\r\n@@Received 'switchBackgroudPush'\r\n");
					#endif
					#if defined EAM_md5AndAesEnrypt
						uint32_t length = len- fix_head_len;//加密后数据长度
						uint8_t *p = ke_malloc (length, KE_MEM_NON_RETENTION);
						if(!p){arch_printf("\r\nNot enough memory!");if(data)ke_free(data);data = NULL;  return 0;}
						AES_Init(session_key);
						//解密数据
						AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
						uint8_t temp;
						temp = p[length - 1];//算出填充长度
						len = len - temp;//取加密前数据总长度
						memcpy(data + fix_head_len, p ,length -temp);//把明文放回
						if(data){ke_free(p);p = NULL;}
					#endif
					SwitchBackgroudPush *switchBackgroundPush = epb_unpack_switch_backgroud_push(data+fix_head_len,len-fix_head_len);
					if(! switchBackgroundPush)
					{
						return errorCodeUnpackSwitchBackgroundPush;
					}	
					epb_unpack_switch_backgroud_push_free(switchBackgroundPush);
				}
				break;
				
			case ECI_err_decode:
				//清除TIMEOUT消息，是否应该放在这里，也是一个小小的问题，原则上需要放在断链的过程中，但并没有断链过程
				//待完善
				vmda1458x_timer_clear(WECHAT_PERIOD_REPORT_TIME_OUT, TASK_WECHAT);
				break;
			
			default:
				{
					#ifdef CATCH_LOG
						arch_printf("\r\n !!ERROR CMDID:%d",ntohs(fix_head->nCmdId));
					#endif
				}
				break;
				
		}//End of switch(ntohs(fix_head->nCmdId))
		return 0;
}

void mpbledemo2_data_error_func(int error_code)
{
	if(error_code)
	{
		#ifdef CATCH_LOG
			arch_printf("\r\n! error: mpbledemo2 reseted");
		#endif
        
		//NVIC_SystemReset();
	}
}
data_handler mpbledemo2_data_handler = {
		.m_product_type                 = PRODUCT_TYPE_MPBLEDEMO2,
		.m_data_produce_func            = &mpbledemo2_data_produce_func,
		.m_data_free_func               = &mpbledemo2_data_free_func,
		.m_data_consume_func            = &mpbledemo2_data_consume_func,
		.m_data_error_func              = &mpbledemo2_data_error_func,
    .m_data_init_peripheral_func    = &mpbledemo2_init_peripheral_func,
		.m_data_init_func               = &mpbledemo2_init,
		.m_data_main_process_func       = &mpbledemo2_main_process,
		.m_data_button_handler_func     = &mpbledemo2_button_handler,
		.m_data_produce_args            = &m_info,
		.next                           = NULL
};

//send data to wechat server
//app_wechat_task.c中的主要应用函数，用于处理{WECHAT_SEND_DATA_TO_MASTER, (ke_msg_func_t)app_wechat_send_data_handler}
int32_t mpbledemo2_sendData(uint8_t* ptrData, uint32_t lengthInByte)
{
  uint8_t *data = NULL;
	uint32_t len = 0;
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_SENDDAT);   
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, send_msg.len, lengthInByte);
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, send_msg.str, (const char *)ptrData);    
    
	m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
	if(data == NULL)
	{
		return errorCodeProduce;
	}
	ble_wechat_indicate_data(data, len);
    return 0;
}

void mpbledemo2_handleSendDataRsp(uint8_t *ptrData, uint32_t lengthInByte)
{
    // TODO: add your own handling to process sending data response from verdor server
}

void mpbledemo2_OpenLight(uint8_t *ptrData, uint32_t lengthInByte)
{
	#ifdef CATCH_LOG
		arch_printf("\r\n light on!! ");
	#endif
	GPIO_SetActive(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN);
	isLightOn = true;
}

void mpbledemo2_CloseLight(uint8_t *ptrData, uint32_t lengthInByte)
{
	#ifdef CATCH_LOG
		arch_printf("\r\n light off!! ");
	#endif    
	GPIO_SetInactive(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN);
	isLightOn = false;
}

void mpbledemo2_readEmcDataResp(uint8_t *ptrData, uint32_t lengthInByte)
{
//	struct wechat_send_data_req * req = KE_MSG_ALLOC(WECHAT_SEND_DATA_TO_MASTER, TASK_APP,
//																		TASK_APP, wechat_send_data_req);
//	uint16_t emcData = 0;
//	emcData = ihu_emc_adc_read();
//	uint8_t emc[2];
//	emc[0] = (emcData>>8) & 0xFF; emc[1] = emcData & 0xFF;
//	req->dataLen = sizeof(emc);
//	req->pDataBuf = emc;
//	ke_msg_send(req);
//	FirstNotificationBit = 1;
//	SecondNotificationBit = 1;

	//循环读取，为了找到合适的瞬时最大值
	UINT16 emcData = 0, tmp = 0;
	UINT32 index = 0, loopReadMax = 100;
	for (index = 0; index < loopReadMax; index++){
		tmp = ihu_emc_adc_read();
		if (tmp > emcData) emcData = tmp;
	}
	uint8_t emc[3];
	//垃圾代码，没有编解码，完全是为了方便后台云代码的解码，从而对齐格式。未来需要完善的自定义数据结构。
	emc[0] = 0x20;
	emc[1] = (emcData>>8) & 0xFF; 
	emc[2] = emcData & 0xFF;
	
  uint8_t *data = NULL;
	data=emc;
	uint32_t len = sizeof(emc);
	
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_SENDDAT_EMC_REPORT);   
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, send_msg.len, len);
	ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, send_msg.str, (const char *)data);    
    
	m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
	
	if(data == NULL)
	{
		return;
	}
	ble_wechat_indicate_data(data, len);
	//闪灯
	vmda1458x_led_blink_once_off_on(LED_ID_7);
	//vmda1458x_led_set(LED_ID_6, LED_MODE_ON);
	vmda1458x_led_set(LED_ID_6, LED_MODE_ON);
	vmda1458x_led_set(LED_ID_7, LED_MODE_ON);
	arch_printf("\r\n light on myc!! ");
  return;
	
	//TMD，估计只能手工编码，不然就给屁朝凉了。。。
}

void mpbledemo2_readTimeSyncReq(uint8_t *ptrData, uint32_t lengthInByte)
{
}
void mpbledemo2_readEquInfoReq(uint8_t *ptrData, uint32_t lengthInByte)
{
}

MPBLEDEMO2_RECEIVED_CMD_HANDLER_T mpbledemo2_cmd_handler[] = 
{
    {sendTextResp,      mpbledemo2_handleSendDataRsp},
    {openLightPush,     mpbledemo2_OpenLight},
    {closeLightPush,    mpbledemo2_CloseLight},
    {readEmcInsPush,    mpbledemo2_readEmcDataResp},
    {readTimeSyncResp,  mpbledemo2_readTimeSyncReq},
    {readEquInfoPush,   mpbledemo2_readEquInfoReq},
		
    // TODO: add customized command handler here
};

void mpbledemo2_handleCmdFromServer(BleDemo2CmdID cmd, uint8_t *ptrData, uint32_t lengthInByte)
{
    MPBLEDEMO2_RECEIVED_CMD_HANDLER_T* pHandlerList = mpbledemo2_cmd_handler;
  
    uint32_t handlerIndex;
    
    for (handlerIndex = 0; handlerIndex < sizeof(mpbledemo2_cmd_handler)/sizeof(MPBLEDEMO2_RECEIVED_CMD_HANDLER_T);
        handlerIndex++)
    {
        if (pHandlerList->commandCode == cmd)
        {
            pHandlerList->handler(ptrData, lengthInByte);
            break;
        }
        
        pHandlerList++;
    }
}

void mpbledemo2_airsync_link_setup_period_report(void)
{
	//先判断链路状态，如果处于正常状态，则干下面的活
	//发送报告数据
	mpbledemo2_readEmcDataResp(NULL, 0);
	//重新启动定时器
	vmda1458x_timer_set(WECHAT_PERIOD_REPORT_TIME_OUT, TASK_WECHAT, BLEDEMO2_TIMER_PERIOD_REPORT);
}
