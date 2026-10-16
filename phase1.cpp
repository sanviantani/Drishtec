// Drishtec - Phase 1 on ESP32: one forward HC-SR04 + buzzer
// Closer obstacle = faster beeps. Beyond range = silence.
//
// Wiring (GPIO numbers):
//   HC-SR04 VCC  -> 5V (VIN)        HC-SR04 GND -> GND
//   HC-SR04 TRIG -> GPIO 5
//   HC-SR04 ECHO -> GPIO 18  (REAL HARDWARE: through a voltage divider,
//                             e.g. 1k in series + 2k to GND, 3.3V max)
//   Buzzer (+)   -> GPIO 19         Buzzer (-)  -> GND
//
// Needs arduino-esp32 core 3.x (tone() support). Wokwi uses it by default.

const int TRIG_PIN   = 5;
const int ECHO_PIN   = 18;
const int BUZZER_PIN = 19;

const float MIN_CM = 20.0;     // closest distance we care about
const float MAX_CM = 300.0;    // forward range (3.0 m)

const int BEEP_FREQ_HZ   = 1000;
const int BEEP_LENGTH_MS = 50;
const int FASTEST_GAP_MS = 100;   // gap between beeps at MIN_CM
const int SLOWEST_GAP_MS = 1000;  // gap between beeps at MAX_CM

const unsigned long PING_EVERY_MS = 60;  // spacing so echoes don't overlap

unsigned long lastPing = 0;
unsigned long lastBeep = 0;
float distanceCm = -1;   // -1 means nothing detected in range

float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 25 ms timeout (about 4.3 m); returns 0 if no echo
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return -1;

  float cm = duration * 0.0343 / 2.0;
  if (cm < MIN_CM || cm > MAX_CM) return -1;
  return cm;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  // 1. Take a new distance reading on schedule
  if (now - lastPing >= PING_EVERY_MS) {
    lastPing = now;
    distanceCm = readDistanceCm();
    Serial.println(distanceCm);
  }

  // 2. Beep faster as the obstacle gets closer (no blocking delay)
  if (distanceCm > 0) {
    long gap = map((long)distanceCm, (long)MIN_CM, (long)MAX_CM,
                   FASTEST_GAP_MS, SLOWEST_GAP_MS);
    gap = constrain(gap, FASTEST_GAP_MS, SLOWEST_GAP_MS);

    if (now - lastBeep >= (unsigned long)gap) {
      lastBeep = now;
      tone(BUZZER_PIN, BEEP_FREQ_HZ, BEEP_LENGTH_MS);
    }
  }
}
