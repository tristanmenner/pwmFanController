#include <Arduino.h>

// ----- Pin assignment (Arduino Nano ATmega328P) -----
constexpr uint8_t FAN_PWM_PIN = 9;        // PWM output to fan driver (use a transistor/MOSFET)
constexpr uint8_t SPEED_SWITCH_PIN = 2;   // LOW = speed control enabled, HIGH = fan off
constexpr uint8_t SPEED_POT_PIN = A0;     // 10k log pot (5V)
constexpr uint8_t DUTY_SWITCH_PIN = 3;    // LOW = duty cycle mode enabled, HIGH = always on (100%)
constexpr uint8_t DUTY_POT_PIN = A1;      // 10k log pot (5V)

// Duty cycle timing bounds (ON time and OFF time are equal).
constexpr unsigned long DUTY_MIN_MS = 5000UL;   // 5 seconds
constexpr unsigned long DUTY_MAX_MS = 60000UL;  // 60 seconds

bool phaseIsOn = true;
unsigned long phaseStartMs = 0;

int readSpeedPwm()
{
  // 0..1023 -> 0..255
  const int raw = analogRead(SPEED_POT_PIN);
  return map(raw, 0, 1023, 0, 255);
}

unsigned long readDutyHalfPeriodMs()
{
  // Pot low -> 5s on / 5s off, pot high -> 60s on / 60s off.
  const int raw = analogRead(DUTY_POT_PIN);
  return map(raw, 0, 1023, DUTY_MIN_MS, DUTY_MAX_MS);
}

void setup()
{
  pinMode(FAN_PWM_PIN, OUTPUT);
  pinMode(SPEED_SWITCH_PIN, INPUT_PULLUP);
  pinMode(DUTY_SWITCH_PIN, INPUT_PULLUP);

  analogWrite(FAN_PWM_PIN, 0);
  phaseStartMs = millis();
}

void loop()
{
  const bool speedEnabled = (digitalRead(SPEED_SWITCH_PIN) == LOW);
  const bool dutyModeEnabled = (digitalRead(DUTY_SWITCH_PIN) == LOW);

  // Switch off for speed control means hard off regardless of other inputs.
  if (!speedEnabled)
  {
    analogWrite(FAN_PWM_PIN, 0);
    phaseIsOn = true;
    phaseStartMs = millis();
    delay(10);
    return;
  }

  const int pwmValue = readSpeedPwm();

  // Switch off for duty control means 100% on (no duty cycling).
  if (!dutyModeEnabled)
  {
    analogWrite(FAN_PWM_PIN, pwmValue);
    phaseIsOn = true;
    phaseStartMs = millis();
    delay(10);
    return;
  }

  // Duty mode enabled: equal ON/OFF phases from 5s..60s.
  const unsigned long halfPeriodMs = readDutyHalfPeriodMs();
  const unsigned long nowMs = millis();

  if (nowMs - phaseStartMs >= halfPeriodMs)
  {
    phaseIsOn = !phaseIsOn;
    phaseStartMs = nowMs;
  }

  analogWrite(FAN_PWM_PIN, phaseIsOn ? pwmValue : 0);
  delay(10);
}
