# LO SCOPO
Questo progetto scalabile sviluppa un braccio telescopico che utilizza un sensore ad ultrasuoni per mantenere una distanza costante da un ostacolo. 

Il sistema è controllato da un microcontrollore ARDUINO e implementa un sistema di controllo PID per la gestione del braccio.

![immagine](/progetto.jpg)

## capacità da dimostrare:
* Padronanza del controllo di un motore
* Implementazione di un sistema di controllo PID (Proportional-Integral-Derivative)
* Gestione di segnali PWM (Pulse Width Modulation)
* Sviluppo di un sistema a loop chiuso per il controllo del braccio


# IL PROGETTO
Il braccio telescopico è composto in versione prototipale da un carrello. Il motore, controllato tramite un ponte H L298N. 
Idealmente il progetto può essere esteso per l'utilizzo di più carrelli.
Il microcontrollore gestisce anche il sensore ad ultrasuoni per la rilevazione della distanza assieme ai sensori di controllo del finecorsa e un semplice semaforo per la segnalazione dello stato attuale.

## materiali:
* 1x Arduino
* 1x sensore ad ultrasuoni
* 1x ponte H (del tipo L298N)
* 1x+ carrello per stampante
* 1x alimentatore per avvitatore
* 3x led con resistenze (per il semaforo)

# problemi riscontrati

### 1) segnalare il fine corsa al microcontrollore:
all'inizio ho adottato la soluzione che prevedeva due pulsanti di fine corsa
due interrupt collegati a ciascun pulsante consentevano:
- segnalare il fine corsa quando il pulsante veniva premuto
- desegnalare il fine corsa quando il pulsante non era piu premuto
  
questi erano però soggetti a bouncing che rendeva il tutto meno affidabile.

#### soluzione trovata:
utilizzo di fotoresistenze che sono in grado di rilevare la presenza della testina, con l'aiuto di led per rendere il sistema indipendene dalla luminosità ambientale corrente

### 2) utilizzo del pid
Inizialmente data l'irregolarità del comportamento del motore ho optato per l'utilizzo del pid per gestire il tempo di attivazione del motore,
il motore si muoveva però a scatti molto evidenti.
vedi file [braccio telescopico v1](src/braccio_telescopico_V1.0/braccio_telescopico_V1.0.ino)

#### soluzione trovata: 
sfruttando un alimentatore differente da quello originale della stampante sono riuscito a utilizzare il pid per gestire una pwm
come si vede nel file [braccio telescopico v2](src/braccio_telescopico_V2.0/braccio_telescopico_V2.0.ino.ino)

### 3) alimentazione Arduino:
l'alimentazione fornita dal ponte h L298N non risultava (con nessun tipo di alimentatore) sufficiente ad alimentare l'Arduino,
questo comportava anomalie nel comportamento del microcontrollore.

#### soluzione trovata:
al posto di collegare i due in serie sfruttando il pin signal del ponte H ho deciso di sfruttare una alimentazione in parallelo
questa (avendo una alimentazione pari a 9Volt) risultava sufficente per alimentare entrambi.

> Federico volpe 