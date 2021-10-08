function plotTable(table, Info,xLim, yLim)
% ySimLegend have to be a cel array {'Jan','Feb','Mar'}
V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;

name = Info.Properties.Description;
[len , ~ ] = size(table);

t = [0:len-1]' * dt;

clf
grid on
hold on
plot(t,table.PWMDead*10,'LineWidth',2)
% plot(t,table.PWM)
plot(t,table.V2_readFilter,'LineWidth',2)
% plot(t,table.V2_readFilter)
plot(t,table.Isense_readFilter,'LineWidth',2)
plot(t,table.eFilter * 10)

legend("PWM% x10", "V_{secondary_{filter}}", "I_{primary_{filter}}", "e_{filter} x10")

title("Experiment: "+name+ "   SampleTime: " + mat2str(dt * 1000000) + "µs")
xlabel("time [s]")
xlim(xLim);
ylim(yLim);

% Figure Export and Save
savePath = "./img/";
[~,~] = mkdir(savePath);
set(gcf, 'PaperUnits', 'normalized')
set(gcf, 'PaperPosition', [0 0 1 1])
set(gcf,'PaperOrientation','landscape');
saveas(gcf,savePath + name + '.png')
saveas(gcf,savePath + name + '.pdf')
end