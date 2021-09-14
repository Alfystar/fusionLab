clear variables
close all
experiment("experiment/capture.txt",1, 0.5)

% experiment("experiment/06-RapidShot-EX354T.txt",0,3.5)
% experiment("experiment/11-triangle-Trasformatore.txt",0,0.25)
% experiment("experiment/12-rapidShot-Trasformatore.txt",0, 1.5)
% experiment("experiment/13-rapidShot-30Khz.txt",0,1.5)
% experiment("experiment/14-rapidShot-30Khz-Condensatore.txt",0,1.5)
% experiment("experiment/15-rapidShot-30Khz-Condensatore-24V.txt",0,3.5)
% 
% experiment("experiment/16-rapidShot-30Khz-Condensatore-UpDeadZone.txt",0,1.5)
% experiment("experiment/17-triangle-Trasformatore.txt",0,1)
% experiment("experiment/18-triangle-slow-Trasformatore.txt")
% experiment("experiment/19-triangle-mean-Trasformatore.txt")
% 
% experiment("experiment/20-triangle-mean-Trasformatore-deadzoneDelete.txt",0,1)
% experiment("experiment/21-rapidShot-30Khz-Condensatore-deadzoneDelete.txt",0,0.5)
% experiment("experiment/22-rapidShot-30Khz-Condensatore-deadzoneDelete-RapidRamp.txt",0,0.5)
% experiment("experiment/23-rapidShot-30Khz-Condensatore-deadzoneDelete-SlowRamp.txt",0)
% experiment("experiment/24-Ctrl-prop-trasformatore.txt",1,1.2)

%% Functions block

function experiment(filepath, vRef, tLim)
if nargin < 2
    vRef = 0;
end
if nargin < 3
    tLim = inf;
end

[home_500us, Info]= importExperiment(filepath);
home_500usRescale = tableRescale(home_500us, Info, vRef);
figure()
plotTable(home_500usRescale, Info, tLim);
end

function plotTable(table, Info, tLim)
V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;
name = Info.Properties.Description;
[len , ~ ] = size(table);

t = [0:len-1]' * dt;

clf
grid on
hold on
plot(t,table.PWM)
plot(t,table.V2_read)
plot(t,table.Isense_read)
plot(t,table.error)
legend("PWM%", "V_{secondary}", "I_{primary}", "e")
title("Experiment: "+name+ "   SampleTime: " + mat2str(dt * 1000000) + "µs")
xlabel("time [s]")
xlim([0 tLim])

% Figure Export and Save
savePath = "./img/";
[~,~] = mkdir(savePath);
set(gcf, 'PaperUnits', 'normalized')
set(gcf, 'PaperPosition', [0 0 1 1])
set(gcf,'PaperOrientation','landscape');
saveas(gcf,savePath + name + '.png')
saveas(gcf,savePath + name + '.pdf')
end


function tableRescale = tableRescale(table,Info,vRef)
V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;

vScale = 5/(2^10-1);        % ADC sensitivity
IScale = vScale / 0.020;    % Isensitivity = 20mv/A    

tableRescale = table;
tableRescale.PWM = table.PWM/255;
tableRescale.V2_read = (table.V2_read - V2Mean) * vScale;
tableRescale.Isense_read = (table.Isense_read - IsenseMean) * IScale;
tableRescale.error = tableRescale.V2_read - vRef;
end