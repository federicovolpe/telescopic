# LO SCOPO
Questo progetto sviluppa un braccio telescopico che utilizza un sensore ad ultrasuoni per mantenere una distanza costante da un ostacolo. 

Il sistema è controllato da un microcontrollore ESP32 e implementa un sistema di controllo PID per la gestione dei due motori del braccio

immagine*[immagine]

## capacità da dimostrare:
* Padronanza del controllo di due motori
* Implementazione di un sistema di controllo PID (Proportional-Integral-Derivative)
* Gestione di segnali PWM (Pulse Width Modulation)
* Sviluppo di un sistema a loop chiuso per il controllo del braccio


# IL PROGETTO:
Il braccio telescopico è composto da due carrelli che si muovono in contemporanea per regolare la distanza dal ostacolo. I due motori sono controllati dall'ESP32 tramite un ponte H L298N. Il microcontrollore gestisce anche il sensore ad ultrasuoni per la rilevazione della distanza.

# materiali
* 1x ESP32
* 1x sensore ad ultrasuoni
* 1x ponte H doppio (del tipo L298N)
* 2x carrelli di stampanti

# problemi riscontrati

come segnalare il fine corsa:
all'inizio ho adottato la soluzione che prevedeva due pulsanti di fine corsa
due interrupt collegati a ciascun pulsante consentevano:
- segnalare il fine corsa quando il pulsante veniva premuto
- desegnalare il fine corsa quando il pulsante non era piu premuto

soluzione trovata:
 utilizzare due pad collegati a interrupt pin a fine corsa in ambo le direzioni,
 una volta arrivato a fine corsa il 5v collegato alla testa avrebbe fato contato con il pad segalando il fine corsa