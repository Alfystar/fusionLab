clear variables
clc

[experiment,  Info]= importExperiment("ExtimateExperiment/E01-EstimateShot.txt");
load('primaryEst.mat')

[len , ~ ] = size(experiment);
t = [0:len-1]' * Info.dt;

yPrimaryRise = lsim(tfPrimaryRise,experiment.PWMDead,t);
yPrimaryFall = lsim(tfPrimaryFall,experiment.PWMDead,t);

plotTable(experiment,Info, [yPrimaryRise, yPrimaryFall], {'yPrimaryRise','yPrimaryFall'});
