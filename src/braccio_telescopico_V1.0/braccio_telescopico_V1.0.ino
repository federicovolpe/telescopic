//V 1.0 : il pid gestisce il tempo in cui il motore resta acceso

// parametri
const double obiettivo = 10;                                  // obiettivo di distanza dal sensore in cm 
#define SOGLIA 1
#define SOUND_SPEED 0.034
#define TRESHOLD 40
volatile int FCAiniziale, FCIiniziale,                        // (fine corsa avanti / indietro) lettura iniziale per l'adattamento alla luminosita' ambientale
              tolleranzaFCA, tolleranzaFCI;                   // parametri di tolleranza per reputare il finecorsa
volatile bool finecorsa_indietro = false, finecorsa_avanti = false; // parametri iniziali per i finecorsa

volatile int direzione = 1;                                   // direzione presa in un determinato momento
volatile double distanza ;                                    // distanza dell'ostacolo dal sensore
volatile double distanza_norm;                                // distanza dal sensore (se scende sotto l'obiettivo riprende a incrementare)
volatile double tempo_movimento = 0;                          // tempo di attivazione del motore
volatile double offset = 0;                                   // la distanza dall'obiettivo

// per il pid
#include <PID_v1.h>
float P = 1, I = .2, D = .2;
PID PIDcontroller (&distanza_norm, &tempo_movimento, &obiettivo ,P ,I ,D, REVERSE);

// ---------------------------- SETUP DEI PIN ----------------------------
// per il motore
const int dir1 = 13, dir2 = 12, pwmPin = 11;  

// pin per il semaforo
const int semaforo_indietro = 10, semaforo_avanti = 8, semaforo_verde = 9;

// pin per le fotoresistenze di finecorsa
const int FCA = A2, FCI = A1; // (FineCorsaAvanti, FineCorsaIndietro)

// pin per il sensore ad ultrasuoni
const int trigPin = A4, echoPin = A5;     

void setup() {
  // pins per il semaforo
  pinMode(semaforo_indietro, OUTPUT); digitalWrite(semaforo_indietro, LOW);
  pinMode(semaforo_verde, OUTPUT);    digitalWrite(semaforo_verde, LOW);
  pinMode(semaforo_avanti, OUTPUT);   digitalWrite(semaforo_avanti, LOW);
  
  // pin per il motore
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwmPin, OUTPUT);

  // pin di finecorsa
  pinMode(FCA, INPUT);
  pinMode(FCI, INPUT);
  // inizializzazione dei valori delle resistenze per il finecorsa
  FCAiniziale = analogRead(FCA);
  tolleranzaFCA = FCAiniziale /5;
  FCIiniziale = analogRead(FCI);
  tolleranzaFCI = FCIiniziale / 5;

  // pin per il sensore ad ultrasuoni
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
   
  Serial.begin(9600);

  // per il pid
  PIDcontroller.SetMode(AUTOMATIC);
  PIDcontroller.SetTunings(P, I, D);

  // un secondo prima di iniziare
  delay(1000);
}


void cambioDirezione() {
  if (distanza > obiettivo){ // se la distanza è maggiore del set point torno indietro
      digitalWrite(dir1, HIGH);
      digitalWrite(dir2, LOW);
      direzione = 1;
  }else{
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, HIGH);
      direzione = 0;
  }
}

void letturaUltrasuoni() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanza = duration * SOUND_SPEED/2;
}

void movimento() { // calcola il tempo di movimento del motore
  distanza_norm = distanza; // utilizzo un altra variabile per non influenzare quella globale
  if (distanza < obiettivo) {distanza_norm = obiettivo + (obiettivo - distanza);}

  PIDcontroller.Compute();
  
  tempo_movimento = map(tempo_movimento, 1, 50, 30, 200);
  digitalWrite(pwmPin, HIGH);
  delay(tempo_movimento);
  digitalWrite(pwmPin, LOW);
}

bool direzioneConcessa(){ // consento il movimento solo nella direzione libera
  if (finecorsa_indietro && direzione == 0) return false; // se posso solo andare avanti
  if (finecorsa_avanti && direzione == 1) return false;
  return true;
}

void calcolo_finecorsa(){
  if( (finecorsa_avanti && direzione == 1)||
    (FCAiniziale - analogRead(FCA)) > tolleranzaFCA){
    finecorsa_avanti = true;
  }else{
    finecorsa_avanti = false; 
  } 

  if( (finecorsa_indietro && direzione == 0)||
   (FCIiniziale - analogRead(FCI)) > tolleranzaFCI){ // altrimenti ricalcolo i finecorsa
    finecorsa_indietro = true;
  }else{
    finecorsa_indietro = false;
  }
}

void gestione_semaforo(){
  if(finecorsa_avanti){digitalWrite(semaforo_avanti, HIGH);}else{digitalWrite(semaforo_avanti, LOW);}
  if(finecorsa_indietro){digitalWrite(semaforo_indietro, HIGH);}else{digitalWrite(semaforo_indietro, LOW);}
  if(!finecorsa_avanti && !finecorsa_indietro){ digitalWrite(semaforo_verde, HIGH);}
  else {                                        digitalWrite(semaforo_verde, LOW); }
}

void stampaParametri () {
  Serial.print("finecorsa Indietro:");
  Serial.println(finecorsa_indietro);
  Serial.print("finecorsa Avanti:");
  Serial.println(finecorsa_avanti); 

  Serial.print("Distance(cm):");
  Serial.println(distanza);

  /*Serial.print("sensore_avanti:");
  Serial.println(analogRead(FCA));
  Serial.print("sensore_indietro:");
  Serial.println(analogRead(FCI));*/

  Serial.print("offset:");
  Serial.println(offset);

  Serial.print("movimento:");
  if ( direzione == 1 ){   Serial.println(tempo_movimento);  }
  else{                   Serial.println(- tempo_movimento);  }
  
  Serial.println("---------------------------------------");
  Serial.print("direzione concessa");
  Serial.println(direzioneConcessa());
  Serial.print("offset > SOGLIA");
  Serial.println(offset > SOGLIA);
  Serial.print("offset < TRESHOLD");
  Serial.println(offset < TRESHOLD);
  
}


void loop() {
  //***************************************** lettura della distanza ********************************************
  letturaUltrasuoni();

  cambioDirezione();

  //***************************************** movimento effettivo del braccio ********************************************
  offset = abs(distanza - obiettivo); 
  if(direzioneConcessa() && offset > SOGLIA && offset < TRESHOLD) { // se mi posso muovere
    movimento(); 
  }else{
    tempo_movimento = 0;
  }

  //**************************** calcolo dei prarametri di fine corsa ***************************
  calcolo_finecorsa();
  gestione_semaforo();

  stampaParametri();

  //delay(30);
}