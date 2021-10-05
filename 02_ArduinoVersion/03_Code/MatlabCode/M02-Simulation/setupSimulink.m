clear variable
clc

load('TransformerSysEst.mat')

[numP,denP] = tfdata(fPWM_V2);
numP = cell2mat(numP);
denP = cell2mat(denP);

Kp=0.8;
K1=100;
K2=500;




