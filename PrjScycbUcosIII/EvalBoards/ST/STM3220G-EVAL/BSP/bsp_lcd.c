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
* Filename      : bsp_lcd.c
* Version       : V1.00
* Programmer(s) : SL
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <bsp_lcd.h>
#include  <stm32f2xx_fsmc.h>


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void         BSP_LCD_WriteReg    (CPU_INT08U            reg_ix,
                                          CPU_INT16U            reg_val);

static  CPU_INT16U   BSP_LCD_ReadReg     (CPU_INT08U            reg_ix);

static  void         BSP_LCD_GPIO_Cfg    (void);
static  void         BSP_LCD_FSMC_Init   (void);


/*
*********************************************************************************************************
*                                      LOCAL GLOBAL VARIABLRS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          BSP_LCD_Init()
*
* Description : Initialize LCD.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : LCD_X_DisplayDriver().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LCD_Init(void)
{
    CPU_INT32U  lcd_id;


    BSP_LCD_GPIO_Cfg();                                         /* Cfg LCD GPIOs.                                      */
    BSP_LCD_FSMC_Init();                                        /* Cfg LCD FSMC.                                       */

    BSP_OS_TimeDlyMs(50);                                       /* Delay 50 ms.                                        */

    lcd_id = BSP_LCD_ReadReg(0x00);                             /* Read the LCD ID.                                    */

                                                                /* Check if the LCD ctrl is ILI9320.                   */
    if(lcd_id == 0x9320) {                                      /* --------------- START INIT SEQUENCE --------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_229, 0x8000);              /* Set the internal vcore voltage.                     */
        BSP_LCD_WriteReg(BSP_LCD_REG_00,  0x0001);              /* Start internal OSC.                                 */
        BSP_LCD_WriteReg(BSP_LCD_REG_01,  0x0100);              /* Set SS and SM bit.                                  */
        BSP_LCD_WriteReg(BSP_LCD_REG_02,  0x0700);              /* Set 1 line inversion.                               */
        BSP_LCD_WriteReg(BSP_LCD_REG_03,  0x1030);              /* Set GRAM write direction and BGR=1.                 */
        BSP_LCD_WriteReg(BSP_LCD_REG_04,  0x0000);              /* Resize register.                                    */
        BSP_LCD_WriteReg(BSP_LCD_REG_08,  0x0202);              /* Set the back porch and front porch.                 */
        BSP_LCD_WriteReg(BSP_LCD_REG_09,  0x0000);              /* Set non-display area refresh cycle ISC[3:0].        */
        BSP_LCD_WriteReg(BSP_LCD_REG_10,  0x0000);              /* FMARK function.                                     */
        BSP_LCD_WriteReg(BSP_LCD_REG_12,  0x0000);              /* RGB interface setting.                              */
        BSP_LCD_WriteReg(BSP_LCD_REG_13,  0x0000);              /* Frame marker position.                              */
        BSP_LCD_WriteReg(BSP_LCD_REG_15,  0x0000);              /* RGB interface polarity.                             */

                                                                /* ---------------- POWER ON SEQUENCE ---------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_16,  0x0000);              /* SAP, BT[3:0], AP, DSTB, SLP, STB.                   */
        BSP_LCD_WriteReg(BSP_LCD_REG_17,  0x0000);              /* DC1[2:0], DC0[2:0], VC[2:0].                        */
        BSP_LCD_WriteReg(BSP_LCD_REG_18,  0x0000);              /* VREG1OUT voltage.                                   */
        BSP_LCD_WriteReg(BSP_LCD_REG_19,  0x0000);              /* VDV[4:0] for VCOM amplitude.                        */
        BSP_OS_TimeDlyMs(200);                                  /* Dis-charge capacitor power voltage (200ms).         */
        BSP_LCD_WriteReg(BSP_LCD_REG_16,  0x17B0);              /* SAP, BT[3:0], AP, DSTB, SLP, STB.                   */
        BSP_LCD_WriteReg(BSP_LCD_REG_17,  0x0137);              /* DC1[2:0], DC0[2:0], VC[2:0].                        */
        BSP_OS_TimeDlyMs(50);                                   /* Delay 50 ms.                                        */
        BSP_LCD_WriteReg(BSP_LCD_REG_18,  0x0139);              /* VREG1OUT voltage.                                   */
        BSP_OS_TimeDlyMs(50);                                   /* Delay 50 ms.                                        */
        BSP_LCD_WriteReg(BSP_LCD_REG_19,  0x1d00);              /* VDV[4:0] for VCOM amplitude.                        */
        BSP_LCD_WriteReg(BSP_LCD_REG_41,  0x0013);              /* VCM[4:0] for VCOMH.                                 */
        BSP_OS_TimeDlyMs(50);                                   /* Delay 50 ms.                                        */
        BSP_LCD_WriteReg(BSP_LCD_REG_32,  0x0000);              /* GRAM horizontal address.                            */
        BSP_LCD_WriteReg(BSP_LCD_REG_33,  0x0000);              /* GRAM Vertical   address.                            */

                                                                /* ---------------- AJUST GAMMA CURVE ---------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_48,  0x0006);
        BSP_LCD_WriteReg(BSP_LCD_REG_49,  0x0101);
        BSP_LCD_WriteReg(BSP_LCD_REG_50,  0x0003);
        BSP_LCD_WriteReg(BSP_LCD_REG_53,  0x0106);
        BSP_LCD_WriteReg(BSP_LCD_REG_54,  0x0b02);
        BSP_LCD_WriteReg(BSP_LCD_REG_55,  0x0302);
        BSP_LCD_WriteReg(BSP_LCD_REG_56,  0x0707);
        BSP_LCD_WriteReg(BSP_LCD_REG_57,  0x0007);
        BSP_LCD_WriteReg(BSP_LCD_REG_60,  0x0600);
        BSP_LCD_WriteReg(BSP_LCD_REG_61,  0x020b);

                                                                /* ------------------ SET GRAM AREA ------------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_80,  0x0000);              /* Horizontal GRAM start address.                       */
        BSP_LCD_WriteReg(BSP_LCD_REG_81,  0x00EF);              /* Horizontal GRAM end   address.                       */
        BSP_LCD_WriteReg(BSP_LCD_REG_82,  0x0000);              /* Vertical   GRAM start address.                       */
        BSP_LCD_WriteReg(BSP_LCD_REG_83,  0x013F);              /* Vertical   GRAM end   address.                       */
        BSP_LCD_WriteReg(BSP_LCD_REG_96,  0x2700);              /* Gate scan line.                                      */
        BSP_LCD_WriteReg(BSP_LCD_REG_97,  0x0001);              /* NDL,VLE, REV.                                        */
        BSP_LCD_WriteReg(BSP_LCD_REG_106, 0x0000);              /* Set scrolling line.                                  */

                                                                /* -------------- PARTIAL DISPLAY CTRL ---------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_128, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_129, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_130, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_131, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_132, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_133, 0x0000);

                                                                /* -------------------- PANEL CTRL -------------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_144, 0x0010);
        BSP_LCD_WriteReg(BSP_LCD_REG_146, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_147, 0x0003);
        BSP_LCD_WriteReg(BSP_LCD_REG_149, 0x0110);
        BSP_LCD_WriteReg(BSP_LCD_REG_151, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_152, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_03,  0x1018);
        BSP_LCD_WriteReg(BSP_LCD_REG_07,  0x0173);

    } else if(lcd_id == 0x9325) {                               /* Check if the LCD ctrl is ILI9325.                    */
                                                                /* --------------- START INIT SEQUENCE ---------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_00,  0x0001);              /* Start internal OSC.                                  */
        BSP_LCD_WriteReg(BSP_LCD_REG_01,  0x0100);              /* Set SS and SM bit.                                   */
        BSP_LCD_WriteReg(BSP_LCD_REG_02,  0x0700);              /* Set 1 line inversion.                                */
        BSP_LCD_WriteReg(BSP_LCD_REG_03,  0x1018);              /* Set GRAM write direction and BGR=1.                  */
        BSP_LCD_WriteReg(BSP_LCD_REG_04,  0x0000);              /* Resize register.                                     */
        BSP_LCD_WriteReg(BSP_LCD_REG_08,  0x0202);              /* Set the back porch and front porch.                  */
        BSP_LCD_WriteReg(BSP_LCD_REG_09,  0x0000);              /* Set non-display area refresh cycle ISC[3:0].         */
        BSP_LCD_WriteReg(BSP_LCD_REG_10,  0x0000);              /* FMARK function.                                      */
        BSP_LCD_WriteReg(BSP_LCD_REG_12,  0x0000);              /* RGB interface setting.                               */
        BSP_LCD_WriteReg(BSP_LCD_REG_13,  0x0000);              /* Frame marker position.                               */
        BSP_LCD_WriteReg(BSP_LCD_REG_15,  0x0000);              /* RGB interface polarity.                              */

                                                                /* ----------------- POWER ON SEQUENCE ---------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_16,  0x0000);              /* SAP, BT[3:0], AP, DSTB, SLP, STB.                    */
        BSP_LCD_WriteReg(BSP_LCD_REG_17,  0x0000);              /* DC1[2:0], DC0[2:0], VC[2:0].                         */
        BSP_LCD_WriteReg(BSP_LCD_REG_18,  0x0000);              /* VREG1OUT voltage.                                    */
        BSP_LCD_WriteReg(BSP_LCD_REG_19,  0x0000);              /* VDV[4:0] for VCOM amplitude.                         */
        BSP_OS_TimeDlyMs(200);                                  /* Dis-charge capacitor power voltage (200ms).          */
        BSP_LCD_WriteReg(BSP_LCD_REG_16,  0x17B0);              /* SAP, BT[3:0], AP, DSTB, SLP, STB.                    */
        BSP_LCD_WriteReg(BSP_LCD_REG_17,  0x0137);              /* DC1[2:0], DC0[2:0], VC[2:0].                         */
        BSP_OS_TimeDlyMs(50);                                   /* Delay 50 ms.                                         */
        BSP_LCD_WriteReg(BSP_LCD_REG_18,  0x0139);              /* VREG1OUT voltage.                                    */
        BSP_OS_TimeDlyMs(50);                                   /* Delay 50 ms.                                         */
        BSP_LCD_WriteReg(BSP_LCD_REG_19,  0x1d00);              /* VDV[4:0] for VCOM amplitude.                         */
        BSP_LCD_WriteReg(BSP_LCD_REG_41,  0x0013);              /* VCM[4:0] for VCOMH.                                  */
        BSP_OS_TimeDlyMs(50);                                   /* Delay 50 ms.                                         */
        BSP_LCD_WriteReg(BSP_LCD_REG_32,  0x0000);              /* GRAM horizontal address.                             */
        BSP_LCD_WriteReg(BSP_LCD_REG_33,  0x0000);              /* GRAM Vertical   address.                             */

                                                                /* ----------------- AJUST GAMMA CURVE ---------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_48,  0x0007);
        BSP_LCD_WriteReg(BSP_LCD_REG_49,  0x0302);
        BSP_LCD_WriteReg(BSP_LCD_REG_50,  0x0105);
        BSP_LCD_WriteReg(BSP_LCD_REG_53,  0x0206);
        BSP_LCD_WriteReg(BSP_LCD_REG_54,  0x0808);
        BSP_LCD_WriteReg(BSP_LCD_REG_55,  0x0206);
        BSP_LCD_WriteReg(BSP_LCD_REG_56,  0x0504);
        BSP_LCD_WriteReg(BSP_LCD_REG_57,  0x0007);
        BSP_LCD_WriteReg(BSP_LCD_REG_60,  0x0105);
        BSP_LCD_WriteReg(BSP_LCD_REG_61,  0x0808);

                                                                /* ------------------ SET GRAM AREA ------------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_80,  0x0000);              /* Horizontal GRAM start address.                       */
        BSP_LCD_WriteReg(BSP_LCD_REG_81,  0x00EF);              /* Horizontal GRAM end   address.                       */
        BSP_LCD_WriteReg(BSP_LCD_REG_82,  0x0000);              /* Vertical   GRAM start address.                       */
        BSP_LCD_WriteReg(BSP_LCD_REG_83,  0x013F);              /* Vertical   GRAM end   address.                       */

        BSP_LCD_WriteReg(BSP_LCD_REG_96,  0xA700);              /* Gate Scan Line(GS=1, scan direction is G320~G1).     */
        BSP_LCD_WriteReg(BSP_LCD_REG_97,  0x0001);              /* NDL,VLE, REV.                                        */
        BSP_LCD_WriteReg(BSP_LCD_REG_106, 0x0000);              /* Set scrolling line.                                  */

                                                                /* -------------- PARTIAL DISPLAY CTRL ---------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_128, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_129, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_130, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_131, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_132, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_133, 0x0000);

                                                                /* -------------------- PANEL CTRL -------------------- */
        BSP_LCD_WriteReg(BSP_LCD_REG_144, 0x0010);
        BSP_LCD_WriteReg(BSP_LCD_REG_146, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_147, 0x0003);
        BSP_LCD_WriteReg(BSP_LCD_REG_149, 0x0110);
        BSP_LCD_WriteReg(BSP_LCD_REG_151, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_152, 0x0000);
        BSP_LCD_WriteReg(BSP_LCD_REG_03,  0x1018);
        BSP_LCD_WriteReg(BSP_LCD_REG_07,  0x0133);
    }
}


/*
*********************************************************************************************************
*                                         BSP_LCD_WriteReg()
*
* Description : Write a value in a register.
*
* Argument(s) : reg_ix      Register index.
*
*               reg_val     Value to write in the register.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_LCD_WriteReg(CPU_INT08U  reg_ix,
                               CPU_INT16U  reg_val)
{
  EVAL_LCD->Index_Status_Reg = reg_ix;                     /* Write 8-bit  index.                                  */
  EVAL_LCD->Control_Gram_Reg = reg_val;                    /* Write 16-bit value.                                  */
}


/*
*********************************************************************************************************
*                                         BSP_LCD_WriteReg()
*
* Description : Read a value from a register.
*
* Argument(s) : reg_ix      Register index.
*
* Return(s)   : Register value.
*
* Caller(s)   : BSP_LCD_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT16U  BSP_LCD_ReadReg(CPU_INT08U reg_ix)
{
  CPU_INT16U  reg_val;

  EVAL_LCD->Index_Status_Reg = reg_ix;                     /* Write 8-bit index.                                        */
  reg_val = EVAL_LCD->Control_Gram_Reg;                    /* Read 16-bit value.                                        */

  return reg_val;
}


/*
*********************************************************************************************************
*                                      BSP_LCD_GPIO_Cfg()
*
* Description : Configures LCD control lines in Output Push-Pull mode.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static void  BSP_LCD_GPIO_Cfg(void)
{

    GPIO_InitTypeDef  GPIO_cfg;

#ifndef USE_EXT_RAM                                             /* Cfg GPIOs if not done during SRAM cfg.               */

                                                                /* -------------------- GPIO CLOCKS --------------------*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD |               /* Enable GPIOD clock.                                  */
                           RCC_AHB1Periph_GPIOG |               /* Enable GPIOG clock.                                  */
                           RCC_AHB1Periph_GPIOE |               /* Enable GPIOE clock.                                  */
                           RCC_AHB1Periph_GPIOF, ENABLE);       /* Enable GPIOF clock.                                  */

                                                                /* --------------------- GPIO CFG ----------------------*/
    GPIO_cfg.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  |
                        GPIO_Pin_4  | GPIO_Pin_5  |
                        GPIO_Pin_8  | GPIO_Pin_9  |
                        GPIO_Pin_10 | GPIO_Pin_14 |
                        GPIO_Pin_15;

    GPIO_cfg.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_cfg.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_cfg.GPIO_OType = GPIO_OType_PP;
    GPIO_cfg.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOD, &GPIO_cfg);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

    GPIO_cfg.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  |
                        GPIO_Pin_9  | GPIO_Pin_10 |
                        GPIO_Pin_11 | GPIO_Pin_12 |
                        GPIO_Pin_13 | GPIO_Pin_14 |
                        GPIO_Pin_15;

    GPIO_Init(GPIOE, &GPIO_cfg);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);


    GPIO_cfg.GPIO_Pin = GPIO_Pin_0;                             /* SRAM address lines configuration.                    */
    GPIO_Init(GPIOF,   &GPIO_cfg);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FSMC);

#endif

    GPIO_cfg.GPIO_Pin = GPIO_Pin_10;                            /* NE3                configuration.                    */
    GPIO_Init(GPIOG,   &GPIO_cfg);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FSMC);
}


/*
*********************************************************************************************************
*                                        BSP_LCD_FSMC_Init()
*
* Description : Configures FSMC for LCD usage.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_LCD_FSMC_Init(void)
{
    FSMC_NORSRAMInitTypeDef        FSMC_SRAM_Init;
    FSMC_NORSRAMTimingInitTypeDef  FSMC_SRAM_TimingInit;


#ifndef USE_EXT_RAM
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);        /* Enable FSMC clock if not done during SRAM init.      */
#endif

                                                                /*------------------- SRAM BANK 3 ----------------------*/
    FSMC_SRAM_TimingInit.FSMC_AddressSetupTime      = 1;
    FSMC_SRAM_TimingInit.FSMC_AddressHoldTime       = 0;
    FSMC_SRAM_TimingInit.FSMC_DataSetupTime         = 9;
    FSMC_SRAM_TimingInit.FSMC_BusTurnAroundDuration = 0;
    FSMC_SRAM_TimingInit.FSMC_CLKDivision           = 0;
    FSMC_SRAM_TimingInit.FSMC_DataLatency           = 0;
    FSMC_SRAM_TimingInit.FSMC_AccessMode            = FSMC_AccessMode_A;

    FSMC_SRAM_Init.FSMC_ReadWriteTimingStruct = &FSMC_SRAM_TimingInit;
    FSMC_SRAM_Init.FSMC_WriteTimingStruct     = &FSMC_SRAM_TimingInit;

                                                                /*-------------------- COLOR LCD ---------------------- */

    FSMC_SRAM_Init.FSMC_Bank                  = FSMC_Bank1_NORSRAM3;

    FSMC_SRAM_Init.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
    FSMC_SRAM_Init.FSMC_MemoryType            = FSMC_MemoryType_SRAM;
    FSMC_SRAM_Init.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
    FSMC_SRAM_Init.FSMC_AsynchronousWait      = FSMC_AsynchronousWait_Disable;
    FSMC_SRAM_Init.FSMC_ExtendedMode          = FSMC_ExtendedMode_Enable;
    FSMC_SRAM_Init.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;

    FSMC_SRAM_Init.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
    FSMC_SRAM_Init.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
    FSMC_SRAM_Init.FSMC_WrapMode              = FSMC_WrapMode_Disable;
    FSMC_SRAM_Init.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_SRAM_Init.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
    FSMC_SRAM_Init.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;

    FSMC_NORSRAMInit(&FSMC_SRAM_Init);

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
}


