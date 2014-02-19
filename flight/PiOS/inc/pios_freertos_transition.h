/**
 ******************************************************************************
 * @addtogroup PIOS PIOS Core hardware abstraction layer
 * @{
 * @addtogroup PIOS_FREERTOS_TRANSITION
 * @brief Transition layer for FreeRTOS primitives
 * @{
 *
 * @file       pios_freertos_transition.h
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2014
 * @brief      FreeRTOS transision layer header
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef PIOS_FREERTOS_TRANSITION_H_
#define PIOS_FREERTOS_TRANSITION_H_

#include "ch.h"
#include "pios_freertos_transition_priv.h"

/* Direct replacements */
#define portENTER_CRITICAL chSysLock
#define portLEAVE_CRITICAL chSysUnlock

#define TICKS2MS(t) (t)
#define MS2TICKS(m) MS2ST(m)
//#define portTICK_RATE_MS (1)

#define portCHAR char
#define portBASE_TYPE long
#define portTickType systime_t

#define portMAX_DELAY TIME_INFINITE
#define pdTRUE TRUE
#define pdFALSE FALSE
#define pdPASS 1
#define pdFAIL 0
#define errQUEUE_FULL 0
#define errQUEUE_EMPTY 0

/* Removed functions */
#define portEND_SWITCHING_ISR(x)
void SystemInit(void);
void SystemCoreClockUpdate(void);

/* Queue */
typedef struct {
	Mailbox mb;
	MemoryPool mp;
} *xQueueHandle;
xQueueHandle xQueueCreate(size_t max_queue_size, size_t obj_size);
signed long xQueueReceive(xQueueHandle queue, void* data, systime_t timeout);
#define xQueueSend xQueueSendToBack
signed long xQueueSendToBack(xQueueHandle queue, void* data, systime_t timeout);
#define xQueueSendFromISR xQueueSendToBackFromISR
signed long xQueueSendToBackFromISR(xQueueHandle queue, void* data, signed long *woken);

#endif /* PIOS_FREERTOS_TRANSITION_H_ */
