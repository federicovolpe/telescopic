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

void setup() {
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(Signal, OUTPUT);

  // pin per il sensore ad ultrasuoni
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  attachInterrupt(digitalPinToInterrupt(23), cambioDirezione, RISING);
  Serial.begin(115200);
}

volatile int voltaggio = 200;
volatile bool direzione = 0;

void cambioDirezione() {
  if (letturaUltrasuoni() >=10){
    //vado verso l'ostacolo
    Serial.println("in avanti !!!");
      digitalWrite(dir1, HIGH);
      digitalWrite(dir2, LOW);
      
  }else{
      Serial.println("indietro !!!");
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, HIGH);
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

void loop() {
  cambioDirezione();

  Serial.println("MI MUOVO");  
  // mi muovo per 100 msec
  analogWrite(pwmPin, voltaggio);
  delay(50);
  analogWrite(pwmPin, 0);
  delay(700);
}
