clear variables
clc


[experiment,  Info]= importExperiment("ExtimateExperiment/T2-RapidShot.txt");

[len , ~ ] = size(experiment);
t = [0:len-1]' * Info.dt;
s = tf('s');
% ##############################################
% ########### PWM-I2 Auto Estimation ###########
% ##############################################
% DAT = iddata(Y,U,Ts) 
PrimaryFrameData = iddata(experiment.Isense_read, experiment.PWMDead, Info.dt);

% ########### PWM-I1 Estimation ###########
% DAT = iddata(Y,U,Ts) 
InPriRiseFrameData = iddata(experiment.Isense_read(1:1200), experiment.PWMDead(1:1200), Info.dt);

% SYS = tfest(DATA, NP, NZ, IODELAY)
tfExtPWM_I1 = tfest(InPriRiseFrameData, 1, 0, NaN)
% lsim(SYS,U,T)
yExtPWM_I1Fit = lsim(tfExtPWM_I1,experiment.PWMDead(1:1200),t(1:1200));

% ########### Primary - Secondary Rise Estimation ###########
% DAT = iddata(Y,U,Ts) 
PriSecRiseFrameData = iddata(experiment.V2_read(1:1200), yExtPWM_I1Fit, Info.dt);
tfExtI1_V2 = tfest(PriSecRiseFrameData, 1, 1, NaN)

% ########### Simulation ###########
yExtPWM_I1 = lsim(tfExtPWM_I1,experiment.PWMDead,t);
yExtI1_V2 = lsim(tfExtI1_V2,yExtPWM_I1,t);

figure(1)
plotTable(experiment,Info,[0.2,1.2],[-inf,inf], [yExtPWM_I1, yExtI1_V2], {'yExtPWM-I1', 'yExtI1-V2'}, 'autoExt');

% ##############################################
% ########## PWM-I2 Manual Estimation ##########
% ##############################################
fPWM_I1 = tf(8.4,[0.022 1]);
yfPWM_I1 = lsim(fPWM_I1,experiment.PWMDead,t);

fI1_V2 = tf([0.0085 0],[1/2800 1]);
yfI1_V2 = lsim(fI1_V2,yfPWM_I1,t);

figure(2)
plotTable(experiment,Info,[0.2,1.2],[-inf,inf], [yfPWM_I1, yfI1_V2], {'yfPWM-I1','yfI1_V2'},'manualExt');

% ##############################################
% ############### Save estimation ##############
% ##############################################

tfExtPWM_I1 = minreal(tf(tfExtPWM_I1));
tfExtI1_V2 = minreal(tf(tfExtI1_V2));
tfEstAuto = minreal(tfExtPWM_I1*tfExtI1_V2);

fPWM_V2 = minreal(fPWM_I1*fI1_V2);


save("TransformerSysEst.mat", "tfExtPWM_I1","tfExtI1_V2","tfEstAuto","fPWM_I1", "fI1_V2", "fPWM_V2");
%% Latex Export
tf2latex(tfExtPWM_I1, "\hat{P}_{p_{wm} I_1}")
tf2latex(tfExtI1_V2,"\hat{P}_{I_1 V_2}")
tf2latex(tfEstAuto, "\hat{P}_{p_{wm} V_2}")

tf2latex(fPWM_I1, "\tilde{P}_{p_{wm} I_1}")
tf2latex(fI1_V2,"\tilde{P}_{I_1 V_2}")
tf2latex(fPWM_V2, "\tilde{P}_{p_{wm} V_2}")
