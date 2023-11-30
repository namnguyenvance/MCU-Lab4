/*
 * scheduler.c
 *
 *  Created on: Nov 29, 2023
 *      Author: nvnha
 */
#include "scheduler.h"

sTasks SCH_tasks_G[SCH_MAX_TASKS];
uint32_t current_index_task = 0;

void SCH_Init(void) {
	current_index_task = 0;
	uint8_t i;
	for (i = 0; i < SCH_MAX_TASKS; i++) {
		SCH_Delete_Task(i);
	}
}

void SCH_Update(void) {
	uint8_t Index;
	for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
		if (SCH_tasks_G[Index].pTask) {
			if (SCH_tasks_G[Index].Delay == 0) {
				SCH_tasks_G[Index].RunMe += 1;
				if (SCH_tasks_G[Index].Period) {
					SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period;
				}
			}
			else {
				SCH_tasks_G[Index].Delay -= 1;
			}
		}
	}
}

uint8_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
	uint8_t Index = 0;
	while ((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS)) {
		Index++;
	}
	if (Index == SCH_MAX_TASKS) {
		return SCH_MAX_TASKS;
	}
	SCH_tasks_G[Index].pTask = pFunction;
	SCH_tasks_G[Index].Delay = DELAY/TICK;
	SCH_tasks_G[Index].Period = PERIOD/TICK;
	SCH_tasks_G[Index].RunMe = 0;
	SCH_tasks_G[Index].TaskID = current_index_task;
	current_index_task++;
	return Index;
}

void SCH_Dispatch_Tasks(void) {
	uint8_t Index;
	for (Index = 0; Index < current_index_task; Index++) {
		if (SCH_tasks_G[Index].RunMe > 0) {
			(*SCH_tasks_G[Index].pTask)();
			SCH_tasks_G[Index].RunMe -= 1;
			if (SCH_tasks_G[Index].Period == 0) {
				SCH_Delete_Task(Index);
			}
		}
	}
}
uint8_t SCH_Delete_Task(const uint32_t TASK_INDEX) {
	if (TASK_INDEX > SCH_MAX_TASKS) {
		return TASK_INDEX;
	}
	for (int i = TASK_INDEX; i < SCH_MAX_TASKS-1; i++) {
		SCH_tasks_G[i] = SCH_tasks_G[i+1];
	}
	SCH_tasks_G[SCH_MAX_TASKS -1].pTask = 0x0000;
	SCH_tasks_G[SCH_MAX_TASKS -1].Delay = 0;
	SCH_tasks_G[SCH_MAX_TASKS -1].Period = 0;
	SCH_tasks_G[SCH_MAX_TASKS -1].RunMe = 0;
	current_index_task--;
	return TASK_INDEX;
}
