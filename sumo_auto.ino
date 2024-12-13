//RCA BIZBOT SUMO
//Code name purpose
//Owner: Bizbot Technology
//web: https://bveetamini.com
//version: 1.0 (latest version if any)
//special isntructions for user
//PLEASE DO NOT DELETE THIS COMMENT//

// Notes of the Mario main theme (frequency in Hz). When turn on robot it play once.
// If you dont like it, you can comment this this.

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



// Declare all the variables being used here:
int frontIR;
int lowRightIR;
int hardRightIR;
int lowLeftIR;
int hardLeftIR;
int rightEdgeIR;
int leftEdgeIR;

// Thresholds for edge detection
const int edgeThreshold = 500; // Adjust based on testing

// Motor control pins
const int E1 = 2; // PWM pin for Motor A
const int M1 = 3; // Direction pin for Motor A
const int E2 = 4; // PWM pin for Motor B
const int M2 = 7; // Direction pin for Motor B

const int ENA = 5;
const int ENB = 6;

// Push button pin for robot start
const int START_BUTTON = 12; // Push button on pin D12

// DIP switch pins for strategy
const int DIP1 = 10;
const int DIP2 = 11;

#define BUZZER_PIN 13

// Strategy variable
int strategy = 0; // Default strategy

// Flags for operation
bool waitingForStart = true;
bool strategyExecuted = false;
bool edgeActionActive = false; // Declare edgeActionActive here

void setup() {
  pinMode(BUZZER_PIN,OUTPUT); //buzzer passive output
  //chirp();            //test buzzer with chirp sound

    // Play the melody
  for (int i = 0; i < sizeof(melody) / sizeof(int); i++) {
    playTone(melody[i], noteDurations[i]);
  }


  // Set motor control pins as output
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(E1, OUTPUT);
  pinMode(E2, OUTPUT);

  // Configure IR sensor pins as inputs
  pinMode(A0, INPUT); // Front IR sensor
  pinMode(A1, INPUT); // Low right IR sensor
  pinMode(A2, INPUT); // Hard right IR sensor
  pinMode(A3, INPUT); // Low left IR sensor
  pinMode(A4, INPUT); // Hard left IR sensor
  // A5 and A6 for edge sensors are analog inputs, no need to set pinMode

  // Configure DIP switch pins as inputs
  pinMode(DIP1, INPUT_PULLUP);
  pinMode(DIP2, INPUT_PULLUP);

  // Configure the start push button as input
  pinMode(START_BUTTON, INPUT_PULLUP); // Use INPUT_PULLUP to handle the push button

  Serial.begin(115200); // Initialize serial communication for debugging
}

void loop() {
  // Check if the start button is pressed
  if (digitalRead(START_BUTTON) == LOW) {
    // If button is pressed, start the 5-second delay
    if (waitingForStart) {
      delay(5400); // Wait for 5.4 seconds
      waitingForStart = false; // Disable waiting
    }

    // Read the strategy from the DIP switch
    strategy = (digitalRead(DIP1) == LOW ? 1 : 0) | (digitalRead(DIP2) == LOW ? 2 : 0);

    // Read IR sensors (still treated as digital)
    frontIR = digitalRead(A0);
    lowRightIR = digitalRead(A1);
    hardRightIR = digitalRead(A2);
    lowLeftIR = digitalRead(A3);
    hardLeftIR = digitalRead(A4);

    // Read edge sensors as analog
    rightEdgeIR = analogRead(A5);
    leftEdgeIR = analogRead(A6);

    // Check edge detection and execute edge action
    if (rightEdgeIR < edgeThreshold || leftEdgeIR < edgeThreshold) {
      edgeActionActive = true;  // Activate edge action flag
      if (rightEdgeIR < edgeThreshold && leftEdgeIR < edgeThreshold) {
        reverseFor2Seconds();
      } else if (rightEdgeIR < edgeThreshold) {
        reverseFor2Seconds();
      } else if (leftEdgeIR < edgeThreshold) {
        reverseFor2Seconds();
      }
      edgeActionActive = false; // Deactivate edge action flag after done
      return;
    }

    // Execute the strategy only once after the 5-second delay
    if (!strategyExecuted) {
      executeStrategy(strategy);
      strategyExecuted = true; // Mark the strategy as executed
      return; // Ensure strategy execution before opponent detection
    }

    // If edge action is active, skip opponent detection
    if (edgeActionActive) {
      return;
    }

    // Continuous forward and backward movement when frontIR detects an opponent in case 3 (11)
    if (strategy == 3 && frontIR == LOW) {
      forwardAndBackward();
      return;
    }

    // Decision making based on opponent detection
    if (frontIR == LOW) {
      executeStrategy(0); // Continue default strategy after initial movement
    } else if (lowRightIR == LOW || hardRightIR == LOW) {
      turnRight();
    } else if (lowLeftIR == LOW || hardLeftIR == LOW) {
      turnLeft();
    } else {
      stopMoving();
    }
  } else {
    // If the button is not pressed, keep the robot in standby mode
    stopMoving(); // Ensure the robot is stopped
    waitingForStart = true; // Reset waiting status to start the process again
  }
}

//Function to execute strategy based on DIP switch position
//Below here are the startegy to be executed
//Make sure to check your dip switch 

void executeStrategy(int strategy) {
  switch (strategy) {
    case 0:
      forward();
      break;
    case 1:
      turnLeft();
      delay(50);
      forward();
      delay(500);
      turnRight();
      delay(200);
      forward();
      delay(100);
      turnRight();
      delay(200);
      forward();
      delay(100);
      stopMoving(); 
      break;
    case 2:
      turnRight();
      delay(50);
      forward();
      delay(400);
      turnLeft();
      delay(200);
      forward();
      delay(100);
      stopMoving(); 
      break;
    case 3:
      // Initial forward and backward, then continuous in loop
      forwardAggressive();
      delay(200);
      reverseFor0_5Seconds();
      stopMoving(); 
      break;
    default:
      stopMoving();
  }
}

// Function definitions

void forward() {
  Serial.println("Moving forward");
  analogWrite(ENA,180);   //speed 180
  digitalWrite(E1, HIGH); // Motor A forward
  digitalWrite(M1, LOW); // Ensure Motor A moves forward

  analogWrite(ENB,180);  //speed 180
  digitalWrite(E2, HIGH); // Motor B forward
  digitalWrite(M2, LOW); // Ensure Motor B moves forward
}

void forwardAndBackward() {
  Serial.println("Forward and backward");
  forward();
  delay(200);
  reverseFor0_5Seconds();
}

void forwardAggressive() {
  Serial.println("Aggressive mode: Moving forward faster");
  analogWrite(ENA,255);   //speed full
  digitalWrite(E1, HIGH); // Motor A forward
  digitalWrite(M1, LOW); // Ensure Motor A moves forward

  analogWrite(ENB,255);  //speed full
  digitalWrite(E2, HIGH); // Motor B forward
  digitalWrite(M2, LOW); // Ensure Motor B moves forward
}

void forwardDefensive() {
  Serial.println("Defensive mode: Moving forward slower");
  analogWrite(ENA,100);   //speed 180
  digitalWrite(E1, HIGH); // Motor A forward
  digitalWrite(M1, LOW); // Ensure Motor A moves forward

  analogWrite(ENB,100);  //speed 180
  digitalWrite(E2, HIGH); // Motor B forward
  digitalWrite(M2, LOW); // Ensure Motor B moves forward
}

void evasiveManeuver() {
  Serial.println("Evasive maneuver: Moving erratically");
  turnLeft(); // Start with a left turn
  delay(200);
  turnRight(); // Then a quick right turn
  delay(200);
  forwardAggressive(); // Then move forward aggressively
}

void turnRight() {
  Serial.println("Turning right");
  analogWrite(ENA,255);
  digitalWrite(E1, LOW); // Motor A forward
  digitalWrite(M1, HIGH); // Motor A moves forward

  analogWrite(ENB,255);
  digitalWrite(E2, HIGH); // Motor B slower for a right turn
  digitalWrite(M2, LOW); // Motor B moves backward for sharper turn
}

void turnLeft() {
  Serial.println("Turning left");
  analogWrite(ENA,255);
  digitalWrite(E1, HIGH); // Motor A slower for a left turn
  digitalWrite(M1, LOW); // Motor A moves backward for sharper turn

  analogWrite(ENB,255);
  digitalWrite(E2, LOW); // Motor B forward
  digitalWrite(M2, HIGH); // Motor B moves forward
}

void reverseFor2Seconds() {
  Serial.println("Reversing for 2 seconds");
  analogWrite(ENA,255);
  digitalWrite(E1, LOW); // Motor A reverse
  digitalWrite(M1, HIGH); // Motor A moves backward

  analogWrite(ENB,255);
  digitalWrite(E2, LOW); // Motor B reverse
  digitalWrite(M2, HIGH); // Motor B moves backward

  delay(500); // Reverse for 2 seconds
}

void reverseFor0_5Seconds() {
  Serial.println("Reversing for 0.5 seconds");
  analogWrite(ENA,60);
  digitalWrite(E1, LOW); // Motor A reverse
  digitalWrite(M1, HIGH); // Motor A moves backward

  analogWrite(ENB,60);
  digitalWrite(E2, LOW); // Motor B reverse
  digitalWrite(M2, HIGH); // Motor B moves backward
  delay(200); // Reverse for 0.5 seconds
}

void stopMoving() {
  Serial.println("Stopping");
  analogWrite(ENA, 0); // Stop Motor A
  analogWrite(ENB, 0); // Stop Motor B
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
