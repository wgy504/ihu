#include "rdtest_lowlevel.h"
#include "rdtest_support.h" 
#include "global_io.h" 

void rdtest_initialize(uint8 version);
void rdtest_vppcontrol(uint8 state);
void rdtest_select_pulsewidth(uint8 length);
void rdtest_uart_connect(uint16 connectmap16);
void rdtest_make_loopback(uint8 port);
void rdtest_vbatcontrol(uint16 connectmap16);
void rdtest_resetpulse(uint16 delayms);
void rdtest_pulse_to_uart(uint16 connectmap16);
void rdtest_generate_pulse(void);
