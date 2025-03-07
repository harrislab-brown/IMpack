%% test parameters

a_ADX_range = 400;  % from the IMpack settings file
a_IIS_range = 16;
a_LSM_range = 32;
g_LSM_range = 2000;


piezo_sensitivity_mvpg = 101.8;  % sensitivity of the reference accelerometer in mV/g

%% processing parameters

% for IIS3DWB:
% fft_window_size = 16384;  % window size in samples
% fft_window_overlap = fft_window_size / 4;
% 
% freq_change_threshold = 0.025;  % how much the dominant frequency has to change to be considered a new test frequency
% freq_change_min_spacing = 20000;  % must be at least this many samples between detected frequency changes

% for LSM6DSx:
% fft_window_size = 4096; 
% fft_window_overlap = fft_window_size / 4;
% 
% freq_change_threshold = 0.025; 
% freq_change_min_spacing = 5000;

% for ADXL37x:
fft_window_size = 4096; 
fft_window_overlap = fft_window_size / 4;

freq_change_threshold = 0.025; 
freq_change_min_spacing = 5000;



%% open the data files

% get the IMpack data file
[data_file, data_path] = uigetfile('*.dat', 'Select IMpack data file', 'multiselect', 'off');
addpath(data_path);
[ta_LSM, a_LSM, tg_LSM, g_LSM, ta_IIS, a_IIS, ta_ADX, a_ADX] = IMpack_get_data(fullfile(data_path, data_file), a_LSM_range, g_LSM_range, a_IIS_range, a_ADX_range);

% also load in the .mat file with the 'ground truth' test data 
[~, data_file_name, ~] = fileparts(data_file);
test_data = open(fullfile(data_path, data_file_name + ".mat"));


%% process the reference acceleration

% using RMS method with average value
%accel_ampl_ref_g = test_data.response_ampl / (0.001 * piezo_sensitivity_mvpg);

% using FFT  or RMS method with specific channel
accel_ampl_ref_g = zeros(1, length(test_data.freq_hz));
for i = 1:length(test_data.freq_hz)

    %ampl1 = get_amplitude_fft(test_data.a1{i}, 1/mean(diff(test_data.t1{i})), test_data.freq_hz(i));
    %ampl2 = get_amplitude_fft(test_data.a2{i}, 1/mean(diff(test_data.t2{i})), test_data.freq_hz(i));
    
    ampl1 = get_amplitude_rms(test_data.a1{i});
    ampl2 = get_amplitude_rms(test_data.a2{i});

    %accel_ampl_ref_g(i) = 0.5 * (ampl1 + ampl2) / (0.001 * piezo_sensitivity_mvpg);
    accel_ampl_ref_g(i) = ampl1 / (0.001 * piezo_sensitivity_mvpg);
end


%% process the IMpack acceleration

% the IMpack data is contained in a single file with all of the test
% frequencies. We need to partition the data into sections corresponding to
% each different excitation frequency


time_s = ta_ADX;
accel_g = a_ADX(:, 3);
accel_g = accel_g - mean(accel_g);

num_samples = length(accel_g);
sampling_freq_hz = 1 / mean(diff(time_s));

freq_change_ind = [];  % data indices corresponding to frequency changes in the test

dominant_freq_hz = [];
window_time = [];

for i = 1:fft_window_overlap:(num_samples - fft_window_size)
    window_data = accel_g(i:(i + fft_window_size - 1));

    % fft
    n = length(window_data);
    y = fft(window_data);
    f = (0:n - 1) * (sampling_freq_hz / n);  % frequencies in the fft
    p2 = abs(y / n);  % two-sided spectrum
    p1 = p2(1:n / 2);  % one-sided spectrum

    [peak_ampl, peak_ind] = max(p1);
    dominant_freq_hz = [dominant_freq_hz; f(peak_ind)];
    window_time = [window_time; time_s(i + fft_window_size / 2)];

    % check if the frequency has changed
    if length(dominant_freq_hz) > 1 && ...
            abs(dominant_freq_hz(end) - dominant_freq_hz(end - 1)) > (freq_change_threshold * dominant_freq_hz(end - 1)) && ...
            (isempty(freq_change_ind) || i + fft_window_size / 2 - freq_change_ind(end) > freq_change_min_spacing)
        freq_change_ind = [freq_change_ind; i + fft_window_size / 2];
    end

end


% option to plot the dominant frequency versus time for tuning the
% processing parameters

% figure
% set(gcf, 'color', 'w')
% hold on
% box on
% plot(window_time, dominant_freq_hz)
% xline(time_s(freq_change_ind))
% xlabel('Time (s)')
% ylabel('Dominant frequency (Hz)')
% title(sprintf("Found %d frequency changes", length(freq_change_ind)))
% set(gca, 'YScale', 'log')


% plot the indices found and give the user the option to edit them
figure
set(gcf, 'color', 'w')
hold on
box on
plot(time_s, accel_g)
xline(time_s(freq_change_ind))
xlabel('Time (s)')
ylabel('Acceleration (g)')
title(sprintf("Found %d frequency changes (target %d)\nClick to add/remove; press 'enter' to continue", length(freq_change_ind), length(test_data.freq_hz) + 1))

while true

    [x, y] = ginput(1);

    if isempty(x)
        break;
    end
    

    [~, change_ind] = min(abs(time_s(freq_change_ind) - x));

    remove_threshold_s = 0.25;
    if abs(time_s(freq_change_ind(change_ind)) - x) < remove_threshold_s
        % remove the clicked point
        freq_change_ind(change_ind) = [];
    else
        % add a new point
        [~, ind] = min(abs(time_s - x));
        freq_change_ind = sort([freq_change_ind; ind]);
    end


    cla
    plot(time_s, accel_g)
    xline(time_s(freq_change_ind))
    title(sprintf("Found %d frequency changes (target %d)\nClick to add/remove; press 'enter' to continue", length(freq_change_ind), length(test_data.freq_hz) + 1))
    drawnow

end

close(gcf)


% get the IMpack acceleration amplitude at each frequency step

accel_ampl_IMpack_g = zeros(1, length(accel_ampl_ref_g));

if length(freq_change_ind) ~= length(accel_ampl_IMpack_g) + 1
    error('Detected the wrong amount of frequency changes')
end

for i = 1:length(accel_ampl_IMpack_g)

    freq_step_window_size = freq_change_ind(i + 1) - freq_change_ind(i);
    freq_step_window_start = round(freq_change_ind(i) + freq_step_window_size / 4);
    freq_step_window_end = round(freq_change_ind(i + 1) - freq_step_window_size / 4);

    window_accel_g = accel_g(freq_step_window_start:freq_step_window_end);
    
    % remove the mean acceleration (gravity)
    window_accel_g = window_accel_g - mean(window_accel_g);

    % get the amplitude based on RMS
    accel_ampl_IMpack_g(i) = get_amplitude_rms(window_accel_g);

    % get the amplitude based on FFT
    %accel_ampl_IMpack_g(i) = get_amplitude_fft(window_accel_g, sampling_freq_hz, test_data.freq_hz(i));

end


%% plot the frequency response

% figure
% set(gcf, 'color', 'w')
% hold on
% box on
% xlabel('Frequency (Hz)')
% ylabel('Response (dB)')
% set(gca, 'XScale', 'log')

plot(test_data.freq_hz, mag2db(accel_ampl_IMpack_g./accel_ampl_ref_g), "LineWidth", 1.5)

xlim tight
ylim padded

%% data processing functions

function [ampl, ind_start, ind_end] = get_amplitude_rms(x)

% x is a sinusoidal signal with some non-idealities
% estimate the amplitude by computing the RMS

[~, ~, ind] = zerocrossrate(x);  % get where the signal crosses zero 
ind(1) = 0;  % the zerocrossrate function always considers the first index a zero crossing but we dont want
ind_start = find(ind, 1, 'first');
ind_end = find(ind, 1, 'last');

ampl = sqrt(mean(x(ind_start:ind_end).^2)) * sqrt(2);

end


function ampl = get_amplitude_fft(x, sample_freq, excitation_freq)

% x is a sinusoidal signal with some non-idealities, sampled at sample_freq
% estimate the amplitude by the taking the value of the fft at the
% excitation frequency

if mod(length(x), 2) == 1
    x = x(1:end-1);
end


n = length(x);
x = x(:) .* hamming(n);  % windowing function
y = fft(x);
p2 = abs(y / n);  % two-sided spectrum
p1 = p2(1:n/2+1);
p1(2:end-1) = 2*p1(2:end-1);
p1 = p1(1:n/2);
f = 0:(sample_freq/n):(sample_freq/2-sample_freq/n);  % frequencies in the fft

[~, ind] = min(abs(f - excitation_freq));
ampl = p1(ind);

end
