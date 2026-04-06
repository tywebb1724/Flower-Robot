//Libraries
#include <Arduino.h>
#include <Servo.h>
#include <NewPing.h>


//Pins for sonar range finder
#define TRIGGER_PIN A1
#define ECHO_PIN A0

//Pins for servos
#define SERVO_PIN 9

//Parameters for servos
#define SERVO_START_ANGLE 0
#define SERVO_UP_LIMIT 180
#define SERVO_DOWN_LIMIT 0
static Servo myServo;

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
  myServo.write(ServoAngle);
}


void setup() {
  //Output pins
  pinMode(TRIGGER_PIN, OUTPUT);
  //Input pins
  pinMode(ECHO_PIN, INPUT);

  //Servo
  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_START_ANGLE);

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


