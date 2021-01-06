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

uint16_t Delay_LED; // Задержка (1/2 периода) мигания LED. В данном случае период тика ОС 1 мс.
const uint16_t *pDelay_LED; // Указатель на Delay_LED для передачи как параметра задачи. const обеспечивает запись во flash (не в стеке). В оригинальном примере
// еще добавлен static

void freeRTOS_Tasks_Ini (void)
{
	Delay_LED = 500;
	pDelay_LED = &Delay_LED;

	// Название задачи (функции), Название задачи для отладки, размер стекка задачи, параметры передаваемые в задачу, приоритет, хэндл (ссылка) на задачу.

	xTaskCreate(vTask_PCB_LED_Toggle, "Task_PCB_LED_Toggle", 40, (void*) pDelay_LED, 1, NULL); // Параметры передаются через указатель на тип void.
	// (void*) pDelay_LED - привдение pDelay_LED к указателю на void.

}



void vTask_PCB_LED_Toggle(void *pvParameters)
{

	uint16_t *pDelay_LED = (uint16_t*) pvParameters; // Получение параметра с привденем к иходному типу данных - uint16_t

	for(;;)
	{
	PCB_LED_Toggle();
	vTaskDelay(*pDelay_LED); // Разыменование т.е. передача значения
	PCB_LED_Toggle();
	vTaskDelay(*(uint16_t*)pvParameters); // Можно и без промежуточных переменных, привести pvParameters к указателю uint16_t "(uint16_t*)pvParameters", а затем разименовать
	}
}


