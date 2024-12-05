/*
 * setting.h
 *
 *  Created on: Apr 24, 2024
 *      Author: johnt
 */

#ifndef INC_SETTING_H_
#define INC_SETTING_H_

#define CHAR_BUF_LEN 128
#define SETTING_DELIMITER '='
#define SETTING_ID_MAX_LEN 64
#define MAX_ALLOWED_SETTING_VALUES 32

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "fatfs.h"



typedef struct
{
	char id[SETTING_ID_MAX_LEN];
	int32_t value;
	int32_t allowed_values[MAX_ALLOWED_SETTING_VALUES];
	uint32_t allowed_values_count;
} Setting;



uint8_t Setting_Parse(Setting* setting, char* file_name)
{
	/* open the settings file and check it for the desired setting (might be more efficient ways of doing this) */
	/* returns true if setting is successfully parsed else false */
	FIL fil;
	if (f_open(&fil, file_name, FA_READ) != FR_OK) {return 0;}

	uint8_t setting_parsed = 0;
	char buf[CHAR_BUF_LEN];
	while (!f_eof(&fil))
	{
		/* read the next line (until \n reached) */
		f_gets(buf, CHAR_BUF_LEN, &fil);

		/* check if the line starts with the setting identifier */
		if (strncmp(setting->id, buf, strlen(setting->id)) == 0)
		{
			/* the setting has been found so get the string after the equal sign */
			char* sep = strchr(buf, SETTING_DELIMITER);
			if (sep != NULL)
			{
				/* sep gives a string starting with the equal sign so increment the pointer when parsing the number */
				/* atoi will ignore any trailing non-number characters */
				/* atoi will return zero for an invalid string so we won't be able to distinguish between a setting with 0 value and a formatting error */
				int32_t value = atoi(sep + 1);

				if (setting->allowed_values_count > 0)
				{
					/* check that this value is allowed for the setting */
					uint8_t value_allowed = 0;
					for (uint32_t i = 0; i < setting->allowed_values_count; i++)
					{
						if (value == setting->allowed_values[i])
						{
							value_allowed = 1;
							break;
						}
					}

					if (value_allowed)
					{
						setting->value = value;
						setting_parsed = 1;
						break;
					}
				}
				else
				{
					/* any value is allowed */
					setting->value = value;
					setting_parsed = 1;
					break;
				}
			}
		}
	}

	f_close(&fil);

	return setting_parsed;
}

uint8_t Setting_Write(Setting* setting, char* file_name, char* comment , uint8_t newline)
{
	/* open the file and append a line for this setting, with preceding or trailing comments */
	/* returns true if setting is successfully written else false */
	FIL fil;
	if (f_open(&fil, file_name, FA_OPEN_APPEND | FA_WRITE) != FR_OK) {return 0;}

	char buf[CHAR_BUF_LEN];

	/* write a comment in the previous line */
	if (comment[0] != '\0')
	{
		snprintf(buf, CHAR_BUF_LEN, "# %s\n", comment);
		f_puts(buf, &fil);
	}

	/* write the setting line */
	snprintf(buf, CHAR_BUF_LEN, "%s = %ld\n", setting->id, setting->value);
	f_puts(buf, &fil);

	/* create an extra newline if specified */
	if (newline) {f_puts("\n", &fil);}

	/* close the file */
	f_close(&fil);

	return 1;
}


Setting* Setting_GetById(Setting* setting_array, uint32_t array_size, char* id)
{
	Setting* setting_ptr = NULL;

	for (uint32_t i = 0; i < array_size; i++)
	{
		if (strcmp(setting_array[i].id, id) == 0)
		{
			setting_ptr = &setting_array[i];
			break;
		}
	}

	return setting_ptr;
}

uint8_t Setting_ParseArray(Setting* setting_array, uint32_t array_size, char* file_name)
{
	/* try to parse each setting in the array from the file */
	/* returns true if all of the settings in the array are successfully parsed */
	uint8_t settings_parsed = 1;

	for (uint32_t i = 0; i < array_size; i++)
	{
		if (!Setting_Parse(&setting_array[i], file_name))
		{
			/* note the error, but continue parsing what we can */
			settings_parsed = 0;
		}
	}

	return settings_parsed;
}

uint8_t Setting_WriteArray(Setting* setting_array, uint32_t array_size, char* file_name)
{
	/* rewrite the settings file from the values in the settings array */
	FIL fil;
	if (f_open(&fil, file_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {return 0;}  /* overwrite (and clear) the existing settings file */
	f_close(&fil);

	for (uint32_t i = 0; i < array_size; i++)
	{
		if (!Setting_Write(&setting_array[i], file_name, "", 0))
		{
			/* failed to write the settings file */
			return 0;
		}
	}

	return 1;
}

#endif /* INC_SETTING_H_ */
