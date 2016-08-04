/**
 ****************************************************************************************
 *
 * @file bxxh_adapt.h
 *
 * @brief Adaptation of SPS project
 *
 * BXXH team
 * Created by ZJL, 20150819
 *
 ****************************************************************************************
 */

#include "vmlayer.h"

extern char pDebugPrintBuf[BX_PRINT_SZ];

//L3 no toughed function
extern void bxxh_init(void);
extern void bxxh_task_entry_hook1(void);

//L3 free call function
extern char *hexbyte2string(uint8_t input);
extern char *hexword2string(uint16_t input);
extern char *hexint2string(unsigned int input);
extern void taskid_print(uint8_t id);
extern void msgid_print(uint16_t id);
extern void SetDebugTraceOnOff(bool debugFlag);
extern void SetDebugTraceOn(void);
extern void SetDebugTraceOff(void);

//外部引用API
extern void task_asylibra_init(void);
extern void task_asylibra_mainloop(void);
extern void task_wxapp_init(void);
extern void task_wxapp_mainloop(void);




