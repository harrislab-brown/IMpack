# IMpack

<p align="center">
  <img src="https://github.com/johntantolik/IMpack/blob/main/assets/IMpack.png" width="1000">
</p>

IMpack is a data logging IMU designed for research applications where sampling rate and compactness are paramount. The board costs less than $100 to fabricate and assemble and features an array of MEMS accelerometer chips which allow it to achieve a maximum acceleration measurement range of +/- 400 g, sampling rates up to 26.6 kHz and an angular rate measurement range of +/- 2000 deg/s. Compared to commercial options that use piezoresistive accelerometer elements and complicated analog frontends, the IMpack is very low cost while still achieving good signal-to-noise ratios via its highly selectable measurement range. The IMpack is 38 mm in diameter and approximately 13 mm thick including the single-cell LiPo battery which powers the device. The battery can be recharged using the micro USB port and the onboard charging circuitry. The microcontroller (STM32F4) gathers data from 3 high performance MEMS IMU chips and logs data to a micro SD card in a plain text CSV format. The parameters for recording including measurement range and sample rate are adjustable via a plain text settings file on the SD card which is used to configure the device on startup. An I2C expansion port allows the IMpack to interface with external sensors, or the synchronization of an array of IMpacks.

# Capabilities

The IMpack can be configured to sample data using any subset of the chips in its sensor array, including all of them at maximum sampling rate simultaneously. All of the chips used measure acceleration in three axes and the LSM6DSO32 contains a gyroscope as well. Sensor components with different strengths are chosen to improve the versatility of the device to different measurement situations. The MEMS IMU parts used and their primary specifications are listed in the following table.

| Sensor | Sample rate (kHz) | Bandwidth (kHz) | Range | Bit depth |
| :---: | :---: | :---: | :---: | :---: |
| IIS3DWB | 26.6 | 6.3 | +/- 16 g | 16 |
| LSM6DSO32 (accel.) | 6.66 | 3.33 | +/- 32 g | 16 |
| LSM6DSO32 (gyro.) | 6.66 | 1.44 | +/- 2000 deg/s | 16 |
| ADXL373 | 5.12 | 2.56 | +/- 400 g | 12 |

The frequency responses of each of 10 IMpack test units are shown in the plots below for data gathered from each IMU chip on the PCB. More details regarding the test setup will be available soon in the accompanying publication. The raw testing data are available in the data directory.

<p align="center">
  <img src="https://github.com/johntantolik/IMpack/blob/main/assets/response.png" width="1000">
</p>

# Instructions for use

<p align="center">
  <img src="https://github.com/johntantolik/IMpack/blob/main/assets/usage.png" width="600">
</p>

## Programming the board

The IMpack is programmed via Serial Wire Debug using an [STLINK-V3MINIE](https://www.mouser.com/ProductDetail/STMicroelectronics/STLINK-V3MINIE?qs=MyNHzdoqoQKcLQe5Jawcgw%3D%3D&mgh=1&utm_id=17222215321&gad_source=1&gclid=Cj0KCQiA3sq6BhD2ARIsAJ8MRwUawTum9u1zJQP-2fxdKjuHkomzrG7uCrwYJbI-F8R0_4U0DyUifOMaAq6kEALw_wcB) debugger and a [Tag-Connect TC2030](https://www.tag-connect.com/product/tc2030-idc-nl) cable. We use [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) to compile and launch the project. In the case of a failed flashing that results in an unresponsive microcontroller, the "boot" jumper on the top of the IMpack can be shorted at power up to initialize the microcontroller in boot mode and recover it.

## Charging the battery

The IMpack can be powered from any single-cell lithium polymer battery that can provide at least 150 mA of current and support a 100 mA charge rate. The battery connector is 2-pin JST-SH. We use a [150 mAh battery](https://tinycircuits.com/products/lithium-ion-polymer-battery-3-7v-150mah?srsltid=AfmBOopFKh3iH_07LgFVlMNDD-mnGZ0MBEidQ0zw9-TLOAf2Frq8NPOu) which provides approximately 2 hours of operation on a full charge. The battery polarity is indicated with silkscreen markings near the battery connector. Be sure that your battery matches since there seems to be some variation among different manufacturers.

The IMpack battery can be charged using the micro USB port with the power switch in either the on or off position. The red LED indicates that the IMpack is receiving power over USB. The IMpack can optionally be powered over USB only with no need for a battery. The orange and green LEDs indicate the charge status of the battery. While the USB is plugged in, the orange LED indicates that the battery is charging and the green LED turns on once the battery is fully charged. When operating the device without the USB plugged in, the orange LED will turn on once the battery voltage is low and it needs to be recharged.

## SD card

The IMpack logs data and reads in the recording parameters using a micro SD card. We use an [8 GB SanDisk Industrial](https://www.amazon.com/SanDisk-Industrial-MicroSDHC-SDSDQAF3-008G-I-Everything/dp/B085GL89HQ?th=1) card. The card should be formatted as FAT32. The IMpack will operate in the top-level directory of the card so it is recommended to use an empty card without any important data. 

The IMpack will likely work well with a variety of SD cards but an important factor to note is the write latency of the SD card. This specification is often provided as a generous upper bound so it is difficult to compare the real world performance of different SD cards based on their datasheets. If the real world write latency is too large, some IMU data can be lost during recording. In our testing the Impack with SanDisk Industrial cards we experience no data loss.

## Taking a recording

With the IMpack connected to a power source (USB, battery or both), flip the power switch to the on position. The microcontroller will initialize the system and attempt to read in the settings file (more on that later). If the initialization is successful, the blue LED will blink twice. If no settings file is provided or the settings file is formatted incorrectly, the blue LED will blink 4 times. In this case the IMpack will write a correctly formatted default settings file to the card which can subsequently be tweaked by the user to configure the device. The blue LED should then begin flashing once every second, indicating that the device is in the idle state and ready to begin a recording. If instead there is a reeating sequence of 4 blinks, then the IMpack is in an error state (commonly due to a missing SD card).  

When the IMpack is idle, press the button to begin a recording. If a delay is configured, the device will enter the setup state where it waits before recording. A repeating sequence of 2 blue blinks indicates that the IMpack is in the setup state. After the configured delay, the device will enter the armed state if the recording is configured to begin based on an acceleration trigger. A repeating sequence of 3 blinks indicates that the IMpack is in the armed state. Once the acceleration trigger is detected, the device will begin recording, as indicated by rapid blinking of the blue LED. The recording will stop either after the configured recording length, or when the user presses the button again. After the recording, if data formatting is enabled, the device will spend some time formatting the raw sensor data into a plain text CSV file. A sequence of 3 slow blinks will indicate this state. Finally the device will return to the idle state at which point a new recording can be started or the device can be powered off. When the IMpack is not in use, the power switch should be in the off position to avoid draining the battery.

The recording sequence can be configured using the settings file and not all of the aforementioned states will necessarily be seen. For instance, if the delay before the armed state is set to zero and acceleration triggering is disabled, the device will immediately enter the recording state and begin logging data once the button is pressed. Similarly, the formatting state with 3 slow blinks will not be seen if data formatting is disabled, which may be desirable for long recordings at high data rates since the formatting can take a long time. In this case, the recording will be a binary file with the raw sensor data.

## Settings file

The settings.txt file on the SD card is used to configure the IMpack at startup. The sampling parameters for each IMU channel can be configured, as well as the overall recording parameters such as whether to wait for an acceleration trigger or whether to perform plain text formatting of the data file. An annotated example of the settings file is shown below describing each of the parameters and their allowed values. If a valid settings file is not found on startup, the IMpack will generate a default one on the SD card - this is the recommended way to get started with the configuration.

```
LSM6DSx_accel_enabled = 1  # enable or disable each measurement channel with values 1 or 0
LSM6DSx_accel_odr_hz = 6660  # allowed values: 13, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660
LSM6DSx_accel_range_g = 16  # allowed values: 4, 8, 16, 32
LSM6DSx_accel_lpf = 2  # setting for the low pass filter, 2 means bandwidth = ODR / 2 and so on (same meaning for other the other accelerometers), allowed values: 2, 4, 10, 20, 45, 100, 200, 400, 800
LSM6DSx_accel_offset_x_mg = 0  # these are DC offsets in milli-g, signed integer
LSM6DSx_accel_offset_y_mg = 0
LSM6DSx_accel_offset_z_mg = 0

LSM6DSx_gyro_enabled = 1
LSM6DSx_gyro_odr_hz = 6660  # allowed values: 13, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660
LSM6DSx_gyro_range_dps = 2000  # allowed values: 125, 250, 500, 1000, 2000
LSM6DSx_gyro_lpf = 2  # this is the bit value that is put in the chip register, consult table 60 in LSM6DSO32 data sheet, allowed values: 0, 1, 2, 3, 4, 5, 6, 7

IIS3DWB_accel_enabled = 1
IIS3DWB_accel_range_g = 16  # allowed values: 2, 4, 8, 16
IIS3DWB_accel_lpf = 2  # allowed values: 4, 10, 20, 45, 100, 200, 400, 800
IIS3DWB_accel_offset_x_mg = 0
IIS3DWB_accel_offset_y_mg = 0
IIS3DWB_accel_offset_z_mg = 0

ADXL37x_accel_enabled = 1
ADXL37x_accel_odr_hz = 5120  # allowed values: 320, 640, 1280, 2560, 5120
ADXL37x_accel_lpf = 2  # allowed values: 2, 4, 8, 16, 32
ADXL37x_accel_offset_x_mg = 0
ADXL37x_accel_offset_y_mg = 0
ADXL37x_accel_offset_z_mg = 0

delay_before_armed_ms = 0  # how long to remain in the staging state in ms
recording_length_ms = 5000  # how long to record for in ms
data_formatting_enabled = 1  # if enabled, will create CSV files after each recording
accel_trigger_enabled = 1  # once armed, the recording will start based on an acceleration trigger if enabled
accel_trigger_on_any_axis = 1  # trigger if any axis exceeds the threshold if 1, else looks only for specific axis
accel_trigger_axis = 2  # 0, 1, 2 for x, y, z, axis selection to trigger from
accel_trigger_level_mg = 500  # level of the trigger in units of milli-g
accel_trigger_rising_edge = 0  # select whether to trigger on rising or falling edge
```

## Data format

When plain text data formatting is enabled, the IMpack will create a separate CSV file for each active channel from the recording. The columns for time stamps and axis measurements are labeled with units, so interpreting the file should be straightforward. The binary data files consist of sequences of data points which each consist of 12 bytes. Each data point contains the unsigned 32 bit time stamp in microseconds, 3 axes of signed 16 bit acceleration/angular rate data, and finally unsigned 16 bit data type tag to indicate which channel produced the data. Example scripts for parsing the binary data in MATLAB and Python are provided in the examples directory. 

# Hardware design

<p align="center">
  <img src="https://github.com/johntantolik/IMpack/blob/main/assets/hardware design.png" width="600">
</p>

The board schematics, production files and KiCad 8 design files are available in the hardware directory. An enclosure or fixture is required to mount the IMpack depending on the application, which could be 3D printed or machined from metal in more stringent testing setups. Design files for the mechanical assemblies into which we mount the IMpack are available in the mechanical directory as examples. The PCB is 4 layers with double sided assembly. The main components and their functions are listed below. 

  - STM32F405 microcontroller (we use SDIO, SPI, NVIC, USB-FS, SWD, I2C hardware)
  - MCP73871 battery charge controller
  - TLV70033 LDO regulator
  - DM3D-SF micro SD card holder (most compact I could find)
  - IIS3DWB super high speed accelerometer with +/- 16 g range
  - ADXL373 high-g range accelerometer
  - LSM6DSO32 combined gyroscope and accelerometer with +/- 32 g range, low cost and widely available fallback option

We ordered 20 fully assembled Impack units in calendar year 2024 to be manufactured by JLCPCB in batches of 5 or 10. The total cost per board ended up between 60 and 80 USD. A large contributor to the cost is the high performance accelerometer chips IIS3DWB and ADXL373 which typically cost 20 USD per chip in low quanitites. The board could be manufactured with the LSM6DSO32 as the only IMU chip, saving approximately 40 USD per unit, without otherwise compromising the functionality. 

# Firmware design

The source code for the IMpack is available as an STM32CubeIDE project in the firmware directory. The IMpack firmware is written in C and developed using the toolchain provided with STM32CubeIDE version 1.16.0 along with ST's Hardware Abstraction Layer library provided in the STM32Cube FW_F4 V1.28.1 firmware package. The IMpack firmware uses an interrupt based scheme to retrieve data from the IMU chips resulting in minimum latency in which the MCU listens to the data ready pin from each chip and initiates the SPI data read on the appropriate edges of the data pin signal. The SPI read subroutine uses direct register manipulation for improved speed. Each data packet is tagged with a time stamp and an identifier for which chip it came from and inserted into a large double buffer in RAM. The buffer is written to a file on the SD card in binary format periodically as each half of the buffer is filled. Finally, at the end of the recording, the binary data file is read back and converted into a CSV text file on the SD card for more convenient processing by the user. A big challenge is the SD card latency (up to 100 ms latency according to the data sheet for the SanDisk Industrial card used). Data from the IMU chips needs to be double buffered so we can put new data from the sensors in one half while the other half is being written to the file. This means we would have to store 200 ms worth of data in memory to guarantee no data loss. At such high data rates, this is not feasible without using additional memory chips or a larger MCU. In practice, the latency of the SD card we selected is much lower so we don't lose data, but this is something to be aware of if a different SD card is used.


