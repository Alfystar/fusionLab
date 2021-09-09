# Report Problemi Lettura secondario

Al fine di leggere la tensione differenziale sul secondario del trasformatore è stata relalizzata questa scheda:

![image-20210630160932401](/home/alfy/Desktop/V2 problem/image-20210630160932401.png)

l'INA126 è un amplificatore differenziale da alimentare in duale

Avendo a disposizione 1 batteria da 12V è stato realizzato un piccolo splitter di tensione (un op-amp  inseguitore con vcc/2 come tensione di riferimento), e si è usata questa finta Massa, per dare la referenve al chip.



Misurando il segnale generato da un generatore di funzione tra Vout(INA126)-Vcc/2 ecco il risultato:

![image-20210630161306718](/home/alfy/Desktop/V2 problem/image-20210630161306718.png)

Tutto in regola.



In base al circuito sopra, mi aspettavo che, misurando sempre Vout ma questa volta tra Vout(INA126)-GND (true), senza modificare altro, il risultato è:

![image-20210630161425129](/home/alfy/Desktop/V2 problem/image-20210630161425129.png)



La prima ipotesi è stata di aver sbagliato nel filtro del 2° ordine, al chè ho scollegato Vout dalla R4, ma i risusltati non cambiano.



Poichè il segnale DEVE essere campionato da una scheda alimentata da alimentazione singola, e non si hanno a disposizione i pin dell'ADC per poter collegare Avcc e Agnd, il segnale in uscita deve essere per forza:
$$
V_{out} = \frac{Vcc}{2} + \delta V_2
$$
 
