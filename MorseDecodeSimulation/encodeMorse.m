%% [xn, cleanxn] = encodeMorse(text,fsig,SNR,fsample,WPM);
% 
% text: text to be encoded 
% fsig: frequency of the Constant Wave (CW) signal.
% SNR:  Signal to Noise Ratio of output
% fsample: sampling frequency of the signal. can be used to model the A2D
%    sampling frequency
% 
% 
% The following are provided as an example 
% For WPM = 12
% timing dit = 1200/WPM = 100 mSEC
% timing dah = 3*(1200/WPM)= 300 mSEC
% letter spacing = 3*100 = 300mSEC
% dit/dah spacing = 1*100 (inter-word)
% 
function [xn cleanxn] = encodeMorse(varargin)
% text,fsig,SNR,fsample,WPM
if(nargin==0)
   fsig=700;  %Rockmite has; 600 to 830 Hz, or 230 Hz Bandwidth
%    sText = 'N2XE LABORATORY RULES';
   sText = 'UPQQABCAQAAX';
   SNR = 2; % some special scaling is done behind the scenes on this, input though is average SNR  
   fsample = 11.025e3; 
   WPM = 12; 
else
    fsig    = varargin{1};
    sText    = varargin{2};
    SNR     = varargin{3};
    fsample = varargin{4};
    WPM     = varargin{5};
end
%% DEFINES
PLOT=0;   % Set to 1 to plot
RECORD=0; % record output into wave file

%% International Morse Code
%
timing = (1.2/WPM)*2*pi*fsig; % seconds
 % spacing in time between samples for sampling frequency
 % for example Fsample = 11025 & t_dit takes 100ms then the number of
 % discrete samples available at Fsample = 11025/100 ~ 1103
t_dit = (0:2*pi*fsig/fsample:timing)';
t_dah = (0:2*pi*fsig/fsample:(timing*3))';
t_ssp = (0:2*pi*fsig/fsample:(timing*1))';
t_letter = (0:2*pi*fsig/fsample:(timing*3))';
t_blank  = (0:2*pi*fsig/fsample:(timing*4))'; % actually is 7 but always append letter_space after each letter which is 3 
% create array lengths from the times above
dit = ones(length(t_dit),1);  % CW on
dah = ones(length(t_dah),1);  % CW on 
blank  = zeros(length(t_blank),1); % CW off
ssp    = zeros(length(t_ssp),1);   % CW off
letter = zeros(length(t_letter),1);% CW off

%% Evaluate string
output=[];
for i=1:length(sText)        
    if(isletter(sText(i)))    % handles numbers too    
        output=[output;getMorseLetterSequence(sText(i),dit,ssp,dah);letter];    
    elseif(isstrprop(sText(i),'wspace'))
        output=[output;blank];
    elseif(ischar(sText(i))) % if it is a numbe its is still a char 
        output=[output;getMorseLetterSequence(sText(i),dit,ssp,dah);letter];  
    end
end
cleanxn=output;

% time of the output
time = length(output)/fsample + 1;% take length of output and add buffer

% Generate morse code signal(s)
% linear drift of frequency
DRIFT = 0;
if DRIFT == 1
    time = 0.23;
    y = [];
    for fsig = 830:-5:580 % freq drift range
        endpoint =time*2*pi*fsig; 
        delta_angle = 2*pi*fsig/fsample;
        t = 0:delta_angle:endpoint;
        y = [y; 0.5*sin(t)'];
        clear t delta_angle endpoint
    end
% No drift
else
    endpoint =time*2*pi*fsig; 
    delta_angle = 2*pi*fsig/fsample;
    t = 0:delta_angle:endpoint;
    y = 0.5*sin(t)';            % create sin wave at defind freq 
    t_out = t./(2.*pi.*fsig)';  % scale output    
end

if(PLOT)
    figure;   
    plot(t_out, y);             % constant frequency at fsig
    title('Modulation Frequency of Constant Wave');
    xlabel('time (seconds)');
end

% Noise portion (average ab = 0.14)
if(PLOT)
    figure;
    plot(output.*y(1:length(output)));
    title('Morse Signal');
    xlabel('Samples');
end

%% get noise
% ab = 1.8262.*wavread('ssb.wav', 44.1e3);
% absr=repmat(ab,8,1);
% ratio = 10^(SNR/20);
% absr = absr./(2*ratio);
% r=output + absr(1:length(output),1); % add noise to square waves

r = awgn(output,SNR,'measured');

% save('2dB_noise_r', 'r'); % changed the name to specific setting
% load('2dB_noise_r', 'r'); % dylan - uncomment to get the noise *.mat file
% imported into workspace.

%% modulate
y2 = y(1:length(r));
out = y2.*r;
%% adjust any value over 0.5 or below -0.5 (clip values)
indx = out >= 0.5;
out(indx) = 0.5;
indx = out <= -0.5;
out(indx) = -0.5;

% output morse code values with modulation fsig 
% and sampling frequency fsample
xn=out+0.5; %scale to 0to1
% plot(t_out(1:length(out)),out)
if(PLOT)
    figure;
    plot(t_out(1:length(xn)),xn);
    title('Morse Signal with Noise');
    xlabel('Time (seconds)');
end
% Comment out if you don't want Morse
if(RECORD)
 wavwrite(xn,11.025e3,'demo.wav'); 
 wavplay(xn, 11.025e3);
end
%EOF
