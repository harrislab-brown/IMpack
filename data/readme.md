## frequency response

TODO: the raw data files for the frequency response are larger than 100 MB so not sure what to do with them

## water entry

Raw data files from the IMpack and enDAQ IMUs embedded in an aluminum impactor with various nose shapes and water impact speeds. The number preceding "mps" in the file name is the impact speed in m/s. The next part decribes the nose geometry which is either a hemisphere or a cone whose included angle is specified. The overall diameter of the impactor is 44.45 mm and additional details can be found in the accompanying publication. The final number in the file name is the trial number and IMpack and enDAQ files with the same trial number correspond to simultaneous recordings. The enDAQ data files have .ide extension and can be opened and processed in enDAQ's free [lab software](https://endaq.com/pages/vibration-shock-analysis-software-endaq-slam-stick-lab?srsltid=AfmBOoo7xRbBNeMLJfOLlSteyIvGvib2A_wtY0az-d9iFSdC5y2pTeTemat).

## Scripts

The MATLAB scripts used to perform the frequency response testing of the IMpack units are included here. The script "frequency_sweep.m" is used to control the waveform generator and oscilloscope over their SCPI interfaces to perform a sweep over the test frequencies and stream reference accelerometer data from the oscilloscope. The script "process_IMpack_frequency_response.m" is used to process the test data and create the frequency response plots.
