Weightscale profile demo application {#wsp_weightscale}
==================================

## Overview

This application is a sample implementation of Weight Scale role as defined by Weight Scale Profile
specification located at: https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=293525

It supports the mandatory Weight Scale Service and Device Information Service and optional services
like User Data Service, Body Composition Service, Battery Service and Current Time Service.

Features:
- The application includes Debug Service. This is a custom created service which supports test
  execution (remove user, remove all, set level or attach user) - check 'Manual Testing' section
  for more details.

- For any read/write characteristics actions an UDS 'consent' procedure is required.

- WSP is supporting multiple users (CFG_MULTIPLE_CLIENTS).

- WSP can be set into 'test mode'. In this mode WSS and BCS services can be accessible without
  sending the 'consent' procedure (measurements are generated to each user connected to WSP).
  This mode can be used while testing WSS and BCS against PTS. Please see 'Manual Testing' section
  for more details (attach_user).

- There is an ability to remove the selected user or all users from the UDS database by using the
  Debug service.

- Measurements for each registered user are stored in the WSP application when it is not possible
  send it to the user (Client Characteristic Configuration is not set, user is disconnected).
  Stored measurement are retransmitted when a user is connected again. Currently platform is able
  to store 25 measurements per user but realistic value depends on the platform memory limit.

- One user ID with multiple connections is supported. One user can be authenticated (using the
  consent procedure) with more than one device.

## Installation procedure

The project is located in the \b `projects/dk_apps/ble_profiles/wsp_weightscale` folder.

To install the project follow the [General Installation and Debugging Procedure](@ref install_and_debug_procedure).

## Suggested Configurable parameters
- The maximum number of supported users can be modified by the CFG_UDS_MAX_USERS macro
  (config/wsp_weightscale_config.h). CFG_UDS_MAX_USERS can have the maximum value of 4.

- Multiple users support can be enabled by seting the CFG_MULTIPLE_CLIENTS macro to 1.

- Maximum stored measurements threshold can be set by using the CFG_MAX_MEAS_TO_STORE macro.

- User select port and pin configuration:
  Define the proper port value using CFG_HW_GPIO_PORT_SELECT_USER in wsp_weightscale_config.h file.
  Define the proper pin value, by which a specific user can be selected, using
  CFG_HW_GPIO_START_PIN_SELECT_USER in wsp_weightscale_config.h file.

## PTS testing
- By using the wsp_weightscale application the user is able to perform PTS tests for services listed
  below:
  * Weightscale Service (WSS),
  * Body Composition Service (BCS),
  * User Data Service (UDS),
  * Device Information Service (DIS),
  * Current Time Service (CTS).
- During testing the WSP please activate multiple users support by setting the CFG_MULTIPLE_CLIENTS
  macro to 1.

- PTS actions that require user intervention are described below:
  * Measurement generation: to be able to trigger WSS and BCS measurements press the K1 button,
  * Current time manual change notification: to be able to trigger CTS 'Manual Change' a write of
    valid data to CTS service needs to be performed.

## Manual testing
- Before any action user configuration shall be done to be able to use wsp_weightscale
  application. Each user has been assigned a specific pin. For a user to be selected "his pin"
  (('User Selected Port'.X + 'User Selected Start Pin'), where X is the user_id) should be connected
  to the ground (GND), while the rest of the pins must be connected to VCC (3.3V). None of the pins
  should be floating!

- By using 'User Control Point' with UUID 0x2A9F located in UDS the user is able to perform
  the actions listed below:
  * (0x01) 'Register New User' - user can be registered in a service (generating measurement is
    enabled)
  * (0x02) 'Consent' - using this command the user is able to authenticate identity in a service,
    (receiving measurement is enabled),
  * (0x03) 'Delete User Data' - the user profile is removed from a service.
  * User data stored in UDS service are non-volatile. To remove this data from the service the user
    must use 'Delete User Data' command or use remove functions available in Debug Service described
    below.

- The procedure to issue the above commands is described below:
  * build the demo for execution from qspi
  * download the demo to qspi and execute
  * select a pin from the 'User Selected Port' port and connect it to the ground (GND). By this
    operation a user is chosen, about whom data will be sent. The rest of the pins on this port
    must be connected to VCC!
  * scan for the device by name "Dialog Weight Scale" and connect to it
  * perform pairing procedure with "Dialog Weight Scale"
  * write value to 'User Control Point' in User Data Service:
    + 0x01 - 'Register New User', the consent value shall be set from range (0-9999) and then
      press send. In a response, 'Response Value' shall be set to 0x01 (success) and a new user
      index value shall be returned.
    + 0x02 - 'Consent'. To be authorized as a user with id set to 0 set 'User index' value to 0
      and 'consent value' corresponding to this user (set before in 'Register New Use' command),
      and then press send. In a response, 'Response Value' shall be set to 0x01 (success).
    + 0x03 - 'Delete User Data' and press send. In a response, 'Response Value' shall be set to
      0x01 (success). Note that to be able to perform this command the user must first perform
      'Consent' command (0x02).

- The procedure to generate WSS and BCS measurements is described below:
  * build the demo for execution from qspi
  * download the demo to qspi and execute
  * select a pin from the 'User Selected Port' port and connect it to the ground (GND). By this
    operation a user is chosen, about whom data will be sent. The rest of the pins on this port
    must be connected to VCC!
  * scan for the device by name "Dialog Weight Scale" and connect
  * perform pairing procedure with "Dialog Weight Scale"
  * write value to 'User Control Point' in User Data Service:
    + 0x01 - 'Register New User', the consent value shall be set from range (0-9999) and then
      press send. In a response, 'Response Value' shall be set to 0x01 (success) and a new user
      index value shall be returned
    + 0x02 - 'Consent'. To be autorized as a user with id set to 0 set 'User index' value to 0
      and 'consent value' corresponding to this user (set before using 'Register New User' command),
      and then press send. In a response, 'Response Value' shall be set to 0x01 (success)
    + in WSS, enable the Weight Measurement Indication
    + in BCS, enable the Body Composition Measurement Indication
    + pressing the K1 button shall generate a measurement for WSS and BCS services

- The procedures to generate CTS 'Manual Change' notification are described below:
  * build the demo for execution from qspi
  * download the demo to qspi and execute
  * scan for the device by name "Dialog Weight Scale" and connect
  * select 'Current Time Service'
  * write valid value to 'Current Time'

- For BCS and WSS, measurements are triggered by using the K1 button.

- The Debug Service can be discovered using the following UUIDs:
  * UUID_DLGDEBUG        "6b559111-c4df-4660-818e-234f9e17b290"
  * UUID_DLGDEBUG_CP     "6b559111-c4df-4660-818e-234f9e17b291"

  + In Debug service, write "uds remove x" to the UUID_DLGDEBUG_CP characteristic where x is a user
    ID selected to remove from the UDS database.

  + In Debug service, write "uds removeall" to the UUID_DLGDEBUG_CP characteristic to remove all
    users from the UDS database.

  + In Debug service, write "pts attach_user x" to the UUID_DLGDEBUG_CP characteristic, where x is
    set to a non-zero value then the feature is enabled (test mode). When this feature is activated
    and at least one user has been registered, stored measurements are transmitted to each
    registered user. This 'Test mode' can be useful when one wishes to test WSS or BCS without UDS
    functionality ("Register New User"/"Consent").

  + In Debug service, write "bas set_level x" to the UUID_DLGDEBUG_CP characteristic, where x is
    a value of the reported battery level.
