/*

source code for the autonomous displacement
and IR communication of our racing robot

08th May 2018
*/

#include <NewPing.h>
#include <Servo.h>
#include <IRremote.h>
#include <Time.h>

Servo ServoSourire;

int STBY = 30;
int PWMA = 6;
int AIN1 = 32;
int AIN2 = 34;
int PWMB = 5;
int BIN1 = 28;
int BIN2 = 26;
int DataTSOP = 9;
int LED = 13;
int LedIR = 3;
int LedTete1 = 2;
int LedTete2 = 4;
int CapteurHall = 12;

boolean StateAvant = true;
boolean StateArriere = true;
boolean Start = true;
uint32_t period = 5000L;

#define TRIGGER_PIN  22
#define ECHO_PIN     24
#define MAX_DISTANCE 450 //Maximum sensor distance: 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
int DistMur = 20; // max distance before detectiong the wall (and any obstacles with the sensor)

IRrecv irrecv(DataTSOP);
decode_results results;

void setup() {
  Serial.begin(9600);

  ServoSourire.attach(10);
  pinMode(LED, OUTPUT);
  pinMode(LedTete1, OUTPUT);
  pinMode(LedTete2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(CapteurHall, INPUT);

  // verification
  digitalWrite(LedTete1, HIGH);
  digitalWrite(LedTete2, HIGH);
  delay(500);
  digitalWrite(LedTete1, LOW);
  digitalWrite(LedTete2, LOW);

  ServoSourire.write(10);
  delay(100);
  ServoSourire.write(0);

  digitalWrite(STBY, HIGH);
  goForward(100);
  delay(250);
  goForward(0);
  goBackward(100);
  delay(250);
  goBackward(0);

  irrecv.enableIRIn(); // activate the IR signal detection
}

void loop() {
  ArretOrNot();
  if (Start) {
    digitalWrite(STBY, HIGH);
    int Distance1 = sonar.ping_cm();
    delay(100);
    int Distance2 = sonar.ping_cm();
    int Distance = max(Distance1, Distance2);
    Serial.println(Distance);
    int IR = IR_Mesure();

    if (IR == 0) {
      if (Distance < 20) {
        if (StateAvant) {
          StateAvant = false;
          StateArriere = true;
          goForward(0);
          delay(50);
          goBackward(100);
          rotateLeft(100);
          LedFuite();
        }
      }
      else {
        if (StateArriere) {
          StateArriere = false;
          StateAvant = true;
          goForward(100);
        }

      }
    }

    else { // we received an IR signal
      Sourire();
      digitalWrite(LedTete1, HIGH);
      digitalWrite(LedTete2, HIGH);
      StateAvant = true;
      StateArriere = true;
      for ( uint32_t tStart = millis();  (millis() - tStart) < period;  ) {
        int Distance1 = sonar.ping_cm();
        delay(100);
        int Distance2 = sonar.ping_cm();
        int Distance = max(Distance1, Distance2);
        if (digitalRead(CapteurHall) == HIGH) {
          if (Distance < 20) {
            if (StateAvant) {
              StateAvant = false;
              StateArriere = true;
              goForward(0);
              delay(50);
              goBackward(150);
              rotateLeft(150);
            }
          }
          else {
            if (StateArriere) {
              StateArriere = false;
              StateAvant = true;
              goForward(200);
            }
          }
        }
        else {
          digitalWrite(STBY, LOW);
          digitalWrite(LedTete1, LOW);
          digitalWrite(LedTete2, LOW);
          delay(20000);
        }
      }
      StateAvant = true;
      StateArriere = true;
      digitalWrite(LedTete1, LOW);
      digitalWrite(LedTete2, LOW);
      Sourire();
      IR = IR_Mesure();
    }

  }
  else {
    digitalWrite(STBY, LOW);
    delay(20000);
  }
  delay(100);
}

void Avant() {
  digitalWrite (AIN1, HIGH);
  digitalWrite (AIN2, LOW);
  digitalWrite (BIN1, HIGH);
  digitalWrite (BIN2, LOW);
}

void Arriere() {
  digitalWrite (AIN1, LOW);
  digitalWrite (AIN2, HIGH);
  digitalWrite (BIN1, LOW);
  digitalWrite (BIN2, HIGH);
}

void goForward (int pwm)
{
  Avant();
  analogWrite(PWMA, pwm);
  analogWrite(PWMB, pwm);
}

void goBackward (int pwm)
{
  Arriere();
  analogWrite(PWMA, pwm);
  analogWrite(PWMB, pwm);
}

void rotateRight (int pwm)
{
  analogWrite(PWMA, 2 * pwm);
  analogWrite(PWMB, pwm);
}

void rotateLeft (int pwm)
{
  analogWrite(PWMA, pwm);
  analogWrite(PWMB, 2 * pwm);
}

void Sourire() {
  int k = ServoSourire.read();
  if (k == 0) {
    ServoSourire.write(150);
  }
  else {
    ServoSourire.write(0);
  }
}

int IR_Mesure()
{
  if (irrecv.decode(&results))
  {
    if (results.value == 0xCFB1D150)
    {
      irrecv.resume();
      LedFuite();
      return 1;
    }
    irrecv.resume();
    return 0;
  }
  else
  {
    irrecv.resume();
    return 0;
  }
}

void LedFuite() {
  digitalWrite(LedTete1, LOW);
  digitalWrite(LedTete2, HIGH);
  delay(100);
  digitalWrite(LedTete1, HIGH);
  digitalWrite(LedTete2, LOW);
  delay(100);
  digitalWrite(LedTete1, LOW);
  digitalWrite(LedTete2, LOW);
}

void ArretOrNot() {
  int EtatHall = digitalRead(CapteurHall);
  if (EtatHall == LOW) {
    Start = false;
  }
  else {
    Start = true;
  }
}
