clear variables
clc


[experiment,  Info]= importExperiment("ExtimateExperiment/T2-RapidShot.txt");

[len , ~ ] = size(experiment);
t = [0:len-1]' * Info.dt;
s = tf('s');
% DAT = iddata(Y,U,Ts) 
PrimaryFrameData = iddata(experiment.Isense_read, experiment.PWMDead, Info.dt);


% ########### Input-Primary Rise Estimation ###########
% DAT = iddata(Y,U,Ts) 
InPriRiseFrameData = iddata(experiment.Isense_read(1:1200), experiment.PWMDead(1:1200), Info.dt);

% SYS = tfest(DATA, NP, NZ, IODELAY)
tfInPriRise = tfest(InPriRiseFrameData, 2, 0, NaN)
P_InPri = pole(tfInPriRise)
num = tfInPriRise.num;
den = tfInPriRise.den;

% tfPrimaryRise = minreal(8.9/((s/(65.1478 +38.4911i)+1)*(s/(65.1478 +38.4911i)+1)))
% tfPrimaryRise = minreal(8.9/((s/(65.1478 +38.4911i)+1)*(s/(65.1478 +38.4911i)+1)))

yInPriRise = lsim(tfInPriRise,experiment.PWMDead(1:1200),t(1:1200));
eyInPriRise = 100*(1-goodnessOfFit(experiment.Isense_read(1:1200), yInPriRise,'NRMSE'));

fprintf("###########################################\n")
fprintf("eyInPriRise estimation Fit:\t%.4f %%\n", eyInPriRise)
fprintf("###########################################\n")

% ########### Primary - Secondary Rise Estimation ###########
% DAT = iddata(Y,U,Ts) 
% PriSecRiseFrameData = iddata(experiment.V2_read(1:1200), yInPriRise, Info.dt);
% tfPriSec = tfest(PriSecRiseFrameData, 1, 1, NaN)
% P_PriSec = pole(tfPriSec)
% num = tfPriSec.num;
% den = tfPriSec.den;

tfPriSec = minreal((0.017*s)/(s/1000+1));

yPriSecRise = lsim(tfPriSec,yInPriRise,t(1:1200));
eyPriSecRise = 100*(1-goodnessOfFit(experiment.V2_readFilter(1:1200), yPriSecRise,'NRMSE'));

fprintf("###########################################\n")
fprintf("eyPriSecRise estimation Fit:\t%.4f %%\n", eyPriSecRise)
fprintf("###########################################\n")



% ########### Input-Primary Fall Estimation ###########

tfInPriFall = minreal(8.9/((s/2000+1)*(s/500+1)))


yInPriFall = lsim(tfInPriFall,experiment.PWMDead(1780:2210),t(1780:2210),[0,8.92]);
eyInPriFall = 100*(1-goodnessOfFit(experiment.Isense_read(1780:2210), yInPriFall,'NRMSE'));

fprintf("###########################################\n")
fprintf("eyInPriFall estimation Fit:\t%.4f %%\n", eyInPriFall)
fprintf("###########################################\n")









% ########### Simulation ###########
yInPriRise = lsim(tfInPriRise,experiment.PWMDead,t);
yPriSecRise = lsim(tfPriSec,yInPriRise,t);

yInPriFall= lsim(tfInPriFall,experiment.PWMDead,t);


plotTable(experiment,Info, [yInPriRise, yPriSecRise , yInPriFall], {'PrimaryRiseEst', 'SecondaryRiseEst', 'PrimaryFallEst'});
xlim([0.2,1.2])

tfInPriRise = tf(tfInPriRise);
tfInPriFall = tf(tfInPriFall);










% disp(tfPrimaryRise)
% disp(tfPrimaryFall)
save("primaryEst.mat", "tfInPriRise","tfInPriFall");
