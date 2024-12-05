/*
 * main application
 *
 *  Created on: Apr 5, 2024
 *      Author: johnt
 */

#include <LSM6DSx.h>
#include "app.h"
#include "config.h"
#include "main.h"
#include "IIS3DWB.h"
#include "ADXL37x.h"
#include "logger.h"
#include "button.h"
#include "led.h"
#include "sensor.h"
#include "setting.h"
#include <stdio.h>
#include <math.h>

#define NUMEL(arr) (sizeof(arr) / sizeof(arr[0]))

/* user settings */
Setting settings_array[] =
{
		{SETTING_LSM6DSx_ACCEL_EN_ID, 1, {0, 1}, 2},
		{SETTING_LSM6DSx_ACCEL_ODR_ID, 6660, {13, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660}, 10},
		{SETTING_LSM6DSx_ACCEL_RANGE_ID, 32, {4, 8, 16, 32}, 4},
		{SETTING_LSM6DSx_ACCEL_LPF_ID, 2, {2, 4, 10, 20, 45, 100, 200, 400, 800}, 9},
		{SETTING_LSM6DSx_ACCEL_OFSX_ID, 0, {}, 0},
		{SETTING_LSM6DSx_ACCEL_OFSY_ID, 0, {}, 0},
		{SETTING_LSM6DSx_ACCEL_OFSZ_ID, 0, {}, 0},

		{SETTING_LSM6DSx_GYRO_EN_ID, 1, {0, 1}, 2},
		{SETTING_LSM6DSx_GYRO_ODR_ID, 6660, {13, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660}, 10},
		{SETTING_LSM6DSx_GYRO_RANGE_ID, 2000, {125, 250, 500, 1000, 2000}, 5},
		{SETTING_LSM6DSx_GYRO_LPF_ID, 3, {0, 1, 2, 3, 4, 5, 6, 7}, 8},

		{SETTING_IIS3DWB_ACCEL_EN_ID, 1, {0, 1}, 2},
		{SETTING_IIS3DWB_ACCEL_RANGE_ID, 16, {2, 4, 8, 16}, 4},
		{SETTING_IIS3DWB_ACCEL_LPF_ID, 4, {4, 10, 20, 45, 100, 200, 400, 800}, 8},
		{SETTING_IIS3DWB_ACCEL_OFSX_ID, 0, {}, 0},
		{SETTING_IIS3DWB_ACCEL_OFSY_ID, 0, {}, 0},
		{SETTING_IIS3DWB_ACCEL_OFSZ_ID, 0, {}, 0},

		{SETTING_ADXL37x_ACCEL_EN_ID, 1, {0, 1}, 2},
		{SETTING_ADXL37x_ACCEL_ODR_ID, 5120, {320, 640, 1280, 2560, 5120}, 5},
		{SETTING_ADXL37x_ACCEL_LPF_ID, 2, {2, 4, 8, 16, 32}, 5},
		{SETTING_ADXL37x_ACCEL_OFSX_ID, 0, {}, 0},
		{SETTING_ADXL37x_ACCEL_OFSY_ID, 0, {}, 0},
		{SETTING_ADXL37x_ACCEL_OFSZ_ID, 0, {}, 0},

		{SETTING_DELAY_BEFORE_ARMED_ID, 0, {}, 0},
		{SETTING_RECORDING_LENGTH_ID, 5000, {}, 0},
		{SETTING_FORMAT_DATA_EN_ID, 1, {0, 1}, 2},
		{SETTING_ACCEL_TRIGGER_EN_ID, 0, {0, 1}, 2},
		{SETTING_ACCEL_TRIGGER_ANY_AXIS_ID, 0, {0, 1}, 2},
		{SETTING_ACCEL_TRIGGER_AXIS_ID, 2, {0, 1, 2}, 3},
		{SETTING_ACCEL_TRIGGER_LEVEL_ID, 500, {}, 0},
		{SETTING_ACCEL_TRIGGER_EDGE_ID, 0, {0, 1}, 2}
};

/* sensor objects: LSM6DSx accelerometer, LSM6DSx gyroscope, IIS3DWB accelerometer, ADXL37x accelerometer */
SPISensor sensor_array[] =
{
		{NULL, SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, LSM6DSx_INT1_Pin, LSM6DSx_ConvertWriteRegister, LSM6DSx_ConvertReadRegister, 0x00, LSM6DSx_ProcessData},
		{NULL, SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, LSM6DSx_INT2_Pin, LSM6DSx_ConvertWriteRegister, LSM6DSx_ConvertReadRegister, 0x00, LSM6DSx_ProcessData},
		{NULL, IIS3DWB_NSS_GPIO_Port, IIS3DWB_NSS_Pin, IIS3DWB_INT1_Pin, IIS3DWB_ConvertWriteRegister, IIS3DWB_ConvertReadRegister, 0x00, IIS3DWB_ProcessData},
		{NULL, ADXL37x_NSS_GPIO_Port, ADXL37x_NSS_Pin, ADXL37x_INT1_Pin, ADXL37x_ConvertWriteRegister, ADXL37x_ConvertReadRegister, 0x00, ADXL37x_ProcessData}
};

/* pointer to microsecond counter */
volatile uint32_t* time_micros_ptr;

/* data logger */
SDLogger logger;
FATFS fs;
FIL raw_data_file;
char raw_data_file_name[16];
FIL output_file_array[4];  /* output files: LSM6DSx accelerometer, LSM6DSx gyroscope, IIS3DWB accelerometer, ADXL37x accelerometer */
uint8_t output_file_is_open[] = {0, 0, 0, 0};
FRESULT fresult;
UINT write_count;

/* de-bounced button */
ButtonDebounced button;

/* indicator LED */
LEDSequence led;
const uint32_t idle_blink_sequence[] = IDLE_BLINK_SEQUENCE;
const uint32_t staging_blink_sequence[] = STAGING_BLINK_SEQUENCE;
const uint32_t armed_blink_sequence[] = ARMED_BLINK_SEQUENCE;
const uint32_t recording_blink_sequence[] = RECORDING_BLINK_SEQUENCE;
const uint32_t saving_blink_sequence[] = SAVING_BLINK_SEQUENCE;
const uint32_t error_blink_sequence[] = ERROR_BLINK_SEQUENCE;
const uint32_t success_burst_sequence[] = SUCCESS_BURST_SEQUENCE;
const uint32_t error_burst_sequence[] = ERROR_BURST_SEQUENCE;

/* storage type for gathered data */
typedef struct
{
	uint32_t time_micros;  /* time stamp in microseconds */
	uint8_t data[6];  /* 3 components of the accelerometer/gyroscope data */
	uint16_t data_type;  /* indicates which sensor the data came from */
} DataPoint;

/* data buffer */
volatile DataPoint data_buffer[CD_LOGGER_DATA_BUFFER_LEN];
volatile uint32_t data_pending_index = 0;  /* increments as each sensor data ready pin triggers */
volatile uint32_t data_read_index = 0;  /* increments once the data at this index has been read from the sensor */

/* IMU state control */
typedef enum
{
	IDLE,
	IDLE_ENTRY,
	STAGING,
	STAGING_ENTRY,
	ARMED,
	ARMED_ENTRY,
	RECORDING,
	RECORDING_ENTRY,
	SAVING,
	SAVING_ENTRY,
	IMU_ERROR,
	IMU_ERROR_ENTRY
} IMUState;
IMUState state = IDLE_ENTRY;

/* recording variables */
uint32_t time_staging, time_recording_started;
uint32_t delay_before_armed, max_recording_length;
uint16_t recording_number;
uint32_t data_formatting_enabled;
uint8_t sensor_enabled[] = {0, 0, 0, 0};
float sensor_units_per_bit[4];

/* triggering based on acceleration */
float accel_threshold_g;
uint32_t trigger_enabled = 0;
uint32_t trigger_on_axis[] = {0, 0, 0};  /* which axes to consider for triggering a recording */
uint32_t trigger_on_rising_edge;



void App_Setup(SD_HandleTypeDef* hsd, SPI_HandleTypeDef* hspi_LSM6DSx, SPI_HandleTypeDef* hspi_IIS3DWB, SPI_HandleTypeDef* hspi_ADXL37x, volatile uint32_t* micros_timer)
{
	/* disable interrupts */
	App_DisableAccelerometerInterrupts();

	/* pointer to time keeping variable */
	time_micros_ptr = micros_timer;

	/* store the SPI handles and data register addresses in the sensor objects */
	sensor_array[0].spi = hspi_LSM6DSx;
	sensor_array[0].data_reg = LSM6DSx_ConvertReadRegister(LSM6DSx_REG_OUTX_L_XL);
	sensor_array[1].spi = hspi_LSM6DSx;
	sensor_array[1].data_reg = LSM6DSx_ConvertReadRegister(LSM6DSx_REG_OUTX_L_G);
	sensor_array[2].spi = hspi_IIS3DWB;
	sensor_array[2].data_reg = IIS3DWB_ConvertReadRegister(IIS3DWB_REG_OUTX_L_XL);
	sensor_array[3].spi = hspi_ADXL37x;
	sensor_array[3].data_reg = ADXL37x_ConvertReadRegister(ADXL37x_REG_XDATA_H);

	/* test sensor communication */
	if (SPISensor_TestCommunication(&sensor_array[0], LSM6DSx_REG_WHO_AM_I, LSM6DSx_DEVICE_ID)) {state = IMU_ERROR_ENTRY;}
	if (SPISensor_TestCommunication(&sensor_array[2], IIS3DWB_REG_WHO_AM_I, IIS3DWB_DEVICE_ID)) {state = IMU_ERROR_ENTRY;}
	if (SPISensor_TestCommunication(&sensor_array[3], ADXL37x_REG_PARTID, ADXL37x_DEVID_PRODUCT)) {state = IMU_ERROR_ENTRY;}

	/* initialize the SDIO peripheral in 4 bit mode (bug: cube always generates code for 1 bit regardless of setting) */
	if (HAL_SD_Init(hsd) != HAL_OK) {state = IMU_ERROR_ENTRY;}
	if (HAL_SD_ConfigWideBusOperation(hsd, SDIO_BUS_WIDE_4B) != HAL_OK) {state = IMU_ERROR_ENTRY;}

	/* initialize the data logger */
	SDLogger_Initialize(&logger, (uint8_t*)data_buffer, sizeof(data_buffer), sizeof(DataPoint));

	/* initialize the user button */
	ButtonDebounced_Init(&button, BUTTON_GPIO_Port, BUTTON_Pin, time_micros_ptr, BUTTON_DEBOUNCE_TIME_MICROS);

	/* initialize the indicator LED */
	LEDSequence_Init(&led, LED_STATUS_GPIO_Port, LED_STATUS_Pin, time_micros_ptr);

	/* try to parse the user settings file */
	if (state == IDLE_ENTRY)
	{
		fresult = f_mount(&fs, "/", 1);
		if (Setting_ParseArray(settings_array, NUMEL(settings_array), SETTINGS_FILE))
		{
			/* successfully parsed all settings */
			LEDSequence_SetBurstSequence(&led, success_burst_sequence, NUMEL(success_burst_sequence));
		}
		else
		{
			/* error during parsing so alert the user, use default values for the settings that could not be parsed */
			LEDSequence_SetBurstSequence(&led, error_burst_sequence, NUMEL(error_burst_sequence));
		}

		/* rewrite the settings file so it will be correct for next time */
		if (!Setting_WriteArray(settings_array, NUMEL(settings_array), SETTINGS_FILE)) {state = IMU_ERROR_ENTRY;}
		fresult = f_mount(NULL, "/", 1);
	}

	/* configure the sensors */
	uint8_t* config_reg;
	uint8_t* config_data;
	uint8_t config_size;
	LSM6DSx_GetConfiguration(Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_ACCEL_LPF_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_GYRO_LPF_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_ACCEL_OFSX_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_ACCEL_OFSY_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_ACCEL_OFSZ_ID)->value,
							 &config_reg, &config_data, &config_size);
	if (SPISensor_WriteMultiple(&sensor_array[0], config_reg, config_data, config_size)) {state = IMU_ERROR_ENTRY;}

	IIS3DWB_GetConfiguration(Setting_GetById(settings_array, NUMEL(settings_array), SETTING_IIS3DWB_ACCEL_LPF_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_IIS3DWB_ACCEL_OFSX_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_IIS3DWB_ACCEL_OFSY_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_IIS3DWB_ACCEL_OFSZ_ID)->value,
							 &config_reg, &config_data, &config_size);
	if (SPISensor_WriteMultiple(&sensor_array[2], config_reg, config_data, config_size)) {state = IMU_ERROR_ENTRY;}

	ADXL37x_GetConfiguration(Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ADXL37x_ACCEL_LPF_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ADXL37x_ACCEL_ODR_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ADXL37x_ACCEL_OFSX_ID)->value,
			 	 	 	 	 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ADXL37x_ACCEL_OFSY_ID)->value,
							 Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ADXL37x_ACCEL_OFSZ_ID)->value,
							 &config_reg, &config_data, &config_size);
	if (SPISensor_WriteMultiple(&sensor_array[3], config_reg, config_data, config_size)) {state = IMU_ERROR_ENTRY;}

	/* configure the sensor enable registers */
	LSM6DSx_GetAccelEnable(Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_ACCEL_LPF_ID)->value,
						   Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_ACCEL_ODR_ID)->value,
						   Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_ACCEL_RANGE_ID)->value,
						   &(sensor_array[0].enable_reg), &(sensor_array[0].enable_data));
	sensor_array[0].disable_reg = LSM6DSx_REG_CTRL1_XL;
	sensor_array[0].disable_data = LSM6DSx_ACCEL_ODR_DISABLE;

	LSM6DSx_GetGyroEnable(Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_GYRO_ODR_ID)->value,
					      Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_GYRO_RANGE_ID)->value,
						  &(sensor_array[1].enable_reg), &(sensor_array[1].enable_data));
	sensor_array[1].disable_reg = LSM6DSx_REG_CTRL2_G;
	sensor_array[1].disable_data = LSM6DSx_GYRO_ODR_DISABLE;

	IIS3DWB_GetEnable(Setting_GetById(settings_array, NUMEL(settings_array), SETTING_IIS3DWB_ACCEL_RANGE_ID)->value,
					  &(sensor_array[2].enable_reg), &(sensor_array[2].enable_data));
	sensor_array[2].disable_reg = IIS3DWB_REG_CTRL1_XL;
	sensor_array[2].disable_data = IIS3DWB_ODR_DISABLE;

	sensor_array[3].enable_reg = ADXL37x_REG_POWER_CTL;
	sensor_array[3].enable_data = ADXL37x_MODE_ENABLE;
	sensor_array[3].disable_reg = ADXL37x_REG_POWER_CTL;
	sensor_array[3].disable_data = ADXL37x_MODE_DISABLE;


	/* configure the recording control variables */
	sensor_enabled[0] = Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_ACCEL_EN_ID)->value;
	sensor_enabled[1] = Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_GYRO_EN_ID)->value;
	sensor_enabled[2] = Setting_GetById(settings_array, NUMEL(settings_array), SETTING_IIS3DWB_ACCEL_EN_ID)->value;
	sensor_enabled[3] = Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ADXL37x_ACCEL_EN_ID)->value;
	sensor_units_per_bit[0] = (float)Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_ACCEL_RANGE_ID)->value / (float)(1 << (LSM6DSx_RESOLUTION - 1));
	sensor_units_per_bit[1] = (float)Setting_GetById(settings_array, NUMEL(settings_array), SETTING_LSM6DSx_GYRO_RANGE_ID)->value / (float)(1 << (LSM6DSx_RESOLUTION - 1));
	sensor_units_per_bit[2] = (float)Setting_GetById(settings_array, NUMEL(settings_array), SETTING_IIS3DWB_ACCEL_RANGE_ID)->value / (float)(1 << (IIS3DWB_RESOLUTION - 1));
	sensor_units_per_bit[3] = (float)ADXL37x_RANGE / (float)(1 << (ADXL37x_RESOLUTION - 1));


	delay_before_armed = 1000 * Setting_GetById(settings_array, NUMEL(settings_array), SETTING_DELAY_BEFORE_ARMED_ID)->value;
	max_recording_length = 1000 * Setting_GetById(settings_array, NUMEL(settings_array), SETTING_RECORDING_LENGTH_ID)->value;
	data_formatting_enabled = Setting_GetById(settings_array, NUMEL(settings_array), SETTING_FORMAT_DATA_EN_ID)->value;


	accel_threshold_g = 0.001f * (float)Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ACCEL_TRIGGER_LEVEL_ID)->value;
	if (Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ACCEL_TRIGGER_EN_ID)->value)
	{
		trigger_enabled = 1;
		if (Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ACCEL_TRIGGER_ANY_AXIS_ID)->value)
		{
			trigger_on_axis[0] = 1;
			trigger_on_axis[1] = 1;
			trigger_on_axis[2] = 1;
		}
		else
		{
			trigger_on_axis[Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ACCEL_TRIGGER_AXIS_ID)->value] = 1;
		}
	}
	trigger_on_rising_edge = Setting_GetById(settings_array, NUMEL(settings_array), SETTING_ACCEL_TRIGGER_EDGE_ID)->value;

}



void App_Loop()
{
	switch (state)
	{
		case IDLE_ENTRY:
		{
			/* set the idle LED sequence */
			LEDSequence_SetBlinkSequence(&led, idle_blink_sequence, NUMEL(idle_blink_sequence));

			/* go to idle state */
			state = IDLE;

			break;
		}

		case IDLE:
		{
			/* check the button */
			if (ButtonDebounced_GetPressed(&button))
			{
				if (delay_before_armed > 0)
				{
					state = STAGING_ENTRY;
				}
				else
				{
					state = ARMED_ENTRY;
				}
			}

			break;
		}

		case STAGING_ENTRY:
		{
			/* set the staging LED sequence */
			LEDSequence_SetBlinkSequence(&led, staging_blink_sequence, NUMEL(staging_blink_sequence));

			/* record the starting delay time */
			time_staging = *time_micros_ptr;
			state = STAGING;

			break;
		}

		case STAGING:
		{
			/* check if time to enter armed state */
			if (*time_micros_ptr - time_staging > delay_before_armed)
			{
				state = ARMED_ENTRY;
			}

			/* press button again to go back to idle state */
			if (ButtonDebounced_GetPressed(&button))
			{
				state = IDLE_ENTRY;
			}

			break;
		}

		case ARMED_ENTRY:
		{
			/* set the armed LED sequence */
			LEDSequence_SetBlinkSequence(&led, armed_blink_sequence, NUMEL(armed_blink_sequence));

			/* get the recording file ready so we can start recording immediately once we see the threshold */
			fresult = f_mount(&fs, "/", 1);
			SDLogger_StartRecording(&logger, DATA_FILE_NAME, DATA_FILE_EXT, raw_data_file_name, &recording_number);


			/* Enable the accelerometers to look for the acceleration threshold but don't record data yet */
			for (uint8_t i = 0; i < 4; i++)
				if (sensor_enabled[i])
					SPISensor_Enable(&sensor_array[i]);

			/* reset the data buffer indices */
			data_pending_index = 0;
			data_read_index = 0;

			/* enable accelerometer interrupts */
			App_EnableAccelerometerInterrupts();

			if (trigger_enabled)
				state = ARMED;
			else
				state = RECORDING_ENTRY;

			break;
		}

		case ARMED:
		{

			/* wait until we see the acceleration threshold from any of the enabled sensors */
			if (data_read_index > 0)
			{
				uint32_t i = data_read_index - 1;  /* this will be the data point we most recently acquired */
				float accel[3];
				uint32_t has_accel_data = 0;
				switch (data_buffer[i].data_type)  /* need to figure out how to convert the raw data to g based on which sensor it came from */
				{
					case LSM6DSx_INT1_Pin:
					{
						has_accel_data = 1;
						sensor_array[0].process_data((uint8_t*)data_buffer[i].data, sensor_units_per_bit[0], &accel[0], &accel[1], &accel[2]);
						break;
					}
					case IIS3DWB_INT1_Pin:
					{
						has_accel_data = 1;
						sensor_array[2].process_data((uint8_t*)data_buffer[i].data, sensor_units_per_bit[2], &accel[0], &accel[1], &accel[2]);
						break;
					}
					case ADXL37x_INT1_Pin:
					{
						has_accel_data = 1;
						sensor_array[3].process_data((uint8_t*)data_buffer[i].data, sensor_units_per_bit[3], &accel[0], &accel[1], &accel[2]);
						break;
					}
				}

				if (has_accel_data)
				{
					for (uint32_t i = 0; i < 3; i++)
					{
						if (trigger_on_rising_edge)
						{
							if (trigger_on_axis[i] && fabs(accel[i]) > accel_threshold_g)
							{
								state = RECORDING_ENTRY;
								break;
							}
						}
						else
						{
							if (trigger_on_axis[i] && fabs(accel[i]) < accel_threshold_g)
							{
								state = RECORDING_ENTRY;
								break;
							}
						}
					}
				}


			}

			break;
		}

		case RECORDING_ENTRY:
		{
			/* set the recording LED sequence */
			LEDSequence_SetBlinkSequence(&led, recording_blink_sequence, NUMEL(recording_blink_sequence));

			/* store the starting time stamp and reset data buffer indices */
			time_recording_started = *time_micros_ptr;
			data_pending_index = 0;
			data_read_index = 0;

			/* go to recording state */
			state = RECORDING;

			break;
		}

		case RECORDING:
		{
			/* update the SD card data logger */
			SDLogger_Update(&logger);

			/* stop the recording if button pressed or max time exceeded */
			if (ButtonDebounced_GetPressed(&button) || *time_micros_ptr - time_recording_started > max_recording_length)
			{
				/* disable accelerometer interrupts */
				App_DisableAccelerometerInterrupts();

				/* put the accelerometer in standby mode */
				for (uint8_t i = 0; i < 4; i++)
				{
					SPISensor_Disable(&sensor_array[i]);
				}

				state = SAVING_ENTRY;
			}

			break;
		}

		case SAVING_ENTRY:
		{
			/* set the saving LED sequence */
			LEDSequence_SetBlinkSequence(&led, saving_blink_sequence, NUMEL(saving_blink_sequence));

			/* write the remaining data in the buffer and close the file */
			SDLogger_StopRecording(&logger);
			fresult = f_mount(NULL, "/", 1);

			if (data_formatting_enabled)
			{
				/* open the binary data file for reading and converting to CSV */
				fresult = f_mount(&fs, "/", 1);
				fresult = f_open(&raw_data_file, raw_data_file_name, FA_READ);

				state = SAVING;
			}
			else
			{
				state = IDLE_ENTRY;
			}

			break;
		}

		case SAVING:
		{
			/* convert the binary data files to CSV */
			if (!f_eof(&raw_data_file))
			{
				/* read the next data from the file */
				DataPoint data_point;
				fresult = f_read(&raw_data_file, &data_point, sizeof(data_point), &write_count);

				/* convert the data to physical units and save to the appropriate file based on which channel it came from */
				float data_x, data_y, data_z;
				char formatted_data[100];
				uint8_t formatted_bytes;
				if (data_point.data_type == LSM6DSx_INT1_Pin)
				{
					/* LSM6DSx accelerometer */
					sensor_array[0].process_data(data_point.data, sensor_units_per_bit[0], &data_x, &data_y, &data_z);

					if (!output_file_is_open[0])  /* open the file and write the header */
					{
						char buf[16];
						snprintf(buf, 16, LSM6DSx_ACCEL_FILE, recording_number);
						fresult = f_open(&output_file_array[0], buf, FA_CREATE_ALWAYS|FA_WRITE);
						f_printf(&output_file_array[0], "Time (us),Accel_x (g),Accel_y (g),Accel_z (g)\n");
						output_file_is_open[0] = 1;
					}

					formatted_bytes = snprintf(formatted_data, 100, "%lu,%f,%f,%f\n", data_point.time_micros, data_x, data_y, data_z);
					fresult = f_write(&output_file_array[0], formatted_data, formatted_bytes, &write_count);
				}
				else if (data_point.data_type == LSM6DSx_INT2_Pin)
				{
					/* LSM6DSx gyroscope */
					sensor_array[1].process_data(data_point.data, sensor_units_per_bit[1], &data_x, &data_y, &data_z);

					if (!output_file_is_open[1])  /* open the file and write the header */
					{
						char buf[16];
						snprintf(buf, 16, LSM6DSx_GYRO_FILE, recording_number);
						fresult = f_open(&output_file_array[1], buf, FA_CREATE_ALWAYS|FA_WRITE);
						f_printf(&output_file_array[1], "Time (us),Rate_x (dps),Rate_y (dps),Rate_z (dps)\n");
						output_file_is_open[1] = 1;
					}

					formatted_bytes = snprintf(formatted_data, 100, "%lu,%f,%f,%f\n", data_point.time_micros, data_x, data_y, data_z);
					fresult = f_write(&output_file_array[1], formatted_data, formatted_bytes, &write_count);
				}
				else if (data_point.data_type == IIS3DWB_INT1_Pin)
				{
					/* IIS3DWB accelerometer */
					sensor_array[2].process_data(data_point.data, sensor_units_per_bit[2], &data_x, &data_y, &data_z);

					if (!output_file_is_open[2])  /* open the file and write the header */
					{
						char buf[16];
						snprintf(buf, 16, IIS3DWB_FILE, recording_number);
						fresult = f_open(&output_file_array[2], buf, FA_CREATE_ALWAYS|FA_WRITE);
						f_printf(&output_file_array[2], "Time (us),Accel_x (g),Accel_y (g),Accel_z (g)\n");
						output_file_is_open[2] = 1;
					}

					formatted_bytes = snprintf(formatted_data, 100, "%lu,%f,%f,%f\n", data_point.time_micros, data_x, data_y, data_z);
					fresult = f_write(&output_file_array[2], formatted_data, formatted_bytes, &write_count);
				}
				else if (data_point.data_type == ADXL37x_INT1_Pin)
				{
					/* ADXL37x accelerometer */
					sensor_array[3].process_data(data_point.data, sensor_units_per_bit[3], &data_x, &data_y, &data_z);

					if (!output_file_is_open[3])  /* open the file and write the header */
					{
						char buf[16];
						snprintf(buf, 16, ADXL37x_FILE, recording_number);
						fresult = f_open(&output_file_array[3], buf, FA_CREATE_ALWAYS|FA_WRITE);
						f_printf(&output_file_array[3], "Time (us),Accel_x (g),Accel_y (g),Accel_z (g)\n");
						output_file_is_open[3] = 1;
					}

					formatted_bytes = snprintf(formatted_data, 100, "%lu,%f,%f,%f\n", data_point.time_micros, data_x, data_y, data_z);
					fresult = f_write(&output_file_array[3], formatted_data, formatted_bytes, &write_count);
				}

			}
			else
			{
				/* we have reached the end of the raw data file */
				for (uint8_t i = 0; i < 4; i++)
				{
					if (output_file_is_open[i])
					{
						fresult = f_close(&output_file_array[i]);
						output_file_is_open[i] = 0;
					}
				}

				fresult = f_close(&raw_data_file);
				fresult = f_mount(NULL, "/", 1);

				state = IDLE_ENTRY;
			}

			break;
		}

		case IMU_ERROR_ENTRY:
		{
			/* set the error LED sequence */
			LEDSequence_SetBlinkSequence(&led, error_blink_sequence, NUMEL(error_blink_sequence));
			state = IMU_ERROR;

			break;
		}

		case IMU_ERROR:
		{
			/* do nothing */
			break;
		}
	}

	/* update the indicator led */
	LEDSequence_Update(&led);
}


/*
 * Interrupt triggered by accelerometer pins
 */
void App_PinInterrupt(uint16_t GPIO_Pin)
{
	/* store the time in the global data buffer */
	data_buffer[data_pending_index].time_micros = *time_micros_ptr - time_recording_started;

	/* store the data type in the global data buffer */
	data_buffer[data_pending_index].data_type = GPIO_Pin;

	/* increment the global data buffer index */
	if (++data_pending_index == CD_LOGGER_DATA_BUFFER_LEN) {data_pending_index = 0;}
}


/*
 * Try to start SPI communications outside of the main loop where SD card can cause significant latency
 */
void App_TimerInterrupt()
{
	/* disable timer interrupts */
	HAL_NVIC_DisableIRQ(TIM3_IRQn);

	/* try to start any pending communications */
	while (data_read_index != data_pending_index)
	{
		/* figure out which sensor has data pending */
		SPISensor* sensor;
		for (uint8_t i = 0; i < 4; i++)
		{
			sensor = &sensor_array[i];
			if (sensor->int_pin == data_buffer[data_read_index].data_type)
			{
				break;
			}
		}

		/* chip select low to start reading data */
		sensor->cs_port->BSRR = (uint32_t)sensor->cs_pin << 16;

		/* transmit the data address byte */
		while (!((sensor->spi->Instance->SR) & (1 << 1))) {};  /* wait for TXE bit to set so we know transmit buffer is empty */
		sensor->spi->Instance->DR = sensor->data_reg;  /* load the address into the data register */
		while (!((sensor->spi->Instance->SR) & (1 << 1))) {};  /* wait for TXE bit to set so we know transmit buffer is empty */
		while ((sensor->spi->Instance->SR) & (1 << 7)) {};  /* wait for the BSY bit to reset */
		uint8_t temp = sensor->spi->Instance->DR;
		(void)temp;
		temp = sensor->spi->Instance->SR;  /* read DR and SR to clear the overrun flag */

		/* receive the 6 bytes of data */
		for (uint8_t i = 0; i < 6; i++)
		{
			while ((sensor->spi->Instance->SR) & (1 << 7)) {};  /* wait for BSY bit to reset */
			sensor->spi->Instance->DR = 0;  /* send dummy data to keep the clock going */
			while (!((sensor->spi->Instance->SR) & (1 << 0))) {};  /* wait for RXNE to be set indicating there is data in the RX buffer */
			data_buffer[data_read_index].data[i] = sensor->spi->Instance->DR;  /* store the data in the buffer */
		}

		/* chip select high */
		sensor->cs_port->BSRR = (uint32_t)sensor->cs_pin;

		/* increment the data read index */
		if (++data_read_index == CD_LOGGER_DATA_BUFFER_LEN) {data_read_index = 0;}

		/* also increment the logger index if we are recording */
		if (state == RECORDING)
			SDLogger_IncrementDataIndex(&logger);

	}

	/* enable timer interrupts */
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}


void App_EnableAccelerometerInterrupts()
{
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


void App_DisableAccelerometerInterrupts()
{
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
	HAL_NVIC_DisableIRQ(EXTI4_IRQn);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}
