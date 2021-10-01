clear variables
clc


% [experiment,  Info]= importExperiment("ExtimateExperiment/T2-RapidShot.txt");
[experiment,  Info]= importExperiment("ExtimateExperiment/T1-Triangolare.txt");
% [experiment,  Info]= importExperiment("ExtimateExperiment/T3-ControllBackup-100Ohm.txt");
% [experiment,  Info]= importExperiment("ExtimateExperiment/E01-EstimateShot.txt");
% [experiment,  Info]= importExperiment("ExtimateExperiment/E02-EstimateShotHalfFlip.txt");
% DAT = iddata(Y,U,Ts)
ExperimentFrameData = iddata(experiment.V2_read, experiment.PWMDead, Info.dt);

% SYS = tfest(DATA, NP, NZ, IODELAY)
tfTansformerEst = tfest(ExperimentFrameData, 2, 1, NaN)
% [num, den] = tfdata(tfTansformerEst);
num = tfTansformerEst.num;
den = tfTansformerEst.den;


[len , ~ ] = size(experiment);
t = [0:len-1]' * Info.dt;

tfTansformer = tf(num, den);

num(2) = 0;
tfTansformerRmZero = tf(num, den);

ySim = lsim(tfTansformer,experiment.PWMDead,t);
ySimRmZero = lsim(tfTansformerRmZero,experiment.PWMDead,t);



eySim = 100*(1-goodnessOfFit(experiment.V2_readFilter, ySim,'NRMSE'));
eySimOnlyZero = 100*(1-goodnessOfFit(experiment.V2_readFilter, ySimRmZero,'NRMSE'));

fprintf("Simulation with tfest Coeffitient generate a:\n\t%.4f %%\tFit to estimation data\n", eySim)
fprintf("Simulation with tfest Coeffitient removing the costant term in numerator generate a:\n\t%.4f %%\tFit to estimation data\n", eySimOnlyZero)
% plotTable(experiment,Info, [ySim, ySimRmZero], {'tfEst','tfEst-zero'});

%% 


%% Manual Estimation
tfPrimario = tf([100000], [1000, 10000]); 
yManualSimPrimario = lsim(tfPrimario,experiment.PWMDead,t);


plotTable(experiment,Info, yManualSimPrimario, {'ManualPrimario'});
xlim([0,0.500])
% #############################
% #      Functions Block      #
% #############################

