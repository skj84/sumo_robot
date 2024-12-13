//RCA BIZBOT SUMO ROBOT RC (MANUAL CONTROL)
//Code name purpose
//Owner: Bizbot Technology
//web: https://bveetamini.com
//version: 1.0 (latest version if any)
//special isntructions for user
//PLEASE DO NOT DELETE THIS COMMENT//

// Notes of the Mario main theme (frequency in Hz). When turn on robot it play once.
// If you dont like it, you can comment this this.

// Notes of the Mario main theme (frequency in Hz). When turn on robot it play once.
// If you dont like it, you can comment this this.

#define BUZZER_PIN 13

#define NOTE_E7 2637
#define NOTE_G7 3136
#define NOTE_A7 3520
#define NOTE_B7 3951
#define NOTE_C7 2093
#define NOTE_E6 1319
#define NOTE_G6 1568
#define NOTE_A6 1760
#define NOTE_B6 1976
#define NOTE_C6 1047
#define NOTE_D6 1175
#define NOTE_FS6 1480
#define NOTE_GS6 1661
#define NOTE_DS6 1245

// Define durations
#define WHOLE_NOTE 1000
#define HALF_NOTE (WHOLE_NOTE / 2)
#define QUARTER_NOTE (WHOLE_NOTE / 4)
#define EIGHTH_NOTE (WHOLE_NOTE / 8)

// Melody and note durations
int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7, 0, NOTE_C7, NOTE_E7, 0, NOTE_G7, 0, 0,  0,
};

int noteDurations[] = {
  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
};

// Define buzzer and motor control pins
int buzzerPin = 13;
#define MOTOR_A_FWD  2
#define MOTOR_A_BWD  3
#define MOTOR_B_FWD  4
#define MOTOR_B_BWD  7

// RC receiver pins and motor enable pins (PWM)
#define RC_SPEED     8  
#define RC_STEERING  9  
#define ENA  5
#define ENB  6

int speedValue = 0;
int steeringValue = 0;

void setup() {
  pinMode(BUZZER_PIN,OUTPUT); //buzzer passive output

      // Play the melody
  for (int i = 0; i < sizeof(melody) / sizeof(int); i++) {
    playTone(melody[i], noteDurations[i]);
  }

  // Set pins for motors, receiver, and buzzer
  pinMode(MOTOR_A_FWD, OUTPUT);
  pinMode(MOTOR_A_BWD, OUTPUT);
  pinMode(MOTOR_B_FWD, OUTPUT);
  pinMode(MOTOR_B_BWD, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(RC_SPEED, INPUT);
  pinMode(RC_STEERING, INPUT);
  pinMode(buzzerPin, OUTPUT);
//  Serial.begin(9600);  // Debugging
}

void loop() {
  // Read RC receiver channels
  speedValue = pulseIn(RC_SPEED, HIGH, 25000);
  steeringValue = pulseIn(RC_STEERING, HIGH, 25000);
/*
  Serial.print("Speed Value: ");
  Serial.print(speedValue);
  Serial.print("  | Steering Value: ");
  Serial.println(steeringValue);
*/
  // Map RC values to motor speeds with dead zone near neutral
  int motorSpeed = map(speedValue, 1004, 1950, -255, 255);
  int steering = map(steeringValue, 1000, 2000, -255, 255);

  // Dead zone for motor speed
  if (abs(motorSpeed) < 15) {  // Adjust as needed to ensure zero movement at neutral
    motorSpeed = 0;
  }

  // Calculate motor speeds based on speed and steering
  int leftMotorSpeed = motorSpeed + steering;
  int rightMotorSpeed = motorSpeed - steering;

  // Control motor A based on left motor speed
  if (leftMotorSpeed > 0) {
    digitalWrite(MOTOR_A_FWD, HIGH);
    digitalWrite(MOTOR_A_BWD, LOW);
  } else if (leftMotorSpeed < 0) {
    digitalWrite(MOTOR_A_FWD, LOW);
    digitalWrite(MOTOR_A_BWD, HIGH);
  } else {
    digitalWrite(MOTOR_A_FWD, LOW);
    digitalWrite(MOTOR_A_BWD, LOW);
  }
  analogWrite(ENA, abs(leftMotorSpeed));

  // Control motor B based on right motor speed
  if (rightMotorSpeed > 0) {
    digitalWrite(MOTOR_B_FWD, HIGH);
    digitalWrite(MOTOR_B_BWD, LOW);
  } else if (rightMotorSpeed < 0) {
    digitalWrite(MOTOR_B_FWD, LOW);
    digitalWrite(MOTOR_B_BWD, HIGH);
  } else {
    digitalWrite(MOTOR_B_FWD, LOW);
    digitalWrite(MOTOR_B_BWD, LOW);
  }
  analogWrite(ENB, abs(rightMotorSpeed));

  // Buzzer control based on max speed
  if (motorSpeed == 255 || motorSpeed == -255) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  delay(5);  // Small delay for stability
}

// Function to stop motors
void stopMotors() {
  digitalWrite(MOTOR_A_FWD, LOW);
  digitalWrite(MOTOR_A_BWD, LOW);
  digitalWrite(MOTOR_B_FWD, LOW);
  digitalWrite(MOTOR_B_BWD, LOW);
}

// Function for chirping sound
void chirp() {
      // First chirp (rise and fall in pitch)
  for (int freq = 1000; freq <= 2000; freq += 50) {
    tone(BUZZER_PIN, freq, 50);  // Play rising tone
    delay(50);                  // Short delay between frequencies
  }
}


void playTone(int note, int duration) {
  if (note == 0) {
    delay(duration);
  } else {
    tone(BUZZER_PIN, note, duration);
    delay(duration * 1.30);
    noTone(BUZZER_PIN);
  }
}

