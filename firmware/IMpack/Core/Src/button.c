/*
 * button.c
 *
 *  Created on: Apr 8, 2024
 *      Author: johnt
 */

#include "button.h"

void ButtonDebounced_Init(ButtonDebounced* button, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, volatile uint32_t* time_micros_ptr, uint32_t time_debounce_micros)
{
	button->GPIO_Port = GPIO_Port;
	button->GPIO_Pin = GPIO_Pin;
	button->time_micros_ptr = time_micros_ptr;
	button->time_debounce_micros = time_debounce_micros;
	button->time_last_press_micros = 0;
}

uint8_t ButtonDebounced_GetPressed(ButtonDebounced* button)
{
	if (HAL_GPIO_ReadPin(button->GPIO_Port, button->GPIO_Pin) == GPIO_PIN_SET && *(button->time_micros_ptr) - button->time_last_press_micros > button->time_debounce_micros)
	{
		button->time_last_press_micros = *(button->time_micros_ptr);
		return 1;
	}
	else
	{
		return 0;
	}
}

