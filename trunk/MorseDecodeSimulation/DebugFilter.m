clf; clear all; clc; close all;
fsample=2100; % REMEMBER - filter design dependent on Sampling Freq
%700Hz
freqsig=700; 
sig700 = (sin(2*pi*(freqsig/fsample) * (1:100)));
figure; plot(sig700);
%600Hz
freqsig=600; sig600 = (sin(2*pi*(freqsig/fsample) * (1:100)));
% 100Hz
freqsig = 100;
sig100 = (sin(2*pi*(freqsig/fsample) * (1:100)));
hold on; plot(sig100, 'r'); title('Two Sin Waves having fun');


%% Filter
d=fdesign.lowpass('Fp,Fst,Ap,Ast',110,180,1,80,fsample);
%  designmethods(d);  % gives details on the designed filter
% hd = design(d,'butter','matchexactly','passband'); 
hd = design(d); % Just designing the filter based on above criteria
fsig100 = filter(hd,sig100);
fsig700 = filter(hd,sig700);
figure; plot(fsig700);  % notice the transients take some time to lock
hold on; plot(fsig100, 'r'); title('Same two waves just got filtered');



