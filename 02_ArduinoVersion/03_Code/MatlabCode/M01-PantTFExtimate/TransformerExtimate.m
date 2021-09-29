clear variables
clc


% [experiment, Info]= importExperiment("ExtimateExperiment/T2-RapidShot.txt");
[experiment, Info]= importExperiment("ExtimateExperiment/T1-Triangolare.txt");
experimentRescale = tableRescale(experiment, Info);

% DAT = iddata(Y,U,Ts)
ExperimentFrameData = iddata(experimentRescale.V2_read, experimentRescale.PWM, Info.dt);

% SYS = tfest(DATA, NP, NZ, IODELAY)
tfTansformerEst = tfest(ExperimentFrameData, 2, 1, NaN)
% [num, den] = tfdata(tfTansformerEst);
num = tfTansformerEst.num;
den = tfTansformerEst.den;


[len , ~ ] = size(experimentRescale);
t = [0:len-1]' * Info.dt;

tfTansformer = tf(num, den);

num(2) = 0;
tfTansformerRmZero = tf(num, den);

ySim = lsim(tfTansformer,experimentRescale.PWM,t);
ySimRmZero = lsim(tfTansformerRmZero,experimentRescale.PWM,t);


eySim = 100*(1-goodnessOfFit(experimentRescale.V2_read, ySim,'NRMSE'));
eySimOnlyZero = 100*(1-goodnessOfFit(experimentRescale.V2_read, ySimRmZero,'NRMSE'));

fprintf("Simulation with tfest Coeffitient generate a:\n\t%.4f %%\tFit to estimation data\n", eySim)
fprintf("Simulation with tfest Coeffitient removing the costant term in numerator generate a:\n\t%.4f %%\tFit to estimation data\n", eySimOnlyZero)

plotTable(experimentRescale,Info, [ySim, ySimRmZero], {'tfEst','tfEst-zero'});

% #############################
% #      Functions Block      #
% #############################

function tableRescale = tableRescale(table,Info)
vScale = 5/(2^10-1);        % ADC sensitivity
IScale = vScale / 0.020;    % Isensitivity = 20mv/A

V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;
vRef = Info.V2Ref_set;

tableRescale = table;
for i = 1 : length(tableRescale.PWM)
    tableRescale.PWM(i) = pwm2duty(table.PWM(i),41,220);
end
tableRescale.V2_read = (table.V2_read - V2Mean) * vScale;
tableRescale.Isense_read = (table.Isense_read - IsenseMean) * IScale;
tableRescale.error = (tableRescale.V2_read - vRef*vScale);
end

function u = pwm2duty(pwm, downDead, upDead)
u = 0;
if(abs(pwm)>=upDead)
    u=sign(pwm);
    return
end
if(abs(pwm)<=downDead)
    u = 0;
    return
end
if(pwm>0)
    u = map(pwm,downDead,upDead,0,1);
    return
else
    u = -map(-pwm,downDead,upDead,0,1);
    return
end

end

function u = map (var, varMin, varMax, outMin , outMax)
u = ((var-varMin)/(varMax-varMin));
u = (u * outMax) + ((1-u)*outMin);
end

function plotTable(table, Info, ySim, ySimLegend)
% ySimLegend have to be a cel array {'Jan','Feb','Mar'}
V2Mean = Info.V2_mean;
IsenseMean = Info.Isense_mean;
dt = Info.dt;
vRef = Info.V2Ref_set;

name = Info.Properties.Description;
[len , ~ ] = size(table);

t = [0:len-1]' * dt;

clf
grid on
hold on
plot(t,table.PWM)
plot(t,table.V2_read)
plot(t,table.Isense_read)
for y = ySim
    plot(t,y);
end
legNorm = {'PWM%', 'V_{secondary}', 'I_{primary}'};
legend(cat(2,legNorm,ySimLegend));

title("Experiment: "+name+ "   SampleTime: " + mat2str(dt * 1000000) + "µs")
xlabel("time [s]")

% Figure Export and Save
savePath = "./img/";
[~,~] = mkdir(savePath);
set(gcf, 'PaperUnits', 'normalized')
set(gcf, 'PaperPosition', [0 0 1 1])
set(gcf,'PaperOrientation','landscape');
saveas(gcf,savePath + name + '.png')
saveas(gcf,savePath + name + '.pdf')
end