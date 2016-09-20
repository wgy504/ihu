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
*                                             I/O Expander
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM3220G-EVAL
*                                         Evaluation Board
*
* Filename      : bsp_ioe.c
* Version       : V1.00
* Programmer(s) : SL
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <bsp_ioe.h>


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void         BSP_IOE_GPIO_Cfg       (void);
static  void         BSP_IOE_I2C_Cfg        (void);

static  void         BSP_IOE_TS_Cfg         (void);
static  void         BSP_IOE_TempSens_Cfg   (void);

static  CPU_INT16U   BSP_IOE_TS_Read_X      (void);
static  CPU_INT16U   BSP_IOE_TS_Read_Y      (void);
static  CPU_INT16U   BSP_IOE_TS_Read_Z      (void);

static  void         BSP_IOE_Reset          (CPU_INT08U         ioe_addr);

static  void         BSP_IOE_FnctCmd        (CPU_INT08U         ioe_addr,
                                             CPU_INT08U         fnct,
                                             FunctionalState    new_state);

static  void         BSP_IOE_IOPin_Cfg      (CPU_INT08U         ioe_addr,
                                             CPU_INT08U         io_pin,
                                             CPU_INT08U         direction);

static  void         BSP_IOE_IOAF_Cfg       (CPU_INT08U         ioe_addr,
                                             CPU_INT08U         io_pin,
                                             FunctionalState    new_state);

static  void         BSP_IOE_IOEdge_Cfg     (CPU_INT08U         ioe_addr,
                                             CPU_INT08U         io_pin,
                                             CPU_INT08U         edge);

static  CPU_INT08U   BSP_IOE_WriteIOPin     (CPU_INT08U         io_pin,
                                             IOE_BitValue       bit_val);

static  CPU_INT08U   BSP_I2C_ReadDevReg     (CPU_INT08U         io_addr,
                                             CPU_INT08U         reg_addr);

static  CPU_INT08U  BSP_I2C_WriteDevReg     (CPU_INT08U         ioe_addr,
                                             CPU_INT08U         reg_addr,
                                             CPU_INT08U         reg_val);

static  void         BSP_IOE_DMA_Config     (IOE_DMADirection   direction,
                                             CPU_INT08U        *p_buf);

static  CPU_INT16U   BSP_I2C_ReadDataBuffer (CPU_INT08U         ioe_addr,
                                             CPU_INT08U         reg_addr);



/*
*********************************************************************************************************
*                                      LOCAL GLOBAL VARIABLRS
*********************************************************************************************************
*/

CPU_INT32U  IOE_TimeOut = TIMEOUT_MAX;

TS_STATE    TS_State;


/*
*********************************************************************************************************
*                                          BSP_IOE_Init()
*
* Description : Initialize I/O expander.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IOE_Init(void)
{

    BSP_IOE_GPIO_Cfg();                                         /* Cfg I/O expander GPIOs.                              */
    BSP_IOE_I2C_Cfg();                                          /* Cfg I2C.                                             */

    BSP_IOE_Reset(IOE_1_ADDR);                                  /* I/O expander #1 soft rst.                            */
    BSP_IOE_Reset(IOE_2_ADDR);                                  /* I/O expander #2 soft rst.                            */

    BSP_IOE_FnctCmd(IOE_1_ADDR, IOE_IO_FCT |                    /* En I/O expander #1 GPIO, touch screen & ADC.         */
                                IOE_TS_FCT |
                                IOE_ADC_FCT,
                                ENABLE);

    BSP_IOE_IOPin_Cfg(IOE_1_ADDR, VBAT_DIV_PIN , Direction_OUT);
    BSP_IOE_IOAF_Cfg( IOE_1_ADDR, VBAT_DIV_PIN, ENABLE);        /* Cfg alternate function for IN1 pin.                  */


    BSP_IOE_WriteIOPin(VBAT_DIV_PIN, BitReset);                 /* Apply the default state for the out pins.            */

    BSP_IOE_IOPin_Cfg(IOE_2_ADDR, (CPU_INT32U)(MEMS_INT1_PIN |  /* Cfg the MEMS interrupt pins in input mode.           */
                                               MEMS_INT2_PIN),
                                               Direction_IN);

    BSP_IOE_IOAF_Cfg(IOE_2_ADDR, (CPU_INT32U)(MEMS_INT1_PIN |   /* En the alternate function for the joystick pins.     */
                                              MEMS_INT2_PIN),
                                              ENABLE);

    BSP_IOE_IOEdge_Cfg(IOE_2_ADDR, (CPU_INT32U)(MEMS_INT1_PIN | MEMS_INT2_PIN),
                                   (CPU_INT32U)(EDGE_FALLING  | EDGE_RISING));

    BSP_IOE_TS_Cfg();                                           /* Cfg touch screen ctrl.                               */


    BSP_IOE_FnctCmd(IOE_2_ADDR, IOE_IO_FCT       |              /* En I/O expander #2 GPIO, temp sensor & ADC.          */
                                IOE_TEMPSENS_FCT |
                                IOE_ADC_FCT,
                                ENABLE);


    BSP_IOE_IOPin_Cfg(IOE_2_ADDR, (CPU_INT32U)(AUDIO_RESET_PIN), Direction_OUT);
    BSP_IOE_IOPin_Cfg(IOE_2_ADDR, (CPU_INT32U)(MII_INT_PIN),     Direction_IN);


    BSP_IOE_IOAF_Cfg(IOE_2_ADDR,  (CPU_INT32U)(AUDIO_RESET_PIN | MII_INT_PIN), ENABLE);


    BSP_IOE_WriteIOPin(AUDIO_RESET_PIN, BitReset);              /* Apply the default state for the out pins.            */
    BSP_IOE_WriteIOPin(MII_INT_PIN, BitReset);

    BSP_IOE_IOPin_Cfg(IOE_2_ADDR, JOY_IO_PINS , Direction_IN);
    BSP_IOE_IOAF_Cfg(IOE_2_ADDR, JOY_IO_PINS, ENABLE);          /* En the alternate function for the joystick pins.     */

    BSP_IOE_IOEdge_Cfg(IOE_2_ADDR, JOY_IO_PINS, (CPU_INT08U)(EDGE_FALLING | EDGE_RISING));

    BSP_IOE_TempSens_Cfg();                                     /* Cfg temperature sensor module.                       */

}


/*
*********************************************************************************************************
*                                      BSP_IOE_GPIO_Cfg()
*
* Description : Configures I/O expander GPIOs.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_GPIO_Cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    RCC_APB1PeriphClockCmd(IOE_I2C_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(IOE_I2C_SCL_GPIO_CLK |
                           IOE_I2C_SDA_GPIO_CLK |
                           IOE_IT_GPIO_CLK, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    RCC_APB1PeriphResetCmd(IOE_I2C_CLK, ENABLE);
    RCC_APB1PeriphResetCmd(IOE_I2C_CLK, DISABLE);

    GPIO_InitStructure.GPIO_Pin   = IOE_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_Init(IOE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = IOE_I2C_SDA_PIN;
    GPIO_Init(IOE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

    GPIO_PinAFConfig(IOE_I2C_SCL_GPIO_PORT, IOE_I2C_SCL_SOURCE, IOE_I2C_SCL_AF);
    GPIO_PinAFConfig(IOE_I2C_SDA_GPIO_PORT, IOE_I2C_SDA_SOURCE, IOE_I2C_SDA_AF);

    GPIO_InitStructure.GPIO_Pin = IOE_IT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(IOE_IT_GPIO_PORT, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(IOE_IT_EXTI_PORT_SOURCE, IOE_IT_EXTI_PIN_SOURCE);
}


/*
*********************************************************************************************************
*                                      BSP_IOE_I2C_Cfg()
*
* Description : Configures I2C.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_I2C_Cfg(void)
{
    I2C_InitTypeDef I2C_InitStructure;

    I2C_InitStructure.I2C_Mode                 = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle            = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1          = 0x00;
    I2C_InitStructure.I2C_Ack                  = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress  = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed           = I2C_SPEED;

    I2C_Init(IOE_I2C, &I2C_InitStructure);
}


/*
*********************************************************************************************************
*                                      BSP_IOE_Reset()
*
* Description : Reset the I/O expander.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_Reset(CPU_INT08U  ioe_addr)
{
    BSP_I2C_WriteDevReg(ioe_addr, IOE_REG_SYS_CTRL1, 0x02);

    BSP_OS_TimeDlyMs(20);

    BSP_I2C_WriteDevReg(ioe_addr, IOE_REG_SYS_CTRL1, 0x00);
}


/*
*********************************************************************************************************
*                                      BSP_IOE_FnctCmd()
*
* Description : Configures the selected I/O expander functionalities.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_FnctCmd(CPU_INT08U       ioe_addr,
                              CPU_INT08U       fnct,
                              FunctionalState  new_state)
{
    CPU_INT08U  reg_val;


    reg_val = BSP_I2C_ReadDevReg(ioe_addr, IOE_REG_SYS_CTRL2);
    if (new_state != DISABLE)  {
      reg_val &= ~(CPU_INT08U)fnct;
    } else {
      reg_val |=  (CPU_INT08U)fnct;
    }

    BSP_I2C_WriteDevReg(ioe_addr, IOE_REG_SYS_CTRL2, reg_val);
}


/*
*********************************************************************************************************
*                                      BSP_IOE_IOPin_Cfg()
*
* Description : Configure the selected pin direction.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_IOPin_Cfg(CPU_INT08U  ioe_addr,
                                CPU_INT08U  io_pin,
                                CPU_INT08U  direction)
{
    CPU_INT08U  reg_val;


    reg_val = BSP_I2C_ReadDevReg(ioe_addr, IOE_REG_GPIO_DIR);
    if (direction != Direction_IN)  {
      reg_val |=  (CPU_INT08U)io_pin;
    } else {
      reg_val &= ~(CPU_INT08U)io_pin;
    }

    BSP_I2C_WriteDevReg(ioe_addr, IOE_REG_GPIO_DIR, reg_val);
}

/*
*********************************************************************************************************
*                                      BSP_IOE_IOAF_Cfg()
*
* Description : Configure the selected pin to be in alternate function or not.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_IOAF_Cfg(CPU_INT08U       ioe_addr,
                               CPU_INT08U       io_pin,
                               FunctionalState  new_state)
{
    CPU_INT08U  reg_val;


    reg_val = BSP_I2C_ReadDevReg(ioe_addr, IOE_REG_GPIO_AF);
    if (new_state != DISABLE)  {
      reg_val |=  (CPU_INT08U)io_pin;
    } else {
      reg_val &= ~(CPU_INT08U)io_pin;
    }

    BSP_I2C_WriteDevReg(ioe_addr, IOE_REG_GPIO_AF, reg_val);
}


/*
*********************************************************************************************************
*                                      BSP_IOE_IOEdge_Cfg()
*
* Description : Configure the edge for which a transition is detectable for the selected pin.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_IOEdge_Cfg(CPU_INT08U  ioe_addr,
                                 CPU_INT08U  io_pin,
                                 CPU_INT08U  edge)
{
    CPU_INT08U  reg_val1;
    CPU_INT08U  reg_val2;

    reg_val1 = BSP_I2C_ReadDevReg(ioe_addr, IOE_REG_GPIO_FE);
    reg_val2 = BSP_I2C_ReadDevReg(ioe_addr, IOE_REG_GPIO_RE);

    reg_val1 &= ~(CPU_INT08U)io_pin;
    reg_val2 &= ~(CPU_INT08U)io_pin;

    if (edge & EDGE_FALLING)  {
      reg_val1 |= (CPU_INT08U)io_pin;
    }

    if (edge & EDGE_RISING)  {
      reg_val2 |= (CPU_INT08U)io_pin;
    }

    BSP_I2C_WriteDevReg(ioe_addr, IOE_REG_GPIO_FE, reg_val1);
    BSP_I2C_WriteDevReg(ioe_addr, IOE_REG_GPIO_RE, reg_val2);
}

/*
*********************************************************************************************************
*                                      BSP_IOE_WriteIOPin()
*
* Description :  Writes a bit value to an output I/O pin.
*
* Argument(s) : none.
*
* Return(s)   : PARAM_ERROR or IOE_OK.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT08U  BSP_IOE_WriteIOPin(CPU_INT08U    io_pin,
                                       IOE_BitValue  bit_val)
{
    CPU_INT08U  ioe_addr;


    if (io_pin & IO1_OUT_ALL_PINS) {
        ioe_addr = IOE_1_ADDR;
    } else if (io_pin & IO2_OUT_ALL_PINS)  {
        ioe_addr = IOE_2_ADDR;
    } else {
        return (PARAM_ERROR);
    }

    if (bit_val == BitReset) {
        BSP_I2C_WriteDevReg(ioe_addr, IOE_REG_GPIO_CLR_PIN, io_pin);
    } else {
        BSP_I2C_WriteDevReg(ioe_addr, IOE_REG_GPIO_SET_PIN, io_pin);
    }

    return IOE_OK;
}

/*
*********************************************************************************************************
*                                      BSP_IOE_TS_Cfg()
*
* Description :  Configures the touch screen controller (single point detection).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_TS_Cfg(void)
{
    CPU_INT08U  reg_val;


    reg_val  = BSP_I2C_ReadDevReg(IOE_1_ADDR, IOE_REG_SYS_CTRL2);
    reg_val &= ~(CPU_INT32U)(IOE_TS_FCT | IOE_ADC_FCT);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_SYS_CTRL2, reg_val);

    reg_val  = BSP_I2C_ReadDevReg(IOE_1_ADDR, IOE_REG_INT_EN);
    reg_val |= (CPU_INT32U)(IOE_GIT_TOUCH | IOE_GIT_FTH | IOE_GIT_FOV);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_INT_EN,   reg_val);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_ADC_CTRL1, 0x49);

    BSP_OS_TimeDlyMs(20);

    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_ADC_CTRL2, 0x01);
    reg_val  = BSP_I2C_ReadDevReg(IOE_1_ADDR, IOE_REG_GPIO_AF);
    reg_val &= ~(CPU_INT08U)TOUCH_IO_ALL;

    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_GPIO_AF,       reg_val);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_TSC_CFG,       0x9A);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_FIFO_TH,       0x01);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_FIFO_STA,      0x01);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_FIFO_STA,      0x00);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_TSC_FRACT_XYZ, 0x01);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_TSC_I_DRIVE,   0x01);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_TSC_CTRL,      0x01);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_INT_STA,       0xFF);

    TS_State.TouchDetected = TS_State.X = TS_State.Y = TS_State.Z = 0;
}

/*
*********************************************************************************************************
*                                      BSP_IOE_TempSens_Cfg()
*
* Description :  Configures the temperature sensor.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_TempSens_Cfg(void)
{
    CPU_INT08U  reg_val;


    reg_val  = BSP_I2C_ReadDevReg(IOE_2_ADDR, IOE_REG_SYS_CTRL2);
    reg_val &= ~(CPU_INT32U)(IOE_TEMPSENS_FCT | IOE_ADC_FCT);
    BSP_I2C_WriteDevReg(IOE_2_ADDR, IOE_REG_SYS_CTRL2, reg_val);

    BSP_I2C_WriteDevReg(IOE_2_ADDR, IOE_REG_TEMP_CTRL, 0x01);
    BSP_I2C_WriteDevReg(IOE_2_ADDR, IOE_REG_TEMP_CTRL, 0x3);
}


/*
*********************************************************************************************************
*                                      BSP_I2C_ReadDevReg()
*
* Description : Reads a register of the device through I2C.
*
* Argument(s) : none.
*
* Return(s)   : Value of the read register
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT08U  BSP_I2C_ReadDevReg(CPU_INT08U  io_addr,
                                       CPU_INT08U  reg_addr)
{
    CPU_INT08U IOE_BufferRX[2] = {0x00, 0x00};


    BSP_IOE_DMA_Config(IOE_DMA_RX, (CPU_INT08U *)IOE_BufferRX);
    I2C_DMALastTransferCmd(IOE_I2C, ENABLE);

    I2C_GenerateSTART(IOE_I2C, ENABLE);

    IOE_TimeOut = TIMEOUT_MAX;

    while (!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_SB)) {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_Send7bitAddress(IOE_I2C, io_addr, I2C_Direction_Transmitter);

    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_SendData(IOE_I2C, reg_addr);

    IOE_TimeOut = TIMEOUT_MAX;
    while ((!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_BTF)))  {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_GenerateSTART(IOE_I2C, ENABLE);

    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_SB))  {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_Send7bitAddress(IOE_I2C, io_addr, I2C_Direction_Receiver);

    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_DMACmd(IOE_I2C,ENABLE);

    DMA_Cmd(IOE_DMA_RX_STREAM, ENABLE);

    IOE_TimeOut = 2 * TIMEOUT_MAX;
    while (!DMA_GetFlagStatus(IOE_DMA_RX_STREAM,IOE_DMA_RX_TCFLAG))  {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_GenerateSTOP(IOE_I2C,  ENABLE);
    DMA_Cmd(IOE_DMA_RX_STREAM, DISABLE);

    I2C_DMACmd(IOE_I2C, DISABLE);
    DMA_ClearFlag(IOE_DMA_RX_STREAM,IOE_DMA_RX_TCFLAG);

    return (CPU_INT08U)IOE_BufferRX[0];
}

/*
*********************************************************************************************************
*                                      BSP_I2C_WriteDevReg()
*
* Description : Write a register of the device through I2C.
*
* Argument(s) : none.
*
* Return(s)   : IOE_OK if all operations are successfull.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT08U  BSP_I2C_WriteDevReg(CPU_INT08U  ioe_addr,
                                        CPU_INT08U  reg_addr,
                                        CPU_INT08U  reg_val)
{
    CPU_INT08U  read_verif;
    CPU_INT08U  IOE_BufferTX;

    IOE_BufferTX = reg_val;

    BSP_IOE_DMA_Config(IOE_DMA_TX, (uint8_t*)(&IOE_BufferTX));


    I2C_GenerateSTART(IOE_I2C, ENABLE);

    IOE_TimeOut = TIMEOUT_MAX;
    while (I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_SB) == RESET)
    {
      if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }
    I2C_Send7bitAddress(IOE_I2C, ioe_addr, I2C_Direction_Transmitter);

    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  {
      if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_SendData(IOE_I2C, reg_addr);

    IOE_TimeOut = TIMEOUT_MAX;
    while ((!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_BTF)))  {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }
    I2C_DMACmd(IOE_I2C,ENABLE);

    DMA_Cmd(IOE_DMA_TX_STREAM, ENABLE);

    IOE_TimeOut = TIMEOUT_MAX;
    while (!DMA_GetFlagStatus(IOE_DMA_TX_STREAM,IOE_DMA_TX_TCFLAG))  {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }
    IOE_TimeOut = 2 * TIMEOUT_MAX;
    while ((!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_BTF)))    {
    }

    I2C_GenerateSTOP(IOE_I2C, ENABLE);
    DMA_Cmd(IOE_DMA_TX_STREAM, DISABLE);

    I2C_DMACmd(IOE_I2C,DISABLE);
    DMA_ClearFlag(IOE_DMA_TX_STREAM,IOE_DMA_TX_TCFLAG);

    read_verif = IOE_OK;
#ifdef VERIFY_WRITTENDATA
    read_verif = I2C_ReadDeviceRegister(ioe_addr, reg_addr);
    if (read_verif != reg_val)  {
        read_verif = IOE_FAILURE;
    } else {
        read_verif = 0;
    }
#endif
    return (read_verif);
}


/*
*********************************************************************************************************
*                                      BSP_I2C_ReadDataBuffer()
*
* Description : Reads a buffer of 2 bytes from the device registers.
*
* Argument(s) : none.
*
* Return(s)   : A pointer to the buffer containing the two returned bytes.
*
* Caller(s)   : BSP_IOE_TS_Read_X(),
*               BSP_IOE_TS_Read_Y().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT16U  BSP_I2C_ReadDataBuffer(CPU_INT08U  ioe_addr,
                                           CPU_INT08U  reg_addr)
{
    CPU_INT08U  tmp= 0;
    CPU_INT08U  IOE_BufferRX[2] = {0x00, 0x00};

    BSP_IOE_DMA_Config(IOE_DMA_RX, (uint8_t*)IOE_BufferRX);
    I2C_DMALastTransferCmd(IOE_I2C, ENABLE);
    I2C_GenerateSTART(IOE_I2C,      ENABLE);

    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_SB)) {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_Send7bitAddress(IOE_I2C, ioe_addr, I2C_Direction_Transmitter);

    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_SendData(IOE_I2C, reg_addr);

    IOE_TimeOut = TIMEOUT_MAX;
    while ((!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_BTF))) {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_GenerateSTART(IOE_I2C, ENABLE);

    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_GetFlagStatus(IOE_I2C,I2C_FLAG_SB)) {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_Send7bitAddress(IOE_I2C, ioe_addr, I2C_Direction_Receiver);

    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_CheckEvent(IOE_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_DMACmd(IOE_I2C,ENABLE);

    DMA_Cmd(IOE_DMA_RX_STREAM, ENABLE);

    IOE_TimeOut = 2 * TIMEOUT_MAX;
    while (!DMA_GetFlagStatus(IOE_DMA_RX_STREAM, IOE_DMA_RX_TCFLAG)) {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    I2C_GenerateSTOP(IOE_I2C, ENABLE);

    DMA_Cmd(IOE_DMA_RX_STREAM, DISABLE);
    I2C_DMACmd(IOE_I2C,DISABLE);

    DMA_ClearFlag(IOE_DMA_RX_STREAM,IOE_DMA_RX_TCFLAG);

    tmp = IOE_BufferRX[0];
    IOE_BufferRX[0] = IOE_BufferRX[1];
    IOE_BufferRX[1] = tmp;

    return (*(CPU_INT16U *) IOE_BufferRX);
}


/*
*********************************************************************************************************
*                                      BSP_IOE_TS_GetState()
*
* Description : Returns status and positions of the touch screen.
*
* Argument(s) : none.
*
* Return(s)   : Status and positions of the touch screen
*
* Caller(s)   : Various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

TS_STATE* BSP_IOE_TS_GetState (void)
{
    CPU_INT16U      xDiff;
    CPU_INT16U      yDiff;
    CPU_INT16U      x;
    CPU_INT16U      y;

    static CPU_INT16U _x = 0;
    static CPU_INT16U _y = 0;

    /* Check if the Touch detect event happened */
    TS_State.TouchDetected = (BSP_I2C_ReadDevReg(IOE_1_ADDR, IOE_REG_TSC_CTRL) & 0x80);
    if(TS_State.TouchDetected) {
        x = BSP_IOE_TS_Read_X();
        y = BSP_IOE_TS_Read_Y();
        xDiff = x > _x? (x - _x): (_x - x);
        yDiff = y > _y? (y - _y): (_y - y);
        if (xDiff + yDiff > 5) {
          _x = x;
          _y = y;
        }
    }

    TS_State.X = _x;
    TS_State.Y = _y;

    TS_State.Z = BSP_IOE_TS_Read_Z();

    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x01);
    BSP_I2C_WriteDevReg(IOE_1_ADDR, IOE_REG_FIFO_STA, 0x00);

    return &TS_State;
}

/*
*********************************************************************************************************
*                                      BSP_IOE_TS_Read_X()
*
* Description : Return touch screen X position value.
*
* Argument(s) : none.
*
* Return(s)   : Touch screen X position value.
*
* Caller(s)   : BSP_IOE_TS_GetState().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16U  BSP_IOE_TS_Read_X (void)
{
    CPU_INT32U  x;
    CPU_INT32U  xr;


    x  = BSP_I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_Y);

    xr =  (x * 320) >> 12;
    xr = ((xr * 32)/29) - 17;

    if(xr <= 0)
        xr = 0;

    return (CPU_INT16U)xr;
}


/*
*********************************************************************************************************
*                                      BSP_IOE_TS_Read_Y()
*
* Description : Return touch screen Y position value.
*
* Argument(s) : none.
*
* Return(s)   : Touch screen Y position value.
*
* Caller(s)   : BSP_IOE_TS_GetState().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16U  BSP_IOE_TS_Read_Y (void)
{
    CPU_INT32U  y;
    CPU_INT32U  yr;


    y  = BSP_I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_X);

    yr = (y * 240) >> 12;
    yr = ((yr * 240) / 217) - 12;

    if(yr <= 0)
      yr = 0;

    return (CPU_INT16U)yr;
}

/*
*********************************************************************************************************
*                                      BSP_IOE_TS_Read_Z()
*
* Description : Return touch screen Z position value.
*
* Argument(s) : none.
*
* Return(s)   : Touch screen Z position value.
*
* Caller(s)   : BSP_IOE_TS_GetState().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16U  BSP_IOE_TS_Read_Z(void)
{
    CPU_INT32U z;


   z = BSP_I2C_ReadDataBuffer(IOE_1_ADDR, IOE_REG_TSC_DATA_Z);
   if(z <= 0)
        z = 0;

    return (CPU_INT16U)z;
}


/*
*********************************************************************************************************
*                                      BSP_IOE_DMA_Config()
*
* Description : Configure the DMA Peripheral used to handle communication via I2C.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_LCD_Touch_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IOE_DMA_Config(IOE_DMADirection    direction,
                                 CPU_INT08U         *p_buf)
{
    DMA_InitTypeDef DMA_InitStructure;


    RCC_AHB1PeriphClockCmd(IOE_DMA_CLK, ENABLE);

    DMA_InitStructure.DMA_Channel            = IOE_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = IOE_I2C_DR;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (CPU_INT32U)p_buf;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;

    if (direction == IOE_DMA_RX) {
        DMA_InitStructure.DMA_DIR        = DMA_DIR_PeripheralToMemory;
        DMA_InitStructure.DMA_BufferSize = 2;
        DMA_DeInit(IOE_DMA_RX_STREAM);
        DMA_Init(IOE_DMA_RX_STREAM, &DMA_InitStructure);
    } else if (direction == IOE_DMA_TX)  {
        DMA_InitStructure.DMA_DIR        = DMA_DIR_MemoryToPeripheral;
        DMA_InitStructure.DMA_BufferSize = 1;
        DMA_DeInit(IOE_DMA_TX_STREAM);
        DMA_Init(IOE_DMA_TX_STREAM, &DMA_InitStructure);
    }
}

