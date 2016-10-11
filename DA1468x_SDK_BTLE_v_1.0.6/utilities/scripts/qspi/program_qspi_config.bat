@echo OFF
SETLOCAL ENABLEDELAYEDEXPANSION
@echo .......................................................................................................................
@echo ..
@echo .. QSPI PROGRAMMING CONFIGURATION
@echo ..
@echo .......................................................................................................................
@echo.
set config_file=program_qspi_ini.cmd

if not exist %config_file% goto PRODUCT_ID_input
call %config_file%
if "%PRODUCT_ID%"=="" goto PRODUCT_ID_input
REM Dump existing configuration
@echo Existing configuration:
@echo ----------------------------
@echo PRODUCT_ID=%PRODUCT_ID%
if %PRODUCT_ID% == DA14681-00 (
@echo ENABLE_UART=%ENABLE_UART%
@echo RAM_SHUFFLING=%RAM_SHUFFLING%
)
@echo ----------------------------
@echo.
@echo.

REM Ask for changing existing configuration
set change_config=n
@echo Change existing configuration? ^(y/n or [ENTER] for n^) 
@set /p change_config=-^> 
@echo.
set result=false
if %change_config% == y set result=true
if %change_config% == Y set result=true
if %result% == false (
	goto Finished
)


REM User input for new configuration
:PRODUCT_ID_input
set PRODUCT_ID=1
@echo Product id options:  
@echo        0:  DA14680/1-00 
@echo        1:  DA14680/1-01 (default)
@echo Product id ?  ^(0..1 or [ENTER] for 1^)
@set /p PRODUCT_ID=-^> 
@echo.
set result=false
if %PRODUCT_ID% GEQ 0 if %PRODUCT_ID% LEQ 1 set result=true
if %result% == false (
	echo "Invalid input" 
	goto PRODUCT_ID_input
)
if %PRODUCT_ID% == 0 set PRODUCT_ID=DA14681-00
if %PRODUCT_ID% == 1 set PRODUCT_ID=DA14681-01
@echo PRODUCT_ID=%PRODUCT_ID% 
@echo.
if %PRODUCT_ID% == DA14681-01 goto SaveAcVars

:ENABLE_UART_input
set ENABLE_UART=y
@echo Enable uart ?  ^(y/n or [ENTER] for y^)
@set /p ENABLE_UART=-^> 
@echo.
set result=false
if %ENABLE_UART% == y set result=true
if %ENABLE_UART% == n set result=true
if %result% == false (
	echo "Invalid input" 
	goto ENABLE_UART_input
)
@echo ENABLE_UART=%ENABLE_UART% 
@echo.

:RAM_SHUFFLING_input
set RAM_SHUFFLING=0
@echo Ram shuffling options:  
@echo        0:  8 - 24 - 32 (default)
@echo        1: 24 -  8 - 32
@echo        2: 32 -  8 - 24
@echo        3: 32 - 24 -  8
@echo Ram shuffling ?  ^(0..3 or [ENTER] for 0^)
@set /p RAM_SHUFFLING=-^> 
@echo.
set result=false
if %RAM_SHUFFLING% GEQ 0 if %RAM_SHUFFLING% LEQ 3 set result=true
if %result% == false (
	echo "Invalid input" 
	goto RAM_SHUFFLING_input
)
@echo RAM_SHUFFLING=%RAM_SHUFFLING% 
@echo.

:SaveAdVars
echo set PRODUCT_ID=%PRODUCT_ID% > program_qspi_ini.cmd
echo set ENABLE_UART=%ENABLE_UART% >> program_qspi_ini.cmd
echo set RAM_SHUFFLING=%RAM_SHUFFLING% >> program_qspi_ini.cmd
goto Finished

:SaveAcVars
echo set PRODUCT_ID=%PRODUCT_ID% > program_qspi_ini.cmd

:Finished
@echo.
@echo .......................................................................................................................
@echo ..
@echo .. CONFIGURATION FINISHED!
@echo ..
@echo .......................................................................................................................

