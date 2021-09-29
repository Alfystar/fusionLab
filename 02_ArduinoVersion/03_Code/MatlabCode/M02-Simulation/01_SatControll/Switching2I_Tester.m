%% TESTING the double integrator
clc
clear all
close all

%% Simulation parameters
Ti = 0;
Tf = 2.5;
Ts = 0.001;
x0 = [0,0];

%% PLANT and CONTROLLER
omega_n = 8;
zita = 1;
P = tf(omega_n^2,[1 2*zita*omega_n omega_n^2]);
Pz = c2d(P,Ts);
Pss = ss(Pz)

C =10; 

%%

time = Ti:Ts:Tf;
Ref_signal = zeros(1,length(time)); 
RefTop = 500E3;
for k=2:length(time)
    if(time(k)>=0.0 && time(k)<=0.1)
        Ref_signal(k) = 0.5*RefTop*(time(k)/0.1);
    end
    if(time(k)>=0.1 && time(k)<=1)
        Ref_signal(k) = 0.5*RefTop+0.5*RefTop*((time(k)-0.1)/0.9);
    end
    if(time(k)>=1 && time(k)<=1.3)
        Ref_signal(k) = RefTop;
    end
    
    if(time(k)>=1.3 && time(k)<=2)
        Ref_signal(k) = RefTop*(1-(time(k)-1.3)/0.7);
    end
    if(time(k)>=2.0 )
        Ref_signal(k) = 0;
    end   
end
Disturbance = 0*ones(1,length(time));


%% double integrator parameters
sampling_time = Ts;
activate = 0;
startTimeDoubleSwitchedIntegrator = 0.0;
endTimeDoubleSwitchedIntegrator = 4.5;
sigma_x1 = 1E6;                       %amplitude saturation for x1
sigma_x2 = 1E6;                   %amplitude saturation for x2
sigma_dx1 = 1E8;                    %derivative saturation for x1
sigma_dx2 = 1.0E14;                  %derivative saturation for x2
sigma_discharge = 5.0E4;           %derivative saturation for x1 exponential discharge
sigma_de = 1.5E6;                   %1E6 d_error amplitude threshold
sigma_e  = 1E5;                     %150E3 error amplitude threshold
k12_bar  = 10;                    %Gain of the first integrator, set to zero for deactivate it
k2e_bar  = 60;                    %Gain of the first integrator
epsilonDischarge = 20.0;            %discharge the x1 integrator exponentially: x1 = epsilonDischarge*x1 
threshold_dusignx1 = 1E1;    % d_u1*sign(x1) <= -threshold_dusignx1  to decide if switching off
discharge = 0;               % discharge: if discharge is not zero the controller state is discharged.
dwell_time = 0.050;         % dwell_time: do not activate again the controller before the dwell_time is elapsed
error_reset = 10E1;        % erorr_reset: set x_2 = 0 if x2*sign(error)<= - error_reset (select 0 if you do not whant this feature)


%derivator pidout
pidout.deltaT = Ts;
pidout.c = 2;
pidout.d = 10;
pidout.mediana = 0;
pidout.reset = 0;
pidout.filterTau = 0.99; %0.75 filter on u1

%derivator iperror
iplaerror.deltaT = Ts;
iplaerror.c = 2;
iplaerror.d = 10;
iplaerror.mediana = 0;
iplaerror.reset = 0;
iplaerror.filterTau = 0.99; %0.75 filter on Ipla

Xp = zeros(2,length(time));
Xp(:,1) = [x0(1);x0(2)];
u1 = zeros(1,length(time));
u2 = zeros(1,length(time));
d_input = zeros(1,length(time));
d_error = zeros(1,length(time));
error = zeros(1,length(time));
x_double = zeros(2,length(time));
rho = zeros(1,length(time));

tic
for k=2:length(time)
    
    error(k-1) = Ref_signal(k-1)-(Pss.c*Xp(:,k-1)+ Pss.d*u2(k-1));
    u1(k-1) = C*error(k-1);
    
    d_error(k-1) = IterativePseudoDerivative_xprova1(Ts,error(k-1),iplaerror.c,iplaerror.d,iplaerror.mediana,iplaerror.reset);
    d_input(k-1) = IterativePseudoDerivative_xprova2(Ts,u1(k-1),pidout.c,pidout.d,pidout.mediana,pidout.reset);
    
    [x1_out, x2_out, rho_out] = Switching2I(time(k), sampling_time ,activate, error(k-1), d_error(k-1), d_input(k-1) ,  startTimeDoubleSwitchedIntegrator , endTimeDoubleSwitchedIntegrator,...
        sigma_x1,sigma_dx1,sigma_x2,sigma_dx2,sigma_discharge,sigma_de,sigma_e,epsilonDischarge,k12_bar,k2e_bar, discharge,threshold_dusignx1,dwell_time,error_reset);
    x_double(:,k-1) = [x1_out;x2_out];
    rho(k-1) = rho_out;
    if(time(k)>=1.707)
        c = 1;
    end
    u2(k-1) = u1(k-1) + x1_out;
    Xp(:,k) = Pss.a*Xp(:,k-1) + Pss.b*u2(k-1);
    
end
toc
%%
MyFontsize = 15;
figure(1)
ax(1) = subplot(4,1,1);
%semilogy(time,abs(error)+1E-6,'LineWidth',2)
plot(time,Ref_signal,'--k',time,Pss.c*Xp(:,:)+ Pss.d*u2,'b','LineWidth',2)
%plot(time,error,'LineWidth',2)
grid on
set(gca,'FontSize',MyFontsize)
legend('I_p ref','I_p','FontSize',MyFontsize);
ylim([-1E4 RefTop*1.2]);

ax(2) = subplot(4,1,2);
plot(time,d_error,'r',time,d_input/max(abs(d_input))*max(abs(d_error)),'b','LineWidth',2)
legend('\dot error','\dot u_1','FontSize',MyFontsize);
grid on
set(gca,'FontSize',MyFontsize)

ax(3) = subplot(4,1,3);
plot(time,x_double(1,:),'r',time,x_double(2,:)/max(abs(x_double(2,:)))*max(abs(x_double(1,:))),'b','LineWidth',2)
legend('x_1','x_2','FontSize',MyFontsize);
ylabel('2I (normalized state)','FontSize',MyFontsize)
grid on
set(gca,'FontSize',MyFontsize)

ax(4) = subplot(4,1,4);
plot(time,rho,'LineWidth',2)
ylabel('rho','FontSize',MyFontsize);
grid on
xlabel('time (s)')
set(gca,'FontSize',MyFontsize)


linkaxes(ax,'x');




