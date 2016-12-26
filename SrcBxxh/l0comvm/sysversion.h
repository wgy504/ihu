/*
 * sysversion.h
 *
 *  Created on: 2016年1月11日
 *      Author: test
 */

#ifndef L0COMVM_SYSVERSION_H_
#define L0COMVM_SYSVERSION_H_

//PEM板子的预定义，严重影响预定义项。未来可以将老式PEM配置项删掉
//.h程序中没法实现常量定义项的逻辑判断，所以只能实现#define和#undef的预编译项

//软件版本定义
#define IHU_EMCWX_CURRENT_SW_RELEASE  3//R01 = 0x01 (XQ.WEMC.SW.R01.456)
#define IHU_EMCWX_CURRENT_SW_DELIVERY 69 //001 = 0x01 (XQ.WEMC.SW.R01.456), starting from 100 as 2015/10/03

#define IHU_EMC68X_CURRENT_SW_RELEASE  IHU_EMCWX_CURRENT_SW_RELEASE//R01 = 0x01
#define IHU_EMC68X_CURRENT_SW_DELIVERY IHU_EMCWX_CURRENT_SW_DELIVERY //001 = 0x01

#define IHU_SCYCB_CURRENT_SW_RELEASE  IHU_EMCWX_CURRENT_SW_RELEASE//R01 = 0x01
#define IHU_SCYCB_CURRENT_SW_DELIVERY IHU_EMCWX_CURRENT_SW_DELIVERY //001 = 0x01

#define CURRENT_SW_RELEASE  IHU_EMCWX_CURRENT_SW_RELEASE//R01 = 0x01
#define CURRENT_SW_DELIVERY IHU_EMCWX_CURRENT_SW_DELIVERY //001 = 0x01

//硬件PEM版本号
#define IHU_EMCWX_CURRENT_HW_TYPE 1 // 1 = WEMC (XQ.WEMC.HW.V01), 2 = PM_CONTROLLER, 3 = WIFI_EMC
#define IHU_EMC68X_CURRENT_HW_TYPE 1
#define IHU_SCYCB_CURRENT_HW_TYPE 1
#define CURRENT_HW_TYPE IHU_EMCWX_CURRENT_HW_TYPE
#define CURRENT_HW_MODULE IHU_EMCWX_CURRENT_HW_TYPE


//软件修改历史记录
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
//= ZJL, 2016 Aug.13, IHU_EMCWX_CURRENT_SW_DELIVERY R03.12
//= ZJL, 2016 Aug.15, IHU_EMCWX_CURRENT_SW_DELIVERY R03.13
//= ZJL, 2016 Aug.16, IHU_EMCWX_CURRENT_SW_DELIVERY R03.14
//= ZJL, 2016 Aug.27, IHU_EMCWX_CURRENT_SW_DELIVERY R03.15
//= ZJL, 2016 Sep.17, IHU_EMCWX_CURRENT_SW_DELIVERY R03.16
//= ZJL, 2016 Sep.17, IHU_EMCWX_CURRENT_SW_DELIVERY R03.17
//= ZJL, 2016 Sep.17, IHU_EMCWX_CURRENT_SW_DELIVERY R03.18
//= ZJL, 2016 Sep.18, IHU_EMCWX_CURRENT_SW_DELIVERY R03.19
//= ZJL, 2016 Sep.19, IHU_EMCWX_CURRENT_SW_DELIVERY R03.20
//= ZJL, 2016 Sep.20, IHU_EMCWX_CURRENT_SW_DELIVERY R03.21
//= ZJL, 2016 Sep.20, IHU_EMCWX_CURRENT_SW_DELIVERY R03.22
//= ZJL, 2016 Sep.21, IHU_EMCWX_CURRENT_SW_DELIVERY R03.23
//= ZJL, 2016 Sep.21, IHU_EMCWX_CURRENT_SW_DELIVERY R03.24
//= ZJL, 2016 Sep.22, IHU_EMCWX_CURRENT_SW_DELIVERY R03.25
//= ZJL, 2016 Sep.22, IHU_EMCWX_CURRENT_SW_DELIVERY R03.26
//= ZJL, 2016 Sep.23, IHU_EMCWX_CURRENT_SW_DELIVERY R03.27
//= ZJL, 2016 Sep.23, IHU_EMCWX_CURRENT_SW_DELIVERY R03.28
//= ZJL, 2016 Sep.24, IHU_EMCWX_CURRENT_SW_DELIVERY R03.29
//= ZJL, 2016 Sep.25, IHU_EMCWX_CURRENT_SW_DELIVERY R03.30
//= ZJL, 2016 Sep.26, IHU_EMCWX_CURRENT_SW_DELIVERY R03.31
//= ZJL, 2016 Sep.26, IHU_EMCWX_CURRENT_SW_DELIVERY R03.32
//= ZJL, 2016 Oct.4, CURRENT_SW_DELIVERY R03.33
//= ZJL, 2016 Oct.11, CURRENT_SW_DELIVERY R03.34
//= ZJL, 2016 Oct.12, CURRENT_SW_DELIVERY R03.35
//= ZJL, 2016 Oct.12, CURRENT_SW_DELIVERY R03.36
//= ZJL/XH, 2016 Oct.22, CURRENT_SW_DELIVERY R03.37
//= ZJL/XH, 2016 Oct.23, CURRENT_SW_DELIVERY R03.38
//= ZJL/XH, 2016 Oct.25, CURRENT_SW_DELIVERY R03.39
//= ZJL/XH, 2016 Oct.27, CURRENT_SW_DELIVERY R03.40
//= ZJL/XH, 2016 Oct.29, CURRENT_SW_DELIVERY R03.41
//= ZJL/XH, 2016 Oct.31, CURRENT_SW_DELIVERY R03.42
//= ZJL/XH, 2016 Oct.31, CURRENT_SW_DELIVERY R03.43
//= ZJL/XH, 2016 Nov.3, CURRENT_SW_DELIVERY R03.44 =>CCL项目
//= ZJL/XH, 2016 Nov.4, CURRENT_SW_DELIVERY R03.45 =>CCL项目
//= ZJL/XH, 2016 Nov.7, CURRENT_SW_DELIVERY R03.46 =>CCL项目
//= ZJL/XH, 2016 Nov.8, CURRENT_SW_DELIVERY R03.47 =>CCL项目
//= ZJL/XH, 2016 Nov.9, CURRENT_SW_DELIVERY R03.48 =>CCL项目
//= ZJL/XH, 2016 Nov.10, CURRENT_SW_DELIVERY R03.49 =>CCL项目
//= ZJL/XH, 2016 Nov.10, CURRENT_SW_DELIVERY R03.50 =>CCL项目
//= ZJL/JT, 2016 Nov.12, CURRENT_SW_DELIVERY R03.51 =>EMCWX项目
//= ZJL, 2016 Dec.6, CURRENT_SW_DELIVERY R03.52 =>EMC68X项目
//= ZJL, 2016 Dec.6, CURRENT_SW_DELIVERY R03.53 =>BFSC项目
//= ZJL, 2016 Dec.7, CURRENT_SW_DELIVERY R03.54 =>BFSC项目
//= ZJL, 2016 Dec.7, CURRENT_SW_DELIVERY R03.55 =>BFSC项目
//= ZJL, 2016 Dec.8, CURRENT_SW_DELIVERY R03.56 =>BFSC项目
//= ZJL, 2016 Dec.8, CURRENT_SW_DELIVERY R03.57 =>BFSC项目
//= ZJL, 2016 Dec.11, CURRENT_SW_DELIVERY R03.58 =>BFSC项目
//= ZJL, 2016 Dec.13, CURRENT_SW_DELIVERY R03.59 =>BFSC项目
//= ZJL, 2016 Dec.14, CURRENT_SW_DELIVERY R03.60 =>CCL项目
//= ZJL, 2016 Dec.15, CURRENT_SW_DELIVERY R03.61 =>CCL项目
//= ZJL, 2016 Dec.15, CURRENT_SW_DELIVERY R03.62 =>CCL项目
//= ZJL, 2016 Dec.15, CURRENT_SW_DELIVERY R03.63 =>CCL项目
//= ZJL, 2016 Dec.16, CURRENT_SW_DELIVERY R03.64 =>CCL项目
//= ZJL, 2016 Dec.16, CURRENT_SW_DELIVERY R03.65 =>CCL项目
//= ZJL, 2016 Dec.18, CURRENT_SW_DELIVERY R03.66 =>CCL项目
//= ZJL, 2016 Dec.19, CURRENT_SW_DELIVERY R03.67 =>CCL项目
//= ZJL, 2016 Dec.25, CURRENT_SW_DELIVERY R03.68 =>CCL项目
//= ZJL, 2016 Dec.26, CURRENT_SW_DELIVERY R03.69 =>CCL项目









#endif /* L0COMVM_SYSVERSION_H_ */
