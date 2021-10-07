clear variable
clc 
% k2/s^2
k2=1
A = [0 1; 0 0];
B = [0; k2];
C = [1 0];
D = 0;
sys = ss(A,B,C,D);
[num,den] = ss2tf(A,B,C,D);
C=tf(num,den)

dt = 3;
d_sys = c2d(sys,dt);
[Ad,Bd,Cd,Dd]=ssdata(d_sys)
[numd,dend]=ss2tf(Ad,Bd,Cd,Dd);
tfD=tf(numd,dend,dt)