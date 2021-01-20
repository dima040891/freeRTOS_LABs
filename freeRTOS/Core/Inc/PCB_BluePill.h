/************************************************************************************************************************************************************
*		Файл: PCB_BluePill.h
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

#ifndef INC_PCB_BLUEPILL_H_
#define INC_PCB_BLUEPILL_H_

#include "main.h"


void PCB_Init(); // Инициализация платы

void PCB_LED_Toggle(); // Ф-я включения/отключения индкаторного LED платы

void PCB_OutString_VCP (unsigned char *out_str);

void PCB_OutData_VCP(uint8_t* data, uint16_t len);



#endif /* INC_PCB_BLUEPILL_H_ */
