clear variables
clc


[experiment,  Info]= importExperiment("ExtimateExperiment/T2-RapidShot.txt");

[len , ~ ] = size(experiment);
t = [0:len-1]' * Info.dt;
s = tf('s');

PrimaryFrameData = iddata(experiment.Isense_read, experiment.PWMDead, Info.dt);


% ########### Primary Rise Estimation ###########
PrimaryRiseFrameData = iddata(experiment.Isense_read(1:1300), experiment.PWMDead(1:1300), Info.dt);

% SYS = tfest(DATA, NP, NZ, IODELAY)
tfPrimaryRise = tfest(PrimaryRiseFrameData, 2, 0, NaN)
P = pole(tfPrimaryRise)
num = tfPrimaryRise.num;
den = tfPrimaryRise.den;

% tfPrimaryRise = minreal(8.9/((s/(65.1478 +38.4911i)+1)*(s/(65.1478 +38.4911i)+1)))
% tfPrimaryRise = minreal(8.9/((s/(65.1478 +38.4911i)+1)*(s/(65.1478 +38.4911i)+1)))

yPrimaryRise = lsim(tfPrimaryRise,experiment.PWMDead(1:1300),t(1:1300));
eyPrimaryRise = 100*(1-goodnessOfFit(experiment.Isense_read(1:1300), yPrimaryRise,'NRMSE'));
fprintf("eyPrimaryRise estimation Fit:\t%.4f %%\n", eyPrimaryRise)

% ########### Primary Fall Estimation ###########

tfPrimaryFall = minreal(8.9/((s/3000+1)*(s/500+1)))


yPrimaryFall = lsim(tfPrimaryFall,experiment.PWMDead(1780:2210),t(1780:2210),[0,8.92]);
eyPrimaryFall = 100*(1-goodnessOfFit(experiment.Isense_read(1780:2210), yPrimaryFall,'NRMSE'));
fprintf("eyPrimaryFall estimation Fit:\t%.4f %%\n", eyPrimaryFall)


% ########### Primary Simulation ###########
yPrimaryRise = lsim(tfPrimaryRise,experiment.PWMDead,t);
yPrimaryFall= lsim(tfPrimaryFall,experiment.PWMDead,t);


plotTable(experiment,Info, [yPrimaryRise, yPrimaryFall], {'PrimaryRiseEst', 'PrimaryFallEst'});
xlim([0.2,1.2])

tfPrimaryRise = tf(tfPrimaryRise);
tfPrimaryFall = tf(tfPrimaryFall);

disp(tfPrimaryRise)
disp(tfPrimaryFall)
save("primaryEst.mat", "tfPrimaryRise","tfPrimaryFall");
