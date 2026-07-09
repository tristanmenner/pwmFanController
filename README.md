# pwmFanController
Repo for Box Fan project.

PlatformIO project for Arduino Nano with:
- 1x PWM fan output
- 1x ON/OFF switch in code (duty mode)
- 1x hardware fan power switch (external, not handled in code)
- 2x 10k log potentiometers

## Behavior

- **Hardware fan power switch OFF**: fan power is cut at 12V (outside code control).
- **Hardware fan power switch ON**: speed pot sets PWM speed.
- **Duty switch OFF**: fan runs continuously (100% on, no duty cycling).
- **Duty switch ON**: duty pot sets equal on/off timing:
  - lowest: `60s ON / 60s OFF`
  - highest: `5s ON / 5s OFF`

## Wiring (as coded)

- `D9` -> fan driver PWM input (through proper transistor/MOSFET stage)
- `A0` -> speed pot wiper (pot ends to `5V` and `GND`)
- `D3` -> duty switch to GND (uses `INPUT_PULLUP`)
- `A1` -> duty pot wiper (pot ends to `5V` and `GND`)
- fan VCC ON/OFF switch -> wired directly in the 12V fan supply path

Duty switch is active-low in this code (`ON` = connected to GND). If your switch orientation is opposite, swap the logic check in `src/main.cpp`.

## Build / Upload (PlatformIO)

```powershell
pio run
pio run -t upload
```
