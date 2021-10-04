clear variables
clc

[experiment,  Info]= importExperiment("ExtimateExperiment/E01-EstimateShot.txt");
load('primaryEst.mat')

[len , ~ ] = size(experiment);
t = [0:len-1]' * Info.dt;

yInPriRise = lsim(tfInPriRise,experiment.PWMDead,t);
yInPriFall = lsim(tfInPriFall,experiment.PWMDead,t);

yfPWM_I1 = lsim(fPWM_I1,experiment.PWMDead,t);
ytfEst = lsim(tfEst,experiment.PWMDead,t);



% plotTable(experiment,Info, [yInPriRise, yInPriFall], {'yPrimaryRise','yPrimaryFall'});
plotTable(experiment,Info, [yfPWM_I1/10,ytfEst], {'yfPWM_I1','ytfEst'});
