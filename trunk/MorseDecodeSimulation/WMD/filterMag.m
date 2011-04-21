clear all;
close all;
fsample=2100
%% Low Pass Filter
% use fdesign to build a low pass filter.
d=fdesign.lowpass('Fp,Fst,Ap,Ast',50,110,1,80,fsample);

% d=fdesign.lowpass('Fp,Fst,Ap,Ast',110,180,1,80,fsample);
 hd = design(d); % Just designing the filter based on above criteria
 save hd;