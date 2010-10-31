%% MorseMain.m
% The following simulation creates a Morse Code signal. The signal is
% transformed and filtered to enhance the constant wave pulse signal of
% morse code. 
% 
% Convert from pulse magnitudes to Text (string)
%
% Author: Sean Winfree
% Date: 09-28-2010 - Orignial Release
% Date: 10-15-2010 - Added simplification of complex number to demonstrate
%                    how to implement in a uProcessor

% Define Simulation Constants
close all; clear all; clc;
PLOT=1;

%% Create Morse Code 
% text: text to be encoded 
% fsig: frequency of the Constant Wave (CW) signal.
% SNR:  Signal to Noise Ratio of output
% fsample: sampling frequency of the signal. can be used to model the A2D
%    sampling frequency
fsig=700;  %Rockmite has; 600 to 830 Hz, or 230 Hz Bandwidth
sText = 'N2XE LABORATORY RULES';
SNR = 2; % some special scaling is done behind the scenes on this, input though is average SNR  
% fsample = 11.025e3; 
fsample = fsig*3; 
WPM = 12; 
[xn cleanxn] = encodeMorse(fsig,sText,SNR,fsample,WPM);
L = length(xn);

if(PLOT)
    %  Stimulus code to test the Morse Decoder     
    figure; plot(xn); title('Noisy Morse'); xlabel('samples');      
    % FFT
    NFFT = 2^nextpow2(L);
    Yorig = fft(xn,NFFT)/L;
    Fs=fsample;
    f = fftshift(Fs*linspace(0,1,NFFT));
    f(f>Fs/2)=f((f>(Fs/2)))-Fs;
    figure;
    plot((f),2*abs(fftshift(Yorig))); % plot the whole freq domain
    title('Frequency Response of Morse Code - CW');
    xlabel('Freq');ylabel('Amp');
end
%
%
%
%% Morse Decoder 
% [yn] = decodeMorse(xn,fosc)
%
% Hilbert - Frequency Shift Transform
%   Shift/Move the 700Hz signal to base-band (DC) 
%   by multipling by a complex sinusoid
% Low Pass Filter
%   Now that the 700Hz signal is at base-band a simple low pass filter is
%   used to get ride of noise
% Magnitude Avereging Window
%   A final window filter is used to enhance/stabilize the magnitude of the
%   pulses.
%

%% Frequency Shift Transform
Mixer = exp(-1i*2*pi*fsig/fsample .* (0:L-1)).'; % real + imaginary
signal = xn.*Mixer;  

%% Frequency Shift Transform - simplified.
% does the same as above but broken out into multiple parts to make it
% easier to understand for uProcessor implementation.
mixerI=sin(2*pi*fsig/fsample .* (0:L-1))';
mixerR=cos(2*pi*fsig/fsample .* (0:L-1))';
sigR = xn.*mixerR;
sigI = xn.*mixerI;
signal = complex(sigR,sigI);

%% Plot frequency results of Shift
if(PLOT)
    Yshift = fft(signal,NFFT)/L;
    figure; plot(f,2*abs(fftshift(Yshift))); 
    title('Frequency Response of Shifted Morse Code - CW');
    xlabel('Freq');ylabel('Amp');
end

%% Low Pass Filter
% use fdesign to build a low pass filter.
d=fdesign.lowpass('Fp,Fst,Ap,Ast',50,100,1,80,fsample);
%  designmethods(d);  % gives details on the designed filter
% hd = design(d,'butter','matchexactly','passband'); 
hd = design(d); % Just designing the filter based on above criteria
% fvtool(hd);  % shows frequency response of the designed filter
% save hd % you can save the coefficients to speed it up
% load hd % UNCOMMENT this if to use saved filter coefficients--> hd.mat 
% fxn = filter(hd,(signal)); % Filter the signal

%% Low Pass Filter - Simplified
% filter both the complex and real number then calc magnitude of results
fxnI = filter(hd,sigI);  % filter imaginary component
fxnR = filter(hd,sigR);  % filter real component
fxn = sqrt(fxnR.^2 + fxnI.^2); % calculate magnitude of Img & Real data 
% fxn is strictly real now.

%% Window Averaging Filter - Additional Filtering
Nw = 100;
mavgwindow = ones(1,Nw)/Nw; % essentially average over Nw = 100 size window
fxn_before_avg=fxn;
fxn = filter(mavgwindow,1,abs(fxn)); % averging window cleans up results

%% PLOTTING
if(PLOT)
    %% Compare before/after Averaging Filter
    figure; plot(fxn_before_avg,'r'); hold on;
    plot(fxn,'g'); 
    title('Filtered Morse Code - CW');xlabel('Samples');ylabel('Amplitude');
    legend('x[n] after FIR Filter', 'x[n] after Window Average Filter');
    hold off;
    
    figure;plot(20*log10(abs((fxn)))); title('Morse Pulse Envelope');
    % Plot orginial Morse Code Pulses over Decoded
    figure;plot((abs((fxn)))*2); 
    hold on; plot(cleanxn/(3.1)+0.1,'r'); 
    title('Performance of Decoded Morse'); xlabel('Samples'); 
    ylabel('Amplitude');
    legend('Decoded Morse', 'Orginal Morse');hold off;
    %% FFT of Shifted and Filtered Signal
    Fs=fsample;
    f = fftshift(Fs*linspace(0,1,NFFT));
    f(f>Fs/2)=f((f>(Fs/2)))-Fs;    
    Y = fft(fxn,NFFT)/L;
    figure; plot(f,2*abs(fftshift(Y(1:NFFT))));
    title('Frequency Response of Shifted & Filtered Morse Code - CW');
    xlabel('Freq');ylabel('Amp');
    
    %% Subplot of frequency effects throughout algorithm
    subplot(1,3,1);  % Original Signal 
    plot(f,2*abs(fftshift(Yorig(1:NFFT))));
    xlabel('Freq');ylabel('Amp');
    title('Morse Code - CW');
    subplot(1,3,2);   % After Frequency Shift
    plot(f,2*abs(fftshift(Yshift(1:NFFT))));
    xlabel('Freq');ylabel('Amp');
    title('Shifted Morse Code - CW');
    subplot(1,3,3);   % After Filtering
    plot(f,2*abs(fftshift(Y(1:NFFT))));
    xlabel('Freq');ylabel('Amp');
    title('Shifted & Filtered Morse Code - CW');
end

%% DECODER to string
% compare the resulting decode with the original signal
%
%% Decode result of Algorithm
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

%% Decode the Original (clean) Signal
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



% Notes
% abs is the magnitude of complex numbers
% mag = 20* log10(abs(fxn));
% 
% EOF