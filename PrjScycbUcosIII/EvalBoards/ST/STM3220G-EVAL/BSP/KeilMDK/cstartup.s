;/*
;********************************************************************************************************
;                                    EXCEPTION VECTORS & STARTUP CODE
;
; File      : cstartup.s
; For       : ARMv7M Cortex-M4
; Mode      : Thumb2
; Toolchain : RealView Development Suite
;             RealView Microcontroller Development Kit (MDK)
;             ARM Developer Suite (ADS)
;             Keil uVision
;********************************************************************************************************
;*/

;/*
;********************************************************************************************************
;*                           <<< Use Configuration Wizard in Context Menu >>>
;*
;* Note(s) : (1) The µVision4 Configuration Wizard enables menu driven configuration of assembler, 
;*               C/C++, or debugger initialization files. The Configuration Wizard uses control items 
;*               that are embedded into the comments of the configuration file.
;*
;********************************************************************************************************
;*/

;/*
;********************************************************************************************************
;*                                              STACK DEFINITIONS
;*
;* Configuration Wizard Menu:
;* // <h> Stack Configuration
;* //   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;* // </h>;
;*********************************************************************************************************
;*/

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


;/*
;********************************************************************************************************
;*                                              STACK DEFINITIONS
; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
;*********************************************************************************************************
;*/

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

                                                      ; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                
                IMPORT  BSP_IntHandlerWWDG
                IMPORT  BSP_IntHandlerPVD
                IMPORT  BSP_IntHandlerTAMPER
                IMPORT  BSP_IntHandlerRTC
                IMPORT  BSP_IntHandlerFLASH
                IMPORT  BSP_IntHandlerRCC
                IMPORT  BSP_IntHandlerEXTI0
                IMPORT  BSP_IntHandlerEXTI1
                IMPORT  BSP_IntHandlerEXTI2
                IMPORT  BSP_IntHandlerEXTI3
                IMPORT  BSP_IntHandlerEXTI4
                IMPORT  BSP_IntHandlerDMA1_CH1
                IMPORT  BSP_IntHandlerDMA1_CH2
                IMPORT  BSP_IntHandlerDMA1_CH3
                IMPORT  BSP_IntHandlerDMA1_CH4
                IMPORT  BSP_IntHandlerDMA1_CH5
                IMPORT  BSP_IntHandlerDMA1_CH6
                IMPORT  BSP_IntHandlerDMA1_CH7
                IMPORT  BSP_IntHandlerADC1_2
                IMPORT  BSP_IntHandlerCAN1_TX
                IMPORT  BSP_IntHandlerCAN1_RX0
                IMPORT  BSP_IntHandlerCAN1_RX1
                IMPORT  BSP_IntHandlerCAN1_SCE
                IMPORT  BSP_IntHandlerEXTI9_5
                IMPORT  BSP_IntHandlerTIM1_BRK
                IMPORT  BSP_IntHandlerTIM1_UP
                IMPORT  BSP_IntHandlerTIM1_TRG_COM
                IMPORT  BSP_IntHandlerTIM1_CC
                IMPORT  BSP_IntHandlerTIM2
                IMPORT  BSP_IntHandlerTIM3
                IMPORT  BSP_IntHandlerTIM4
                IMPORT  BSP_IntHandlerI2C1_EV
                IMPORT  BSP_IntHandlerI2C1_ER
                IMPORT  BSP_IntHandlerI2C2_EV
                IMPORT  BSP_IntHandlerI2C2_ER
                IMPORT  BSP_IntHandlerSPI1
                IMPORT  BSP_IntHandlerSPI2
                IMPORT  BSP_IntHandlerUSART1
                IMPORT  BSP_IntHandlerUSART2
                IMPORT  BSP_IntHandlerUSART3
                IMPORT  BSP_IntHandlerEXTI15_10
                IMPORT  BSP_IntHandlerRTCAlarm
                IMPORT  BSP_IntHandlerUSBWakeUp
                IMPORT  BSP_IntHandlerTIM8_BRK_TIM12
                IMPORT  BSP_IntHandlerTIM8_UP_TIM13
                IMPORT  BSP_IntHandlerTIM8_TRG_COM_TIM14
                IMPORT  BSP_IntHandlerTIM8_CC
                IMPORT  BSP_IntHandlerDMA1_STREAM7
                IMPORT  BSP_IntHandlerFSMC
                IMPORT  BSP_IntHandlerSDIO
                IMPORT  BSP_IntHandlerTIM5
                IMPORT  BSP_IntHandlerSPI3
                IMPORT  BSP_IntHandlerUSART4
                IMPORT  BSP_IntHandlerUSART5
                IMPORT  BSP_IntHandlerTIM6
                IMPORT  BSP_IntHandlerTIM7
                IMPORT  BSP_IntHandlerDMA2_CH1
                IMPORT  BSP_IntHandlerDMA2_CH2
                IMPORT  BSP_IntHandlerDMA2_CH3
                IMPORT  BSP_IntHandlerDMA2_CH4
                IMPORT  BSP_IntHandlerDMA2_CH5
                IMPORT  BSP_IntHandlerETH
                IMPORT  BSP_IntHandlerETHWakeup
                IMPORT  BSP_IntHandlerCAN2_TX
                IMPORT  BSP_IntHandlerCAN2_RX0
                IMPORT  BSP_IntHandlerCAN2_RX1
                IMPORT  BSP_IntHandlerCAN2_SCE
                IMPORT  BSP_IntHandlerOTG
                IMPORT  BSP_IntHandlerDMA2_CH6
                IMPORT  BSP_IntHandlerDMA2_CH7
                IMPORT  BSP_IntHandlerDMA2_CH8
                IMPORT  BSP_IntHandlerUSART6
                IMPORT  BSP_IntHandlerI2C3_EV
                IMPORT  BSP_IntHandlerI2C3_ER
                IMPORT  BSP_IntHandlerOTG_HS_EP1_OUT
                IMPORT  BSP_IntHandlerOTG_HS_EP1_IN
                IMPORT  BSP_IntHandlerOTG_HS_WKUP
                IMPORT  BSP_IntHandlerOTG_HS
                IMPORT  BSP_IntHandlerDCMI
                IMPORT  BSP_IntHandlerCRYP
                IMPORT  BSP_IntHandlerHASH_RNG
                
                IMPORT  OS_CPU_PendSVHandler                                        
                IMPORT  OS_CPU_SysTickHandler  

__Vectors       DCD     __initial_sp                        ;/*  0, Top of Stack
                DCD     Reset_Handler                       ;/*  1, Reset Handler
                DCD     App_NMI_ISR                         ;/*  2, NMI.                                               */
                DCD     App_Fault_ISR                       ;/*  3, Hard Fault.                                        */
                DCD     App_MemFault_ISR                    ;/*  4, Memory Management.                                 */
                DCD     App_BusFault_ISR                    ;/*  5, Bus Fault.                                         */
                DCD     App_UsageFault_ISR                  ;/*  6, Usage Fault.                                       */
                DCD     App_Spurious_ISR                    ;/*  7, Reserved.                                          */
                DCD     App_Spurious_ISR                    ;/*  8, Reserved.                                          */
                DCD     App_Spurious_ISR                    ;/*  9, Reserved.                                          */
                DCD     App_Spurious_ISR                    ;/* 10, Reserved.                                          */
                DCD     App_Spurious_ISR                    ;/* 11, SVCall.                                            */
                DCD     App_Spurious_ISR                    ;/* 12, Debug Monitor.                                     */
                DCD     App_Spurious_ISR                    ;/* 13, Reserved.                                          */
                DCD     OS_CPU_PendSVHandler                ;/* 14, PendSV Handler.                                    */
                DCD     OS_CPU_SysTickHandler               ;/* 15, uC/OS-III Tick ISR Handler.                        */

                                                            ; External Interrupts
                DCD     BSP_IntHandlerWWDG                  ;/* 16, INTISR[  0]  Window Watchdog.                    */
                DCD     BSP_IntHandlerPVD                   ;/* 17, INTISR[  1]  PVD through EXTI Line Detection.    */
                DCD     BSP_IntHandlerTAMPER                ;/* 18, INTISR[  2]  Tamper Interrupt.                   */
                DCD     BSP_IntHandlerRTC                   ;/* 19, INTISR[  3]  RTC Global Interrupt.               */
                DCD     BSP_IntHandlerFLASH                 ;/* 20, INTISR[  4]  FLASH Global Interrupt.             */
                DCD     BSP_IntHandlerRCC                   ;/* 21, INTISR[  5]  RCC Global Interrupt.               */
                DCD     BSP_IntHandlerEXTI0                 ;/* 22, INTISR[  6]  EXTI Line0 Interrupt.               */
                DCD     BSP_IntHandlerEXTI1                 ;/* 23, INTISR[  7]  EXTI Line1 Interrupt.               */
                DCD     BSP_IntHandlerEXTI2                 ;/* 24, INTISR[  8]  EXTI Line2 Interrupt.               */
                DCD     BSP_IntHandlerEXTI3                 ;/* 25, INTISR[  9]  EXTI Line3 Interrupt.               */
                DCD     BSP_IntHandlerEXTI4                 ;/* 26, INTISR[ 10]  EXTI Line4 Interrupt.               */
                DCD     BSP_IntHandlerDMA1_CH1              ;/* 27, INTISR[ 11]  DMA Channel1 Global Interrupt.      */
                DCD     BSP_IntHandlerDMA1_CH2              ;/* 28, INTISR[ 12]  DMA Channel2 Global Interrupt.      */
                DCD     BSP_IntHandlerDMA1_CH3              ;/* 29, INTISR[ 13]  DMA Channel3 Global Interrupt.      */
                DCD     BSP_IntHandlerDMA1_CH4              ;/* 30, INTISR[ 14]  DMA Channel4 Global Interrupt.      */
                DCD     BSP_IntHandlerDMA1_CH5              ;/* 31, INTISR[ 15]  DMA Channel5 Global Interrupt.      */
                DCD     BSP_IntHandlerDMA1_CH6              ;/* 32, INTISR[ 16]  DMA Channel6 Global Interrupt.      */
                DCD     BSP_IntHandlerDMA1_CH7              ;/* 33, INTISR[ 17]  DMA Channel7 Global Interrupt.      */
                DCD     BSP_IntHandlerADC1_2                ;/* 34, INTISR[ 18]  ADC1 & ADC2 Global Interrupt.       */
                DCD     BSP_IntHandlerCAN1_TX               ;/* 35, INTISR[ 19]  USB High Prio / CAN TX  Interrupts. */
                DCD     BSP_IntHandlerCAN1_RX0              ;/* 36, INTISR[ 20]  USB Low  Prio / CAN RX0 Interrupts. */
                DCD     BSP_IntHandlerCAN1_RX1              ;/* 37, INTISR[ 21]  CAN RX1 Interrupt.                  */
                DCD     BSP_IntHandlerCAN1_SCE              ;/* 38, INTISR[ 22]  CAN SCE Interrupt.                  */
                DCD     BSP_IntHandlerEXTI9_5               ;/* 39, INTISR[ 23]  EXTI Line[9:5] Interrupt.           */
                DCD     BSP_IntHandlerTIM1_BRK              ;/* 40, INTISR[ 24]  TIM1 Break  Interrupt.              */
                DCD     BSP_IntHandlerTIM1_UP               ;/* 41, INTISR[ 25]  TIM1 Update Interrupt.              */
                DCD     BSP_IntHandlerTIM1_TRG_COM          ;/* 42, INTISR[ 26]  TIM1 Trig & Commutation Interrupts. */
                DCD     BSP_IntHandlerTIM1_CC               ;/* 43, INTISR[ 27]  TIM1 Capture Compare Interrupt.     */
                DCD     BSP_IntHandlerTIM2                  ;/* 44, INTISR[ 28]  TIM2 Global Interrupt.              */
                DCD     BSP_IntHandlerTIM3                  ;/* 45, INTISR[ 29]  TIM3 Global Interrupt.              */
                DCD     BSP_IntHandlerTIM4                  ;/* 46, INTISR[ 30]  TIM4 Global Interrupt.              */
                DCD     BSP_IntHandlerI2C1_EV               ;/* 47, INTISR[ 31]  I2C1 Event  Interrupt.              */
                DCD     BSP_IntHandlerI2C1_ER               ;/* 48, INTISR[ 32]  I2C1 Error  Interrupt.              */
                DCD     BSP_IntHandlerI2C2_EV               ;/* 49, INTISR[ 33]  I2C2 Event  Interrupt.              */
                DCD     BSP_IntHandlerI2C2_ER               ;/* 50, INTISR[ 34]  I2C2 Error  Interrupt.              */
                DCD     BSP_IntHandlerSPI1                  ;/* 51, INTISR[ 35]  SPI1 Global Interrupt.              */
                DCD     BSP_IntHandlerSPI2                  ;/* 52, INTISR[ 36]  SPI2 Global Interrupt.              */
                DCD     BSP_IntHandlerUSART1                ;/* 53, INTISR[ 37]  USART1 Global Interrupt.            */
                DCD     BSP_IntHandlerUSART2                ;/* 54, INTISR[ 38]  USART2 Global Interrupt.            */
                DCD     BSP_IntHandlerUSART3                ;/* 55, INTISR[ 39]  USART3 Global Interrupt.            */
                DCD     BSP_IntHandlerEXTI15_10             ;/* 56, INTISR[ 40]  EXTI Line [15:10] Interrupts.       */
                DCD     BSP_IntHandlerRTCAlarm              ;/* 57, INTISR[ 41]  RTC Alarm EXT Line Interrupt.       */
                DCD     BSP_IntHandlerUSBWakeUp             ;/* 58, INTISR[ 42]  USB Wakeup from Suspend EXTI Int.   */
                DCD     BSP_IntHandlerTIM8_BRK_TIM12        ;/* 59, INTISR[ 43]  TIM8 Break and TIM12 global Int.    */
                DCD     BSP_IntHandlerTIM8_UP_TIM13         ;/* 60, INTISR[ 44]  TIM8 Update and TIM13 global Int.   */
                DCD     BSP_IntHandlerTIM8_TRG_COM_TIM14    ;/* 61, INTISR[ 45]  TIM8 Trigger/Commutation and TIM14. */
                DCD     BSP_IntHandlerTIM8_CC               ;/* 62, INTISR[ 46]  TIM8 Capture Compare Int.           */
                DCD     BSP_IntHandlerDMA1_STREAM7          ;/* 63, INTISR[ 47]  DMA1 Stream7 Int.                   */
                DCD     BSP_IntHandlerFSMC                  ;/* 64, INTISR[ 48]  FSMC global Int.                    */
                DCD     BSP_IntHandlerSDIO                  ;/* 65, INTISR[ 49]  SDIO global Int.                    */
                DCD     BSP_IntHandlerTIM5                  ;/* 66, INTISR[ 50]  TIM5 global Interrupt.              */
                DCD     BSP_IntHandlerSPI3                  ;/* 67, INTISR[ 51]  SPI3 global Interrupt.              */
                DCD     BSP_IntHandlerUSART4                ;/* 68, INTISR[ 52]  UART4 global Interrupt.             */
                DCD     BSP_IntHandlerUSART5                ;/* 69, INTISR[ 53]  UART5 global Interrupt.             */
                DCD     BSP_IntHandlerTIM6                  ;/* 70, INTISR[ 54]  TIM6 global Interrupt.              */
                DCD     BSP_IntHandlerTIM7                  ;/* 71, INTISR[ 55]  TIM7 global Interrupt.              */
                DCD     BSP_IntHandlerDMA2_CH1              ;/* 72, INTISR[ 56]  DMA2 Channel 1 global Interrupt.    */
                DCD     BSP_IntHandlerDMA2_CH2              ;/* 73, INTISR[ 57]  DMA2 Channel 2 global Interrupt.    */
                DCD     BSP_IntHandlerDMA2_CH3              ;/* 74, INTISR[ 58]  DMA2 Channel 3 global Interrupt.    */
                DCD     BSP_IntHandlerDMA2_CH4              ;/* 75, INTISR[ 59]  DMA2 Channel 4 global Interrupt.    */
                DCD     BSP_IntHandlerDMA2_CH5              ;/* 76, INTISR[ 60]  DMA2 Channel 5 global Interrupt.    */
                DCD     BSP_IntHandlerETH                   ;/* 77, INTISR[ 61]  ETH global Interrupt.               */
                DCD     BSP_IntHandlerETHWakeup             ;/* 78, INTISR[ 62]  ETH WakeUp from EXTI line Int.      */
                DCD     BSP_IntHandlerCAN2_TX               ;/* 79, INTISR[ 63]  CAN2 TX Interrupts.                 */
                DCD     BSP_IntHandlerCAN2_RX0              ;/* 80, INTISR[ 64]  CAN2 RX0 Interrupts.                */
                DCD     BSP_IntHandlerCAN2_RX1              ;/* 81, INTISR[ 65]  CAN2 RX1 Interrupt.                 */
                DCD     BSP_IntHandlerCAN2_SCE              ;/* 82, INTISR[ 66]  CAN2 SCE Interrupt.                 */
                DCD     BSP_IntHandlerOTG                   ;/* 83, INTISR[ 67]  OTG global Interrupt.               */
                DCD     BSP_IntHandlerDMA2_CH6              ;/* 84, INTISR[ 68]  DMA2 Channel 6 global Interrupt.    */
                DCD     BSP_IntHandlerDMA2_CH7              ;/* 85, INTISR[ 69]  DMA2 Channel 7 global Interrupt.    */
                DCD     BSP_IntHandlerDMA2_CH8              ;/* 86, INTISR[ 70]  DMA2 Channel 8 global Interrupt.    */
                DCD     BSP_IntHandlerUSART6                ;/* 87, INTISR[ 71]  UART6 global Interrupt.             */
                DCD     BSP_IntHandlerI2C3_EV               ;/* 88, INTISR[ 72]  I2C3 Event  Interrupt.              */
                DCD     BSP_IntHandlerI2C3_ER               ;/* 89, INTISR[ 73]  I2C3 Error  Interrupt.              */
                DCD     BSP_IntHandlerOTG_HS_EP1_OUT        ;/* 90, INTISR[ 74]  OTG HS EP1 Output Interrupt.        */
                DCD     BSP_IntHandlerOTG_HS_EP1_IN         ;/* 91, INTISR[ 75]  OTG HS EP1 Input Interrupt.         */
                DCD     BSP_IntHandlerOTG_HS_WKUP           ;/* 92, INTISR[ 76]  OTG HS Wakeup Interrupt.            */
                DCD     BSP_IntHandlerOTG_HS                ;/* 93, INTISR[ 77]  OTG HS Interrupt.                   */
                DCD     BSP_IntHandlerDCMI                  ;/* 94, INTISR[ 78]  DCMI Interrupt.                     */
                DCD     BSP_IntHandlerCRYP                  ;/* 95, INTISR[ 79]  CRYPT Interrupt.                    */
                DCD     BSP_IntHandlerHASH_RNG              ;/* 96, INTISR[ 80]  HASH RNG Interrupt.                 */ 
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  __main
                
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)                

App_NMI_ISR      PROC
                EXPORT  App_NMI_ISR                   [WEAK]
                B       .
                ENDP
App_Fault_ISR\
                PROC
                EXPORT  App_Fault_ISR                 [WEAK]
                B       .
                ENDP
App_MemFault_ISR\
                PROC
                EXPORT  App_MemFault_ISR             [WEAK]
                B       .
                ENDP
App_BusFault_ISR\
                PROC
                EXPORT  App_BusFault_ISR             [WEAK]
                B       .
                ENDP
App_UsageFault_ISR\
                PROC
                EXPORT  App_UsageFault_ISR            [WEAK]
                B       .
                ENDP
App_Spurious_ISR\
                PROC
                EXPORT  App_Spurious_ISR                [WEAK]
                B       .
                ENDP
App_Reserved_ISR\
                PROC
                EXPORT  App_Reserved_ISR                [WEAK]
                B       .
                ENDP


                ALIGN


                                                                ; User Inital Stack & Heap
                 IF      :DEF:__MICROLIB
                
                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit
                
                 ELSE
                
                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap
                 
__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END
