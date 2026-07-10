#include <Arduino.h>

// ----- Pin assignment (Arduino Nano ATmega328P) -----
constexpr uint8_t FAN_PWM_PIN = 9;        // PWM output to fan driver (use a transistor/MOSFET)
constexpr uint8_t SPEED_POT_PIN = A0;     // 10k log pot (5V)
constexpr uint8_t DUTY_SWITCH_PIN = 3;    // LOW = duty cycle mode enabled, HIGH = always on (100%)
constexpr uint8_t DUTY_POT_PIN = A1;      // 10k log pot (5V)

// Duty cycle timing bounds (ON time and OFF time are equal).
constexpr unsigned long DUTY_MIN_MS = 5000UL;   // 5 seconds
constexpr unsigned long DUTY_MAX_MS = 60000UL;  // 60 seconds

bool phaseIsOn = true;
unsigned long phaseStartMs = 0;

void writeFanPwm(uint8_t speedPwm)
{
  // Hardware path is inverted: higher Arduino PWM value means lower fan drive.
  analogWrite(FAN_PWM_PIN, 255 - speedPwm);
}

void writeFanOff()
{
  // Inverted driver requires full-scale output for hard OFF.
  analogWrite(FAN_PWM_PIN, 0);
}

int readSpeedPwm()
{
  // Pot direction reversed: low end = full speed, high end = minimum speed.
  const int raw = analogRead(SPEED_POT_PIN);
  return map(raw, 0, 1023, 0, 255);
}

unsigned long readDutyHalfPeriodMs()
{
  // Pot direction reversed: low end = 60s on/off, high end = 5s on/off.
  const int raw = analogRead(DUTY_POT_PIN);
  return map(raw, 0, 1023, DUTY_MAX_MS, DUTY_MIN_MS);
}

void setup()
{
  pinMode(FAN_PWM_PIN, OUTPUT);
  pinMode(DUTY_SWITCH_PIN, INPUT_PULLUP);

  writeFanOff();
  phaseStartMs = millis();
}

void loop()
{
  const bool dutyModeEnabled = (digitalRead(DUTY_SWITCH_PIN) == LOW);
  const int pwmValue = readSpeedPwm();

  // Switch off for duty control means 100% on (no duty cycling).
  if (!dutyModeEnabled)
  {
    writeFanPwm(pwmValue);
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

  if (phaseIsOn)
  {
    writeFanPwm(pwmValue);
  }
  else
  {
    writeFanOff();
  }

  delay(10);
}
