/************************************************************************************************************************************************************
*		Файл: freeRTOS_tasks.c
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

#include "freeRTOS_tasks.h"
#include "PCB_BluePill.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os.h"
#include "semphr.h"


#include "usb_device.h"
#include "usbd_cdc_if.h"

uint16_t Delay_LED; // Задержка (1/2 периода) мигания LED. В данном случае период тика ОС 1 мс.
const uint16_t *pDelay_LED; // Указатель на Delay_LED для передачи как параметра задачи. const обеспечивает запись во flash (не в стеке). В оригинальном примере
// еще добавлен static

char USB_Tx_Buf_Task1[24]; // Буфер для передачи в ПК.
const char *USB_Tx_Buf_Task2 = "Task2 send\r\n"; // Указатель на массив символов

xQueueHandle xQueue1; // Декларирование переменной xQueueHandle т.е. создание ссылки на будущую очередь

// Внимание! QueueHandle_t является более современным аналогом xQueueHandle. Разобраться с этим позже

void freeRTOS_Tasks_Ini (void)
{
	xQueue1 = xQueueCreate(4, sizeof(char)); // Создание очереди из 4 элементов размерностью 8 бит

	vSemaphoreCreateBinary(xSemaphoreBinary1); // Создание двоичного семафора

	xTaskCreate(vTask_USB_Init, "Task_USB_Init", 100, NULL, 2, NULL); // З-а сброса лнии D+ после каждого запуска МК. Необхадимо для определения устройсва на шине USB.
	//xTaskCreate(vTask_Transmit_VCP, "Task_Transmit_VCP", 120, NULL, 1, NULL); // З-а переиодческой отправки сообщения в VCP. Задача должна быть запущена после удаления vTask_USB_Init.
	//xTaskCreate(vTask_Transmit_VCP_2, "Task_Transmit_VCP_2", 120, (void*) USB_Tx_Buf_Task2, 1, NULL); // Вывод второго тестового сообщения
	xTaskCreate(vTask_Sync_Recieve_VCP, "Task_Sync_Recieve_VCP", 120, NULL, 2, NULL);

	Delay_LED = 500;
	pDelay_LED = &Delay_LED;


	if(xQueue1 != NULL) // Если очередь создалась успешно (хватило место в куче), то создать задачи отправки получения данных
	{
		//xTaskCreate(vTask_Queue_Data_Send, "Task_Queue_Data_Send", 90, NULL, 1, NULL); // З-а отправки данных в очередь
		//xTaskCreate(vTask_Queue_Data_Recieve, "Task_Queue_Data_Recieve", 90, NULL, 1, NULL); // З-а которая получает данные из очереди и отправляет тестовое сообщение.

		if (xTaskCreate(vTask_PCB_LED_Blink, "Task_PCB_LED_Blink", 40, (void*) pDelay_LED, 1, NULL) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) // З-а мигания LED
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		}
		else
		{

		}
	}
	else
	{
		//Если все хорошо то ветка else не исполнится, что означает что очередь создана.

	}

	osKernelStart();

}

void vTask_Sync_Recieve_VCP(void *pvParameters)
{

	for(;;)
	{
		//vTaskDelay(1000 / portTICK_RATE_MS );

		xSemaphoreTake( xSemaphoreBinary1, portMAX_DELAY );

		PCB_OutString_VCP((unsigned char*) "Data received from VCP = ");

		//(CDC_Transmit_FS((unsigned char*)"Data received from VCP = ", strlen("Data received from VCP = ")));
		vTaskDelay(50 / portTICK_RATE_MS );
		//(CDC_Transmit_FS((unsigned char*) &VCP_Rx_Buf, 10));
		PCB_OutData_VCP((unsigned char*) &VCP_Rx_Buf, 10);
		vTaskDelay(50 / portTICK_RATE_MS );

		PCB_OutString_VCP((unsigned char*) "\r\n");

		//(CDC_Transmit_FS((unsigned char*)"\r\n", strlen("\r\n")));
		vTaskDelay(50 / portTICK_RATE_MS );

	}
	vTaskDelete(NULL);
}



void vTask_Queue_Data_Send(void *pvParameters)
{
	char Queue_Data_Send = 'Q'; // Передаваемы данные

	portBASE_TYPE xStatus; // Статус операции

	for(;;)
	{
		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

		// Отправка данных в очередь xQueue1, данные - Queue_Data, время ожидания появления свободного места в очереди 0, т.е. в очереди должны быть хотя бы одно свободное место

		xStatus = xQueueSendToBack(xQueue1, &Queue_Data_Send, 0);

		if (xStatus != pdPASS)
		{
			while (CDC_Transmit_FS((unsigned char*)"Could not send to the queue.\r\n", strlen("Could not send to the queue.\r\n"))); // Если не было свободного места в очереди
		}
		else
		{
			//while (CDC_Transmit_FS((unsigned char*)"send to the queue.\r\n", strlen("send to the queue.\r\n")));
		}

		/*Вызов taskYIELD() информирует шедулер, что сейчас сразу
        нужно переключиться к другой задаче, а не поддерживать эту задачу в
        состоянии Running до окончания текущего слайса времени*/

		//taskYIELD();
		vTaskDelay(1000 / portTICK_RATE_MS );

	}
	vTaskDelete(NULL);
}

void vTask_Queue_Data_Recieve(void *pvParameters)
{
	char Queue_Data_Recieve; // Переменная для хранения полученных данных из очереди

	portBASE_TYPE xStatus; // Статус операции

	for(;;)
	{
		// Попытаться (если очередь не пуста) получить данные из очереди



		xStatus = xQueueReceive(xQueue1, &Queue_Data_Recieve, 150); // Очередь откуда брать данные, переменная куда сохраняются данные, время ожидания появления данных в очереди

		if(xStatus == pdPASS)
		{
			while (CDC_Transmit_FS((unsigned char*)"Received from xQueue1 = ", strlen("Received from xQueue1 = ")));
			while (CDC_Transmit_FS((unsigned char*) &Queue_Data_Recieve, 1));
			while (CDC_Transmit_FS((unsigned char*)"\r\n", strlen("\r\n")));
		}
		else
		{
			while (CDC_Transmit_FS((unsigned char*)"Could not receive from the queue.\r\n", strlen("Could not receive from the queue.\r\n")));
		}
		vTaskDelay(1000 / portTICK_RATE_MS );
	}

	vTaskDelete(NULL);
}



void vTask_Transmit_VCP_2(void *pvParameters)
{
	char *vTask2_Name;

	vTask2_Name = (char*) pvParameters;

	for(;;)
	{
		while(CDC_Transmit_FS((unsigned char*)vTask2_Name, strlen(vTask2_Name))); // Пытаться послать данные до тех пор USB не будет готов к передаче очерендной посылки.
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


