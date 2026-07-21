#include "config.h"

// =============================================================================
// GLOBAL AND STATIC VARIABLES
// =============================================================================

static const char *messages[MESSAGES_LEN];
static int servoAngle;
static int sonarDistance;
static int currentMessage;

static ServoState currentServoState;

//Hardware Drivers
static Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
static LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
static NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// =============================================================================
// HARDWARE HELPER FUNCTIONS
// =============================================================================

float get_pin_voltage(int pin) {
  return analogRead(pin) * VOLTAGE_CONVERSION;
}

//Function to detect if there's someone by the flower
bool is_detected() {
  //Get distance from sonar range finder
  sonarDistance = sonar.ping_cm();
  return (sonarDistance != 0) && (sonarDistance < RANGE);
}

//Detect whether the switch is off
bool is_off() {
  return (get_pin_voltage(TRANS_INPUT) > 1.0f);
}

// Convert angle (0–180) to pulse length
int angle_to_pulse(int angle) {
  return map(angle, 0, MAX_ANGLE, SERVO_MIN, SERVO_MAX);
}

void servo_set_angle() {
  //Go through each servo and set their angles
  for (int i = 0; i <= FINAL_SERVO; i += SERVO_SPACING) {
    pwm.setPWM(i, 0, angle_to_pulse(servoAngle));
  }
}

// =============================================================================
// INITIALIZATION FUNCTIONS
// =============================================================================

void messages_init() {
  messages[0] = "Hi beautiful!";
  messages[1] = "You're perfect";
  messages[2] = "Baddie!";
  currentMessage = 0;
}

void servo_init() {
  //Start communication with servo driver
  Wire.begin();
  //Initialize pwm communication with servos
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);
  //Start the servos at the maximum angle (closed state)
  servoAngle = SERVO_MAX_ANGLE;
  servo_set_angle();
}

void lcd_init() {
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.setCursor(0, 1);
  lcd.print(messages[currentMessage]);
}

void setup() {
  //Initialize pins
  pinMode(TRANS_OUTPUT, OUTPUT);
  pinMode(TRANS_INPUT, INPUT_PULLUP);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  //Keep power on
  digitalWrite(TRANS_OUTPUT, HIGH);
  //Initialization
  messages_init();
  servo_init();
  lcd_init();
  //Initialize state of servos
  currentServoState = SERVO_CLOSE;
  //Serial
  Serial.begin(BAUD_RATE);
  //Slight delay before loop
  delay(SETUP_DELAY);
}

// =============================================================================
// MAIN CONTROL LOOP
// =============================================================================

void loop() {
  //Transition
  switch (currentServoState) {
    //Closing the servos
    case SERVO_CLOSE:
      //If sensor detects something, open servos
      if (is_detected()) {
        currentServoState = SERVO_OPEN;
        //If the servos close enough, change the messsage
        if (servoAngle >= SERVO_THRESHOLD) {
          //Change messages
          currentMessage = (currentMessage + 1) % MESSAGES_LEN;
          //Print the new message
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print(messages[currentMessage]);
        }
      }
      //If switch turns off, power off protocol
      if (is_off()) {
        currentServoState = SERVO_OFF;
      }
      break;

    //Opening the servos
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
      if (servoAngle < SERVO_MAX_ANGLE) {
        servoAngle += SERVO_INCREMENT;
      }
      //Set the servos
      servo_set_angle();
      break;

    //Closing the servos
    case SERVO_OPEN:
      //If flower needs to open and it's not fully open, open more
      if (servoAngle > SERVO_MIN_ANGLE) {
        servoAngle -= SERVO_INCREMENT;
      }
      //Set the servos
      servo_set_angle();
      break;

    //Powering off
    case SERVO_OFF:
      //If flower not fully closed, close more
      if (servoAngle < SERVO_MAX_ANGLE) {
        servoAngle += POWER_OFF_INCREMENT;
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