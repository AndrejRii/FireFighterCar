#include <Stepper.h>

// LED diódy
const int led1 = 0;
const int led2 = 1;

// Ultrazvukový senzor HC
const int trigPin = 3;
const int echoPin = 2;

// IR senzory ohňa
const int flameSensor1 = 4;  // Flame sensor 1
const int flameSensor2 = 5;  // Flame sensor 2

// Motory 1, 2, 3, 4
const int motor1A = 6;  // Motor 1 
const int motor1B = 7;  // Motor 1 
const int motor2A = 8;  // Motor 2 
const int motor2B = 9;  // Motor 2 
const int motor3A = 10;  // Motor 3 
const int motor3B = 11;  // Motor 3 
const int motor4A = 12;  // Motor 4 
const int motor4B = 13;  // Motor 4

// Reproduktor
const int buzzer = A0;  // Analog pin used as digital

// Relé a krokový motor
const int pumpRelay = A1;  // Relay control for water pump
const int stepperControl1 = A2;  // Stepper motor control 1
const int stepperControl2 = A3;  // Stepper motor control 2
const int stepperControl3 = A4;  // Stepper motor control 3
const int stepperControl4 = A5;  // Stepper motor control 4

// Premenné
int state;
int stepperState;

// Debouncing 
unsigned long debounceTime;
unsigned long lastFlameDetectedTime;
bool flameDebounced;
unsigned long lastFlameDetectedTime2;
bool flameDebounced2;

static unsigned long previousMillis = 0;
 static unsigned long previousMillisBuzzer = 0;
static int blinkState = 0;

static int blink = 0;

// Inicializácia krokového motora
Stepper myStepper(2038, stepperControl1, stepperControl3, stepperControl2, stepperControl4);

void setup() {

  // Inicializácia

  lastFlameDetectedTime = 0;
  lastFlameDetectedTime2 = 0;
  debounceTime = 150;
  flameDebounced = false;
  flameDebounced2 = false;
  state = 0;
  stepperState = 1;

  // Piny motorov sú OUTPUT
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);
  pinMode(motor3A, OUTPUT);
  pinMode(motor3B, OUTPUT);
  pinMode(motor4A, OUTPUT);
  pinMode(motor4B, OUTPUT);

  // Piny IR senzorov sú INPUT
  pinMode(flameSensor1, INPUT);
  pinMode(flameSensor2, INPUT);

  // Piny LED diód sú OUTPUT
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // Pin reproduktora je OUTPUT
  pinMode(buzzer, OUTPUT);

  // Piny ultrazvukového senzora sú aj OUTPUT aj INPUT
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Piny relé a krokového motora sú OUTPUT
  pinMode(pumpRelay, OUTPUT);
  pinMode(stepperControl1, OUTPUT);
  pinMode(stepperControl2, OUTPUT);
  pinMode(stepperControl3, OUTPUT);
  pinMode(stepperControl4, OUTPUT);

  // Nastavenie relé vodnej pumpy na HIGH (vypnutý stav)
  digitalWrite(pumpRelay, HIGH);

  // Čakanie na začiatku programu, čas na umiestnenie auta na zem
  delay(5000);
}

void loop() {

  if(blink == 1){
    blinkLEDs();
    activateBuzzer();
  }

  // Premenná state riadi chod nekonečnej slučky loop
  if (state == 0) {
    tankDrive(HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW, HIGH);
    if (flameDetected()) {
      state = 1;
    }
  } else if (state == 1) {
    state = 10;
    alignWithFlame();
    blink = 1;
    //delay(100);
  } else if (state == 2){
    state = 5;
    moveToDistance(23);
  } else if (state == 3){
    blink = 0;
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(pumpRelay, LOW);
    if(stepperState == 1){
      state = 10;
      int count = 3;
      // Hasenie ohňa
      while(count > 0){
        myStepper.setSpeed(1);  
        myStepper.step(40);
        myStepper.step(-40);
        myStepper.step(-40);
        myStepper.step(40);
        count--;
      }
      // Vypnutie vodnej pumpy cez relé a vypnutie krokového motora
      digitalWrite(pumpRelay, HIGH);
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A5, LOW);
      // Kontrola ohňa, ak je stále prítomný, opakuj hasenie, ak je uhasený, hľadaj ďalší oheň
      if(flameDetected()){
        state = 3; 
        delay(200);               
      } else {
        state = 0; 
        delay(2000);       
      }
    }  
  }
}


void tankDrive(int motor1Adir, int motor1Bdir, int motor2Adir, int motor2Bdir,int motor3Adir, int motor3Bdir, int motor4Adir, int motor4Bdir) {
  digitalWrite(motor1A, motor1Adir);
  digitalWrite(motor1B, motor1Bdir);

  digitalWrite(motor2A, motor2Adir);
  digitalWrite(motor2B, motor2Bdir);

  digitalWrite(motor3A, motor3Adir);
  digitalWrite(motor3B, motor3Bdir);

  digitalWrite(motor4A, motor4Adir);
  digitalWrite(motor4B, motor4Bdir);
}

bool flameDetected() {
  // Kontrolujte, či aspoň jeden senzor vidí oheň (LOW)
  if (digitalRead(flameSensor1) == LOW || digitalRead(flameSensor2) == LOW) {
    // Zaznamená čas prvej detekcie
    if (!flameDebounced) {
      lastFlameDetectedTime = millis();
      flameDebounced = true;
    }

    // Kontrolujte, či bol plameň videný aspoň bo dobu debounceTime
    if (millis() - lastFlameDetectedTime > debounceTime) {
      return true;
    }
  } else {
    // Resetujte debouncing, ak plameň nebol videný
    flameDebounced = false;
  }

  return false;
}

bool flameDetectedOneSensor(int sensor) {
  // Kontrolujte, či daný senzor vidí oheň (LOW)
  if (digitalRead(sensor) == LOW) {
    
    // Zaznamená čas prvej detekcie
    if (!flameDebounced) {
      lastFlameDetectedTime2 = millis();
      flameDebounced2 = true;
    }

    // Kontrolujte, či bol plameň videný aspoň bo dobu debounceTime
    if (millis() - lastFlameDetectedTime2 > debounceTime) {
      return true;
    }
  } else {
    // Resetujte debouncing, ak plameň nebol videný
    flameDebounced2 = false;
  }

  return false;
}

void blinkLEDs() {
  unsigned long currentMillis = millis();

  // Blikanie LED diódami ako hasiči
  if (currentMillis - previousMillis >= 300) {
    previousMillis = currentMillis;

    if (blinkState == 0) {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      blinkState = 1;
    } else {
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      blinkState = 0;
    }
  }
}

void activateBuzzer() {
  unsigned long currentMillis = millis();
  static int buzzState = 0;

  // Hraj tón, zmeň ho každých 400ms
  if (currentMillis - previousMillisBuzzer >= 400) {
    previousMillisBuzzer = currentMillis;
    digitalWrite(buzzer, HIGH);
    if (buzzState == 0) {
      for (int hz = 440; hz < 1000; hz++) {
        tone(buzzer, hz, 100);
      }
    } else {
      for (int hz = 1000; hz > 440; hz--) {
        tone(buzzer, hz, 100);
      }
    }
    digitalWrite(buzzer, LOW);
    buzzState = 1 - buzzState;  // Toggle the state (0 becomes 1, 1 becomes 0)
  }
}

void alignWithFlame() {
  //Zastaví auto pred zarovnávaním s plameňom
    tankDrive(LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW);

    // Otočí auto do potrebného smeru tak, aby oba senzory videli oheň
    if(!digitalRead(flameSensor1)){
      while (!flameDetectedOneSensor(flameSensor2)) {
        tankDrive(LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH, LOW);
      }
    }else if (!digitalRead(flameSensor2)){
      while (!flameDetectedOneSensor(flameSensor1)) {
        tankDrive(HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW, HIGH);
      } 
    } 
      
    tankDrive(LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW);
    state = 2;

}

void moveToDistance(int targetDistance) {
    int distance = 0;
    unsigned long previousMillis = millis();
    unsigned long interval = 100;  // Set the interval to 100 milliseconds

    while (state == 5) {
        blinkLEDs();
        activateBuzzer();
        // Check if the specified interval has passed
        if (millis() - previousMillis >= interval) {
            // Update the previousMillis for the next iteration
            previousMillis = millis();

            // Vyšle signál a odmeria vzdialenosť
            digitalWrite(trigPin, LOW);
            delayMicroseconds(2);
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10);
            digitalWrite(trigPin, LOW);
            unsigned long duration = pulseIn(echoPin, HIGH);
            distance = duration * 0.034 / 2;

            if (distance > targetDistance) {
                // Hýbe sa ďalej ak je moc ďaleko
                tankDrive(HIGH, LOW, LOW, HIGH, LOW, HIGH, HIGH, LOW);
            } else {
                // Zastaví ak už dosiahol danú vzdialenosť
                tankDrive(LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW);
                state = 3;
            }
        }
    }
}
