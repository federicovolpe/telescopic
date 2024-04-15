//per il pid
# include <PID_v1.h>
double setpoint = 10; // distanza costante desiderata
//double input; // input dal sensore ad ultrasuoni
//double output; // distanza da spostarsi (espressa in millisecondi di attivazione del motore)
//double Kp = 0, Ki = 10, Kd = 0;
//PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT); // ISTANZA DEL PID

// setup pins
// per il motore
const int dir1 = 2;
const int dir2 = 4;
const int pwmPin = 15;
const int Signal = 13;  // apparentemente non serve

// per il sensore ad ultrasuoni
const int trigPin = 5;
const int echoPin = 18;     
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
long duration;
float distanceCm;
float distanceInch;

volatile bool finecorsa_indietro = false;
volatile bool finecorsa_avanti = false;
volatile int direzione = 1;

void setup() {
  //pulsante di fine corsa
  pinMode(19, INPUT);
  attachInterrupt(digitalPinToInterrupt(19), fineCorsaIndietro ,RISING);
  attachInterrupt(digitalPinToInterrupt(19), attivaCorsaIndietro ,FALLING);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(Signal, OUTPUT);

  // pin per il sensore ad ultrasuoni
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  // riguardo il pid
  myPID.SetMode(AUTOMATIC); // accensione del processo pid
  myPID.SetTunings(Kp, Ki, Kd); //   passaggio dei parametri per il pid
   
  Serial.begin(115200);
}

// ---------------------------------- funzioni di interrupt -------------------------------------------
void fineCorsaIndietro(){
  finecorsa_indietro = true;
}
void attivaCorsaIndietro() {
  finecorsa_indietro = false;
}

void cambioDirezione() {
  if (input > setpoint){ // se la distanza è maggiore del set point torno indietro
      digitalWrite(dir1, HIGH);
      digitalWrite(dir2, LOW);
      direzione = 1;
  }else{
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, HIGH);
      direzione = -1;
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
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance(cm):");
  Serial.println(distanceCm);
  return distanceCm;
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

bool cisonoOstacoli(){
  return (finecorsa_indietro && direzione == 0) || (finecorsa_avanti && direzione == 1);
}


void loop() {
  //***************************************** lettura della distanza ********************************************
  input = letturaUltrasuoni();
  cambioDirezione();

  //***************************************** movimento effettivo del braccio ********************************************
  if(!cisonoOstacoli()) { // se mi posso muovere
    delta_mov = movimento(input);  
  }

  //***************************************** stampa dei parametri di controllo ********************************************
  Serial.print("durata_movimento:");
  Serial.println(calcolaOutput(input));  
  Serial.print("pulsante:");
  Serial.println(digitalRead(19)); 


  delay(100); //delay di sicurezza per nonfare bruciare il motore
}