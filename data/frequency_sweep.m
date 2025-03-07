%% connect instruments

% connect to the function generator over USB as a VISA device (using intrument control toolbox)
dev_list = visadevlist;

% connect to function generator
clear fgen;
fgen_ind = find(dev_list.Model == "SDG1025");
if isempty(fgen_ind)
    error("Function generator not connected.");
else
    fgen = visadev(dev_list.ResourceName(fgen_ind));
    fprintf("Function generator SG1025 connected.\n");
end
fgen_disable_output(fgen);

% connect to scope
clear scope;
scope_ind = find(dev_list.Model == "SDS1204X-E");
if isempty(scope_ind)
    error("Oscilloscope not connected.");
else
    scope = visadev(dev_list.ResourceName(scope_ind));
    fprintf("Oscilloscope SDS1204X-E connected.\n");
end
scope.Timeout = 30;  % seconds? 


%% frequency sweep

% autosetting the scope can be pretty slow but it is an option
% scope_autoset(scope);

% note: requesting the scope data crashes the program if there is
% nothing on the screen of the scope. make sure the trigger is in the
% right place so it gets an acquisition at each frequency

% shaker_response file holds the response of the shaker at constant
% excitation amplitude so we can invert it to get a constant acceleration
load("shaker_excitation.mat")

% shaker excitation

% or can do a simpler sweep that is directly specified:
%freq_hz = logspace(1.3010, 4, 100);
%exct_ampl_vpp = ones(1, length(freq_hz)) * 0.1; 

num_steps = length(freq_hz);

% to iteratively update the excitation arrays:
%target_response_ampl = 0.5;
%exct_ampl_vpp_new = exct_ampl_vpp .* target_response_ampl ./ response_ampl;


if (max(exct_ampl_vpp) / (2 * sqrt(2)) > 1.9)
    error('Excitation amplitude too large for shaker')
end

% measured response
response_ampl = zeros(1, num_steps);
response_difference = zeros(1, num_steps);

t1 = cell(1, num_steps);  % arrays of the scope data (from both accelerometers) at each step
t2 = cell(1, num_steps);
a1 = cell(1, num_steps);
a2 = cell(1, num_steps);

% scope settings
vdiv = 0.2; 

scope_start(scope);
writeline(scope, "MSIZ 70K");

scope_configure_channel(scope, 1, 1, 0.0, vdiv);
scope_configure_channel(scope, 2, 1, 0.0, vdiv);
pause(5);


for i = 1:num_steps
    fgen_set_sine_wave(fgen, freq_hz(i), exct_ampl_vpp(i), 0, 0);
    
    if i == 1
        fgen_enable_output(fgen);
    end

    scope_configure_timebase(scope, 1 / freq_hz(i));


    pause(scope_get_tdiv(scope) * 28 + 0.5);  % wait enough time for the full trace to be available


    scope_stop(scope);
    

    pause(0.5);

    
    [t1{i}, a1{i}] = scope_get_data(scope, 1); 
    [t2{i}, a2{i}] = scope_get_data(scope, 2); 
    [ampl1, ind_start1, ind_end1] = get_amplitude_rms(a1{i});
    [ampl2, ind_start2, ind_end2] = get_amplitude_rms(a2{i});
    response_ampl(i) = 0.5 * (ampl1 + ampl2);
    response_difference(i) = abs(ampl1 - ampl2);


%     figure
%     set(gcf,'color', 'w')
%     hold on
%     box on
%     plot(t1{i}, a1{i})
%     plot(t2{i}, a2{i})
%     xline(t1([ind_start1, ind_end1]), 'b')
%     xline(t2([ind_start2, ind_end2]), 'r')
%     yline([response_ampl(i), -response_ampl(i)], 'k--')
%     xlim tight
%     ylim padded
%     xlabel('Time (s)')
%     ylabel('Voltage')
%     title(sprintf("%.2f Hz, %.3f response amplitude", freq_hz(i), response_ampl(i)))


    scope_start(scope);
    
end
fgen_disable_output(fgen);


%% plot results

figure
set(gcf,'color', 'w')

subplot(2, 1, 1)
hold on
box on
plot(freq_hz, response_ampl)
set(gca, 'xscale', 'log')
set(gca, 'yscale', 'log')
xlabel('Frequency (Hz)')
ylabel('Response amplitude (V)')

subplot(2, 1, 2)
hold on
box on
plot(freq_hz, response_difference)
set(gca, 'xscale', 'log')
set(gca, 'yscale', 'linear')
xlabel('Frequency (Hz)')
ylabel('Difference between accelerometer amplitudes (V)')

%% save results

[file,location] = uiputfile('*.mat');
if isequal(file, 0) || isequal(location, 0)
   disp('No file saved.')
else
   disp('Saving data files.')
   save(fullfile(location,file), "t1", "t2", "a1", "a2", "freq_hz", "response_ampl", "response_difference")
end

%% function generator helper functions

function fgen_enable_output(fgen)
    writeline(fgen, "C1:OUTP ON")
end

function fgen_disable_output(fgen)
    writeline(fgen, "C1:OUTP OFF")
end

function fgen_set_sine_wave(fgen, freq_hz, ampl_vpp, phase_deg, offset_v)
    wave_conf_str = sprintf("C1:BSWV WVTP,SINE,FRQ,%.3fHZ,AMP,%.3fV,OFST,%.3fV,PHSE,%.3f", freq_hz, ampl_vpp, offset_v, phase_deg);
    writeline(fgen, wave_conf_str);
end

%% scope helper functions

function scope_configure_channel(scope, channel_num, ch_on, offset_v, volt_div)
    attn_str = sprintf("C%d:ATTN 1", channel_num);  % sets the attenuation to 1x
    writeline(scope, attn_str);

    bwl_str = sprintf("BWL C%d, OFF", channel_num);  % turns off the bandwidth limit
    writeline(scope, bwl_str);

    cpl_str = sprintf("C%d:CPL D1M", channel_num);  % sets to DC coupling, 1M impedance
    writeline(scope, cpl_str);

    ofst_str = sprintf("C%d:OFST %.3fV", channel_num, offset_v);  % vertical offset
    writeline(scope, ofst_str);

    skew_str = sprintf("C%d:SKEW 0S", channel_num);  % no skew (time delay) between channels
    writeline(scope, skew_str);

    if ch_on
        tra_str = sprintf("C%d:TRA ON", channel_num);
    else
        tra_str = sprintf("C%d:TRA OFF", channel_num);
    end
    writeline(scope, tra_str);  % turn the trace on or off

    unit_str = sprintf("C%d:UNIT V", channel_num);  % sets the units to volts (why is needed?)
    writeline(scope, unit_str);

    vdiv_str = sprintf("C%d:VDIV %.3fV", channel_num, volt_div);  % sets the volts per division
    writeline(scope, vdiv_str);

    invs_str = sprintf("C%d:INVS OFF", channel_num);  % turns off channel inversion
    writeline(scope, invs_str)


    writeline(scope, "*OPC?");
    readline(scope);  % wait until the operation is complete
end

function scope_configure_timebase(scope, time_div_s)

    writeline(scope, "TRDL 0S");  % no trigger delay
    
    tdiv_str = sprintf("TDIV %.9fS", time_div_s);
    writeline(scope, tdiv_str);  % sets the time per division

    % these commands open the second zoomed view on the scope
    % writeline(scope, "HPOS 0S");  % no horizontal offset

    % hmag_str = sprintf("HMAG %.9fS", time_div_s);
    % writeline(scope, hmag_str);  % no magnification

end

function scope_autoset(scope)
    writeline(scope, "ASET");
    writeline(scope, "*OPC?");
    readline(scope);  % wait until the operation is complete
end

function [t, wf] = scope_get_data(scope, channel_num)
    
    % figure out the memory size so we know how much data to read out
    writeline(scope, "MSIZ?");
    msiz_str = readline(scope);
    while isempty(msiz_str)  % hacky - sometimes the scope gets stuck and doesn't respond so try again 
        writeline(scope, "MSIZ?");
        msiz_str = readline(scope);
    end

    msiz_str = split(msiz_str);
    msiz_str = msiz_str(2);
    if (msiz_str{1}(end) == 'K')
        m_factor = 1000;
    elseif (msiz_str{1}(end) == 'M')
        m_factor = 1000000;
    else
        disp(msiz_str)
        error('Unexpected format for scope memory string.')
    end
    msize = str2double(msiz_str{1}(1:end-1));

    % figure out the offset and division sizes
    writeline(scope, sprintf("C%d:VDIV?", channel_num));
    vdiv_str = readline(scope);
    while isempty(vdiv_str)
        writeline(scope, sprintf("C%d:VDIV?", channel_num));
        vdiv_str = readline(scope);
    end
    vdiv_str = split(vdiv_str);
    vdiv_str = vdiv_str(2);
    vdiv = str2double(vdiv_str{1}(1:end-1));

    
    writeline(scope, sprintf("C%d:OFST?", channel_num));
    ofst_str = readline(scope);
    while isempty(ofst_str)
        writeline(scope, sprintf("C%d:OFST?", channel_num));
        ofst_str = readline(scope);
    end
    ofst_str = split(ofst_str);
    ofst_str = ofst_str(2);
    offset_v = str2double(ofst_str{1}(1:end-1));


    writeline(scope, "TDIV?");
    tdiv_str = readline(scope);
    while isempty(tdiv_str)
        writeline(scope, "TDIV?");
        tdiv_str = readline(scope);
    end
    tdiv_str = split(tdiv_str);
    tdiv_str = tdiv_str(2);
    tdiv = str2double(tdiv_str{1}(1:end-1));
    

    writeline(scope, sprintf("C%d:WF? DAT2", channel_num));  % ask for data
    pause(0.1);
    header = read(scope, 70, "char");  % waveform header

    if (length(header) < 70)
        disp(header)
        error("Unexpected data from scope.")
    end

    
    % somehow there's always 48 less bytes of data than the nominal value
    wf = read(scope, msize * m_factor - 48, "uint8");  % read the binary data block
    

    % interpret the binary data based on the manual
    
    % "Note: If the decimal is greater than "127”, it should minus 256. Then the value is code value."
    % "voltage value (V) = code value *(vdiv /25) - voffset"

    for i = 1:length(wf)
        if wf(i) > 127
            wf(i) = wf(i) - 256;
        end
    end

    wf = wf(1:end-2);  % last 2 bytes are terminator
    wf = cast(wf, "double") * (vdiv / 25.0) - offset_v;  % convert to voltage
    

    % create the time vector
    num_divs = 14;  % this is how many divisions are on the screen (14 for our scope)
    t = linspace(0, num_divs * tdiv, length(wf));
            
end

function scope_plot_data(scope)
    % plots all of the active traces on the scope

    figure
    set(gcf, 'color', 'w')
    hold on
    box on
    xlabel('Time (s)')
    ylabel('Voltage')

    for i = 1:4
        if scope_is_channel_on(scope, i)
            [t, v] = scope_get_data(scope, i);
            plot(t, v);
        end
    end

end

function channel_on = scope_is_channel_on(scope, channel_num)

    writeline(scope, sprintf("C%d:TRA?", channel_num));
    res_str = readline(scope);
    while isempty(res_str)
        writeline(scope, sprintf("C%d:TRA?", channel_num));
        res_str = readline(scope);
    end

    res_str = split(res_str);
    res_str = res_str(2);

    channel_on = (res_str == "ON");

end

function scope_stop(scope)
    writeline(scope, "STOP");
end

function scope_start(scope)
    writeline(scope, "ARM");
    writeline(scope, "TRMD AUTO");
end

function tdiv = scope_get_tdiv(scope)

    writeline(scope, "TDIV?");
    tdiv_str = readline(scope);
    while isempty(tdiv_str)
        writeline(scope, "TDIV?");
        tdiv_str = readline(scope);
    end
    tdiv_str = split(tdiv_str);
    tdiv_str = tdiv_str(2);
    tdiv = str2double(tdiv_str{1}(1:end-1));

end

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
