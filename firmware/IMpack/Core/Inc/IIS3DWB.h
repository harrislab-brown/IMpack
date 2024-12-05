/*
 * Minimal driver to get linear acceleration from IIS3DWB, SPI with DMA
 */

#ifndef IIS3DWB_SPI_DRIVER_H
#define IIS3DWB_SPI_DRIVER_H

#include <stdint.h>

/* device defines */
#define IIS3DWB_DEVICE_ID 0x7B  /* fixed value of WHO_AM_I register */

#define IIS3DWB_RESOLUTION 16  /* bit depth of sensor */
#define IIS3DWB_OFFSET_WEIGHT 0.0009765625f  /* g per bit of user offset */

/* device register addresses (p.26) */
#define IIS3DWB_REG_WHO_AM_I 		 0x0F
#define IIS3DWB_REG_INT1_CTRL 		 0x0D
#define IIS3DWB_REG_CTRL1_XL  		 0x10
#define IIS3DWB_REG_CTRL4_C  		 0x13
#define IIS3DWB_REG_CTRL6_C 		 0x15
#define IIS3DWB_REG_CTRL7_C 		 0x16
#define IIS3DWB_REG_CTRL8_XL         0x17
#define IIS3DWB_REG_COUNTER_BDR_REG1 0x0B
#define IIS3DWB_REG_STATUS_REG       0x1E
#define IIS3DWB_REG_OUTX_L_XL 0x28  /* accelerometer output registers */
#define IIS3DWB_REG_OUTX_H_XL 0x29
#define IIS3DWB_REG_OUTY_L_XL 0x2A
#define IIS3DWB_REG_OUTY_H_XL 0x2B
#define IIS3DWB_REG_OUTZ_L_XL 0x2C
#define IIS3DWB_REG_OUTZ_H_XL 0x2D
#define IIS3DWB_REG_X_OFS_USR 0x73  /* user offsets */
#define IIS3DWB_REG_Y_OFS_USR 0x74
#define IIS3DWB_REG_Z_OFS_USR 0x75

/* register values */
#define IIS3DWB_ODR_DISABLE 0b00000000  /* CTRL1_XL register */
#define IIS3DWB_ODR_26667HZ 0b10100010  /* also enables the LPF2 path */
#define IIS3DWB_G_RANGE_2  0b00000000  /* CTRL1_XL register */
#define IIS3DWB_G_RANGE_4  0b00001000
#define IIS3DWB_G_RANGE_8  0b00001100
#define IIS3DWB_G_RANGE_16 0b00000100

#define IIS3DWB_LPF_4					0b00000000  /* CTRL8_XL register */
#define IIS3DWB_LPF_10					0b00100000
#define IIS3DWB_LPF_20					0b01000000
#define IIS3DWB_LPF_45					0b01100000
#define IIS3DWB_LPF_100					0b10000000
#define IIS3DWB_LPF_200					0b10100000
#define IIS3DWB_LPF_400					0b11000000
#define IIS3DWB_LPF_800					0b11100000


/* sensor configuration */
#define IIS3DWB_CONFIGURATION_REG  {IIS3DWB_REG_CTRL1_XL, IIS3DWB_REG_CTRL4_C, IIS3DWB_REG_CTRL6_C, IIS3DWB_REG_COUNTER_BDR_REG1, IIS3DWB_REG_INT1_CTRL, IIS3DWB_REG_CTRL8_XL, IIS3DWB_REG_CTRL7_C, IIS3DWB_REG_X_OFS_USR, IIS3DWB_REG_Y_OFS_USR, IIS3DWB_REG_Z_OFS_USR}
#define IIS3DWB_CONFIGURATION_DATA {0x00, 0x04, 0x00, 0x80, 0x01, 0x40, 0x02, 0x00, 0x00, 0x00}

/*
 * Configuration sequence:
 * power down the device (p. 32)
 * disable the I2C interface (p. 34)
 * select three axis sampling mode and weight of user offsets (p. 35)
 * set the data-ready interrupt to pulse mode (p. 30)
 * enable the data-ready interrupt on INT1 pin (p. 31)
 * enable the low pass filter with ODR/20 cutoff (p. 36)
 * enable the user offset correction block (p. 35)
 * user offsets
 */

static uint8_t IIS3DWB_config_reg[] = IIS3DWB_CONFIGURATION_REG;
static uint8_t IIS3DWB_config_data[] = IIS3DWB_CONFIGURATION_DATA;
static uint8_t IIS3DWB_config_size = sizeof(IIS3DWB_config_reg) / sizeof(IIS3DWB_config_reg[0]);


void IIS3DWB_GetConfiguration(uint32_t lpf, int32_t ofsx_mg, int32_t ofsy_mg, int32_t ofsz_mg,
		uint8_t** config_reg, uint8_t** config_data, uint8_t* config_size)
{
	/*
	 * Get register and data arrays for configuring the sensor based on the input parameters for DC offsets
	 */

	/* user DC offsets */
	IIS3DWB_config_data[7] = (int8_t)((float)ofsx_mg * 0.001f / IIS3DWB_OFFSET_WEIGHT);
	IIS3DWB_config_data[8] = (int8_t)((float)ofsy_mg * 0.001f / IIS3DWB_OFFSET_WEIGHT);
	IIS3DWB_config_data[9] = (int8_t)((float)ofsz_mg * 0.001f / IIS3DWB_OFFSET_WEIGHT);

	/* LPF setting */
	uint8_t lpf_data;
	switch(lpf)
	{
	case 4:
		lpf_data = IIS3DWB_LPF_4;
		break;
	case 10:
		lpf_data = IIS3DWB_LPF_10;
		break;
	case 20:
		lpf_data = IIS3DWB_LPF_20;
		break;
	case 45:
		lpf_data = IIS3DWB_LPF_45;
		break;
	case 100:
		lpf_data = IIS3DWB_LPF_100;
		break;
	case 200:
		lpf_data = IIS3DWB_LPF_200;
		break;
	case 400:
		lpf_data = IIS3DWB_LPF_400;
		break;
	case 800:
		lpf_data = IIS3DWB_LPF_800;
		break;
	default:
		lpf_data = IIS3DWB_LPF_20;
		break;
	}
	IIS3DWB_config_data[5] = lpf_data;


	*config_reg = IIS3DWB_config_reg;
	*config_data = IIS3DWB_config_data;
	*config_size = IIS3DWB_config_size;
}

void IIS3DWB_GetEnable(uint32_t range, uint8_t* reg, uint8_t* data)
{
	*reg = IIS3DWB_REG_CTRL1_XL;

	uint8_t range_data;
	switch(range)
	{
	case 2:
		range_data = IIS3DWB_G_RANGE_2;
		break;
	case 4:
		range_data = IIS3DWB_G_RANGE_4;
		break;
	case 8:
		range_data = IIS3DWB_G_RANGE_8;
		break;
	case 16:
		range_data = IIS3DWB_G_RANGE_16;
		break;
	default:
		range_data = IIS3DWB_G_RANGE_2;
		break;
	}

	*data = (IIS3DWB_ODR_26667HZ | range_data);
}

uint8_t IIS3DWB_ConvertWriteRegister(uint8_t reg)
{
	return reg;
}

uint8_t IIS3DWB_ConvertReadRegister(uint8_t reg)
{
	return (reg | 0x80);
}

void IIS3DWB_ProcessData(uint8_t* raw_data, float units_per_bit, float* data_x, float* data_y, float* data_z)
{
	int16_t raw_data_x = ((int16_t)(raw_data[0])) | (((int16_t)(raw_data[1])) << 8);
	int16_t raw_data_y = ((int16_t)(raw_data[2])) | (((int16_t)(raw_data[3])) << 8);
	int16_t raw_data_z = ((int16_t)(raw_data[4])) | (((int16_t)(raw_data[5])) << 8);

	*data_x = units_per_bit * (float)raw_data_x;
	*data_y = units_per_bit * (float)raw_data_y;
	*data_z = units_per_bit * (float)raw_data_z;
}

#endif
