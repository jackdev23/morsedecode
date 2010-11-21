function [r,t,f,sig] = mag_detect(iq, nl, genplots)

if nargin < 2
    genplots = 0;
end

iq = iq(:); % convert to column vector.
sig.m  = abs(iq);
sig.fm = nan(1,length(sig.m));

PC = 0;

N = 16;

detect = nl+5;
reject = nl+7;
r   = nan(1,5500);
t   = nan(1,5500);
f   = nan(1,5500);
state = 0;

Ts = 1/11000;
Tunit = 1.2/13 * 1/Ts;

for s = 1:length(sig.m)
    if s > N  % wait until N samples have been gathered
        temp_fm = sum(sig.m(s-N:s))./N;
        sig.fm(s) = db(temp_fm); % convert averaged signal to dB. must have a least N samples per dit to work. 
    end
    
    switch state
        case 0
            if sig.fm(s) > detect % Rising edge detect
                state = 1;
            end
        case 1
            if sig.fm(s) > reject % Qualify pulse (meets "reject" threshold)
                state = 2;
                PC = PC + 1;
                r(PC) = s;
                t(PC) = s;
                peak_power = sig.fm(s);
                
                if PC > 1 && r(PC) - f(PC-1) > 5*Tunit
                    fprintf(' / ');
                end
                
            elseif sig.fm(s) < detect - 3  % Discard due to low power
                state = 0;
            end
        case 2
            if sig.fm(s) > peak_power+3
                peak_power = sig.fm(s);
                lb = r(PC);
                hb = s;
                t(PC) = lb + (sig.fm(s)-3 - sig.fm(lb))/(sig.fm(hb)-sig.fm(lb));
            end

            if sig.fm(s) < detect - 3 % Falling edge detected before we transition
                state = 0;
                f(PC) = s;
                
                if f(PC)-r(PC) > 2*Tunit
                    fprintf('-');
                else
                    fprintf('.');
                end 
            end
    end
end

fprintf('\n');

r = r(1:PC);
t = t(1:PC);
f = f(1:PC);

if ~genplots
    return;
else
    lim = inf;
    if isinf(lim)
        II = 1:length(sig.m);
        rP = t;
    else
        II = 1:lim;
        rP = t(r < lim);
    end
    
    h1 = figure; set(h1, 'WindowStyle', 'Docked', 'Name', 'fmag');
    plot(sig.fm(II));
    hold all;
    plot([1,II(end)],[detect,detect]);
    if ~isempty(r)
        plot(rP', detect, 'rx');
    end
    hold off;

    h2 = figure; set(h2, 'WindowStyle', 'Docked', 'Name', 'mag');
    plot(db(sig.m(II)));
    hold all;
    plot([1,II(end)],[detect,detect]);
    if ~isempty(r)
        plot(rP', detect, 'rx');
    end
    hold off;
end