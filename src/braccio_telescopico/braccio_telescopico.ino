//V 1.0 : il pid gestisce il tempo in cui il motore resta acceso
// parametri
const int obiettivo = 10; // obiettivo di distanza in cm 
#define SOGLIA 1 
#define SOUND_SPEED 0.034 
volatile int sogliaFineCorsaAvanti, sogliaFineCorsaIndietro; // parametri di riferimento iniziali per le fotoresistenze
volatile bool finecorsa_indietro = false, finecorsa_avanti = false;
volatile int direzione = 1;

// setup pins
// per il motore
const int dir1 = 13, dir2 = 12, pwmPin = 11, Signal = 0;  // apparentemente non serve

// pin per il semaforo
const int semaforo_indietro = 10, semaforo_avanti = 8, semaforo_verde = 9;

// pin per le fotoresistenze di finecorsa
const int finecAvanti = A2, finecIndietro = A1;

// pin per il sensore ad ultrasuoni
const int trigPin = A4, echoPin = A5;     

void setup() {
  // pins per il semaforo
  pinMode(semaforo_indietro, OUTPUT); digitalWrite(semaforo_indietro, LOW);
  pinMode(semaforo_verde, OUTPUT);    digitalWrite(semaforo_verde, LOW);
  pinMode(semaforo_avanti, OUTPUT);   digitalWrite(semaforo_avanti, HIGH);
  
  // pin per il motore
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwmPin, OUTPUT);

  // pin di finecorsa
  pinMode(finecAvanti, INPUT);
  pinMode(finecIndietro, INPUT);
  // inizializzazione dei valori delle resistenze per il finecorsa
  sogliaFineCorsaAvanti = analogRead(finecAvanti);
  sogliaFineCorsaIndietro = analogRead(finecIndietro);

  // pin per il sensore ad ultrasuoni
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
   
  Serial.begin(9600);

  // un secondo prima di iniziare
  delay(1000);
}


void cambioDirezione(int input) {
  if (input > obiettivo){ // se la distanza è maggiore del set point torno indietro
      digitalWrite(dir1, HIGH);
      digitalWrite(dir2, LOW);
      direzione = 1;
  }else{
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, HIGH);
      direzione = 0;
  }
  if(direzione == 1){
    Serial.println("avanti");

  }else{
    Serial.print("indietro" );
  }
}

int letturaUltrasuoni() {
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
  float distanceCm = duration * SOUND_SPEED/2;
  
  /* Prints the distance in the Serial Monitor*/
  Serial.print("Distance(cm):");
  Serial.println(distanceCm);
  return distanceCm;
}


int movimento(int input) { // calcola il tempo di movimento del motore e ritorna lo stesso
  int tempo_movimento = 0;
  
  if(abs(input - obiettivo) > SOGLIA) { // se la distanza è off di 2cm
    if(direzione == 0){ // nel caso mi dovessi muovere indietro il valore in cm deve essere moltiplicato poichè 2ms sarebbero troppo pochi per il motore
      tempo_movimento = map(abs(input - obiettivo), SOGLIA, 10, 20, 40);
    }
    tempo_movimento = map(abs(input - obiettivo), SOGLIA, 200, 20, 40);
  }
  Serial.print("------------      tempo calcolato :");
  Serial.print(tempo_movimento);
  Serial.println("     ------------");
  
  digitalWrite(pwmPin, HIGH);
  delay(tempo_movimento);
  //delay(500);
  digitalWrite(pwmPin, LOW);

  return tempo_movimento;
}

bool direzioneConcessa(){ // consento il movimento solo nella direzione libera
  if (finecorsa_indietro && direzione == 0) return false; // se posso solo andare avanti
  if (finecorsa_avanti && direzione == 1) return false;
  return true;
}

void calcolo_finecorsa(){
  if(finecorsa_avanti && direzione == 1) {
    Serial.println("--------non cambio");
  }else if ( (sogliaFineCorsaAvanti - analogRead(finecAvanti)) > (sogliaFineCorsaAvanti / 5)){
    Serial.println("--------  fine corsa avanti !!!!!! ");
    digitalWrite(semaforo_avanti, HIGH);
    finecorsa_avanti = true;
  }else{
    digitalWrite(semaforo_avanti, LOW);
    finecorsa_avanti = false;
  } 

  if(finecorsa_indietro && direzione == 0){// non cambia se continuo a voler andare verso il finecorsa
    Serial.println("--------non cambio");
  }else if( (sogliaFineCorsaIndietro - analogRead(finecIndietro)) > (sogliaFineCorsaIndietro / 5)){ // altrimenti ricalcolo i finecorsa
    Serial.println("--------  fine corsa indietro !!!!!! ");
    digitalWrite(semaforo_indietro, HIGH);
    finecorsa_indietro = true;
  }else{
    finecorsa_indietro = false;
    digitalWrite(semaforo_indietro, LOW);
  }

  if(!finecorsa_avanti && !finecorsa_indietro){ digitalWrite(semaforo_verde, HIGH);}
  else {                                        digitalWrite(semaforo_verde, LOW); }
}

void stampaParametri () {
  /*Serial.print("finecorsa Indietro:");
  Serial.println(finecorsa_indietro);
  Serial.print("finecorsa Avanti:");
  Serial.println(finecorsa_avanti); */

  Serial.print("sensore_avanti:");
  Serial.println(analogRead(finecAvanti));
  Serial.print("sensore_indietro:");
  Serial.println(analogRead(finecIndietro));
  
  Serial.println("---------------------------------------");
}


void loop() {
  //***************************************** lettura della distanza ********************************************
  int input = letturaUltrasuoni();
  cambioDirezione(input);

  //***************************************** movimento effettivo del braccio ********************************************
  int delta_mov = 0;
  if(direzioneConcessa()) { // se mi posso muovere
    delta_mov = movimento(input); 
  }

  //**************************** calcolo dei prarametri di fine corsa ***************************
  calcolo_finecorsa();

  //***************************************** stampa dei parametri di controllo ********************************************
  Serial.print("movimento:");
  if ( direzione == 1 ){   Serial.println(delta_mov);  }
  else{                   Serial.println(- delta_mov);  }

  stampaParametri();
  //delay(30);
}