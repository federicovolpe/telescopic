//V 1.0 : il pid gestisce il tempo in cui il motore resta acceso


# include <PID_v1.h>
double setpoint = 10; // distanza costante desiderata
//double input; // input dal sensore ad ultrasuoni
//double output; // distanza da spostarsi (espressa in millisecondi di attivazione del motore)
//double Kp = 0, Ki = 10, Kd = 0;
//PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT); // ISTANZA DEL PID

// setup pins
// per il motore
const int dir1 = 13, dir2 = 12, pwmPin = 11, Signal = 0;  // apparentemente non serve

// pin di interrupt fine corsa
const int finecAvanti = A1, finecIndietro = A3;
volatile int sogliaFineCorsaAvanti, sogliaFineCorsaIndietro;

// per il sensore ad ultrasuoni
const int trigPin = 5, echoPin = 4;     
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
long duration;
float distanceCm;
float distanceInch;

volatile bool finecorsa_indietro = false, finecorsa_avanti = false;
volatile int direzione = 1;

void setup() {
  // pins per il semaforo
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(10, LOW);
  digitalWrite(9, LOW);
  digitalWrite(8, HIGH);

  // pin per il motore
  pinMode(A0, INPUT);
  
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(Signal, OUTPUT);

  // pin di interrupt
  pinMode(finecAvanti, INPUT);
  pinMode(finecIndietro, INPUT);
  sogliaFineCorsaAvanti = analogRead(finecAvanti);
  sogliaFineCorsaIndietro = analogRead(finecIndietro);

  // pin per il sensore ad ultrasuoni
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  // riguardo il pid
  //myPID.SetMode(AUTOMATIC); // accensione del processo pid
  //myPID.SetTunings(Kp, Ki, Kd); //   passaggio dei parametri per il pid
   
  Serial.begin(9600);

  // un secondo prima di iniziare
  delay(1000);
}



void cambioDirezione(int input) {
  if (input > setpoint){ // se la distanza è maggiore del set point torno indietro
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
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  /* Prints the distance in the Serial Monitor*/
  Serial.print("Distance(cm):");
  Serial.println(distanceCm);
  return distanceCm;
}


// calcola il tempo di movimento del motore e ritorna lo stesso
int movimento(int input) {
  int tempo_movimento = 0;
  
  if(abs(input - setpoint) > 2) {
    if(direzione == -1){ // nel caso mi dovessi muovere indietro il valore in cm deve essere moltiplicato poichè 2ms sarebbero troppo pochi per il motore
      tempo_movimento = map(abs(input - setpoint), 2, 10, 10, 30);
    }
    tempo_movimento = map(abs(input - setpoint), 2, 10, 10, 30);
  }
  
  analogWrite(pwmPin, 255);
  delay(tempo_movimento);
  analogWrite(pwmPin, 0);

  return tempo_movimento;
}

bool direzioneConcessa(){ // consento il movimento solo nella direzione libera
  if (finecorsa_indietro && direzione == 0) return false; // se posso solo andare avanti
  if (finecorsa_avanti && direzione == 1) return false;
  return true;
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
  if(finecorsa_avanti && direzione == 1) {
    Serial.println("--------non cambio");
  }else if ( (sogliaFineCorsaAvanti - analogRead(finecAvanti)) > (sogliaFineCorsaAvanti / 5)){
    Serial.println("--------  fine corsa avanti !!!!!! ");
    digitalWrite(8, HIGH);
    finecorsa_avanti = true;
  }else{
    digitalWrite(8, LOW);
    finecorsa_avanti = false;
  } 

  if(finecorsa_indietro && direzione == 0){// non cambia se continuo a voler andare verso il finecorsa
    Serial.println("--------non cambio");
  }else if( (sogliaFineCorsaIndietro - analogRead(finecIndietro)) > (sogliaFineCorsaIndietro / 5)){ // altrimenti ricalcolo i finecorsa
    Serial.println("--------  fine corsa indietro !!!!!! ");
    digitalWrite(10, HIGH);
    finecorsa_indietro = true;
  }else{
    finecorsa_indietro = false;
    digitalWrite(10, LOW);
  }
  

  //***************************************** stampa dei parametri di controllo ********************************************
  /*Serial.print("movimento:");
  if(direzione == 1){
  Serial.println(delta_mov);  
  }else{
    Serial.println(- delta_mov);  
  }
  /*Serial.print("finecorsa Indietro:");
  Serial.println(finecorsa_indietro);
  Serial.print("finecorsa Avanti:");
  Serial.println(finecorsa_avanti); */

  Serial.print("sensore_avanti:");
  Serial.println(analogRead(finecAvanti));
  Serial.print("sensore_indietro:");
  Serial.println(analogRead(finecIndietro));

  // segnalazione semaforo finecorsa
  if(!finecorsa_avanti && !finecorsa_indietro){digitalWrite(9, HIGH);}else {digitalWrite(9, LOW);}
  Serial.println("---------------------------------------");

  delay(200); //delay di sicurezza per nonfare bruciare il motore
}