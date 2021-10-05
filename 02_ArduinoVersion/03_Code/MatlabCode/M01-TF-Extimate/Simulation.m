clear variables
clc

[experiment,  Info]= importExperiment("ExtimateExperiment/E01-EstimateShot.txt");
load('TransformerSysEst.mat')

[len , ~ ] = size(experiment);
t = [0:len-1]' * Info.dt;

yfPWM_I1 = lsim(fPWM_I1,experiment.PWMDead,t);
yfPWM_V2 = lsim(fPWM_V2,experiment.PWMDead,t);

plotTable(experiment,Info,[0, inf], [-inf, inf], [yfPWM_I1,yfPWM_V2], {'yfPWM_I1','ytfEst'} , 'manual')
interestPlot(experiment,Info,[yfPWM_I1,yfPWM_V2], {'yfPWM_I1','ytfEst'} , 'manual')

function interestPlot(experiment,Info, ySim, ySimLegend , nameSave)
figure()
plotTable(experiment,Info,[0.05, 0.6], [-8.5, 8.5], ySim, ySimLegend , nameSave + "-1")

figure()
plotTable(experiment,Info,[1.35, 2.15], [-8.5, 8.5], ySim, ySimLegend , nameSave + "-2")

figure()
plotTable(experiment,Info,[5.78, 6.6], [-8.5, 8.5], ySim, ySimLegend , nameSave + "-3")
end