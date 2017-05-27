/**
 ****************************************************************************************
 *
 * @file mod_cam.c
 *
 * @brief CAM module VM Middleware
 *
 * BXXH team
 * Created by ZJL, 20170516
 *
 ****************************************************************************************
 */
 
#include "vmmw_cam.h"

//全局变量，引用外部
extern int8_t zIhuBspStm32SpsCamRxBuff[IHU_BSP_STM32_SPS_CAM_REC_MAX_LEN];			//串口CAM数据接收缓冲区 
extern int16_t zIhuBspStm32SpsCamRxCount;																				//当前接收数据的字节数 	  

/* 函数体 --------------------------------------------------------------------*/
//同步消息
UINT8  zIhuVmmwCamUlcdsc03FrameCamSynTxSnd[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x0D, 0x00, 0x00, 0x00, 0x00};
UINT8  zIhuVmmwCamUlcdsc03FrameCamSynRxAns[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x0E, 0x0D, 0xFF, 0x00, 0x00};  //0xFF任何数值
UINT8  zIhuVmmwCamUlcdsc03FrameCamSynTxCmd[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x0D, 0x00, 0x00, 0x00, 0x00}; 
//修改波特率：0xAA 0x07，缺省115200，建议不要设置
//拍照消息
UINT8  zIhuVmmwCamUlcdsc03FrameCamPicTxStart[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x04, 0x05, 0x00, 0x00, 0x00};
UINT8  zIhuVmmwCamUlcdsc03FrameCamPicRxAns[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x0E, 0x04, 0xFF, 0x00, 0x00};  //0xFF任何数值
UINT8  zIhuVmmwCamUlcdsc03FrameCamPicRxComp[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x0A, 0x05, 0x00, 0x00, 0x00}; //最后三字节LEN_L, LEN_H, LEN_U   
UINT8  zIhuVmmwCamUlcdsc03FrameCamPicRxErr[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x0F, 0x05, 0x00, 0xFF, 0x00};  //0xFF任何数值
//获得图像：
UINT8  zIhuVmmwCamUlcdsc03FrameCamPicTxPkg[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x0E, 0x00, 0x00, 0x00, 0x00};  //最后两字节 ID_L, ID_H
UINT8  zIhuVmmwCamUlcdsc03FrameCamPicRxPkg[IHU_VMMW_CAM_UART_PKG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //ID_L, ID_H, PKG_L, PKG_H, package，最后是checksum/0
//设置图像质量（默认为最好，可以不用设置）
//UINT8  zIhuVmmwCamUlcdsc03FrameCamSetTxQuality[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x10, 0x02, 0x00, 0x00, 0x00};  //第3个字节，2为最大压缩率，1为中等，0位最低
//UINT8  zIhuVmmwCamUlcdsc03FrameCamSetRxQuafb[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x0E, 0x10, 0xFF, 0x00, 0x00};  //0xFF任何数值
//设置图像分辨率（默认为最好，可以不用设置）
//UINT8  zIhuVmmwCamUlcdsc03FrameCamSetTxResolution[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x01, 0x02, 0x00, 0x00, 0x00};  //第6个字节，2为最大压缩率，1为中等，0位最低
/*  N＝3：160×120 （数据最少） N＝5：320×240 N＝7：640×480 N＝9：800×600 N＝a：1024×768 N＝b：1280×960 N＝c：1600×1200 （数据最多） */
UINT8  zIhuVmmwCamUlcdsc03FrameCamSetRxResfb[IHU_VMMW_CAM_UART_FRAME_SIZE] = {0xAA, 0x0E, 0x01, 0xFF, 0x00, 0x00};  //0xFF任何数值
//设置包大小（默认为512，可以不用设置）
//保存修改的参数：暂时不用


//入口主函数，用于上层调用
//buffer指向10KB左右的数据缓冲区，最好使用任务的上下文
//bufLen指的是，buffer的总长度，实际不得超越这个长度
OPSTAT ihu_vmmw_cam_ulcdsc03_uart_get_picture(uint8_t *buffer, uint32_t bufLen, uint32_t *actualPkg)
{
	int i = 0, j = 0;
	UINT32 pkgSize = 0;
	UINT8 tmpBuffer[IHU_VMMW_CAM_UART_PKG_SIZE];
	UINT16 tmpPkg = 0;
	UINT32 totalLen = 0;
	
	//第一步：发送同步帧
	while ((i < IHU_VMMW_CAM_UART_SYN_REPEAT_CNT) && (ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_sync() == IHU_FAILURE)){
		i++;
	}
	if (i == IHU_VMMW_CAM_UART_SYN_REPEAT_CNT) IHU_ERROR_PRINT_CAM("VMFO: VMMV CAM syn with sensor error!\n");
	
	//第二步：指令拍照
	pkgSize = ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_take_picture();
	if (pkgSize == 0) IHU_ERROR_PRINT_CAM("VMFO: VMMV CAM take picture error from camera!\n");
	
	//第三步：分布将取得数据
	memset(buffer, 0, totalLen);
	for (i = 0; i<pkgSize; i++){
		//重新清零接收缓冲区
		memset(tmpBuffer, 0, sizeof(tmpBuffer));
		//单次接收
		j = 0;
		tmpPkg = ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_fetch_picture(tmpBuffer, i);
		while ((j<IHU_VMMW_CAM_UART_PKG_REPEAT_CNT) && (tmpPkg == 0)){
			tmpPkg = ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_fetch_picture(tmpBuffer, i);
			j++;
		}
		if (j == IHU_VMMW_CAM_UART_PKG_REPEAT_CNT) IHU_ERROR_PRINT_CAM("VMFO: VMMV CAM not receive correct picture data from camera!\n");
		totalLen += tmpPkg;
		if (totalLen > bufLen) IHU_ERROR_PRINT_CAM("VMFO: VMMV CAM receive too long data from camera!\n");
		//将数据复制到目的地
		memcpy(buffer, &tmpBuffer[4], tmpPkg);
		buffer += tmpPkg;
	}
	
	//将真实的长度返回上给上层
	*actualPkg = totalLen;
	
	//复制成功以后的返回
	return IHU_SUCCESS;
}


OPSTAT ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_sync(void)
{
	//发送同步帧
	ihu_usleep(100);
	if (ihu_l1hd_sps_cam_send_data(zIhuVmmwCamUlcdsc03FrameCamSynTxSnd, IHU_VMMW_CAM_UART_FRAME_SIZE) == BSP_FAILURE)
		return IHU_FAILURE;
	ihu_usleep(100);
	
	//等待去收摄像头的应答
	uint8_t rcvBuffer[IHU_VMMW_CAM_UART_FRAME_SIZE];
	memset(rcvBuffer, 0, sizeof(rcvBuffer));
	if (ihu_l1hd_sps_cam_rcv_data_timeout(rcvBuffer, IHU_VMMW_CAM_UART_FRAME_SIZE, IHU_VMMW_CAM_UART_RX_MAX_DELAY) == BSP_FAILURE)
		return IHU_FAILURE;
	
	int i = 0;
	for (i = 0; i < IHU_VMMW_CAM_UART_FRAME_SIZE; i++){
		if ((zIhuVmmwCamUlcdsc03FrameCamSynRxAns[i] != 0xFF) && (zIhuVmmwCamUlcdsc03FrameCamSynRxAns[i] != rcvBuffer[i]))
			return IHU_FAILURE;
	}
	
	return IHU_SUCCESS;
}

UINT32 ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_take_picture(void)
{
	UINT32 segment = 0;
	UINT8 rcvBuffer[IHU_VMMW_CAM_UART_FRAME_SIZE];
	int i = 0;
	
	//发送开始拍照指令
	ihu_usleep(100);
	if (ihu_l1hd_sps_cam_send_data(zIhuVmmwCamUlcdsc03FrameCamPicTxStart, IHU_VMMW_CAM_UART_FRAME_SIZE) == BSP_FAILURE)
		return 0;
	ihu_usleep(100);
	
	//等待摄像头应答
	memset(rcvBuffer, 0, sizeof(rcvBuffer));
	if (ihu_l1hd_sps_cam_rcv_data_timeout(rcvBuffer, IHU_VMMW_CAM_UART_FRAME_SIZE, IHU_VMMW_CAM_UART_RX_MAX_DELAY) == BSP_FAILURE)
		return 0;
	for (i = 0; i < IHU_VMMW_CAM_UART_FRAME_SIZE; i++){
		if ((zIhuVmmwCamUlcdsc03FrameCamPicRxAns[i] != 0xFF) && (zIhuVmmwCamUlcdsc03FrameCamPicRxAns[i] != rcvBuffer[i]))
			return 0;
	}
	
	//等待摄像头启动拍照程序的结果
	memset(rcvBuffer, 0, sizeof(rcvBuffer));
	if (ihu_l1hd_sps_cam_rcv_data_timeout(rcvBuffer, IHU_VMMW_CAM_UART_FRAME_SIZE, IHU_VMMW_CAM_UART_PICTURE_MAX_DELAY) == BSP_FAILURE)
		return 0;
	for (i = 0; i < 3; i++){
		if ((zIhuVmmwCamUlcdsc03FrameCamPicRxComp[i] != 0xFF) && (zIhuVmmwCamUlcdsc03FrameCamPicRxComp[i] != rcvBuffer[i]))
			return 0;
	}
	
	segment = zIhuVmmwCamUlcdsc03FrameCamPicRxComp[3] + zIhuVmmwCamUlcdsc03FrameCamPicRxComp[4]*256 + zIhuVmmwCamUlcdsc03FrameCamPicRxComp[5]*65536;
	
	return segment;
}

UINT16 ihu_vmmw_cam_ulcdsc03_uart_frame_procedure_fetch_picture(UINT8 *buffer, UINT16 index)
{
	UINT8 indexLow = 0, indexHigh = 0;
	UINT16 pkgSize = 0;
	UINT8 checksum = 0;
	int i = 0;
	
	//发送开始拍照指令
	ihu_usleep(100);
	indexLow = index & 0xFF;
	indexHigh = ((index & 0xFF00)>>8);
	zIhuVmmwCamUlcdsc03FrameCamPicTxPkg[IHU_VMMW_CAM_UART_FRAME_SIZE-2] = indexLow;
	zIhuVmmwCamUlcdsc03FrameCamPicTxPkg[IHU_VMMW_CAM_UART_FRAME_SIZE-1] = indexHigh;
	if (ihu_l1hd_sps_cam_send_data(zIhuVmmwCamUlcdsc03FrameCamPicTxPkg, IHU_VMMW_CAM_UART_FRAME_SIZE) == BSP_FAILURE)
		return 0;
	ihu_usleep(100);	

	//接收摄像头应答
	memset(buffer, 0, sizeof(buffer));
	if (ihu_l1hd_sps_cam_rcv_data_timeout(buffer, sizeof(buffer), IHU_VMMW_CAM_UART_RX_MAX_DELAY) == BSP_FAILURE)
		return 0;
	if ((buffer[0] != indexLow) || (buffer[1] != indexHigh))
		return 0;
	//收到的报长
	pkgSize = buffer[2] + buffer[3]*256;
	if ((pkgSize == 0) || (pkgSize > (IHU_VMMW_CAM_UART_PKG_SIZE-6)))
		return 0;
	//检查校验和
	checksum = 0;
	for (i = 0; i < (pkgSize+4); i++){
		checksum += buffer[i];
	}
	if ((checksum != buffer[pkgSize+4]) || (buffer[pkgSize+5] != 0))
		return 0;

	//收到正确的报文
	return pkgSize;
}





