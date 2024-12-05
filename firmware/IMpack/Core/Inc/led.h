/*
 * led.h
 *
 *  Created on: Apr 8, 2024
 *      Author: johnt
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "main.h"

/*
 * A blink sequencer to use as an indicator LED
 */
typedef struct
{
	/* physical pin for the LED */
	GPIO_TypeDef* GPIO_Port;
	uint16_t GPIO_Pin;

	/* pointer to microsecond time-keeping variable */
	volatile uint32_t* time_micros_ptr;

	/* blink sequence */
	const uint32_t* blink_sequence_array;  /* time delays in microseconds between toggling the LED, starts in the off state */
	uint16_t blink_sequence_len, blink_sequence_index;
	uint32_t time_last_blink;
	uint8_t blink_state;

	/* burst sequence */
	const uint32_t* burst_sequence_array;  /* time delays in microseconds between toggling the LED, starts in the off state */
	uint16_t burst_sequence_len, burst_sequence_index;
	uint32_t time_last_burst;
	uint8_t burst_state;

} LEDSequence;

void LEDSequence_Init(LEDSequence* led, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, volatile uint32_t* time_micros_ptr);
void LEDSequence_Update(LEDSequence* led);  /* run the sequencer */

void LEDSequence_SetBlinkSequence(LEDSequence* led, const uint32_t* blink_sequence_array, const uint16_t blink_sequence_len);  /* set the continuous blink sequence for the LED */
void LEDSequence_SetBurstSequence(LEDSequence* led, const uint32_t* burst_sequence_array, const uint16_t burst_sequence_len);  /* set a burst sequence to be performed once */


#endif /* INC_LED_H_ */
