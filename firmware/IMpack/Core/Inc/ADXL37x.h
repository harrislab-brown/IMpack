/*
 * ADXL37x.h
 *
 *  Created on: Apr 4, 2024
 *      Author: johnantolik
 */

#ifndef INC_ADXL37x_H_
#define INC_ADXL37x_H_

#include <stdint.h>

/* device defines */
#define ADXL37x_DEVID_PRODUCT 		0xFA

#define ADXL37x_RESOLUTION 			12  /* bit depth of sensor */
#define ADXL37x_RANGE 				400  /* +/- 400 g sensor range */
#define ADXL37x_OFFSET_WEIGHT 		1.46484375f /* g per bit of user offset */

/* device register addresses (p. 31) */
#define ADXL37x_REG_PARTID 			0x02
#define ADXL37x_REG_HPF       		0x38  /* high pass filter setting (p. 49) */
#define ADXL37x_REG_INT1_MAP  		0x3B  /* p. 51 */
#define ADXL37x_REG_TIMING    		0x3D  /* p. 52 */
#define ADXL37x_REG_MEASURE   		0x3E  /* p. 53 */
#define ADXL37x_REG_POWER_CTL 		0x3F  /* p. 54 */
#define ADXL37x_REG_XDATA_H 		0x08  /* accelerometer data registers (p. 36) */
#define ADXL37x_REG_XDATA_L 		0x09
#define ADXL37x_REG_YDATA_H 		0x0A
#define ADXL37x_REG_YDATA_L 		0x0B
#define ADXL37x_REG_ZDATA_H 		0x0C
#define ADXL37x_REG_ZDATA_L 		0x0D
#define ADXL37x_REG_OFFSET_X 		0x20  /* user offsets */
#define ADXL37x_REG_OFFSET_Y 		0x21
#define ADXL37x_REG_OFFSET_Z 		0x22

/* register values */
#define ADXL37x_MODE_DISABLE 		0b00000100  /* POWER_CTL register, LPF enabled */
#define ADXL37x_MODE_ENABLE  		0b00000111
#define ADXL37x_ODR_320HZ   		0b00000000  /* TIMING register */
#define ADXL37x_ODR_640HZ   		0b00100000
#define ADXL37x_ODR_1280HZ  		0b01000000
#define ADXL37x_ODR_2560HZ 			0b01100000
#define ADXL37x_ODR_5120HZ 			0b10000000

/* sensor configuration */
#define ADXL37x_CONFIGURATION_REG  {ADXL37x_REG_POWER_CTL, ADXL37x_REG_HPF, ADXL37x_REG_MEASURE, ADXL37x_REG_INT1_MAP, ADXL37x_REG_TIMING, ADXL37x_REG_OFFSET_X, ADXL37x_REG_OFFSET_Y, ADXL37x_REG_OFFSET_Z}
#define ADXL37x_CONFIGURATION_DATA {ADXL37x_MODE_DISABLE, 0x03, 0x0C, 0x01, 0x00, 0x00, 0x00, 0x00}

/*
 * Configuration sequence:
 * power down the sensor, disable the activity detect LPF and HPF, set the mode to standby (p. 54)
 * select lowest HPF corner frequency (p. 49)
 * set to low noise mode and maximum bandwidth (p. 53)
 * set the interrupt pin function to interrupt when data ready (p. 51)
 * set the default sampling rate
 */

static uint8_t ADXL37x_config_reg[] = ADXL37x_CONFIGURATION_REG;
static uint8_t ADXL37x_config_data[] = ADXL37x_CONFIGURATION_DATA;
static uint8_t ADXL37x_config_size = sizeof(ADXL37x_config_reg) / sizeof(ADXL37x_config_reg[0]);

void ADXL37x_GetConfiguration(uint32_t lpf, uint32_t odr, int32_t ofsx_mg, int32_t ofsy_mg, int32_t ofsz_mg,
		uint8_t** config_reg, uint8_t** config_data, uint8_t* config_size)
{

	ADXL37x_config_data[5] = (0b00001111) & (int8_t)((float)ofsx_mg * 0.001f / ADXL37x_OFFSET_WEIGHT);  /* x offset, 4 bits signed, approximately 1.5 g/LSB */
	ADXL37x_config_data[6] = (0b00001111) & (int8_t)((float)ofsy_mg * 0.001f / ADXL37x_OFFSET_WEIGHT);  /* y offset */
	ADXL37x_config_data[7] = (0b00001111) & (int8_t)((float)ofsz_mg * 0.001f / ADXL37x_OFFSET_WEIGHT);  /* z offset */

	/* set the ODR */
	uint8_t odr_data;
	switch(odr)
	{
	case 320:
		odr_data = ADXL37x_ODR_320HZ;
		break;
	case 640:
		odr_data = ADXL37x_ODR_640HZ;
		break;
	case 1280:
		odr_data = ADXL37x_ODR_1280HZ;
		break;
	case 2560:
		odr_data = ADXL37x_ODR_2560HZ;
		break;
	case 5120:
		odr_data = ADXL37x_ODR_5120HZ;
		break;
	default:
		odr_data = ADXL37x_ODR_320HZ;
		break;
	}
	ADXL37x_config_data[4] = odr_data;


	/* set the LPF bandwidth */
	uint8_t lpf_data;
	switch(lpf)
	{
	case 2:
		lpf_data = 4;
		break;
	case 4:
		lpf_data = 3;
		break;
	case 8:
		lpf_data = 2;
		break;
	case 16:
		lpf_data = 1;
		break;
	case 32:
		lpf_data = 0;
		break;
	default:
		lpf_data = 4;
		break;
	}
	ADXL37x_config_data[2] = (lpf_data | 0b00001000);  /* also set to low noise operation mode */


	*config_reg = ADXL37x_config_reg;
	*config_data = ADXL37x_config_data;
	*config_size = ADXL37x_config_size;
}

uint8_t ADXL37x_ConvertWriteRegister(uint8_t reg)
{
	return (reg << 1);
}

uint8_t ADXL37x_ConvertReadRegister(uint8_t reg)
{
	return ((reg << 1) | 0x01);
}

void ADXL37x_ProcessData(uint8_t* raw_data, float units_per_bit, float* data_x, float* data_y, float* data_z)
{
	int16_t raw_data_x = (raw_data[0] << 8) | (raw_data[1]);
	int16_t raw_data_y = (raw_data[2] << 8) | (raw_data[3]);
	int16_t raw_data_z = (raw_data[4] << 8) | (raw_data[5]);

	raw_data_x = raw_data_x >> 4;
	raw_data_y = raw_data_y >> 4;
	raw_data_z = raw_data_z >> 4;

	*data_x = units_per_bit * (float)raw_data_x;
	*data_y = units_per_bit * (float)raw_data_y;
	*data_z = units_per_bit * (float)raw_data_z;
}

#endif /* INC_ADXL37X_H_ */
