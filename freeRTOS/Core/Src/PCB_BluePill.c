#include "PCB_BluePill.h"
#include "main.h"

//#include "FreeRTOS.h"
//#include "task.h"

void PCB_Init()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

void PCB_LED_Toggle()
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}


