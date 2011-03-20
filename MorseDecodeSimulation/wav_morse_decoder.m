%% WAV_MORSE_DECODER
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

%% Create Morse Code 
% text: text to be encoded 
% fsig: frequency of the Constant Wave (CW) signal.
% SNR:  Signal to Noise Ratio of output
% fsample: sampling frequency of the signal. can be used to model the A2D
%    sampling frequency
fsig=700;  %Rockmite has; 600 to 830 Hz, or 230 Hz Bandwidth


%% Open Wave-File
FILENAME = 'n2xe_blank_pstate_trimmed.wav';
 [x,fsample,NBITS]=WAVREAD(FILENAME);
 
 %remove stereo if needed
 if(size(x,2)==2)
     x=x(:,1);
 end
 
 decimate_rate = 10;
 xn=decimate(x,decimate_rate,'fir');
 fsample = fsample/decimate_rate;
 L = length(xn);


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
% Mixer = exp(-1i*2*pi*fsig/fsample .* (0:L-1)).'; % real + imaginary
% signal = xn.*Mixer;  

%% Frequency Shift Transform - simplified.
% does the same as above but broken out into multiple parts to make it
% easier to understand for uProcessor implementation.
mixerI=sin(2*pi*fsig/fsample .* (0:L-1))';
mixerR=cos(2*pi*fsig/fsample .* (0:L-1))';
sigR = xn.*mixerR;
sigI = xn.*mixerI;
clear mixerI
clear mixerR
clear xn
% signal = complex(sigR,sigI);
% clear sigR
% clear sigI

%% Low Pass Filter
% use fdesign to build a low pass filter.
d=fdesign.lowpass('Fp,Fst,Ap,Ast',50,100,1,80,fsample);
% d=fdesign.lowpass('Fp,Fst,Ap,Ast',110,180,1,90,fsample);
% %  designmethods(d);  % gives details on the designed filter
% % hd = design(d,'butter','matchexactly','passband'); 
% hd = design(d); % Just designing the filter based on above criteria
% fvtool(hd);  % shows frequency response of the designed filter
% save hd % you can save the coefficients to speed it up
load hd % UNCOMMENT this if to use saved filter coefficients--> hd.mat 


%% Low Pass Filter - Simplified
% filter both the complex and real number then calc magnitude of results
fxnI = filter(hd,sigI);  % filter imaginary component
fxnR = filter(hd,sigR);  % filter real component
 clear sigR
 clear sigI
fxn = sqrt(fxnR.^2 + fxnI.^2); % calculate magnitude of Img & Real data 
% fxn is strictly real now.

%% Window Averaging Filter - Additional Filtering
Nw = 100;
mavgwindow = ones(1,Nw)/Nw; % essentially average over Nw = 100 size window
% fxn_before_avg=fxn;
fxn = filter(mavgwindow,1,abs(fxn)); % averging window cleans up results


%% DECODER to string
% compare the resulting decode with the original signal
%
%% Decode result of Algorithm
Ts = 1/fsample;
[r,t,f] = mag_detect_wave(fxn, -20, 1,Ts);

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



