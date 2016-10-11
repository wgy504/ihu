/**
\addtogroup BSP
\{
\addtogroup Configuration
\{
\addtogroup FreeRTOS
\{
*/

/**
****************************************************************************************
*
* @file custom_config_ram.h
*
* @brief Custom configuration file for FreeRTOS applications executing from RAM.
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

#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"


#define __HEAP_SIZE  0x0600
#define __STACK_SIZE 0x0100

#define CONFIG_RTT


#define dg_configPOWER_CONFIG                   (POWER_CONFIGURATION_2)

#define dg_configUSE_LP_CLK                     LP_CLK_32768
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_NONE
#define dg_configEXT_CRYSTAL_FREQ               EXT_CRYSTAL_IS_16M

#define dg_configIMAGE_SETUP                    DEVELOPMENT_MODE
#define dg_configEMULATE_OTP_COPY               (0)

#define dg_configUSER_CAN_USE_TIMER1            (0)

#define dg_configMEM_RETENTION_MODE             (0x1F)
#define dg_configMEM_RETENTION_MODE_PRESERVE_IMAGE       (0x1F)
#define dg_configSHUFFLING_MODE                 (0x3)

#define dg_configUSE_WDOG                       (0)

#define dg_configUSE_DCDC                       (1)

#define dg_configPOWER_FLASH                    (0)
#define dg_configFLASH_POWER_DOWN               (0)
#define dg_configFLASH_POWER_OFF                (1)

#define dg_configBATTERY_TYPE                   (BATTERY_TYPE_LIMN2O4)
#define dg_configBATTERY_CHARGE_CURRENT         2       // 30mA
#define dg_configBATTERY_PRECHARGE_CURRENT      20      // 2.1mA
#define dg_configBATTERY_CHARGE_NTC             1       // disabled

#define dg_configUSE_USB                        0
#define dg_configUSE_USB_CHARGER                0
#define dg_configALLOW_CHARGING_NOT_ENUM        1

#define dg_configUSE_ProDK                      (1)

#define dg_configUSE_SW_CURSOR                  (1)


/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    14000   /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripheral specific config
 */
#define dg_configUSE_HW_RF                      (0)
#define dg_configUSE_HW_TEMPSENS                (0)
#define dg_configRF_ADAPTER                     (0)
#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)

/* Include bsp default values */
#include "bsp_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */

/**
\}
\}
\}
*/
