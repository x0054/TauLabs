/**
 ******************************************************************************
 * @addtogroup PIOS PIOS Core hardware abstraction layer
 * @{
 * @addtogroup PIOS_FREERTOS_TRANSITION
 * @brief Transition layer for FreeRTOS primitives
 * @{
 *
 * @file       pios_freertos_transition.c
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2014
 * @brief      FreeRTOS transision layer source
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

#include "pios.h"
#include "pios_freertos_transition.h"

#include <string.h>

/* Removed functions */
void SystemInit(void)
{
}

void SystemCoreClockUpdate(void)
{
}

/* Queue */
#define QUEUE_MAX_WAITERS 1
xQueueHandle xQueueCreate(size_t max_queue_size, size_t obj_size)
{
	/* Create the xQueueHandle struct. */
	xQueueHandle queue = PIOS_malloc(sizeof(*queue));
	if (queue == NULL)
		return NULL;

	/* Create the memory pool. */
	chPoolInit(&queue->mp, obj_size, NULL);
	chPoolLoadArray(&queue->mp,
			PIOS_malloc(obj_size * (max_queue_size + QUEUE_MAX_WAITERS)),
			(max_queue_size + QUEUE_MAX_WAITERS));

	/* Now create the mailbox. */
	msg_t *mb_buf = PIOS_malloc(sizeof(msg_t) * max_queue_size);
	chMBInit(&queue->mb, mb_buf, max_queue_size);

	return queue;
}

signed long xQueueSendToBack(xQueueHandle queue, void* data, systime_t timeout)
{
	void* buf = chPoolAlloc(&queue->mp);
	if (buf == NULL)
		return errQUEUE_FULL;

	memcpy(buf, data, queue->mp.mp_object_size);

	msg_t result = chMBPost(&queue->mb, (msg_t)buf, timeout);

	if (result == RDY_OK)
		return pdTRUE;

	chPoolFree(&queue->mp, buf);

	return errQUEUE_FULL;
}

signed long xQueueReceive(xQueueHandle queue, void* data, systime_t timeout)
{
	msg_t buf;

	msg_t result = chMBFetch(&queue->mb, &buf, timeout);

	if (result != RDY_OK)
		return pdFALSE;

	memcpy(data, (void*)buf, queue->mp.mp_object_size);

	chPoolFree(&queue->mp, (void*)buf);

	return pdTRUE;
}

signed long xQueueSendToBackFromISR(xQueueHandle queue, void* data, signed long *woken)
{
	chSysLock();
	void *buf = chPoolAllocI(&queue->mp);
	if (buf == NULL)
	{
		chSysUnlock();
		return errQUEUE_FULL;
	}

	memcpy(buf, data, queue->mp.mp_object_size);

	msg_t result = chMBPostI(&queue->mb, (msg_t)buf);

	if (result == RDY_OK)
		return pdTRUE;

	chPoolFreeI(&queue->mp, buf);

	chSysUnlock();

	return errQUEUE_FULL;
}
