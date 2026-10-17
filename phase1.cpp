
/* Drishtec - Phase 1 on Arduino Uno
 * One HC-SR04 + buzzer. Closer obstacle = faster beeps.
 *
 * Wiring: TRIG -> pin 9, ECHO -> pin 10, Buzzer (+) -> pin 8,
 *         sensor VCC -> 5V, sensor GND and buzzer (-) -> GND.
 * No voltage divider needed on the Uno (it runs at 5 V).
 */

int trigPin = 9;
int echoPin = 10;
int buzzerPin = 8;

float getDistance(void) {
  long duration;
  float distance;

  /* send a 10 microsecond pulse */
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  /* measure how long the echo takes (give up after 25 ms) */
  duration = pulseIn(echoPin, HIGH, 2500);

  /* no echo received */
  if (duration == 0) {
    return -1;
  }

  /* sound travels 0.0343 cm per microsecond, there and back */
  distance = duration * 0.0343 / 2;
  return distance;
}

void setup(void) {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop(void) {
  float distance;
  float gap;

  distance = getDistance();
  Serial.println(distance);

  /* only react between 20 cm and 300 cm */
  if (distance >= 5 && distance <= 30) {
    /* 20 cm -> 100 ms gap, 300 cm -> 1000 ms gap */
    gap = 100 + (distance - 20) * 900 / 280;

    tone(buzzerPin, 1000);   /* beep on */
    delay(50);
    noTone(buzzerPin);       /* beep off */
    delay((unsigned long)gap);
  } else {
    delay(100);              /* nothing in range, stay quiet */
  }
}
