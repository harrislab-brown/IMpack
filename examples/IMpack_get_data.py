import struct
from collections import defaultdict
import matplotlib.pyplot as plt

def IMpack_get_data(file_name, range_LSM_accel, range_LSM_gyro, range_IIS, range_ADX):

    ID_LSM_ACCEL = 0x1000
    ID_LSM_GYRO = 0x0020
    ID_IIS = 0x8000
    ID_ADX = 0x0010

    with open(file_name, 'rb') as file:
        data = list(struct.iter_unpack('<LhhhH', file.read()))
        grouped_data = defaultdict(list)
        for item in data:
            grouped_data[item[4]].append(list(item[0:4]))

        # split data by channel
        data_LSM_accel = grouped_data[ID_LSM_ACCEL]
        data_LSM_gyro = grouped_data[ID_LSM_GYRO]
        data_IIS = grouped_data[ID_IIS]
        data_ADX = grouped_data[ID_ADX]

        # convert the binary acceleration to g or degree/s
        for i in range(len(data_LSM_accel)):
            data_LSM_accel[i][0] *= 1e-6  # convert to seconds
            data_LSM_accel[i][1] *= range_LSM_accel / 2**15  # convert to g
            data_LSM_accel[i][2] *= range_LSM_accel / 2**15
            data_LSM_accel[i][3] *= range_LSM_accel / 2**15

        for i in range(len(data_LSM_gyro)):
            data_LSM_gyro[i][0] *= 1e-6  # convert to seconds
            data_LSM_gyro[i][1] *= range_LSM_gyro / 2**15  # convert to degree/s
            data_LSM_gyro[i][2] *= range_LSM_gyro / 2**15
            data_LSM_gyro[i][3] *= range_LSM_gyro / 2**15

        for i in range(len(data_IIS)):
            data_IIS[i][0] *= 1e-6  # convert to seconds
            data_IIS[i][1] *= range_IIS / 2**15  # convert to g
            data_IIS[i][2] *= range_IIS / 2**15
            data_IIS[i][3] *= range_IIS / 2**15

        # the ADXL373 is more complicated to convert because it is a 12 bit sensor with opposite endianness
        for i in range(len(data_ADX)):
            data_ADX[i][0] *= 1e-6  # convert to seconds
            data_ADX[i][1] = struct.unpack('>h', struct.pack('<h', data_ADX[i][1]))[0] * range_ADX / 2**15  # convert to g
            data_ADX[i][2] = struct.unpack('>h', struct.pack('<h', data_ADX[i][2]))[0] * range_ADX / 2**15
            data_ADX[i][3] = struct.unpack('>h', struct.pack('<h', data_ADX[i][3]))[0] * range_ADX / 2**15


        return [data_LSM_accel, data_LSM_gyro, data_IIS, data_ADX]


if __name__ == "__main__":

    # call the parsing function with the appropriate file path and sensor ranges (depending how they are configured)
    [data_LSM_accel, data_LSM_gyro, data_IIS, data_ADX] = IMpack_get_data("4mps_sphere_1.dat", 32, 2000, 16, 400)
    fig, ax = plt.subplots(2, 2)

    # LSM6DSx accelerometer
    ax[0, 0].plot([packet[0] for packet in data_LSM_accel], [packet[1] for packet in data_LSM_accel], label = "x")
    ax[0, 0].plot([packet[0] for packet in data_LSM_accel], [packet[2] for packet in data_LSM_accel], label = "y")
    ax[0, 0].plot([packet[0] for packet in data_LSM_accel], [packet[3] for packet in data_LSM_accel], label = "z")
    ax[0, 0].set_title("LSM6DSx Accelerometer")
    ax[0, 0].set_xlabel("Time (s)")
    ax[0, 0].set_ylabel("Acceleration (g)")
    ax[0, 0].legend()

    # LSM6DSx gyroscope
    ax[0, 1].plot([packet[0] for packet in data_LSM_gyro], [packet[1] for packet in data_LSM_gyro], label = "x")
    ax[0, 1].plot([packet[0] for packet in data_LSM_gyro], [packet[2] for packet in data_LSM_gyro], label = "y")
    ax[0, 1].plot([packet[0] for packet in data_LSM_gyro], [packet[3] for packet in data_LSM_gyro], label = "z")
    ax[0, 1].set_title("LSM6DSx Gyroscope")
    ax[0, 1].set_xlabel("Time (s)")
    ax[0, 1].set_ylabel("Angular rate (deg/s)")
    ax[0, 1].legend()

    # IIS3DWB accelerometer
    ax[1, 0].plot([packet[0] for packet in data_IIS], [packet[1] for packet in data_IIS], label = "x")
    ax[1, 0].plot([packet[0] for packet in data_IIS], [packet[2] for packet in data_IIS], label = "y")
    ax[1, 0].plot([packet[0] for packet in data_IIS], [packet[3] for packet in data_IIS], label = "z")
    ax[1, 0].set_title("IIS3DWB Accelerometer")
    ax[1, 0].set_xlabel("Time (s)")
    ax[1, 0].set_ylabel("Acceleration (g)")
    ax[1, 0].legend()

    # ADXL373 accelerometer
    ax[1, 1].plot([packet[0] for packet in data_ADX], [packet[1] for packet in data_ADX], label = "x")
    ax[1, 1].plot([packet[0] for packet in data_ADX], [packet[2] for packet in data_ADX], label = "y")
    ax[1, 1].plot([packet[0] for packet in data_ADX], [packet[3] for packet in data_ADX], label = "z")
    ax[1, 1].set_title("ADXL373 Accelerometer")
    ax[1, 1].set_xlabel("Time (s)")
    ax[1, 1].set_ylabel("Acceleration (g)")
    ax[1, 1].legend()

    plt.show()