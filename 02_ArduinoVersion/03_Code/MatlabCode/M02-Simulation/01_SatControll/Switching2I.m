function [x1_out, x2_out, rho_out] = Switching2I(time, samplingTime ,activate, error, d_error, d_input ,  startTimeDoubleSwitchedIntegrator , endTimeDoubleSwitchedIntegrator,...
    sigma_x1,sigma_dx1,sigma_x2,sigma_dx2,sigma_discharge,sigma_de,sigma_e,epsilonDischarge,k12_bar,k2e_bar, discharge,threshold_dusignx1,dwell_time,error_reset)

% sampling_time  Ts;
% activate: to activate (not zero) the controller 
% startTimeDoubleSwitchedIntegrator: start time
% endTimeDoubleSwitchedIntegrator: final time (it is switched off and state discharged! take care....)
% sigma_x1: amplitude saturation for x1
% sigma_x2 : amplitude saturation for x2
% sigma_dx1 : derivative saturation for x1
% sigma_dx2 : derivative saturation for x2
% sigma_discharge: derivative saturation for x1 exponential discharge
% sigma_de : d_error amplitude threshold
% sigma_e  : error amplitude threshold
% k12_bar  : Gain of the first state integrator, set to zero for deactivate it
% k2e_bar  : Gain of the second state  integrator
% epsilonDischarge: discharge the x1 integrator exponentially: x1 = epsilonDischarge*x1 
% threshold_dusignx1: d_u1*sign(x1) <= -threshold_dusignx1  to decide if switching off
% discharge: if discharge is not zero the controller state is discharged.
% dwell_time: do not activate again the controller before the dwell_time is elapsed
% erorr_reset: set x_2 = 0 if x2*sign(error)<= - error_reset (select 0 if you do not whant this feature)


persistent rho counter x1 x2 t_last_off

ddIntEnabled = 0;
d_u1 = d_input;
if(activate)
    if(isempty(counter))
        counter = 0;
        x1 = 0.0;
        x2 = 0.0;
        rho = 0.0;
        t_last_off = time-dwell_time*2;
    end
    
    abs_Error = abs(error);
    abs_dError = abs(d_error);
    
    
    %Check the time interval where the code is executed
    if (time >= startTimeDoubleSwitchedIntegrator &&  time <= endTimeDoubleSwitchedIntegrator)
        ddIntEnabled = 1;
    end
    
    if( ( ( (abs_dError > sigma_de) || (abs_Error > sigma_e) ) && d_u1*sign(x1) <= -threshold_dusignx1 )|| ~ddIntEnabled || discharge)
        rho = 0.0; %discharge the integrator
        t_last_off = time;
    else
        if(time-t_last_off>= dwell_time ) %switch it on if a dwell time is elapsed 
           rho = 1.0; %switch it on
        else
           rho = 0.0;
        end
    end
    
    
     
    %% Selection of the K12 jump if the dot e is small and integrator output is below the threshold
    if( (abs_dError < sigma_de) && (abs_Error < sigma_e) )
        k12 = k12_bar;
    else
        k12 = 0.0;
    end
    
    %% Selection of the K2e jump if the dot e is small and integrator output is below the threshold
    if( (abs_dError < sigma_de) && (abs_Error < sigma_e) )
        k2e = k2e_bar;
    else
        k2e = 0.0;
    end
    
    %% Error reset when error*x2<0
    if(error_reset ~= 0 && sign(error)*x2 <= - error_reset)
         x2 = 0.0 ;
    end
    
    if(rho==1)
        %state update with amplitude and rate saturation
        x1 = min(sigma_x1,max(-sigma_x1, x1 + samplingTime*min(sigma_dx1,max(-sigma_dx1,k12*x2))));
        x2 = min(sigma_x2,max(-sigma_x2, x2 + samplingTime*min(sigma_dx2,max(-sigma_dx2,k2e*error))));
    else
        %exponential discharge with rate saturation on x_1
        x1 = x1 - min(sigma_discharge,max(-sigma_discharge,(1-exp(-epsilonDischarge*samplingTime))*x1));
        x2 = 0;
    end
    
    x1_out=x1;
    x2_out=x2;
    rho_out = rho;
    counter = mod(counter,1E6)+1;
    
else
    
    x1_out=0.0;
    x2_out=0.0;
    rho_out = 0.0;%rho_out;
    
end