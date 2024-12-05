/*
 * led.c
 *
 *  Created on: Apr 8, 2024
 *      Author: johnt
 */

#include "led.h"

void LEDSequence_Init(LEDSequence* led, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, volatile uint32_t* time_micros_ptr)
{
	/* set the physical pin */
	led->GPIO_Port = GPIO_Port;
	led->GPIO_Pin = GPIO_Pin;

	/* time-keeping variable */
	led->time_micros_ptr = time_micros_ptr;

	/* default values for sequence variables */
	led->blink_sequence_array = NULL;
	led->blink_sequence_len = 0;
	led->blink_sequence_index = 0;
	led->time_last_blink = 0;
	led->blink_state = 0;

	led->burst_sequence_array = NULL;
	led->burst_sequence_len = 0;
	led->burst_sequence_index = 0;
	led->time_last_burst = 0;
	led->burst_state = 0;

	/* turn off the LED */
	HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, GPIO_PIN_RESET);
}

void LEDSequence_Update(LEDSequence* led)
{
	/* run the sequencer */

	/* update the blink sequence */
	if (led->blink_sequence_array != NULL)
	{
		if (*(led->time_micros_ptr) - led->time_last_blink > led->blink_sequence_array[led->blink_sequence_index])
		{
			led->blink_state = !led->blink_state;  /* invert the blink state */
			led->time_last_blink = *(led->time_micros_ptr);  /* store the blink time */

			if (++led->blink_sequence_index == led->blink_sequence_len) /* increment the index */
			{
				led->blink_sequence_index = 0;
			}
		}
	}

	/* update the burst sequence */
	if (led->burst_sequence_array != NULL && led->burst_sequence_index < led->burst_sequence_len)
	{
		if (*(led->time_micros_ptr) - led->time_last_burst > led->burst_sequence_array[led->burst_sequence_index])
		{
			led->burst_state = !led->burst_state;  /* invert the burst state */
			led->time_last_burst = *(led->time_micros_ptr);  /* store the time */
			++led->burst_sequence_index;  /* increment the index */
		}
	}

	/* set the LED state */
	if (led->burst_sequence_index < led->burst_sequence_len)
	{
		/* currently running a burst sequence which supersedes the blink sequence */
		HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, led->burst_state);
	}
	else
	{
		/* use the blink sequence state to set the LED */
		HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, led->blink_state);
	}

}

void LEDSequence_SetBlinkSequence(LEDSequence* led, const uint32_t* blink_sequence_array, const uint16_t blink_sequence_len)
{
	/* set the continuous blink sequence for the LED */
	led->blink_sequence_array = blink_sequence_array;
	led->blink_sequence_len = blink_sequence_len;
	led->blink_sequence_index = 0;

	/* reset the sequence state variables */
	led->time_last_blink = *(led->time_micros_ptr);
	led->blink_state = 0;

}

void LEDSequence_SetBurstSequence(LEDSequence* led, const uint32_t* burst_sequence_array, const uint16_t burst_sequence_len)
{
	/* set a burst sequence to be performed once */
	led->burst_sequence_array = burst_sequence_array;
	led->burst_sequence_len = burst_sequence_len;
	led->burst_sequence_index = 0;

	/* reset the burst state variables */
	led->time_last_burst = *(led->time_micros_ptr);
	led->burst_state = 0;
}

