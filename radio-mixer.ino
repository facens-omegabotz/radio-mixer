/*
 * RADIO MIXER FOR 2.4 GHz TX4 CONTROLLER
 *
 * 
 * OBS. INTERNA: ESCRITO COM BASE NO RECEPTOR E TRANSMISSOR DO
 * CONTROLE COM O ADESIVO DA NUBANK.
 * @Authors: Lucas Tomazeli, Renato Júnior, Felipe Mastromauro
 */

#include <Servo.h>

// MOTORS DEFINITION

Servo motorA;
Servo motorB;

// PINS DEFINITION

#define MOTORA_PIN 9
#define MOTORB_PIN 10

#define CH1_PIN 6
#define CH2_PIN 7

// "MAGIC NUMBERS" DEFINITION

#define DEBUG 1
#define DEADZONE 20
#define TIMEOUT 50000
#define UPPER_PULSE 1900
#define LOWER_PULSE 990
#define DEFAULT_PULSE 1470

// GLOBALS

unsigned long ch1Pulse, ch2Pulse;
unsigned long ch1Value, ch2Value;
int ch1Servo, ch2Servo;
int leftMotor, rightMotor;

// FUNCTION PROTOTYPES

void readRadio();         // Read radio receptor signal
void normalize();         // Normalization of readings
void mix();               // Mixing the controller commands
void controlMotors();     // Controlling motors
void debug();             // Serial monitor info

void setup() {
  if (DEBUG != 0){
    Serial.begin(9600);
  }

  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);

  motorA.attach(MOTORA_PIN);
  motorB.attach(MOTORB_PIN);
}

void loop() {
  readRadio();
  if(DEBUG != 0) {
    debug();
  }
  controlMotors();
}

void readRadio(){
  ch1Pulse = pulseIn(CH1_PIN, HIGH, TIMEOUT);
  ch2Pulse = pulseIn(CH2_PIN, HIGH, TIMEOUT);

  if ((ch1Pulse == 0) || (ch1Pulse - DEFAULT_PULSE > -DEADZONE && ch1Pulse - DEFAULT_PULSE < DEADZONE)){
    ch1Pulse = DEFAULT_PULSE;
  }
  if ((ch2Pulse == 0) || (ch2Pulse - DEFAULT_PULSE > -DEADZONE && ch2Pulse - DEFAULT_PULSE < DEADZONE)){
    ch2Pulse = DEFAULT_PULSE;
  }
  normalize();
}

// TODO: Refactor and test values in this function
void normalize(){
  ch1Value = constrain(ch1Pulse, LOWER_PULSE, UPPER_PULSE);
  ch2Value = constrain(ch2Pulse, LOWER_PULSE, UPPER_PULSE);

  // Normalize 
  if(ch1Value > DEFAULT_PULSE && ch1Value <= (UPPER_PULSE + DEFAULT_PULSE)/2){
    ch1Servo = map(ch1Value, DEFAULT_PULSE, (UPPER_PULSE + DEFAULT_PULSE)/2, 90, 45);
  }

  if(ch1Value > (UPPER_PULSE + DEFAULT_PULSE)/2 && ch1Value <= UPPER_PULSE){
    ch1Servo = map(ch1Value, UPPER_PULSE - (UPPER_PULSE - DEFAULT_PULSE), UPPER_PULSE, 45, 0);
  }
  
  if(ch1Value >= (LOWER_PULSE + DEFAULT_PULSE)/2 && ch1Value < DEFAULT_PULSE){
    ch1Servo = map(ch1Value, (LOWER_PULSE + DEFAULT_PULSE)/2, DEFAULT_PULSE, 135, 90);
  }
  
  if(ch1Value >= LOWER_PULSE && ch1Value < (LOWER_PULSE + DEFAULT_PULSE)/2){
    ch1Servo = map(ch1Value, LOWER_PULSE, (LOWER_PULSE + DEFAULT_PULSE)/2, 180, 135);
  }

  if(ch2Value > DEFAULT_PULSE && ch2Value <= (UPPER_PULSE + DEFAULT_PULSE)/2){
    ch2Servo = map(ch2Value, DEFAULT_PULSE, (UPPER_PULSE + DEFAULT_PULSE)/2, 89, 45);
  }

  if(ch2Value > (UPPER_PULSE + DEFAULT_PULSE)/2 && ch2Value <= UPPER_PULSE){
    ch2Servo = map(ch2Value, (UPPER_PULSE + DEFAULT_PULSE)/2, UPPER_PULSE, 45, 1);
  }
  
  if(ch2Value >= (LOWER_PULSE + DEFAULT_PULSE)/2 && ch2Value < DEFAULT_PULSE){
    ch2Servo = map(ch2Value, (LOWER_PULSE + DEFAULT_PULSE)/2, DEFAULT_PULSE, 135, 91);
  }
  
  if(ch2Value >= 1240 && ch2Value < (LOWER_PULSE + DEFAULT_PULSE)/2){
    ch2Servo = map(ch2Value, LOWER_PULSE, (LOWER_PULSE + DEFAULT_PULSE)/2, 179, 135);
  }

  if(ch1Value == DEFAULT_PULSE){
    ch1Servo = 90;
   }

   if(ch2Value == DEFAULT_PULSE){
    ch2Servo = 90;
   }
  mix();
}

void mix(){
  if(ch2Servo > 80){
    leftMotor = ch2Servo - (ch1Servo - 90);
    rightMotor = ch2Servo + (ch1Servo - 90);
  }

  if(ch2Servo < 100){
    leftMotor = ch2Servo - (ch1Servo - 90);
    rightMotor = ch2Servo + (ch1Servo - 90);
  }
  
  leftMotor = constrain(leftMotor, 0, 180);
  rightMotor = constrain(rightMotor, 0, 180);
}

void controlMotors(){
  motorA.write(leftMotor);
  motorB.write(rightMotor);
}

void debug(){
  Serial.print("SIGNAL CH1: ");
  Serial.print(ch1Pulse);
  Serial.print("| SIGNAL CH2: ");
  Serial.print(ch2Pulse);
  Serial.print("\n");

  Serial.print("VALUE CH1: ");
  Serial.print(ch1Value);
  Serial.print("| VALUE CH2: ");
  Serial.print(ch2Value);
  Serial.print("\n");

  Serial.print("LEFT MOTOR: ");
  Serial.print(leftMotor);
  Serial.print("| RIGHT MOTOR: ");
  Serial.print(rightMotor);
  Serial.print("\n");
}