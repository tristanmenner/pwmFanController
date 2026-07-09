# pwmFanController
Repo for Box Fan project.

PlatformIO project for Arduino Nano with:
- 1x PWM fan output
- 2x ON/OFF switches
- 2x 10k log potentiometers

## Behavior

- **Speed switch OFF**: fan output is OFF.
- **Speed switch ON**: speed pot sets PWM speed.
- **Duty switch OFF**: fan runs continuously (100% on, no duty cycling).
- **Duty switch ON**: duty pot sets equal on/off timing:
  - lowest: `5s ON / 5s OFF`
  - highest: `60s ON / 60s OFF`

## Wiring (as coded)

- `D9` -> fan driver PWM input (through proper transistor/MOSFET stage)
- `D2` -> speed switch to GND (uses `INPUT_PULLUP`)
- `A0` -> speed pot wiper (pot ends to `5V` and `GND`)
- `D3` -> duty switch to GND (uses `INPUT_PULLUP`)
- `A1` -> duty pot wiper (pot ends to `5V` and `GND`)

Switches are active-low in this code (`ON` = connected to GND). If your switch orientation is opposite, swap the logic checks in `src/main.cpp`.

## Build / Upload (PlatformIO)

```powershell
pio run
pio run -t upload
```
