/*
 * sysversion.h
 *
 *  Created on: 2016年1月11日
 *      Author: test
 */

#ifndef L0COMVM_SYSVERSION_H_
#define L0COMVM_SYSVERSION_H_

#include "da1458x_config_basic.h"

//PEM板子的预定义
//#define IHU_EMCWX_CFG_HW_PEM1_0 //PEM1.0板子
//#define IHU_EMCWX_CFG_HW_PEM1_1  //PEM1.1板子
//#define IHU_EMCWX_CFG_HW_PEM2_0  //PEM2.0板子
#define IHU_EMCWX_CFG_HW_PEM2_1  //PEM2.1板子
//#define IHU_EMCWX_CFG_HW_PEM3  //PEM3板子
#ifdef IHU_EMCWX_CFG_HW_PEM1_0
	#undef IHU_EMCWX_CFG_HW_PEM1_1
	#undef IHU_EMCWX_CFG_HW_PEM2_0
	#undef IHU_EMCWX_CFG_HW_PEM2_1
	#undef IHU_EMCWX_CFG_HW_PEM3
#endif
#ifdef IHU_EMCWX_CFG_HW_PEM1_1
	#undef IHU_EMCWX_CFG_HW_PEM1_0
	#undef IHU_EMCWX_CFG_HW_PEM2_0
	#undef IHU_EMCWX_CFG_HW_PEM2_1
	#undef IHU_EMCWX_CFG_HW_PEM3
#endif
#ifdef IHU_EMCWX_CFG_HW_PEM2_0
	#undef IHU_EMCWX_CFG_HW_PEM1_0
	#undef IHU_EMCWX_CFG_HW_PEM1_1
	#undef IHU_EMCWX_CFG_HW_PEM2_1
	#undef IHU_EMCWX_CFG_HW_PEM3
#endif
#ifdef IHU_EMCWX_CFG_HW_PEM2_1
	#undef IHU_EMCWX_CFG_HW_PEM1_0
	#undef IHU_EMCWX_CFG_HW_PEM1_1
	#undef IHU_EMCWX_CFG_HW_PEM2_0
	#undef IHU_EMCWX_CFG_HW_PEM3
#endif
#ifdef IHU_EMCWX_CFG_HW_PEM3
	#undef IHU_EMCWX_CFG_HW_PEM1_0
	#undef IHU_EMCWX_CFG_HW_PEM1_1
	#undef IHU_EMCWX_CFG_HW_PEM2_0
	#undef IHU_EMCWX_CFG_HW_PEM2_1
#endif


//硬件版本号
#define IHU_EMCWX_CURRENT_HW_TYPE 1 // 1 = WEMC (XQ.WEMC.HW.V01), 2 = PM_CONTROLLER, 3 = WIFI_EMC
//利用DA14580_Config.h中的硬件定义，产生唯一的硬件版本号
//#undef的技巧，自行体会 :-)
#define IHU_EMCWX_CURRENT_HW_PEM 0 // 0 = NULL, 1 = PEM1, 2 = PEM1.1, 3 = PEM2
#ifdef IHU_EMCWX_CFG_HW_PEM1_0
	#undef IHU_EMCWX_CURRENT_HW_PEM
	#define IHU_EMCWX_CURRENT_HW_PEM 1 // 0 = NULL, 1 = PEM1, 2 = PEM1.1, 3 = PEM2
#endif
#ifdef IHU_EMCWX_CFG_HW_PEM1_1
	#undef IHU_EMCWX_CURRENT_HW_PEM
	#define IHU_EMCWX_CURRENT_HW_PEM 2 // 0 = NULL, 1 = PEM1, 2 = PEM1.1, 3 = PEM2
#endif
#ifdef IHU_EMCWX_CFG_HW_PEM2_0
	#undef IHU_EMCWX_CURRENT_HW_PEM
	#define IHU_EMCWX_CURRENT_HW_PEM 3 // 0 = NULL, 1 = PEM1, 2 = PEM1.1, 3 = PEM2
#endif
#ifdef IHU_EMCWX_CFG_HW_PEM2_1
	#undef IHU_EMCWX_CURRENT_HW_PEM
	#define IHU_EMCWX_CURRENT_HW_PEM 4 // 0 = NULL, 1 = PEM1, 2 = PEM1.1, 3 = PEM2, 4=PEM2.1
#endif
#ifdef IHU_EMCWX_CFG_HW_PEM3
	#undef IHU_EMCWX_CURRENT_HW_PEM
	#define IHU_EMCWX_CURRENT_HW_PEM 5 // 0 = NULL, 1 = PEM1, 2 = PEM1.1, 3 = PEM2, 4=PEM2.1, 5=PEM3
#endif

#define IHU_EMCWX_CURRENT_SW_RELEASE  3//R01 = 0x01 (XQ.WEMC.SW.R01.456)
#define IHU_EMCWX_CURRENT_SW_DELIVERY 11 //001 = 0x01 (XQ.WEMC.SW.R01.456), starting from 100 as 2015/10/03
//========================================R1.0 START============================================
//2015/10/04, MYC, IHU_EMCWX_CURRENT_SW_DELIVERY 2
//= ZJL, 2015 Oct.7, IHU_EMCWX_CURRENT_SW_DELIVERY 3
//= ZJL, 2015 Oct.15, IHU_EMCWX_CURRENT_SW_DELIVERY 4
//= ZJL, 2015 Oct.18, IHU_EMCWX_CURRENT_SW_DELIVERY 5
//= ZJL, 2015 Oct.21, IHU_EMCWX_CURRENT_SW_DELIVERY 6
//= MYC, 2015 Oct.29, IHU_EMCWX_CURRENT_SW_DELIVERY 7
//= MYC, 2015 Nov.2, IHU_EMCWX_CURRENT_SW_DELIVERY 8
//= MYC, 2015 Nov.8, IHU_EMCWX_CURRENT_SW_DELIVERY 9
//========================================R2.0 START============================================
//= ZJL, 2016 Jan.17, IHU_EMCWX_CURRENT_SW_DELIVERY R2.001
//= ZJL, 2016 Jan.26, IHU_EMCWX_CURRENT_SW_DELIVERY R2.002
//= ZJL, 2016 Jan.26, IHU_EMCWX_CURRENT_SW_DELIVERY R2.003
//= ZJL, 2016 Jan.26, IHU_EMCWX_CURRENT_SW_DELIVERY R2.004
//= ZJL, 2016 Jan.26, IHU_EMCWX_CURRENT_SW_DELIVERY R2.005
//= ZJL, 2016 Jan.27, IHU_EMCWX_CURRENT_SW_DELIVERY R2.006
//= ZJL, 2016 Jan.28, IHU_EMCWX_CURRENT_SW_DELIVERY R2.007
//= ZJL, 2016 Jan.30, IHU_EMCWX_CURRENT_SW_DELIVERY R2.008
//= ZJL, 2016 Jan.31, IHU_EMCWX_CURRENT_SW_DELIVERY R2.009
//= ZJL, 2016 Feb.1, IHU_EMCWX_CURRENT_SW_DELIVERY R2.010
//= ZJL, 2016 Feb.1, IHU_EMCWX_CURRENT_SW_DELIVERY R2.011
//= ZJL, 2016 Feb.2, IHU_EMCWX_CURRENT_SW_DELIVERY R2.012
//= ZJL, 2016 Feb.3, IHU_EMCWX_CURRENT_SW_DELIVERY R2.013
//= ZJL, 2016 Feb.4, IHU_EMCWX_CURRENT_SW_DELIVERY R2.014
//========================================R2.1 START============================================
//= ZJL, 2016 Feb.5, IHU_EMCWX_CURRENT_SW_DELIVERY R2.101
//= ZJL, 2016 Feb.5, IHU_EMCWX_CURRENT_SW_DELIVERY R2.102
//========================================R3.0 START============================================
//= ZJL, 2016 Feb.5, IHU_EMCWX_CURRENT_SW_DELIVERY R3.01
//= ZJL, 2016 Feb.5, IHU_EMCWX_CURRENT_SW_DELIVERY R3.02
//= ZJL, 2016 Feb.6, IHU_EMCWX_CURRENT_SW_DELIVERY R3.03
//= ZJL, 2016 Feb.21, IHU_EMCWX_CURRENT_SW_DELIVERY R3.04
//= ZJL, 2016 Feb.25, IHU_EMCWX_CURRENT_SW_DELIVERY R3.05
//= ZJL, 2016 Feb.27, IHU_EMCWX_CURRENT_SW_DELIVERY R3.06
//= ZJL, 2016 Apr.18, IHU_EMCWX_CURRENT_SW_DELIVERY R3.07
//= ZJL, 2016 July.27, IHU_EMCWX_CURRENT_SW_DELIVERY R3.08
//= ZJL, 2016 Aug.4, IHU_EMCWX_CURRENT_SW_DELIVERY R03.09
//= ZJL, 2016 Aug.4, IHU_EMCWX_CURRENT_SW_DELIVERY R03.10
//= ZJL, 2016 Aug.5, IHU_EMCWX_CURRENT_SW_DELIVERY R03.11





#endif /* L0COMVM_SYSVERSION_H_ */
