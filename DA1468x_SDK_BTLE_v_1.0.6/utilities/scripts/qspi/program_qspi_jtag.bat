@SETLOCAL
@set on_off=OFF
@echo %on_off%
SETLOCAL ENABLEDELAYEDEXPANSION
@echo .......................................................................................................................
@echo ..
@echo .. QSPI PROGRAMMING
@echo ..
@echo .......................................................................................................................
@echo.

set TMPCFG=

:more_options
if "%1"=="" goto :no_more_options
if /i "%1"=="--id" (set id=%2
    set device_sn=%2
    shift
    shift
    goto :more_options)
if /i "%1"=="--cfg" (set id=%2
    set cfg=%2
    shift
    shift
    goto :more_options)
:no_more_options

set a=%2

REM Remove trailing slash if any
IF %a:~-2,-1%==/ SET a=%a:~0,-2%" 

REM Transform slashes to backslashes
set a=%a:/=\%
@echo JlinkGDBServer path used: %a%

@REM Remove quotes if any
for /F "tokens=*" %%i in (%a%) do set jlink_path=%%~i

if NOT exist %1 (
@echo %1 not found!
@echo Please select the folder which contains the binary you want to program and try again.
goto :Finished
)

set IMAGE=%1

if exist program_qspi_ini.cmd goto config_found
call program_qspi_config.bat

:config_found
call program_qspi_ini.cmd

if not "%PRODUCT_ID%"=="" set PRODUCT_ID=--prod-id %PRODUCT_ID%
if not "%ENABLE_UART%"=="" set ENABLE_UART=--enable-uart %ENABLE_UART%
if not "%RAM_SHUFFLING%"=="" set RAM_SHUFFLING=--ram-shuffling %RAM_SHUFFLING%

IF NOT "%CFG%"=="" goto :exec_cli

set TMPCFG=%TEMP%\cfg_%RANDOM%.ini
set CFG=%TMPCFG%


set jlink_output_file=%CD%\t
if NOT "%device_sn%" == "" goto START_gdb
set device_sn=Unknown
set device_total=Unknown
REM Read attached Jlink devices
echo exit | "%jlink_path%\JLink.exe" > %jlink_output_file%
for /f "tokens=2 SKIP=2" %%f in ('%windir%\system32\FIND.exe "S/N" %jlink_output_file%') do set device_sn=%%f

if %device_sn% == Unknown (
@echo No Jlink device found.
goto :Finished
)

:START_gdb
@echo Use device %device_sn%
if exist %CFG% goto add_device_id
@rem create cli_programmer.ini in current directory
"..\..\..\binaries\cli_programmer.exe" --save %CFG%

:add_device_id
@rem Put device sn in cli_programmer.ini file
%windir%\system32\CScript /Nologo prepare_local_ini_file.vbs --id %device_sn% --cfg "%CFG%"

:exec_cli

@echo on
"..\..\..\binaries\cli_programmer.exe" --cfg "%CFG%" %PRODUCT_ID% %ENABLE_UART% %RAM_SHUFFLING% gdbserver write_qspi_exec %IMAGE%

@echo %on_off%

if not "%TMPCFG%"=="" del %TMPCFG%

goto :Finished

:Finished
@echo.
@echo .......................................................................................................................
@echo ..
@echo .. FINISHED!
@echo ..
@echo .......................................................................................................................
