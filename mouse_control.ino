/*
 * PW3 Autonomous Mouse - PD Position Control
 * Author: ma3403 - Mervyn Asokantha
 * Unit: EE22005 Engineering Practice and Design
 * University of Bath
 *
 * Two 220uH pickup coils mounted either side of centre at the front.
 * Each coil feeds: RLC bandpass filter (20kHz) -> TL071 precision
 * rectifier -> Arduino ADC pin, giving a DC voltage proportional
 * to signal strength on each side.
 *
 * Error = Left - Right
 *   error = 0  -> centred, go straight
 *   error > 0  -> drifted right, steer left
 *   error < 0  -> drifted left, steer right
 */

// ── PIN DEFINITIONS ──────────────────────────────────────────────────────────
const int LEFT_COIL_PIN   = A0;  // Left coil rectified DC output
const int RIGHT_COIL_PIN  = A1;  // Right coil rectified DC output
const int LEFT_MOTOR_PIN  = 9;   // PWM to left MOSFET driver
const int RIGHT_MOTOR_PIN = 10;  // PWM to right MOSFET driver

// ── PD GAINS ─────────────────────────────────────────────────────────────────
float Kp = 1.0;   // Proportional gain
float Kd = 0.05;  // Derivative gain

// ── MOTOR SPEED LIMITS ───────────────────────────────────────────────────────
const int BASE_SPEED = 140;  // Straight line speed (0-255)
const int MAX_SPEED  = 210;  // Upper PWM cap
const int MIN_SPEED  = 50;   // Lower PWM cap - prevents stall

// ── STATE ────────────────────────────────────────────────────────────────────
float lastError = 0;

void setup() {
  pinMode(LEFT_MOTOR_PIN,  OUTPUT);
  pinMode(RIGHT_MOTOR_PIN, OUTPUT);
  Serial.begin(9600);
  delay(2000);  // 2 second pause before moving
}

void loop() {

  // 1. Read both coil voltages (0-1023)
  int leftVal  = analogRead(LEFT_COIL_PIN);
  int rightVal = analogRead(RIGHT_COIL_PIN);

  // 2. Calculate position error
  float error = (float)(leftVal - rightVal);

  // 3. Calculate derivative
  float derivative = error - lastError;

  // 4. PD correction
  float correction = (Kp * error) + (Kd * derivative);

  // 5. Apply to motors
  int leftSpeed  = BASE_SPEED - (int)correction;
  int rightSpeed = BASE_SPEED + (int)correction;

  // 6. Clamp to safe range
  leftSpeed  = constrain(leftSpeed,  MIN_SPEED, MAX_SPEED);
  rightSpeed = constrain(rightSpeed, MIN_SPEED, MAX_SPEED);

  // 7. Drive motors
  analogWrite(LEFT_MOTOR_PIN,  leftSpeed);
  analogWrite(RIGHT_MOTOR_PIN, rightSpeed);

  // 8. Save error for next derivative calculation
  lastError = error;

  // 9. Serial debug - remove before race
  Serial.print("L:"); Serial.print(leftVal);
  Serial.print(" R:"); Serial.print(rightVal);
  Serial.print(" Err:"); Serial.print(error);
  Serial.print(" Cor:"); Serial.print(correction);
  Serial.print(" LP:"); Serial.print(leftSpeed);
  Serial.print(" RP:"); Serial.println(rightSpeed);

  delay(10);  // 100Hz control loop
}


Initial commit: PD control code
