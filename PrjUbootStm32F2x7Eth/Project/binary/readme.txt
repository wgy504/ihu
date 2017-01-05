
These 2 binary images are provided to be loaded with the IAP over Ethernet, they
are compiled to run at address 0x0800C000:
   - STM322xG_EVAL_SysTick.bin: binary image of systick example, refer to the "STM32F2xx
                                Standard Peripherals Firmware Library" for the source code 
   - STM322xG_EVAL_httpserver.bin: binary image of httpserver demo, refer to the 
                                   "AN3384 LwIP TCP/IP stack demonstration for
                                   STM32F2x7xx microcontrollers" for the source code

    @Note In order to make the STM322xG_EVAL_httpserver demo works, please ensure
          that the STM322xG_EVAL board RevB jumpers are connected as bellow:
             - JP5 in position 1-2
             - JP6 in position 2-3
             - JP8 not fitted   

