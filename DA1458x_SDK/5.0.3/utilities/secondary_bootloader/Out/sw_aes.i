#line 1 "src\\sw_aes.c"




























 




#line 1 "..\\..\\sdk\\platform\\arch\\arch.h"














 





 
 
 
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
        (((priority << (8 - 2)) & 0xFF) << ( (((uint32_t)(IRQn) ) & 0x03) * 8 )); }
  else {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[( ((uint32_t)(IRQn) >> 2) )] = (((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[( ((uint32_t)(IRQn) >> 2) )] & ~(0xFF << ( (((uint32_t)(IRQn) ) & 0x03) * 8 ))) |
        (((priority << (8 - 2)) & 0xFF) << ( (((uint32_t)(IRQn) ) & 0x03) * 8 )); }
}












 
static __inline uint32_t NVIC_GetPriority(IRQn_Type IRQn)
{

  if(IRQn < 0) {
    return((uint32_t)((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[( ((((uint32_t)(IRQn) & 0x0F)-8) >> 2) )] >> ( (((uint32_t)(IRQn) ) & 0x03) * 8 ) ) >> (8 - 2)));  }  
  else {
    return((uint32_t)((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[ ( ((uint32_t)(IRQn) >> 2) )] >> ( (((uint32_t)(IRQn) ) & 0x03) * 8 ) ) >> (8 - 2)));  }  
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
  NVIC_SetPriority (SysTick_IRQn, (1<<2) - 1);   
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

#line 35 "src\\sw_aes.c"

#line 1 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"
 
 
 




 
 



 






   














  


 








#line 54 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"


  



    typedef unsigned int size_t;    
#line 70 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"






    



    typedef unsigned short wchar_t;  
#line 91 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"

typedef struct div_t { int quot, rem; } div_t;
    
typedef struct ldiv_t { long int quot, rem; } ldiv_t;
    

typedef struct lldiv_t { long long quot, rem; } lldiv_t;
    


#line 112 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"
   



 

   




 
#line 131 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"
   


 
extern __declspec(__nothrow) int __aeabi_MB_CUR_MAX(void);

   




 

   




 




extern __declspec(__nothrow) double atof(const char *  ) __attribute__((__nonnull__(1)));
   



 
extern __declspec(__nothrow) int atoi(const char *  ) __attribute__((__nonnull__(1)));
   



 
extern __declspec(__nothrow) long int atol(const char *  ) __attribute__((__nonnull__(1)));
   



 

extern __declspec(__nothrow) long long atoll(const char *  ) __attribute__((__nonnull__(1)));
   



 


extern __declspec(__nothrow) double strtod(const char * __restrict  , char ** __restrict  ) __attribute__((__nonnull__(1)));
   

















 

extern __declspec(__nothrow) float strtof(const char * __restrict  , char ** __restrict  ) __attribute__((__nonnull__(1)));
extern __declspec(__nothrow) long double strtold(const char * __restrict  , char ** __restrict  ) __attribute__((__nonnull__(1)));
   

 

extern __declspec(__nothrow) long int strtol(const char * __restrict  ,
                        char ** __restrict  , int  ) __attribute__((__nonnull__(1)));
   



























 
extern __declspec(__nothrow) unsigned long int strtoul(const char * __restrict  ,
                                       char ** __restrict  , int  ) __attribute__((__nonnull__(1)));
   


























 

 
extern __declspec(__nothrow) long long strtoll(const char * __restrict  ,
                                  char ** __restrict  , int  )
                          __attribute__((__nonnull__(1)));
   




 
extern __declspec(__nothrow) unsigned long long strtoull(const char * __restrict  ,
                                            char ** __restrict  , int  )
                                   __attribute__((__nonnull__(1)));
   



 

extern __declspec(__nothrow) int rand(void);
   







 
extern __declspec(__nothrow) void srand(unsigned int  );
   






 

struct _rand_state { int __x[57]; };
extern __declspec(__nothrow) int _rand_r(struct _rand_state *);
extern __declspec(__nothrow) void _srand_r(struct _rand_state *, unsigned int);
struct _ANSI_rand_state { int __x[1]; };
extern __declspec(__nothrow) int _ANSI_rand_r(struct _ANSI_rand_state *);
extern __declspec(__nothrow) void _ANSI_srand_r(struct _ANSI_rand_state *, unsigned int);
   


 

extern __declspec(__nothrow) void *calloc(size_t  , size_t  );
   



 
extern __declspec(__nothrow) void free(void *  );
   





 
extern __declspec(__nothrow) void *malloc(size_t  );
   



 
extern __declspec(__nothrow) void *realloc(void *  , size_t  );
   













 

extern __declspec(__nothrow) int posix_memalign(void **  , size_t  , size_t  );
   









 

typedef int (*__heapprt)(void *, char const *, ...);
extern __declspec(__nothrow) void __heapstats(int (*  )(void *  ,
                                           char const *  , ...),
                        void *  ) __attribute__((__nonnull__(1)));
   










 
extern __declspec(__nothrow) int __heapvalid(int (*  )(void *  ,
                                           char const *  , ...),
                       void *  , int  ) __attribute__((__nonnull__(1)));
   














 
extern __declspec(__nothrow) __declspec(__noreturn) void abort(void);
   







 

extern __declspec(__nothrow) int atexit(void (*  )(void)) __attribute__((__nonnull__(1)));
   




 
#line 436 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"


extern __declspec(__nothrow) __declspec(__noreturn) void exit(int  );
   












 

extern __declspec(__nothrow) __declspec(__noreturn) void _Exit(int  );
   







      

extern __declspec(__nothrow) char *getenv(const char *  ) __attribute__((__nonnull__(1)));
   









 

extern __declspec(__nothrow) int  system(const char *  );
   









 

extern  void *bsearch(const void *  , const void *  ,
              size_t  , size_t  ,
              int (*  )(const void *, const void *)) __attribute__((__nonnull__(1,2,5)));
   












 
#line 524 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"


extern  void qsort(void *  , size_t  , size_t  ,
           int (*  )(const void *, const void *)) __attribute__((__nonnull__(1,4)));
   









 

#line 553 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"

extern __declspec(__nothrow) __attribute__((const)) int abs(int  );
   



 

extern __declspec(__nothrow) __attribute__((const)) div_t div(int  , int  );
   









 
extern __declspec(__nothrow) __attribute__((const)) long int labs(long int  );
   



 




extern __declspec(__nothrow) __attribute__((const)) ldiv_t ldiv(long int  , long int  );
   











 







extern __declspec(__nothrow) __attribute__((const)) long long llabs(long long  );
   



 




extern __declspec(__nothrow) __attribute__((const)) lldiv_t lldiv(long long  , long long  );
   











 
#line 634 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"




 
typedef struct __sdiv32by16 { int quot, rem; } __sdiv32by16;
typedef struct __udiv32by16 { unsigned int quot, rem; } __udiv32by16;
    
typedef struct __sdiv64by32 { int rem, quot; } __sdiv64by32;

__value_in_regs extern __declspec(__nothrow) __attribute__((const)) __sdiv32by16 __rt_sdiv32by16(
     int  ,
     short int  );
   

 
__value_in_regs extern __declspec(__nothrow) __attribute__((const)) __udiv32by16 __rt_udiv32by16(
     unsigned int  ,
     unsigned short  );
   

 
__value_in_regs extern __declspec(__nothrow) __attribute__((const)) __sdiv64by32 __rt_sdiv64by32(
     int  , unsigned int  ,
     int  );
   

 




 
extern __declspec(__nothrow) unsigned int __fp_status(unsigned int  , unsigned int  );
   







 























 
extern __declspec(__nothrow) int mblen(const char *  , size_t  );
   












 
extern __declspec(__nothrow) int mbtowc(wchar_t * __restrict  ,
                   const char * __restrict  , size_t  );
   















 
extern __declspec(__nothrow) int wctomb(char *  , wchar_t  );
   













 





 
extern __declspec(__nothrow) size_t mbstowcs(wchar_t * __restrict  ,
                      const char * __restrict  , size_t  ) __attribute__((__nonnull__(2)));
   














 
extern __declspec(__nothrow) size_t wcstombs(char * __restrict  ,
                      const wchar_t * __restrict  , size_t  ) __attribute__((__nonnull__(2)));
   














 

extern __declspec(__nothrow) void __use_realtime_heap(void);
extern __declspec(__nothrow) void __use_realtime_division(void);
extern __declspec(__nothrow) void __use_two_region_memory(void);
extern __declspec(__nothrow) void __use_no_heap(void);
extern __declspec(__nothrow) void __use_no_heap_region(void);

extern __declspec(__nothrow) char const *__C_library_version_string(void);
extern __declspec(__nothrow) int __C_library_version_number(void);











#line 892 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdlib.h"





 
#line 37 "src\\sw_aes.c"
#line 1 ".\\includes\\sw_aes.h"
 




#line 1 ".\\includes\\os_int.h"




























 





 








#line 54 ".\\includes\\os_int.h"

#line 59 ".\\includes\\os_int.h"








#line 7 ".\\includes\\sw_aes.h"




 





#line 24 ".\\includes\\sw_aes.h"






typedef struct aes_key_st 
{
    uint16_t rounds;
    uint16_t key_size;
    uint32_t ks[(14+1)*8];
    uint8_t iv[16];
} AES_CTX;

typedef enum
{
    AES_MODE_128,
    AES_MODE_256
} AES_MODE;



 
void AES_set_key(AES_CTX *ctx, const uint8_t *key, 
        const uint8_t *iv, AES_MODE mode);



 
void AES_cbc_encrypt(AES_CTX *ctx, const uint8_t *msg, 
        uint8_t *out, int length);



 
void AES_cbc_decrypt(AES_CTX *ks, const uint8_t *in, uint8_t *out, int length);



 
void AES_convert_key(AES_CTX *ctx);



 
void AES_decrypt(const AES_CTX *ctx, uint32_t *data);



 
void AES_encrypt(const AES_CTX *ctx, uint32_t *data);


#line 38 "src\\sw_aes.c"






 

#line 47 "src\\sw_aes.c"
#line 1 ".\\includes\\os_port.h"




























 





 








#line 45 ".\\includes\\os_port.h"
#line 1 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"
 
 
 





 






 







 




  
 








#line 47 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"


  



    typedef unsigned int size_t;    









 
 

 



    typedef struct __va_list __va_list;






   




 




typedef struct __fpos_t_struct {
    unsigned __int64 __pos;
    



 
    struct {
        unsigned int __state1, __state2;
    } __mbstate;
} fpos_t;
   


 


   

 

typedef struct __FILE FILE;
   






 

#line 136 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"


extern FILE __stdin, __stdout, __stderr;
extern FILE *__aeabi_stdin, *__aeabi_stdout, *__aeabi_stderr;

#line 166 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"
    

    

    





     



   


 


   


 

   



 

   


 




   


 





    


 






extern __declspec(__nothrow) int remove(const char *  ) __attribute__((__nonnull__(1)));
   





 
extern __declspec(__nothrow) int rename(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   








 
extern __declspec(__nothrow) FILE *tmpfile(void);
   




 
extern __declspec(__nothrow) char *tmpnam(char *  );
   











 

extern __declspec(__nothrow) int fclose(FILE *  ) __attribute__((__nonnull__(1)));
   







 
extern __declspec(__nothrow) int fflush(FILE *  );
   







 
extern __declspec(__nothrow) FILE *fopen(const char * __restrict  ,
                           const char * __restrict  ) __attribute__((__nonnull__(1,2)));
   








































 
extern __declspec(__nothrow) FILE *freopen(const char * __restrict  ,
                    const char * __restrict  ,
                    FILE * __restrict  ) __attribute__((__nonnull__(2,3)));
   








 
extern __declspec(__nothrow) void setbuf(FILE * __restrict  ,
                    char * __restrict  ) __attribute__((__nonnull__(1)));
   




 
extern __declspec(__nothrow) int setvbuf(FILE * __restrict  ,
                   char * __restrict  ,
                   int  , size_t  ) __attribute__((__nonnull__(1)));
   















 
#pragma __printf_args
extern __declspec(__nothrow) int fprintf(FILE * __restrict  ,
                    const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   


















 
#pragma __printf_args
extern __declspec(__nothrow) int _fprintf(FILE * __restrict  ,
                     const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   



 
#pragma __printf_args
extern __declspec(__nothrow) int printf(const char * __restrict  , ...) __attribute__((__nonnull__(1)));
   




 
#pragma __printf_args
extern __declspec(__nothrow) int _printf(const char * __restrict  , ...) __attribute__((__nonnull__(1)));
   



 
#pragma __printf_args
extern __declspec(__nothrow) int sprintf(char * __restrict  , const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   






 
#pragma __printf_args
extern __declspec(__nothrow) int _sprintf(char * __restrict  , const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   



 

#pragma __printf_args
extern __declspec(__nothrow) int __ARM_snprintf(char * __restrict  , size_t  ,
                     const char * __restrict  , ...) __attribute__((__nonnull__(3)));


#pragma __printf_args
extern __declspec(__nothrow) int snprintf(char * __restrict  , size_t  ,
                     const char * __restrict  , ...) __attribute__((__nonnull__(3)));
   















 

#pragma __printf_args
extern __declspec(__nothrow) int _snprintf(char * __restrict  , size_t  ,
                      const char * __restrict  , ...) __attribute__((__nonnull__(3)));
   



 
#pragma __scanf_args
extern __declspec(__nothrow) int fscanf(FILE * __restrict  ,
                    const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   






























 
#pragma __scanf_args
extern __declspec(__nothrow) int _fscanf(FILE * __restrict  ,
                     const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   



 
#pragma __scanf_args
extern __declspec(__nothrow) int scanf(const char * __restrict  , ...) __attribute__((__nonnull__(1)));
   






 
#pragma __scanf_args
extern __declspec(__nothrow) int _scanf(const char * __restrict  , ...) __attribute__((__nonnull__(1)));
   



 
#pragma __scanf_args
extern __declspec(__nothrow) int sscanf(const char * __restrict  ,
                    const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   








 
#pragma __scanf_args
extern __declspec(__nothrow) int _sscanf(const char * __restrict  ,
                     const char * __restrict  , ...) __attribute__((__nonnull__(1,2)));
   



 

 
extern __declspec(__nothrow) int vfscanf(FILE * __restrict  , const char * __restrict  , __va_list) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) int vscanf(const char * __restrict  , __va_list) __attribute__((__nonnull__(1)));
extern __declspec(__nothrow) int vsscanf(const char * __restrict  , const char * __restrict  , __va_list) __attribute__((__nonnull__(1,2)));

extern __declspec(__nothrow) int _vfscanf(FILE * __restrict  , const char * __restrict  , __va_list) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) int _vscanf(const char * __restrict  , __va_list) __attribute__((__nonnull__(1)));
extern __declspec(__nothrow) int _vsscanf(const char * __restrict  , const char * __restrict  , __va_list) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) int __ARM_vsscanf(const char * __restrict  , const char * __restrict  , __va_list) __attribute__((__nonnull__(1,2)));

extern __declspec(__nothrow) int vprintf(const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1)));
   





 
extern __declspec(__nothrow) int _vprintf(const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1)));
   



 
extern __declspec(__nothrow) int vfprintf(FILE * __restrict  ,
                    const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1,2)));
   






 
extern __declspec(__nothrow) int vsprintf(char * __restrict  ,
                     const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1,2)));
   






 
extern __declspec(__nothrow) int __ARM_vsnprintf(char * __restrict  , size_t  ,
                     const char * __restrict  , __va_list  ) __attribute__((__nonnull__(3)));

extern __declspec(__nothrow) int vsnprintf(char * __restrict  , size_t  ,
                     const char * __restrict  , __va_list  ) __attribute__((__nonnull__(3)));
   







 

extern __declspec(__nothrow) int _vsprintf(char * __restrict  ,
                      const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1,2)));
   



 
extern __declspec(__nothrow) int _vfprintf(FILE * __restrict  ,
                     const char * __restrict  , __va_list  ) __attribute__((__nonnull__(1,2)));
   



 
extern __declspec(__nothrow) int _vsnprintf(char * __restrict  , size_t  ,
                      const char * __restrict  , __va_list  ) __attribute__((__nonnull__(3)));
   



 

#pragma __printf_args
extern __declspec(__nothrow) int asprintf(char **  , const char * __restrict  , ...) __attribute__((__nonnull__(2)));
extern __declspec(__nothrow) int vasprintf(char **  , const char * __restrict  , __va_list  ) __attribute__((__nonnull__(2)));

#pragma __printf_args
extern __declspec(__nothrow) int __ARM_asprintf(char **  , const char * __restrict  , ...) __attribute__((__nonnull__(2)));
extern __declspec(__nothrow) int __ARM_vasprintf(char **  , const char * __restrict  , __va_list  ) __attribute__((__nonnull__(2)));
   








 

extern __declspec(__nothrow) int fgetc(FILE *  ) __attribute__((__nonnull__(1)));
   







 
extern __declspec(__nothrow) char *fgets(char * __restrict  , int  ,
                    FILE * __restrict  ) __attribute__((__nonnull__(1,3)));
   










 
extern __declspec(__nothrow) int fputc(int  , FILE *  ) __attribute__((__nonnull__(2)));
   







 
extern __declspec(__nothrow) int fputs(const char * __restrict  , FILE * __restrict  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) int getc(FILE *  ) __attribute__((__nonnull__(1)));
   







 




    extern __declspec(__nothrow) int (getchar)(void);

   





 
extern __declspec(__nothrow) char *gets(char *  ) __attribute__((__nonnull__(1)));
   









 
extern __declspec(__nothrow) int putc(int  , FILE *  ) __attribute__((__nonnull__(2)));
   





 




    extern __declspec(__nothrow) int (putchar)(int  );

   



 
extern __declspec(__nothrow) int puts(const char *  ) __attribute__((__nonnull__(1)));
   





 
extern __declspec(__nothrow) int ungetc(int  , FILE *  ) __attribute__((__nonnull__(2)));
   






















 

extern __declspec(__nothrow) size_t fread(void * __restrict  ,
                    size_t  , size_t  , FILE * __restrict  ) __attribute__((__nonnull__(1,4)));
   











 

extern __declspec(__nothrow) size_t __fread_bytes_avail(void * __restrict  ,
                    size_t  , FILE * __restrict  ) __attribute__((__nonnull__(1,3)));
   











 

extern __declspec(__nothrow) size_t fwrite(const void * __restrict  ,
                    size_t  , size_t  , FILE * __restrict  ) __attribute__((__nonnull__(1,4)));
   







 

extern __declspec(__nothrow) int fgetpos(FILE * __restrict  , fpos_t * __restrict  ) __attribute__((__nonnull__(1,2)));
   








 
extern __declspec(__nothrow) int fseek(FILE *  , long int  , int  ) __attribute__((__nonnull__(1)));
   














 
extern __declspec(__nothrow) int fsetpos(FILE * __restrict  , const fpos_t * __restrict  ) __attribute__((__nonnull__(1,2)));
   










 
extern __declspec(__nothrow) long int ftell(FILE *  ) __attribute__((__nonnull__(1)));
   











 
extern __declspec(__nothrow) void rewind(FILE *  ) __attribute__((__nonnull__(1)));
   





 

extern __declspec(__nothrow) void clearerr(FILE *  ) __attribute__((__nonnull__(1)));
   




 

extern __declspec(__nothrow) int feof(FILE *  ) __attribute__((__nonnull__(1)));
   


 
extern __declspec(__nothrow) int ferror(FILE *  ) __attribute__((__nonnull__(1)));
   


 
extern __declspec(__nothrow) void perror(const char *  );
   









 

extern __declspec(__nothrow) int _fisatty(FILE *   ) __attribute__((__nonnull__(1)));
    
 

extern __declspec(__nothrow) void __use_no_semihosting_swi(void);
extern __declspec(__nothrow) void __use_no_semihosting(void);
    





 











#line 1021 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\stdio.h"



 

#line 46 ".\\includes\\os_port.h"

#line 54 ".\\includes\\os_port.h"






#line 125 ".\\includes\\os_port.h"






#line 1 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\errno.h"
 
 
 
 





 












#line 30 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\errno.h"


extern __declspec(__nothrow) __attribute__((const)) volatile int *__aeabi_errno_addr(void);






















 





 





 
#line 76 "C:\\Keil_v5\\ARM\\ARMCC\\Bin\\..\\include\\errno.h"










 












 










 








 





 



 

#line 132 ".\\includes\\os_port.h"














 











 



void exit_now(const char *format, ...);


 
#line 187 ".\\includes\\os_port.h"





#line 48 "src\\sw_aes.c"
#line 49 "src\\sw_aes.c"

















 
#line 73 "src\\sw_aes.c"

#line 85 "src\\sw_aes.c"



 
static const uint8_t aes_sbox[256] =
{
	0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,
	0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
	0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,
	0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
	0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,
	0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
	0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,
	0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
	0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,
	0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
	0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,
	0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
	0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,
	0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
	0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,
	0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
	0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,
	0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
	0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,
	0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
	0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,
	0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
	0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,
	0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
	0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,
	0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
	0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,
	0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
	0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,
	0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
	0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,
	0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16,
};



 
static const uint8_t aes_isbox[256] = 
{
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,
    0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,
    0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,
    0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,
    0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,
    0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,
    0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,
    0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,
    0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,
    0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,
    0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,
    0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,
    0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,
    0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,
    0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,
    0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,
    0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const unsigned char Rcon[30]=
{
	0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,
	0x1b,0x36,0x6c,0xd8,0xab,0x4d,0x9a,0x2f,
	0x5e,0xbc,0x63,0xc6,0x97,0x35,0x6a,0xd4,
	0xb3,0x7d,0xfa,0xef,0xc5,0x91,
};

 
void AES_encrypt(const AES_CTX *ctx, uint32_t *data);
void AES_decrypt(const AES_CTX *ctx, uint32_t *data);


 
static unsigned char AES_xtime(uint32_t x)
{
	return (x&0x80) ? (x<<1)^0x1b : x<<1;
}



 
void AES_set_key(AES_CTX *ctx, const uint8_t *key, 
        const uint8_t *iv, AES_MODE mode)
{
    int i, ii;
    uint32_t *W, tmp, tmp2;
    const unsigned char *ip;
    int words;

    switch (mode)
    {
        case AES_MODE_128:
            i = 10;
            words = 4;
            break;

        case AES_MODE_256:
            i = 14;
            words = 8;
            break;

        default:         
            return;
    }

    ctx->rounds = i;
    ctx->key_size = words;
    W = ctx->ks;
    for (i = 0; i < words; i+=2)
    {
        W[i+0]=	((uint32_t)key[ 0]<<24)|
            ((uint32_t)key[ 1]<<16)|
            ((uint32_t)key[ 2]<< 8)|
            ((uint32_t)key[ 3]    );
        W[i+1]=	((uint32_t)key[ 4]<<24)|
            ((uint32_t)key[ 5]<<16)|
            ((uint32_t)key[ 6]<< 8)|
            ((uint32_t)key[ 7]    );
        key += 8;
    }

    ip = Rcon;
    ii = 4 * (ctx->rounds+1);
    for (i = words; i<ii; i++)
    {
        tmp = W[i-1];

        if ((i % words) == 0)
        {
            tmp2 =(uint32_t)aes_sbox[(tmp    )&0xff]<< 8;
            tmp2|=(uint32_t)aes_sbox[(tmp>> 8)&0xff]<<16;
            tmp2|=(uint32_t)aes_sbox[(tmp>>16)&0xff]<<24;
            tmp2|=(uint32_t)aes_sbox[(tmp>>24)     ];
            tmp=tmp2^(((unsigned int)*ip)<<24);
            ip++;
        }

        if ((words == 8) && ((i % words) == 4))
        {
            tmp2 =(uint32_t)aes_sbox[(tmp    )&0xff]    ;
            tmp2|=(uint32_t)aes_sbox[(tmp>> 8)&0xff]<< 8;
            tmp2|=(uint32_t)aes_sbox[(tmp>>16)&0xff]<<16;
            tmp2|=(uint32_t)aes_sbox[(tmp>>24)     ]<<24;
            tmp=tmp2;
        }

        W[i]=W[i-words]^tmp;
    }

     
    memcpy(ctx->iv, iv, 16);
}



 
void AES_convert_key(AES_CTX *ctx)
{
    int i;
    uint32_t *k,w,t1,t2,t3,t4;

    k = ctx->ks;
    k += 4;

    for (i= ctx->rounds*4; i > 4; i--)
    {
        w= *k;
        w = ( (t1)=((t1)=((w)&0x80808080), ((((w)+(w))&0xfefefefe)^(((t1)-((t1)>>7))&0x1b1b1b1b))), (t2)=((t2)=((t1)&0x80808080), ((((t1)+(t1))&0xfefefefe)^(((t2)-((t2)>>7))&0x1b1b1b1b))), (t3)=((t3)=((t2)&0x80808080), ((((t2)+(t2))&0xfefefefe)^(((t3)-((t3)>>7))&0x1b1b1b1b))), (t4)=(w)^(t3), (t3)=((t1)^(t2)^(t3)), (t1)^=(t4), (t2)^=(t4), (t3)^=(((t1) << 8) | ((t1) >> 24)), (t3)^=(((t2) << 16) | ((t2) >> 16)), (t3)^(((t4) << 24) | ((t4) >> 8)));
        *k++ =w;
    }
}

#line 319 "src\\sw_aes.c"



 
void AES_cbc_decrypt(AES_CTX *ctx, const uint8_t *msg, uint8_t *out, int length)
{
    int i;
    uint32_t tin[4], xor[4], tout[4], data[4], iv[4];

    memcpy(iv, ctx->iv, 16);
    for (i = 0; i < 4; i++)
        xor[i] = (((((iv[i])) >> 24) & 0x000000ff) | ((((iv[i])) >> 8) & 0x0000ff00) | ((((iv[i])) << 8) & 0x00ff0000) | ((((iv[i])) << 24) & 0xff000000));

    for (length -= 16; length >= 0; length -= 16)
    {
        uint32_t msg_32[4];
        uint32_t out_32[4];
        memcpy(msg_32, msg, 16);
        msg += 16;

        for (i = 0; i < 4; i++)
        {
            tin[i] = (((((msg_32[i])) >> 24) & 0x000000ff) | ((((msg_32[i])) >> 8) & 0x0000ff00) | ((((msg_32[i])) << 8) & 0x00ff0000) | ((((msg_32[i])) << 24) & 0xff000000));
            data[i] = tin[i];
        }

        AES_decrypt(ctx, data);

        for (i = 0; i < 4; i++)
        {
            tout[i] = data[i]^xor[i];
            xor[i] = tin[i];
            out_32[i] = ((((tout[i]) >> 24) & 0x000000ff) | (((tout[i]) >> 8) & 0x0000ff00) | (((tout[i]) << 8) & 0x00ff0000) | (((tout[i]) << 24) & 0xff000000));
        }

        memcpy(out, out_32, 16);
        out += 16;
    }

    for (i = 0; i < 4; i++)
        iv[i] = ((((xor[i]) >> 24) & 0x000000ff) | (((xor[i]) >> 8) & 0x0000ff00) | (((xor[i]) << 8) & 0x00ff0000) | (((xor[i]) << 24) & 0xff000000));
    memcpy(ctx->iv, iv, 16);
}

#line 416 "src\\sw_aes.c"



 
void AES_decrypt(const AES_CTX *ctx, uint32_t *data)
{ 
    uint32_t tmp[4];
    uint32_t xt0,xt1,xt2,xt3,xt4,xt5,xt6;
    uint32_t a0, a1, a2, a3, row;
    int curr_rnd;
    int rounds = ctx->rounds;
    const uint32_t *k = ctx->ks + ((rounds+1)*4);

     
    for (row=4; row > 0;row--)
        data[row-1] ^= *(--k);

     
    for (curr_rnd = 0; curr_rnd < rounds; curr_rnd++)
    {
         
        for (row = 4; row > 0; row--)
        {
            a0 = aes_isbox[(data[(row+3)%4]>>24)&0xFF];
            a1 = aes_isbox[(data[(row+2)%4]>>16)&0xFF];
            a2 = aes_isbox[(data[(row+1)%4]>>8)&0xFF];
            a3 = aes_isbox[(data[row%4])&0xFF];

             
            if (curr_rnd<(rounds-1))
            {
                

 
                xt0 = AES_xtime(a0^a1);
                xt1 = AES_xtime(a1^a2);
                xt2 = AES_xtime(a2^a3);
                xt3 = AES_xtime(a3^a0);
                xt4 = AES_xtime(xt0^xt1);
                xt5 = AES_xtime(xt1^xt2);
                xt6 = AES_xtime(xt4^xt5);

                xt0 ^= a1^a2^a3^xt4^xt6;
                xt1 ^= a0^a2^a3^xt5^xt6;
                xt2 ^= a0^a1^a3^xt4^xt6;
                xt3 ^= a0^a1^a2^xt5^xt6;
                tmp[row-1] = ((xt0<<24)|(xt1<<16)|(xt2<<8)|xt3);
            }
            else
                tmp[row-1] = ((a0<<24)|(a1<<16)|(a2<<8)|a3);
        }

        for (row = 4; row > 0; row--)
            data[row-1] = tmp[row-1] ^ *(--k);
    }
}

