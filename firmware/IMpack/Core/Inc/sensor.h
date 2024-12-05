/*
 * sensor.h
 *
 *  Created on: Apr 23, 2024
 *      Author: johnt
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#include "stm32f4xx_hal.h"

typedef struct
{
	/* SPI handle */
	SPI_HandleTypeDef* spi;

	/* chip select pin */
	GPIO_TypeDef* cs_port;
	uint16_t cs_pin;

	/* data ready interrupt pin */
	uint16_t int_pin;

	/* function pointers for converting the register to the SPI bit sequence to send (some sensors have the address shifted or require a R/W bit to be set) */
	uint8_t (*convert_reg_write)(uint8_t reg);
	uint8_t (*convert_reg_read)(uint8_t reg);

	/* address of sensor data register */
	uint8_t data_reg;

	/* function to convert data to physical units */
	void (*process_data)(uint8_t* raw_data, float units_per_bit, float* data_x, float* data_y, float* data_z);

	/* registers for enabling and disabling the sensor */
	uint8_t enable_reg, enable_data;
	uint8_t disable_reg, disable_data;

} SPISensor;

uint8_t SPISensor_TestCommunication(SPISensor* sensor, uint8_t reg, uint8_t data);  /* verify communication by reading a register for the expected data (e.g. WHO_AM_I) */
uint8_t SPISensor_WriteMultiple(SPISensor* sensor, const uint8_t* reg, const uint8_t* data, uint8_t size);  /* write a sequence of data to a sequence of device registers */

HAL_StatusTypeDef SPISensor_ReadRegister(SPISensor* sensor, uint8_t reg, uint8_t* data);
HAL_StatusTypeDef SPISensor_WriteRegister(SPISensor* sensor, uint8_t reg, uint8_t data);

HAL_StatusTypeDef SPISensor_Enable(SPISensor* sensor);
HAL_StatusTypeDef SPISensor_Disable(SPISensor* sensor);

#endif /* INC_SENSOR_H_ */
