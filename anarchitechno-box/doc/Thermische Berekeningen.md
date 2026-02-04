## Thermische berekening IRFZ44

ATMEGAdimmer gebruikt IRFZ44 MOSFETs. Deze worden met 12V aangestuurd, en hebben 28mOhm Rds(on) bij Vgs=10V. 

De thermische weerstand van een TO-220 "free standing" is 62C/W. Met Tj(max)=175C en Ta(max)=35C hebben we: P(max)=(175 - 35)/62 = 2.26W. Met 28mOhm en P=I^2*R => I^2=80.7A^2 => Imax=8.98A

Dit is de maximum DC stroom. Met PWM wordt het maximum wegens schakelverliezen nog (flink) lager. Maar zo'n berekening is wat lastiger en laat ik even achterwege.
Ook de recirculation stroom is niet meegenomen. Onduidelijk van schema is hoe die oorspronkelijk werd opgevangen.

## Thermische berekening DRV8245

Dit is een H-brug, wat extra mogelijkheden geeft wat betreft aansturing van solenoids en motors. 

In een H-brug configuratie betekent het wel dat de stroom door 2 transistors loopt. Maar dit staat gelijk aan 2 kanalen enkel high-side aansturen.

De low-side en high-side transistor heeft een Rds(on) van 40mOhm (HTSSOP) of 32mOhm (VQFN).

Tj(max)=150C
Ta(max)=45C (alle verliezen gaan 1 gezamelijke PCB in, dus deze schat ik iets hoger in)
Rj-a=27.7C/W (HTSSOP package), of 41.3C/W (VQFN package)

We gebruiken de HTSSOP package, want:

P(max)=(150-45)/27.7=3.8W
P=I^2*R => I^2=94.8A^2 => 9.7A 1-ch
Omdat er 2 actieve transistors in 1 package zitten, mag eigenlijk elke transistor maar 1.9W dissiperen. Dan:
I^2 = 47.7A^2 => 6.88A 2-ch (of H-brug)

Voor VQFN:
P(max)=(150-45)/41.3=2.54W
I^2 = 79.4A^2 => 8.91A 1-ch, en 6.32A 2-ch

De datasheet geeft pessimistischere waarden omdat deze met 85C ambient temperatuur rekent. Wel staat er een model in voor 10-sec belastingen alsook in combinatie met 20kHz PWM.

Daarin zeggen ze:
- DC+PWM: 4.5A
- 10sec+PWM: 7.4A
- DC+no PWM: 5.4A
- 10sec+no PWM: 8.4A
- 0.1sec+no PWM: 16.1A

20kHz PWM kost dus 12-17% van de maximale stroom capaciteit.

We kunnen eventueel [https://www.tinytronics.nl/en/mechanics-and-actuators/parts/cooling-elements/aluminum-heatsink-7x7x7mm-with-adhesive](met heatsinks) en een 140mm fan de thermische weerstand (flink) verlagen..

Betreffende recirculation:
 
In geval van een high-side only power switch, is het verstandig om een losse vrijloop Schottky diode toe te voegen. Ik verwacht deze een lagere VF heeft dan de DRV8245.
In geval van H-brug aansturing is er keuze tussen fast en short decay, respectievelijk open-circuit (stroom wordt door body diodes van MOSFETs gejaagd) of short-circuit (zo min mogelijk serie verliezen). 
