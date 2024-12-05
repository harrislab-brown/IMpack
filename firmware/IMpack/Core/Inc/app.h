/*
 * main application
 *
 *  Created on: Apr 5, 2024
 *      Author: johnt
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "main.h"

void App_Setup(SD_HandleTypeDef* hsd, SPI_HandleTypeDef* hspi_LSM6DS3, SPI_HandleTypeDef* hspi_IIS3DWB, SPI_HandleTypeDef* hspi_ADXL372, volatile uint32_t* micros_timer);
void App_Loop();
void App_PinInterrupt(uint16_t GPIO_Pin);
void App_TimerInterrupt();

void App_EnableAccelerometerInterrupts();
void App_DisableAccelerometerInterrupts();

#endif /* INC_APP_H_ */
