#include "config.h"

//Array to store the messages
static const char *messages[MESSAGES_LEN];
//Current servo angle
static int servoAngle;
//Distance returned by sonar range finder
static int sonarDistance;
//Index of the current message
static int currentMessage;
//Current state of the servos
static servoState currentServoState;
//PWM servo variable
static Adafruit_PWMServoDriver pwm;

//Initialize LCD screen
static LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
//Initialize sonar range finder
static NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);


//Get voltage of input pin
float get_pin_voltage(int pin) {
  return analogRead(pin) * VOLTAGE_CONVERSION;
}

//Function to detect if there's someone by the flower
bool is_detected() {
  //Get distance from sonar range finder
  sonarDistance = sonar.ping_cm();
  //If there is a valid distance from the sensor
  if (sonarDistance != 0) {
    //True if distance is within the range of the sensor
    return (sonarDistance < RANGE);
  }
  else {
    return false;
  }
}

// Convert angle (0–180) to pulse length
int angle_to_pulse(int angle) {
  return map(angle, 0, MAX_ANGLE, SERVO_MIN, SERVO_MAX);
}

//Set the servos to the correct angle
void servo_set_angle() {
    //Go through each servo and set their angles
    for (int i = 0; i <= FINAL_SERVO; i += SERVO_SPACING) {
        pwm.setPWM(i, 0, angle_to_pulse(servoAngle));
    }
}

//Detect whether the switch is off
bool is_off() {
  return (get_pin_voltage(TRANS_INPUT) > 1);
}

//Initialize the messages
void messages_init() {
  //Initialize each message
  messages[0] = "Hi beautiful!";
  messages[1] = "You're perfect";
  messages[2] = "Baddie!";
  //Set the index of the current message
  currentMessage = 0;
}

//Initialize servos
void servo_init() {
  //Start communication with servo driver
  Wire.begin();
  //Initialize pwm communication with servos
  pwm = Adafruit_PWMServoDriver();
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);
  //Start the servos at the minimum angle
  servoAngle = SERVO_MIN_ANGLE;
  servo_set_angle();
}

//Initialize lcd screen
void lcd_init() {
  lcd.begin(LCD_COLS, LCD_ROWS);
  //Set the cursor to bottom row
  lcd.setCursor(0 , 1);
  //Write the first message
  lcd.print(messages[currentMessage]);
}

//Setup systems
void setup() {
  //Initialize pins
  pinMode(TRANS_OUTPUT, OUTPUT);
  pinMode(TRANS_INPUT, INPUT_PULLUP);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  //Keep power on
  digitalWrite(TRANS_OUTPUT, HIGH);
  //Initialize messages
  messages_init();
  //Initialize servos
  servo_init();
  //Display
  lcd_init();
  //Initialize state of servos
  currentServoState = SERVO_CLOSE;
  //Serial
  Serial.begin(BAUD_RATE);
  //Slight delay before loop
  delay(SETUP_DELAY);
}

//Loop function
void loop() {

  //Transition
  switch (currentServoState) {
        
    //Opening the servos
    case SERVO_CLOSE:
      //If sensor detects something, open servos
      if (is_detected()) {
        currentServoState = SERVO_OPEN;
        //If the servos close enough, change the messsage
        if (servoAngle <= SERVO_THRESHOLD) {
          //If it's the last message, change to the first one
          if (currentMessage == MESSAGES_LEN - 1) {
          currentMessage = 0;
          }
          //Change to the next message
          else {
            currentMessage += 1;
          }
          //Print the new message
          lcd.print(messages[currentMessage]);
        }
      }
      //If switch turns off, power off protocol
      if (is_off()) {
        currentServoState = SERVO_OFF;
      }
      break;

    //Closing the servos
    case SERVO_OPEN:
      //If sensor doesn't detect something, close servos
      if (!is_detected()) {
        currentServoState = SERVO_CLOSE;
      }
      //If switch turns off, power off protocol
      if (is_off()) {
        currentServoState = SERVO_OFF;
      }
      break;

    //Powering off
    case SERVO_OFF:
      break;

    }

  //Action
  switch (currentServoState) {

    //Opening the servos
    case SERVO_CLOSE:
      //If flower needs to close and it's not fully closed, close more
      if (servoAngle > SERVO_MIN_ANGLE) {
        servoAngle -= SERVO_INCREMENT;
      }
      //Set the servos
      servo_set_angle();
      break;

    //Closing the servos
    case SERVO_OPEN:
      //If flower needs to open and it's not fully open, open more
      if (servoAngle < SERVO_MAX_ANGLE) {
        servoAngle += SERVO_INCREMENT;
      }
      //Set the servos
      servo_set_angle();
      break;

    //Powering off
    case SERVO_OFF:
      //If flower not fully closed, close more
      if (servoAngle > SERVO_MIN_ANGLE) {
        servoAngle -= POWER_OFF_INCREMENT;
      }
      //Once servos fully closed, power off
      else {
        //Final wait delay
        delay(POWER_OFF_DELAY);
        //Power off
        digitalWrite(TRANS_OUTPUT, LOW);
      }
      //Set the servos
      servo_set_angle();
      break;
            
    }
  
  //Delay between loops
  delay(LOOP_DELAY);
}



