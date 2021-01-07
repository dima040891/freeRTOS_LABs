/************************************************************************************************************************************************************
*		Файл: freeRTOS_tasks.c
*		Версия/дата: v0.00/05.01.2021
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

#include "freeRTOS_tasks.h"
#include "PCB_BluePill.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"

uint16_t Delay_LED; // Задержка (1/2 периода) мигания LED. В данном случае период тика ОС 1 мс.
const uint16_t *pDelay_LED; // Указатель на Delay_LED для передачи как параметра задачи. const обеспечивает запись во flash (не в стеке). В оригинальном примере
// еще добавлен static

void freeRTOS_Tasks_Ini (void)
{

	xTaskCreate(vTask_USB_Init, "Task_USB_Init", 400, NULL, 2, NULL);

	Delay_LED = 500;
	pDelay_LED = &Delay_LED;

	xTaskCreate(vTask_PCB_LED_Toggle, "Task_PCB_LED_Toggle", 40, (void*) pDelay_LED, 1, NULL); // Задача минания LED

}


void vTask_USB_Init(void *pvParameters)
{

	/* Кратковременный перевод GPIOA_12 на выход с записью в него лог. 0 + задержка. Необхадимо для определения USB устройсва после
	 перепрошивки/сброса МК. Не забыть закмментить родной вызов MX_USB_DEVICE_Init();  после каждой генерации STM32IDE.*/

	// НЕ ЗАБЫТЬ ЗАКОММЕНТИТЬ ВЫЗОВ MX_USB_DEVICE_Init(); ИЗ ЗАДАЧИ StartDefaultTask (main.c) ПОСЛЕ КАЖДОЙ ГЕНЕРАЦИИ ПРОЕКТА!!!

	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Pin = GPIO_PIN_12;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); // Запись 0, притянуть D+ к земле.

	  vTaskDelay(2000);

	  MX_USB_DEVICE_Init(); // Инициализация USB микроконтроллераа

	  vTaskDelete(NULL); // Удаленеи задачи самой себя

	for(;;)
	{
		vTaskDelay(1000);
	}


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


