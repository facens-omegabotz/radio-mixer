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

#define CH1_PIN 15
#define CH2_PIN 16

// "MAGIC NUMBERS" DEFINITION

#define DEBUG 1
#define DEADZONE 20
#define TIMEOUT 50000
#define UPPER_PULSE 2000
#define LOWER_PULSE 1020
#define MIDDLE_PULSE 1510

#define interval 5000

#define MOTORA_LOWER 800
#define MOTORA_UPPER 2200

#define MOTORB_LOWER 700
#define MOTORB_UPPER 2300

#define MOTOR_LOWER_POINT 800
#define MOTOR_LOWER_MIDDLE_POINT 1150
#define MOTOR_MIDDLE_POINT 1500
#define MOTOR_MIDDLE_UPPER_POINT 1850
#define MOTOR_UPPER_POINT 2200



// GLOBALS

unsigned long ch1Pulse, ch2Pulse;
unsigned long ch1Value, ch2Value;
int ch1Servo, ch2Servo;
int leftMotor, rightMotor;
int statCounter;
unsigned long previousMillis = 0;

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

  if ((ch1Pulse == 0) || (ch1Pulse - MIDDLE_PULSE > -DEADZONE && ch1Pulse - MIDDLE_PULSE < DEADZONE)){
    ch1Pulse = MIDDLE_PULSE;
  }
  if ((ch2Pulse == 0) || (ch2Pulse - MIDDLE_PULSE > -DEADZONE && ch2Pulse - MIDDLE_PULSE < DEADZONE)){
    ch2Pulse = MIDDLE_PULSE;
  }
  normalize();
}

// TODO: Refactor and test values in this function
void normalize(){
  ch1Value = constrain(ch1Pulse, LOWER_PULSE, UPPER_PULSE);
  ch2Value = constrain(ch2Pulse, LOWER_PULSE, UPPER_PULSE);

  // Normalize 
  if(ch1Value > MIDDLE_PULSE && ch1Value <= (UPPER_PULSE + MIDDLE_PULSE)/2){
    ch1Servo = map(ch1Value, MIDDLE_PULSE, (UPPER_PULSE + MIDDLE_PULSE)/2, MOTOR_MIDDLE_POINT, MOTOR_LOWER_MIDDLE_POINT);
  }

  if(ch1Value > (UPPER_PULSE + MIDDLE_PULSE)/2 && ch1Value <= UPPER_PULSE){
    ch1Servo = map(ch1Value, UPPER_PULSE - (UPPER_PULSE - MIDDLE_PULSE), UPPER_PULSE, MOTOR_LOWER_MIDDLE_POINT, MOTOR_LOWER_POINT);
  }
  
  if(ch1Value >= (LOWER_PULSE + MIDDLE_PULSE)/2 && ch1Value < MIDDLE_PULSE){
    ch1Servo = map(ch1Value, (LOWER_PULSE + MIDDLE_PULSE)/2, MIDDLE_PULSE, MOTOR_MIDDLE_UPPER_POINT, MOTOR_MIDDLE_POINT);
  }
  
  if(ch1Value >= LOWER_PULSE && ch1Value < (LOWER_PULSE + MIDDLE_PULSE)/2){
    ch1Servo = map(ch1Value, LOWER_PULSE, (LOWER_PULSE + MIDDLE_PULSE)/2, MOTOR_UPPER_POINT, MOTOR_MIDDLE_UPPER_POINT);
  }

  if(ch2Value > MIDDLE_PULSE && ch2Value <= (UPPER_PULSE + MIDDLE_PULSE)/2){
    ch2Servo = map(ch2Value, MIDDLE_PULSE, (UPPER_PULSE + MIDDLE_PULSE)/2, MOTOR_MIDDLE_POINT, MOTOR_LOWER_MIDDLE_POINT);
  }

  if(ch2Value > (UPPER_PULSE + MIDDLE_PULSE)/2 && ch2Value <= UPPER_PULSE){
    ch2Servo = map(ch2Value, (UPPER_PULSE + MIDDLE_PULSE)/2, UPPER_PULSE, MOTOR_LOWER_MIDDLE_POINT, MOTOR_LOWER_POINT);
  }
  
  if(ch2Value >= (LOWER_PULSE + MIDDLE_PULSE)/2 && ch2Value < MIDDLE_PULSE){
    ch2Servo = map(ch2Value, (LOWER_PULSE + MIDDLE_PULSE)/2, MIDDLE_PULSE, MOTOR_MIDDLE_UPPER_POINT, MOTOR_MIDDLE_POINT);
  }
  
  if(ch2Value >= 1240 && ch2Value < (LOWER_PULSE + MIDDLE_PULSE)/2){
    ch2Servo = map(ch2Value, LOWER_PULSE, (LOWER_PULSE + MIDDLE_PULSE)/2, MOTOR_UPPER_POINT, MOTOR_MIDDLE_UPPER_POINT);
  }

  if(ch1Value == MIDDLE_PULSE){
    ch1Servo = 1500;
   }

   if(ch2Value == MIDDLE_PULSE){
    ch2Servo = 1500;
   }
  mix();
}

void mix(){
  if(ch2Servo > 1300){
    leftMotor = ch2Servo - (ch1Servo - 1500);
    rightMotor = ch2Servo + (ch1Servo - 1500);
  }

  if(ch2Servo < 1700){
    leftMotor = ch2Servo - (ch1Servo - 1500);
    rightMotor = ch2Servo + (ch1Servo - 1500);
  }
  
  leftMotor = constrain(leftMotor, MOTORA_LOWER, MOTORA_UPPER);
  rightMotor = constrain(rightMotor, MOTORB_LOWER, MOTORB_UPPER);
}

void controlMotors(){
  motorA.writeMicroseconds(leftMotor);
  motorB.writeMicroseconds(rightMotor);
}

void debug(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {        
    previousMillis = currentMillis;      
    statCounter=statCounter+1; //increment counter
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
    
    if(statCounter=10){
      statCounter=0;  //reset counter
    }    
  }
}