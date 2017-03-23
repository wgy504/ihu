/**
 ****************************************************************************************
 *
 * @file mod_ble.c
 *
 * @brief BLE module control
 *
 * BXXH team
 * Created by ZJL, 20161027
 *
 ****************************************************************************************
 */
 
#include "vmmw_ble.h"

//全局变量，引用外部
extern int8_t zIhuBspStm32SpsBleRxBuff[IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN];			//串口BLE数据接收缓冲区 
extern int16_t zIhuBspStm32SpsBleRxCount;																				//当前接收数据的字节数 	  

//本驱动程序中，HC05工作在从模式，等待APP/手机做为主模式来连接

BLTDev bltDevList;   //蓝牙设备列表，在main文件中定义
#define DEFAULT_HC05_ROLE           0 // 默认从模式  
//#define DEFAULT_HC05_ROLE           1 // 默认主模式
BLTDev bltDevList;
char zIhuBlemodSendData[1024];
char zIhuBlemodLineBuff[1024];
static __IO uint32_t TimingDelay=0;


/*******************************************************************************
* 函数名 : func_blemod_uart_send_AT_command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 1:正常  -1:错误
* 注意   : 
* OPSTAT func_blemod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, uint8_t wait_time)  
* 这里的发送，只有成功返回ACK对应的回复时，才算成功
*******************************************************************************/
OPSTAT func_blemod_uart_send_AT_command(uint8_t *cmd, uint8_t *ack, uint16_t wait_time) //in Second
{
	int res;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
	func_blemod_uart_clear_receive_buffer();
	ihu_l1hd_sps_ble_send_data((uint8_t *)cmd, strlen((char*)cmd));
	func_blemod_uart_send_LR();	
	
	res = IHU_FAILURE;
	while((tickTotal > 0) && (res == IHU_FAILURE))
	{
		ihu_usleep(IHU_BSP_STM32_SPS_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
		tickTotal--;
		if(strstr((const char*)zIhuBspStm32SpsBleRxBuff, (char*)ack)==NULL)
			 res = IHU_FAILURE;
		else
			 res = IHU_SUCCESS;
	}
	return res;
}

//in Second
OPSTAT func_blemod_wait_AT_command_fb(uint8_t *ack, uint16_t wait_time)
{
	int ret = 0;
	
	//等待的时间长度，到底是以tick为单位的，还是以ms为单位的？经过验证，都是以ms为单位的，所以不用担心！！！
	uint32_t tickTotal = wait_time * 1000 / IHU_BSP_STM32_SPS_RX_MAX_DELAY;

	//清理接收缓冲区
	ret = IHU_FAILURE;

	while((tickTotal > 0) && (ret == IHU_FAILURE))
	{
		ihu_usleep(IHU_BSP_STM32_SPS_RX_MAX_DELAY); //这里的周期就是以绝对ms为单位的
		tickTotal--;
		if(strstr((const char*)zIhuBspStm32SpsBleRxBuff, (char*)ack)==NULL)
			 ret = IHU_FAILURE;
		else
			 ret = IHU_SUCCESS;
	}
	return ret;
}


/*******************************************************************************
* 函数名 : func_blemod_uart_clear_receive_buffer
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void func_blemod_uart_clear_receive_buffer(void)
{
	uint16_t k;
	for(k=0;k<IHU_BSP_STM32_SPS_BLE_REC_MAX_LEN;k++)      //将缓存内容清零
	{
		zIhuBspStm32SpsBleRxBuff[k] = 0x00;
	}
  zIhuBspStm32SpsBleRxCount = 0;               //接收字符串的起始存储位置
}

/*******************************************************************************
* 函数名  : USART_BLE_SendString
* 描述    : USART_BLE发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void func_blemod_uart_send_string(char* s)
{
	while(*s != '\0')//检测字符串结束符
	{
		ihu_l1hd_sps_ble_send_data((uint8_t *)s++, 1);
	}
}

/*******************************************************************************
* 函数名 : Find
* 描述   : 判断缓存中是否含有指定的字符串
* 输入   : 
* 输出   : 
* 返回   : unsigned char:1 找到指定字符，0 未找到指定字符 
* 注意   : 
*******************************************************************************/
OPSTAT func_blemod_find_char(char *a)
{ 
  if(strstr((char *)zIhuBspStm32SpsBleRxBuff, a)!=NULL)
	    return IHU_SUCCESS;
	else
		return IHU_FAILURE;
}

/**
  * 函数功能: 获取接收到的数据和长度 
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
char *ihu_bsp_stm32_ble_get_rebuff(uint16_t *len) 
{
    *len = zIhuBspStm32SpsBleRxCount;
    return (char *)zIhuBspStm32SpsBleRxBuff;
}

/*******************************************************************************
* 函数名 : BLE_UART_fetch_mac_add_procedure
* 描述   : 获取BLE MAC地址测试代码
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 通过参数传递指针进来，从而获得MAC地址信息，待完善
*******************************************************************************/
  /* Address should look like "+ADDR:<NAP>:<UAP>:<LAP>",
   * where actual address will look like "1234:56:abcdef".
   */
OPSTAT ihu_vmmw_blemod_hc05_uart_fetch_mac_addr_in_AT_cmd_mode(uint8_t *macAddr, uint8_t length)
{
	uint8_t repeatCnt = IHU_VMMW_BLEMOD_UART_REPEAT_CNT;
	uint8_t *p1, *p2;

	//固定在AT模式+从模式，让外部的手机可以连接到该模块
  //uint8_t hc05_mode=0;  // 0：SPP规范      1：AT模式
  //uint8_t hc05_role=DEFAULT_HC05_ROLE;  // 0：从模式       1：主模式
  //uint8_t hc05_connect=0; // 0:未连接       1：连接成功，可以进行数据传输
  //char hc05_mode_str[10]="SLAVE";
	//char hc05_name[30]="HC05_SLAVE";
  char hc05_nameCMD[40];
		
	//参数检查
	if (macAddr == NULL) IHU_ERROR_PRINT_BLEMOD("VMMWBLE: invalid input paramter received!\n");
	
	//初始化BLE模块
  ihu_l1hd_dido_f2board_ble_atcmd_mode_ctrl_off();
	func_blemod_uart_hc05_init();
	func_blemod_uart_clear_receive_buffer();
	ihu_usleep(500);
	
	//先进入AT命令模式
	ihu_l1hd_dido_f2board_ble_atcmd_mode_ctrl_on();
	
	//最大循环次数等待AT命令反馈
	repeatCnt = IHU_VMMW_BLEMOD_UART_REPEAT_CNT;
	while((repeatCnt > 0) && (func_blemod_uart_send_AT_command((uint8_t*)"AT", (uint8_t*)"OK", 2) != IHU_SUCCESS))//查询是否应到AT指令
	{
		repeatCnt--;
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: Not detect BLE module, trying to reconnecting!\n");
		}
		ihu_usleep(200);
		if (repeatCnt == 0) IHU_ERROR_PRINT_BLEMOD("VMMWBLE: BLE sensor (HC05) in slave/broadcasting mode, not detected!\n");
	}

	
	//查阅版本
	func_blemod_uart_clear_receive_buffer();
	if (func_blemod_uart_send_AT_command((uint8_t*)"AT+VERSION?", (uint8_t*)"OK", 2) == IHU_SUCCESS){
		if(strstr((const char*)zIhuBspStm32SpsBleRxBuff, "+VERSION:") != NULL){
			IHU_DEBUG_PRINT_INF("VMMWBLE: BLE Version = [%s]!\n", zIhuBspStm32SpsBleRxBuff);
		}
	}else{
		IHU_ERROR_PRINT_BLEMOD("VMMWBLE: BLE inquery version failure!\n");
	}
	
	//复位、恢复默认状态
	if (func_blemod_uart_send_AT_command((uint8_t*)"AT+ORGL", (uint8_t*)"OK", 2) == IHU_FAILURE)
		IHU_ERROR_PRINT_BLEMOD("VMMWBLE: BLE ORGL failure!\n");
	if (func_blemod_uart_send_AT_command((uint8_t*)"AT+RESET", (uint8_t*)"OK", 2) == IHU_FAILURE)
		IHU_ERROR_PRINT_BLEMOD("VMMWBLE: BLE RESET failure!\n");

	//再次进入
	repeatCnt = IHU_VMMW_BLEMOD_UART_REPEAT_CNT;
	while((repeatCnt > 0) && (func_blemod_uart_send_AT_command((uint8_t*)"AT", (uint8_t*)"OK", 2) != IHU_SUCCESS))//查询是否应到AT指令
	{
		repeatCnt--;
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: Not detect BLE module, trying to reconnecting!\n");
		}
		ihu_usleep(200);
		if (repeatCnt == 0) IHU_ERROR_PRINT_BLEMOD("VMMWBLE: BLE detect failure!\n");
	}
	
	//AT命令模式+从模式：发送广播，等待着手机主动连接
	if(func_blemod_uart_send_AT_command((uint8_t*)"AT+ROLE=0", (uint8_t*)"OK", 2) == IHU_SUCCESS)	
	{
		ihu_usleep(100);
		memset(hc05_nameCMD, 0, sizeof(hc05_nameCMD));
		srand(ihu_l1hd_rtc_get_current_unix_time());
		sprintf(hc05_nameCMD, "AT+NAME=YCBLE_SLAVE_%d", (uint8_t)(rand()%256));
		
		if(func_blemod_uart_send_AT_command((uint8_t*)hc05_nameCMD, (uint8_t*)"OK", 2) == IHU_SUCCESS){
			IHU_DEBUG_PRINT_INF("VMFO：Euqipment has been changed to be [%s]!\n", hc05_nameCMD);
		}
		else{
			IHU_ERROR_PRINT_BLEMOD("VMMWBLE: Change name error!\n");
		}
	}else{
		IHU_ERROR_PRINT_BLEMOD("VMMWBLE: Set role error!\n");
	}
	
	//先初始化设备SFP
	if (func_blemod_uart_send_AT_command((uint8_t*)"AT+INIT", (uint8_t*)"OK", 2) != IHU_SUCCESS) IHU_ERROR_PRINT_BLEMOD("VMMWBLE: Init error!\n");
	if (func_blemod_uart_send_AT_command((uint8_t*)"AT+CLASS=0", (uint8_t*)"OK", 2) != IHU_SUCCESS) IHU_ERROR_PRINT_BLEMOD("VMMWBLE: Set Class error!\n");
	if (func_blemod_uart_send_AT_command((uint8_t*)"AT+INQM=1,9,48", (uint8_t*)"OK", 2) != IHU_SUCCESS) IHU_ERROR_PRINT_BLEMOD("VMMWBLE: Set INQM error!\n");
	if (func_blemod_uart_send_AT_command((uint8_t*)"AT+RMAAD", (uint8_t*)"OK", 2) != IHU_SUCCESS) IHU_ERROR_PRINT_BLEMOD("VMMWBLE: Remove RMAAD error!\n");
	
	//最大循环次数，等待接入，并判定状态
	//未来可以在这里打入LED闪灯状态，以及BEEP状态，以便通知外部人员的操作
	//这里的时间长度，未来也许可以设置的长一点，但用户感受会变肉，所以需要根据实际情况进行调整优化
	repeatCnt = 3*IHU_VMMW_BLEMOD_UART_REPEAT_CNT;
	while(repeatCnt > 0)//查询是否应到AT指令
	{
		//先退出AT命令模式，给外部模块机会
		func_blemod_uart_send_AT_command((uint8_t*)"AT+RESET", (uint8_t*)"OK", 2);
		ihu_l1hd_dido_f2board_ble_atcmd_mode_ctrl_off();
		ihu_usleep(2000);

		//进AT命令模式
		ihu_l1hd_dido_f2board_ble_atcmd_mode_ctrl_on();
		//首先查阅状态
		func_blemod_uart_send_AT_command((uint8_t*)"AT+STATE?", (uint8_t*)"OK", 2);
		IHU_DEBUG_PRINT_IPT("VMMWBLE: BLE Matched with mobile phone, status is[%s]\n", (const char*)zIhuBspStm32SpsBleRxBuff);
		//如果是配对成功
		if ((strstr((const char*)zIhuBspStm32SpsBleRxBuff, "PAIRED") != NULL) || (strstr((const char*)zIhuBspStm32SpsBleRxBuff, "PAIRABLE") != NULL)){
			//再查阅是否只有单个客户进来
			func_blemod_uart_send_AT_command((uint8_t*)"AT+ADCN?", (uint8_t*)"OK", 2);
			if (strstr((const char*)zIhuBspStm32SpsBleRxBuff, "+ADCN:1") != NULL){
				//如果是的，再调出该客户的MAC地址
				func_blemod_uart_send_AT_command((uint8_t*)"AT+MRAD?", (uint8_t*)"OK", 2);
				//+MRAD:7423:44:261859
				p1 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsBleRxBuff, "+MRAD:");
				p2 = (uint8_t*)strstr((const char*)p1, "\r");
				p1 = p1 + strlen("+MRAD:");
				if ((p1!=NULL) && (p2!=NULL) && (p1<p2)){
					char s[4];
					uint8_t res = 0;
					//暂时不适用这是方式，因为这个数据有独特的结构
					//memcpy(macAddr, p1, ((p2-p1)<length)?(p2-p1):length);
					//当数据看待，而非字符串看待
					//NAP-1
					memset(s, 0, sizeof(s));
					strncpy(s, (char*)(p1), 2);
					res = strtoul(s, NULL, 16) & 0xFF;
					memcpy(macAddr, &res, 1);
					//NAP-2
					memset(s, 0, sizeof(s));
					strncpy(s, (char*)(p1+2), 2);
					res = strtoul(s, NULL, 16) & 0xFF;
					memcpy(macAddr+1, &res, 1);
					//UAP-1
					memset(s, 0, sizeof(s));
					strncpy(s, (char*)(p1+5), 2);
					res = strtoul(s, NULL, 16) & 0xFF;
					memcpy(macAddr+2, &res, 1);
					//LAP-1
					memset(s, 0, sizeof(s));
					strncpy(s, (char*)(p1+8), 2);
					res = strtoul(s, NULL, 16) & 0xFF;
					memcpy(macAddr+3, &res, 1);
					//LAP-2
					memset(s, 0, sizeof(s));
					strncpy(s, (char*)(p1+10), 2);
					res = strtoul(s, NULL, 16) & 0xFF;
					memcpy(macAddr+4, &res, 1);
					//LAP-3
					memset(s, 0, sizeof(s));
					strncpy(s, (char*)(p1+12), 2);
					res = strtoul(s, NULL, 16) & 0xFF;
					memcpy(macAddr+5, &res, 1);
					
					//读取成功，退出循环
					break;
				}
			}
		}
		repeatCnt--;
		if (repeatCnt == 0) IHU_ERROR_PRINT_BLEMOD("VMMWBLE: BLE read Equipment failure!\n");
	}

	//设置BLE模块进入透传状态
	func_blemod_uart_send_AT_command((uint8_t*)"AT+RESET", (uint8_t*)"OK", 2);
	ihu_l1hd_dido_f2board_ble_atcmd_mode_ctrl_off();

	return IHU_SUCCESS;
}

//获取手机等远程外设的MAC地址，目前由于HC05模块本身的能力问题，只能直接进入串口透传模式，所以采用了独特的APP
//辅助IHU获取远程MAC地址
OPSTAT ihu_vmmw_blemod_hc05_uart_fetch_mac_addr_in_transparant_mode(uint8_t *macAddr, uint8_t len)
{
	int i = 0;
	uint8_t *p1, *p2;

	//参数检查
	if (macAddr == NULL) IHU_ERROR_PRINT_BLEMOD("VMMWBLE: invalid input paramter received!\n");	
	
	//最长5秒，还没搞定，就结束了
	func_blemod_uart_clear_receive_buffer();
	for (i = 0; i < IHU_VMWM_BLEMOD_SCAN_HC05_MAX_TIME; i++){
		func_blemod_uart_send_string("MAC/MIMA=?");
		func_blemod_uart_send_LR();
		if (func_blemod_wait_AT_command_fb((uint8_t*)"MAC/MIMA=", 1) == IHU_SUCCESS) break;
	}
	p1 = (uint8_t*)strstr((const char*)zIhuBspStm32SpsBleRxBuff, "MAC/MIMA=");
	p2 = (uint8_t*)strstr((const char*)p1, "\r");
	p1 = (uint8_t*)strstr((const char*)p1, "="); //实际上从"="算起，包括后面的":"都算作一个3符号组的字符串
	if ((p1!=NULL) && (p2!=NULL) && (p1<p2) && (p2==(p1+len*3) )){
		char s[4];
		uint8_t res = 0;
		//当数据看待，而非字符串看待
		for (i = 0; i<len; i++) {
			s[0] = '\0';
			strncpy(s, (char*)(p1+1+i*3), 2);
			res = strtoul(s, NULL, 16) & 0xFF;
			memcpy(macAddr+i, &res, 1);
		}
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: Received remote MAC address=[%s]\n", p1);
	}else{
		zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
		IhuErrorPrint("VMMWBLE: BLE fetch address failure!\n");
		return IHU_FAILURE;		
	}	
	
	return IHU_SUCCESS;
}



/**
  * 函数功能: 初始化GPIO及检测HC05模块
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void func_blemod_uart_hc05_init(void)
{
	uint8_t i;	
	//HC05_GPIO_Config();
	//HC05_USARTx_Init();	
	for(i=0;i<BLTDEV_MAX_NUM;i++)
	{
		sprintf(bltDevList.unpraseAddr[i]," ");
		sprintf(bltDevList.name[i]," ");
	}	
	bltDevList.num = 0;
}

/**
  * 函数功能: 把接收到的字符串转化成16进制形式的数字变量(主要用于转化蓝牙地址)
  * 输入参数: str：待转换字符串
  * 返 回 值: 无
  * 说    明：无
  */
unsigned long htoul(const char *str)
{
  long result = 0;

  if (!str)
    return 0;

  while (*str)
  {
    uint8_t value;

    if (*str >= 'a' && *str <= 'f')
      value = (*str - 'a') + 10;
    else if (*str >= 'A' && *str <= 'F')
      value = (*str - 'A') + 10;
    else if (*str >= '0' && *str <= '9')
      value = *str - '0';
    else
      break;

    result = (result * 16) + value;
    ++str;
  }
  return result;
}


/**
  * 函数功能: 在str中，跳过它前面的prefix字符串,
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
char *skipPrefix(char *str, size_t str_length, const char *prefix)
{
  uint16_t prefix_length = strlen(prefix);
  if (!str || str_length == 0 || !prefix)
    return 0;
  if (str_length >= prefix_length && strncmp(str, prefix, prefix_length) == 0)
    return str + prefix_length;
  return 0;
}

/**
  * 函数功能: 从stream中获取一行字符串到line中
  * 输入参数: line,存储获得行的字符串数组
  *           stream，原字符串数据流       max_size，stream的大小   
  * 返 回 值: line的长度，若stream中没有‘\0’，'\r'，'\n'，则返回0
  * 说    明：无
  */
int func_blemod_uart_hc05_get_line(char* line, char* stream ,int max_size)  
{  
  char *p;	
  int len = 0;  
  p=stream;
  while( *p != '\0' && len < max_size )
  {  
    line[len++] = *p;  
    p++;
    if('\n' == *p || '\r'==*p)  
        break;  
  }
  if(*p != '\0' && *p != '\n' && *p != '\r')
    return 0;
  line[len] = '\0';  
  return len;  
} 


/**
  * 函数功能: 扫描周边的蓝牙设备，并存储到设备列表中
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
uint8_t func_belmod_uart_hc05_scan_bluetooth_address(BLTDev *bltDev)
{
  /* Address should look like "+ADDR:<NAP>:<UAP>:<LAP>",
   * where actual address will look like "1234:56:abcdef".
   */
	char* redata;
	uint16_t len;
	char zIhuBlemodLineBuff[50];
	uint16_t linelen;
	uint16_t getlen=0;
	uint8_t linenum=0;	
	uint8_t i;
	char *p;

	func_blemod_uart_send_AT_command((uint8_t*)"AT+INQ", (uint8_t*)"OK", 2);
	redata =ihu_bsp_stm32_ble_get_rebuff(&len);
	if(redata[0] != 0 && strstr(redata, "+INQ:") != 0)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: rebuf =%s\n",redata);

getNewLine:
		while(getlen < len-2*linenum )
		{	
			linelen = func_blemod_uart_hc05_get_line(zIhuBlemodLineBuff,redata+getlen+2*linenum,len);
			if(linelen>50 && linelen != 0)
			{
				func_blemod_uart_send_AT_command((uint8_t*)"AT+INQC", (uint8_t*)"OK", 2);//退出前中断查询
				return 1;
			}
			getlen += linelen;
			linenum++;			
			p = skipPrefix(zIhuBlemodLineBuff,linelen,"+INQ:");
			if(p!=0)
			{
				uint8_t num ;
				num = bltDev->num;
				func_blemod_uart_hc05_blt_addr_convert_str(bltDev,':');
				for(i=0;i<=num;i++)
				{
					if(strstr(zIhuBlemodLineBuff,bltDev->unpraseAddr[i]) != NULL)	
					{
						goto getNewLine;	//!=null时，表示该地址与解码语句的地址相同
					}
				}							
				/*若蓝牙设备不在列表中，对地址进行解码*/	
				bltDev->addr[num].NAP = htoul(p);			
				p = strchr(p,':');

				if (p == 0)
				{
					func_blemod_uart_send_AT_command((uint8_t*)"AT+INQC", (uint8_t*)"OK", 2);//退出前中断查询
					return 1;
				}
				bltDev->addr[num].UAP = htoul(++p);
				p = strchr(p,':');
				if (p == 0)
				{
					func_blemod_uart_send_AT_command((uint8_t*)"AT+INQC", (uint8_t*)"OK", 1);//退出前中断查询
					return 1;
				}
				bltDev->addr[num].LAP = htoul(++p);
				/*存储蓝牙地址(字符串形式)*/
				sprintf(bltDev->unpraseAddr[num],"%X:%X:%X",bltDev->addr[num].NAP,bltDev->addr[num].UAP,bltDev->addr[num].LAP);
				bltDev->num++;
			}
		}
		func_blemod_uart_clear_receive_buffer();
		func_blemod_uart_send_AT_command((uint8_t*)"AT+INQC", (uint8_t*)"OK", 2);//退出前中断查询
		return 0;
	}	
	else
	{
		func_blemod_uart_clear_receive_buffer();
		func_blemod_uart_send_AT_command((uint8_t*)"AT+INQC", (uint8_t*)"OK", 2);//退出前中断查询
		return 1;	
	}
}

/**
  * 函数功能: 把蓝牙地址转化成字符串形式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void func_blemod_uart_hc05_blt_addr_convert_str(BLTDev *bltDev, char delimiter)  
{
	uint8_t i;
	
	if(bltDev->num==0)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: /*******No other BLT Device********/\n");
	}
	else
	{
		for(i=0;i<bltDev->num;i++)
		{
			sprintf(bltDev->unpraseAddr[i],"%X%c%X%c%X",bltDev->addr[i].NAP,delimiter,bltDev->addr[i].UAP,delimiter,bltDev->addr[i].LAP);
		}
	}
}

/**
  * 函数功能: 获取远程蓝牙设备的名称
  * 输入参数: bltDev ，蓝牙设备列表指针
  * 返 回 值: 0获取成功，非0不成功
  * 说    明：无
  */
uint8_t func_blemod_uart_hc05_get_remote_device_name(BLTDev *bltDev)
{
	uint8_t i;
	char *redata;
	uint16_t len;
	
	char zIhuBlemodLineBuff[50];
	uint16_t linelen;
	char *p;
	
	char cmdbuff[100];
	
	func_blemod_uart_hc05_blt_addr_convert_str(bltDev,',');

	if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: device num =%d",bltDev->num);
	
	for(i=0;i<bltDev->num;i++)
	{
		sprintf(cmdbuff,"AT+RNAME?%s",bltDev->unpraseAddr[i]);
		func_blemod_uart_send_AT_command((uint8_t*)cmdbuff, (uint8_t*)"OK", 2);
		redata =ihu_bsp_stm32_ble_get_rebuff(&len);
		if(redata[0] != 0 && strstr(redata, "OK") != 0)
		{
			linelen = func_blemod_uart_hc05_get_line(zIhuBlemodLineBuff,redata,len);
			if(linelen>50 && linelen !=0 ) zIhuBlemodLineBuff[linelen]='\0';	//超长截断
					
			p = skipPrefix(zIhuBlemodLineBuff,linelen,"+RNAME:");
			if(p!=0)
			{
				strcpy(bltDev->name[i],p);
			}
		}
		else
		{
			func_blemod_uart_clear_receive_buffer();
			return 1;	
		}
		func_blemod_uart_clear_receive_buffer();
	}
	return 0;
}

/**
  * 函数功能: 输出蓝牙设备列表
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void func_blemod_uart_hc05_print_blt_info(BLTDev *bltDev)  
{
	uint8_t i;
	if(bltDev->num==0)
	{
		if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: /*******No remote BLT Device or in SLAVE mode********/\n");
	}
	else
	{
		//IhuDebugPrint("VMMWBLE: 扫描到 %d 个蓝牙设备\n", bltDev->num);

		for(i=0;i<bltDev->num;i++)
		{
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: /*******Device[%d]********/\n",i);	
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: Device Addr: %s\n",bltDev->unpraseAddr[i]);
			if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: Device name: %s\n",bltDev->name[i]);
		}
	}
}

/**
  * 函数功能: 扫描蓝牙设备，并连接名称中含有"HC05"的设备
  * 输入参数: 无
  * 返 回 值: 0获取成功，非0不成功
  * 说    明：无
  */
uint8_t func_blemod_uart_hc05_link_remote_device(void)
{
	uint8_t i=0;
	char cmdbuff[100];
	
	func_belmod_uart_hc05_scan_bluetooth_address(&bltDevList);
	func_blemod_uart_hc05_get_remote_device_name(&bltDevList);
	func_blemod_uart_hc05_print_blt_info(&bltDevList);
	
	for(i=0;i<=bltDevList.num;i++)
	{
		if(strstr(bltDevList.name[i],"HC05") != NULL) //非NULL表示找到有名称部分为HC05的设备
		{
			//IhuDebugPrint("VMMWBLE: 搜索到远程HC05模块，即将进行配对连接...\n");
			func_blemod_uart_hc05_blt_addr_convert_str(&bltDevList,',');		
			//配对
			sprintf(cmdbuff,"AT+PAIR=%s,20",bltDevList.unpraseAddr[i]);
			func_blemod_uart_send_AT_command((uint8_t*)cmdbuff, (uint8_t*)"OK", 2);
			//连接	
			sprintf(cmdbuff,"AT+LINK=%s",bltDevList.unpraseAddr[i]);
			return func_blemod_uart_send_AT_command((uint8_t*)cmdbuff, (uint8_t*)"OK", 2);		
		}
	}
	return 1;
}

//测试整个过程
//由于BLE蓝牙详细过程涉及到具体的需求，暂时放在这里，未来待完善。
void ihu_vmmw_blemod_hc05_working_process(void)
{
  uint8_t hc05_connect=0; // 0:未连接       1：连接成功，可以进行数据传输
  uint8_t hc05_mode=0;  // 0：SPP规范      1：AT模式
  char hc05_mode_str[10]="SLAVE";
  uint8_t hc05_role=DEFAULT_HC05_ROLE;  // 0：从模式       1：主模式
	char* redata;
	uint16_t len;
	char hc05_name[30]="HC05_SLAVE";
  char hc05_nameCMD[40];
	
  func_blemod_uart_hc05_init();
  //HC05_EN_LOW(); 	

	HAL_Delay(500);

	//透传模式，SPP规范
  while(hc05_mode==0)
  {
		//这个是拉高引脚
    //HC05_EN_HIGHT(); 
    HAL_Delay(500);
    func_blemod_uart_clear_receive_buffer();
    /* 发送一个AT指令 */
		func_blemod_uart_send_AT_command((uint8_t *)"AT", (uint8_t*)"OK", 2);	
  }  	
	
	//AT模式：这个需要主动拉高引脚
	if(hc05_mode==1)  
  {    
    /*复位、恢复默认状态*/
    func_blemod_uart_send_AT_command((uint8_t*)"AT+RESET", (uint8_t*)"OK", 2);	
    HAL_Delay(800);
    
    func_blemod_uart_send_AT_command((uint8_t*)"AT+ORGL", (uint8_t*)"OK", 2);
    HAL_Delay(200);

		//从模式：发送广播，等待着手机主动连接
    if(hc05_role==0)
    {
      if(func_blemod_uart_send_AT_command((uint8_t*)"AT+ROLE=0", (uint8_t*)"OK", 2) == IHU_SUCCESS)	
      {				
        HAL_Delay(100);        
        sprintf(hc05_mode_str,"SLAVE");
        if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: hc05_mode  = %s\n",hc05_mode_str);	

        sprintf(hc05_name,"HC05_%s_%d",hc05_mode_str,(uint8_t)rand());
        sprintf(hc05_nameCMD,"AT+NAME=%s",hc05_name);
        
        if(func_blemod_uart_send_AT_command((uint8_t*)hc05_nameCMD, (uint8_t*)"OK", 2) == 0)
          if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO：Euqipment has been changed to be [%s]!\n",hc05_name);
        else{
					zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
					IhuErrorPrint("VMMWBLE: change name error!\n");
					return;					
				}
      }
    }
		
		//主模式：主动扫描手机并连接上
    else  
    {
      if(func_blemod_uart_send_AT_command((uint8_t*)"AT+ROLE=1", (uint8_t*)"OK", 2) == 0)	
      {
        HAL_Delay(100);        
        sprintf(hc05_mode_str,"MASTER");
        IhuDebugPrint("VMMWBLE: HC05 mode  = %s\n",hc05_mode_str);          
        sprintf(hc05_name,"HC05_%s_%d",hc05_mode_str,(uint8_t)rand());
        sprintf(hc05_nameCMD,"AT+NAME=%s",hc05_name);	        
        if(func_blemod_uart_send_AT_command((uint8_t*)hc05_nameCMD, (uint8_t*)"OK", 2) == 0){
          if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMFO：Euqipment has been changed to be [%s]!\n",hc05_name);
        }else{
					zIhuSysStaPm.taskRunErrCnt[TASK_ID_VMFO]++;
					IhuErrorPrint("VMMWBLE: change name error!\n");
					return;						
				}
      }
    }
    //清空蓝牙设备列表
    bltDevList.num = 0;
  }
  else
  {  
    hc05_connect=1;
  }

	while(1)
	{
		//搜索蓝牙模块，并进行连接
		if(hc05_connect==0)
		{
      HAL_Delay(100);
			if(hc05_role == 1)	//主模式
			{
				if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: Scaning BLE equipment...\n");				
				while(func_blemod_uart_hc05_link_remote_device()==1)
        {          
        }
			}
			else	//从模式
			{
        func_blemod_uart_send_AT_command((uint8_t*)"AT+INQ", (uint8_t*)"OK", 2);//模块在查询状态，才能容易被其它设备搜索到
        HAL_Delay(1000);        
			}
      hc05_connect=1;
      func_blemod_uart_send_AT_command((uint8_t*)"AT+INIT", (uint8_t*)"OK", 2);
      HAL_Delay(100);
      func_blemod_uart_send_AT_command((uint8_t*)"AT+CLASS=0", (uint8_t*)"OK", 2);
      HAL_Delay(100);
      func_blemod_uart_send_AT_command((uint8_t*)"AT+INQM=1,9,48", (uint8_t*)"OK", 2);  
      HAL_Delay(1000);
      func_blemod_uart_clear_receive_buffer();
      //HC05_EN_HIGHT(); 
      HAL_Delay(500);        
      //HC05_EN_HIGHT(); 
      HAL_Delay(500);   
		}
		//连接后每隔一段时间检查接收缓冲区
		if(hc05_connect==1)
		{
      uint16_t linelen;

      /*获取数据*/
      redata = ihu_bsp_stm32_ble_get_rebuff(&len); 
      linelen = func_blemod_uart_hc05_get_line(zIhuBlemodLineBuff,redata,len);    
      /*检查数据是否有更新*/
      if(linelen<200 && linelen != 0)
      {				                
        if(strcmp(redata,"AT+LED1=ON")==0)
        {
          func_blemod_uart_send_string("+LED1:ON\r\nOK\r\n");

        }
        else if(strcmp(redata,"AT+LED1=OFF")==0)
        {
          func_blemod_uart_send_string("+LED1:OFF\r\nOK\r\n");
        }
        else
        {
          if ((zIhuSysEngPar.debugMode & IHU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE) IhuDebugPrint("VMMWBLE: receive:[%s]\n",redata);
                     switch(redata[len-1]-'0')
           {
             case 0:
              
             break;
              
             case 1:
              
             break;
             
        } 
        }  
        /*处理数据后，清空接收蓝牙模块数据的缓冲区*/
        func_blemod_uart_clear_receive_buffer();       
      }
		}		
		//连接后每隔一段时间通过蓝牙模块发送字符串
		if(hc05_connect==1)
		{
			static uint8_t testdata=0;
			sprintf(zIhuBlemodSendData,"%s>>testdata=%d\r\n",hc05_name,testdata++);
			func_blemod_uart_send_string(zIhuBlemodSendData);			
		}		
	}
	
}

