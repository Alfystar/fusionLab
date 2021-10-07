clear variables
close all

% experiment("experimentControlled/capture.txt")


experiment("experimentControlled/01-RandomCapture.txt",[2900,4800],[-11,11])
experiment("experimentControlled/02-Bi-FrontCapture.txt",[450,1200],[-11,11])
experiment("experimentControlled/03-PosNeg-capture.txt",[450,1150],[-11,11])

% TODO: Usando imporExperimentOriginal, modificare importExperiment creato
% per la simulazione (per avere le cose allineate)
% E modificare PlotTable per usare lo stesso stile di visualizzazione delle
% simulazioni

%% Functions block

function experiment(filepath, tLim, yLim)
if nargin < 2
    tLim = [0,inf];
end
if nargin < 3
    yLim = [-inf,inf];
end
if(length(tLim)==1)
    tLim = [0,tLim];
end

if ~isfile(filepath)
    return % File not exists.
end

[home_500us, Info]= importExperiment(filepath);
home_500usRescale = tableRescale(home_500us, Info);
figure()
plotTable(home_500usRescale, Info, tLim, yLim);
end

function plotTable(table, Info, tLim, yLim)
V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;

name = Info.Properties.Description;
[len , ~ ] = size(table);

t = [0:len-1]' * dt * 1000;


d = designfilt('lowpassfir', ...
    'PassbandFrequency',0.15,'StopbandFrequency',0.2, ...
    'PassbandRipple',1,'StopbandAttenuation',60, ...
    'DesignMethod','equiripple');

clf
% First SubPlot
subplot(2,1,1)
grid on
hold on
plot(t,table.PWM)
plot(t,table.V2_read)
plot(t,table.Isense_read)
plot(t,table.error)
legend("PWM%", "V_{secondary}", "I_{primary}", "e")
title("Experiment: "+name+ "   SampleTime: " + mat2str(dt * 1000000) + "µs")
xlabel("time [ms]")

ax1 = gca;
% Second SubPlot
subplot(2,1,2)

grid on
hold on
plot(t,table.PWM)

V2_readPlotFilt = filtfilt(d,table.V2_read);
plot(t,V2_readPlotFilt)

iSensePlotFilt = filtfilt(d,table.Isense_read);
plot(t,iSensePlotFilt)

errPlotFilt = filtfilt(d,table.error);
plot(t,errPlotFilt)
legend("PWM%", "V_{secondary}", "I_{primary}", "e")
title("Data Filtering")
xlabel("time [ms]")
ax2 = gca;

linkaxes([ax1 ax2],'xy')

xlim(gca, tLim)
ylim(gca, yLim)


% Figure Export and Save
savePath = "./img/";
[~,~] = mkdir(savePath);
set(gcf, 'PaperUnits', 'normalized')
set(gcf, 'PaperPosition', [0 0 1 1])
set(gcf,'PaperOrientation','landscape');
saveas(gcf,savePath + name + '.png')
saveas(gcf,savePath + name + '.pdf')
end


function tableRescale = tableRescale(table,Info)
vScale = 5/(2^10-1);        % ADC sensitivity
IScale = vScale / 0.020;    % Isensitivity = 20mv/A

V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;

zoom = 10;

tableRescale = table;
tableRescale.PWM = table.PWM/255 * zoom;
tableRescale.V2_read = (table.V2_read - V2Mean) * vScale;
tableRescale.Isense_read = (table.Isense_read - IsenseMean) * IScale;
tableRescale.error = table.e * vScale;
end