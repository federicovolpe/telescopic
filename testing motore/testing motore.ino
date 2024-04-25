// file con il solo scopo di poter testare nuovi pid e algoritmi per la gestione del motore

//per il pid
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
const int finecAvanti = A1, finecIndietro = A2;
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
  // pin per il motore
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  
  // pin per il sensore ad ultrasuoni
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  // pin di interrupt
  pinMode(finecAvanti, INPUT);
  pinMode(finecIndietro, INPUT);
  sogliaFineCorsaAvanti = analogRead(finecAvanti);
  sogliaFineCorsaIndietro = analogRead(finecIndietro);
  

  // riguardo il pid
  //myPID.SetMode(AUTOMATIC); // accensione del processo pid
  //myPID.SetTunings(Kp, Ki, Kd); //   passaggio dei parametri per il pid
   
  Serial.begin(9600);
}

void cambioDirezione(int input) {
  if (input > setpoint){ // se la distanza è maggiore del set point torno indietro
      digitalWrite(dir1, HIGH);
      digitalWrite(dir2, LOW);
      direzione = 1;
  }else{
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, HIGH);
      direzione = -1;
  }
  if(direzione == 1){
    Serial.println("avanti");

  }else{
    Serial.println("indietro");
  }
}

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
  
  //***************************************** movimento effettivo del braccio ********************************************
  if(direzioneConcessa()) { // se mi posso muovere
    digitalWrite(pwmPin, HIGH);
    digitalWrite(dir1, HIGH);
    
    // lettura se il motore si è fermato
    Serial.print("A1:");
    Serial.println(analogRead(A1));

    delay(30);
    analogWrite(pwmPin, 0);  
  }

  //***************************************** stampa dei parametri di controllo ********************************************


  delay(1500); //delay di sicurezza per nonfare bruciare il motore
}