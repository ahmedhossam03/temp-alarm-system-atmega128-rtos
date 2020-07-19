#ifndef RTOS_TAS_H_
#define RTOS_TAS_H_

#include "avr/io.h"
#include "utils/data_types.h"
#include "utils/bit_handle.h"

#include "FreeRTOS/inc/FreeRTOS.h"
#include "FreeRTOS/inc/task.h"
#include "FreeRTOS/inc/queue.h"
#include "FreeRTOS/inc/semphr.h"
#include "FreeRTOS/inc/event_groups.h"

#define TAS_LINE_EDIT		0
#define TAS_TEMP_EDIT		2
#define TAS_THRE_EDIT		8
#define TAS_AM_EDIT			15

//rtos event group
#define E_ALRM_ACTV (1 << 0)			//event for activating alarm
#define E_MAIN_DISP (1 << 1)			//event for displaying main
#define E_KCON_DISP (1 << 2)			//event for displaying key config
#define E_TCON_DISP (1 << 3)			//event for displaying terminal config
#define E_ALRM_DISP (1 << 4)			//event for displaying alarm
#define E_TEMP_DISP (1 << 5)			//event for updating temp display
#define E_THRE_DISP (1 << 6)			//event for updating threshold
#define E_AMOD_DISP (1 << 7)			//event for updating alarm mode
EventGroupHandle_t	eg_tas;
EventBits_t			eb_vals;

//task functions
void t_tas_alarm(void* ptr);
void t_tas_check(void* ptr);
void t_tas_key_config(void* ptr);
void t_tas_temp_update(void *ptr);
void t_tas_uart_config(void* ptr);
void t_tas_disp(void* ptr);

//main functions
void rtos_tas_init();
void rtos_tas_run();
void rtos_tas_toggle_am();

#endif /* RTOS_TAS_H_ */