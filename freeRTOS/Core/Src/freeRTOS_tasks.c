/*
 * freeRTOS_tasks.c
 *
 *  Created on: 5 янв. 2021 г.
 *      Author: dima0
 */

#include "freeRTOS_tasks.h"
#include "PCB_BluePill.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

void freeRTOS_Tasks_Ini (void)
{
	xTaskCreate(vTask_PCB_LED_Toggle, "Task_PCB_LED_Toggle", 40, NULL, 1, NULL); // Название задачи (функции), Название задачи для отладки, размер стекка задачи,
																				  // параметры передаваемые в задачу, приоритет, хэндл (ссылка) на задачу.
}



void vTask_PCB_LED_Toggle(void *pvParameters)
{

	for(;;)
	{
	PCB_LED_Toggle();
	vTaskDelay(500);
	PCB_LED_Toggle();
	vTaskDelay(500);
	}
}


