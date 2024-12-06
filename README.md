# IMpack

<p align="center">
  <img src="https://github.com/johntantolik/IMpack/blob/main/assets/IMpack.jpg" width="400">
</p>

IMpack is a data logging IMU designed for research applications where sampling rate and compactness are paramount. The board costs less than $100 to fabricate and assemble and features an array of MEMS accelerometer chips which allow it to achieve a maximum acceleration measurement range of +/- 400 g, sampling rates up to 26.6 kHz and an angular rate measurement range of +/- 2000 deg/s. Compared to commercial options that use piezoresistive accelerometer elements and complicated analog frontends, the IMpack is very low cost while still achieving good signal-to-noise ratios via its highly selectable measurement range. The IMpack is 38 mm in diameter and approximately 10 mm thick including the single-cell LiPo battery which powers the device. The battery can be recharged using the micro USB port and the onboard charging circuitry. The microcontroller (STM32F4) gathers data from 3 high performance MEMS IMU chips and logs data to a micro SD card in a plain text CSV format. The parameters for recording including measurement range and sample rate are adjustable via a plain text settings file on the SD card which is used to configure the device on startup. An I2C expansion port allows the IMpack to interface with external sensors, or the synchronization of an array of IMpacks.

# Capabilities

- plots of frequency response
- example data from water entry
- more detailed specs (battery life, recording length, resonant frequency, table of sensor chip specs)

# Instructions for use

## Programming the board

The IMpack is programmed via Serial Wire Debug using an [STLINK-V3MINIE](https://www.mouser.com/ProductDetail/STMicroelectronics/STLINK-V3MINIE?qs=MyNHzdoqoQKcLQe5Jawcgw%3D%3D&mgh=1&utm_id=17222215321&gad_source=1&gclid=Cj0KCQiA3sq6BhD2ARIsAJ8MRwUawTum9u1zJQP-2fxdKjuHkomzrG7uCrwYJbI-F8R0_4U0DyUifOMaAq6kEALw_wcB) debugger and a [Tag-Connect TC2030](https://www.tag-connect.com/product/tc2030-idc-nl) cable. We use [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) to compile and launch the project. In the case of a failed flashing that results in an unresponsive microcontroller, the "boot" jumper on the top of the IMpack can be shorted at power up to initialize the microcontroller in boot mode and recover it.

## Charging the battery

The IMpack can be powered from any single-cell lithium polymer battery that can provide at least 150 mA of current and support a 100 mA charge rate. The battery connector is 2-pin JST-SH. We use a [150 mAh battery](https://tinycircuits.com/products/lithium-ion-polymer-battery-3-7v-150mah?srsltid=AfmBOopFKh3iH_07LgFVlMNDD-mnGZ0MBEidQ0zw9-TLOAf2Frq8NPOu) which provides approximately 2 hours of operation on a full charge. The battery polarity is indicated with silkscreen markings near the battery connector. Be sure that your battery matches since there seems to be some variation among different manufacturers.

The IMpack battery can be charged using the micro USB port with the power switch in either the on or off position. The red LED indicates that the IMpack is receiving power over USB. The IMpack can optionally be powered over USB only with no need for a battery. The orange and green LEDs indicate the charge status of the battery. While the USB is plugged in, the orange LED indicates that the battery is charging and the green LED turns on once the battery is fully charged. When operating the device without the USB plugged in, the orange LED will turn on once the battery voltage is low and it needs to be recharged.

## SD card

The IMpack logs data and reads in the recording parameters using a micro SD card. We use an [8 GB SanDisk Industrial](https://www.amazon.com/SanDisk-Industrial-MicroSDHC-SDSDQAF3-008G-I-Everything/dp/B085GL89HQ?th=1) card. The card should use the FAT32 format. The IMpack will operate in the top-level directory of the card so it is recommended to use an empty card without any important data. 

The IMpack will likely work well with a variety of SD cards but an important factor to note is the write latency of the SD card. This specification is often provided as a generous upper bound so it is difficult to compare the real world performance of different SD cards based on their datasheets. If the real world write latency is too large, some IMU data can be lost during recording. In our testing the Impack with SanDisk Industrial cards we experience no data loss.

## Taking a recording

indicator LED sequences

## Settings file

## Data format

(add example scripts in MATLAB and python to parse both the binary and CSV data)

# Hardware design

- list of main chips and their function
  - STM32F405 microcontroller (we use SDIO, SPI, NVIC, USB-FS, SWD, I2C hardware)
  - MCP73871 battery charge controller
  - TLV70033 LDO regulator
  - DM3D-SF micro SD card holder (most compact I could find)
  - IIS3DWB super high speed accelerometer
  - ADXL373 high g range accelerometer
  - LSM6DSO32 combined gyroscope and accelerometer with +/- 32 g range
- discussion of overall board design (4 layer, 2-sided assembly, mounting surfaces)
- discussion of interchangeable IMU chips
- ECAD and MCAD files (hole pattern drawing)

# Firmware design

- discussion of overall structure (written in C object oriented, uses HAL but sometimes registers for speed, favors speed and simplicity, user interface made as easy as possible)
- initializes the SPI chips and enables their data ready pin interrupts when recording, logs the time stamp and that data is ready in highest priority interrupt. Then a lower priority timer interrupt for doing the actual SPI transaction, done blocking with registers for lowest latency. Writing to SD card happens in main loop (can hang for a while), streaming the raw binary data to a file. When a recording finishes, we re-open the data file and convert it all to plain text CSV which can take a while for large recordings so this feature can be turned off.
- A big challenge is the SD card latency (spec says 100 ms latency allowed?). Data from the IMU chips needs to be double buffered so we can put new data from the sensors in one half while the other half is being written to the file. This means we would have to store 200 ms worth of data in memory to guarantee no data loss. At such high data rates, this is not feasible without using additional memory chips or a larger MCU. In practice, the latency of the SD card we selected is much lower so we don't lose data, but something to be aware of.

# Future work

- longer recording times
- USB interface, real time plotter
- firmware support for synchronization for modal analysis
- RAM chip (or different MCU) to guarantee no data loss


