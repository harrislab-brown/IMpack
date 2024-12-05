/*
 * Double buffered data logger to SD card
 *
 *  Created on: Apr 5, 2024
 *      Author: johnt
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "fatfs.h"

typedef struct
{
	/* data buffer */
	uint8_t* data_buffer;
	uint32_t data_buffer_len;  /* length in bytes of the full buffer */
	volatile uint32_t data_buffer_index;  /* stored index into the data byte array */
	uint16_t data_point_size;  /* size in bytes of each data point */

	/* double buffering */
	volatile uint8_t ready_to_write;
	uint8_t* volatile write_ptr;

	/* SD card */
	FIL fil;
	FRESULT fresult;
	UINT write_count;

} SDLogger;

void SDLogger_Initialize(SDLogger* logger, uint8_t* data_buffer, uint32_t data_buffer_len, uint16_t data_point_size);

void SDLogger_IncrementDataIndex(SDLogger* logger);  /* call this each time a new data point is added to the buffer */

void SDLogger_StartRecording(SDLogger* logger, char* data_file_name, char* data_file_ext, char* data_file_full, uint16_t* recording_number);  /* open a file to start recording */
void SDLogger_Update(SDLogger* logger);  /* write data to the SD card if it is time to do so */
void SDLogger_StopRecording(SDLogger* logger);  /* write remaining data close the file */

#endif /* INC_LOGGER_H_ */
