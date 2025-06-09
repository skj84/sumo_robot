/*Please do not remove this comment for better future
//use with BIZBOT RCA SUMO ROBOT CONTROLLER V2.4
//Please contact hello@bveetamini.com for technical support

We respects all the originality. Used this code as is.
Good luck.

*/

// Almost same as URC10 Motor Driver Pins
#define leftPWM 5
#define leftDir1 2
#define leftDir2 3

#define rightPWM 6
#define rightDir1 4
#define rightDir2 7

// Receiver pins
#define RX_CH_1 9   // Steering Channel (Turn Left / Right)
#define RX_CH_2 8  // Throttle channel (Move Forward / Backward)

// Parameters
#define deadZone 40  // Anything between -20 and 20 is STOP

int throttleCH, steeringCH;
int leftMotorSpeed, rightMotorSpeed;

int buzzerPin = 13; 
void sound();

void setup() {
  Serial.begin(9600);
  pinMode(leftPWM, OUTPUT);
  pinMode(leftDir1, OUTPUT);
  pinMode(leftDir2, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(rightDir1, OUTPUT);
  pinMode(rightDir2, OUTPUT);
  pinMode(RX_CH_1, INPUT);
  pinMode(RX_CH_2, INPUT);

  digitalWrite(leftDir1, LOW); 
  digitalWrite(leftDir2, LOW); 
  analogWrite(leftPWM, 0);
  digitalWrite(rightDir1, LOW);
  digitalWrite(rightDir2, LOW);
  analogWrite(rightPWM, 0);

  sound();
  sound();
}

// Main Loop
void loop() {

  // Read pulse width from receiver
  throttleCH = pulseIn(RX_CH_2, HIGH, 25000);
  steeringCH = pulseIn(RX_CH_1, HIGH, 25000);
  // Serial.print(" steeringCH : ");
  // Serial.println(steeringCH);
  // Serial.print(" throttleCH: ");
  // Serial.println(steeringCH);

  // Convert to PWM value (-255 to 255)
  throttleCH = pulseToPWM(throttleCH);
  steeringCH = pulseToPWM(steeringCH);

  // Channel 1 and Channel 2 Mixing
  leftMotorSpeed = throttleCH + steeringCH;
  rightMotorSpeed = throttleCH - steeringCH;

  // Drive motor
  drive(leftMotorSpeed, rightMotorSpeed);

  //code below is for debug. please comment when not use
  //Serial.print("leftMotorSpeed : ");
  //Serial.println(leftMotorSpeed);
  //Serial.print("   rightMotorSpeed: ");
  //Serial.println(rightMotorSpeed);

  //Serial.print("steering=");
  //Serial.println(steeringCH);
  //Serial.print("throttle=");
  //Serial.println(throttleCH);
  //delay(100);
  
}

// Positive for forward, negative for reverse
void drive(int leftVal, int rightVal) {

  // Limit speed between -255 and 255
  leftVal = constrain(leftVal, -200, 200);
  rightVal = constrain(rightVal, -200, 200);

  // Set direction for left motor
  if (leftVal == 0) {
    digitalWrite(leftDir1, 1);
    digitalWrite(leftDir2, 1);
    analogWrite(leftPWM, 255);

  } else if (leftVal > 0) {
    digitalWrite(leftDir1, 0);
    digitalWrite(leftDir2, 1);
    analogWrite(leftPWM, abs(leftVal));
  } else if (leftVal < 0) {
    digitalWrite(leftDir1, 1);
    digitalWrite(leftDir2, 0);
    analogWrite(leftPWM, abs(leftVal));
  }
  else{
    breakMotor();
  }


  // Set direction for right motor
  if (rightVal == 0) {
    digitalWrite(rightDir1, 1);
    digitalWrite(rightDir2, 1);
    analogWrite(rightPWM, 255);

  } else if (rightVal > 0) {
    digitalWrite(rightDir1, 0);
    digitalWrite(rightDir2, 1);
    analogWrite(rightPWM, abs(rightVal));
  } else if (rightVal < 0){
    digitalWrite(rightDir1, 1);
    digitalWrite(rightDir2, 0);
    analogWrite(rightPWM, abs(rightVal));
  }
  
  else{
    breakMotor();
  }
}

// Convert RC pulse value to motor PWM value
int pulseToPWM(int pulse) {

  // If we're receiving numbers, convert them to motor PWM
  if (pulse > 1000) {
    pulse = map(pulse, 1000, 2000, -500, 500);
    pulse = constrain(pulse, -255, 255);
  } else {
    pulse = 0;
  }

  // Anything in deadzone should stop the motor
  if (abs(pulse) <= deadZone) {
    pulse = 0;
  }
  return pulse;
}


/*
This functions to create wheesel sound on active buzzer
*/
void sound() {
  // Generate a chirp-like sound
  for (int i = 1000; i <= 1500; i += 100) { // Ascending frequency
    tone(buzzerPin, i); // Play the tone
    delay(25);          // Short delay between tones
  }  
  for (int i = 1500; i >= 1000; i -= 100) { // Descending frequency
    tone(buzzerPin, i); // Play the tone
    delay(25);          // Short delay between tones
  }
  noTone(buzzerPin);  // Stop the sound after the chirp
}

void breakMotor(){
    digitalWrite(rightDir1, 1);
    digitalWrite(rightDir2, 1);
    analogWrite(rightPWM, 255);
    digitalWrite(rightDir1, 1);
    digitalWrite(rightDir2, 1);
    analogWrite(rightPWM, 255);
}
