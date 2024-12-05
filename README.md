# IMpack

<p align="center">
  <img src="https://github.com/johntantolik/IMpack/blob/main/assets/IMpack.jpg" width="400">
</p>

IMpack is a data logging IMU designed for research applications where sampling rate and compactness are paramount. The board costs less than $100 to fabricate and assemble and features an array of MEMS accelerometer chips which allow it to achieve a maximum acceleration measurement range of +/- 400 g, sampling rates up to 26.6 kHz and an angular rate measurement range of +/- 2000 deg/s. Compared to commercial options that use piezoresistive accelerometer elements and complicated analog frontends, the IMpack is very low cost while still achieving good signal-to-noise ratios via its highly selectable measurement range. The IMpack is 38 mm in diameter and approximately 10 mm thick including the single-cell LiPo battery which powers the device. The battery can be recharged using the micro USB port and the onboard charging circuitry. The microcontroller (STM32F4) gathers data from 3 high performance MEMS IMU chips and logs data to a micro SD card in a plain text CSV format. The parameters for recording including measurement range and sample rate are adjustable via a plain text settings file on the SD card which is used to configure the device on startup. An I2C expansion port allows the IMpack to interface with external sensors, or the construction of a synchronized array of IMpacks.

# Capabilities

- plots of frequency response
- example data from water entry
- more detailed specs (battery life, recording length, resonant frequency, table of sensor chip specs)

# Instructions for use

- flashing the board (STLink, tag-connect)
- charging (battery type, charge rate, battery life)
- SD card (we use SanDisk industrial cards for their low latency)
- indicator led sequences
- beginning a recording
- settings file format
- data file format (binary or CSV) (examples to parse and plot either format in matlab or python)

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


