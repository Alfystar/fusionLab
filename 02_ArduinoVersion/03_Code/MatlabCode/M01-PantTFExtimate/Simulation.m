clear variables
clc

[experiment,  Info]= importExperiment("ExtimateExperiment/E01-EstimateShot.txt");
load('primaryEst.mat')

[len , ~ ] = size(experiment);
t = [0:len-1]' * Info.dt;

yInPriRise = lsim(tfInPriRise,experiment.PWMDead,t);
yInPriFall = lsim(tfInPriFall,experiment.PWMDead,t);

plotTable(experiment,Info, [yInPriRise, yInPriFall], {'yPrimaryRise','yPrimaryFall'});
