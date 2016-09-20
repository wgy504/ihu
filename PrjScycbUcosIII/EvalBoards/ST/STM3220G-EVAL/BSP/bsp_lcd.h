/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*                                             LCD LAYER
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM3220G-EVAL
*                                         Evaluation Board
*
* Filename      : bsp_lcd.h
* Version       : V1.00
* Programmer(s) : SL
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef  BSP_LCD_PRESENT
#define  BSP_LCD_PRESENT

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <bsp.h>
#include  <stm32f2xx.h>


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

typedef struct
{
  CPU_INT16U  Index_Status_Reg;
  CPU_INT16U  Control_Gram_Reg;
} EVAL_LCD_TypeDef;


/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define EVAL_LCD_BASE        ((CPU_INT32U)(0x60000000 | 0x08000000))
#define EVAL_LCD             ((EVAL_LCD_TypeDef *) EVAL_LCD_BASE)

#define BSP_LCD_REG_00                                  0x00
#define BSP_LCD_REG_01                                  0x01
#define BSP_LCD_REG_02                                  0x02
#define BSP_LCD_REG_03                                  0x03
#define BSP_LCD_REG_04                                  0x04
#define BSP_LCD_REG_05                                  0x05
#define BSP_LCD_REG_06                                  0x06
#define BSP_LCD_REG_07                                  0x07
#define BSP_LCD_REG_08                                  0x08
#define BSP_LCD_REG_09                                  0x09
#define BSP_LCD_REG_10                                  0x0A
#define BSP_LCD_REG_12                                  0x0C
#define BSP_LCD_REG_13                                  0x0D
#define BSP_LCD_REG_14                                  0x0E
#define BSP_LCD_REG_15                                  0x0F
#define BSP_LCD_REG_16                                  0x10
#define BSP_LCD_REG_17                                  0x11
#define BSP_LCD_REG_18                                  0x12
#define BSP_LCD_REG_19                                  0x13
#define BSP_LCD_REG_20                                  0x14
#define BSP_LCD_REG_21                                  0x15
#define BSP_LCD_REG_22                                  0x16
#define BSP_LCD_REG_23                                  0x17
#define BSP_LCD_REG_24                                  0x18
#define BSP_LCD_REG_25                                  0x19
#define BSP_LCD_REG_26                                  0x1A
#define BSP_LCD_REG_27                                  0x1B
#define BSP_LCD_REG_28                                  0x1C
#define BSP_LCD_REG_29                                  0x1D
#define BSP_LCD_REG_30                                  0x1E
#define BSP_LCD_REG_31                                  0x1F
#define BSP_LCD_REG_32                                  0x20
#define BSP_LCD_REG_33                                  0x21
#define BSP_LCD_REG_34                                  0x22
#define BSP_LCD_REG_36                                  0x24
#define BSP_LCD_REG_37                                  0x25
#define BSP_LCD_REG_40                                  0x28
#define BSP_LCD_REG_41                                  0x29
#define BSP_LCD_REG_43                                  0x2B
#define BSP_LCD_REG_45                                  0x2D
#define BSP_LCD_REG_48                                  0x30
#define BSP_LCD_REG_49                                  0x31
#define BSP_LCD_REG_50                                  0x32
#define BSP_LCD_REG_51                                  0x33
#define BSP_LCD_REG_52                                  0x34
#define BSP_LCD_REG_53                                  0x35
#define BSP_LCD_REG_54                                  0x36
#define BSP_LCD_REG_55                                  0x37
#define BSP_LCD_REG_56                                  0x38
#define BSP_LCD_REG_57                                  0x39
#define BSP_LCD_REG_58                                  0x3A
#define BSP_LCD_REG_59                                  0x3B
#define BSP_LCD_REG_60                                  0x3C
#define BSP_LCD_REG_61                                  0x3D
#define BSP_LCD_REG_62                                  0x3E
#define BSP_LCD_REG_63                                  0x3F
#define BSP_LCD_REG_64                                  0x40
#define BSP_LCD_REG_65                                  0x41
#define BSP_LCD_REG_66                                  0x42
#define BSP_LCD_REG_67                                  0x43
#define BSP_LCD_REG_68                                  0x44
#define BSP_LCD_REG_69                                  0x45
#define BSP_LCD_REG_70                                  0x46
#define BSP_LCD_REG_71                                  0x47
#define BSP_LCD_REG_72                                  0x48
#define BSP_LCD_REG_73                                  0x49
#define BSP_LCD_REG_74                                  0x4A
#define BSP_LCD_REG_75                                  0x4B
#define BSP_LCD_REG_76                                  0x4C
#define BSP_LCD_REG_77                                  0x4D
#define BSP_LCD_REG_78                                  0x4E
#define BSP_LCD_REG_79                                  0x4F
#define BSP_LCD_REG_80                                  0x50
#define BSP_LCD_REG_81                                  0x51
#define BSP_LCD_REG_82                                  0x52
#define BSP_LCD_REG_83                                  0x53
#define BSP_LCD_REG_96                                  0x60
#define BSP_LCD_REG_97                                  0x61
#define BSP_LCD_REG_106                                 0x76
#define BSP_LCD_REG_128                                 0x80
#define BSP_LCD_REG_129                                 0x81
#define BSP_LCD_REG_130                                 0x82
#define BSP_LCD_REG_131                                 0x83
#define BSP_LCD_REG_132                                 0x84
#define BSP_LCD_REG_133                                 0x85
#define BSP_LCD_REG_134                                 0x86
#define BSP_LCD_REG_135                                 0x87
#define BSP_LCD_REG_136                                 0x88
#define BSP_LCD_REG_137                                 0x89
#define BSP_LCD_REG_139                                 0x8B
#define BSP_LCD_REG_140                                 0x8C
#define BSP_LCD_REG_141                                 0x8D
#define BSP_LCD_REG_143                                 0x8F
#define BSP_LCD_REG_144                                 0x90
#define BSP_LCD_REG_145                                 0x91
#define BSP_LCD_REG_146                                 0x92
#define BSP_LCD_REG_147                                 0x93
#define BSP_LCD_REG_148                                 0x94
#define BSP_LCD_REG_149                                 0x95
#define BSP_LCD_REG_150                                 0x96
#define BSP_LCD_REG_151                                 0x97
#define BSP_LCD_REG_152                                 0x98
#define BSP_LCD_REG_153                                 0x99
#define BSP_LCD_REG_154                                 0x9A
#define BSP_LCD_REG_157                                 0x9D
#define BSP_LCD_REG_192                                 0xC0
#define BSP_LCD_REG_193                                 0xC1
#define BSP_LCD_REG_229                                 0xE5


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         BSP_LCD_Init        (void);


#endif

