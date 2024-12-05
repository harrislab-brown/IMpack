
#ifndef LSM6DSx_SPI_DRIVER_H
#define LSM6DSx_SPI_DRIVER_H

#include <stdint.h>

/* device defines */
#define LSM6DSx_DEVICE_ID 0x6C  /* fixed value of WHO_AM_I register */

#define LSM6DSx_RESOLUTION 16  /* 16 bits of sensor resolution */
#define LSM6DSx_OFFSET_WEIGHT 0.0009765625f  /* g per bit of user offset */

/* device register addresses (p.49) */
#define LSM6DSx_REG_WHO_AM_I   0x0F
#define LSM6DSx_REG_CTRL1_XL 0x10  /* sensor control registers */
#define LSM6DSx_REG_CTRL8_XL 0x17
#define LSM6DSx_REG_CTRL2_G  0x11
#define LSM6DSx_REG_CTRL4_C 0x13
#define LSM6DSx_REG_CTRL6_C 0x15
#define LSM6DSx_REG_INT1_CTRL 0x0D  /* interrupt pin controls */
#define LSM6DSx_REG_INT2_CTRL 0x0E
#define LSM6DSx_REG_OUTX_L_XL 0x28  /* accelerometer output registers */
#define LSM6DSx_REG_OUTX_H_XL 0x29
#define LSM6DSx_REG_OUTY_L_XL 0x2A
#define LSM6DSx_REG_OUTY_H_XL 0x2B
#define LSM6DSx_REG_OUTZ_L_XL 0x2C
#define LSM6DSx_REG_OUTZ_H_XL 0x2D
#define LSM6DSx_REG_OUTX_L_G 0x22  /* gyroscope output registers */
#define LSM6DSx_REG_OUTX_H_G 0x23
#define LSM6DSx_REG_OUTY_L_G 0x24
#define LSM6DSx_REG_OUTY_H_G 0x25
#define LSM6DSx_REG_OUTZ_L_G 0x26
#define LSM6DSx_REG_OUTZ_H_G 0x27
#define LSM6DSx_REG_X_OFS_USR 0x73  /* offset registers */
#define LSM6DSx_REG_Y_OFS_USR 0x74
#define LSM6DSx_REG_Z_OFS_USR 0x75

/* register values */
#define LSM6DSx_ACCEL_ODR_DISABLE 0b00000000  /* CTRL1_XL register */
#define LSM6DSx_ACCEL_ODR_13HZ    0b00010000
#define LSM6DSx_ACCEL_ODR_26HZ    0b00100000
#define LSM6DSx_ACCEL_ODR_52HZ    0b00110000
#define LSM6DSx_ACCEL_ODR_104HZ   0b01000000
#define LSM6DSx_ACCEL_ODR_208HZ   0b01010000
#define LSM6DSx_ACCEL_ODR_416HZ   0b01100000
#define LSM6DSx_ACCEL_ODR_833HZ   0b01110000
#define LSM6DSx_ACCEL_ODR_1660HZ  0b10000000
#define LSM6DSx_ACCEL_ODR_3330HZ  0b10010000
#define LSM6DSx_ACCEL_ODR_6660HZ  0b10100000
#define LSM6DSx_G_RANGE_4  0b00000000  /* CTRL1_XL register */
#define LSM6DSx_G_RANGE_8  0b00001000
#define LSM6DSx_G_RANGE_16  0b00001100
#define LSM6DSx_G_RANGE_32 0b00000100
#define LSM6DSx_GYRO_ODR_DISABLE 0b00000000  /* CTRL2_G register */
#define LSM6DSx_GYRO_ODR_13HZ    0b00010000
#define LSM6DSx_GYRO_ODR_26HZ    0b00100000
#define LSM6DSx_GYRO_ODR_52HZ    0b00110000
#define LSM6DSx_GYRO_ODR_104HZ   0b01000000
#define LSM6DSx_GYRO_ODR_208HZ   0b01010000
#define LSM6DSx_GYRO_ODR_416HZ   0b01100000
#define LSM6DSx_GYRO_ODR_833HZ   0b01110000
#define LSM6DSx_GYRO_ODR_1660HZ  0b10000000
#define LSM6DSx_GYRO_ODR_3330HZ  0b10010000
#define LSM6DSx_GYRO_ODR_6660HZ  0b10100000
#define LSM6DSx_DPS_RANGE_125  0b00000010  /* CTRL2_G register */
#define LSM6DSx_DPS_RANGE_245  0b00000000
#define LSM6DSx_DPS_RANGE_500  0b00000100
#define LSM6DSx_DPS_RANGE_1000 0b00001000
#define LSM6DSx_DPS_RANGE_2000 0b00001100

/* sensor configuration */
#define LSM6DSx_CONFIGURATION_REG  {LSM6DSx_REG_CTRL1_XL, LSM6DSx_REG_CTRL2_G, LSM6DSx_REG_INT1_CTRL, LSM6DSx_REG_INT2_CTRL, LSM6DSx_REG_CTRL4_C, LSM6DSx_REG_CTRL6_C, LSM6DSx_REG_X_OFS_USR, LSM6DSx_REG_Y_OFS_USR, LSM6DSx_REG_Z_OFS_USR, LSM6DSx_REG_CTRL8_XL}
#define LSM6DSx_CONFIGURATION_DATA {0x00, 0x00, 0x01, 0x02, 0x0C, 0x03, 0x00, 0x00, 0x00}

/*
 * Configuration sequence:
 * power down the accelerometer
 * power down the gyroscope
 * set INT1 pin to trigger when acceleration data ready (p. 59)
 * set INT2 pin to trigger when gyroscope data ready (p. 60)
 * mask the data ready pin until filters have settled, also disable I2C (p. 57)
 * set the weight of the offsets and set the gyroscope to max bandwidth (p. 66)
 * set the acceleration DC offsets (p. 96)
 * set the accelerometer low pass filter
 */

static uint8_t LSM6DSx_config_reg[] = LSM6DSx_CONFIGURATION_REG;
static uint8_t LSM6DSx_config_data[] = LSM6DSx_CONFIGURATION_DATA;
static uint8_t LSM6DSx_config_size = sizeof(LSM6DSx_config_reg) / sizeof(LSM6DSx_config_reg[0]);

void LSM6DSx_GetConfiguration(uint32_t lpf_a, uint32_t lpf_g, int32_t ofsx_mg, int32_t ofsy_mg, int32_t ofsz_mg,
		uint8_t** config_reg, uint8_t** config_data, uint8_t* config_size)
{
	/*
	 * Get register and data arrays for configuring the sensor based on the input parameters for DC offsets
	 */

	/* accelerometer DC offsets */
	LSM6DSx_config_data[6] = (int8_t)((float)ofsx_mg * 0.001f / LSM6DSx_OFFSET_WEIGHT);  /* x offset, 2^(-10) g/LSB, in two's complement */
	LSM6DSx_config_data[7] = (int8_t)((float)ofsy_mg * 0.001f / LSM6DSx_OFFSET_WEIGHT);  /* y offset */
	LSM6DSx_config_data[8] = (int8_t)((float)ofsz_mg * 0.001f / LSM6DSx_OFFSET_WEIGHT);  /* z offset */

	/* accelerometer LPF */
	uint8_t lpf_data;
	switch(lpf_a)
	{
	case 2:
		lpf_data = 0 << 5;
		break;
	case 4:
		lpf_data = 0 << 5;
		break;
	case 10:
		lpf_data = 1 << 5;
		break;
	case 20:
		lpf_data = 2 << 5;
		break;
	case 45:
		lpf_data = 3 << 5;
		break;
	case 100:
		lpf_data = 4 << 5;
		break;
	case 200:
		lpf_data = 5 << 5;
		break;
	case 400:
		lpf_data = 6 << 5;
		break;
	case 800:
		lpf_data = 7 << 5;
		break;
	default:
		break;
	}
	LSM6DSx_config_data[9] = lpf_data;


	/* gyroscope LPF */
	LSM6DSx_config_data[5] = (uint8_t)lpf_g;


	*config_reg = LSM6DSx_config_reg;
	*config_data = LSM6DSx_config_data;
	*config_size = LSM6DSx_config_size;
}

void LSM6DSx_GetAccelEnable(uint32_t lpf, uint32_t odr, uint32_t range, uint8_t* reg, uint8_t* data)
{
	/*
	 * return the address and data to enable the sensor with the desired data rate and range
	 */

	*reg = LSM6DSx_REG_CTRL1_XL;

	uint8_t odr_data;
	switch(odr)
	{
	case 13:
		odr_data = LSM6DSx_ACCEL_ODR_13HZ;
		break;
	case 26:
		odr_data = LSM6DSx_ACCEL_ODR_26HZ;
		break;
	case 52:
		odr_data = LSM6DSx_ACCEL_ODR_52HZ;
		break;
	case 104:
		odr_data = LSM6DSx_ACCEL_ODR_104HZ;
		break;
	case 208:
		odr_data = LSM6DSx_ACCEL_ODR_208HZ;
		break;
	case 416:
		odr_data = LSM6DSx_ACCEL_ODR_416HZ;
		break;
	case 833:
		odr_data = LSM6DSx_ACCEL_ODR_833HZ;
		break;
	case 1660:
		odr_data = LSM6DSx_ACCEL_ODR_1660HZ;
		break;
	case 3330:
		odr_data = LSM6DSx_ACCEL_ODR_3330HZ;
		break;
	case 6660:
		odr_data = LSM6DSx_ACCEL_ODR_6660HZ;
		break;
	default:
		odr_data = LSM6DSx_ACCEL_ODR_DISABLE;
		break;
	}

	uint8_t range_data;
	switch(range)
	{
	case 4:
		range_data = LSM6DSx_G_RANGE_4;
		break;
	case 8:
		range_data = LSM6DSx_G_RANGE_8;
		break;
	case 16:
		range_data = LSM6DSx_G_RANGE_16;
		break;
	case 32:
		range_data = LSM6DSx_G_RANGE_32;
		break;
	default:
		range_data = LSM6DSx_G_RANGE_4;
		break;
	}

	*data = (odr_data | range_data) | (lpf == 2 ? 0x00 : 0x02);
}

void LSM6DSx_GetGyroEnable(uint32_t odr, uint32_t range, uint8_t* reg, uint8_t* data)
{
	/*
	 * return the address and data to enable the sensor with the desired data rate and range
	 */

	*reg = LSM6DSx_REG_CTRL2_G;

	uint8_t odr_data;
	switch(odr)
	{
	case 13:
		odr_data = LSM6DSx_GYRO_ODR_13HZ;
		break;
	case 26:
		odr_data = LSM6DSx_GYRO_ODR_26HZ;
		break;
	case 52:
		odr_data = LSM6DSx_GYRO_ODR_52HZ;
		break;
	case 104:
		odr_data = LSM6DSx_GYRO_ODR_104HZ;
		break;
	case 208:
		odr_data = LSM6DSx_GYRO_ODR_208HZ;
		break;
	case 416:
		odr_data = LSM6DSx_GYRO_ODR_416HZ;
		break;
	case 833:
		odr_data = LSM6DSx_GYRO_ODR_833HZ;
		break;
	case 1660:
		odr_data = LSM6DSx_GYRO_ODR_1660HZ;
		break;
	case 3330:
		odr_data = LSM6DSx_GYRO_ODR_3330HZ;
		break;
	case 6660:
		odr_data = LSM6DSx_GYRO_ODR_6660HZ;
		break;
	default:
		odr_data = LSM6DSx_GYRO_ODR_DISABLE;
		break;
	}

	uint8_t range_data;
	switch(range)
	{
	case 125:
		range_data = LSM6DSx_DPS_RANGE_125;
		break;
	case 245:
		range_data = LSM6DSx_DPS_RANGE_245;
		break;
	case 500:
		range_data = LSM6DSx_DPS_RANGE_500;
		break;
	case 1000:
		range_data = LSM6DSx_DPS_RANGE_1000;
		break;
	case 2000:
		range_data = LSM6DSx_DPS_RANGE_2000;
		break;
	default:
		range_data = LSM6DSx_DPS_RANGE_245;
		break;
	}

	*data = (odr_data | range_data);
}

uint8_t LSM6DSx_ConvertWriteRegister(uint8_t reg)
{
	return reg;
}

uint8_t LSM6DSx_ConvertReadRegister(uint8_t reg)
{
	return (reg | 0x80);
}

void LSM6DSx_ProcessData(uint8_t* raw_data, float units_per_bit, float* data_x, float* data_y, float* data_z)
{
	int16_t raw_data_x = ((int16_t)(raw_data[0])) | (((int16_t)(raw_data[1])) << 8);
	int16_t raw_data_y = ((int16_t)(raw_data[2])) | (((int16_t)(raw_data[3])) << 8);
	int16_t raw_data_z = ((int16_t)(raw_data[4])) | (((int16_t)(raw_data[5])) << 8);

	*data_x = units_per_bit * (float)raw_data_x;
	*data_y = units_per_bit * (float)raw_data_y;
	*data_z = units_per_bit * (float)raw_data_z;
}

#endif
