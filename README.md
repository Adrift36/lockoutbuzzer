# Lockout Buzzer MVP (Arduino)

This repo now uses an Arduino-first MVP to reduce setup friction.

## Current MVP

- One firmware sketch for all boards: `arduino/lockout_mvp/lockout_mvp.ino`
- Role by jumper pin:
  - `HOST_SELECT` low (to GND) => Host
  - high => Player
- Transport: BLE advertising packets (2.4 GHz)
- Host locks on first valid player press it receives

## Important MVP limitations

- This is intentionally minimal.
- Players currently transmit buzz packets to host.
- Host decides winner and indicates on host LED/serial.
- Full winner broadcast to all players is next step.

## Pin defaults (change as needed)

- `PIN_LED = LED_BUILTIN`
- `PIN_BUTTON = PIN_A0` (active-low to GND)
- `PIN_HOST_SELECT = PIN_A1` (jumper to GND = host)

## GitHub Actions

Workflow: `.github/workflows/arduino-build.yml`

It compiles the sketch with Adafruit nRF52 core (`feather52840` target) to validate build health.

## Next small step

After this build is green, add host->player winner broadcast in a second commit.
