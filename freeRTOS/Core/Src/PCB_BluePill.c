/************************************************************************************************************************************************************
*		Файл: PCB_BluePill.c
*		Версия/дата: v1.00/04.01.2021
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

#include "PCB_BluePill.h"
#include "main.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"

void PCB_Init()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

void PCB_LED_Toggle()
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

void PCB_OutString_VCP (unsigned char *out_str)
{
	CDC_Transmit_FS(out_str, strlen((char*)out_str));
}

void PCB_OutData_VCP(uint8_t* data, uint16_t len)
{
	CDC_Transmit_FS(data, len);
}


