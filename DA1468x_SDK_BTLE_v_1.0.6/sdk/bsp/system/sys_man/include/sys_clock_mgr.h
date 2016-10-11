/**
 \addtogroup BSP
 \{
 \addtogroup SYSTEM
 \{
 \addtogroup CLOCK_MANAGER
 * 
 * \brief Clock Manager
 *
 \{
 */

/**
 ****************************************************************************************
 *
 * @file sys_clock_mgr.h
 *
 * @brief Clock Manager header file.
 *
 * Copyright (C) 2015. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd. All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 * <black.orca.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */

#ifndef SYS_CLOCK_MGR_H_
#define SYS_CLOCK_MGR_H_

#include <stdint.h>
#include <stdbool.h>
#include "hw_cpm.h"

/**
 * \brief Initialize clocks after power-up.
 *
 * \param[in] type The clock source to use as the system clock.
 *
 * \return void
 *
 * \warning It must be called with interrupts enabled! It is called only once, after power-up.
 * 
 */
void cm_sys_clk_init(sys_clk_t type);

/**
 * \brief Set the system clock.
 * 
 * \details It attempts to set the sys_clk to one of the available options. If it is not possible 
 *          because, for example, it was requested to switch to RC16 and a MAC or the
 *          UART, etc., runs, it returns false. If the request involves turning on the XTAL16M, 
 *          which was turned-off for some reason, then the task will block and the XTAL16M will be 
 *          powered on. The task will resume execution when the XTAL16M settles. The CM will restore 
 *          the last setting after each wake-up, automatically, whenever the XTAL16M settles. 
 *          Note: if the SysTick runs then it is the dg_configABORT_IF_SYSTICK_CLK_ERR setting that 
 *          controls whether the switch will be aborted or not.
 *
 * \param[in] type The clock source to use as the system clock.
 *
 * \return True if the requested clock switch was applied, else false.
 *
 * \warning It may block. It cannot be called from Interrupt Context.
 * 
 */
bool cm_sys_clk_set(sys_clk_t type);

/**
 * \brief Set the CPU clock.
 * 
 * \details It attempts to set the sys_clk and the AMBA High speed bus divider to achieve the CPU
 *          clock that is requested. ARM Cortex-M0 runs using the AHB clock. Any restrictions of the
 *          cm_sys_clk_set(), cm_ahb_set_clock_divider() and cm_apb_set_clock_divider() apply here
 *          as well. The APB bus clock will be set to the maximum frequency.
 *          The function may return false if the requested frequency is not achievable. For 
 *          example, the frequency of 1MHz can be achieved only if the RC16 or the XTAL16M is used.
 *          If the system clock is not RC16 though (see warning) and the crystal connected to the 
 *          XTAL16M is 32MHz then it is not possible to get 1MHz AHB clock frequency. The smallest
 *          that can be achieved is 2MHz.
 *
 * \param[in] clk The CPU clock frequency.
 *
 * \return True if the requested clock switch was applied, else false.
 *
 * \warning Since some frequencies can be achieved with RC16, this function will not change to using 
 *          the XTAL or the PLL, if the RC16 is the current system clock. It is the responsibility 
 *          of the caller to switch to the XTAL or the PLL before calling this function. After 
 *          switching, the function will not revert to using the RC16 at any case. Thus, switching
 *          from/to RC16 may be considered as "manual" while the switching from/to any other system
 *          clock source is done automatically from this function.
 *          The setting of the clocks is done via calls to cm_sys_clk_set(), 
 *          cm_ahb_set_clock_divider() and cm_apb_set_clock_divider(). If the task that has called
 *          this function is suspended and another task changes any of these clocks then the result
 *          will not be the intended one.
 *          It may block. It cannot be called from Interrupt Context.
 * 
 */
bool cm_cpu_clk_set(cpu_clk_t clk);

/**
 * \brief Set the system and the AHB bus clock (interrupt safe version).
 * 
 * \details It sets the sys_clk to XTAL16M or PLL and the AHB divider. Actually, it sets the clock
 *          frequency that the ARM Cortex-M0 will run. 
 *
 * \param[in] a The clock source to use as the system clock.
 * \param[in] b The divider of the AHB clock.
 *
 * \return void
 *
 * \warning It is called with interrupts disabled. The caller must have checked that the current 
 *          sys_clk is not the desired one before calling this function.
 * 
 */
void cm_cpu_clk_set_fromISR(sys_clk_t a, ahb_div_t b);

/**
 * \brief Change the divider of the AMBA Peripheral Bus clock.
 * 
 * \details The frequency of the APB clock is: 
 *              (cm_sysclk * 16 / (1 << cm_ahbclk)) / (1 << cm_apbclk).
 *              
 * \param[in] div The new value of the APB divider.
 * 
 * \return void
 *
 * \warning It may block. It cannot be called from Interrupt Context.
 * 
 */
void cm_apb_set_clock_divider(apb_div_t div);

/**
 * \brief Change the divider of the AMBA High speed Bus clock.
 * 
 * \details The frequency of the AHB clock is: 
 *              (cm_sysclk * 16 / (1 << cm_ahbclk)).
 *          Note: if the SysTick runs then it is the dg_configABORT_IF_SYSTICK_CLK_ERR setting that 
 *          controls whether the switch will be aborted or not.
 * 
 * \param[in] div The new value of the AHB divider.
 * 
 * \return True if the divider was changed to the requested value, else false.
 *
 * \warning It may block. It cannot be called from Interrupt Context.
 * 
 */
bool cm_ahb_set_clock_divider(ahb_div_t div);

/**
 * \brief Returns the sys_clk that the system uses at that moment.
 * 
 * \return The real sys_clk used by the system.
 *
 * \warning It may block. It cannot be called from Interrupt Context.
 * 
 */
sys_clk_t cm_sys_clk_get(void);

/**
 * \brief Returns the sys_clk that the system uses at that moment (interrupt safe version).
 * 
 * \return The real sys_clk used by the system.
 *
 */
sys_clk_t cm_sys_clk_get_fromISR(void);

/**
 * \brief Returns the AMBA Peripheral Bus clock divider.
 * 
 * \return The pclk being used.
 *
 * \warning It may block. It cannot be called from Interrupt Context.
 * 
 */
apb_div_t cm_apb_get_clock_divider(void);

/**
 * \brief Returns the AMBA Peripheral Bus clock divider (interrupt safe).
 *
 * \return The pclk being used.
 *
 * \warning It can be called from Interrupt Context.
 *
 */
apb_div_t cm_apb_get_clock_divider_fromISR(void);

/**
 * \brief Returns the AMBA High speed Bus clock divider.
 * 
 * \return The hclk being used.
 *
 * \warning It may block. It cannot be called from Interrupt Context.
 * 
 */
ahb_div_t cm_ahb_get_clock_divider(void);

/**
 * \brief Returns the AMBA High speed Bus clock divider (interrupt safe).
 *
 * \return The hclk being used.
 *
 * \warning It can be called from Interrupt Context.
 *
 */
ahb_div_t cm_ahb_get_clock_divider_fromISR(void);

/**
 * \brief Returns the CPU clock frequency.
 * 
 * \return The CPU clock being used.
 *
 * \warning Any restrictions of the cm_sys_clk_get() and cm_ahb_get_clock_divider() apply here as 
 *          well. It may block. It cannot be called from Interrupt Context.
 * 
 */
cpu_clk_t cm_cpu_clk_get(void);

/**
 * \brief Returns the CPU clock frequency (interrupt safe).
 *
 * \return The CPU clock being used.
 *
 * \warning It can be called from Interrupt Context.
 *
 */
cpu_clk_t cm_cpu_clk_get_fromISR(void);

/**
 * \brief Calibrate RC32K.
 * 
 * \return void
 *
 */
void cm_calibrate_rc32k(void);

/**
 * \brief Start RCX calibration.
 *
 * \return void
 *
 */
__RETAINED_CODE void cm_calibrate_rcx_start(void);

/**
 * \brief Get RCX calibration results and update environment variables.
 *
 * \return True, if calibration has finished and sleep can continue, else false.
 *
 */
bool cm_calibrate_rcx_update(void);

/**
 * \brief Converts usec to RCX cycles.
 *
 * \return The number of RCX cycles for the given time period.
 *
 * \warning Maximum time period is 4.095msec.
 *
 */
__RETAINED_CODE uint32_t cm_rcx_us_2_lpcycles(uint32_t usec);

/**
 * \brief Converts time to RCX cycles.
 *
 * \return The number of RCX cycles for the given time period.
 *
 * \warning This is a low accuracy function. To have good accuracy, the minimum time period should
 *        be 1msec and the maximum 200msec. Above 200msec, the function calculates more RCX cycles
 *        than necessary.
 *
 */
uint32_t cm_rcx_us_2_lpcycles_low_acc(uint32_t usec);

/**
 * \brief Set proper trim values to RC32K, XTAL16M and RC16.
 * 
 * \return void
 *
 * \deprecated This functionality is part of the system initialization.
 *
 */
void cm_set_trim_values(void) DEPRECATED;

/**
 * \brief Block until the XTAL16M is ready. If the XTAL16M is running then the function exits 
 *        immediately.
 * 
 * \return void
 *
 * \warning It may block. It cannot be called from Interrupt Context.
 * 
 */
void cm_wait_xtal16m_ready(void);

/**
 * \brief Initialize the Low Power clock.
 * 
 * \details It initializes and sets as LP clock either the RCX or the XTAL32K. Since the XTAL32K
 *          settling takes a long time, the system is kept in active mode until this completes. 
 * 
 * \return void
 *
 */
void cm_lp_clk_init(void);

/**
 * \brief Start the timer the blocks sleep while the low power clock is settling.
 *
 * \details It starts the timer that blocks system from sleeping for
 *          dg_configINITIAL_SLEEP_DELAY_TIME. This is needed when the XTAL32K is used to make sure
 *          that the clock has settled properly before going back to sleep again.
 *
 * \return void
 *
 */
void cm_lp_clk_timer_start(void);

/**
 * \brief Check if the Low Power clock is available.
 * 
 * \return true if the LP clock is available, else false.
 *
 * \warning It does not block. It cannot be called from Interrupt Context.
 *
 */
bool cm_lp_clk_is_avail(void);

/**
 * \brief Check if the Low Power clock is available, interrupt safe version.
 *
 * \return true if the LP clock is available, else false.
 *
 * \warning It does not block. It can be called from Interrupt Context.
 *
 */
bool cm_lp_clk_is_avail_fromISR(void);

/**
 * \brief Wait until the Low Power clock is available.
 * 
 * \return void
 *
 * \warning It may block. It cannot be called from Interrupt Context.
 *
 */
void cm_wait_lp_clk_ready(void);

/**
 * \brief   Clear the flag that indicates that the Low Power clock is available.
 *
 * \details It is called when the system wakes up from a "forced" deep sleep state and the XTAL32K
 *          is used as the LP clock so that the system won't enter into sleep until the crystal has
 *          settled.
 *
 * \return void
 *
 * \warning It may block. It cannot be called from Interrupt Context.
 *
 */
void cm_lp_clk_wakeup(void);

/**
 * \brief Interrupt handler of the XTAL16RDY_IRQn.
 * 
 * \return void
 *
 */
void XTAL16RDY_Handler(void);

/* ---------------------------------------------------------------------------------------------- */

/*
 * Functions intended to be used only by the Clock and Power Manager or in hooks.
 */

/**
 * \brief Lower all clocks to the lowest frequency possible (best effort).
 * 
 * \details 1. Check which is the lowest system clock that can be used.
 *             The RC16 is the lowest but it does not make sense to use it if the system clock is  
 *             the XTAL or the PLL. Thus, the lowest system clock setting will always be the XTAL if  
 *             the current system clock is not the RC16.
 *  
 *             If the PLL is on then the switch to the XTAL will be done without disabling the PLL. 
 * 
 *             Affects: OTPC, KBSCN*, SPI*, I2C*, ADC*, QSPI and Timer0/2* (* = conditionally) and 
 *             SysTick. Only the Timer0/2 is inquired. If any timer is active and uses the system   
 *             clock then the system clock is left unchanged. 
 *             No block is informed about the change. If there is an active SPI or I2C transaction, 
 *             it may fail.
 *             
 *          2. Check which is the lowest AHB clock that can be used.
 *             When a MAC is active, the lowest AHB clock is 16MHz.
 *             The frequency change will destroy any ongoing IR transaction.
 *             
 *          Note: if the SysTick runs then it is the dg_configABORT_IF_SYSTICK_CLK_ERR setting that 
 *          controls whether the switch will continue or it will be aborted.
 *          
 *          3. The APB clock is always set to the lowest setting.
 * 
 * \return void
 *
 * \warning It must be called with all interrupts disabled. Cannot be called by Application tasks!
 * 
 */
void cm_lower_all_clocks(void);

/**
 * \brief Restore all clocks to the speed set by the user.
 * 
 * \return void
 *
 * \warning It must be called with all interrupts disabled. Cannot be called by Application tasks!
 * 
 */
void cm_restore_all_clocks(void);

/**
 * \brief Wait until the XTAL16M is ready and then switch clocks if necessary.
 *
 * \return void
 *
 * \warning It must be called from Interrupt Context.
 *
 */
void cm_wait_xtal16m_ready_fromISR(void);

/**
 * \brief Check if the XTAL16M is ready.
 *
 * \return True if the XTAL16M has settled, else false.
 *
 */
__RETAINED_CODE bool cm_poll_xtal16m_ready(void);

/**
 * \brief Check if the XTAL16M has started and is settling.
 *
 * \warning A WDOG reset will be issued in case the XTAL has not started properly. The NMI Handler
 *        may be called, depending on the NMI_RST bit setting.
 *
 */
void cm_check_xtal_startup(void);

/**
 * \brief Basic initialization of the system and low power clocks.
 *
 * \details It switches to RC16M, restarts XTAL16M and waits for it to settle. It also sets the
 *          DIVN. Then it sets up the low power clock.
 *
 * \return void
 *
 * \warning It must be called once, before the OS scheduler is started.
 *
 */
void cm_clk_init_low_level(void);

/* ---------------------------------------------------------------------------------------------- */

/*
 * Functions intended to be used only by the Clock and Power Manager.
 */

/**
 * \brief Set the system clock (unprotected).
 * 
 * \details It attempts to set the sys_clk to:
 *              - 16MHz : called when the system is entering power-down mode. The system clock 
 *                        settings of the application are kept in order to be able to restore them.
 *                        When the system clock is RC16, it is not changed. When it is XTAL or PLL,
 *                        it is changed to RC16.
 *                        (It is called with the scheduler stopped and all interrupts disabled in 
 *                        this case.)
 *              - Previous setting : called when the XTAL16M settles.
 *                        (It is called from ISR context with all interrupts disabled in this case.) 
 *
 * \param[in] entering_sleep True if the system is going to sleep, else false.
 *
 * \return void
 *
 * \warning It must be called from Interrupt Context and/or with all interrupts disabled.
 *          The function is internal to the CPM and should not be used externally!
 * 
 */
void cm_sys_clk_sleep(bool entering_sleep);

/**
 * \brief Clear the Event Groups Bit(s) and the "settled" flag.
 * 
 * \details It pends the clearing of the Event Groups bit(s) to the OS task daemon. In the case of
 *          waking up from the Tick timer, no other task is ready-to-run anyway. In the case
 *          though of waking up from an external interrupt to WKUPCT then another task of equal 
 *          priority to the OS daemon task may also become ready-to-run. But even in this case, the 
 *          first task that is made ready-to-run is the OS daemon task and this is the task that the
 *          scheduler will execute first. 
 * 
 * \return void
 *
 * \warning It must be called from Interrupt Context and with all interrupts disabled.
 *          The priority of the Timers task (OS daemon task) must be the highest!
 *          The function is internal to the CPM and should not be used externally!
 * 
 */
void cm_sys_clk_wakeup(void);

/**
 * \brief Halt until the XTAL16M has settled..
 *
 * \details It executes a WFI() call waiting for the XTAL16RDY_IRQ. Any other interrupts that hit
 *          (the WKUP_GPIO_IRQn should be the only one that can hit at that period), are served.
 *
 */
__RETAINED_CODE void cm_halt_until_xtal16m_ready(void);

/**
 * \brief The XTAL16M has never stopped! Inform the Adapters upon "wake-up".
 *
 * \details The debugger did not detach and the chip did not enter into sleep. Thus, the XTAL16
 *        is still running. Use this to inform Adapters that the XTAL16M is available.
 *
 * \warning It is provided only for debugging purposes since when the debugger is attached, the
 *        XTAL16M does not stop and no XTAL16RDU_IRQn hits! It will never be called in normal
 *        operation.
 *
 */
void cm_sys_xtal16m_running(void);


extern bool cm_rcx_calibration_is_on;
extern bool cm_rcx_calibration_done;
extern bool ref_calibration_failed;
extern uint16_t rcx_clock_hz;
extern uint32_t rcx_clock_period;                              // usec multiplied by 1024 * 1024
extern uint32_t ble_slot_duration_in_rcx;                      // multiplied by 1000000
extern uint8_t rcx_tick_period;                                // # of cycles in 1 tick
extern uint16_t rcx_tick_rate_hz;

/*
 * RCX frequency table
 *
 *                       min Hz     max Hz     max error
 *                       450        550        0.09%
 *
 *
 * freq        ticks     Hz                 Hz * ticks / freq (== 1sec)     Error
 * ==============================================================================
 * 9600        20        480                1                            ** 0.00%
 * 9601        20        480                0.999895844                     0.01%
 * 9602        20        480                0.99979171                      0.02%
 * 9603        20        480                0.999687598                     0.03%
 * 9604        20        480                0.999583507                     0.04%
 * 9605        20        480                0.999479438                     0.05%
 * 9606        20        480                0.99937539                      0.06%
 * 9607        19        506                1.000728635                     0.07%
 * 9608        19        506                1.00062448                      0.06%
 * 9609        19        506                1.000520346                     0.05%
 * 9610        19        506                1.000416233                     0.04%
 * 9611        19        506                1.000312142                     0.03%
 * 9612        19        506                1.000208073                     0.02%
 * 9613        19        506                1.000104026                     0.01%
 * 9614        19        506                1                            ** 0.00%
 * 9615        19        506                0.999895996                     0.01%
 * 9616        21        458                1.000207987                     0.02%
 * 9617        21        458                1.000103983                     0.01%
 * 9618        21        458                1                            ** 0.00%
 * 9619        20        481                1.000103961                     0.01%
 * 9620        20        481                1                            ** 0.00%
 * 9621        20        481                0.999896061                     0.01%
 * 9622        20        481                0.999792143                     0.02%
 * 9623        20        481                0.999688247                     0.03%
 * 9624        20        481                0.999584372                     0.04%
 * 9625        20        481                0.999480519                     0.05%
 * 9626        20        481                0.999376688                     0.06%
 * 9627        19        507                1.000623247                     0.06%
 * 9628        19        507                1.000519319                     0.05%
 * 9629        19        507                1.000415412                     0.04%
 * 9630        19        507                1.000311526                     0.03%
 * 9631        19        507                1.000207663                     0.02%
 * 9632        19        507                1.000103821                     0.01%
 * 9633        19        507                1                            ** 0.00%
 * 9634        19        507                0.999896201                     0.01%
 * 9635        19        507                0.999792423                     0.02%
 * 9636        21        459                1.000311333                     0.03%
 * 9637        21        459                1.000207533                     0.02%
 * 9638        21        459                1.000103756                     0.01%
 * 9639        21        459                1                            ** 0.00%
 * 9640        20        482                1                            ** 0.00%
 * 9641        20        482                0.999896276                     0.01%
 * 9642        20        482                0.999792574                     0.02%
 * 9643        20        482                0.999688893                     0.03%
 * 9644        20        482                0.999585234                     0.04%
 * 9645        20        482                0.999481597                     0.05%
 * 9646        19        508                1.000622019                     0.06%
 * 9647        19        508                1.000518296                     0.05%
 * 9648        19        508                1.000414594                     0.04%
 * 9649        19        508                1.000310913                     0.03%
 * 9650        19        508                1.000207254                     0.02%
 * 9651        19        508                1.000103616                     0.01%
 * 9652        19        508                1                            ** 0.00%
 * 9653        19        508                0.999896405                     0.01%
 * 9654        19        508                0.999792832                     0.02%
 * 9655        19        508                0.99968928                      0.03%
 * 9656        20        483                1.00041425                      0.04%
 * 9657        20        483                1.000310655                     0.03%
 * 9658        20        483                1.000207082                     0.02%
 * 9659        20        483                1.00010353                      0.01%
 * 9660        20        483                1                            ** 0.00%
 * 9661        20        483                0.999896491                     0.01%
 * 9662        20        483                0.999793004                     0.02%
 * 9663        20        483                0.999689537                     0.03%
 * 9664        20        483                0.999586093                     0.04%
 * 9665        20        483                0.999482669                     0.05%
 * 9666        19        509                1.000517277                     0.05%
 * 9667        19        509                1.000413779                     0.04%
 * 9668        19        509                1.000310302                     0.03%
 * 9669        19        509                1.000206847                     0.02%
 * 9670        19        509                1.000103413                     0.01%
 * 9671        19        509                1                            ** 0.00%
 * 9672        19        509                0.999896609                     0.01%
 * 9673        19        509                0.999793239                     0.02%
 * 9674        19        509                0.99968989                      0.03%
 * 9675        19        509                0.999586563                     0.04%
 * 9676        20        484                1.000413394                     0.04%
 * 9677        20        484                1.000310013                     0.03%
 * 9678        20        484                1.000206654                     0.02%
 * 9679        20        484                1.000103316                     0.01%
 * 9680        20        484                1                            ** 0.00%
 * 9681        21        461                1                            ** 0.00%
 * 9682        21        461                0.999896716                     0.01%
 * 9683        21        461                0.999793452                     0.02%
 * 9684        21        461                0.999690211                     0.03%
 * 9685        21        461                0.99958699                      0.04%
 * 9686        19        510                1.000412967                     0.04%
 * 9687        19        510                1.000309693                     0.03%
 * 9688        19        510                1.000206441                     0.02%
 * 9689        19        510                1.00010321                      0.01%
 * 9690        19        510                1                            ** 0.00%
 * 9691        19        510                0.999896811                     0.01%
 * 9692        19        510                0.999793644                     0.02%
 * 9693        19        510                0.999690498                     0.03%
 * 9694        19        510                0.999587374                     0.04%
 * 9695        20        485                1.00051573                      0.05%
 * 9696        20        485                1.000412541                     0.04%
 * 9697        20        485                1.000309374                     0.03%
 * 9698        20        485                1.000206228                     0.02%
 * 9699        20        485                1.000103103                     0.01%
 * 9700        20        485                1                            ** 0.00%
 * 9701        21        462                1.000103082                     0.01%
 * 9702        21        462                1                            ** 0.00%
 * 9703        21        462                0.999896939                     0.01%
 * 9704        21        462                0.999793899                     0.02%
 * 9705        21        462                0.999690881                     0.03%
 * 9706        19        511                1.000309087                     0.03%
 * 9707        19        511                1.000206037                     0.02%
 * 9708        19        511                1.000103008                     0.01%
 * 9709        19        511                1                            ** 0.00%
 * 9710        19        511                0.999897013                     0.01%
 * 9711        19        511                0.999794048                     0.02%
 * 9712        19        511                0.999691104                     0.03%
 * 9713        19        511                0.999588181                     0.04%
 * 9714        19        511                0.999485279                     0.05%
 * 9715        20        486                1.000514668                     0.05%
 * 9716        20        486                1.000411692                     0.04%
 * 9717        20        486                1.000308737                     0.03%
 * 9718        20        486                1.000205804                     0.02%
 * 9719        20        486                1.000102891                     0.01%
 * 9720        20        486                1                            ** 0.00%
 * 9721        20        486                0.99989713                      0.01%
 * 9722        21        463                1.000102859                     0.01%
 * 9723        21        463                1                            ** 0.00%
 * 9724        21        463                0.999897162                     0.01%
 * 9725        21        463                0.999794344                     0.02%
 * 9726        19        512                1.000205634                     0.02%
 * 9727        19        512                1.000102807                     0.01%
 * 9728        19        512                1                            ** 0.00%
 * 9729        19        512                0.999897215                     0.01%
 * 9730        19        512                0.99979445                      0.02%
 * 9731        19        512                0.999691707                     0.03%
 * 9732        19        512                0.999588985                     0.04%
 * 9733        19        512                0.999486284                     0.05%
 * 9734        20        487                1.000616396                     0.06%
 * 9735        20        487                1.000513611                     0.05%
 * 9736        20        487                1.000410846                     0.04%
 * 9737        20        487                1.000308103                     0.03%
 * 9738        20        487                1.000205381                     0.02%
 * 9739        20        487                1.00010268                      0.01%
 * 9740        20        487                1                            ** 0.00%
 * 9741        20        487                0.999897341                     0.01%
 * 9742        21        464                1.000205297                     0.02%
 * 9743        21        464                1.000102638                     0.01%
 * 9744        21        464                1                            ** 0.00%
 * 9745        21        464                0.999897383                     0.01%
 * 9746        19        513                1.000102606                     0.01%
 * 9747        19        513                1                            ** 0.00%
 * 9748        19        513                0.999897415                     0.01%
 * 9749        19        513                0.999794851                     0.02%
 * 9750        19        513                0.999692308                     0.03%
 * 9751        19        513                0.999589786                     0.04%
 * 9752        19        513                0.999487285                     0.05%
 * 9753        19        513                0.999384805                     0.06%
 * 9754        20        488                1.000615132                     0.06%
 * 9755        20        488                1.000512558                     0.05%
 * 9756        20        488                1.000410004                     0.04%
 * 9757        20        488                1.000307472                     0.03%
 * 9758        20        488                1.00020496                      0.02%
 * 9759        20        488                1.00010247                      0.01%
 * 9760        20        488                1                            ** 0.00%
 * 9761        20        488                0.999897551                     0.01%
 * 9762        20        488                0.999795124                     0.02%
 * 9763        21        465                1.000204855                     0.02%
 * 9764        21        465                1.000102417                     0.01%
 * 9765        21        465                1                            ** 0.00%
 * 9766        19        514                1                            ** 0.00%
 * 9767        19        514                0.999897614                     0.01%
 * 9768        19        514                0.99979525                      0.02%
 * 9769        19        514                0.999692906                     0.03%
 * 9770        19        514                0.999590583                     0.04%
 * 9771        19        514                0.999488282                     0.05%
 * 9772        19        514                0.999386001                     0.06%
 * 9773        20        489                1.000716259                     0.07%
 * 9774        20        489                1.000613874                     0.06%
 * 9775        20        489                1.000511509                     0.05%
 * 9776        20        489                1.000409165                     0.04%
 * 9777        20        489                1.000306843                     0.03%
 * 9778        20        489                1.000204541                     0.02%
 * 9779        20        489                1.00010226                      0.01%
 * 9780        20        489                1                            ** 0.00%
 * 9781        20        489                0.999897761                     0.01%
 * 9782        20        489                0.999795543                     0.02%
 * 9783        19        515                1.000204436                     0.02%
 * 9784        19        515                1.000102208                     0.01%
 * 9785        19        515                1                            ** 0.00%
 * 9786        21        466                1                            ** 0.00%
 * 9787        21        466                0.999897824                     0.01%
 * 9788        21        466                0.999795668                     0.02%
 * 9789        21        466                0.999693534                     0.03%
 * 9790        21        466                0.99959142                      0.04%
 * 9791        21        466                0.999489327                     0.05%
 * 9792        21        466                0.999387255                     0.06%
 * 9793        20        490                1.000714796                     0.07%
 * 9794        20        490                1.00061262                      0.06%
 * 9795        20        490                1.000510465                     0.05%
 * 9796        20        490                1.00040833                      0.04%
 * 9797        20        490                1.000306216                     0.03%
 * 9798        20        490                1.000204123                     0.02%
 * 9799        20        490                1.000102051                     0.01%
 * 9800        20        490                1                            ** 0.00%
 * 9801        20        490                0.99989797                      0.01%
 * 9802        19        516                1.00020404                      0.02%
 * 9803        19        516                1.00010201                      0.01%
 * 9804        19        516                1                            ** 0.00%
 * 9805        19        516                0.999898011                     0.01%
 * 9806        21        467                1.000101978                     0.01%
 * 9807        21        467                1                            ** 0.00%
 * 9808        21        467                0.999898042                     0.01%
 * 9809        21        467                0.999796106                     0.02%
 * 9810        21        467                0.99969419                      0.03%
 * 9811        21        467                0.999592294                     0.04%
 * 9812        21        467                0.99949042                      0.05%
 * 9813        21        467                0.999388566                     0.06%
 * 9814        20        491                1.000611372                     0.06%
 * 9815        20        491                1.000509424                     0.05%
 * 9816        20        491                1.000407498                     0.04%
 * 9817        20        491                1.000305592                     0.03%
 * 9818        20        491                1.000203707                     0.02%
 * 9819        20        491                1.000101843                     0.01%
 * 9820        20        491                1                            ** 0.00%
 * 9821        20        491                0.999898177                     0.01%
 * 9822        19        517                1.000101812                     0.01%
 * 9823        19        517                1                            ** 0.00%
 * 9824        19        517                0.999898208                     0.01%
 * 9825        19        517                0.999796438                     0.02%
 * 9826        21        468                1.000203542                     0.02%
 * 9827        21        468                1.00010176                      0.01%
 * 9828        21        468                1                            ** 0.00%
 * 9829        21        468                0.99989826                      0.01%
 * 9830        21        468                0.999796541                     0.02%
 * 9831        21        468                0.999694843                     0.03%
 * 9832        21        468                0.999593165                     0.04%
 * 9833        21        468                0.999491508                     0.05%
 * 9834        20        492                1.000610128                     0.06%
 * 9835        20        492                1.000508388                     0.05%
 * 9836        20        492                1.000406669                     0.04%
 * 9837        20        492                1.000304971                     0.03%
 * 9838        20        492                1.000203293                     0.02%
 * 9839        20        492                1.000101636                     0.01%
 * 9840        20        492                1                            ** 0.00%
 * 9841        19        518                1.000101616                     0.01%
 * 9842        19        518                1                            ** 0.00%
 * 9843        19        518                0.999898405                     0.01%
 * 9844        19        518                0.999796831                     0.02%
 * 9845        19        518                0.999695277                     0.03%
 * 9846        21        469                1.000304692                     0.03%
 * 9847        21        469                1.000203108                     0.02%
 * 9848        21        469                1.000101543                     0.01%
 * 9849        21        469                1                            ** 0.00%
 * 9850        21        469                0.999898477                     0.01%
 * 9851        21        469                0.999796975                     0.02%
 * 9852        21        469                0.999695493                     0.03%
 * 9853        21        469                0.999594032                     0.04%
 * 9854        21        469                0.999492592                     0.05%
 * 9855        20        493                1.000507357                     0.05%
 * 9856        20        493                1.000405844                     0.04%
 * 9857        20        493                1.000304352                     0.03%
 * 9858        20        493                1.000202881                     0.02%
 * 9859        20        493                1.00010143                      0.01%
 * 9860        20        493                1                            ** 0.00%
 * 9861        19        519                1                            ** 0.00%
 * 9862        19        519                0.999898601                     0.01%
 * 9863        19        519                0.999797222                     0.02%
 * 9864        19        519                0.999695864                     0.03%
 * 9865        19        519                0.999594526                     0.04%
 * 9866        21        470                1.000405433                     0.04%
 * 9867        21        470                1.000304044                     0.03%
 * 9868        21        470                1.000202675                     0.02%
 * 9869        21        470                1.000101327                     0.01%
 * 9870        21        470                1                            ** 0.00%
 * 9871        21        470                0.999898693                     0.01%
 * 9872        21        470                0.999797407                     0.02%
 * 9873        21        470                0.999696141                     0.03%
 * 9874        21        470                0.999594896                     0.04%
 * 9875        19        520                1.000506329                     0.05%
 * 9876        19        520                1.000405022                     0.04%
 * 9877        19        520                1.000303736                     0.03%
 * 9878        19        520                1.00020247                      0.02%
 * 9879        19        520                1.000101225                     0.01%
 * 9880        19        520                1                            ** 0.00%
 * 9881        19        520                0.999898796                     0.01%
 * 9882        19        520                0.999797612                     0.02%
 * 9883        19        520                0.999696448                     0.03%
 * 9884        19        520                0.999595306                     0.04%
 * 9885        19        520                0.999494183                     0.05%
 * 9886        21        471                1.000505766                     0.05%
 * 9887        21        471                1.000404572                     0.04%
 * 9888        21        471                1.000303398                     0.03%
 * 9889        21        471                1.000202245                     0.02%
 * 9890        21        471                1.000101112                     0.01%
 * 9891        21        471                1                            ** 0.00%
 * 9892        21        471                0.999898908                     0.01%
 * 9893        21        471                0.999797837                     0.02%
 * 9894        21        471                0.999696786                     0.03%
 * 9895        19        521                1.000404245                     0.04%
 * 9896        19        521                1.000303153                     0.03%
 * 9897        19        521                1.000202081                     0.02%
 * 9898        19        521                1.000101031                     0.01%
 * 9899        19        521                1                            ** 0.00%
 * 9900        20        495                1                            ** 0.00%
 * 9901        20        495                0.999899                        0.01%
 * 9902        20        495                0.999798021                     0.02%
 * 9903        20        495                0.999697061                     0.03%
 * 9904        20        495                0.999596123                     0.04%
 * 9905        20        495                0.999495204                     0.05%
 * 9906        21        472                1.000605694                     0.06%
 * 9907        21        472                1.000504694                     0.05%
 * 9908        21        472                1.000403714                     0.04%
 * 9909        21        472                1.000302755                     0.03%
 * 9910        21        472                1.000201816                     0.02%
 * 9911        21        472                1.000100898                     0.01%
 * 9912        21        472                1                            ** 0.00%
 * 9913        21        472                0.999899122                     0.01%
 * 9914        21        472                0.999798265                     0.02%
 * 9915        19        522                1.000302572                     0.03%
 * 9916        19        522                1.000201694                     0.02%
 * 9917        19        522                1.000100837                     0.01%
 * 9918        19        522                1                            ** 0.00%
 * 9919        20        496                1.000100817                     0.01%
 * 9920        20        496                1                            ** 0.00%
 * 9921        22        451                1.000100796                     0.01%
 * 9922        22        451                1                            ** 0.00%
 * 9923        22        451                0.999899224                     0.01%
 * 9924        22        451                0.999798468                     0.02%
 * 9925        22        451                0.999697733                     0.03%
 * 9926        22        451                0.999597018                     0.04%
 * 9927        22        451                0.999496323                     0.05%
 * 9928        21        473                1.000503626                     0.05%
 * 9929        21        473                1.00040286                      0.04%
 * 9930        21        473                1.000302115                     0.03%
 * 9931        21        473                1.00020139                      0.02%
 * 9932        21        473                1.000100685                     0.01%
 * 9933        21        473                1                            ** 0.00%
 * 9934        21        473                0.999899336                     0.01%
 * 9935        19        523                1.000201309                     0.02%
 * 9936        19        523                1.000100644                     0.01%
 * 9937        19        523                1                            ** 0.00%
 * 9938        19        523                0.999899376                     0.01%
 * 9939        20        497                1.000100614                     0.01%
 * 9940        20        497                1                            ** 0.00%
 * 9941        20        497                0.999899406                     0.01%
 * 9942        22        452                1.000201167                     0.02%
 * 9943        22        452                1.000100573                     0.01%
 * 9944        22        452                1                            ** 0.00%
 * 9945        22        452                0.999899447                     0.01%
 * 9946        22        452                0.999798914                     0.02%
 * 9947        22        452                0.999698402                     0.03%
 * 9948        22        452                0.999597909                     0.04%
 * 9949        21        474                1.000502563                     0.05%
 * 9950        21        474                1.00040201                      0.04%
 * 9951        21        474                1.000301477                     0.03%
 * 9952        21        474                1.000200965                     0.02%
 * 9953        21        474                1.000100472                     0.01%
 * 9954        21        474                1                            ** 0.00%
 * 9955        19        524                1.000100452                     0.01%
 * 9956        19        524                1                            ** 0.00%
 * 9957        19        524                0.999899568                     0.01%
 * 9958        20        498                1.000200844                     0.02%
 * 9959        20        498                1.000100412                     0.01%
 * 9960        20        498                1                            ** 0.00%
 * 9961        20        498                0.999899608                     0.01%
 * 9962        20        498                0.999799237                     0.02%
 * 9963        22        453                1.000301114                     0.03%
 * 9964        22        453                1.000200723                     0.02%
 * 9965        22        453                1.000100351                     0.01%
 * 9966        22        453                1                            ** 0.00%
 * 9967        22        453                0.999899669                     0.01%
 * 9968        22        453                0.999799358                     0.02%
 * 9969        22        453                0.999699067                     0.03%
 * 9970        22        453                0.999598796                     0.04%
 * 9971        19        525                1.000401163                     0.04%
 * 9972        19        525                1.000300842                     0.03%
 * 9973        19        525                1.000200541                     0.02%
 * 9974        19        525                1.000100261                     0.01%
 * 9975        19        525                1                            ** 0.00%
 * 9976        19        525                0.999899759                     0.01%
 * 9977        19        525                0.999799539                     0.02%
 * 9978        20        499                1.000200441                     0.02%
 * 9979        20        499                1.00010021                      0.01%
 * 9980        20        499                1                            ** 0.00%
 * 9981        20        499                0.99989981                      0.01%
 * 9982        20        499                0.999799639                     0.02%
 * 9983        20        499                0.999699489                     0.03%
 * 9984        22        454                1.000400641                     0.04%
 * 9985        22        454                1.000300451                     0.03%
 * 9986        22        454                1.00020028                      0.02%
 * 9987        22        454                1.00010013                      0.01%
 * 9988        22        454                1                            ** 0.00%
 * 9989        22        454                0.99989989                      0.01%
 * 9990        22        454                0.9997998                       0.02%
 * 9991        19        526                1.00030027                      0.03%
 * 9992        19        526                1.00020016                      0.02%
 * 9993        19        526                1.00010007                      0.01%
 * 9994        19        526                1                            ** 0.00%
 * 9995        21        476                1.00010005                      0.01%
 * 9996        21        476                1                            ** 0.00%
 * 9997        21        476                0.99989997                      0.01%
 * 9998        20        500                1.00020004                      0.02%
 * 9999        20        500                1.00010001                      0.01%
 * 10000       20        500                1                            ** 0.00%
 * 10001       20        500                0.99990001                      0.01%
 * 10002       20        500                0.99980004                      0.02%
 * 10003       20        500                0.99970009                      0.03%
 * 10004       20        500                0.99960016                      0.04%
 * 10005       22        455                1.00049975                      0.05%
 * 10006       22        455                1.00039976                      0.04%
 * 10007       22        455                1.00029979                      0.03%
 * 10008       22        455                1.00019984                      0.02%
 * 10009       22        455                1.00009991                      0.01%
 * 10010       22        455                1                            ** 0.00%
 * 10011       22        455                0.99990011                      0.01%
 * 10012       19        527                1.00009988                      0.01%
 * 10013       19        527                1                            ** 0.00%
 * 10014       19        527                0.99990014                      0.01%
 * 10015       21        477                1.0001997                       0.02%
 * 10016       21        477                1.00009984                      0.01%
 * 10017       21        477                1                            ** 0.00%
 * 10018       21        477                0.99990018                      0.01%
 * 10019       20        501                1.00009981                      0.01%
 * 10020       20        501                1                            ** 0.00%
 * 10021       20        501                0.99990021                      0.01%
 * 10022       20        501                0.999800439                     0.02%
 * 10023       20        501                0.999700688                     0.03%
 * 10024       20        501                0.999600958                     0.04%
 * 10025       20        501                0.999501247                     0.05%
 * 10026       19        528                1.000598444                     0.06%
 * 10027       19        528                1.000498654                     0.05%
 * 10028       19        528                1.000398883                     0.04%
 * 10029       19        528                1.000299133                     0.03%
 * 10030       19        528                1.000199402                     0.02%
 * 10031       19        528                1.000099691                     0.01%
 * 10032       19        528                1                            ** 0.00%
 * 10033       19        528                0.999900329                     0.01%
 * 10034       19        528                0.999800678                     0.02%
 * 10035       21        478                1.000298954                     0.03%
 * 10036       21        478                1.000199283                     0.02%
 * 10037       21        478                1.000099631                     0.01%
 * 10038       21        478                1                            ** 0.00%
 * 10039       20        502                1.000099612                     0.01%
 * 10040       20        502                1                            ** 0.00%
 * 10041       20        502                0.999900408                     0.01%
 * 10042       20        502                0.999800836                     0.02%
 * 10043       20        502                0.999701284                     0.03%
 * 10044       20        502                0.999601752                     0.04%
 * 10045       20        502                0.99950224                      0.05%
 * 10046       19        529                1.000497711                     0.05%
 * 10047       19        529                1.000398129                     0.04%
 * 10048       19        529                1.000298567                     0.03%
 * 10049       19        529                1.000199025                     0.02%
 * 10050       19        529                1.000099502                     0.01%
 * 10051       19        529                1                            ** 0.00%
 * 10052       19        529                0.999900517                     0.01%
 * 10053       22        457                1.000099473                     0.01%
 * 10054       22        457                1                            ** 0.00%
 * 10055       22        457                0.999900547                     0.01%
 * 10056       22        457                0.999801114                     0.02%
 * 10057       21        479                1.000198866                     0.02%
 * 10058       21        479                1.000099423                     0.01%
 * 10059       21        479                1                            ** 0.00%
 * 10060       20        503                1                            ** 0.00%
 * 10061       20        503                0.999900606                     0.01%
 * 10062       20        503                0.999801232                     0.02%
 * 10063       20        503                0.999701878                     0.03%
 * 10064       20        503                0.999602544                     0.04%
 * 10065       19        530                1.000496771                     0.05%
 * 10066       19        530                1.000397377                     0.04%
 * 10067       19        530                1.000298003                     0.03%
 * 10068       19        530                1.000198649                     0.02%
 * 10069       19        530                1.000099315                     0.01%
 * 10070       19        530                1                            ** 0.00%
 * 10071       19        530                0.999900705                     0.01%
 * 10072       19        530                0.99980143                      0.02%
 * 10073       22        458                1.000297826                     0.03%
 * 10074       22        458                1.000198531                     0.02%
 * 10075       22        458                1.000099256                     0.01%
 * 10076       22        458                1                            ** 0.00%
 * 10077       22        458                0.999900764                     0.01%
 * 10078       20        504                1.000198452                     0.02%
 * 10079       20        504                1.000099216                     0.01%
 * 10080       20        504                1                            ** 0.00%
 * 10081       20        504                0.999900803                     0.01%
 * 10082       20        504                0.999801627                     0.02%
 * 10083       20        504                0.99970247                      0.03%
 * 10084       20        504                0.999603332                     0.04%
 * 10085       19        531                1.000396629                     0.04%
 * 10086       19        531                1.000297442                     0.03%
 * 10087       19        531                1.000198275                     0.02%
 * 10088       19        531                1.000099128                     0.01%
 * 10089       19        531                1                            ** 0.00%
 * 10090       19        531                0.999900892                     0.01%
 * 10091       19        531                0.999801804                     0.02%
 * 10092       19        531                0.999702735                     0.03%
 * 10093       19        531                0.999603686                     0.04%
 * 10094       22        459                1.000396275                     0.04%
 * 10095       22        459                1.000297177                     0.03%
 * 10096       22        459                1.000198098                     0.02%
 * 10097       22        459                1.000099039                     0.01%
 * 10098       22        459                1                            ** 0.00%
 * 10099       20        505                1.00009902                      0.01%
 * 10100       20        505                1                            ** 0.00%
 * 10101       21        481                1                            ** 0.00%
 * 10102       21        481                0.99990101                      0.01%
 * 10103       21        481                0.999802039                     0.02%
 * 10104       21        481                0.999703088                     0.03%
 * 10105       19        532                1.000296883                     0.03%
 * 10106       19        532                1.000197902                     0.02%
 * 10107       19        532                1.000098941                     0.01%
 * 10108       19        532                1                            ** 0.00%
 * 10109       19        532                0.999901078                     0.01%
 * 10110       19        532                0.999802176                     0.02%
 * 10111       19        532                0.999703293                     0.03%
 * 10112       19        532                0.99960443                      0.04%
 * 10113       19        532                0.999505587                     0.05%
 * 10114       20        506                1.000593237                     0.06%
 * 10115       20        506                1.000494315                     0.05%
 * 10116       20        506                1.000395413                     0.04%
 * 10117       20        506                1.000296531                     0.03%
 * 10118       20        506                1.000197668                     0.02%
 * 10119       20        506                1.000098824                     0.01%
 * 10120       20        506                1                            ** 0.00%
 * 10121       21        482                1.000098804                     0.01%
 * 10122       21        482                1                            ** 0.00%
 * 10123       21        482                0.999901215                     0.01%
 * 10124       21        482                0.99980245                      0.02%
 * 10125       19        533                1.000197531                     0.02%
 * 10126       19        533                1.000098756                     0.01%
 * 10127       19        533                1                            ** 0.00%
 * 10128       19        533                0.999901264                     0.01%
 * 10129       19        533                0.999802547                     0.02%
 * 10130       19        533                0.99970385                      0.03%
 * 10131       19        533                0.999605172                     0.04%
 * 10132       19        533                0.999506514                     0.05%
 * 10133       19        533                0.999407875                     0.06%
 * 10134       20        507                1.000592066                     0.06%
 * 10135       20        507                1.00049334                      0.05%
 * 10136       20        507                1.000394633                     0.04%
 * 10137       20        507                1.000295946                     0.03%
 * 10138       20        507                1.000197278                     0.02%
 * 10139       20        507                1.000098629                     0.01%
 * 10140       20        507                1                            ** 0.00%
 * 10141       22        461                1.00009861                      0.01%
 * 10142       22        461                1                            ** 0.00%
 * 10143       21        483                1                            ** 0.00%
 * 10144       21        483                0.99990142                      0.01%
 * 10145       19        534                1.000098571                     0.01%
 * 10146       19        534                1                            ** 0.00%
 * 10147       19        534                0.999901449                     0.01%
 * 10148       19        534                0.999802917                     0.02%
 * 10149       19        534                0.999704404                     0.03%
 * 10150       19        534                0.999605911                     0.04%
 * 10151       19        534                0.999507438                     0.05%
 * 10152       19        534                0.999408983                     0.06%
 * 10153       20        508                1.000689451                     0.07%
 * 10154       20        508                1.0005909                       0.06%
 * 10155       20        508                1.000492368                     0.05%
 * 10156       20        508                1.000393856                     0.04%
 * 10157       20        508                1.000295363                     0.03%
 * 10158       20        508                1.000196889                     0.02%
 * 10159       20        508                1.000098435                     0.01%
 * 10160       20        508                1                            ** 0.00%
 * 10161       20        508                0.999901584                     0.01%
 * 10162       21        484                1.000196812                     0.02%
 * 10163       21        484                1.000098396                     0.01%
 * 10164       21        484                1                            ** 0.00%
 * 10165       19        535                1                            ** 0.00%
 * 10166       19        535                0.999901633                     0.01%
 * 10167       19        535                0.999803285                     0.02%
 * 10168       19        535                0.999704957                     0.03%
 * 10169       19        535                0.999606648                     0.04%
 * 10170       19        535                0.999508358                     0.05%
 * 10171       19        535                0.999410088                     0.06%
 * 10172       19        535                0.999311836                     0.07%
 * 10173       20        509                1.000688096                     0.07%
 * 10174       20        509                1.000589739                     0.06%
 * 10175       20        509                1.0004914                       0.05%
 * 10176       20        509                1.000393082                     0.04%
 * 10177       20        509                1.000294782                     0.03%
 * 10178       20        509                1.000196502                     0.02%
 * 10179       20        509                1.000098241                     0.01%
 * 10180       20        509                1                            ** 0.00%
 * 10181       20        509                0.999901778                     0.01%
 * 10182       19        536                1.000196425                     0.02%
 * 10183       19        536                1.000098203                     0.01%
 * 10184       19        536                1                            ** 0.00%
 * 10185       21        485                1                            ** 0.00%
 * 10186       22        463                1                            ** 0.00%
 * 10187       22        463                0.999901836                     0.01%
 * 10188       22        463                0.999803691                     0.02%
 * 10189       22        463                0.999705565                     0.03%
 * 10190       22        463                0.999607458                     0.04%
 * 10191       22        463                0.999509371                     0.05%
 * 10192       22        463                0.999411303                     0.06%
 * 10193       20        510                1.000686746                     0.07%
 * 10194       20        510                1.000588582                     0.06%
 * 10195       20        510                1.000490436                     0.05%
 * 10196       20        510                1.000392311                     0.04%
 * 10197       20        510                1.000294204                     0.03%
 * 10198       20        510                1.000196117                     0.02%
 * 10199       20        510                1.000098049                     0.01%
 * 10200       20        510                1                            ** 0.00%
 * 10201       20        510                0.99990197                      0.01%
 * 10202       19        537                1.00009802                      0.01%
 * 10203       19        537                1                            ** 0.00%
 * 10204       19        537                0.999901999                     0.01%
 * 10205       21        486                1.000097991                     0.01%
 * 10206       21        486                1                            ** 0.00%
 * 10207       22        464                1.000097972                     0.01%
 * 10208       22        464                1                            ** 0.00%
 * 10209       22        464                0.999902047                     0.01%
 * 10210       22        464                0.999804114                     0.02%
 * 10211       22        464                0.999706199                     0.03%
 * 10212       22        464                0.999608304                     0.04%
 * 10213       22        464                0.999510428                     0.05%
 * 10214       20        511                1.000587429                     0.06%
 * 10215       20        511                1.000489476                     0.05%
 * 10216       20        511                1.000391543                     0.04%
 * 10217       20        511                1.000293628                     0.03%
 * 10218       20        511                1.000195733                     0.02%
 * 10219       20        511                1.000097857                     0.01%
 * 10220       20        511                1                            ** 0.00%
 * 10221       19        538                1.000097838                     0.01%
 * 10222       19        538                1                            ** 0.00%
 * 10223       19        538                0.999902181                     0.01%
 * 10224       19        538                0.999804382                     0.02%
 * 10225       21        487                1.000195599                     0.02%
 * 10226       21        487                1.00009779                      0.01%
 * 10227       21        487                1                            ** 0.00%
 * 10228       21        487                0.999902229                     0.01%
 * 10229       22        465                1.000097761                     0.01%
 * 10230       22        465                1                            ** 0.00%
 * 10231       22        465                0.999902258                     0.01%
 * 10232       22        465                0.999804535                     0.02%
 * 10233       22        465                0.999706831                     0.03%
 * 10234       22        465                0.999609146                     0.04%
 * 10235       20        512                1.00048852                      0.05%
 * 10236       20        512                1.000390778                     0.04%
 * 10237       20        512                1.000293055                     0.03%
 * 10238       20        512                1.000195351                     0.02%
 * 10239       20        512                1.000097666                     0.01%
 * 10240       20        512                1                            ** 0.00%
 * 10241       19        539                1                            ** 0.00%
 * 10242       19        539                0.999902363                     0.01%
 * 10243       19        539                0.999804745                     0.02%
 * 10244       19        539                0.999707146                     0.03%
 * 10245       21        488                1.000292826                     0.03%
 * 10246       21        488                1.000195198                     0.02%
 * 10247       21        488                1.00009759                      0.01%
 * 10248       21        488                1                            ** 0.00%
 * 10249       21        488                0.99990243                      0.01%
 * 10250       22        466                1.000195122                     0.02%
 * 10251       22        466                1.000097551                     0.01%
 * 10252       22        466                1                            ** 0.00%
 * 10253       22        466                0.999902468                     0.01%
 * 10254       22        466                0.999804954                     0.02%
 * 10255       22        466                0.99970746                      0.03%
 * 10256       19        540                1.000390016                     0.04%
 * 10257       19        540                1.000292483                     0.03%
 * 10258       19        540                1.00019497                      0.02%
 * 10259       19        540                1.000097475                     0.01%
 * 10260       19        540                1                            ** 0.00%
 * 10261       19        540                0.999902544                     0.01%
 * 10262       19        540                0.999805106                     0.02%
 * 10263       19        540                0.999707688                     0.03%
 * 10264       19        540                0.999610288                     0.04%
 * 10265       21        489                1.000389674                     0.04%
 * 10266       21        489                1.000292227                     0.03%
 * 10267       21        489                1.000194799                     0.02%
 * 10268       21        489                1.00009739                      0.01%
 * 10269       21        489                1                            ** 0.00%
 * 10270       21        489                0.999902629                     0.01%
 * 10271       21        489                0.999805277                     0.02%
 * 10272       22        467                1.000194704                     0.02%
 * 10273       22        467                1.000097343                     0.01%
 * 10274       22        467                1                            ** 0.00%
 * 10275       22        467                0.999902676                     0.01%
 * 10276       22        467                0.999805372                     0.02%
 * 10277       19        541                1.000194609                     0.02%
 * 10278       19        541                1.000097295                     0.01%
 * 10279       19        541                1                            ** 0.00%
 * 10280       20        514                1                            ** 0.00%
 * 10281       20        514                0.999902733                     0.01%
 * 10282       20        514                0.999805485                     0.02%
 * 10283       20        514                0.999708256                     0.03%
 * 10284       20        514                0.999611046                     0.04%
 * 10285       21        490                1.000486145                     0.05%
 * 10286       21        490                1.000388878                     0.04%
 * 10287       21        490                1.00029163                      0.03%
 * 10288       21        490                1.000194401                     0.02%
 * 10289       21        490                1.000097191                     0.01%
 * 10290       21        490                1                            ** 0.00%
 * 10291       21        490                0.999902828                     0.01%
 * 10292       21        490                0.999805674                     0.02%
 * 10293       22        468                1.00029146                      0.03%
 * 10294       22        468                1.000194288                     0.02%
 * 10295       22        468                1.000097135                     0.01%
 * 10296       22        468                1                            ** 0.00%
 * 10297       19        542                1.000097116                     0.01%
 * 10298       19        542                1                            ** 0.00%
 * 10299       20        515                1.000097097                     0.01%
 * 10300       20        515                1                            ** 0.00%
 * 10301       20        515                0.999902922                     0.01%
 * 10302       20        515                0.999805863                     0.02%
 * 10303       20        515                0.999708823                     0.03%
 * 10304       20        515                0.999611801                     0.04%
 * 10305       20        515                0.999514799                     0.05%
 * 10306       21        491                1.000485154                     0.05%
 * 10307       21        491                1.000388086                     0.04%
 * 10308       21        491                1.000291036                     0.03%
 * 10309       21        491                1.000194005                     0.02%
 * 10310       21        491                1.000096993                     0.01%
 * 10311       21        491                1                            ** 0.00%
 * 10312       21        491                0.999903026                     0.01%
 * 10313       21        491                0.99980607                      0.02%
 * 10314       19        543                1.000290867                     0.03%
 * 10315       19        543                1.000193892                     0.02%
 * 10316       19        543                1.000096937                     0.01%
 * 10317       19        543                1                            ** 0.00%
 * 10318       22        469                1                            ** 0.00%
 * 10319       20        516                1.000096909                     0.01%
 * 10320       20        516                1                            ** 0.00%
 * 10321       20        516                0.99990311                      0.01%
 * 10322       20        516                0.999806239                     0.02%
 * 10323       20        516                0.999709387                     0.03%
 * 10324       20        516                0.999612553                     0.04%
 * 10325       20        516                0.999515738                     0.05%
 * 10326       21        492                1.000581058                     0.06%
 * 10327       21        492                1.000484168                     0.05%
 * 10328       21        492                1.000387297                     0.04%
 * 10329       21        492                1.000290444                     0.03%
 * 10330       21        492                1.000193611                     0.02%
 * 10331       21        492                1.000096796                     0.01%
 * 10332       21        492                1                            ** 0.00%
 * 10333       21        492                0.999903223                     0.01%
 * 10334       19        544                1.000193536                     0.02%
 * 10335       19        544                1.000096759                     0.01%
 * 10336       19        544                1                            ** 0.00%
 * 10337       19        544                0.99990326                      0.01%
 * 10338       20        517                1.000193461                     0.02%
 * 10339       20        517                1.000096721                     0.01%
 * 10340       20        517                1                            ** 0.00%
 * 10341       20        517                0.999903298                     0.01%
 * 10342       20        517                0.999806614                     0.02%
 * 10343       20        517                0.999709949                     0.03%
 * 10344       20        517                0.999613302                     0.04%
 * 10345       23        450                1.000483325                     0.05%
 * 10346       23        450                1.000386623                     0.04%
 * 10347       23        450                1.000289939                     0.03%
 * 10348       23        450                1.000193274                     0.02%
 * 10349       23        450                1.000096628                     0.01%
 * 10350       23        450                1                            ** 0.00%
 * 10351       23        450                0.999903391                     0.01%
 * 10352       21        493                1.0000966                       0.01%
 * 10353       21        493                1                            ** 0.00%
 * 10354       19        545                1.000096581                     0.01%
 * 10355       19        545                1                            ** 0.00%
 * 10356       19        545                0.999903438                     0.01%
 * 10357       19        545                0.999806894                     0.02%
 * 10358       20        518                1.000193087                     0.02%
 * 10359       20        518                1.000096534                     0.01%
 * 10360       20        518                1                            ** 0.00%
 * 10361       22        471                1.000096516                     0.01%
 * 10362       22        471                1                            ** 0.00%
 * 10363       22        471                0.999903503                     0.01%
 * 10364       22        471                0.999807024                     0.02%
 * 10365       22        471                0.999710564                     0.03%
 * 10366       22        471                0.999614123                     0.04%
 * 10367       22        471                0.9995177                       0.05%
 * 10368       23        451                1.000482253                     0.05%
 * 10369       23        451                1.000385765                     0.04%
 * 10370       23        451                1.000289296                     0.03%
 * 10371       23        451                1.000192845                     0.02%
 * 10372       23        451                1.000096413                     0.01%
 * 10373       23        451                1                            ** 0.00%
 * 10374       19        546                1                            ** 0.00%
 * 10375       19        546                0.999903614                     0.01%
 * 10376       19        546                0.999807247                     0.02%
 * 10377       20        519                1.000289101                     0.03%
 * 10378       20        519                1.000192715                     0.02%
 * 10379       20        519                1.000096348                     0.01%
 * 10380       20        519                1                            ** 0.00%
 * 10381       20        519                0.99990367                      0.01%
 * 10382       22        472                1.000192641                     0.02%
 * 10383       22        472                1.000096311                     0.01%
 * 10384       22        472                1                            ** 0.00%
 * 10385       22        472                0.999903707                     0.01%
 * 10386       22        472                0.999807433                     0.02%
 * 10387       22        472                0.999711177                     0.03%
 * 10388       22        472                0.99961494                      0.04%
 * 10389       19        547                1.000385023                     0.04%
 * 10390       19        547                1.000288739                     0.03%
 * 10391       19        547                1.000192474                     0.02%
 * 10392       19        547                1.000096228                     0.01%
 * 10393       19        547                1                            ** 0.00%
 * 10394       21        495                1.000096209                     0.01%
 * 10395       21        495                1                            ** 0.00%
 * 10396       23        452                1                            ** 0.00%
 * 10397       23        452                0.999903818                     0.01%
 * 10398       20        520                1.000192345                     0.02%
 * 10399       20        520                1.000096163                     0.01%
 * 10400       20        520                1                            ** 0.00%
 * 10401       20        520                0.999903855                     0.01%
 * 10402       20        520                0.999807729                     0.02%
 * 10403       22        473                1.000288378                     0.03%
 * 10404       22        473                1.000192234                     0.02%
 * 10405       22        473                1.000096108                     0.01%
 * 10406       22        473                1                            ** 0.00%
 * 10407       22        473                0.999903911                     0.01%
 * 10408       22        473                0.99980784                      0.02%
 * 10409       19        548                1.000288212                     0.03%
 * 10410       19        548                1.000192123                     0.02%
 * 10411       19        548                1.000096052                     0.01%
 * 10412       19        548                1                            ** 0.00%
 * 10413       19        548                0.999903966                     0.01%
 * 10414       21        496                1.000192049                     0.02%
 * 10415       21        496                1.000096015                     0.01%
 * 10416       21        496                1                            ** 0.00%
 * 10417       21        496                0.999904003                     0.01%
 * 10418       23        453                1.000095988                     0.01%
 * 10419       23        453                1                            ** 0.00%
 * 10420       20        521                1                            ** 0.00%
 * 10421       20        521                0.99990404                      0.01%
 * 10422       20        521                0.999808098                     0.02%
 * 10423       20        521                0.999712175                     0.03%
 * 10424       22        474                1.00038373                      0.04%
 * 10425       22        474                1.00028777                      0.03%
 * 10426       22        474                1.000191828                     0.02%
 * 10427       22        474                1.000095905                     0.01%
 * 10428       22        474                1                            ** 0.00%
 * 10429       22        474                0.999904114                     0.01%
 * 10430       19        549                1.000095877                     0.01%
 * 10431       19        549                1                            ** 0.00%
 * 10432       19        549                0.999904141                     0.01%
 * 10433       19        549                0.999808301                     0.02%
 * 10434       21        497                1.000287522                     0.03%
 * 10435       21        497                1.000191663                     0.02%
 * 10436       21        497                1.000095822                     0.01%
 * 10437       21        497                1                            ** 0.00%
 * 10438       21        497                0.999904196                     0.01%
 * 10439       20        522                1.000095795                     0.01%
 * 10440       20        522                1                            ** 0.00%
 * 10441       23        454                1.000095776                     0.01%
 * 10442       23        454                1                            ** 0.00%
 * 10443       23        454                0.999904242                     0.01%
 * 10444       23        454                0.999808502                     0.02%
 * 10445       23        454                0.999712781                     0.03%
 * 10446       19        550                1.000382922                     0.04%
 * 10447       19        550                1.000287164                     0.03%
 * 10448       19        550                1.000191424                     0.02%
 * 10449       19        550                1.000095703                     0.01%
 * 10450       19        550                1                            ** 0.00%
 * 10451       19        550                0.999904315                     0.01%
 * 10452       19        550                0.999808649                     0.02%
 * 10453       19        550                0.999713001                     0.03%
 * 10454       21        498                1.000382629                     0.04%
 * 10455       21        498                1.000286944                     0.03%
 * 10456       21        498                1.000191278                     0.02%
 * 10457       21        498                1.00009563                      0.01%
 * 10458       21        498                1                            ** 0.00%
 * 10459       20        523                1.000095611                     0.01%
 * 10460       20        523                1                            ** 0.00%
 * 10461       20        523                0.999904407                     0.01%
 * 10462       20        523                0.999808832                     0.02%
 * 10463       23        455                1.00019115                      0.02%
 * 10464       23        455                1.000095566                     0.01%
 * 10465       23        455                1                            ** 0.00%
 * 10466       23        455                0.999904453                     0.01%
 * 10467       23        455                0.999808923                     0.02%
 * 10468       23        455                0.999713412                     0.03%
 * 10469       22        476                1.00028656                      0.03%
 * 10470       22        476                1.000191022                     0.02%
 * 10471       22        476                1.000095502                     0.01%
 * 10472       22        476                1                            ** 0.00%
 * 10473       22        476                0.999904516                     0.01%
 * 10474       22        476                0.999809051                     0.02%
 * 10475       22        476                0.999713604                     0.03%
 * 10476       21        499                1.000286369                     0.03%
 * 10477       21        499                1.000190894                     0.02%
 * 10478       21        499                1.000095438                     0.01%
 * 10479       21        499                1                            ** 0.00%
 * 10480       20        524                1                            ** 0.00%
 * 10481       20        524                0.999904589                     0.01%
 * 10482       20        524                0.999809197                     0.02%
 * 10483       20        524                0.999713822                     0.03%
 * 10484       23        456                1.000381534                     0.04%
 * 10485       23        456                1.000286123                     0.03%
 * 10486       23        456                1.00019073                      0.02%
 * 10487       23        456                1.000095356                     0.01%
 * 10488       23        456                1                            ** 0.00%
 * 10489       23        456                0.999904662                     0.01%
 * 10490       23        456                0.999809342                     0.02%
 * 10491       22        477                1.000285959                     0.03%
 * 10492       22        477                1.000190621                     0.02%
 * 10493       22        477                1.000095302                     0.01%
 * 10494       22        477                1                            ** 0.00%
 * 10495       22        477                0.999904717                     0.01%
 * 10496       22        477                0.999809451                     0.02%
 * 10497       20        525                1.000285796                     0.03%
 * 10498       20        525                1.000190512                     0.02%
 * 10499       20        525                1.000095247                     0.01%
 * 10500       20        525                1                            ** 0.00%
 * 10501       20        525                0.999904771                     0.01%
 * 10502       20        525                0.99980956                      0.02%
 * 10503       20        525                0.999714367                     0.03%
 * 10504       20        525                0.999619193                     0.04%
 * 10505       20        525                0.999524036                     0.05%
 * 10506       23        457                1.000475919                     0.05%
 * 10507       23        457                1.000380699                     0.04%
 * 10508       23        457                1.000285497                     0.03%
 * 10509       23        457                1.000190313                     0.02%
 * 10510       23        457                1.000095147                     0.01%
 * 10511       23        457                1                            ** 0.00%
 * 10512       23        457                0.999904871                     0.01%
 * 10513       23        457                0.999809759                     0.02%
 * 10514       22        478                1.000190223                     0.02%
 * 10515       22        478                1.000095102                     0.01%
 * 10516       22        478                1                            ** 0.00%
 * 10517       22        478                0.999904916                     0.01%
 * 10518       20        526                1.00019015                      0.02%
 * 10519       20        526                1.000095066                     0.01%
 * 10520       20        526                1                            ** 0.00%
 * 10521       21        501                1                            ** 0.00%
 * 10522       21        501                0.999904961                     0.01%
 * 10523       21        501                0.99980994                      0.02%
 * 10524       21        501                0.999714937                     0.03%
 * 10525       21        501                0.999619952                     0.04%
 * 10526       21        501                0.999524986                     0.05%
 * 10527       21        501                0.999430037                     0.06%
 * 10528       23        458                1.000569909                     0.06%
 * 10529       23        458                1.000474879                     0.05%
 * 10530       23        458                1.000379867                     0.04%
 * 10531       23        458                1.000284873                     0.03%
 * 10532       23        458                1.000189897                     0.02%
 * 10533       23        458                1.00009494                      0.01%
 * 10534       23        458                1                            ** 0.00%
 * 10535       23        458                0.999905078                     0.01%
 * 10536       22        479                1.000189825                     0.02%
 * 10537       22        479                1.000094904                     0.01%
 * 10538       22        479                1                            ** 0.00%
 * 10539       20        527                1.000094886                     0.01%
 * 10540       20        527                1                            ** 0.00%
 * 10541       21        502                1.000094868                     0.01%
 * 10542       21        502                1                            ** 0.00%
 * 10543       21        502                0.99990515                      0.01%
 * 10544       21        502                0.999810319                     0.02%
 * 10545       21        502                0.999715505                     0.03%
 * 10546       21        502                0.999620709                     0.04%
 * 10547       21        502                0.999525932                     0.05%
 * 10548       21        502                0.999431172                     0.06%
 * 10549       21        502                0.99933643                      0.07%
 * 10550       23        459                1.000663507                     0.07%
 * 10551       23        459                1.000568666                     0.06%
 * 10552       23        459                1.000473844                     0.05%
 * 10553       23        459                1.000379039                     0.04%
 * 10554       23        459                1.000284252                     0.03%
 * 10555       23        459                1.000189484                     0.02%
 * 10556       23        459                1.000094733                     0.01%
 * 10557       23        459                1                            ** 0.00%
 * 10558       23        459                0.999905285                     0.01%
 * 10559       20        528                1.000094706                     0.01%
 * 10560       20        528                1                            ** 0.00%
 * 10561       20        528                0.999905312                     0.01%
 * 10562       21        503                1.000094679                     0.01%
 * 10563       21        503                1                            ** 0.00%
 * 10564       21        503                0.999905339                     0.01%
 * 10565       21        503                0.999810696                     0.02%
 * 10566       21        503                0.99971607                      0.03%
 * 10567       21        503                0.999621463                     0.04%
 * 10568       21        503                0.999526874                     0.05%
 * 10569       21        503                0.999432302                     0.06%
 * 10570       21        503                0.999337748                     0.07%
 * 10571       21        503                0.999243213                     0.08%
 * 10572       20        529                1.000756716                     0.08%
 * 10573       20        529                1.000662064                     0.07%
 * 10574       20        529                1.00056743                      0.06%
 * 10575       20        529                1.000472813                     0.05%
 * 10576       20        529                1.000378215                     0.04%
 * 10577       20        529                1.000283634                     0.03%
 * 10578       20        529                1.000189072                     0.02%
 * 10579       20        529                1.000094527                     0.01%
 * 10580       20        529                1                            ** 0.00%
 * 10581       22        481                1.000094509                     0.01%
 * 10582       22        481                1                            ** 0.00%
 * 10583       21        504                1.000094491                     0.01%
 * 10584       21        504                1                            ** 0.00%
 * 10585       21        504                0.999905527                     0.01%
 * 10586       21        504                0.999811071                     0.02%
 * 10587       21        504                0.999716634                     0.03%
 * 10588       21        504                0.999622214                     0.04%
 * 10589       21        504                0.999527812                     0.05%
 * 10590       21        504                0.999433428                     0.06%
 * 10591       21        504                0.999339061                     0.07%
 * 10592       20        530                1.000755287                     0.08%
 * 10593       20        530                1.000660814                     0.07%
 * 10594       20        530                1.000566358                     0.06%
 * 10595       20        530                1.000471921                     0.05%
 * 10596       20        530                1.000377501                     0.04%
 * 10597       20        530                1.000283099                     0.03%
 * 10598       20        530                1.000188715                     0.02%
 * 10599       20        530                1.000094349                     0.01%
 * 10600       20        530                1                            ** 0.00%
 * 10601       20        530                0.999905669                     0.01%
 * 10602       23        461                1.000094322                     0.01%
 * 10603       23        461                1                            ** 0.00%
 * 10604       22        482                1                            ** 0.00%
 * 10605       21        505                1                            ** 0.00%
 * 10606       21        505                0.999905714                     0.01%
 * 10607       21        505                0.999811445                     0.02%
 * 10608       21        505                0.999717195                     0.03%
 * 10609       21        505                0.999622962                     0.04%
 * 10610       21        505                0.999528746                     0.05%
 * 10611       21        505                0.999434549                     0.06%
 * 10612       21        505                0.999340369                     0.07%
 * 10613       20        531                1.000659568                     0.07%
 * 10614       20        531                1.000565291                     0.06%
 * 10615       20        531                1.000471032                     0.05%
 * 10616       20        531                1.00037679                      0.04%
 * 10617       20        531                1.000282566                     0.03%
 * 10618       20        531                1.000188359                     0.02%
 * 10619       20        531                1.000094171                     0.01%
 * 10620       20        531                1                            ** 0.00%
 * 10621       20        531                0.999905847                     0.01%
 * 10622       20        531                0.999811712                     0.02%
 * 10623       21        506                1.000282406                     0.03%
 * 10624       21        506                1.000188253                     0.02%
 * 10625       21        506                1.000094118                     0.01%
 * 10626       21        506                1                            ** 0.00%
 * 10627       21        506                0.9999059                       0.01%
 * 10628       21        506                0.999811818                     0.02%
 * 10629       21        506                0.999717753                     0.03%
 * 10630       21        506                0.999623706                     0.04%
 * 10631       21        506                0.999529677                     0.05%
 * 10632       21        506                0.999435666                     0.06%
 * 10633       20        532                1.000658328                     0.07%
 * 10634       20        532                1.000564228                     0.06%
 * 10635       20        532                1.000470146                     0.05%
 * 10636       20        532                1.000376081                     0.04%
 * 10637       20        532                1.000282034                     0.03%
 * 10638       20        532                1.000188005                     0.02%
 * 10639       20        532                1.000093994                     0.01%
 * 10640       20        532                1                            ** 0.00%
 * 10641       20        532                0.999906024                     0.01%
 * 10642       20        532                0.999812065                     0.02%
 * 10643       20        532                0.999718125                     0.03%
 * 10644       21        507                1.000281849                     0.03%
 * 10645       21        507                1.000187882                     0.02%
 * 10646       21        507                1.000093932                     0.01%
 * 10647       21        507                1                            ** 0.00%
 * 10648       22        484                1                            ** 0.00%
 * 10649       23        463                1                            ** 0.00%
 * 10650       23        463                0.999906103                     0.01%
 * 10651       23        463                0.999812224                     0.02%
 * 10652       23        463                0.999718363                     0.03%
 * 10653       23        463                0.999624519                     0.04%
 * 10654       23        463                0.999530693                     0.05%
 * 10655       20        533                1.000469263                     0.05%
 * 10656       20        533                1.000375375                     0.04%
 * 10657       20        533                1.000281505                     0.03%
 * 10658       20        533                1.000187652                     0.02%
 * 10659       20        533                1.000093817                     0.01%
 * 10660       20        533                1                            ** 0.00%
 * 10661       20        533                0.9999062                       0.01%
 * 10662       20        533                0.999812418                     0.02%
 * 10663       20        533                0.999718653                     0.03%
 * 10664       21        508                1.000375094                     0.04%
 * 10665       21        508                1.000281294                     0.03%
 * 10666       21        508                1.000187512                     0.02%
 * 10667       21        508                1.000093747                     0.01%
 * 10668       21        508                1                            ** 0.00%
 * 10669       22        485                1.000093729                     0.01%
 * 10670       22        485                1                            ** 0.00%
 * 10671       23        464                1.000093712                     0.01%
 * 10672       23        464                1                            ** 0.00%
 * 10673       23        464                0.999906306                     0.01%
 * 10674       23        464                0.999812629                     0.02%
 * 10675       23        464                0.99971897                      0.03%
 * 10676       20        534                1.000374672                     0.04%
 * 10677       20        534                1.000280978                     0.03%
 * 10678       20        534                1.000187301                     0.02%
 * 10679       20        534                1.000093642                     0.01%
 * 10680       20        534                1                            ** 0.00%
 * 10681       20        534                0.999906376                     0.01%
 * 10682       20        534                0.999812769                     0.02%
 * 10683       20        534                0.99971918                      0.03%
 * 10684       20        534                0.999625608                     0.04%
 * 10685       21        509                1.000374357                     0.04%
 * 10686       21        509                1.000280741                     0.03%
 * 10687       21        509                1.000187143                     0.02%
 * 10688       21        509                1.000093563                     0.01%
 * 10689       21        509                1                            ** 0.00%
 * 10690       21        509                0.999906455                     0.01%
 * 10691       22        486                1.000093537                     0.01%
 * 10692       22        486                1                            ** 0.00%
 * 10693       22        486                0.999906481                     0.01%
 * 10694       23        465                1.00009351                      0.01%
 * 10695       23        465                1                            ** 0.00%
 * 10696       23        465                0.999906507                     0.01%
 * 10697       23        465                0.999813032                     0.02%
 * 10698       20        535                1.000186951                     0.02%
 * 10699       20        535                1.000093467                     0.01%
 * 10700       20        535                1                            ** 0.00%
 * 10701       20        535                0.999906551                     0.01%
 * 10702       20        535                0.999813119                     0.02%
 * 10703       20        535                0.999719705                     0.03%
 * 10704       20        535                0.999626308                     0.04%
 * 10705       21        510                1.000467071                     0.05%
 * 10706       21        510                1.000373622                     0.04%
 * 10707       21        510                1.000280191                     0.03%
 * 10708       21        510                1.000186776                     0.02%
 * 10709       21        510                1.000093379                     0.01%
 * 10710       21        510                1                            ** 0.00%
 * 10711       21        510                0.999906638                     0.01%
 * 10712       22        487                1.000186706                     0.02%
 * 10713       22        487                1.000093345                     0.01%
 * 10714       22        487                1                            ** 0.00%
 * 10715       22        487                0.999906673                     0.01%
 * 10716       23        466                1.000186637                     0.02%
 * 10717       23        466                1.00009331                      0.01%
 * 10718       23        466                1                            ** 0.00%
 * 10719       20        536                1.000093292                     0.01%
 * 10720       20        536                1                            ** 0.00%
 * 10721       20        536                0.999906725                     0.01%
 * 10722       20        536                0.999813468                     0.02%
 * 10723       20        536                0.999720228                     0.03%
 * 10724       20        536                0.999627005                     0.04%
 * 10725       20        536                0.9995338                       0.05%
 * 10726       21        511                1.000466157                     0.05%
 * 10727       21        511                1.000372891                     0.04%
 * 10728       21        511                1.000279642                     0.03%
 * 10729       21        511                1.000186411                     0.02%
 * 10730       21        511                1.000093197                     0.01%
 * 10731       21        511                1                            ** 0.00%
 * 10732       21        511                0.999906821                     0.01%
 * 10733       21        511                0.999813659                     0.02%
 * 10734       22        488                1.000186324                     0.02%
 * 10735       22        488                1.000093153                     0.01%
 * 10736       22        488                1                            ** 0.00%
 * 10737       22        488                0.999906864                     0.01%
 * 10738       20        537                1.000186254                     0.02%
 * 10739       20        537                1.000093119                     0.01%
 * 10740       20        537                1                            ** 0.00%
 * 10741       23        467                1                            ** 0.00%
 * 10742       23        467                0.999906907                     0.01%
 * 10743       23        467                0.999813832                     0.02%
 * 10744       23        467                0.999720774                     0.03%
 * 10745       23        467                0.999627734                     0.04%
 * 10746       23        467                0.999534711                     0.05%
 * 10747       21        512                1.000465246                     0.05%
 * 10748       21        512                1.000372162                     0.04%
 * 10749       21        512                1.000279096                     0.03%
 * 10750       21        512                1.000186047                     0.02%
 * 10751       21        512                1.000093015                     0.01%
 * 10752       21        512                1                            ** 0.00%
 * 10753       21        512                0.999907003                     0.01%
 * 10754       21        512                0.999814023                     0.02%
 * 10755       22        489                1.00027894                      0.03%
 * 10756       22        489                1.000185943                     0.02%
 * 10757       22        489                1.000092963                     0.01%
 * 10758       22        489                1                            ** 0.00%
 * 10759       20        538                1.000092945                     0.01%
 * 10760       20        538                1                            ** 0.00%
 * 10761       20        538                0.999907072                     0.01%
 * 10762       23        468                1.000185839                     0.02%
 * 10763       23        468                1.000092911                     0.01%
 * 10764       23        468                1                            ** 0.00%
 * 10765       23        468                0.999907106                     0.01%
 * 10766       23        468                0.99981423                      0.02%
 * 10767       23        468                0.999721371                     0.03%
 * 10768       23        468                0.999628529                     0.04%
 * 10769       21        513                1.000371437                     0.04%
 * 10770       21        513                1.000278552                     0.03%
 * 10771       21        513                1.000185684                     0.02%
 * 10772       21        513                1.000092833                     0.01%
 * 10773       21        513                1                            ** 0.00%
 * 10774       21        513                0.999907184                     0.01%
 * 10775       21        513                0.999814385                     0.02%
 * 10776       21        513                0.999721604                     0.03%
 * 10777       20        539                1.000278371                     0.03%
 * 10778       20        539                1.000185563                     0.02%
 * 10779       20        539                1.000092773                     0.01%
 * 10780       20        539                1                            ** 0.00%
 * 10781       20        539                0.999907244                     0.01%
 * 10782       20        539                0.999814506                     0.02%
 * 10783       20        539                0.999721784                     0.03%
 * 10784       23        469                1.00027819                      0.03%
 * 10785       23        469                1.000185443                     0.02%
 * 10786       23        469                1.000092713                     0.01%
 * 10787       23        469                1                            ** 0.00%
 * 10788       23        469                0.999907304                     0.01%
 * 10789       23        469                0.999814626                     0.02%
 * 10790       23        469                0.999721965                     0.03%
 * 10791       21        514                1.000278009                     0.03%
 * 10792       21        514                1.000185322                     0.02%
 * 10793       21        514                1.000092653                     0.01%
 * 10794       21        514                1                            ** 0.00%
 * 10795       21        514                0.999907365                     0.01%
 * 10796       21        514                0.999814746                     0.02%
 * 10797       20        540                1.000277855                     0.03%
 * 10798       20        540                1.000185219                     0.02%
 * 10799       20        540                1.000092601                     0.01%
 * 10800       20        540                1                            ** 0.00%
 * 10801       22        491                1.000092584                     0.01%
 * 10802       22        491                1                            ** 0.00%
 * 10803       22        491                0.999907433                     0.01%
 * 10804       22        491                0.999814883                     0.02%
 * 10805       22        491                0.999722351                     0.03%
 * 10806       23        470                1.000370165                     0.04%
 * 10807       23        470                1.000277598                     0.03%
 * 10808       23        470                1.000185048                     0.02%
 * 10809       23        470                1.000092515                     0.01%
 * 10810       23        470                1                            ** 0.00%
 * 10811       23        470                0.999907502                     0.01%
 * 10812       23        470                0.99981502                      0.02%
 * 10813       21        515                1.000184963                     0.02%
 * 10814       21        515                1.000092473                     0.01%
 * 10815       21        515                1                            ** 0.00%
 * 10816       21        515                0.999907544                     0.01%
 * 10817       21        515                0.999815106                     0.02%
 * 10818       20        541                1.000184877                     0.02%
 * 10819       20        541                1.00009243                      0.01%
 * 10820       20        541                1                            ** 0.00%
 * 10821       20        541                0.999907587                     0.01%
 * 10822       22        492                1.000184809                     0.02%
 * 10823       22        492                1.000092396                     0.01%
 * 10824       22        492                1                            ** 0.00%
 * 10825       22        492                0.999907621                     0.01%
 * 10826       22        492                0.99981526                      0.02%
 * 10827       22        492                0.999722915                     0.03%
 * 10828       22        492                0.999630587                     0.04%
 * 10829       23        471                1.000369379                     0.04%
 * 10830       23        471                1.000277008                     0.03%
 * 10831       23        471                1.000184655                     0.02%
 * 10832       23        471                1.000092319                     0.01%
 * 10833       23        471                1                            ** 0.00%
 * 10834       23        471                0.999907698                     0.01%
 * 10835       21        516                1.000092293                     0.01%
 * 10836       21        516                1                            ** 0.00%
 * 10837       21        516                0.999907724                     0.01%
 * 10838       20        542                1.000184536                     0.02%
 * 10839       20        542                1.000092259                     0.01%
 * 10840       20        542                1                            ** 0.00%
 * 10841       20        542                0.999907758                     0.01%
 * 10842       20        542                0.999815532                     0.02%
 * 10843       22        493                1.000276676                     0.03%
 * 10844       22        493                1.000184434                     0.02%
 * 10845       22        493                1.000092208                     0.01%
 * 10846       22        493                1                            ** 0.00%
 * 10847       22        493                0.999907809                     0.01%
 * 10848       22        493                0.999815634                     0.02%
 * 10849       22        493                0.999723477                     0.03%
 * 10850       22        493                0.999631336                     0.04%
 * 10851       23        472                1.000460787                     0.05%
 * 10852       23        472                1.000368596                     0.04%
 * 10853       23        472                1.000276421                     0.03%
 * 10854       23        472                1.000184264                     0.02%
 * 10855       23        472                1.000092123                     0.01%
 * 10856       23        472                1                            ** 0.00%
 * 10857       21        517                1                            ** 0.00%
 * 10858       21        517                0.999907902                     0.01%
 * 10859       20        543                1.00009209                      0.01%
 * 10860       20        543                1                            ** 0.00%
 * 10861       20        543                0.999907927                     0.01%
 * 10862       20        543                0.999815872                     0.02%
 * 10863       20        543                0.999723833                     0.03%
 * 10864       22        494                1.000368189                     0.04%
 * 10865       22        494                1.000276116                     0.03%
 * 10866       22        494                1.00018406                      0.02%
 * 10867       22        494                1.000092022                     0.01%
 * 10868       22        494                1                            ** 0.00%
 * 10869       22        494                0.999907995                     0.01%
 * 10870       22        494                0.999816007                     0.02%
 * 10871       22        494                0.999724036                     0.03%
 * 10872       22        494                0.999632082                     0.04%
 * 10873       21        518                1.000459855                     0.05%
 * 10874       21        518                1.00036785                      0.04%
 * 10875       21        518                1.000275862                     0.03%
 * 10876       21        518                1.000183891                     0.02%
 * 10877       21        518                1.000091937                     0.01%
 * 10878       21        518                1                            ** 0.00%
 * 10879       23        473                1                            ** 0.00%
 * 10880       20        544                1                            ** 0.00%
 * 10881       20        544                0.999908097                     0.01%
 * 10882       20        544                0.99981621                      0.02%
 * 10883       20        544                0.999724341                     0.03%
 * 10884       20        544                0.999632488                     0.04%
 * 10885       22        495                1.000459348                     0.05%
 * 10886       22        495                1.000367444                     0.04%
 * 10887       22        495                1.000275558                     0.03%
 * 10888       22        495                1.000183688                     0.02%
 * 10889       22        495                1.000091836                     0.01%
 * 10890       22        495                1                            ** 0.00%
 * 10891       22        495                0.999908181                     0.01%
 * 10892       22        495                0.999816379                     0.02%
 * 10893       22        495                0.999724594                     0.03%
 * 10894       22        495                0.999632825                     0.04%
 * 10895       21        519                1.000367141                     0.04%
 * 10896       21        519                1.00027533                      0.03%
 * 10897       21        519                1.000183537                     0.02%
 * 10898       21        519                1.00009176                      0.01%
 * 10899       21        519                1                            ** 0.00%
 * 10900       20        545                1                            ** 0.00%
 * 10901       23        474                1.000091735                     0.01%
 * 10902       23        474                1                            ** 0.00%
 * 10903       23        474                0.999908282                     0.01%
 * 10904       23        474                0.999816581                     0.02%
 * 10905       23        474                0.999724897                     0.03%
 * 10906       23        474                0.999633229                     0.04%
 * 10907       22        496                1.000458421                     0.05%
 * 10908       22        496                1.000366703                     0.04%
 * 10909       22        496                1.000275002                     0.03%
 * 10910       22        496                1.000183318                     0.02%
 * 10911       22        496                1.000091651                     0.01%
 * 10912       22        496                1                            ** 0.00%
 * 10913       22        496                0.999908366                     0.01%
 * 10914       22        496                0.999816749                     0.02%
 * 10915       22        496                0.999725149                     0.03%
 * 10916       20        546                1.000366435                     0.04%
 * 10917       20        546                1.000274801                     0.03%
 * 10918       20        546                1.000183184                     0.02%
 * 10919       20        546                1.000091583                     0.01%
 * 10920       20        546                1                            ** 0.00%
 * 10921       20        546                0.999908433                     0.01%
 * 10922       20        546                0.999816883                     0.02%
 * 10923       23        475                1.0001831                       0.02%
 * 10924       23        475                1.000091542                     0.01%
 * 10925       23        475                1                            ** 0.00%
 * 10926       23        475                0.999908475                     0.01%
 * 10927       23        475                0.999816967                     0.02%
 * 10928       23        475                0.999725476                     0.03%
 * 10929       23        475                0.999634001                     0.04%
 * 10930       22        497                1.000365965                     0.04%
 * 10931       22        497                1.000274449                     0.03%
 * 10932       22        497                1.000182949                     0.02%
 * 10933       22        497                1.000091466                     0.01%
 * 10934       22        497                1                            ** 0.00%
 * 10935       22        497                0.999908551                     0.01%
 * 10936       22        497                0.999817118                     0.02%
 * 10937       20        547                1.000274298                     0.03%
 * 10938       20        547                1.000182849                     0.02%
 * 10939       20        547                1.000091416                     0.01%
 * 10940       20        547                1                            ** 0.00%
 * 10941       21        521                1                            ** 0.00%
 * 10942       21        521                0.999908609                     0.01%
 * 10943       21        521                0.999817235                     0.02%
 * 10944       21        521                0.999725877                     0.03%
 * 10945       23        476                1.000274098                     0.03%
 * 10946       23        476                1.000182715                     0.02%
 * 10947       23        476                1.000091349                     0.01%
 * 10948       23        476                1                            ** 0.00%
 * 10949       23        476                0.999908667                     0.01%
 * 10950       23        476                0.999817352                     0.02%
 * 10951       23        476                0.999726052                     0.03%
 * 10952       22        498                1.00036523                      0.04%
 * 10953       22        498                1.000273898                     0.03%
 * 10954       22        498                1.000182582                     0.02%
 * 10955       22        498                1.000091283                     0.01%
 * 10956       22        498                1                            ** 0.00%
 * 10957       22        498                0.999908734                     0.01%
 * 10958       20        548                1.000182515                     0.02%
 * 10959       20        548                1.000091249                     0.01%
 * 10960       20        548                1                            ** 0.00%
 * 10961       21        522                1.000091233                     0.01%
 * 10962       21        522                1                            ** 0.00%
 * 10963       21        522                0.999908784                     0.01%
 * 10964       21        522                0.999817585                     0.02%
 * 10965       21        522                0.999726402                     0.03%
 * 10966       21        522                0.999635236                     0.04%
 * 10967       23        477                1.000364731                     0.04%
 * 10968       23        477                1.000273523                     0.03%
 * 10969       23        477                1.000182332                     0.02%
 * 10970       23        477                1.000091158                     0.01%
 * 10971       23        477                1                            ** 0.00%
 * 10972       23        477                0.999908859                     0.01%
 * 10973       23        477                0.999817734                     0.02%
 * 10974       23        477                0.999726627                     0.03%
 * 10975       22        499                1.000273349                     0.03%
 * 10976       22        499                1.000182216                     0.02%
 * 10977       22        499                1.0000911                       0.01%
 * 10978       22        499                1                            ** 0.00%
 * 10979       20        549                1.000091083                     0.01%
 * 10980       20        549                1                            ** 0.00%
 * 10981       20        549                0.999908934                     0.01%
 * 10982       21        523                1.000091058                     0.01%
 * 10983       21        523                1                            ** 0.00%
 * 10984       21        523                0.999908958                     0.01%
 * 10985       21        523                0.999817934                     0.02%
 * 10986       21        523                0.999726925                     0.03%
 * 10987       21        523                0.999635933                     0.04%
 * 10988       21        523                0.999544958                     0.05%
 * 10989       23        478                1.000455                        0.05%
 * 10990       23        478                1.000363967                     0.04%
 * 10991       23        478                1.000272951                     0.03%
 * 10992       23        478                1.000181951                     0.02%
 * 10993       23        478                1.000090967                     0.01%
 * 10994       23        478                1                            ** 0.00%
 * 10995       23        478                0.99990905                      0.01%
 * 10996       23        478                0.999818116                     0.02%
 * 10997       20        550                1.000272802                     0.03%
 * 10998       20        550                1.000181851                     0.02%
 * 10999       20        550                1.000090917                     0.01%
 * 11000       20        550                1                            ** 0.00%
 * 11001       20        550                0.999909099                     0.01%
 * 11002       21        524                1.000181785                     0.02%
 * 11003       21        524                1.000090884                     0.01%
 * 11004       21        524                1                            ** 0.00%
 * 11005       21        524                0.999909132                     0.01%
 * 11006       21        524                0.999818281                     0.02%
 * 11007       21        524                0.999727446                     0.03%
 * 11008       21        524                0.999636628                     0.04%
 * 11009       21        524                0.999545826                     0.05%
 * 11010       21        524                0.999455041                     0.05%
 * 11011       23        479                1.00054491                      0.05%
 * 11012       23        479                1.00045405                      0.05%
 * 11013       23        479                1.000363207                     0.04%
 * 11014       23        479                1.000272381                     0.03%
 * 11015       23        479                1.000181571                     0.02%
 * 11016       23        479                1.000090777                     0.01%
 * 11017       23        479                1                            ** 0.00%
 * 11018       23        479                0.999909239                     0.01%
 * 11019       23        479                0.999818495                     0.02%
 * 11020       22        501                1.000181488                     0.02%
 * 11021       22        501                1.000090736                     0.01%
 * 11022       22        501                1                            ** 0.00%
 * 11023       22        501                0.999909281                     0.01%
 * 11024       21        525                1.000090711                     0.01%
 * 11025       21        525                1                            ** 0.00%
 * 11026       21        525                0.999909305                     0.01%
 * 11027       21        525                0.999818627                     0.02%
 * 11028       21        525                0.999727965                     0.03%
 * 11029       21        525                0.99963732                      0.04%
 * 11030       21        525                0.999546691                     0.05%
 * 11031       21        525                0.999456078                     0.05%
 * 11032       21        525                0.999365482                     0.06%
 * 11033       23        480                1.00063446                      0.06%
 * 11034       23        480                1.000543774                     0.05%
 * 11035       23        480                1.000453104                     0.05%
 * 11036       23        480                1.00036245                      0.04%
 * 11037       23        480                1.000271813                     0.03%
 * 11038       23        480                1.000181192                     0.02%
 * 11039       23        480                1.000090588                     0.01%
 * 11040       23        480                1                            ** 0.00%
 * 11041       23        480                0.999909428                     0.01%
 * 11042       22        502                1.000181127                     0.02%
 * 11043       22        502                1.000090555                     0.01%
 * 11044       22        502                1                            ** 0.00%
 * 11045       21        526                1.000090539                     0.01%
 * 11046       21        526                1                            ** 0.00%
 * 11047       21        526                0.999909478                     0.01%
 * 11048       21        526                0.999818972                     0.02%
 * 11049       21        526                0.999728482                     0.03%
 * 11050       21        526                0.999638009                     0.04%
 * 11051       21        526                0.999547552                     0.05%
 * 11052       21        526                0.999457112                     0.05%
 * 11053       21        526                0.999366688                     0.06%
 * 11054       21        526                0.99927628                      0.07%
 * 11055       23        481                1.000723654                     0.07%
 * 11056       23        481                1.00063314                      0.06%
 * 11057       23        481                1.000542643                     0.05%
 * 11058       23        481                1.000452161                     0.05%
 * 11059       23        481                1.000361696                     0.04%
 * 11060       23        481                1.000271248                     0.03%
 * 11061       23        481                1.000180815                     0.02%
 * 11062       23        481                1.0000904                       0.01%
 * 11063       23        481                1                            ** 0.00%
 * 11064       23        481                0.999909617                     0.01%
 * 11065       22        503                1.000090375                     0.01%
 * 11066       22        503                1                            ** 0.00%
 * 11067       21        527                1                            ** 0.00%
 * 11068       21        527                0.999909649                     0.01%
 * 11069       21        527                0.999819315                     0.02%
 * 11070       21        527                0.999728997                     0.03%
 * 11071       21        527                0.999638696                     0.04%
 * 11072       21        527                0.99954841                      0.05%
 * 11073       21        527                0.999458141                     0.05%
 * 11074       21        527                0.999367889                     0.06%
 * 11075       21        527                0.999277652                     0.07%
 * 11076       21        527                0.999187432                     0.08%
 * 11077       23        482                1.000812494                     0.08%
 * 11078       23        482                1.000722152                     0.07%
 * 11079       23        482                1.000631826                     0.06%
 * 11080       23        482                1.000541516                     0.05%
 * 11081       23        482                1.000451223                     0.05%
 * 11082       23        482                1.000360946                     0.04%
 * 11083       23        482                1.000270685                     0.03%
 * 11084       23        482                1.00018044                      0.02%
 * 11085       23        482                1.000090212                     0.01%
 * 11086       23        482                1                            ** 0.00%
 * 11087       21        528                1.000090196                     0.01%
 * 11088       21        528                1                            ** 0.00%
 * 11089       21        528                0.999909821                     0.01%
 * 11090       21        528                0.999819657                     0.02%
 * 11091       21        528                0.99972951                      0.03%
 * 11092       21        528                0.99963938                      0.04%
 * 11093       21        528                0.999549265                     0.05%
 * 11094       21        528                0.999459167                     0.05%
 * 11095       21        528                0.999369085                     0.06%
 * 11096       21        528                0.999279019                     0.07%
 * 11097       21        528                0.99918897                      0.08%
 * 11098       21        528                0.999098937                     0.09%
 * 11099       21        529                1.000900982                     0.09%
 * 11100       21        529                1.000810811                     0.08%
 * 11101       21        529                1.000720656                     0.07%
 * 11102       21        529                1.000630517                     0.06%
 * 11103       21        529                1.000540394                     0.05%
 * 11104       21        529                1.000450288                     0.05%
 * 11105       21        529                1.000360198                     0.04%
 * 11106       21        529                1.000270124                     0.03%
 * 11107       21        529                1.000180067                     0.02%
 * 11108       21        529                1.000090025                     0.01%
 * 11109       21        529                1                            ** 0.00%
 * 11110       22        505                1                            ** 0.00%
 * 11111       22        505                0.999909999                     0.01%
 * 11112       22        505                0.999820014                     0.02%
 * 11113       22        505                0.999730046                     0.03%
 * 11114       22        505                0.999640094                     0.04%
 * 11115       22        505                0.999550157                     0.04%
 * 11116       22        505                0.999460237                     0.05%
 * 11117       22        505                0.999370334                     0.06%
 * 11118       22        505                0.999280446                     0.07%
 * 11119       22        505                0.999190575                     0.08%
 * 11120       21        530                1.000899281                     0.09%
 * 11121       21        530                1.00080928                      0.08%
 * 11122       21        530                1.000719295                     0.07%
 * 11123       21        530                1.000629327                     0.06%
 * 11124       21        530                1.000539374                     0.05%
 * 11125       21        530                1.000449438                     0.04%
 * 11126       21        530                1.000359518                     0.04%
 * 11127       21        530                1.000269614                     0.03%
 * 11128       21        530                1.000179727                     0.02%
 * 11129       21        530                1.000089855                     0.01%
 * 11130       21        530                1                            ** 0.00%
 * 11131       22        506                1.000089839                     0.01%
 * 11132       22        506                1                            ** 0.00%
 * 11133       22        506                0.999910177                     0.01%
 * 11134       22        506                0.99982037                      0.02%
 * 11135       22        506                0.999730579                     0.03%
 * 11136       22        506                0.999640805                     0.04%
 * 11137       22        506                0.999551046                     0.04%
 * 11138       22        506                0.999461304                     0.05%
 * 11139       22        506                0.999371577                     0.06%
 * 11140       22        506                0.999281867                     0.07%
 * 11141       22        506                0.999192173                     0.08%
 * 11142       21        531                1.000807754                     0.08%
 * 11143       21        531                1.00071794                      0.07%
 * 11144       21        531                1.000628141                     0.06%
 * 11145       21        531                1.000538358                     0.05%
 * 11146       21        531                1.000448591                     0.04%
 * 11147       21        531                1.000358841                     0.04%
 * 11148       21        531                1.000269107                     0.03%
 * 11149       21        531                1.000179388                     0.02%
 * 11150       21        531                1.000089686                     0.01%
 * 11151       21        531                1                            ** 0.00%
 * 11152       21        531                0.99991033                      0.01%
 * 11153       22        507                1.000089662                     0.01%
 * 11154       22        507                1                            ** 0.00%
 * 11155       23        485                1                            ** 0.00%
 * 11156       23        485                0.999910362                     0.01%
 * 11157       23        485                0.99982074                      0.02%
 * 11158       23        485                0.999731135                     0.03%
 * 11159       23        485                0.999641545                     0.04%
 * 11160       23        485                0.999551971                     0.04%
 * 11161       23        485                0.999462414                     0.05%
 * 11162       23        485                0.999372872                     0.06%
 * 11163       23        485                0.999283347                     0.07%
 * 11164       21        532                1.000716589                     0.07%
 * 11165       21        532                1.000626959                     0.06%
 * 11166       21        532                1.000537346                     0.05%
 * 11167       21        532                1.000447748                     0.04%
 * 11168       21        532                1.000358166                     0.04%
 * 11169       21        532                1.000268601                     0.03%
 * 11170       21        532                1.000179051                     0.02%
 * 11171       21        532                1.000089518                     0.01%
 * 11172       21        532                1                            ** 0.00%
 * 11173       21        532                0.999910499                     0.01%
 * 11174       22        508                1.000178987                     0.02%
 * 11175       22        508                1.000089485                     0.01%
 * 11176       22        508                1                            ** 0.00%
 * 11177       23        486                1.000089469                     0.01%
 * 11178       23        486                1                            ** 0.00%
 * 11179       23        486                0.999910547                     0.01%
 * 11180       23        486                0.999821109                     0.02%
 * 11181       23        486                0.999731688                     0.03%
 * 11182       23        486                0.999642282                     0.04%
 * 11183       23        486                0.999552893                     0.04%
 * 11184       23        486                0.999463519                     0.05%
 * 11185       23        486                0.999374162                     0.06%
 * 11186       21        533                1.000625782                     0.06%
 * 11187       21        533                1.000536337                     0.05%
 * 11188       21        533                1.000446907                     0.04%
 * 11189       21        533                1.000357494                     0.04%
 * 11190       21        533                1.000268097                     0.03%
 * 11191       21        533                1.000178715                     0.02%
 * 11192       21        533                1.00008935                      0.01%
 * 11193       21        533                1                            ** 0.00%
 * 11194       21        533                0.999910666                     0.01%
 * 11195       21        533                0.999821349                     0.02%
 * 11196       22        509                1.000178635                     0.02%
 * 11197       22        509                1.00008931                      0.01%
 * 11198       22        509                1                            ** 0.00%
 * 11199       22        509                0.999910706                     0.01%
 * 11200       23        487                1.000089286                     0.01%
 * 11201       23        487                1                            ** 0.00%
 * 11202       23        487                0.99991073                      0.01%
 * 11203       23        487                0.999821476                     0.02%
 * 11204       23        487                0.999732238                     0.03%
 * 11205       23        487                0.999643017                     0.04%
 * 11206       23        487                0.99955381                      0.04%
 * 11207       23        487                0.99946462                      0.05%
 * 11208       21        534                1.000535332                     0.05%
 * 11209       21        534                1.00044607                      0.04%
 * 11210       21        534                1.000356824                     0.04%
 * 11211       21        534                1.000267594                     0.03%
 * 11212       21        534                1.00017838                      0.02%
 * 11213       21        534                1.000089182                     0.01%
 * 11214       21        534                1                            ** 0.00%
 * 11215       21        534                0.999910834                     0.01%
 * 11216       21        534                0.999821683                     0.02%
 * 11217       22        510                1.000267451                     0.03%
 * 11218       22        510                1.000178285                     0.02%
 * 11219       22        510                1.000089135                     0.01%
 * 11220       22        510                1                            ** 0.00%
 * 11221       22        510                0.999910881                     0.01%
 * 11222       23        488                1.000178221                     0.02%
 * 11223       23        488                1.000089103                     0.01%
 * 11224       23        488                1                            ** 0.00%
 * 11225       23        488                0.999910913                     0.01%
 * 11226       23        488                0.999821842                     0.02%
 * 11227       23        488                0.999732787                     0.03%
 * 11228       23        488                0.999643748                     0.04%
 * 11229       23        488                0.999554724                     0.04%
 * 11230       21        535                1.000445236                     0.04%
 * 11231       21        535                1.000356157                     0.04%
 * 11232       21        535                1.000267094                     0.03%
 * 11233       21        535                1.000178047                     0.02%
 * 11234       21        535                1.000089015                     0.01%
 * 11235       21        535                1                            ** 0.00%
 * 11236       21        535                0.999911                        0.01%
 * 11237       21        535                0.999822017                     0.02%
 * 11238       21        535                0.999733049                     0.03%
 * 11239       22        511                1.000266928                     0.03%
 * 11240       22        511                1.000177936                     0.02%
 * 11241       22        511                1.00008896                      0.01%
 * 11242       22        511                1                            ** 0.00%
 * 11243       22        511                0.999911056                     0.01%
 * 11244       22        511                0.999822127                     0.02%
 * 11245       23        489                1.000177857                     0.02%
 * 11246       23        489                1.000088921                     0.01%
 * 11247       23        489                1                            ** 0.00%
 * 11248       23        489                0.999911095                     0.01%
 * 11249       23        489                0.999822206                     0.02%
 * 11250       23        489                0.999733333                     0.03%
 * 11251       23        489                0.999644476                     0.04%
 * 11252       21        536                1.000355492                     0.04%
 * 11253       21        536                1.000266596                     0.03%
 * 11254       21        536                1.000177715                     0.02%
 * 11255       21        536                1.000088849                     0.01%
 * 11256       21        536                1                            ** 0.00%
 * 11257       21        536                0.999911166                     0.01%
 * 11258       21        536                0.999822349                     0.02%
 * 11259       21        536                0.999733546                     0.03%
 * 11260       22        512                1.00035524                      0.04%
 * 11261       22        512                1.000266406                     0.03%
 * 11262       22        512                1.000177588                     0.02%
 * 11263       22        512                1.000088786                     0.01%
 * 11264       22        512                1                            ** 0.00%
 * 11265       22        512                0.999911229                     0.01%
 * 11266       22        512                0.999822475                     0.02%
 * 11267       23        490                1.000266264                     0.03%
 * 11268       23        490                1.000177494                     0.02%
 * 11269       23        490                1.000088739                     0.01%
 * 11270       23        490                1                            ** 0.00%
 * 11271       23        490                0.999911277                     0.01%
 * 11272       23        490                0.999822569                     0.02%
 * 11273       23        490                0.999733877                     0.03%
 * 11274       21        537                1.000266099                     0.03%
 * 11275       21        537                1.000177384                     0.02%
 * 11276       21        537                1.000088684                     0.01%
 * 11277       21        537                1                            ** 0.00%
 * 11278       21        537                0.999911332                     0.01%
 * 11279       21        537                0.999822679                     0.02%
 * 11280       21        537                0.999734043                     0.03%
 * 11281       21        537                0.999645422                     0.04%
 * 11282       22        513                1.000354547                     0.04%
 * 11283       22        513                1.000265887                     0.03%
 * 11284       22        513                1.000177242                     0.02%
 * 11285       22        513                1.000088613                     0.01%
 * 11286       22        513                1                            ** 0.00%
 * 11287       22        513                0.999911402                     0.01%
 * 11288       22        513                0.999822821                     0.02%
 * 11289       22        513                0.999734255                     0.03%
 * 11290       23        491                1.000265722                     0.03%
 * 11291       23        491                1.000177132                     0.02%
 * 11292       23        491                1.000088558                     0.01%
 * 11293       23        491                1                            ** 0.00%
 * 11294       23        491                0.999911457                     0.01%
 * 11295       23        491                0.999822931                     0.02%
 * 11296       21        538                1.000177054                     0.02%
 * 11297       21        538                1.000088519                     0.01%
 * 11298       21        538                1                            ** 0.00%
 * 11299       21        538                0.999911497                     0.01%
 * 11300       21        538                0.999823009                     0.02%
 * 11301       21        538                0.999734537                     0.03%
 * 11302       21        538                0.99964608                      0.04%
 * 11303       22        514                1.00044236                      0.04%
 * 11304       22        514                1.000353857                     0.04%
 * 11305       22        514                1.000265369                     0.03%
 * 11306       22        514                1.000176897                     0.02%
 * 11307       22        514                1.000088441                     0.01%
 * 11308       22        514                1                            ** 0.00%
 * 11309       22        514                0.999911575                     0.01%
 * 11310       22        514                0.999823165                     0.02%
 * 11311       22        514                0.999734771                     0.03%
 * 11312       23        492                1.000353607                     0.04%
 * 11313       23        492                1.000265182                     0.03%
 * 11314       23        492                1.000176772                     0.02%
 * 11315       23        492                1.000088378                     0.01%
 * 11316       23        492                1                            ** 0.00%
 * 11317       23        492                0.999911637                     0.01%
 * 11318       21        539                1.000088355                     0.01%
 * 11319       21        539                1                            ** 0.00%
 * 11320       21        539                0.999911661                     0.01%
 * 11321       21        539                0.999823337                     0.02%
 * 11322       21        539                0.999735029                     0.03%
 * 11323       21        539                0.999646737                     0.04%
 * 11324       21        539                0.99955846                      0.04%
 * 11325       22        515                1.000441501                     0.04%
 * 11326       22        515                1.00035317                      0.04%
 * 11327       22        515                1.000264854                     0.03%
 * 11328       22        515                1.000176554                     0.02%
 * 11329       22        515                1.000088269                     0.01%
 * 11330       22        515                1                            ** 0.00%
 * 11331       22        515                0.999911747                     0.01%
 * 11332       22        515                0.999823509                     0.02%
 * 11333       22        515                0.999735286                     0.03%
 * 11334       22        515                0.99964708                      0.04%
 * 11335       23        493                1.000352889                     0.04%
 * 11336       23        493                1.000264644                     0.03%
 * 11337       23        493                1.000176414                     0.02%
 * 11338       23        493                1.000088199                     0.01%
 * 11339       23        493                1                            ** 0.00%
 * 11340       21        540                1                            ** 0.00%
 * 11341       21        540                0.999911824                     0.01%
 * 11342       21        540                0.999823664                     0.02%
 * 11343       21        540                0.99973552                      0.03%
 * 11344       21        540                0.999647391                     0.04%
 * 11345       21        540                0.999559277                     0.04%
 * 11346       22        516                1.000528821                     0.05%
 * 11347       22        516                1.000440645                     0.04%
 * 11348       22        516                1.000352485                     0.04%
 * 11349       22        516                1.00026434                      0.03%
 * 11350       22        516                1.000176211                     0.02%
 * 11351       22        516                1.000088098                     0.01%
 * 11352       22        516                1                            ** 0.00%
 * 11353       22        516                0.999911918                     0.01%
 * 11354       22        516                0.999823851                     0.02%
 * 11355       22        516                0.999735799                     0.03%
 * 11356       22        516                0.999647763                     0.04%
 * 11357       21        541                1.000352206                     0.04%
 * 11358       21        541                1.000264131                     0.03%
 * 11359       21        541                1.000176072                     0.02%
 * 11360       21        541                1.000088028                     0.01%
 * 11361       21        541                1                            ** 0.00%
 * 11362       23        494                1                            ** 0.00%
 * 11363       23        494                0.999911995                     0.01%
 * 11364       23        494                0.999824006                     0.02%
 * 11365       23        494                0.999736032                     0.03%
 * 11366       23        494                0.999648073                     0.04%
 * 11367       23        494                0.99956013                      0.04%
 * 11368       22        517                1.000527797                     0.05%
 * 11369       22        517                1.000439792                     0.04%
 * 11370       22        517                1.000351803                     0.04%
 * 11371       22        517                1.000263829                     0.03%
 * 11372       22        517                1.000175871                     0.02%
 * 11373       22        517                1.000087928                     0.01%
 * 11374       22        517                1                            ** 0.00%
 * 11375       22        517                0.999912088                     0.01%
 * 11376       22        517                0.999824191                     0.02%
 * 11377       22        517                0.99973631                      0.03%
 * 11378       21        542                1.000351556                     0.04%
 * 11379       21        542                1.000263644                     0.03%
 * 11380       21        542                1.000175747                     0.02%
 * 11381       21        542                1.000087866                     0.01%
 * 11382       21        542                1                            ** 0.00%
 * 11383       21        542                0.99991215                      0.01%
 * 11384       23        495                1.000087843                     0.01%
 * 11385       23        495                1                            ** 0.00%
 * 11386       23        495                0.999912173                     0.01%
 * 11387       23        495                0.999824361                     0.02%
 * 11388       23        495                0.999736565                     0.03%
 * 11389       23        495                0.999648784                     0.04%
 * 11390       23        495                0.999561018                     0.04%
 * 11391       22        518                1.000438943                     0.04%
 * 11392       22        518                1.000351124                     0.04%
 * 11393       22        518                1.00026332                      0.03%
 * 11394       22        518                1.000175531                     0.02%
 * 11395       22        518                1.000087758                     0.01%
 * 11396       22        518                1                            ** 0.00%
 * 11397       22        518                0.999912258                     0.01%
 * 11398       22        518                0.999824531                     0.02%
 * 11399       22        518                0.999736819                     0.03%
 *
 */


#endif /* SYS_CLOCK_MGR_H_ */

/**
 \}
 \}
 \}
 */
