
figure(1)
% plotSim(out.cBaseSim,[0,inf],[-0.1 1.1], "cBaseSim")
plotSim(out.cBaseSim,[0,2],[-0.1 1.1],"Kp=10" ,"cBaseSim")

name = "K_p="+ Kp + " K_1=" + K1 + " K_2=" + K2;
% Kp
% K1
% K2
figure(2)
plotSim(out.cSim,[0,2],[-1.1 1.1],name, "cSim-" + strrep(name," ","-"))


function plotSim(out ,Xlim, YLim,titlePlot , nameSave)
t = out.time;
u = out.signals.values(:,1);
v2 = out.signals.values(:,2);
I1 = out.signals.values(:,3);
e = out.signals.values(:,4);
clf
grid on
hold on
plot(t,u,'LineWidth',2)
plot(t,v2,'LineWidth',2)
plot(t,I1/10,'LineWidth',2)
plot(t,e*10,'LineWidth',1)

legend('PWM%','V_{secondary_{NORM}}','I_{primary} / 10', 'e \cdot 10');
title(titlePlot)
xlabel("time [s]")
xlim(Xlim);
ylim(YLim);

% Figure Export and Save
savePath = "./img/";
[~,~] = mkdir(savePath);
set(gcf, 'PaperUnits', 'normalized')
set(gcf, 'PaperPosition', [0 0 1 1])
set(gcf,'PaperOrientation','landscape');
saveas(gcf,savePath + "Simulation-" + nameSave + '.png')
saveas(gcf,savePath + "Simulation-" + nameSave + '.pdf')
end