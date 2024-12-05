/*
 * Double buffered data logger to SD card
 *
 *  Created on: Apr 5, 2024
 *      Author: johnt
 */

#include "logger.h"

void SDLogger_Initialize(SDLogger* logger, uint8_t* data_buffer, uint32_t data_buffer_len, uint16_t data_point_size)
{
	/* initialize the member variables */
	logger->data_buffer = data_buffer;
	logger->data_buffer_len = data_buffer_len;
	logger->data_point_size = data_point_size;
	logger->data_buffer_index = 0;

	logger->ready_to_write = 0;
	logger->write_ptr = NULL;
}

void SDLogger_IncrementDataIndex(SDLogger* logger)
{
	/* increment the data buffer index */
	/* call this each time a new data point is added to the data buffer */
	logger->data_buffer_index += logger->data_point_size;

	if (logger->data_buffer_index == logger->data_buffer_len / 2)
	{
		/* ready to write the first half of the data buffer */
		logger->ready_to_write = 1;
		logger->write_ptr = &(logger->data_buffer[0]);
	}
	else if (logger->data_buffer_index == logger->data_buffer_len)
	{
		/* ready to write the second half of the data buffer */
		logger->ready_to_write = 1;
		logger->write_ptr = &(logger->data_buffer[logger->data_buffer_len / 2]);

		/* wrap the buffer index */
		logger->data_buffer_index = 0;
	}
}

void SDLogger_StartRecording(SDLogger* logger, char* data_file_name, char* data_file_ext, char* data_file_full, uint16_t* recording_number)
{

	logger->data_buffer_index = 0;  /* reset the data buffer */
	logger->ready_to_write = 0;


	/*
	 * Determine the correct full file name for the new data file. It should start with the data file name, followed by
	 * a number that increments with each new recording, and then the file extension. This function will return the recording
	 * number by pointer. The resulting data file will be written in the location pointed by data file full.
	 */


	/* get all the file names in the root directory and check if they match the pattern */
	DIR dir;
	FILINFO fno;
	uint8_t n = 0;

	uint32_t nlen = strlen(data_file_name);
	uint32_t elen = strlen(data_file_ext);

	(void)f_opendir(&dir, "/");
	do
	{
		(void)f_readdir(&dir, &fno);
		if (fno.fname[0])
		{
			/* we found a file, check the name */
			uint32_t flen = strlen(fno.fname);

			if (!strncmp(data_file_name, fno.fname, nlen) && !strncmp(data_file_ext, fno.fname + flen - elen, elen))  /* check name and extension matches */
			{
				char num[8];  /* allow up to 8 digits */
				strncpy(num, fno.fname + nlen, flen - nlen - elen);  /* extract numeric substring (this assumes that the substring is numeric which isn't guaranteed in general but who cares it'll work) */
				num[flen - nlen - elen] = '\0';  /* add terminator */
				if (atoi(num) > n)
				{
					n = atoi(num);  /* keep track of the largest trial number found */
				}
			}
		}
	} while (fno.fname[0]);
	(void)f_closedir(&dir);

	/* now that we know how many data files are already in the directory, open the new file with correctly incremented file name */
	strncpy(data_file_full, data_file_name, nlen);
	uint32_t ilen = snprintf(NULL, 0, "%d", ++n);  /* figure out the length of the trial number string */
	snprintf(data_file_full + nlen, ilen + 1, "%d", n);  /* write the trial number */
	strncpy(data_file_full + nlen + ilen, data_file_ext, elen);  /* add the extension */
	data_file_full[nlen + ilen + elen] = '\0';  /* add the null terminator */

	*recording_number = n;
	logger->fresult = f_open(&(logger->fil), data_file_full, FA_CREATE_ALWAYS|FA_WRITE);  /* open the file for writing */
}

void SDLogger_Update(SDLogger* logger)
{
	/* write data to the SD card if it is time to do so */
	if (logger->ready_to_write)
	{
		logger->fresult = f_write(&(logger->fil), logger->write_ptr, logger->data_buffer_len / 2, &(logger->write_count));

		/* reset the flag */
		logger->ready_to_write = 0;
	}
}

void SDLogger_StopRecording(SDLogger* logger)
{
	if (logger->ready_to_write)
	{
		/* if we are still ready to write, then there is a complete half of the buffer */
		SDLogger_Update(logger);
	}
	else
	{
		/* write whatever data is remaining in the buffer */
		uint8_t* data_ptr;
		uint32_t num_bytes;

		if (logger->data_buffer_index < logger->data_buffer_len / 2)
		{
			/* the remaining data is in the first half */
			data_ptr = &(logger->data_buffer[0]);
			num_bytes = logger->data_buffer_index;
		}
		else
		{
			/* the remaining data is in the second half */
			data_ptr = &(logger->data_buffer[logger->data_buffer_len / 2]);
			num_bytes = logger->data_buffer_index - logger->data_buffer_len / 2;
		}

		logger->fresult = f_write(&(logger->fil), data_ptr, num_bytes, &(logger->write_count));

	}

	/* close the file */
	logger->fresult = f_close(&(logger->fil));
}
