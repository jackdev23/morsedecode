%% MorseMain.m
% The following simulation creates a Morse Code signal. The signal is
% transformed and filtered to enhance the constant wave pulse signal of
% morse code. 
% 
% TODO: Convert from pulse magnitudes to Text (string)
%
% Author: Sean Winfree
% Date: 9-28-2010

% Define Simulation Constants
close all; clear all; clc;
PLOT=0;

%% Create Morse Code 
% text: text to be encoded 
% fsig: frequency of the Constant Wave (CW) signal.
% SNR:  Signal to Noise Ratio of output
% fsample: sampling frequency of the signal. can be used to model the A2D
%    sampling frequency
fsig=700;  %Rockmite has; 600 to 830 Hz, or 230 Hz Bandwidth
sText = 'N2XE LABORATORY RULES';
SNR = -4; % some special scaling is done behind the scenes on this, input though is average SNR  
% fsample = 11.025e3; 
fsample = fsig*3; 
WPM = 12; 
[xn cleanxn] = encodeMorse(fsig,sText,SNR,fsample,WPM);
L = length(xn);

if(PLOT)
    %  Stimulus code to test the Morse Decoder     
     figure; plot(xn); title('Noisy Morse'); xlabel('samples');
    % FFT of xn   
    NFFT = 2^nextpow2(L);
    Y = fft(xn,NFFT)/L;
    Fs=fsample;
    % f = Fs/2*linspace(0,1,NFFT/2);
     f = Fs/2*linspace(0,1,NFFT);
    figure;
    % plot(f,2*abs(Y(1:NFFT/2)));
    plot(f,2*abs(Y)); % plot the whole freq domain
    title('Frequency Response of Morse Code - CW');
    xlabel('freq');
end
%
%
%
%% Morse Decoder 
% [yn] = decodeMorse(xn,fosc)
%
% Hilbert Transform
%   Shift/Move the 700Hz signal to base-band (DC) 
%   by multipling by a complex sinusoid
% Low Pass Filter
%   Now that the 700Hz signal is at base-band a simple low pass filter is
%   used to get ride of noise
% Magnitude Avereging Window
%   A final window filter is used to enhance/stabilize the magnitude of the
%   pulses.
%
Mixer = exp(-1i*2*pi*fsig/fsample .* (0:L-1)).'; % real + imaginary

signal = xn.*Mixer;

if(PLOT); figure; plot(2*abs(fft(signal))); end;
%% Low Pass Filter
% use fdesign to build a low pass filter.
d=fdesign.lowpass('Fp,Fst,Ap,Ast',50,100,1,80,fsample);
%  designmethods(d);  % gives details on the designed filter
% hd = design(d,'butter','matchexactly','passband'); 
hd = design(d); 
% fvtool(hd);  % shows frequency response of the designed filter
% save hd
% load hd % UNCOMMENT this if to use saved filter coefficients--> hd.mat 
fxn = filter(hd,double(signal));

%% Additional Filtering
Nw = 100;
mavgwindow = ones(1,Nw)/Nw; % essentially average over Nw = 100 size window
fxn = filter(mavgwindow,1,abs(fxn)); 

%% Plots
% abs is the magnitude of complex numbers
% mag = 20* log10(abs(fxn));

if(PLOT)
    figure;plot(20*log10(abs((fxn)))); title('Morse Pulse Envelope');
    % Plot orginial Morse Code Pulses over Decoded
    figure;plot((abs((fxn)))*2); 
    hold on; plot(cleanxn/(3.1)+0.1,'r'); 
    title('Performance of Decoded Morse'); xlabel('Samples'); 
    ylabel('Amplitude');
    legend('Decoded Morse', 'Orginal Morse');hold off;
end

%% FFT of fxn
% L = length(fxn);
% NFFT = 2^nextpow2(L);
% Y = fft(fxn,NFFT)/L;
% Fs=fsample;
% f = Fs/2*linspace(0,1,NFFT);
% figure; plot(f,2*abs(Y(1:NFFT)));
% title('Frequency Response of Filtered Morse Code - CW');
% xlabel('freq');


fxn_out = real(fxn);

%% DECODER to string
Ts = 1/fsample;
[r,t,f] = mag_detect(fxn_out, -23, 1);

widths = (f-r);
midpoint = (max(widths)+min(widths))/2;
Tunit = mean(widths(widths < midpoint))*Ts;

symbols = (f-r)*Ts > 2*Tunit;
spaces  = [(r(2:end)-f(1:end-1)),0]*Ts > 5*Tunit;

chars = {''};

for i = 1:length(symbols)
   
    if symbols(i)
        chars{end}(end+1) = '-';
        fprintf('-');
    else
        chars{end}(end+1) = '.';
        fprintf('.');
    end
    
    if spaces(i)
        chars{end+1} = '';
        fprintf(' / ');
    end
    
end
fprintf('\n');
dit_dah_string=chars;
% outstring =  demorse_btc(dit_dah_string{1});


[r,t,f] = mag_detect(cleanxn, -23, 1);
widths = (f-r);
midpoint = (max(widths)+min(widths))/2;
Tunit = mean(widths(widths < midpoint))*Ts;
symbols = (f-r)*Ts > 2*Tunit;
spaces  = [(r(2:end)-f(1:end-1)),0]*Ts > 5*Tunit;
chars = {''};
for i = 1:length(symbols)
 
    if symbols(i)
        chars{end}(end+1) = '-';
        fprintf('-');
    else
        chars{end}(end+1) = '.';
        fprintf('.');
    end
    
    if spaces(i)
        chars{end+1} = '';
        fprintf(' / ');
    end
    
end
fprintf('\n');
dit_dah_string=chars;