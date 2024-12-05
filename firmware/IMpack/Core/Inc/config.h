/*
 * configuration parameters
 *
 *  Created on: Apr 5, 2024
 *      Author: johnt
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_



/*
 * SETTINGS ID
 */


#define SETTING_LSM6DSx_ACCEL_EN_ID     	"LSM6DSx_accel_enabled"
#define SETTING_LSM6DSx_ACCEL_ODR_ID    	"LSM6DSx_accel_odr_hz"
#define SETTING_LSM6DSx_ACCEL_RANGE_ID  	"LSM6DSx_accel_range_g"
#define SETTING_LSM6DSx_ACCEL_LPF_ID		"LSM6DSx_accel_lpf"
#define SETTING_LSM6DSx_ACCEL_OFSX_ID		"LSM6DSx_accel_offset_x_mg"
#define SETTING_LSM6DSx_ACCEL_OFSY_ID		"LSM6DSx_accel_offset_y_mg"
#define SETTING_LSM6DSx_ACCEL_OFSZ_ID		"LSM6DSx_accel_offset_z_mg"


#define SETTING_LSM6DSx_GYRO_EN_ID 			"LSM6DSx_gyro_enabled"
#define SETTING_LSM6DSx_GYRO_ODR_ID 		"LSM6DSx_gyro_odr_hz"
#define SETTING_LSM6DSx_GYRO_RANGE_ID 		"LSM6DSx_gyro_range_dps"
#define SETTING_LSM6DSx_GYRO_LPF_ID			"LSM6DSx_gyro_lpf"


#define SETTING_IIS3DWB_ACCEL_EN_ID 		"IIS3DWB_accel_enabled"
#define SETTING_IIS3DWB_ACCEL_RANGE_ID 		"IIS3DWB_accel_range_g"
#define SETTING_IIS3DWB_ACCEL_LPF_ID		"IIS3DWB_accel_lpf"
#define SETTING_IIS3DWB_ACCEL_OFSX_ID		"IIS3DWB_accel_offset_x_mg"
#define SETTING_IIS3DWB_ACCEL_OFSY_ID		"IIS3DWB_accel_offset_y_mg"
#define SETTING_IIS3DWB_ACCEL_OFSZ_ID		"IIS3DWB_accel_offset_z_mg"


#define SETTING_ADXL37x_ACCEL_EN_ID 		"ADXL37x_accel_enabled"
#define SETTING_ADXL37x_ACCEL_ODR_ID 		"ADXL37x_accel_odr_hz"
#define SETTING_ADXL37x_ACCEL_LPF_ID		"ADXL37x_accel_lpf"
#define SETTING_ADXL37x_ACCEL_OFSX_ID		"ADXL37x_accel_offset_x_mg"
#define SETTING_ADXL37x_ACCEL_OFSY_ID		"ADXL37x_accel_offset_y_mg"
#define SETTING_ADXL37x_ACCEL_OFSZ_ID		"ADXL37x_accel_offset_z_mg"


#define SETTING_DELAY_BEFORE_ARMED_ID 	    "delay_before_armed_ms"
#define SETTING_RECORDING_LENGTH_ID 		"recording_length_ms"
#define SETTING_FORMAT_DATA_EN_ID 			"data_formatting_enabled"
#define SETTING_ACCEL_TRIGGER_EN_ID			"accel_trigger_enabled"
#define SETTING_ACCEL_TRIGGER_ANY_AXIS_ID	"accel_trigger_on_any_axis"
#define SETTING_ACCEL_TRIGGER_AXIS_ID		"accel_trigger_axis"
#define SETTING_ACCEL_TRIGGER_LEVEL_ID		"accel_trigger_level_mg"
#define SETTING_ACCEL_TRIGGER_EDGE_ID		"accel_trigger_rising_edge"




/*
 * SETTINGS FILE
 */

#define SETTINGS_FILE "settings.txt"

/*
 * DATALOGGING
 */

#define CD_LOGGER_DATA_BUFFER_LEN 	8192  /* number of data points to store at a time */
#define DATA_FILE_NAME      		"DATA"
#define DATA_FILE_EXT				".DAT"
#define LSM6DSx_ACCEL_FILE  		"LSM_ac%d.csv"
#define LSM6DSx_GYRO_FILE			"LSM_gy%d.csv"
#define IIS3DWB_FILE				"IIS_ac%d.csv"
#define ADXL37x_FILE				"ADX_ac%d.csv"

/*
 * BUTTON
 */

#define BUTTON_DEBOUNCE_TIME_MICROS 500000

/*
 * INDICATOR LED
 */

#define IDLE_BLINK_SEQUENCE      {1000000, 100000}
#define STAGING_BLINK_SEQUENCE   {500000, 100000, 100000, 100000}
#define ARMED_BLINK_SEQUENCE     {500000, 100000, 100000, 100000, 100000, 100000}
#define RECORDING_BLINK_SEQUENCE {100000, 100000}
#define SAVING_BLINK_SEQUENCE    {1000000, 250000, 250000, 250000, 250000, 250000}
#define ERROR_BLINK_SEQUENCE	 {500000, 100000, 100000, 100000, 100000, 100000, 100000, 100000}

#define SUCCESS_BURST_SEQUENCE   {1000000, 100000, 100000, 100000}
#define ERROR_BURST_SEQUENCE     {1000000, 100000, 100000, 100000, 100000, 100000, 100000, 100000}


#endif /* INC_CONFIG_H_ */
