/*
 * sensor.c
 *
 *  Created on: Apr 23, 2024
 *      Author: johnt
 */

#include "sensor.h"

uint8_t SPISensor_TestCommunication(SPISensor* sensor, uint8_t reg, uint8_t data)
{
	/* verify communication by reading a register for the expected data (e.g. WHO_AM_I) */
	uint8_t err_num = 0;
	HAL_StatusTypeDef status;

	uint8_t response;
	status = SPISensor_ReadRegister(sensor, reg, &response);
	err_num += (status != HAL_OK);

	if (response != data) {return 255;}

	return err_num;
}

uint8_t SPISensor_WriteMultiple(SPISensor* sensor, const uint8_t* reg, const uint8_t* data, uint8_t size)
{
	/* write a sequence of data to a sequence of device registers */
	uint8_t err_num = 0;
	HAL_StatusTypeDef status;

	for (uint8_t i = 0; i < size; i++)
	{
		status = SPISensor_WriteRegister(sensor, reg[i], data[i]);
		err_num += (status != HAL_OK);
	}

	return err_num;
}

HAL_StatusTypeDef SPISensor_ReadRegister(SPISensor* sensor, uint8_t reg, uint8_t* data)
{
	uint8_t tx_buf[2] = {sensor->convert_reg_read(reg), 0x00};
	uint8_t rx_buf[2];

	HAL_GPIO_WritePin(sensor->cs_port, sensor->cs_pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(sensor->spi, tx_buf, rx_buf, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(sensor->cs_port, sensor->cs_pin, GPIO_PIN_SET);

	*data = rx_buf[1];

	return status;
}

HAL_StatusTypeDef SPISensor_WriteRegister(SPISensor* sensor, uint8_t reg, uint8_t data)
{
	uint8_t tx_buf[2] = {sensor->convert_reg_write(reg), data};

	HAL_GPIO_WritePin(sensor->cs_port, sensor->cs_pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef status = HAL_SPI_Transmit(sensor->spi, tx_buf, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(sensor->cs_port, sensor->cs_pin, GPIO_PIN_SET);

	return status;
}

HAL_StatusTypeDef SPISensor_Enable(SPISensor* sensor)
{
	return SPISensor_WriteRegister(sensor, sensor->enable_reg, sensor->enable_data);
}

HAL_StatusTypeDef SPISensor_Disable(SPISensor* sensor)
{
	return SPISensor_WriteRegister(sensor, sensor->disable_reg, sensor->disable_data);
}

