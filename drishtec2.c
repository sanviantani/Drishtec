/* 
 * Drishtec - Phase 1 on Arduino Uno
 * One HC-SR04 + Buzzer. Range: 5 cm to 30 cm.
 * 
 * Wiring:
 *   - HC-SR04 VCC  -> 5V
 *   - HC-SR04 GND  -> GND
 *   - HC-SR04 TRIG -> Pin 4
 *   - HC-SR04 ECHO -> Pin 5
 *   - Buzzer (+)   -> Pin 8
 *   - Buzzer (-)   -> GND
 */

// Pin Definitions (Updated)
const int TRIG_PIN   = 4;
const int ECHO_PIN   = 5;
const int BUZZER_PIN = 8;

// Distance Range Constants (in centimeters)
const float MIN_CM = 5.0;   // Closest distance (fastest beeps)
const float MAX_CM = 30.0;  // Farthest distance range (slowest beeps)

// Buzzer & Timing Configuration
const int BEEP_FREQ_HZ   = 1000;
const int BEEP_LENGTH_MS = 50;
const int FASTEST_GAP_MS = 100;   // Gap at MIN_CM (5 cm)
const int SLOWEST_GAP_MS = 1000;  // Gap at MAX_CM (30 cm)
const unsigned long PING_EVERY_MS = 60; // Spacing between sensor readings

// State Variables
unsigned long lastPing = 0;
unsigned long lastBeep = 0;
float distanceCm = -1;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  // 1. Take a new distance reading on a non-blocking schedule
  if (now - lastPing >= PING_EVERY_MS) {
    lastPing = now;
    distanceCm = readDistanceCm();
    
    // Optional: Print distance to Serial Monitor for debugging
    if (distanceCm > 0) {
      Serial.print("Distance: ");
      Serial.print(distanceCm);
      Serial.println(" cm");
    }
  }

  // 2. Control the buzzer based on the distance reading (5 cm to 30 cm)
  if (distanceCm >= MIN_CM && distanceCm <= MAX_CM) {
    // Map distance to beep gap (closer distance = smaller gap = faster beeps)
    long gap = map((long)distanceCm, (long)MIN_CM, (long)MAX_CM, FASTEST_GAP_MS, SLOWEST_GAP_MS);
    gap = constrain(gap, FASTEST_GAP_MS, SLOWEST_GAP_MS);

    // Non-blocking beep generator
    if (now - lastBeep >= (unsigned long)gap) {
      lastBeep = now;
      tone(BUZZER_PIN, BEEP_FREQ_HZ, BEEP_LENGTH_MS);
    }
  } else {
    // Out of range or nothing detected, stay quiet
    noTone(BUZZER_PIN);
  }
}

// Custom function to measure distance safely (placed outside setup/loop)
float readDistanceCm() {
  // Send a 10 microsecond trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse duration (timeout after 25ms)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  
  if (duration == 0) {
    return -1; // No echo received / out of bounds
  }

  // Sound travels at 0.0343 cm per microsecond (there and back, so divide by 2)
  float cm = duration * 0.0343 / 2.0;
  return cm;
}