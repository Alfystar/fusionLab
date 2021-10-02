function plotTable(table, Info, ySim, ySimLegend)
% ySimLegend have to be a cel array {'Jan','Feb','Mar'}
V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;
vRef = Info.V2Ref_set;

name = Info.Properties.Description;
[len , ~ ] = size(table);

t = [0:len-1]' * dt;

clf
grid on
hold on
plot(t,table.PWMDead)
% plot(t,table.PWM)
plot(t,table.V2_read)
% plot(t,table.V2_readFilter)
plot(t,table.Isense_read)
for y = ySim
    plot(t,y);
end
legNorm = {'PWM%-DeadFilter','V_{secondary}','I_{primary}'};

% legNorm = {'PWM%-DeadFilter', 'PWM%-Real', 'V_{secondary}', 'V_{secondaryFilter}','I_{primary}'};
legend(cat(2,legNorm,ySimLegend));

title("Experiment: "+name+ "   SampleTime: " + mat2str(dt * 1000000) + "µs")
xlabel("time [s]")

% Figure Export and Save
savePath = "./img/";
[~,~] = mkdir(savePath);
set(gcf, 'PaperUnits', 'normalized')
set(gcf, 'PaperPosition', [0 0 1 1])
set(gcf,'PaperOrientation','landscape');
saveas(gcf,savePath + name + '.png')
saveas(gcf,savePath + name + '.pdf')
end