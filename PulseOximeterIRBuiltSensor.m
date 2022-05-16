%% Estimation of heart rate in bpm using IR PPG data from built IR sensor.

clear 
clc
clf

PPGdata = readtable('irppgonly.csv');

IR_raw =PPGdata(:,1);

R2 = table2array( IR_raw );


% FIR Window Bandpass filter designed using the FIR1 function.

% All frequency values are in Hz.
Fs = 500;  % Sampling Frequency 500

N    = 513;      % Order 513
Fc1  = 0.5;      % First Cutoff Frequency
Fc2  = 5;        % Second Cutoff Frequency
flag = 'scale';  % Sampling Flag
Beta = 4.53;     % Window Parameter 4.53
% Create the window vector for the design algorithm.
win = kaiser(N+1, Beta);

% Calculate the coefficients using the FIR1 function.
b  = fir1(N, [Fc1 Fc2]/(Fs/2), 'bandpass', win, flag);
Hd = dfilt.dffir(b)
%end

 
% Analyse the filter using the Filter Visualization Tool
fvtool(Hd, 'Fs', Fs)
 
% Filter the signal with the FIR filter

X2 = filter(b, 1, R2);

%% Data input for Heart rate calculation
y2 = X2(:,1); %IR 
fs=500; 
NFFT=64; % FFT size
 
%% Beat count for heart rate
% Moving average filter
for i=1:(length(y2)-fs/5)
    local_sum=0;
    for j=1:fs/5
        local_sum=local_sum+y2(i+j);
    end
        y(i)=local_sum/(fs/5);
end
% Find peaks
count=0;
peak_i=1;
while count<=(length(y)-fs)
    
for i=1:fs
    yy(i) = y(i+count);
end
local_i_max = 1;
local_max = yy(local_i_max);
for i=2:fs
    if local_max<yy(i)
        local_i_max = i;
        local_max=yy(i);
    end
end
pk(peak_i)=count+local_i_max;
peak_i=peak_i+1;
count =count + fs;
end
beat_count = 0;
beat_count_i = 1;
for i=1:length(pk)-1
    if pk(i)<(pk(i+1)-10)    
        beat_count=beat_count+(fs/(pk(i+1)-pk(i)))*60;
        beat_count_i=beat_count_i+1;
    end
end
if beat_count_i>1
    HEART_RATE=round(beat_count/(beat_count_i-1))
end


%% Obtained Maximum and Minimum Heart rate values in bpm

Maximum_HR_Value=max(HEART_RATE,[],'all')
Minimum_HR_Value=min(HEART_RATE,[],'all')

%% plots

figure()
%subplot(2,1,1);
plot(R2,'r');
%title("IR signals after FIR filtering")
hold on 
%subplot(2,1,2);
plot(X2,'b');
legend('PRE-FIR','POST-FIR');
title("IR signals after FIR filtering")


figure(2)
subplot(2,1,1);
plot(R2,'b');
title("IR signals before FIR filtering")
xlabel("Sampled signals")
ylabel("Amplitude")
%hold on 
subplot(2,1,2);
plot(X2,'b');
%legend('RED','IR');
title("IR signals after FIR filtering")
xlabel("Sampled signals")
ylabel("Amplitude")
