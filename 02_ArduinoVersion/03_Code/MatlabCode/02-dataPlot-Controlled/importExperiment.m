function [Table,Info] = importExperiment(filename)
% #####################
% #### Mean Import ####
% #####################
opts = delimitedTextImportOptions("NumVariables", 3);

% Specify range and delimiter
opts.DataLines = [2, 2];
opts.Delimiter = "\t";

% Specify column names and types
opts.VariableNames = ["V2_mean", "Isense_mean", "dt"];
opts.SelectedVariableNames = ["V2_mean", "Isense_mean", "dt"];
opts.VariableTypes = ["double", "double", "double"];

% Specify file level properties
opts.ExtraColumnsRule = "ignore";
opts.EmptyLineRule = "read";


% Import the data
Info = readtable(filename, opts);
Info.dt = Info.dt * 1E-6; % dt shuld be in second
[~ , name, ~] = fileparts(filename);
Info.Properties.Description = name;

% #####################
% #### Data Import ####
% #####################
% Set up the Import Options and import the data
opts = delimitedTextImportOptions("NumVariables", 3);
dataLines = [4, Inf];
% Specify range and delimiter
opts.DataLines = dataLines;
opts.Delimiter = "\t";

% Specify column names and types
opts.VariableNames = ["PWM", "V2_read", "Isense_read", "e"];
opts.VariableTypes = ["double", "double", "double", "double"];

% Specify file level properties
opts.ExtraColumnsRule = "ignore";
opts.EmptyLineRule = "read";

% Import the data
Table = readtable(filename, opts);
Table = tableConvert(Table,Info);
end

function tCon = tableConvert(table,Info)
vScale = 5/(2^10-1);        % ADC sensitivity
IScale = vScale / 0.020;    % Isensitivity = 20mv/A

V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;

tCon = table;


tCon.PWMDead = pwm2duty(tCon.PWM,40,225);
tCon.PWM=tCon.PWM/255;
tCon.V2_read = (table.V2_read - V2Mean) * vScale;
tCon.Isense_read = (table.Isense_read - IsenseMean) * IScale;
tCon.e = tCon.e * vScale;

d = designfilt('lowpassfir', ...
    'PassbandFrequency',0.01,'StopbandFrequency',0.4, ...
    'PassbandRipple',1,'StopbandAttenuation',60, ...
    'DesignMethod','equiripple');
tCon.V2_readFilter = filtfilt(d,tCon.V2_read);
tCon.Isense_readFilter = filtfilt(d,tCon.Isense_read);
tCon.eFilter = filtfilt(d,tCon.e);

end

function u = pwm2duty(pwmList, downDead, upDead)
u = zeros(length(pwmList),1);
for i = 1:length(pwmList)
    pwm = pwmList(i);
    if(abs(pwm)>=upDead)
        u(i)=sign(pwm);
        continue
    end
    if(abs(pwm)<=downDead)
        u(i) = 0;
        continue
    end
    if(pwm>0)
        u(i) = map(pwm,downDead,upDead,0,1);
        continue
    else
        u(i) = -map(-pwm,downDead,upDead,0,1);
        continue
    end
end

end

function u = map (var, varMin, varMax, outMin , outMax)
u = ((var-varMin)/(varMax-varMin));
u = (u * outMax) + ((1-u)*outMin);
end