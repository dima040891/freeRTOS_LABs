/************************************************************************************************************************************************************
*		Файл: freeRTOS_tasks.h
*		Версия/дата: v1.01/20.01.2021
*		Наименование проекта:
*		Разработал: Тимофеев Д.Г.
*		Организация:
*
*		Описание к файлу:
*
*
*		Примечания:
*
*
************************************************************************************************************************************************************/

#ifndef INC_FREERTOS_TASKS_H_
#define INC_FREERTOS_TASKS_H_

#include "FreeRTOS.h"
#include "semphr.h"

xSemaphoreHandle xSemaphoreBinary1; // Декларирование переменной xSemaphoreBinary1, т.е. создание ссылки на будущий семафор

xQueueHandle xQueue1; // Декларирование переменной xQueueHandle т.е. создание ссылки на будущую очередь

void freeRTOS_Tasks_Ini (void); // Функция создания задач freeRTOS. Задачи создаются из задачи idle task на запущенном планировщике.

void vTask_PCB_LED_Blink(void *pvParameters); // З-а мигания индикаторного светодиода.

void vTask_USB_Init(void *pvParameters); // З-а сброса лнии D+ после каждого запуска МК. Необхадимо для определения устройсва на шине USB.

void vTask_Transmit_VCP(void *pvParameters); // З-а переиодческой отправки сообщения в VCP. Задача должна быть запущена после удаления vTask_USB_Init.

void vTask_Transmit_VCP_2(void *pvParameters); // З-а переиодческой отправки сообщения №2 в VCP. Задача должна быть запущена после удаления vTask_USB_Init.

void vTask_Queue_Data_Send(void *pvParameters); // З-а отправки данных в очередь

void vTask_Queue_Data_Recieve(void *pvParameters); // З-а которая получает данные из очереди и отправляет тестовое сообщение.

void vTask_Sync_Recieve_VCP(void *pvParameters); // З-а засинхронизированная с приемом данных по VCP с помощью семфара

void vTask_SyncQueueISR_Data_Recieve(void *pvParameters); // З-а засинхронизированная с приемом данных по VCP, данные при этом передаются из прерываения в задачу по средством защенной версии очереди

#endif /* INC_FREERTOS_TASKS_H_ */
