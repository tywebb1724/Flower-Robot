#ifndef CONFIG_H
#define CONFIG_H

//Libraries
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <NewPing.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

//Baud rate
#define BAUD_RATE 9600
//Delays
#define LOOP_DELAY 100
#define SETUP_DELAY 10
#define POWER_OFF_DELAY 500
//Pins for transistor power and reading
#define TRANS_OUTPUT A3
#define TRANS_INPUT A2
//Conversion from raw input to voltage
#define VOLTAGE_CONVERSION (5.0 / 1023.0)
//Pins for sonar range finder
#define TRIGGER_PIN 8
#define ECHO_PIN 9
//Sensor macros
#define MAX_DISTANCE 300
#define RANGE 120
//Parameters variables for servos
#define PWM_FREQ 60
#define SERVO_MIN  150 // 0 degrees
#define SERVO_MAX  600 // 180 degrees
#define MAX_ANGLE 180
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180
#define SERVO_THRESHOLD 90
#define SERVO_INCREMENT 4
#define POWER_OFF_INCREMENT (2 * SERVO_INCREMENT)
#define SERVO_SPACING 4
#define FINAL_SERVO 12
//LCD macros
#define LCD_COLS 16
#define LCD_ROWS 2
//Number of LCD messages
#define MESSAGES_LEN 3


typedef enum {
    SERVO_CLOSE,
    SERVO_OPEN,
    SERVO_OFF
} servoState;

#endif