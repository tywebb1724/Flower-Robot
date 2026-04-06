//Libraries
#include <Arduino.h>
//#include <Servo.h>
#include <NewPing.h>


//Pins for sonar range finder
#define TRIGGER_PIN A1 //(2nd to rightmost bit of DDRC)
#define ECHO_PIN A0 //(Rightmost bit of DDRC)

//Pins for servos
//#define SERVO_PIN 9

//Parameters for servos
#define SERVO_START_ANGLE 90
#define SERVO_UP_LIMIT 180
#define SERVO_DOWN_LIMIT 0
//static Servo myServo;

//Sensor state definitions
#define DETECTION_YES 1
#define DETECTION_NO 0

//Setting up sonar range finder
#define MAX_DISTANCE 200
#define RANGE 120

//Servo definitions
#define SERVO_OPEN 0
#define SERVO_CLOSE 1

//Initializing states
int DetectionState = DETECTION_NO;
int ActionServo = SERVO_CLOSE;
static NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//Function to detect if there's someone by the flower
bool isDetected() {
  int sonar_distance = sonar.ping_cm();
  if (sonar_distance != 0) {
    return (sonar_distance < RANGE);
  }
  else {
    return false;
  }
}

//Convert the angle to number of ticks
uint16_t angleToTicks(int angle) {
  if (angle < 0) {
    angle = 0;
  }
  if (angle > 180) {
    angle = 180;
  }

  // Map 0..180 degrees to 1000..2000 us
  uint16_t pulse_us = 1000 + ((uint32_t)angle * 1000) / 180;

  // Each tick is 0.5 us, so multiply by 2
  return pulse_us * 2;
}

//Write servo angle
void servoWriteAngle(int angle) {
  OCR1A = angleToTicks(angle);
}

//Set up servo
void servoInit() {
  //Set PB1/OC1A (Pin 9) as output
  DDRB |= (1 << DDB1);

  //Clear control registers
  TCCR1A = 0;
  TCCR1B = 0;

  //Fast PWM mode 14; TOP = ICR1
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);

  // Non-inverting output on OC1A
  TCCR1A |= (1 << COM1A1);

  // Prescaler = 8
  TCCR1B |= (1 << CS11);

  // 20 ms period at 16 MHz with prescaler 8
  // Timer tick = 0.5 us
  // 20,000 us / 0.5 us = 40,000 ticks
  ICR1 = 39999;

  //Set initial angle
  OCR1A = angleToTicks(SERVO_START_ANGLE);
}

void FlowerPerception() {
  if (isDetected()) {
    DetectionState = DETECTION_YES;
  }
  else {
    DetectionState = DETECTION_NO;
  }
}


void FlowerPlanning() {
  if (DetectionState == DETECTION_YES) {
    ActionServo = SERVO_OPEN;
  }
  else {
    ActionServo = SERVO_CLOSE;
  }
}


void FlowerAction() {
  static int ServoAngle = SERVO_START_ANGLE;
  //If flower needs to open and it's not fully open, open more
  if (ActionServo == SERVO_OPEN && ServoAngle < SERVO_UP_LIMIT) {
    ServoAngle += 2;
    Serial.println("Open");
  }
  //If flower needs to close and it's not fully closed, close more
  else if (ActionServo == SERVO_CLOSE && ServoAngle > SERVO_DOWN_LIMIT) {
    ServoAngle -= 2;
    Serial.println("Close");
  }
  servoWriteAngle(ServoAngle);
}


void setup() {
  //Output pins
  //DDRC |= (1 << DDC1); 
  pinMode(TRIGGER_PIN, OUTPUT);
  //Input pins
  //DDRC &= ~(1 << DDC0); 
  pinMode(ECHO_PIN, INPUT);

  //Servo
  //myServo.attach(SERVO_PIN);
  //my.Servo.write(SERVO_START_ANGLE);
  servoInit();


  //Serial
  Serial.begin(9600);
}

void loop() {
  //Perception of surroundings
  FlowerPerception();
  //Planning action
  FlowerPlanning();
  //Action
  FlowerAction();

  delay(30);
}


