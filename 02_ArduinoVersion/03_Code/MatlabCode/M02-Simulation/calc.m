clear variable
clc

P = tf([1 0], [1 3 2]) % (s+1)(s+2), il vero impianto ha poli stabili

C1 = tf([1], [1 0 0])       %  I^2
C2 = tf([1 1], [1 0 0])     %  I^2 + I
C3 = tf([1 1 1], [1 0 0])   %  I^2 + I + P

PC1 = minreal(P*C1)
PC2 = minreal(P*C2)
PC3 = minreal(P*C3)

Wyr1 = minreal(PC1/(1+PC1))
Wyr2 = minreal(PC2/(1+PC2))
Wyr3 = minreal(PC3/(1+PC3))

B1 = isstable(Wyr1)
Poli1 = (pole(Wyr1))

B2 = isstable(Wer2)
Poli2 = (pole(Wer2))

B3 = isstable(Wer3)
Poli3 = (pole(Wer3))
