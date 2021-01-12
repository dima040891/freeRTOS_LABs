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

char USB_Tx_Buf_Task1[24]; // Буфер для передачи в ПК.
const char *USB_Tx_Buf_Task2 = "Task2 send\r\n"; // Указатель на массив символов

void freeRTOS_Tasks_Ini (void)
{
	xTaskCreate(vTask_USB_Init, "Task_USB_Init", 100, NULL, 2, NULL); // З-а сброса лнии D+ после каждого запуска МК. Необхадимо для определения устройсва на шине USB.
	xTaskCreate(vTask_Transmit_VCP, "Task_Transmit_VCP", 120, NULL, 1, NULL); // З-а переиодческой отправки сообщения в VCP. Задача должна быть запущена после удаления vTask_USB_Init.
	xTaskCreate(vTask_Transmit_VCP_2, "Task_Transmit_VCP_2", 120, (void*) USB_Tx_Buf_Task2, 1, NULL); // Вывод второго тестового сообщения

	Delay_LED = 500;
	pDelay_LED = &Delay_LED;
	xTaskCreate(vTask_PCB_LED_Blink, "Task_PCB_LED_Blink", 10, (void*) pDelay_LED, 1, NULL); // З-а мигания LED
}

void vTask_Transmit_VCP_2(void *pvParameters)
{
	char *vTask2_Name;

	vTask2_Name = (char*) pvParameters;

	for(;;)
	{
		while (CDC_Transmit_FS((unsigned char*)vTask2_Name, strlen(vTask2_Name))); // Пытаться послать данные до тех пор USB не будет готов к передаче очерендной посылки.
			//скорее всего ф-я CDC_Transmit_FS() проверяет свобдны ли аппаратные ресурсы МК к передаче по USB. Если нет, то сразу выходит из функции с ошибкой.
		vTaskDelay(500 / portTICK_RATE_MS );
	}
	vTaskDelete(NULL);
}

void vTask_Transmit_VCP(void *pvParameters)
{
	sprintf(USB_Tx_Buf_Task1, "Task1 send\r\n");

	for(;;)
	{
		while(CDC_Transmit_FS((unsigned char*)USB_Tx_Buf_Task1, strlen(USB_Tx_Buf_Task1))); // // Пытаться послать данные до тех пор USB не будет готов к передаче очерендной посылки.
		vTaskDelay(500 / portTICK_RATE_MS );
	}
	vTaskDelete(NULL);

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

	  vTaskDelay(2000 / portTICK_RATE_MS );

	  MX_USB_DEVICE_Init(); // Инициализация USB микроконтроллераа

	  vTaskDelete(NULL); // Удаленеи задачи самой себя

	for(;;)
	{
		vTaskDelay(1000 / portTICK_RATE_MS );
	}
	vTaskDelete(NULL);


}

void vTask_PCB_LED_Blink(void *pvParameters)
{

	uint16_t *pDelay_LED = (uint16_t*) pvParameters; // Получение параметра с привденем к иходному типу данных - uint16_t

	for(;;)
	{
	PCB_LED_Toggle();
	vTaskDelay(*pDelay_LED / portTICK_RATE_MS ); // Разыменование т.е. передача значения задержки
	PCB_LED_Toggle();
	vTaskDelay(*(uint16_t*)pvParameters / portTICK_RATE_MS ); // Можно и без промежуточных переменных, привести pvParameters к указателю uint16_t "(uint16_t*)pvParameters", а затем разименовать
	}
	vTaskDelete(NULL);
}


