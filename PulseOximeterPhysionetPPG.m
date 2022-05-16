%% Estimation of SpO2 level and heart rate using PPG data from Physionet.

clear 
clc
clf

PPG = readtable('physionetshort.csv');

Red_raw = PPG(:,19); 
IR_raw =PPG(:,20);

R1 = table2array( Red_raw );
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
X1 = filter(b, 1, R1);
X2 = filter(b, 1, R2);

fs=500; %sampling rate 500Hz
FFT_size=64; 
%% Data frame for Red and IR PPG data
for n=1:fix((length(X1)/(2*fs))-2)
y1=X1(n*fs:(n*fs+FFT_size-1),1); %For RED
y2=X2(n*fs:(n*fs+FFT_size-1),1); %For IR
%% FFT Transform for RED PPG data
NFFT = FFT_size ; % Next power of 2 from length of y
Y1 = fft(y1,NFFT);
f1 = fs/2*linspace(0,1,NFFT/2+1);
%% FFT Transform for IR PPG data
NFFT = FFT_size; % Next power of 2 from length of y
Y2 = fft(y2,NFFT);
f2 = fs/2*linspace(0,1,NFFT/2+1);
%% Finding the local maximum value in the RED spectrum
YY=abs(Y1(6:12));
local_maximum_i=1;
local_maximum=YY(1);
for i=2:(length(YY)-1)
    if local_maximum<(YY(i))
        local_maximum_i=i;
        local_maximum=YY(i);
    end    
end
peak_RED_i=6-1+local_maximum_i;
%% Finding the local maximum value in the IR spectrum
YY=abs(Y2(6:12));
local_maximum_i=1;
local_maximum=YY(1);
for i=2:(length(YY)-1)
    if local_maximum<(YY(i))
        local_maximum_i=i;
        local_maximum=YY(i);
    end    
end
peak_IR_i=6-1+local_maximum_i;
%% Heart rate Calculation
HEART_RATE(n) = f2(peak_IR_i)*60;  

%% SpO2 Calculation
R_RED = abs(Y1(peak_RED_i)/abs(Y1(1)));
R_IR = abs(Y2(peak_IR_i)/abs(Y2(1)));
R=R_RED/R_IR
SpO2(n) = 110 - 25*R; 

end
%% Take average value of heart rate and SpO2
HR=sum(HEART_RATE(2:(length(HEART_RATE)-1)))/(length(HEART_RATE)-2);
S=sum(SpO2(2:(length(SpO2)-1)))/(length(SpO2)-2);
Heart_Rate=round(HR)
SpO2_Level=round(S)

%For SpO2
Maximum_SpO2_Value=max(SpO2,[],'all')
Minimum_SpO2_Value=min(SpO2,[],'all')
%For Heartrate
Maximum_HR_Value=max(HEART_RATE,[],'all')
Minimum_HR_Value=min(HEART_RATE,[],'all')

%% plots

subplot(2,1,1);
plot(R1,'r');
% hold on 
% plot(X1,'r');
% legend('Raw data','FIR Filtered');
title("Red signals before FIR filtering")
subplot(2,1,2);
plot(X1,'r');
% hold on
% plot(X2,'b');
%legend('FIR','FFT');
title("Red signals after FIR filtering")

figure(2)
subplot(2,1,1);
plot(R2,'b');
title("IR signals after FIR filtering")
%hold on 
subplot(2,1,2);
plot(X2,'b');
%legend('RED','IR');
title("IR signals after FIR filtering")

figure(3)
subplot(2,1,1);
plot(SpO2,'r');
title("SpO2 Reading Range")
hold on 
subplot(2,1,2);
plot(HEART_RATE,'b');
%legend('SpO2','HEART RATE');
title("BPM Reading Range")

