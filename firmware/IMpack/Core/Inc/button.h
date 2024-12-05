/*
 * button.h
 *
 *  Created on: Apr 8, 2024
 *      Author: johnt
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

typedef struct
{
	/* physical pin for the button */
	GPIO_TypeDef* GPIO_Port;
	uint16_t GPIO_Pin;

	/* pointer to microsecond time-keeping variable */
	volatile uint32_t* time_micros_ptr;

	/* de-bounce time */
	uint32_t time_debounce_micros;

	/* time of last press */
	uint32_t time_last_press_micros;

} ButtonDebounced;

void ButtonDebounced_Init(ButtonDebounced* button, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, volatile uint32_t* time_micros_ptr, uint32_t time_debounce_micros);
uint8_t ButtonDebounced_GetPressed(ButtonDebounced* button);

#endif /* INC_BUTTON_H_ */
