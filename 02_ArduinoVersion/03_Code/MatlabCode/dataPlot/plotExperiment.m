clear variables

% home_1ms = importExperiment("experiment/01-1ms-Home.dat");
[home_500us, V2Mean, IsenseMean]= importExperiment("experiment/05-500Us-Home.dat");
% [home_500us, V2Mean, IsenseMean]= importExperiment("capture.dat");

home_500usRescale = tableRescale(home_500us,V2Mean, IsenseMean);
% figure(1)
% clf
% plotTable(home_1ms, 1E-3);

figure(2)
plotTable(home_500usRescale, 500E-6, "05-500Us-Home");

function plotTable(table, dt, name)
% dt = Sample time in second

[len , ~ ] = size(table);

t = [0:len-1]' * dt;

clf
grid on
hold on
plot(t,table.PWM)
plot(t,table.V2_read)
plot(t,table.Isense_read)
legend("PWM", "V2-read", "Isense-read")
title("Experiment Capture SampleTime: " + mat2str(dt * 1000) + "ms")
xlabel("time [s]")

% Figure Export and Save
savePath = "./img/";
[status,msg] = mkdir(savePath);
set(gcf, 'PaperUnits', 'normalized')
set(gcf, 'PaperPosition', [0 0 1 1])
set(gcf,'PaperOrientation','landscape');
saveas(gcf,savePath + name + '.png')
saveas(gcf,savePath + name + '.pdf')
end


function tableRescale = tableRescale(table,V2Mean, IsenseMean)
vScale = 5/(2^10-1);    % ADC sensitivity
IScale = vScale / 0.020;    % Isensitivity = 20mv/A    

tableRescale = table;
tableRescale.PWM = table.PWM/255;
tableRescale.V2_read = (table.V2_read - V2Mean) * vScale;
tableRescale.Isense_read = (table.Isense_read - IsenseMean) * IScale;

end