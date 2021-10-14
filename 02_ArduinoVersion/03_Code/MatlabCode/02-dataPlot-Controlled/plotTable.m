function plotTable(table, Info,xLim, yLim)
% ySimLegend have to be a cel array {'Jan','Feb','Mar'}
V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;

name = Info.Properties.Description;
[len , ~ ] = size(table);

t = [0:len-1]' * dt;

clf
% subplot(1,2,1)
% grid on
% hold on
% plot(t,table.PWMDead*10,'LineWidth',2)
% % plot(t,table.PWM)
% plot(t,table.V2_read*10,'LineWidth',2)
% % plot(t,table.V2_readFilter)
% plot(t,table.Isense_read,'LineWidth',2)
% plot(t,table.e * 10)
% plot(t,(table.V2_read + table.e) * 10, '--')
% 
% legend("PWM% x10", "V_{secondary} x10", "I_{primary}", "e_{filter} x10","V_{2_{ref}}")
% 
% title("Experiment: "+name+ "   SampleTime: " + mat2str(dt * 1000000) + "µs")
% xlabel("time [s]")
% ax1 = gca;
% 
% subplot(1,2,2)
grid on
hold on
plot(t,table.PWMDead*10,'LineWidth',2)
% plot(t,table.PWM)
plot(t,table.V2_readFilter*10,'LineWidth',2)
% plot(t,table.V2_readFilter)
plot(t,table.Isense_readFilter,'LineWidth',2)
plot(t,table.eFilter * 10)
plot(t,(table.V2_readFilter + table.eFilter) * 10, '-.','LineWidth',1.5)

legend("PWM% x10", "V_{secondary_{filter}} x10", "I_{primary_{filter}}", "e_{filter} x10","V_{2_{ref}}")

title("Experiment: "+name+ "   SampleTime: " + mat2str(dt * 1000000) + "µs")
xlabel("time [s]")
% ax2 = gca;
% 
% linkaxes([ax1 ax2],'xy')
% 
% xlim(gca, xLim)
% ylim(gca, yLim)

xlim(xLim)
ylim(yLim)

% Figure Export and Save
savePath = "./img/";
[~,~] = mkdir(savePath);
set(gcf, 'PaperUnits', 'normalized')
set(gcf, 'PaperPosition', [0 0 1 1])
set(gcf,'PaperOrientation','landscape');
saveas(gcf,savePath + name + '.png')
saveas(gcf,savePath + name + '.pdf')
end