#line 1 "src\\timer.c"















 
 
#line 1 "..\\..\\sdk\\platform\\include\\global_io.h"





























































 


typedef unsigned char      uint8;   
typedef char               int8;    
typedef unsigned short     uint16;  
typedef short              int16;   
typedef unsigned long      uint32;  
typedef long               int32;   
typedef unsigned long long uint64;  
typedef long long          int64;   







 


 
typedef unsigned char      BYTE;     
typedef unsigned short     HWORD;    
typedef unsigned long      WORD;     
typedef long long          DWORD;    


#line 1 "..\\..\\sdk\\platform\\include\\datasheet.h"































  



  
  
  

#line 1 "..\\..\\sdk\\platform\\include\\global_io.h"
#line 41 "..\\..\\sdk\\platform\\include\\datasheet.h"





 





 
typedef enum IRQn
{
     
    NonMaskableInt_IRQn = -14,     
    HardFault_IRQn      = -13,     
    SVCall_IRQn         = -5,      
    PendSV_IRQn         = -2,      
    SysTick_IRQn        = -1,      

     
    BLE_WAKEUP_LP_IRQn  = 0,       
    BLE_FINETGTIM_IRQn  = 1,       
    BLE_GROSSTGTIM_IRQn = 2,       
    BLE_CSCNT_IRQn      = 3,       
    BLE_SLP_IRQn        = 4,       
    BLE_ERROR_IRQn      = 5,       
    BLE_RX_IRQn         = 6,       
    BLE_EVENT_IRQn      = 7,       
    SWTIM_IRQn          = 8,       
    WKUP_QUADEC_IRQn    = 9,       
    BLE_RF_DIAG_IRQn    = 10,      
    BLE_CRYPT_IRQn      = 11,      
    UART_IRQn           = 12,      
    UART2_IRQn          = 13,      
    I2C_IRQn            = 14,      
    SPI_IRQn            = 15,      
    ADC_IRQn            = 16,      
    KEYBRD_IRQn         = 17,      
    RFCAL_IRQn          = 18,      
    GPIO0_IRQn          = 19,      
    GPIO1_IRQn          = 20,      
    GPIO2_IRQn          = 21,      
    GPIO3_IRQn          = 22,      
    GPIO4_IRQn          = 23,      
} IRQn_Type;


 
 
 
 







  
  
  



  
  
  





 
 
 
#line 240 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_IO_CTRL1_REG
 
{
    WORD BITFLD_RFIO_TRIM1_CAP                       : 8;
};


 
struct __RF_LNA_CTRL1_REG
 
{
    WORD BITFLD_LNA_TRIM_CD_LF                       : 6;
    WORD BITFLD_LNA_TRIM_CD_HF                       : 6;
};



 
struct __RF_LNA_CTRL2_REG
 
{
    WORD BITFLD_LNA_TRIM_GM_HI                       : 6;
    WORD BITFLD_LNA_TRIM_GM_LO                       : 6;
};



 
struct __RF_LNA_CTRL3_REG
 
{
    WORD BITFLD_LNA_TRIM_CGS                         : 5;
};


 
struct __RF_RSSI_COMP_CTRL_REG
 
{
    WORD BITFLD_RSSI_COMP01                          : 4;
    WORD BITFLD_RSSI_COMP10                          : 4;
    WORD BITFLD_RSSI_COMP11                          : 4;
    WORD BITFLD_RSSI_COMP00                          : 4;
};





 
struct __RF_VCO_CTRL_REG
 
{
    WORD BITFLD_VCO_AMPL_SET                         : 4;
};


 
struct __SPOTP_TEST_REG
 
{
    WORD BITFLD_SPOTP_ACTIVE                         : 1;
    WORD BITFLD_LDO_OTP_WRITE                        : 1;
};



 
struct __DCDC_CTRL_REG
 
{
    WORD                                             : 1;
    WORD BITFLD_DCDC_VBAT1V_LEV                      : 3;
    WORD                                             : 1;
    WORD BITFLD_DCDC_MODE                            : 3;
    WORD BITFLD_DCDC_DRIVE_NSW                       : 2;
    WORD BITFLD_DCDC_DRIVE_PSW                       : 2;
    WORD BITFLD_DCDC_DRIVE_OSW                       : 2;
    WORD BITFLD_DCDC_TUNE                            : 2;
};
#line 328 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __DCDC_CTRL2_REG
 
{
    WORD BITFLD_DCDC_AUTO_CAL                        : 3;
    WORD BITFLD_DCDC_CUR_LIM                         : 4;
    WORD BITFLD_DCDC_TON                             : 2;
    WORD BITFLD_DCDC_VBAT3V_LEV                      : 3;
    WORD BITFLD_DCDC_VOLT_LEV                        : 4;
};






 
struct __DCDC_CTRL3_REG
 
{
    WORD BITFLD_BUCK_ENABLE                          : 1;
    WORD BITFLD_DCDC_IDLE_CLK                        : 2;
    WORD BITFLD_DCDC_TIMEOUT                         : 2;
};




 
struct __DCDC_CAL1_REG
 
{
    WORD BITFLD_DCDC_CAL1                            : 6;
};


 
struct __DCDC_CAL2_REG
 
{
    WORD BITFLD_DCDC_CAL2                            : 6;
};


 
struct __DCDC_CAL3_REG
 
{
    WORD BITFLD_DCDC_CAL3                            : 6;
};


 
struct __CLK_REF_SEL_REG
 
{
    WORD BITFLD_REF_CLK_SEL                          : 2;
    WORD BITFLD_REF_CAL_START                        : 1;
};



 
struct __CLK_REF_CNT_REG
 
{
    WORD BITFLD_REF_CNT_VAL                          : 16;
};


 
struct __CLK_REF_VAL_L_REG
 
{
    WORD BITFLD_XTAL_CNT_VAL                         : 16;
};


 
struct __CLK_REF_VAL_H_REG
 
{
    WORD BITFLD_XTAL_CNT_VAL                         : 16;
};


 
struct __RF_BMCW_REG
 
{
    WORD BITFLD_CN_WR                                : 8;
    WORD BITFLD_CN_SEL                               : 1;
};



 
struct __RF_OVERRULE_REG
 
{
    WORD BITFLD_TX_DIS_WR                            : 1;
    WORD BITFLD_TX_EN_WR                             : 1;
    WORD BITFLD_RX_DIS_WR                            : 1;
    WORD BITFLD_RX_EN_WR                             : 1;
};





 
struct __RF_CALSTATE_REG
 
{
    WORD BITFLD_CALSTATE                             : 4;
};


 
struct __RF_CALCAP1_REG
 
{
    WORD BITFLD_VCO_CALCAP_LOW                       : 16;
};


 
struct __RF_CALCAP2_REG
 
{
    WORD BITFLD_VCO_CALCAP_HIGH                      : 2;
};


 
struct __RF_SCAN_FEEDBACK_REG
 
{
    WORD BITFLD_LF_RES                               : 4;
    WORD BITFLD_CP_CUR                               : 4;
};



 
struct __RF_CAL_CTRL_REG
 
{
    WORD BITFLD_SO_CAL                               : 1;
    WORD BITFLD_EO_CAL                               : 1;
    WORD BITFLD_MGAIN_CAL_DIS                        : 1;
    WORD BITFLD_IFF_CAL_DIS                          : 1;
    WORD BITFLD_DC_OFFSET_CAL_DIS                    : 1;
    WORD BITFLD_VCO_CAL_DIS                          : 1;
};
#line 490 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_REF_OSC_REG
 
{
    WORD BITFLD_CNT_RO                               : 6;
    WORD BITFLD_CNT_CLK                              : 9;
};



 
struct __RF_IRQ_CTRL_REG
 
{
    WORD BITFLD_EO_CAL_CLEAR                         : 1;
};


 
struct __RF_IFF_RESULT_REG
 
{
    WORD BITFLD_IF_CAL_CAP_RD                        : 5;
};


 
struct __RF_ADCI_DC_OFFSET_REG
 
{
    WORD BITFLD_ADC_OFFP_I_RD                        : 8;
    WORD BITFLD_ADC_OFFN_I_RD                        : 8;
};



 
struct __RF_ADCQ_DC_OFFSET_REG
 
{
    WORD BITFLD_ADC_OFFP_Q_RD                        : 8;
    WORD BITFLD_ADC_OFFN_Q_RD                        : 8;
};



 
struct __RF_DC_OFFSET_RESULT_REG
 
{
    WORD BITFLD_DCOFFSET_I_RD                        : 8;
    WORD BITFLD_DCOFFSET_Q_RD                        : 8;
};



 
struct __RF_SYNTH_RESULT_REG
 
{
    WORD BITFLD_GAUSS_GAIN_CAL_RD                    : 8;
    WORD BITFLD_VCO_FREQTRIM_RD                      : 4;
};



 
struct __RF_SYNTH_RESULT2_REG
 
{
    WORD BITFLD_GAUSS_GAIN_RD                        : 8;
    WORD BITFLD_CN_CAL_RD                            : 7;
};



 
struct __RF_SYNTH_RESULT3_REG
 
{
    WORD BITFLD_MDSTATE_RD                           : 16;
};


 
struct __RF_ENABLE_CONFIG1_REG
 
{
    WORD BITFLD_lna_core_en                          : 8;
    WORD BITFLD_lna_ldo_en                           : 8;
};



 
struct __RF_ENABLE_CONFIG2_REG
 
{
    WORD BITFLD_mix_ldo_en                           : 8;
    WORD BITFLD_lna_cgm_en                           : 8;
};



 
struct __RF_ENABLE_CONFIG3_REG
 
{
    WORD BITFLD_ifadc_ldo_en                         : 8;
    WORD BITFLD_iff_ldo_en                           : 8;
};



 
struct __RF_ENABLE_CONFIG4_REG
 
{
    WORD BITFLD_md_ldo_en                            : 8;
    WORD BITFLD_vco_ldo_en                           : 8;
};



 
struct __RF_ENABLE_CONFIG5_REG
 
{
    WORD BITFLD_pa_ldo_en                            : 8;
    WORD BITFLD_pfd_ldo_en                           : 8;
};



 
struct __RF_ENABLE_CONFIG6_REG
 
{
    WORD BITFLD_vco_bias_en                          : 8;
    WORD BITFLD_cp_switch_en                         : 8;
};



 
struct __RF_ENABLE_CONFIG7_REG
 
{
    WORD BITFLD_lna_ldo_zero                         : 8;
    WORD BITFLD_cp_bias_en                           : 8;
};



 
struct __RF_ENABLE_CONFIG8_REG
 
{
    WORD BITFLD_pa_en                                : 8;
    WORD BITFLD_pa_ramp_en                           : 8;
};



 
struct __RF_ENABLE_CONFIG9_REG
 
{
    WORD BITFLD_iff_en                               : 8;
    WORD BITFLD_mix_en                               : 8;
};



 
struct __RF_ENABLE_CONFIG10_REG
 
{
    WORD BITFLD_vco_en                               : 8;
    WORD BITFLD_adc_en                               : 8;
};



 
struct __RF_ENABLE_CONFIG11_REG
 
{
    WORD BITFLD_cp_en                                : 8;
    WORD BITFLD_md_lobuf_en                          : 8;
};



 
struct __RF_ENABLE_CONFIG12_REG
 
{
    WORD BITFLD_gauss_en                             : 8;
    WORD BITFLD_pfd_en                               : 8;
};



 
struct __RF_ENABLE_CONFIG13_REG
 
{
    WORD BITFLD_lobuf_pa_en                          : 8;
    WORD BITFLD_rfio_en                              : 8;
};



 
struct __RF_ENABLE_CONFIG14_REG
 
{
    WORD BITFLD_div2_en                              : 8;
    WORD BITFLD_lobuf_rxiq_en                        : 8;
};



 
struct __RF_ENABLE_CONFIG15_REG
 
{
    WORD BITFLD_vco_bias_sh_open_en                  : 8;
    WORD BITFLD_cp_bias_sh_open                      : 8;
};



 
struct __RF_ENABLE_CONFIG16_REG
 
{
    WORD BITFLD_gauss_bias_sh_open_en                : 8;
    WORD BITFLD_iff_bias_sh_open_en                  : 8;
};



 
struct __RF_ENABLE_CONFIG17_REG
 
{
    WORD BITFLD_plldig_en                            : 8;
    WORD BITFLD_mix_bias_sh_open_en                  : 8;
};



 
struct __RF_ENABLE_CONFIG18_REG
 
{
    WORD BITFLD_dem_en                               : 8;
    WORD BITFLD_pllclosed_en                         : 8;
};



 
struct __RF_ENABLE_CONFIG19_REG
 
{
    WORD BITFLD_cal_en                               : 8;
    WORD BITFLD_ldo_zero_en                          : 8;
};



 
struct __RF_ENABLE_CONFIG20_REG
 
{
    WORD BITFLD_ldo_rfio_en                          : 8;
    WORD BITFLD_tdc_en                               : 8;
};



 
struct __RF_ENABLE_CONFIG21_REG
 
{
    WORD BITFLD_rfio_bias_sh_open                    : 8;
    WORD BITFLD_rfio_bias_en                         : 8;
};



 
struct __RF_ENABLE_CONFIG22_REG
 
{
    WORD BITFLD_adc_clk_en                           : 8;
    WORD BITFLD_ldo_radio_en                         : 8;
};



 
struct __RF_ENABLE_CONFIG23_REG
 
{
    WORD BITFLD_spare_en_3                           : 8;
    WORD BITFLD_tr_pwm_off_en                        : 8;
};



 
struct __RF_CNTRL_TIMER_1_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_2_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_3_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_4_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_5_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_6_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_7_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_8_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_9_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_10_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_11_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_12_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_13_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __RF_CNTRL_TIMER_14_REG
 
{
    WORD BITFLD_SET_OFFSET                           : 8;
    WORD BITFLD_RESET_OFFSET                         : 8;
};



 
struct __BIAS_CTRL1_REG
 
{
    WORD BITFLD_MIX_BIAS_SET                         : 4;
    WORD BITFLD_CP_BIAS_SET                          : 4;
    WORD BITFLD_VCO_BIAS_SET                         : 4;
    WORD BITFLD_IFF_BIAS_SET                         : 4;
};





 
struct __RF_SPARE1_REG
 
{
    WORD BITFLD_RF_SPARE1                            : 16;
};


 
struct __RF_MIXER_CTRL1_REG
 
{
    WORD BITFLD_MIX_TRIM_GMBIAS                      : 4;
    WORD BITFLD_MIX_TRIM_IBIAS                       : 4;
    WORD BITFLD_MIX_TRIM_VCM                         : 4;
    WORD BITFLD_MIX_SPARE                            : 4;
};





 
struct __RF_MIXER_CTRL2_REG
 
{
    WORD BITFLD_MIX_CAL_CAP_WR                       : 5;
    WORD BITFLD_MIX_CAL_CAP_SEL                      : 1;
};



 
struct __RF_IFF_CTRL1_REG
 
{
    WORD BITFLD_IF_CAL_CAP_WR                        : 5;
    WORD BITFLD_IF_CAL_CAP_SEL                       : 1;
    WORD BITFLD_IF_MUTE                              : 1;
    WORD BITFLD_RO_TO_PINS                           : 1;
    WORD BITFLD_IFF_DCOC_DAC_DIS                     : 1;
};






 
struct __RF_ADC_CTRL1_REG
 
{
    WORD BITFLD_ADC_DC_OFFSET_SEL                    : 1;
    WORD                                             : 12;
    WORD BITFLD_ADC_MUTE                             : 1;
    WORD BITFLD_ADC_SIGN                             : 1;
};




 
struct __RF_ADC_CTRL2_REG
 
{
    WORD BITFLD_ADC_OFFP_I_WR                        : 8;
    WORD BITFLD_ADC_OFFN_I_WR                        : 8;
};



 
struct __RF_ADC_CTRL3_REG
 
{
    WORD BITFLD_ADC_OFFP_Q_WR                        : 8;
    WORD BITFLD_ADC_OFFN_Q_WR                        : 8;
};



 
struct __RF_DEM_CTRL_REG
 
{
    WORD BITFLD_RXDATA_INV                           : 1;
    WORD BITFLD_DEM_HSI_POL                          : 1;
    WORD BITFLD_MATCH0101_TH                         : 4;
    WORD BITFLD_EQUAL_EN                             : 1;
};





 
struct __RF_AGC_LUT_01_REG
 
{
    WORD BITFLD_VGA2_GAIN0                           : 3;
    WORD BITFLD_VGA1_GAIN0                           : 3;
    WORD BITFLD_LNA_GAIN0                            : 2;
    WORD BITFLD_VGA2_GAIN1                           : 3;
    WORD BITFLD_VGA1_GAIN1                           : 3;
    WORD BITFLD_LNA_GAIN1                            : 2;
};
#line 1071 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_AGC_LUT_23_REG
 
{
    WORD BITFLD_VGA2_GAIN2                           : 3;
    WORD BITFLD_VGA1_GAIN2                           : 3;
    WORD BITFLD_LNA_GAIN2                            : 2;
    WORD BITFLD_VGA2_GAIN3                           : 3;
    WORD BITFLD_VGA1_GAIN3                           : 3;
    WORD BITFLD_LNA_GAIN3                            : 2;
};
#line 1089 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_AGC_LUT_45_REG
 
{
    WORD BITFLD_VGA2_GAIN4                           : 3;
    WORD BITFLD_VGA1_GAIN4                           : 3;
    WORD BITFLD_LNA_GAIN4                            : 2;
    WORD BITFLD_VGA2_GAIN5                           : 3;
    WORD BITFLD_VGA1_GAIN5                           : 3;
    WORD BITFLD_LNA_GAIN5                            : 2;
};
#line 1107 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_AGC_LUT_67_REG
 
{
    WORD BITFLD_VGA2_GAIN6                           : 3;
    WORD BITFLD_VGA1_GAIN6                           : 3;
    WORD BITFLD_LNA_GAIN6                            : 2;
    WORD BITFLD_VGA2_GAIN7                           : 3;
    WORD BITFLD_VGA1_GAIN7                           : 3;
    WORD BITFLD_LNA_GAIN7                            : 2;
};
#line 1125 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_AGC_LUT_89_REG
 
{
    WORD BITFLD_VGA2_GAIN8                           : 3;
    WORD BITFLD_VGA1_GAIN8                           : 3;
    WORD BITFLD_LNA_GAIN8                            : 2;
    WORD BITFLD_VGA2_GAIN9                           : 3;
    WORD BITFLD_VGA1_GAIN9                           : 3;
    WORD BITFLD_LNA_GAIN9                            : 2;
};
#line 1143 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_AGC_CTRL1_REG
 
{
    WORD BITFLD_AGC_TH_LOW                           : 7;
    WORD BITFLD_AGC_TH_HIGH                          : 7;
    WORD BITFLD_AGC_MODE                             : 2;
};




 
struct __RF_AGC_CTRL2_REG
 
{
    WORD BITFLD_RSSI_TH                              : 6;
    WORD BITFLD_EN_FRZ_GAIN                          : 1;
    WORD BITFLD_AGCSETTING_SEL                       : 1;
    WORD BITFLD_AGCSETTING_WR                        : 4;
    WORD BITFLD_SLOW_AGC                             : 1;
};






 
struct __RF_AFC_CTRL_REG
 
{
    WORD BITFLD_AFC_MODE                             : 4;
    WORD BITFLD_POLE1                                : 2;
    WORD BITFLD_POLE2                                : 2;
};




 
struct __RF_DC_OFFSET_CTRL1_REG
 
{
    WORD BITFLD_DCOFFSET_I_WR                        : 8;
    WORD BITFLD_DCOFFSET_Q_WR                        : 8;
};



 
struct __RF_DC_OFFSET_CTRL2_REG
 
{
    WORD BITFLD_DCOFFSET_SEL                         : 1;
    WORD BITFLD_DCPARCAL_EN                          : 1;
    WORD BITFLD_DCPOLE                               : 2;
    WORD BITFLD_DCNSTEP                              : 3;
    WORD BITFLD_DCNGAIN                              : 2;
};






 
struct __RF_DC_OFFSET_CTRL3_REG
 
{
    WORD BITFLD_DCBETA_I                             : 8;
    WORD BITFLD_DCBETA_Q                             : 8;
};



 
struct __RF_DC_OFFSET_CTRL4_REG
 
{
    WORD BITFLD_DCAGCSETTING_FULL0                   : 4;
    WORD BITFLD_DCAGCSETTING_FULL1                   : 4;
    WORD BITFLD_DCAGCSETTING_FULL2                   : 4;
    WORD BITFLD_DCAGCSETTING_FULL3                   : 4;
};





 
struct __RF_RADIG_SPARE_REG
 
{
    WORD BITFLD_RADIG_SPARE                          : 16;
};


 
struct __RF_AGC_RESULT_REG
 
{
    WORD BITFLD_AFC_RD                               : 8;
    WORD BITFLD_AGCSETTING_RD                        : 4;
};



 
struct __RF_RSSI_RESULT_REG
 
{
    WORD BITFLD_RSSI_PH_RD                           : 6;
    WORD BITFLD_RSSI_AVG_RD                          : 10;
};



 
struct __RF_PA_CTRL_REG
 
{
    WORD BITFLD_PA_GAIN                              : 2;
    WORD BITFLD_PA_PW                                : 3;
    WORD BITFLD_PA_RAMPSPEED                         : 2;
    WORD                                             : 4;
    WORD BITFLD_LEVEL_LDO_RFPA                       : 4;
};





 
struct __RF_SYNTH_CTRL1_REG
 
{
    WORD BITFLD_CHANNEL_ZERO                         : 12;
    WORD BITFLD_SGN                                  : 1;
    WORD BITFLD_CS                                   : 1;
    WORD BITFLD_PLL_HSI_POL                          : 1;
};





 
struct __RF_SYNTH_CTRL2_REG
 
{
    WORD BITFLD_SD_ORDER_RX                          : 2;
    WORD BITFLD_SD_ORDER_TX                          : 2;
    WORD BITFLD_MODINDEX                             : 2;
    WORD BITFLD_DELAY                                : 2;
    WORD BITFLD_GAUSS_INV                            : 1;
    WORD BITFLD_GAUSS_86                             : 1;
    WORD BITFLD_TXDATA_INV                           : 1;
    WORD BITFLD_EO_PACKET_DIS                        : 1;
    WORD BITFLD_BT_SEL                               : 1;
};
#line 1314 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_SYNTH_CTRL3_REG
 
{
    WORD BITFLD_MODVAL_WR                            : 14;
    WORD BITFLD_MODVAL_SEL                           : 1;
};



 
struct __RF_VCOCAL_CTRL_REG
 
{
    WORD BITFLD_VCO_FREQTRIM_WR                      : 4;
    WORD BITFLD_VCO_FREQTRIM_SEL                     : 1;
    WORD BITFLD_VCOCAL_PERIOD                        : 2;
};




 
struct __RF_MGAIN_CTRL_REG
 
{
    WORD BITFLD_GAUSS_GAIN_WR                        : 8;
    WORD BITFLD_GAUSS_GAIN_SEL                       : 1;
    WORD BITFLD_MGAIN_DBL_TRANSMIT                   : 1;
    WORD BITFLD_MGAIN_CMP_INV                        : 1;
    WORD BITFLD_MGAIN_AVER                           : 2;
    WORD BITFLD_KMOD_ALPHA                           : 3;
};
#line 1354 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_MGAIN_CTRL2_REG
 
{
    WORD BITFLD_MGAIN_TRANSMIT_LENGTH                : 7;
};


 
struct __RF_MGC_CTRL_REG
 
{
    WORD BITFLD_MGC_GAIN_SET                         : 1;
    WORD BITFLD_MGC_POLE_SW                          : 1;
    WORD BITFLD_GAUSS_DAC_CTRL                       : 2;
};




 
struct __RF_VCOVAR_CTRL_REG
 
{
    WORD BITFLD_TUNE_VAR_V0                          : 3;
    WORD BITFLD_TUNE_VAR_V1                          : 3;
    WORD BITFLD_TUNE_VAR_V2                          : 3;
    WORD BITFLD_TUNE_VAR_V3                          : 3;
    WORD BITFLD_MOD_VAR_V0                           : 2;
    WORD BITFLD_MOD_VAR_V1                           : 2;
};
#line 1392 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __RF_VCO_CALCAP_BIT14_REG
 
{
    WORD BITFLD_VCO_CALCAP_BIT14                     : 16;
};


 
struct __RF_VCO_CALCAP_BIT15_REG
 
{
    WORD BITFLD_VCO_CALCAP_BIT15                     : 16;
};


 
struct __RF_PFD_CTRL_REG
 
{
    WORD BITFLD_FIXED_CUR_SET                        : 2;
    WORD BITFLD_FIXED_CUR_EN                         : 1;
    WORD BITFLD_PFD_POLARITY                         : 1;
};




 
struct __RF_CP_CTRL_REG
 
{
    WORD BITFLD_CP_CUR_SET_RX                        : 4;
    WORD BITFLD_CP_CUR_SET_TX                        : 4;
    WORD BITFLD_CP_CUR_RX                            : 4;
    WORD BITFLD_CP_CUR_TX                            : 4;
};





 
struct __RF_LF_RES_CTRL_REG
 
{
    WORD BITFLD_LF_RES_SET_RX                        : 4;
    WORD BITFLD_LF_RES_SET_TX                        : 4;
    WORD BITFLD_LF_RES_RX                            : 4;
    WORD BITFLD_LF_RES_TX                            : 4;
};





 
struct __RF_LF_CTRL_REG
 
{
    WORD BITFLD_LF_CAL_CAP_WR                        : 5;
    WORD BITFLD_LF_CAL_CAP_SEL                       : 1;
    WORD BITFLD_LF_SHORT_R4                          : 1;
};




 
struct __RF_TDC_CTRL_REG
 
{
    WORD BITFLD_CTRL_FAST                            : 4;
    WORD BITFLD_CTRL_SLOW                            : 4;
    WORD BITFLD_CAL_PH_1                             : 1;
    WORD BITFLD_CAL_PH_2                             : 1;
    WORD BITFLD_REF_CTRL                             : 2;
    WORD BITFLD_TDC_CONNECT                          : 1;
};
#line 1478 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __TEST_CTRL_REG
 
{
    WORD BITFLD_SHOW_CLOCKS                          : 1;
    WORD BITFLD_ENABLE_RFPT                          : 1;
    WORD BITFLD_SHOW_DC_STATE                        : 1;
    WORD BITFLD_SHOW_DC_COMP                         : 1;
    WORD BITFLD_XTAL16M_CAP_TEST_EN                  : 1;
    WORD BITFLD_SHOW_IF_RO                           : 1;
    WORD BITFLD_PLL_TST_MODE                         : 1;
};
#line 1498 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __TEST_CTRL2_REG
 
{
    WORD BITFLD_ANA_TESTMUX_CTRL                     : 4;
    WORD                                             : 4;
    WORD BITFLD_RF_IN_TESTMUX_CTRL                   : 2;
};



 
struct __TEST_CTRL3_REG
 
{
    WORD BITFLD_RF_TEST_OUT_SEL                      : 6;
    WORD                                             : 2;
    WORD BITFLD_RF_TEST_OUT_PARAM                    : 8;
};



 
struct __TEST_CTRL4_REG
 
{
    WORD BITFLD_RF_TEST_IN_SEL                       : 3;
    WORD                                             : 5;
    WORD BITFLD_RF_TEST_IN_PARAM                     : 8;
};



 
struct __TEST_CTRL5_REG
 
{
    WORD BITFLD_TEST_VDD                             : 1;
    WORD BITFLD_TEST_OTP_ADC_VVD                     : 1;
    WORD BITFLD_TEST_OTP_OTA                         : 1;
    WORD BITFLD_TEST_OTP_VSS                         : 1;
    WORD                                             : 4;
    WORD BITFLD_TEST_STRUCT                          : 4;
    WORD BITFLD_DCDC_NSW                             : 1;
    WORD BITFLD_DCDC_PSW                             : 1;
    WORD BITFLD_DCDC_OUTSW                           : 1;
    WORD BITFLD_DCDC_FORCE_IDLE                      : 1;
};
#line 1556 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BIST_CTRL_REG
 
{
    WORD BITFLD_RAM_BIST_CONFIG                      : 2;
    WORD BITFLD_ROMBIST_ENABLE                       : 1;
    WORD BITFLD_RAMBIST_ENABLE                       : 1;
    WORD BITFLD_SHOW_BIST                            : 1;
    WORD BITFLD_ROM_BIST_BUSY                        : 1;
    WORD BITFLD_RETRAM_BIST_LINE_FAIL                : 1;
    WORD BITFLD_RETRAM_BIST_FAIL                     : 1;
    WORD BITFLD_RETRAM_BIST_BUSY                     : 1;
    WORD BITFLD_SYSRAM_BIST_LINE_FAIL                : 1;
    WORD BITFLD_SYSRAM_BIST_FAIL                     : 1;
    WORD BITFLD_SYSRAM_BIST_BUSY                     : 1;
    WORD BITFLD_RAM_BIST_PATTERN                     : 2;
};
#line 1586 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __ROMBIST_RESULTL_REG
 
{
    WORD BITFLD_ROMBIST_RESULTL                      : 16;
};


 
struct __ROMBIST_RESULTH_REG
 
{
    WORD BITFLD_ROMBIST_RESULTH                      : 16;
};


 
struct __RFPT_CTRL_REG
 
{
    WORD BITFLD_RFPT_PACK_EN                         : 1;
    WORD                                             : 1;
};


 
struct __RFPT_ADDR_REG
 
{
    WORD                                             : 2;
    WORD BITFLD_RFPT_ADDR                            : 14;
};


 
struct __RFPT_LEN_REG
 
{
    WORD BITFLD_RFPT_LEN                             : 14;
};


 
struct __RFPT_STAT_REG
 
{
    WORD BITFLD_RFPT_ACTIVE                          : 1;
    WORD BITFLD_RFPT_OFLOW_STK                       : 1;
};



 
 
 
#line 1690 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_RWBTLECNTL_REG
 
{
    WORD BITFLD_SYNCERR                              : 3;
    WORD                                             : 1;
    WORD BITFLD_RXWINSZDEF                           : 4;
    WORD BITFLD_RWBLE_EN                             : 1;
    WORD BITFLD_WLSYNC_EN                            : 1;
    WORD BITFLD_ADVERRFILT_EN                        : 1;
    WORD BITFLD_RXDESCPTRSEL                         : 1;
    WORD BITFLD_TXWINOFFSEL                          : 1;
    WORD                                             : 3;
    WORD BITFLD_HOP_REMAP_DSB                        : 1;
    WORD BITFLD_CRC_DSB                              : 1;
    WORD BITFLD_WHIT_DSB                             : 1;
    WORD BITFLD_CRYPT_DSB                            : 1;
    WORD BITFLD_NESN_DSB                             : 1;
    WORD BITFLD_SN_DSB                               : 1;
    WORD BITFLD_MD_DSB                               : 1;
    WORD                                             : 1;
    WORD BITFLD_SCAN_ABORT                           : 1;
    WORD BITFLD_ADVERT_ABORT                         : 1;
    WORD BITFLD_RFTEST_ABORT                         : 1;
    WORD                                             : 2;
    WORD BITFLD_REG_SOFT_RST                         : 1;
    WORD BITFLD_MASTER_TGSOFT_RST                    : 1;
    WORD BITFLD_MASTER_SOFT_RST                      : 1;
};
#line 1740 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_VERSION_REG
 
{
    WORD BITFLD_BUILD                                : 8;
    WORD BITFLD_UPG                                  : 8;
    WORD BITFLD_REL                                  : 8;
    WORD BITFLD_TYP                                  : 8;
};





 
struct __BLE_RWBTLECONF_REG
 
{
    WORD BITFLD_BUSWIDTH                             : 1;
    WORD BITFLD_USECRYPT                             : 1;
    WORD BITFLD_USEDBG                               : 1;
    WORD BITFLD_WLAN                                 : 1;
    WORD BITFLD_INTMODE                              : 1;
    WORD BITFLD_DMMODE                               : 1;
    WORD                                             : 2;
    WORD BITFLD_CLK_SEL                              : 6;
    WORD                                             : 2;
    WORD BITFLD_RFIF                                 : 7;
    WORD                                             : 1;
    WORD BITFLD_ADD_WIDTH                            : 6;
};
#line 1781 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_INTCNTL_REG
 
{
    WORD BITFLD_CSCNTINTMSK                          : 1;
    WORD BITFLD_RXINTMSK                             : 1;
    WORD BITFLD_SLPINTMSK                            : 1;
    WORD BITFLD_EVENTINTMSK                          : 1;
    WORD BITFLD_CRYPTINTMSK                          : 1;
    WORD BITFLD_ERRORINTMSK                          : 1;
    WORD BITFLD_GROSSTGTIMINTMSK                     : 1;
    WORD BITFLD_FINETGTIMINTMSK                      : 1;
    WORD BITFLD_RADIOCNTLINTMSK                      : 1;
    WORD                                             : 6;
    WORD BITFLD_CSCNTDEVMSK                          : 1;
    WORD BITFLD_INTCSCNTL                            : 16;
};
#line 1810 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_INTSTAT_REG
 
{
    WORD BITFLD_CSCNTINTSTAT                         : 1;
    WORD BITFLD_RXINTSTAT                            : 1;
    WORD BITFLD_SLPINTSTAT                           : 1;
    WORD BITFLD_EVENTINTSTAT                         : 1;
    WORD BITFLD_CRYPTINTSTAT                         : 1;
    WORD BITFLD_ERRORINTSTAT                         : 1;
    WORD BITFLD_GROSSTGTIMINTSTAT                    : 1;
    WORD BITFLD_FINETGTIMINTSTAT                     : 1;
    WORD BITFLD_RADIOCNTLINTSTAT                     : 1;
};
#line 1834 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_INTRAWSTAT_REG
 
{
    WORD BITFLD_CSCNTINTRAWSTAT                      : 1;
    WORD BITFLD_RXINTRAWSTAT                         : 1;
    WORD BITFLD_SLPINTRAWSTAT                        : 1;
    WORD BITFLD_EVENTINTRAWSTAT                      : 1;
    WORD BITFLD_CRYPTINTRAWSTAT                      : 1;
    WORD BITFLD_ERRORINTRAWSTAT                      : 1;
    WORD BITFLD_GROSSTGTIMINTRAWSTAT                 : 1;
    WORD BITFLD_FINETGTIMINTRAWSTAT                  : 1;
    WORD BITFLD_RADIOCNTLINTRAWSTAT                  : 1;
};
#line 1858 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_INTACK_REG
 
{
    WORD BITFLD_CSCNTINTACK                          : 1;
    WORD BITFLD_RXINTACK                             : 1;
    WORD BITFLD_SLPINTACK                            : 1;
    WORD BITFLD_EVENTINTACK                          : 1;
    WORD BITFLD_CRYPTINTACK                          : 1;
    WORD BITFLD_ERRORINTACK                          : 1;
    WORD BITFLD_GROSSTGTIMINTACK                     : 1;
    WORD BITFLD_FINETGTIMINTACK                      : 1;
    WORD BITFLD_RADIOCNTLINTACK                      : 1;
};
#line 1882 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_BASETIMECNT_REG
 
{
    DWORD BITFLD_BASETIMECNT                         : 27;
};


 
struct __BLE_FINETIMECNT_REG
 
{
    WORD BITFLD_FINECNT                              : 10;
};


 
struct __BLE_BDADDRL_REG
 
{
    DWORD BITFLD_BDADDRL                             : 32;
};


 
struct __BLE_BDADDRU_REG
 
{
    WORD BITFLD_BDADDRU                              : 16;
    WORD BITFLD_PRIV_NPUB                            : 1;
};



 
struct __BLE_CURRENTRXDESCPTR_REG
 
{
    WORD BITFLD_CURRENTRXDESCPTR                     : 14;
};


 
struct __BLE_DEEPSLCNTL_REG
 
{
    WORD BITFLD_DEEP_SLEEP_IRQ_EN                    : 2;
    WORD BITFLD_DEEP_SLEEP_ON                        : 1;
    WORD BITFLD_DEEP_SLEEP_CORR_EN                   : 1;
    WORD BITFLD_SOFT_WAKEUP_REQ                      : 1;
    WORD                                             : 10;
    WORD BITFLD_DEEP_SLEEP_STAT                      : 1;
    WORD                                             : 15;
    WORD BITFLD_EXTWKUPDSB                           : 1;
};
#line 1944 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_DEEPSLWKUP_REG
 
{
    DWORD BITFLD_DEEPSLTIME                          : 32;
};


 
struct __BLE_DEEPSLSTAT_REG
 
{
    DWORD BITFLD_DEEPSLDUR                           : 32;
};


 
struct __BLE_ENBPRESET_REG
 
{
    WORD BITFLD_TWIRQ_RESET                          : 10;
    WORD BITFLD_TWIRQ_SET                            : 11;
    WORD BITFLD_TWEXT                                : 11;
};




 
struct __BLE_FINECNTCORR_REG
 
{
    WORD BITFLD_FINECNTCORR                          : 10;
};


 
struct __BLE_BASETIMECNTCORR_REG
 
{
    DWORD BITFLD_BASETIMECNTCORR                     : 27;
};


 
struct __BLE_DIAGCNTL_REG
 
{
    WORD BITFLD_DIAG0                                : 6;
    WORD                                             : 1;
    WORD BITFLD_DIAG0_EN                             : 1;
    WORD BITFLD_DIAG1                                : 6;
    WORD                                             : 1;
    WORD BITFLD_DIAG1_EN                             : 1;
    WORD BITFLD_DIAG2                                : 6;
    WORD                                             : 1;
    WORD BITFLD_DIAG2_EN                             : 1;
    WORD BITFLD_DIAG3                                : 6;
    WORD                                             : 1;
    WORD BITFLD_DIAG3_EN                             : 1;
};
#line 2014 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_DIAGSTAT_REG
 
{
    WORD BITFLD_DIAG0STAT                            : 8;
    WORD BITFLD_DIAG1STAT                            : 8;
    WORD BITFLD_DIAG2STAT                            : 8;
    WORD BITFLD_DIAG3STAT                            : 8;
};





 
struct __BLE_DEBUGADDMAX_REG
 
{
    WORD BITFLD_ADDMAX                               : 16;
};


 
struct __BLE_DEBUGADDMIN_REG
 
{
    WORD BITFLD_ADDMIN                               : 16;
};


 
struct __BLE_ERRORTYPESTAT_REG
 
{
    WORD BITFLD_TXCRYPT_ERROR                        : 1;
    WORD BITFLD_PKTCNTL_EMACC_ERROR                  : 1;
    WORD BITFLD_TXDESC_ERROR                         : 1;
    WORD BITFLD_APFM_ERROR                           : 1;
    WORD BITFLD_WHITELIST_ERROR                      : 1;
    WORD BITFLD_RXCRYPT_ERROR                        : 1;
    WORD BITFLD_IFS_UNDERRUN                         : 1;
    WORD BITFLD_LLCHMAP_ERROR                        : 1;
    WORD BITFLD_RADIO_EMACC_ERROR                    : 1;
    WORD                                             : 1;
    WORD BITFLD_CSTXPTR_ERROR                        : 1;
    WORD BITFLD_CSFORMAT_ERROR                       : 1;
};
#line 2073 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_SWPROFILING_REG
 
{
    DWORD BITFLD_SWPROFVAL                           : 32;
};


 
struct __BLE_RADIOCNTL0_REG
 
{
    DWORD                                            : 22;
    WORD BITFLD_DPCORR_EN                            : 1;
};


 
struct __BLE_RADIOCNTL1_REG
 
{
    WORD                                             : 16;
    WORD BITFLD_XRFSEL                               : 5;
};


 
struct __BLE_RADIOPWRUPDN_REG
 
{
    WORD BITFLD_TXPWRUP                              : 8;
    WORD BITFLD_TXPWRDN                              : 4;
    WORD                                             : 4;
    WORD BITFLD_RXPWRUP                              : 8;
    WORD BITFLD_RTRIP_DELAY                          : 7;
};





 
struct __BLE_ADVCHMAP_REG
 
{
    WORD BITFLD_ADVCHMAP                             : 3;
};


 
struct __BLE_ADVTIM_REG
 
{
    WORD BITFLD_ADVINT                               : 14;
};


 
struct __BLE_ACTSCANSTAT_REG
 
{
    WORD BITFLD_UPPERLIMIT                           : 9;
    WORD                                             : 7;
    WORD BITFLD_BACKOFF                              : 9;
};



 
struct __BLE_WLPUBADDPTR_REG
 
{
    WORD BITFLD_WLPUBADDPTR                          : 16;
};


 
struct __BLE_WLPRIVADDPTR_REG
 
{
    WORD BITFLD_WLPRIVADDPTR                         : 16;
};


 
struct __BLE_WLNBDEV_REG
 
{
    WORD BITFLD_NBPUBDEV                             : 8;
    WORD BITFLD_NBPRIVDEV                            : 8;
};



 
struct __BLE_AESCNTL_REG
 
{
    WORD BITFLD_AES_START                            : 1;
};


 
struct __BLE_AESKEY31_0_REG
 
{
    DWORD BITFLD_AESKEY31_0                          : 32;
};


 
struct __BLE_AESKEY63_32_REG
 
{
    DWORD BITFLD_AESKEY63_32                         : 32;
};


 
struct __BLE_AESKEY95_64_REG
 
{
    DWORD BITFLD_AESKEY95_64                         : 32;
};


 
struct __BLE_AESKEY127_96_REG
 
{
    DWORD BITFLD_AESKEY127_96                        : 32;
};


 
struct __BLE_AESPTR_REG
 
{
    WORD BITFLD_AESPTR                               : 16;
};


 
struct __BLE_TXMICVAL_REG
 
{
    DWORD BITFLD_TXMICVAL                            : 32;
};


 
struct __BLE_RXMICVAL_REG
 
{
    DWORD BITFLD_RXMICVAL                            : 32;
};


 
struct __BLE_RFTESTCNTL_REG
 
{
    WORD BITFLD_TXLENGTH                             : 9;
    WORD                                             : 3;
    WORD BITFLD_TXPLDSRC                             : 1;
    WORD BITFLD_PRBSTYPE                             : 1;
    WORD BITFLD_TXLENGTHSRC                          : 1;
    WORD BITFLD_INFINITETX                           : 1;
    WORD                                             : 15;
    WORD BITFLD_INFINITERX                           : 1;
};
#line 2251 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_TIMGENCNTL_REG
 
{
    WORD BITFLD_PREFTECH_TIME                        : 8;
};


 
struct __BLE_GROSSTIMTGT_REG
 
{
    WORD BITFLD_GROSSTARGET                          : 16;
};


 
struct __BLE_FINETIMTGT_REG
 
{
    DWORD BITFLD_FINETARGET                          : 27;
};


 
struct __BLE_SAMPLECLK_REG
 
{
    WORD BITFLD_SAMP                                 : 1;
};


 
struct __BLE_CNTL2_REG
 
{
    WORD BITFLD_EMACCERRSTAT                         : 1;
    WORD BITFLD_EMACCERRACK                          : 1;
    WORD BITFLD_EMACCERRMSK                          : 1;
    WORD BITFLD_DIAGPORT_SEL                         : 2;
    WORD BITFLD_DIAGPORT_REVERSE                     : 1;
    WORD BITFLD_BLE_CLK_STAT                         : 1;
    WORD BITFLD_MON_LP_CLK                           : 1;
    WORD BITFLD_RADIO_PWRDN_ALLOW                    : 1;
    WORD BITFLD_BLE_CLK_SEL                          : 6;
    WORD                                             : 2;
    WORD BITFLD_RADIO_ONLY                           : 1;
    WORD BITFLD_BB_ONLY                              : 1;
    WORD BITFLD_SW_RPL_SPI                           : 1;
    WORD BITFLD_WAKEUPLPSTAT                         : 1;
    WORD BITFLD_BLE_RSSI_SEL                         : 1;
};
#line 2318 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BLE_RF_DIAGIRQ_REG
 
{
    WORD BITFLD_DIAGIRQ_MASK_0                       : 1;
    WORD BITFLD_DIAGIRQ_WSEL_0                       : 2;
    WORD BITFLD_DIAGIRQ_BSEL_0                       : 3;
    WORD BITFLD_DIAGIRQ_EDGE_0                       : 1;
    WORD BITFLD_DIAGIRQ_STAT_0                       : 1;
    WORD BITFLD_DIAGIRQ_MASK_1                       : 1;
    WORD BITFLD_DIAGIRQ_WSEL_1                       : 2;
    WORD BITFLD_DIAGIRQ_BSEL_1                       : 3;
    WORD BITFLD_DIAGIRQ_EDGE_1                       : 1;
    WORD BITFLD_DIAGIRQ_STAT_1                       : 1;
    WORD BITFLD_DIAGIRQ_MASK_2                       : 1;
    WORD BITFLD_DIAGIRQ_WSEL_2                       : 2;
    WORD BITFLD_DIAGIRQ_BSEL_2                       : 3;
    WORD BITFLD_DIAGIRQ_EDGE_2                       : 1;
    WORD BITFLD_DIAGIRQ_STAT_2                       : 1;
    WORD BITFLD_DIAGIRQ_MASK_3                       : 1;
    WORD BITFLD_DIAGIRQ_WSEL_3                       : 2;
    WORD BITFLD_DIAGIRQ_BSEL_3                       : 3;
    WORD BITFLD_DIAGIRQ_EDGE_3                       : 1;
    WORD BITFLD_DIAGIRQ_STAT_3                       : 1;
};
#line 2364 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
 
 
#line 2382 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __CLK_AMBA_REG
 
{
    WORD BITFLD_HCLK_DIV                             : 2;
    WORD                                             : 2;
    WORD BITFLD_PCLK_DIV                             : 2;
    WORD                                             : 1;
    WORD BITFLD_OTP_ENABLE                           : 1;
};




 
struct __CLK_FREQ_TRIM_REG
 
{
    WORD BITFLD_FINE_ADJ                             : 8;
    WORD BITFLD_COARSE_ADJ                           : 3;
};



 
struct __CLK_PER_REG
 
{
    WORD BITFLD_TMR_DIV                              : 2;
    WORD                                             : 1;
    WORD BITFLD_TMR_ENABLE                           : 1;
    WORD BITFLD_WAKEUPCT_ENABLE                      : 1;
    WORD BITFLD_I2C_ENABLE                           : 1;
    WORD BITFLD_UART2_ENABLE                         : 1;
    WORD BITFLD_UART1_ENABLE                         : 1;
    WORD BITFLD_SPI_DIV                              : 2;
    WORD                                             : 1;
    WORD BITFLD_SPI_ENABLE                           : 1;
    WORD                                             : 2;
    WORD BITFLD_QUAD_ENABLE                          : 1;
};
#line 2433 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __CLK_RADIO_REG
 
{
    WORD BITFLD_RFCU_DIV                             : 2;
    WORD                                             : 1;
    WORD BITFLD_RFCU_ENABLE                          : 1;
    WORD BITFLD_BLE_DIV                              : 2;
    WORD BITFLD_BLE_LP_RESET                         : 1;
    WORD BITFLD_BLE_ENABLE                           : 1;
};






 
struct __CLK_CTRL_REG
 
{
    WORD BITFLD_SYS_CLK_SEL                          : 2;
    WORD BITFLD_XTAL16M_DISABLE                      : 1;
    WORD BITFLD_XTAL16M_SPIKE_FLT_DISABLE            : 1;
    WORD                                             : 1;
    WORD BITFLD_RUNNING_AT_32K                       : 1;
    WORD BITFLD_RUNNING_AT_RC16M                     : 1;
    WORD BITFLD_RUNNING_AT_XTAL16M                   : 1;
};
#line 2469 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __PMU_CTRL_REG
 
{
    WORD BITFLD_RESET_ON_WAKEUP                      : 1;
    WORD BITFLD_PERIPH_SLEEP                         : 1;
    WORD BITFLD_RADIO_SLEEP                          : 1;
    WORD BITFLD_OTP_COPY_DIV                         : 2;
    WORD BITFLD_FORCE_BUCK                           : 1;
    WORD BITFLD_FORCE_BOOST                          : 1;
    WORD BITFLD_RETENTION_MODE                       : 4;
};
#line 2489 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __SYS_CTRL_REG
 
{
    WORD BITFLD_REMAP_ADR0                           : 2;
    WORD BITFLD_RET_SYSRAM                           : 1;
    WORD BITFLD_CLK32_SOURCE                         : 1;
    WORD BITFLD_OTP_COPY                             : 1;
    WORD BITFLD_PAD_LATCH_EN                         : 1;
    WORD BITFLD_OTPC_RESET_REQ                       : 1;
    WORD BITFLD_DEBUGGER_ENABLE                      : 1;
    WORD                                             : 1;
    WORD BITFLD_TIMEOUT_DISABLE                      : 1;
    WORD BITFLD_SW_RESET                             : 1;
};
#line 2514 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __SYS_STAT_REG
 
{
    WORD BITFLD_RAD_IS_DOWN                          : 1;
    WORD BITFLD_RAD_IS_UP                            : 1;
    WORD BITFLD_PER_IS_DOWN                          : 1;
    WORD BITFLD_PER_IS_UP                            : 1;
    WORD BITFLD_DBG_IS_DOWN                          : 1;
    WORD BITFLD_DBG_IS_UP                            : 1;
    WORD BITFLD_XTAL16_TRIM_READY                    : 1;
    WORD BITFLD_XTAL16_SETTLED                       : 1;
};
#line 2536 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __TRIM_CTRL_REG
 
{
    WORD BITFLD_SETTLE_TIME                          : 4;
    WORD BITFLD_TRIM_TIME                            : 4;
};



 
struct __CLK_32K_REG
 
{
    WORD BITFLD_XTAL32K_ENABLE                       : 1;
    WORD BITFLD_XTAL32K_RBIAS                        : 2;
    WORD BITFLD_XTAL32K_CUR                          : 4;
    WORD BITFLD_RC32K_ENABLE                         : 1;
    WORD BITFLD_RC32K_TRIM                           : 4;
    WORD BITFLD_XTAL32K_DISABLE_AMPREG               : 1;
};
#line 2564 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __CLK_16M_REG
 
{
    WORD BITFLD_RC16M_ENABLE                         : 1;
    WORD BITFLD_RC16M_TRIM                           : 4;
    WORD BITFLD_XTAL16_CUR_SET                       : 3;
    WORD BITFLD_XTAL16_BIAS_SH_DISABLE               : 1;
    WORD BITFLD_XTAL16_NOISE_FILT_ENABLE             : 1;
};






 
struct __CLK_RCX20K_REG
 
{
    WORD BITFLD_RCX20K_TRIM                          : 4;
    WORD BITFLD_RCX20K_NTC                           : 4;
    WORD BITFLD_RCX20K_BIAS                          : 2;
    WORD BITFLD_RCX20K_LOWF                          : 1;
    WORD BITFLD_RCX20K_ENABLE                        : 1;
    WORD BITFLD_RCX20K_SELECT                        : 1;
};
#line 2598 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __BANDGAP_REG
 
{
    WORD BITFLD_BGR_TRIM                             : 5;
    WORD BITFLD_BGR_ITRIM                            : 5;
    WORD BITFLD_LDO_RET_TRIM                         : 4;
    WORD BITFLD_BGR_LOWPOWER                         : 1;
};





 
struct __ANA_STATUS_REG
 
{
    WORD BITFLD_VBAT1V_AVAILABLE                     : 1;
    WORD BITFLD_VBAT1V_OK                            : 1;
    WORD BITFLD_VDCDC_OK                             : 1;
    WORD BITFLD_LDO_OTP_OK                           : 1;
    WORD BITFLD_LDO_VDD_OK                           : 1;
    WORD BITFLD_LDO_ANA_OK                           : 1;
    WORD BITFLD_BOOST_VBAT_OK                        : 1;
    WORD BITFLD_BANDGAP_OK                           : 1;
    WORD                                             : 1;
    WORD BITFLD_BOOST_SELECTED                       : 1;
};
#line 2637 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
 
 
#line 2649 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __GP_ADC_CTRL_REG
 
{
    WORD BITFLD_GP_ADC_EN                            : 1;
    WORD BITFLD_GP_ADC_START                         : 1;
    WORD                                             : 1;
    WORD BITFLD_GP_ADC_CLK_SEL                       : 1;
    WORD BITFLD_GP_ADC_INT                           : 1;
    WORD BITFLD_GP_ADC_MINT                          : 1;
    WORD BITFLD_GP_ADC_SEL                           : 4;
    WORD BITFLD_GP_ADC_SIGN                          : 1;
    WORD BITFLD_GP_ADC_SE                            : 1;
    WORD BITFLD_GP_ADC_MUTE                          : 1;
    WORD BITFLD_GP_ADC_CHOP                          : 1;
    WORD BITFLD_GP_ADC_LDO_EN                        : 1;
    WORD BITFLD_GP_ADC_LDO_ZERO                      : 1;
};
#line 2680 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __GP_ADC_CTRL2_REG
 
{
    WORD BITFLD_GP_ADC_DELAY_EN                      : 1;
    WORD BITFLD_GP_ADC_ATTN3X                        : 1;
    WORD BITFLD_GP_ADC_IDYN                          : 1;
    WORD BITFLD_GP_ADC_I20U                          : 1;
};





 
struct __GP_ADC_OFFP_REG
 
{
    WORD BITFLD_GP_ADC_OFFP                          : 10;
};


 
struct __GP_ADC_OFFN_REG
 
{
    WORD BITFLD_GP_ADC_OFFN                          : 10;
};


 
struct __GP_ADC_CLEAR_INT_REG
 
{
    WORD BITFLD_GP_ADC_CLR_INT                       : 16;
};


 
struct __GP_ADC_RESULT_REG
 
{
    WORD BITFLD_GP_ADC_VAL                           : 10;
};


 
struct __GP_ADC_DELAY_REG
 
{
    WORD BITFLD_DEL_LDO_EN                           : 8;
    WORD BITFLD_DEL_LDO_ZERO                         : 8;
};



 
struct __GP_ADC_DELAY2_REG
 
{
    WORD BITFLD_DEL_ADC_EN                           : 8;
    WORD BITFLD_DEL_ADC_START                        : 8;
};



 
 
 
#line 2797 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __P0_DATA_REG
 
{
    WORD BITFLD_P0_DATA                              : 8;
};


 
struct __P0_SET_DATA_REG
 
{
    WORD BITFLD_P0_SET                               : 8;
};


 
struct __P0_RESET_DATA_REG
 
{
    WORD BITFLD_P0_RESET                             : 8;
};


 
struct __P00_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P01_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P02_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P03_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P04_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P05_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P06_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P07_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P1_DATA_REG
 
{
    WORD BITFLD_P1_DATA                              : 8;
};


 
struct __P1_SET_DATA_REG
 
{
    WORD BITFLD_P1_SET                               : 8;
};


 
struct __P1_RESET_DATA_REG
 
{
    WORD BITFLD_P1_RESET                             : 8;
};


 
struct __P10_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P11_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P12_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P13_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P14_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P15_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P2_DATA_REG
 
{
    WORD BITFLD_P2_DATA                              : 10;
};


 
struct __P2_SET_DATA_REG
 
{
    WORD BITFLD_P2_SET                               : 10;
};


 
struct __P2_RESET_DATA_REG
 
{
    WORD BITFLD_P2_RESET                             : 10;
};


 
struct __P20_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P21_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P22_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P23_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P24_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P25_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P26_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P27_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P28_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P29_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P01_PADPWR_CTRL_REG
 
{
    WORD BITFLD_P0_OUT_CTRL                          : 8;
    WORD BITFLD_P1_OUT_CTRL                          : 6;
};



 
struct __P2_PADPWR_CTRL_REG
 
{
    WORD BITFLD_P2_OUT_CTRL                          : 10;
};


 
struct __P3_PADPWR_CTRL_REG
 
{
    WORD BITFLD_P3_OUT_CTRL                          : 8;
};


 
struct __P3_DATA_REG
 
{
    WORD BITFLD_P3_DATA                              : 8;
};


 
struct __P3_SET_DATA_REG
 
{
    WORD BITFLD_P3_SET                               : 8;
};


 
struct __P3_RESET_DATA_REG
 
{
    WORD BITFLD_P3_RESET                             : 8;
};


 
struct __P30_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P31_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P32_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P33_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P34_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P35_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P36_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
struct __P37_MODE_REG
 
{
    WORD BITFLD_PID                                  : 5;
    WORD                                             : 3;
    WORD BITFLD_PUPD                                 : 2;
};



 
 
 






 
struct __SET_FREEZE_REG
 
{
    WORD BITFLD_FRZ_WKUPTIM                          : 1;
    WORD BITFLD_FRZ_SWTIM                            : 1;
    WORD BITFLD_FRZ_BLETIM                           : 1;
    WORD BITFLD_FRZ_WDOG                             : 1;
};





 
struct __RESET_FREEZE_REG
 
{
    WORD BITFLD_FRZ_WKUPTIM                          : 1;
    WORD BITFLD_FRZ_SWTIM                            : 1;
    WORD BITFLD_FRZ_BLETIM                           : 1;
    WORD BITFLD_FRZ_WDOG                             : 1;
};





 
struct __DEBUG_REG
 
{
    WORD BITFLD_DEBUGS_FREEZE_EN                     : 1;
};


 
struct __GP_STATUS_REG
 
{
    WORD BITFLD_CAL_PHASE                            : 1;
};


 
struct __GP_CONTROL_REG
 
{
    WORD BITFLD_BLE_WAKEUP_REQ                       : 1;
    WORD BITFLD_EM_MAP                               : 5;
};



 
 
 
#line 3372 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __I2C_CON_REG
 
{
    WORD BITFLD_I2C_MASTER_MODE                      : 1;
    WORD BITFLD_I2C_SPEED                            : 2;
    WORD BITFLD_I2C_10BITADDR_SLAVE                  : 1;
    WORD BITFLD_I2C_10BITADDR_MASTER                 : 1;
    WORD BITFLD_I2C_RESTART_EN                       : 1;
    WORD BITFLD_I2C_SLAVE_DISABLE                    : 1;
};
#line 3390 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __I2C_TAR_REG
 
{
    WORD BITFLD_IC_TAR                               : 10;
    WORD BITFLD_GC_OR_START                          : 1;
    WORD BITFLD_SPECIAL                              : 1;
};




 
struct __I2C_SAR_REG
 
{
    WORD BITFLD_IC_SAR                               : 10;
};


 
struct __I2C_DATA_CMD_REG
 
{
    WORD BITFLD_DAT                                  : 8;
    WORD BITFLD_CMD                                  : 1;
};



 
struct __I2C_SS_SCL_HCNT_REG
 
{
    WORD BITFLD_IC_SS_SCL_HCNT                       : 16;
};


 
struct __I2C_SS_SCL_LCNT_REG
 
{
    WORD BITFLD_IC_SS_SCL_LCNT                       : 16;
};


 
struct __I2C_FS_SCL_HCNT_REG
 
{
    WORD BITFLD_IC_FS_SCL_HCNT                       : 16;
};


 
struct __I2C_FS_SCL_LCNT_REG
 
{
    WORD BITFLD_IC_FS_SCL_LCNT                       : 16;
};


 
struct __I2C_INTR_STAT_REG
 
{
    WORD BITFLD_R_RX_UNDER                           : 1;
    WORD BITFLD_R_RX_OVER                            : 1;
    WORD BITFLD_R_RX_FULL                            : 1;
    WORD BITFLD_R_TX_OVER                            : 1;
    WORD BITFLD_R_TX_EMPTY                           : 1;
    WORD BITFLD_R_RD_REQ                             : 1;
    WORD BITFLD_R_TX_ABRT                            : 1;
    WORD BITFLD_R_RX_DONE                            : 1;
    WORD BITFLD_R_ACTIVITY                           : 1;
    WORD BITFLD_R_STOP_DET                           : 1;
    WORD BITFLD_R_START_DET                          : 1;
    WORD BITFLD_R_GEN_CALL                           : 1;
};
#line 3482 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __I2C_INTR_MASK_REG
 
{
    WORD BITFLD_M_RX_UNDER                           : 1;
    WORD BITFLD_M_RX_OVER                            : 1;
    WORD BITFLD_M_RX_FULL                            : 1;
    WORD BITFLD_M_TX_OVER                            : 1;
    WORD BITFLD_M_TX_EMPTY                           : 1;
    WORD BITFLD_M_RD_REQ                             : 1;
    WORD BITFLD_M_TX_ABRT                            : 1;
    WORD BITFLD_M_RX_DONE                            : 1;
    WORD BITFLD_M_ACTIVITY                           : 1;
    WORD BITFLD_M_STOP_DET                           : 1;
    WORD BITFLD_M_START_DET                          : 1;
    WORD BITFLD_M_GEN_CALL                           : 1;
};
#line 3512 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __I2C_RAW_INTR_STAT_REG
 
{
    WORD BITFLD_RX_UNDER                             : 1;
    WORD BITFLD_RX_OVER                              : 1;
    WORD BITFLD_RX_FULL                              : 1;
    WORD BITFLD_TX_OVER                              : 1;
    WORD BITFLD_TX_EMPTY                             : 1;
    WORD BITFLD_RD_REQ                               : 1;
    WORD BITFLD_TX_ABRT                              : 1;
    WORD BITFLD_RX_DONE                              : 1;
    WORD BITFLD_ACTIVITY                             : 1;
    WORD BITFLD_STOP_DET                             : 1;
    WORD BITFLD_START_DET                            : 1;
    WORD BITFLD_GEN_CALL                             : 1;
};
#line 3542 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __I2C_RX_TL_REG
 
{
    WORD BITFLD_RX_TL                                : 5;
};


 
struct __I2C_TX_TL_REG
 
{
    WORD BITFLD_RX_TL                                : 5;
};


 
struct __I2C_CLR_INTR_REG
 
{
    WORD BITFLD_CLR_INTR                             : 1;
};


 
struct __I2C_CLR_RX_UNDER_REG
 
{
    WORD BITFLD_CLR_RX_UNDER                         : 1;
};


 
struct __I2C_CLR_RX_OVER_REG
 
{
    WORD BITFLD_CLR_RX_OVER                          : 1;
};


 
struct __I2C_CLR_TX_OVER_REG
 
{
    WORD BITFLD_CLR_TX_OVER                          : 1;
};


 
struct __I2C_CLR_RD_REQ_REG
 
{
    WORD BITFLD_CLR_RD_REQ                           : 1;
};


 
struct __I2C_CLR_TX_ABRT_REG
 
{
    WORD BITFLD_CLR_TX_ABRT                          : 1;
};


 
struct __I2C_CLR_RX_DONE_REG
 
{
    WORD BITFLD_CLR_RX_DONE                          : 1;
};


 
struct __I2C_CLR_ACTIVITY_REG
 
{
    WORD BITFLD_CLR_ACTIVITY                         : 1;
};


 
struct __I2C_CLR_STOP_DET_REG
 
{
    WORD BITFLD_CLR_ACTIVITY                         : 1;
};


 
struct __I2C_CLR_START_DET_REG
 
{
    WORD BITFLD_CLR_START_DET                        : 1;
};


 
struct __I2C_CLR_GEN_CALL_REG
 
{
    WORD BITFLD_CLR_GEN_CALL                         : 1;
};


 
struct __I2C_ENABLE_REG
 
{
    WORD BITFLD_CTRL_ENABLE                          : 1;
};


 
struct __I2C_STATUS_REG
 
{
    WORD BITFLD_I2C_ACTIVITY                         : 1;
    WORD BITFLD_TFNF                                 : 1;
    WORD BITFLD_TFE                                  : 1;
    WORD BITFLD_RFNE                                 : 1;
    WORD BITFLD_RFF                                  : 1;
    WORD BITFLD_MST_ACTIVITY                         : 1;
    WORD BITFLD_SLV_ACTIVITY                         : 1;
};
#line 3674 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __I2C_TXFLR_REG
 
{
    WORD BITFLD_TXFLR                                : 6;
};


 
struct __I2C_RXFLR_REG
 
{
    WORD BITFLD_RXFLR                                : 6;
};


 
struct __I2C_SDA_HOLD_REG
 
{
    WORD BITFLD_IC_SDA_HOLD                          : 16;
};


 
struct __I2C_TX_ABRT_SOURCE_REG
 
{
    WORD BITFLD_ABRT_7B_ADDR_NOACK                   : 1;
    WORD BITFLD_ABRT_10ADDR1_NOACK                   : 1;
    WORD BITFLD_ABRT_10ADDR2_NOACK                   : 1;
    WORD BITFLD_ABRT_TXDATA_NOACK                    : 1;
    WORD BITFLD_ABRT_GCALL_NOACK                     : 1;
    WORD BITFLD_ABRT_GCALL_READ                      : 1;
    WORD BITFLD_ABRT_HS_ACKDET                       : 1;
    WORD BITFLD_ABRT_SBYTE_ACKDET                    : 1;
    WORD BITFLD_ABRT_HS_NORSTRT                      : 1;
    WORD BITFLD_ABRT_SBYTE_NORSTRT                   : 1;
    WORD BITFLD_ABRT_10B_RD_NORSTRT                  : 1;
    WORD BITFLD_ABRT_MASTER_DIS                      : 1;
    WORD BITFLD_ARB_LOST                             : 1;
    WORD BITFLD_ABRT_SLVFLUSH_TXFIFO                 : 1;
    WORD BITFLD_ABRT_SLV_ARBLOST                     : 1;
    WORD BITFLD_ABRT_SLVRD_INTX                      : 1;
};
#line 3736 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __I2C_SDA_SETUP_REG
 
{
    WORD BITFLD_SDA_SETUP                            : 8;
};


 
struct __I2C_ACK_GENERAL_CALL_REG
 
{
    WORD BITFLD_ACK_GEN_CALL                         : 1;
};


 
struct __I2C_ENABLE_STATUS_REG
 
{
    WORD BITFLD_IC_EN                                : 1;
    WORD BITFLD_SLV_DISABLED_WHILE_BUSY              : 1;
    WORD BITFLD_SLV_RX_DATA_LOST                     : 1;
};




 
struct __I2C_IC_FS_SPKLEN_REG
 
{
    WORD BITFLD_IC_FS_SPKLEN                         : 8;
};


 
 
 
#line 3786 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __GPIO_IRQ0_IN_SEL_REG
 
{
    WORD BITFLD_KBRD_IRQ0_SEL                        : 6;
};


 
struct __GPIO_IRQ1_IN_SEL_REG
 
{
    WORD BITFLD_KBRD_IRQ1_SEL                        : 6;
};


 
struct __GPIO_IRQ2_IN_SEL_REG
 
{
    WORD BITFLD_KBRD_IRQ2_SEL                        : 6;
};


 
struct __GPIO_IRQ3_IN_SEL_REG
 
{
    WORD BITFLD_KBRD_IRQ3_SEL                        : 6;
};


 
struct __GPIO_IRQ4_IN_SEL_REG
 
{
    WORD BITFLD_KBRD_IRQ4_SEL                        : 6;
};




 
struct __GPIO_DEBOUNCE_REG
 
{
    WORD BITFLD_DEB_VALUE                            : 6;
    WORD                                             : 2;
    WORD BITFLD_DEB_ENABLE0                          : 1;
    WORD BITFLD_DEB_ENABLE1                          : 1;
    WORD BITFLD_DEB_ENABLE2                          : 1;
    WORD BITFLD_DEB_ENABLE3                          : 1;
    WORD BITFLD_DEB_ENABLE4                          : 1;
    WORD BITFLD_DEB_ENABLE_KBRD                      : 1;
};
#line 3849 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __GPIO_RESET_IRQ_REG
 
{
    WORD BITFLD_RESET_GPIO0_IRQ                      : 1;
    WORD BITFLD_RESET_GPIO1_IRQ                      : 1;
    WORD BITFLD_RESET_GPIO2_IRQ                      : 1;
    WORD BITFLD_RESET_GPIO3_IRQ                      : 1;
    WORD BITFLD_RESET_GPIO4_IRQ                      : 1;
    WORD BITFLD_RESET_KBRD_IRQ                       : 1;
};
#line 3867 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __GPIO_INT_LEVEL_CTRL_REG
 
{
    WORD BITFLD_INPUT_LEVEL0                         : 1;
    WORD BITFLD_INPUT_LEVEL1                         : 1;
    WORD BITFLD_INPUT_LEVEL2                         : 1;
    WORD BITFLD_INPUT_LEVEL3                         : 1;
    WORD BITFLD_INPUT_LEVEL4                         : 1;
    WORD                                             : 2;
    WORD BITFLD_EDGE_LEVELn0                         : 1;
    WORD BITFLD_EDGE_LEVELn1                         : 1;
    WORD BITFLD_EDGE_LEVELn2                         : 1;
    WORD BITFLD_EDGE_LEVELn3                         : 1;
    WORD BITFLD_EDGE_LEVELn4                         : 1;
};
#line 3894 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __KBRD_IRQ_IN_SEL0_REG
 
{
    WORD BITFLD_KBRD_P00_EN                          : 1;
    WORD BITFLD_KBRD_P01_EN                          : 1;
    WORD BITFLD_KBRD_P02_EN                          : 1;
    WORD BITFLD_KBRD_P03_EN                          : 1;
    WORD BITFLD_KBRD_P04_EN                          : 1;
    WORD BITFLD_KBRD_P05_EN                          : 1;
    WORD BITFLD_KBRD_P06_EN                          : 1;
    WORD BITFLD_KBRD_P07_EN                          : 1;
    WORD BITFLD_KEY_REPEAT                           : 6;
    WORD BITFLD_KBRD_LEVEL                           : 1;
    WORD BITFLD_KBRD_REL                             : 1;
};
#line 3922 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __KBRD_IRQ_IN_SEL1_REG
 
{
    WORD BITFLD_KBRD_P20_EN                          : 1;
    WORD BITFLD_KBRD_P21_EN                          : 1;
    WORD BITFLD_KBRD_P22_EN                          : 1;
    WORD BITFLD_KBRD_P23_EN                          : 1;
    WORD BITFLD_KBRD_P24_EN                          : 1;
    WORD BITFLD_KBRD_P25_EN                          : 1;
    WORD BITFLD_KBRD_P26_EN                          : 1;
    WORD BITFLD_KBRD_P27_EN                          : 1;
    WORD BITFLD_KBRD_P28_EN                          : 1;
    WORD BITFLD_KBRD_P29_EN                          : 1;
    WORD BITFLD_KBRD_P10_EN                          : 1;
    WORD BITFLD_KBRD_P11_EN                          : 1;
    WORD BITFLD_KBRD_P12_EN                          : 1;
    WORD BITFLD_KBRD_P13_EN                          : 1;
    WORD BITFLD_KBRD_P14_EN                          : 1;
    WORD BITFLD_KBRD_P15_EN                          : 1;
};
#line 3960 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
 
 
#line 3972 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __OTPC_MODE_REG
 
{
    WORD BITFLD_OTPC_MODE_MODE                       : 3;
    WORD                                             : 1;
    WORD BITFLD_OTPC_MODE_USE_DMA                    : 1;
    WORD BITFLD_OTPC_MODE_FIFO_FLUSH                 : 1;
    WORD BITFLD_OTPC_MODE_TWO_CC_ACC                 : 1;
    WORD BITFLD_OTPC_MODE_PRG_PORT_SEL               : 1;
    WORD BITFLD_OPTC_MODE_PRG_FAST                   : 1;
    DWORD                                            : 19;
    WORD BITFLD_OTPC_MODE_PRG_PORT_MUX               : 2;
};
#line 3994 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __OTPC_PCTRL_REG
 
{
    WORD BITFLD_OTPC_PCTRL_WADDR                     : 13;
    WORD                                             : 3;
    WORD BITFLD_OTPC_PCTRL_BADRL                     : 3;
    WORD BITFLD_OTPC_PCTRL_BSELL                     : 1;
    WORD BITFLD_OTPC_PCTRL_BADRU                     : 3;
    WORD BITFLD_OTPC_PCTRL_BSELU                     : 1;
    WORD BITFLD_OTPC_PCTRL_BITL                      : 1;
    WORD BITFLD_OTPC_PCTRL_ENL                       : 1;
    WORD BITFLD_OTPC_PCTRL_BITU                      : 1;
    WORD BITFLD_OTPC_PCTRL_ENU                       : 1;
};
#line 4019 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __OTPC_STAT_REG
 
{
    WORD BITFLD_OTPC_STAT_PRDY                       : 1;
    WORD BITFLD_OTPC_STAT_PERROR                     : 1;
    WORD BITFLD_OTPC_STAT_TRDY                       : 1;
    WORD BITFLD_OTPC_STAT_TERROR                     : 1;
    WORD BITFLD_OTPC_STAT_ARDY                       : 1;
    WORD                                             : 3;
    WORD BITFLD_OTPC_STAT_FWORDS                     : 4;
    WORD BITFLD_OTPC_STAT_PERR_L                     : 1;
    WORD BITFLD_OTPC_STAT_PERR_U                     : 1;
    WORD BITFLD_OTPC_STAT_TERR_L                     : 1;
    WORD BITFLD_OTPC_STAT_TERR_U                     : 1;
    WORD BITFLD_OTPC_STAT_NWORDS                     : 13;
};
#line 4048 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __OTPC_AHBADR_REG
 
{
    WORD                                             : 2;
    DWORD BITFLD_OTPC_AHBADR                         : 30;
};


 
struct __OTPC_CELADR_REG
 
{
    WORD BITFLD_OTPC_CELADR                          : 13;
};


 
struct __OTPC_NWORDS_REG
 
{
    WORD BITFLD_OTPC_NWORDS                          : 13;
};


 
struct __OTPC_FFPRT_REG
 
{
    DWORD BITFLD_OTPC_FFPRT                          : 32;
};


 
struct __OTPC_FFRD_REG
 
{
    DWORD BITFLD_OTPC_FFRD                           : 32;
};


 
 
 
#line 4112 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __PATCH_VALID_REG
 
{
    WORD BITFLD_PATCH_VALID                          : 8;
};


 
struct __PATCH_VALID_SET_REG
 
{
    WORD BITFLD_PATCH_VALID_SET                      : 8;
};


 
struct __PATCH_VALID_RESET_REG
 
{
    WORD BITFLD_PATCH_VALID_RESET                    : 8;
};


 
struct __PATCH_ADDR0_REG
 
{
    DWORD BITFLD_PATCH_ADDR                          : 32;
};


 
struct __PATCH_DATA0_REG
 
{
    DWORD BITFLD_PATCH_DATA                          : 32;
};


 
struct __PATCH_ADDR1_REG
 
{
    DWORD BITFLD_PATCH_ADDR                          : 32;
};


 
struct __PATCH_DATA1_REG
 
{
    DWORD BITFLD_PATCH_DATA                          : 32;
};


 
struct __PATCH_ADDR2_REG
 
{
    DWORD BITFLD_PATCH_ADDR                          : 32;
};


 
struct __PATCH_DATA2_REG
 
{
    DWORD BITFLD_PATCH_DATA                          : 32;
};


 
struct __PATCH_ADDR3_REG
 
{
    DWORD BITFLD_PATCH_ADDR                          : 32;
};


 
struct __PATCH_DATA3_REG
 
{
    DWORD BITFLD_PATCH_DATA                          : 32;
};


 
struct __PATCH_ADDR4_REG
 
{
    DWORD BITFLD_PATCH_ADDR                          : 32;
};


 
struct __PATCH_DATA4_REG
 
{
    DWORD BITFLD_PATCH_DATA                          : 32;
};


 
struct __PATCH_ADDR5_REG
 
{
    DWORD BITFLD_PATCH_ADDR                          : 32;
};


 
struct __PATCH_DATA5_REG
 
{
    DWORD BITFLD_PATCH_DATA                          : 32;
};


 
struct __PATCH_ADDR6_REG
 
{
    DWORD BITFLD_PATCH_ADDR                          : 32;
};


 
struct __PATCH_DATA6_REG
 
{
    DWORD BITFLD_PATCH_DATA                          : 32;
};


 
struct __PATCH_ADDR7_REG
 
{
    DWORD BITFLD_PATCH_ADDR                          : 32;
};


 
struct __PATCH_DATA7_REG
 
{
    DWORD BITFLD_PATCH_DATA                          : 32;
};


 
 
 
#line 4274 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __QDEC_CTRL_REG
 
{
    WORD BITFLD_QD_IRQ_MASK                          : 1;
    WORD BITFLD_QD_IRQ_CLR                           : 1;
    WORD BITFLD_QD_IRQ_STATUS                        : 1;
    WORD BITFLD_QD_IRQ_THRES                         : 7;
};





 
struct __QDEC_XCNT_REG
 
{
    WORD BITFLD_X_counter                            : 16;
};


 
struct __QDEC_YCNT_REG
 
{
    WORD BITFLD_Y_counter                            : 16;
};


 
struct __QDEC_CLOCKDIV_REG
 
{
    WORD BITFLD_clock_divider                        : 10;
};


 
struct __QDEC_CTRL2_REG
 
{
    WORD BITFLD_CHX_PORT_SEL                         : 4;
    WORD BITFLD_CHY_PORT_SEL                         : 4;
    WORD BITFLD_CHZ_PORT_SEL                         : 4;
};




 
struct __QDEC_ZCNT_REG
 
{
    WORD BITFLD_Z_counter                            : 16;
};


 
 
 






 
struct __SPI_CTRL_REG
 
{
    WORD BITFLD_SPI_ON                               : 1;
    WORD BITFLD_SPI_PHA                              : 1;
    WORD BITFLD_SPI_POL                              : 1;
    WORD BITFLD_SPI_CLK                              : 2;
    WORD BITFLD_SPI_DO                               : 1;
    WORD BITFLD_SPI_SMN                              : 1;
    WORD BITFLD_SPI_WORD                             : 2;
    WORD BITFLD_SPI_RST                              : 1;
    WORD BITFLD_SPI_FORCE_DO                         : 1;
    WORD BITFLD_SPI_TXH                              : 1;
    WORD BITFLD_SPI_DI                               : 1;
    WORD BITFLD_SPI_INT_BIT                          : 1;
    WORD BITFLD_SPI_MINT                             : 1;
    WORD BITFLD_SPI_EN_CTRL                          : 1;
};
#line 4375 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __SPI_RX_TX_REG0
 
{
    WORD BITFLD_SPI_DATA0                            : 16;
};


 
struct __SPI_RX_TX_REG1
 
{
    WORD BITFLD_SPI_DATA1                            : 16;
};


 
struct __SPI_CLEAR_INT_REG
 
{
    WORD BITFLD_SPI_CLEAR_INT                        : 16;
};


 
struct __SPI_CTRL_REG1
 
{
    WORD BITFLD_SPI_FIFO_MODE                        : 2;
    WORD BITFLD_SPI_PRIORITY                         : 1;
    WORD BITFLD_SPI_BUSY                             : 1;
    WORD BITFLD_SPI_9BIT_VAL                         : 1;
};





 
 
 
#line 4426 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __TIMER0_CTRL_REG
 
{
    WORD BITFLD_TIM0_CTRL                            : 1;
    WORD BITFLD_TIM0_CLK_SEL                         : 1;
    WORD BITFLD_TIM0_CLK_DIV                         : 1;
    WORD BITFLD_PWM_MODE                             : 1;
};





 
struct __TIMER0_ON_REG
 
{
    WORD BITFLD_TIM0_ON                              : 16;
};


 
struct __TIMER0_RELOAD_M_REG
 
{
    WORD BITFLD_TIM0_M                               : 16;
};


 
struct __TIMER0_RELOAD_N_REG
 
{
    WORD BITFLD_TIM0_N                               : 16;
};


 
struct __PWM2_DUTY_CYCLE
 
{
    WORD BITFLD_DUTY_CYCLE                           : 14;
};


 
struct __PWM3_DUTY_CYCLE
 
{
    WORD BITFLD_DUTY_CYCLE                           : 14;
};


 
struct __PWM4_DUTY_CYCLE
 
{
    WORD BITFLD_DUTY_CYCLE                           : 14;
};


 
struct __TRIPLE_PWM_FREQUENCY
 
{
    WORD BITFLD_FREQ                                 : 14;
};


 
struct __TRIPLE_PWM_CTRL_REG
 
{
    WORD BITFLD_TRIPLE_PWM_ENABLE                    : 1;
    WORD BITFLD_SW_PAUSE_EN                          : 1;
    WORD BITFLD_HW_PAUSE_EN                          : 1;
};




 
 
 
#line 4592 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __UART_RBR_THR_DLL_REG
 
{
    WORD BITFLD_RBR_THR_DLL                          : 8;
};


 
struct __UART_IER_DLH_REG
 
{
    WORD BITFLD_ERBFI_dlh0                           : 1;
    WORD BITFLD_ETBEI_dlh1                           : 1;
    WORD BITFLD_ELSI_dhl2                            : 1;
    WORD BITFLD_EDSSI_dlh3                           : 1;
    WORD                                             : 3;
    WORD BITFLD_PTIME_dlh7                           : 1;
};






 
struct __UART_IIR_FCR_REG
 
{
    DWORD BITFLD_IIR_FCR                             : 32;
};


 
struct __UART_LCR_REG
 
{
    WORD BITFLD_UART_DLS                             : 2;
    WORD BITFLD_UART_STOP                            : 1;
    WORD BITFLD_UART_PEN                             : 1;
    WORD BITFLD_UART_EPS                             : 1;
    WORD                                             : 1;
    WORD BITFLD_UART_BC                              : 1;
    WORD BITFLD_UART_DLAB                            : 1;
};
#line 4644 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __UART_MCR_REG
 
{
    WORD                                             : 1;
    WORD BITFLD_UART_RTS                             : 1;
    WORD BITFLD_UART_OUT1                            : 1;
    WORD BITFLD_UART_OUT2                            : 1;
    WORD BITFLD_UART_LB                              : 1;
    WORD BITFLD_UART_AFCE                            : 1;
    WORD BITFLD_UART_SIRE                            : 1;
};
#line 4663 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __UART_LSR_REG
 
{
    WORD BITFLD_UART_DR                              : 1;
    WORD BITFLD_UART_OE                              : 1;
    WORD BITFLD_UART_PE                              : 1;
    WORD BITFLD_UART_FE                              : 1;
    WORD BITFLD_UART_B1                              : 1;
    WORD BITFLD_UART_THRE                            : 1;
    WORD BITFLD_UART_TEMT                            : 1;
    WORD BITFLD_UART_RFE                             : 1;
};
#line 4685 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __UART_MSR_REG
 
{
    WORD BITFLD_UART_DCTS                            : 1;
    WORD                                             : 1;
    WORD BITFLD_UART_TERI                            : 1;
    WORD BITFLD_UART_DDCD                            : 1;
    WORD BITFLD_UART_CTS                             : 1;
    WORD                                             : 1;
    WORD BITFLD_UART_R1                              : 1;
    WORD BITFLD_UART_DCD                             : 1;
};
#line 4705 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __UART_SCR_REG
 
{
    WORD BITFLD_UART_SCRATCH_PAD                     : 8;
};


 
struct __UART_LPDLL_REG
 
{
    WORD BITFLD_UART_LPDLL                           : 8;
};


 
struct __UART_LPDLH_REG
 
{
    WORD BITFLD_UART_LPDLH                           : 8;
};


 
struct __UART_SRBR_STHR0_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR1_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR2_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR3_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR4_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR5_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR6_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR7_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR8_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR9_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR10_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR11_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR12_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR13_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR14_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_SRBR_STHR15_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};


 
struct __UART_USR_REG
 
{
    WORD                                             : 1;
    WORD BITFLD_UART_TFNF                            : 1;
    WORD BITFLD_UART_TFE                             : 1;
    WORD BITFLD_UART_RFNE                            : 1;
    WORD BITFLD_UART_RFF                             : 1;
};





 
struct __UART_TFL_REG
 
{
    WORD BITFLD_UART_TRANSMIT_FIFO_LEVEL             : 16;
};


 
struct __UART_RFL_REG
 
{
    WORD BITFLD_UART_RECEIVE_FIFO_LEVEL              : 16;
};


 
struct __UART_SRR_REG
 
{
    WORD BITFLD_UART_UR                              : 1;
    WORD BITFLD_UART_RFR                             : 1;
    WORD BITFLD_UART_XFR                             : 1;
};




 
struct __UART_SRTS_REG
 
{
    WORD BITFLD_UART_SHADOW_REQUEST_TO_SEND          : 1;
};


 
struct __UART_SBCR_REG
 
{
    WORD BITFLD_UART_SHADOW_BREAK_CONTROL            : 1;
};


 
struct __UART_SDMAM_REG
 
{
    WORD BITFLD_UART_SHADOW_DMA_MODE                 : 1;
};


 
struct __UART_SFE_REG
 
{
    WORD BITFLD_UART_SHADOW_FIFO_ENABLE              : 1;
};


 
struct __UART_SRT_REG
 
{
    WORD BITFLD_UART_SHADOW_RCVR_TRIGGER             : 2;
};


 
struct __UART_STET_REG
 
{
    WORD BITFLD_UART_SHADOW_TX_EMPTY_TRIGGER         : 2;
};


 
struct __UART_HTX_REG
 
{
    WORD BITFLD_UART_HALT_TX                         : 1;
};


 
struct __UART_CPR_REG
 
{
    DWORD BITFLD_CPR                                 : 32;
};


 
struct __UART_UCV_REG
 
{
    DWORD BITFLD_UCV                                 : 32;
};


 
struct __UART_CTR_REG
 
{
    DWORD BITFLD_CTR                                 : 32;
};


 
struct __UART2_RBR_THR_DLL_REG
 
{
    WORD BITFLD_RBR_THR_DLL                          : 8;
};
    
    

 
struct __UART2_IER_DLH_REG
 
{
    WORD BITFLD_ERBFI_dlh0                           : 1;
    WORD BITFLD_ETBEI_dlh1                           : 1;
    WORD BITFLD_ELSI_dhl2                            : 1;
    WORD BITFLD_EDSSI_dlh3                           : 1;
    WORD                                             : 3;
    WORD BITFLD_PTIME_dlh7                           : 1;
};
    
    
    
    
    
    

 
struct __UART2_IIR_FCR_REG
 
{
    DWORD BITFLD_IIR_FCR                             : 32;
};
    
    

 
struct __UART2_LCR_REG
 
{
    WORD BITFLD_UART_DLS                             : 2;
    WORD BITFLD_UART_STOP                            : 1;
    WORD BITFLD_UART_PEN                             : 1;
    WORD BITFLD_UART_EPS                             : 1;
    WORD                                             : 1;
    WORD BITFLD_UART_BC                              : 1;
    WORD BITFLD_UART_DLAB                            : 1;
};
    
    
    
    
    
    
    

 
struct __UART2_MCR_REG
 
{
    WORD                                             : 1;
    WORD BITFLD_UART_RTS                             : 1;
    WORD BITFLD_UART_OUT1                            : 1;
    WORD BITFLD_UART_OUT2                            : 1;
    WORD BITFLD_UART_LB                              : 1;
    WORD BITFLD_UART_AFCE                            : 1;
    WORD BITFLD_UART_SIRE                            : 1;
};
    
    
    
    
    
    
    

 
struct __UART2_LSR_REG
 
{
    WORD BITFLD_UART_DR                              : 1;
    WORD BITFLD_UART_OE                              : 1;
    WORD BITFLD_UART_PE                              : 1;
    WORD BITFLD_UART_FE                              : 1;
    WORD BITFLD_UART_B1                              : 1;
    WORD BITFLD_UART_THRE                            : 1;
    WORD BITFLD_UART_TEMT                            : 1;
    WORD BITFLD_UART_RFE                             : 1;
};
    
    
    
    
    
    
    
    
    

 
struct __UART2_MSR_REG
 
{
    WORD BITFLD_UART_DCTS                            : 1;
    WORD                                             : 1;
    WORD BITFLD_UART_TERI                            : 1;
    WORD BITFLD_UART_DDCD                            : 1;
    WORD BITFLD_UART_CTS                             : 1;
    WORD                                             : 1;
    WORD BITFLD_UART_R1                              : 1;
    WORD BITFLD_UART_DCD                             : 1;
};
    
    
    
    
    
    
    

 
struct __UART2_SCR_REG
 
{
    WORD BITFLD_UART_SCRATCH_PAD                     : 8;
};
    
    

 
struct __UART2_LPDLL_REG
 
{
    WORD BITFLD_UART_LPDLL                           : 8;
};
    
    

 
struct __UART2_LPDLH_REG
 
{
    WORD BITFLD_UART_LPDLH                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR0_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR1_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR2_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR3_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR4_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR5_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR6_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR7_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR8_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR9_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR10_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR11_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR12_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR13_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR14_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_SRBR_STHR15_REG
 
{
    WORD BITFLD_SRBR_STHRx                           : 8;
};
    
    

 
struct __UART2_USR_REG
 
{
    WORD                                             : 1;
    WORD BITFLD_UART_TFNF                            : 1;
    WORD BITFLD_UART_TFE                             : 1;
    WORD BITFLD_UART_RFNE                            : 1;
    WORD BITFLD_UART_RFF                             : 1;
};
    
    
    
    
    

 
struct __UART2_TFL_REG
 
{
    WORD BITFLD_UART_TRANSMIT_FIFO_LEVEL             : 16;
};
    
    

 
struct __UART2_RFL_REG
 
{
    WORD BITFLD_UART_RECEIVE_FIFO_LEVEL              : 16;
};
    
    

 
struct __UART2_SRR_REG
 
{
    WORD BITFLD_UART_UR                              : 1;
    WORD BITFLD_UART_RFR                             : 1;
    WORD BITFLD_UART_XFR                             : 1;
};
    
    
    
    

 
struct __UART2_SRTS_REG
 
{
    WORD BITFLD_UART_SHADOW_REQUEST_TO_SEND          : 1;
};
    
    

 
struct __UART2_SBCR_REG
 
{
    WORD BITFLD_UART_SHADOW_BREAK_CONTROL            : 1;
};
    
    

 
struct __UART2_SDMAM_REG
 
{
    WORD BITFLD_UART_SHADOW_DMA_MODE                 : 1;
};
    
    

 
struct __UART2_SFE_REG
 
{
    WORD BITFLD_UART_SHADOW_FIFO_ENABLE              : 1;
};
    
    

 
struct __UART2_SRT_REG
 
{
    WORD BITFLD_UART_SHADOW_RCVR_TRIGGER             : 2;
};
    
    

 
struct __UART2_STET_REG
 
{
    WORD BITFLD_UART_SHADOW_TX_EMPTY_TRIGGER         : 2;
};
    
    

 
struct __UART2_HTX_REG
 
{
    WORD BITFLD_UART_HALT_TX                         : 1;
};
    
    

 
struct __UART2_CPR_REG
 
{
    DWORD BITFLD_CPR                                 : 32;
};
    
    

 
struct __UART2_UCV_REG
 
{
    DWORD BITFLD_UCV                                 : 32;
};
    
    

 
struct __UART2_CTR_REG
 
{
    DWORD BITFLD_CTR                                 : 32;
};
    
    

 
 
 
#line 5420 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __CHIP_ID1_REG
 
{
    WORD BITFLD_CHIP_ID1                             : 8;
};


 
struct __CHIP_ID2_REG
 
{
    WORD BITFLD_CHIP_ID2                             : 8;
};


 
struct __CHIP_ID3_REG
 
{
    WORD BITFLD_CHIP_ID3                             : 8;
};


 
struct __CHIP_SWC_REG
 
{
    WORD BITFLD_CHIP_SWC                             : 4;
};


 
struct __CHIP_REVISION_REG
 
{
    WORD BITFLD_REVISION_ID                          : 8;
};


 
struct __CHIP_CONFIG1_REG
 
{
    WORD BITFLD_CHIP_CONFIG1                         : 8;
};


 
struct __CHIP_CONFIG2_REG
 
{
    WORD BITFLD_CHIP_CONFIG2                         : 8;
};


 
struct __CHIP_CONFIG3_REG
 
{
    WORD BITFLD_CHIP_CONFIG3                         : 8;
};


 
 
 
#line 5501 "..\\..\\sdk\\platform\\include\\datasheet.h"

 
struct __WKUP_CTRL_REG
 
{
    WORD BITFLD_WKUP_DEB_VALUE                       : 6;
    WORD BITFLD_WKUP_SFT_KEYHIT                      : 1;
    WORD BITFLD_WKUP_ENABLE_IRQ                      : 1;
};




 
struct __WKUP_COMPARE_REG
 
{
    WORD BITFLD_COMPARE                              : 8;
};


 
struct __WKUP_RESET_IRQ_REG
 
{
    WORD BITFLD_WKUP_IRQ_RST                         : 16;
};


 
struct __WKUP_COUNTER_REG
 
{
    WORD BITFLD_EVENT_VALUE                          : 8;
};


 
struct __WKUP_RESET_CNTR_REG
 
{
    WORD BITFLD_WKUP_CNTR_RST                        : 16;
};


 
struct __WKUP_SELECT_P0_REG
 
{
    WORD BITFLD_WKUP_SELECT_P0                       : 8;
};


 
struct __WKUP_SELECT_P1_REG
 
{
    WORD BITFLD_WKUP_SELECT_P1                       : 6;
};


 
struct __WKUP_SELECT_P2_REG
 
{
    WORD BITFLD_WKUP_SELECT_P2                       : 10;
};


 
struct __WKUP_SELECT_P3_REG
 
{
    WORD BITFLD_WKUP_SELECT_P3                       : 8;
};


 
struct __WKUP_POL_P0_REG
 
{
    WORD BITFLD_WKUP_POL_P0                          : 8;
};


 
struct __WKUP_POL_P1_REG
 
{
    WORD BITFLD_WKUP_POL_P1                          : 6;
};


 
struct __WKUP_POL_P2_REG
 
{
    WORD BITFLD_WKUP_POL_P2                          : 10;
};


 
struct __WKUP_POL_P3_REG
 
{
    WORD BITFLD_WKUP_POL_P3                          : 8;
};


 
 
 



 
struct __WATCHDOG_REG
 
{
    WORD BITFLD_WDOG_VAL                             : 8;
    WORD BITFLD_WDOG_VAL_NEG                         : 1;
    WORD BITFLD_WDOG_WEN                             : 7;
};




 
struct __WATCHDOG_CTRL_REG
 
{
    WORD BITFLD_NMI_RST                              : 1;
};


  
  
  


  
  
  
     
     
     






#line 5661 "..\\..\\sdk\\platform\\include\\datasheet.h"











     






    




  
  
  


#line 91 "..\\..\\sdk\\platform\\include\\global_io.h"

#line 19 "src\\timer.c"
#line 1 ".\\includes\\timer.h"















 
 

#line 1 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"
 
 





 









     
#line 27 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"
     











#line 46 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"





 

     

     
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       __int64 int64_t;

     
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __int64 uint64_t;

     

     
     
typedef   signed          char int_least8_t;
typedef   signed short     int int_least16_t;
typedef   signed           int int_least32_t;
typedef   signed       __int64 int_least64_t;

     
typedef unsigned          char uint_least8_t;
typedef unsigned short     int uint_least16_t;
typedef unsigned           int uint_least32_t;
typedef unsigned       __int64 uint_least64_t;

     

     
typedef   signed           int int_fast8_t;
typedef   signed           int int_fast16_t;
typedef   signed           int int_fast32_t;
typedef   signed       __int64 int_fast64_t;

     
typedef unsigned           int uint_fast8_t;
typedef unsigned           int uint_fast16_t;
typedef unsigned           int uint_fast32_t;
typedef unsigned       __int64 uint_fast64_t;

     




typedef   signed           int intptr_t;
typedef unsigned           int uintptr_t;


     
typedef   signed     long long intmax_t;
typedef unsigned     long long uintmax_t;




     

     





     





     





     

     





     





     





     

     





     





     





     

     






     






     






     

     


     


     


     

     
#line 216 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"

     



     






     
    
 



#line 241 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"

     







     










     











#line 305 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdint.h"






 
#line 22 ".\\includes\\timer.h"
void start_timer(WORD times_x_fourms);
void stop_timer(void);

   
#line 20 "src\\timer.c"
#line 1 "..\\..\\sdk\\platform\\driver\\gpio\\gpio.h"















 







 

#line 1 "..\\..\\sdk\\platform\\arch\\arch.h"














 





 
 
 
#line 25 "..\\..\\sdk\\platform\\arch\\arch.h"
#line 1 "..\\..\\sdk\\platform\\arch\\compiler\\rvds\\compiler.h"












 




#line 46 "..\\..\\sdk\\platform\\arch\\compiler\\rvds\\compiler.h"

























#line 26 "..\\..\\sdk\\platform\\arch\\arch.h"
#line 1 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdbool.h"
 






 





#line 25 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdbool.h"



#line 27 "..\\..\\sdk\\platform\\arch\\arch.h"







 
 
 


typedef enum
{
	mode_active = 0,
	mode_idle,
	mode_ext_sleep,
	mode_deep_sleep,
	mode_sleeping,
}sleep_mode_t;





 
struct lld_sleep_env_tag
{
    uint32_t irq_mask;
};


struct mem_usage_log
{
    
    uint16_t max_used_sz;
    
    uint16_t used_sz;
    
    uint16_t max_used_other_sz;
    
    uint16_t used_other_sz;
};

 
 
 




 






 



#line 96 "..\\..\\sdk\\platform\\arch\\arch.h"









 































    

























#line 170 "..\\..\\sdk\\platform\\arch\\arch.h"
































































#line 243 "..\\..\\sdk\\platform\\arch\\arch.h"
















 		














 







 







 



#line 302 "..\\..\\sdk\\platform\\arch\\arch.h"




extern uint16_t clk_freq_trim_reg_value;















 
 




typedef enum {
    NOT_MEASURED = 0,
    APPLY_OVERHEAD,
    NO_OVERHEAD,
} boost_overhead_st;
    




 









 




























extern uint32_t startup_sleep_delay;




 




 














 
uint16_t get_stack_usage(void);















 
void platform_reset(uint32_t error);

void wrap_platform_reset(uint32_t error);

_Bool app_use_lower_clocks_check(void);

void set_sleep_delay(void);

void patch_func(void); 

void calibrate_rcx20(uint16_t cal_time);

void read_rcx_freq(uint16_t cal_time);

uint32_t lld_sleep_lpcycles_2_us_sel_func(uint32_t lpcycles);

uint32_t lld_sleep_us_2_lpcycles_sel_func(uint32_t us);

void conditionally_run_radio_cals(void);

_Bool check_gtl_state(void);

void init_rand_seed_from_trng(void);

uint8_t check_sys_startup_period(void);

void set_xtal16m_trim_value(uint16_t trim_value);

void dbg_prod_output(int mode, unsigned long *hardfault_args);











extern const uint32_t * const jump_table_base[88];










#line 1 "..\\..\\sdk\\platform\\arch\\ll\\rvds\\ll.h"












 








#line 23 "..\\..\\sdk\\platform\\arch\\ll\\rvds\\ll.h"
#line 1 "..\\..\\sdk\\platform\\arch\\arch.h"














 


#line 24 "..\\..\\sdk\\platform\\arch\\ll\\rvds\\ll.h"
#line 1 "..\\..\\sdk\\platform\\include\\reg_intc.h"



#line 5 "..\\..\\sdk\\platform\\include\\reg_intc.h"
#line 1 "..\\..\\sdk\\platform\\include\\_reg_intc.h"










#line 6 "..\\..\\sdk\\platform\\include\\reg_intc.h"
#line 7 "..\\..\\sdk\\platform\\include\\reg_intc.h"
#line 8 "..\\..\\sdk\\platform\\include\\reg_intc.h"
#line 1 "..\\..\\sdk\\platform\\driver\\reg\\reg_access.h"












 













 





 
#line 1 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
 
 
 
 




 








 












#line 38 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"


  



    typedef unsigned int size_t;    
#line 54 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"




extern __declspec(__nothrow) void *memcpy(void * __restrict  ,
                    const void * __restrict  , size_t  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) void *memmove(void *  ,
                    const void *  , size_t  ) __attribute__((__nonnull__(1,2)));
   







 
extern __declspec(__nothrow) char *strcpy(char * __restrict  , const char * __restrict  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) char *strncpy(char * __restrict  , const char * __restrict  , size_t  ) __attribute__((__nonnull__(1,2)));
   





 

extern __declspec(__nothrow) char *strcat(char * __restrict  , const char * __restrict  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) char *strncat(char * __restrict  , const char * __restrict  , size_t  ) __attribute__((__nonnull__(1,2)));
   






 






 

extern __declspec(__nothrow) int memcmp(const void *  , const void *  , size_t  ) __attribute__((__nonnull__(1,2)));
   





 
extern __declspec(__nothrow) int strcmp(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) int strncmp(const char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   






 
extern __declspec(__nothrow) int strcasecmp(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   





 
extern __declspec(__nothrow) int strncasecmp(const char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   






 
extern __declspec(__nothrow) int strcoll(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   







 

extern __declspec(__nothrow) size_t strxfrm(char * __restrict  , const char * __restrict  , size_t  ) __attribute__((__nonnull__(2)));
   













 


#line 193 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) void *memchr(const void *  , int  , size_t  ) __attribute__((__nonnull__(1)));

   





 

#line 209 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) char *strchr(const char *  , int  ) __attribute__((__nonnull__(1)));

   




 

extern __declspec(__nothrow) size_t strcspn(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   




 

#line 232 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) char *strpbrk(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));

   




 

#line 247 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) char *strrchr(const char *  , int  ) __attribute__((__nonnull__(1)));

   





 

extern __declspec(__nothrow) size_t strspn(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   



 

#line 270 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"
extern __declspec(__nothrow) char *strstr(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));

   





 

extern __declspec(__nothrow) char *strtok(char * __restrict  , const char * __restrict  ) __attribute__((__nonnull__(2)));
extern __declspec(__nothrow) char *_strtok_r(char *  , const char *  , char **  ) __attribute__((__nonnull__(2,3)));

extern __declspec(__nothrow) char *strtok_r(char *  , const char *  , char **  ) __attribute__((__nonnull__(2,3)));

   

































 

extern __declspec(__nothrow) void *memset(void *  , int  , size_t  ) __attribute__((__nonnull__(1)));
   



 
extern __declspec(__nothrow) char *strerror(int  );
   





 
extern __declspec(__nothrow) size_t strlen(const char *  ) __attribute__((__nonnull__(1)));
   



 

extern __declspec(__nothrow) size_t strlcpy(char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   
















 

extern __declspec(__nothrow) size_t strlcat(char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   






















 

extern __declspec(__nothrow) void _membitcpybl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpybb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpyhl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpyhb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpywl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpywb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovebl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovebb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovehl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovehb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovewl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovewb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
    














































 







#line 502 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\string.h"



 

#line 35 "..\\..\\sdk\\platform\\driver\\reg\\reg_access.h"

#line 42 "..\\..\\sdk\\platform\\driver\\reg\\reg_access.h"




 

#line 55 "..\\..\\sdk\\platform\\driver\\reg\\reg_access.h"




 

































 

#line 107 "..\\..\\sdk\\platform\\driver\\reg\\reg_access.h"

#line 115 "..\\..\\sdk\\platform\\driver\\reg\\reg_access.h"


#line 9 "..\\..\\sdk\\platform\\include\\reg_intc.h"












 





__forceinline static uint32_t intc_irqstatus_get(void)
{
    return (*(volatile uint32_t *)(0x10001000));
}








__forceinline static uint32_t intc_irqstatus_irqstat_getf(void)
{
    uint32_t localVal = (*(volatile uint32_t *)(0x10001000));
    ;
    return (localVal >> 0);
}








 





__forceinline static uint32_t intc_irqrawstatus_get(void)
{
    return (*(volatile uint32_t *)(0x10001004));
}








__forceinline static uint32_t intc_irqrawstatus_irqrawstat_getf(void)
{
    uint32_t localVal = (*(volatile uint32_t *)(0x10001004));
    ;
    return (localVal >> 0);
}








 





__forceinline static uint32_t intc_irqenableset_get(void)
{
    return (*(volatile uint32_t *)(0x10001008));
}

__forceinline static void intc_irqenableset_set(uint32_t value)
{
    (*(volatile uint32_t *)(0x10001008)) = (value);
}








__forceinline static uint32_t intc_irqenableset_getf(void)
{
    uint32_t localVal = (*(volatile uint32_t *)(0x10001008));
    ;
    return (localVal >> 0);
}

__forceinline static void intc_irqenableset_setf(uint32_t irqenableset)
{
    ;
    (*(volatile uint32_t *)(0x10001008)) = ((uint32_t)irqenableset << 0);
}








 





__forceinline static void intc_irqenableclear_set(uint32_t value)
{
    (*(volatile uint32_t *)(0x1000100C)) = (value);
}








__forceinline static void intc_irqenableclear_setf(uint32_t irqenableclear)
{
    ;
    (*(volatile uint32_t *)(0x1000100C)) = ((uint32_t)irqenableclear << 0);
}








 





__forceinline static uint32_t intc_irqsoft_get(void)
{
    return (*(volatile uint32_t *)(0x10001010));
}

__forceinline static void intc_irqsoft_set(uint32_t value)
{
    (*(volatile uint32_t *)(0x10001010)) = (value);
}







__forceinline static void intc_irqsoft_irqsft_setf(uint8_t irqsft)
{
    ;
    (*(volatile uint32_t *)(0x10001010)) = ((uint32_t)irqsft << 2);
}








 





__forceinline static void intc_irqack_set(uint32_t value)
{
    (*(volatile uint32_t *)(0x10001014)) = (value);
}








__forceinline static void intc_irqack_setf(uint32_t irqack)
{
    ;
    (*(volatile uint32_t *)(0x10001014)) = ((uint32_t)irqack << 0);
}








 





__forceinline static uint32_t intc_irqindex_get(void)
{
    return (*(volatile uint32_t *)(0x1000101C));
}








__forceinline static uint8_t intc_irqindex_getf(void)
{
    uint32_t localVal = (*(volatile uint32_t *)(0x1000101C));
    ;
    return (localVal >> 0);
}








 





__forceinline static uint32_t intc_fiqstatus_get(void)
{
    return (*(volatile uint32_t *)(0x10001100));
}








__forceinline static uint32_t intc_fiqstatus_fiqstat_getf(void)
{
    uint32_t localVal = (*(volatile uint32_t *)(0x10001100));
    ;
    return (localVal >> 0);
}








 





__forceinline static uint32_t intc_fiqrawstatus_get(void)
{
    return (*(volatile uint32_t *)(0x10001104));
}








__forceinline static uint32_t intc_fiqrawstatus_fiqrawstat_getf(void)
{
    uint32_t localVal = (*(volatile uint32_t *)(0x10001104));
    ;
    return (localVal >> 0);
}








 





__forceinline static uint32_t intc_fiqenableset_get(void)
{
    return (*(volatile uint32_t *)(0x10001108));
}

__forceinline static void intc_fiqenableset_set(uint32_t value)
{
    (*(volatile uint32_t *)(0x10001108)) = (value);
}








__forceinline static uint32_t intc_fiqenableset_getf(void)
{
    uint32_t localVal = (*(volatile uint32_t *)(0x10001108));
    ;
    return (localVal >> 0);
}

__forceinline static void intc_fiqenableset_setf(uint32_t fiqenableset)
{
    ;
    (*(volatile uint32_t *)(0x10001108)) = ((uint32_t)fiqenableset << 0);
}








 





__forceinline static void intc_fiqenableclear_set(uint32_t value)
{
    (*(volatile uint32_t *)(0x1000110C)) = (value);
}








__forceinline static void intc_fiqenableclear_setf(uint32_t fiqenableclear)
{
    ;
    (*(volatile uint32_t *)(0x1000110C)) = ((uint32_t)fiqenableclear << 0);
}








 





__forceinline static void intc_fiqack_set(uint32_t value)
{
    (*(volatile uint32_t *)(0x10001110)) = (value);
}








__forceinline static void intc_fiqack_setf(uint32_t fiqack)
{
    ;
    (*(volatile uint32_t *)(0x10001110)) = ((uint32_t)fiqack << 0);
}








 





__forceinline static uint32_t intc_fiqindex_get(void)
{
    return (*(volatile uint32_t *)(0x10001114));
}

__forceinline static void intc_fiqindex_set(uint32_t value)
{
    (*(volatile uint32_t *)(0x10001114)) = (value);
}








__forceinline static uint8_t intc_fiqindex_getf(void)
{
    uint32_t localVal = (*(volatile uint32_t *)(0x10001114));
    ;
    return (localVal >> 0);
}




#line 25 "..\\..\\sdk\\platform\\arch\\ll\\rvds\\ll.h"

#line 1 "..\\..\\sdk\\platform\\include\\ARMCM0.h"
 





















 





#line 30 "..\\..\\sdk\\platform\\include\\ARMCM0.h"





 

 
#line 46 "..\\..\\sdk\\platform\\include\\ARMCM0.h"


#line 1 "..\\..\\sdk\\platform\\include\\core_cm0.h"
 




















 






















 




 


 

 















#line 87 "..\\..\\sdk\\platform\\include\\core_cm0.h"


 







#line 112 "..\\..\\sdk\\platform\\include\\core_cm0.h"

#line 114 "..\\..\\sdk\\platform\\include\\core_cm0.h"
#line 1 "..\\..\\sdk\\platform\\include\\core_cmInstr.h"
 




















 





 



 


 









 







 







 






 








 







 







 









 









 

__attribute__((section(".rev16_text"))) static __inline __asm uint32_t __REV16(uint32_t value)
{
  rev16 r0, r0
  bx lr
}








 

__attribute__((section(".revsh_text"))) static __inline __asm int32_t __REVSH(int32_t value)
{
  revsh r0, r0
  bx lr
}










 



#line 267 "..\\..\\sdk\\platform\\include\\core_cmInstr.h"



#line 618 "..\\..\\sdk\\platform\\include\\core_cmInstr.h"

   

#line 115 "..\\..\\sdk\\platform\\include\\core_cm0.h"
#line 1 "..\\..\\sdk\\platform\\include\\core_cmFunc.h"
 




















 





 



 


 





 
 






 
static __inline uint32_t __get_CONTROL(void)
{
  register uint32_t __regControl         __asm("control");
  return(__regControl);
}







 
static __inline void __set_CONTROL(uint32_t control)
{
  register uint32_t __regControl         __asm("control");
  __regControl = control;
}







 
static __inline uint32_t __get_IPSR(void)
{
  register uint32_t __regIPSR          __asm("ipsr");
  return(__regIPSR);
}







 
static __inline uint32_t __get_APSR(void)
{
  register uint32_t __regAPSR          __asm("apsr");
  return(__regAPSR);
}







 
static __inline uint32_t __get_xPSR(void)
{
  register uint32_t __regXPSR          __asm("xpsr");
  return(__regXPSR);
}







 
static __inline uint32_t __get_PSP(void)
{
  register uint32_t __regProcessStackPointer  __asm("psp");
  return(__regProcessStackPointer);
}







 
static __inline void __set_PSP(uint32_t topOfProcStack)
{
  register uint32_t __regProcessStackPointer  __asm("psp");
  __regProcessStackPointer = topOfProcStack;
}







 
static __inline uint32_t __get_MSP(void)
{
  register uint32_t __regMainStackPointer     __asm("msp");
  return(__regMainStackPointer);
}







 
static __inline void __set_MSP(uint32_t topOfMainStack)
{
  register uint32_t __regMainStackPointer     __asm("msp");
  __regMainStackPointer = topOfMainStack;
}







 
static __inline uint32_t __get_PRIMASK(void)
{
  register uint32_t __regPriMask         __asm("primask");
  return(__regPriMask);
}







 
static __inline void __set_PRIMASK(uint32_t priMask)
{
  register uint32_t __regPriMask         __asm("primask");
  __regPriMask = (priMask);
}


#line 257 "..\\..\\sdk\\platform\\include\\core_cmFunc.h"


#line 293 "..\\..\\sdk\\platform\\include\\core_cmFunc.h"


#line 612 "..\\..\\sdk\\platform\\include\\core_cmFunc.h"

 


#line 116 "..\\..\\sdk\\platform\\include\\core_cm0.h"








 
#line 141 "..\\..\\sdk\\platform\\include\\core_cm0.h"

 






 
#line 157 "..\\..\\sdk\\platform\\include\\core_cm0.h"

 










 


 





 


 
typedef union
{
  struct
  {

    uint32_t _reserved0:27;               





    uint32_t Q:1;                         
    uint32_t V:1;                         
    uint32_t C:1;                         
    uint32_t Z:1;                         
    uint32_t N:1;                         
  } b;                                    
  uint32_t w;                             
} APSR_Type;



 
typedef union
{
  struct
  {
    uint32_t ISR:9;                       
    uint32_t _reserved0:23;               
  } b;                                    
  uint32_t w;                             
} IPSR_Type;



 
typedef union
{
  struct
  {
    uint32_t ISR:9;                       

    uint32_t _reserved0:15;               





    uint32_t T:1;                         
    uint32_t IT:2;                        
    uint32_t Q:1;                         
    uint32_t V:1;                         
    uint32_t C:1;                         
    uint32_t Z:1;                         
    uint32_t N:1;                         
  } b;                                    
  uint32_t w;                             
} xPSR_Type;



 
typedef union
{
  struct
  {
    uint32_t nPRIV:1;                     
    uint32_t SPSEL:1;                     
    uint32_t FPCA:1;                      
    uint32_t _reserved0:29;               
  } b;                                    
  uint32_t w;                             
} CONTROL_Type;

 






 


 
typedef struct
{
  volatile uint32_t ISER[1];                  
       uint32_t RESERVED0[31];
  volatile uint32_t ICER[1];                  
       uint32_t RSERVED1[31];
  volatile uint32_t ISPR[1];                  
       uint32_t RESERVED2[31];
  volatile uint32_t ICPR[1];                  
       uint32_t RESERVED3[31];
       uint32_t RESERVED4[64];
  volatile uint32_t IP[8];                    
}  NVIC_Type;

 






 


 
typedef struct
{
  volatile const  uint32_t CPUID;                    
  volatile uint32_t ICSR;                     
       uint32_t RESERVED0;
  volatile uint32_t AIRCR;                    
  volatile uint32_t SCR;                      
  volatile uint32_t CCR;                      
       uint32_t RESERVED1;
  volatile uint32_t SHP[2];                   
  volatile uint32_t SHCSR;                    
} SCB_Type;

 















 



























 















 









 






 



 






 


 
typedef struct
{
  volatile uint32_t CTRL;                     
  volatile uint32_t LOAD;                     
  volatile uint32_t VAL;                      
  volatile const  uint32_t CALIB;                    
} SysTick_Type;

 












 



 



 









 








 
 






 

 










 









 

 



 




 

 
 










 
static __inline void NVIC_EnableIRQ(IRQn_Type IRQn)
{
  ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[0] = (1 << ((uint32_t)(IRQn) & 0x1F));
}







 
static __inline void NVIC_DisableIRQ(IRQn_Type IRQn)
{
  ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICER[0] = (1 << ((uint32_t)(IRQn) & 0x1F));
}











 
static __inline uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  return((uint32_t) ((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[0] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}







 
static __inline void NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[0] = (1 << ((uint32_t)(IRQn) & 0x1F));
}







 
static __inline void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICPR[0] = (1 << ((uint32_t)(IRQn) & 0x1F));  
}










 
static __inline void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if(IRQn < 0) {
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[( ((((uint32_t)(IRQn) & 0x0F)-8) >> 2) )] = (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[( ((((uint32_t)(IRQn) & 0x0F)-8) >> 2) )] & ~(0xFF << ( (((uint32_t)(IRQn) ) & 0x03) * 8 ))) |
        (((priority << (8 - (2))) & 0xFF) << ( (((uint32_t)(IRQn) ) & 0x03) * 8 )); }
  else {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[( ((uint32_t)(IRQn) >> 2) )] = (((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[( ((uint32_t)(IRQn) >> 2) )] & ~(0xFF << ( (((uint32_t)(IRQn) ) & 0x03) * 8 ))) |
        (((priority << (8 - (2))) & 0xFF) << ( (((uint32_t)(IRQn) ) & 0x03) * 8 )); }
}












 
static __inline uint32_t NVIC_GetPriority(IRQn_Type IRQn)
{

  if(IRQn < 0) {
    return((uint32_t)((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[( ((((uint32_t)(IRQn) & 0x0F)-8) >> 2) )] >> ( (((uint32_t)(IRQn) ) & 0x03) * 8 ) ) >> (8 - (2))));  }  
  else {
    return((uint32_t)((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[ ( ((uint32_t)(IRQn) >> 2) )] >> ( (((uint32_t)(IRQn) ) & 0x03) * 8 ) ) >> (8 - (2))));  }  
}





 
static __inline void NVIC_SystemReset(void)
{
  __dsb(0xF);                                                     
 
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR  = ((0x5FA << 16)      |
                 (1UL << 2));
  __dsb(0xF);                                                      
  while(1);                                                     
}

 



 




 

















 
static __inline uint32_t SysTick_Config(uint32_t ticks)
{
  if (ticks > (0xFFFFFFUL << 0))  return (1);             

  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->LOAD  = (ticks & (0xFFFFFFUL << 0)) - 1;       
  NVIC_SetPriority (SysTick_IRQn, (1<<(2)) - 1);   
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->VAL   = 0;                                           
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL  = (1UL << 2) |
                   (1UL << 1)   |
                   (1UL << 0);                     
  return (0);                                                   
}



 








#line 49 "..\\..\\sdk\\platform\\include\\ARMCM0.h"
#line 1 "..\\..\\sdk\\platform\\include\\system_ARMCM0.h"
 





















 









extern uint32_t SystemCoreClock;      










 
extern void SystemInit (void);









 
extern void SystemCoreClockUpdate (void);





#line 50 "..\\..\\sdk\\platform\\include\\ARMCM0.h"


 
 
 

 
typedef union
{
  volatile uint32_t WORD;  
  volatile uint8_t  BYTE[4];
} GPIO_Data_TypeDef;

typedef struct
{
  GPIO_Data_TypeDef DATA[256];
  volatile uint32_t  DIR;
  uint32_t      RESERVED[3];
  volatile uint32_t  IE;
} ARM_GPIO_TypeDef;


 
 
 
 



 







 
 
 





#line 27 "..\\..\\sdk\\platform\\arch\\ll\\rvds\\ll.h"





  








 







 









    


#line 72 "..\\..\\sdk\\platform\\arch\\ll\\rvds\\ll.h"








 




#line 472 "..\\..\\sdk\\platform\\arch\\arch.h"

#line 27 "..\\..\\sdk\\platform\\driver\\gpio\\gpio.h"
#line 28 "..\\..\\sdk\\platform\\driver\\gpio\\gpio.h"




 

typedef enum {
    INPUT = 0,
    INPUT_PULLUP = 0x100,
    INPUT_PULLDOWN = 0x200,
    OUTPUT = 0x300,
} GPIO_PUPD;

typedef enum {
    GPIO_POWER_RAIL_3V = 0,
    GPIO_POWER_RAIL_1V = 1,
} GPIO_POWER_RAIL;

typedef enum {
    GPIO_PORT_0 = 0,
    GPIO_PORT_1 = 1,
    GPIO_PORT_2 = 2,
    GPIO_PORT_3 = 3,
    GPIO_PORT_3_REMAP = 4,
} GPIO_PORT;

typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1 = 1,
    GPIO_PIN_2 = 2,
    GPIO_PIN_3 = 3,
    GPIO_PIN_4 = 4,
    GPIO_PIN_5 = 5,
    GPIO_PIN_6 = 6,
    GPIO_PIN_7 = 7,
    GPIO_PIN_8 = 8,
    GPIO_PIN_9 = 9,
} GPIO_PIN;

typedef enum {
    PID_GPIO = 0,
    PID_UART1_RX,
    PID_UART1_TX,
    PID_UART2_RX,
    PID_UART2_TX,
    PID_SPI_DI,
    PID_SPI_DO,
    PID_SPI_CLK,
    PID_SPI_EN,
    PID_I2C_SCL,
    PID_I2C_SDA,
    PID_UART1_IRDA_RX,
    PID_UART1_IRDA_TX,
    PID_UART2_IRDA_RX,
    PID_UART2_IRDA_TX,
    PID_ADC,
    PID_PWM0,
    PID_PWM1,
    PID_BLE_DIAG,
    PID_UART1_CTSN,
    PID_UART1_RTSN,
    PID_UART2_CTSN,
    PID_UART2_RTSN,
    PID_PWM2,
    PID_PWM3,
    PID_PWM4,
} GPIO_FUNCTION;

typedef enum {
    GPIO_IRQ_INPUT_LEVEL_HIGH = 0,
    GPIO_IRQ_INPUT_LEVEL_LOW = 1,
} GPIO_IRQ_INPUT_LEVEL;















#line 126 "..\\..\\sdk\\platform\\driver\\gpio\\gpio.h"

typedef void (*GPIO_handler_function_t)(void);




 








 
extern void GPIO_RegisterCallback(IRQn_Type irq,
                                         GPIO_handler_function_t callback);












 
extern void GPIO_EnableIRQ(GPIO_PORT port,
                                  GPIO_PIN pin,
                                  IRQn_Type irq,
                                  _Bool low_input,
                                  _Bool release_wait,
                                  uint8_t debounce_ms);







 
extern GPIO_IRQ_INPUT_LEVEL GPIO_GetIRQInputLevel(IRQn_Type irq);








 
extern void GPIO_SetIRQInputLevel(IRQn_Type irq, GPIO_IRQ_INPUT_LEVEL level);







 
extern void GPIO_ResetIRQ(IRQn_Type irq);






 
extern void GPIO_init(void);










 
extern void GPIO_SetPinFunction(GPIO_PORT port, GPIO_PIN pin, GPIO_PUPD mode, GPIO_FUNCTION function);











 
extern void GPIO_ConfigurePin(GPIO_PORT port, GPIO_PIN pin, GPIO_PUPD mode, GPIO_FUNCTION function,
                               const _Bool high);








 
extern void GPIO_SetActive(GPIO_PORT port, GPIO_PIN pin);








 
extern void GPIO_SetInactive(GPIO_PORT port, GPIO_PIN pin);








 
extern _Bool GPIO_GetPinStatus(GPIO_PORT port, GPIO_PIN pin);









 
extern void GPIO_ConfigurePinPower(GPIO_PORT port, GPIO_PIN pin, GPIO_POWER_RAIL power_rail);



#line 21 "src\\timer.c"
#line 1 "..\\..\\sdk\\platform\\include\\core_cm0.h"
 




















 








#line 118 "..\\..\\sdk\\platform\\include\\core_cm0.h"



#line 664 "..\\..\\sdk\\platform\\include\\core_cm0.h"



#line 22 "src\\timer.c"
#line 1 ".\\includes\\user_periph_setup.h"















 







 

#line 27 ".\\includes\\user_periph_setup.h"
#line 28 ".\\includes\\user_periph_setup.h"
#line 1 "..\\..\\sdk\\platform\\driver\\i2c_eeprom\\i2c_eeprom.h"















 

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 







 

#line 54 "..\\..\\sdk\\platform\\driver\\i2c_eeprom\\i2c_eeprom.h"




 

enum I2C_SPEED_MODES{
  I2C_STANDARD = 1,
  I2C_FAST,
};

enum I2C_ADDRESS_MODES{
  I2C_7BIT_ADDR,
  I2C_10BIT_ADDR,
};

enum I2C_ADRESS_BYTES_COUNT{
  I2C_1BYTE_ADDR,
  I2C_2BYTES_ADDR,
  I2C_3BYTES_ADDR,
};










 
void i2c_eeprom_init(uint16_t dev_address, uint8_t speed, uint8_t address_mode, uint8_t address_size);






 
void i2c_eeprom_release(void);






 
void i2c_wait_until_eeprom_ready(void);







 
uint8_t i2c_eeprom_read_byte(uint32_t address);









 
uint32_t i2c_eeprom_read_data(uint8_t *rd_data_ptr, uint32_t address, uint32_t size);








 
void i2c_eeprom_write_byte(uint32_t address, uint8_t wr_data);









 
uint16_t i2c_eeprom_write_page(uint8_t *wr_data_ptr, uint32_t address, uint16_t size);









 
uint32_t i2c_eeprom_write_data (uint8_t *wr_data_ptr, uint32_t address, uint32_t size);

#line 29 ".\\includes\\user_periph_setup.h"
#line 1 "..\\..\\sdk\\platform\\driver\\spi\\spi.h"















 







 

#line 27 "..\\..\\sdk\\platform\\driver\\spi\\spi.h"




 







 

typedef enum SPI_WORD_MODES{
	SPI_MODE_8BIT,
	SPI_MODE_16BIT,
	SPI_MODE_32BIT,
	SPI_MODE_9BIT,
}SPI_Word_Mode_t;

typedef enum SPI_ROLES{
	SPI_ROLE_MASTER,
	SPI_ROLE_SLAVE,
}SPI_Role_t;

typedef enum SPI_POL_MODES{
	SPI_CLK_IDLE_POL_LOW,
	SPI_CLK_IDLE_POL_HIGH,
}SPI_Polarity_Mode_t;

typedef enum SPI_PHA_MODES{
	SPI_PHA_MODE_0,
	SPI_PHA_MODE_1,
}SPI_PHA_Mode_t;

typedef enum SPI_MINT_MODES{
	SPI_MINT_DISABLE,
	SPI_MINT_ENABLE,
}SPI_MINT_Mode_t;

typedef enum SPI_FREQ_MODES{
  SPI_XTAL_DIV_8,
  SPI_XTAL_DIV_4,
	SPI_XTAL_DIV_2,
	SPI_XTAL_DIV_14,
}SPI_XTAL_Freq_t;

typedef struct
{
	GPIO_PORT port;
	GPIO_PIN pin;
}SPI_Pad_t;




 






 
void spi_cs_low(void);






 
void spi_cs_high(void);













 
void spi_init(SPI_Pad_t *cs_pad_param, SPI_Word_Mode_t bitmode, SPI_Role_t role, SPI_Polarity_Mode_t clk_pol, SPI_PHA_Mode_t pha_mode, SPI_MINT_Mode_t irq, SPI_XTAL_Freq_t freq);







 
void spi_set_bitmode(SPI_Word_Mode_t spiBitMode);






 
void spi_release(void);






 
void spi_disable(void);







 
uint32_t spi_access(uint32_t dataToSend);







 
uint32_t spi_transaction(uint32_t dataToSend);

#line 30 ".\\includes\\user_periph_setup.h"




 





#line 46 ".\\includes\\user_periph_setup.h"
















#line 68 ".\\includes\\user_periph_setup.h"







#line 84 ".\\includes\\user_periph_setup.h"
















#line 23 "src\\timer.c"
  
extern volatile WORD timeout_multi;
extern volatile WORD timeout_occur;
   










 
void start_timer(WORD times_x_fourms)
{
    ( (* ( volatile uint16*)(((0x50000004)))=(((* ( volatile uint16*)((0x50000004)) )&(~(uint16)((0x0008)))) | (((uint16)(1))<<((((0x0008)))&0x0001?0: (((0x0008)))&0x0002?1: (((0x0008)))&0x0004?2: (((0x0008)))&0x0008?3: (((0x0008)))&0x0010?4: (((0x0008)))&0x0020?5: (((0x0008)))&0x0040?6: (((0x0008)))&0x0080?7: (((0x0008)))&0x0100?8: (((0x0008)))&0x0200?9: (((0x0008)))&0x0400?10:(((0x0008)))&0x0800?11: (((0x0008)))&0x1000?12:(((0x0008)))&0x2000?13:(((0x0008)))&0x4000?14: 15))) ));    
    (* ( volatile uint16*)((0x50003302))=((0x0002)) );   
    timeout_occur=0;                          
    (* ( volatile uint16*)((0x50003400))=(0x0) );           
    (* ( volatile uint16*)((0x50003404))=(0x7D00) );    
    (* ( volatile uint16*)((0x50003406))=(0x7D00) );    
    timeout_multi=times_x_fourms;             
    (* ( volatile uint16*)((0x50003400))=(0x07) );          
    NVIC_EnableIRQ(SWTIM_IRQn);               
}






 
void stop_timer()
{
    (* ( volatile uint16*)((0x50003400))=(0x6) );      

    NVIC_ClearPendingIRQ(SWTIM_IRQn);
    NVIC_DisableIRQ(SWTIM_IRQn);         
}

void SWTIM_Handler(void) 
{
    if(  timeout_multi==0) {                
        timeout_occur=1;                    
        (* ( volatile uint16*)((0x50003400))=(0x6) );     
        
    } else
        timeout_multi--;              
}
