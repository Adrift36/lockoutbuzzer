# Lockout Buzzer MVP (Arduino)

This project uses a **local Arduino flashing workflow** (no GitHub Actions required).

## Current MVP

- One firmware sketch for all boards: `arduino/lockout_mvp/lockout_mvp.ino`
- Role by jumper pin:
  - `HOST_SELECT` low (to GND) => Host
  - high => Player
- Transport: BLE advertising packets (2.4 GHz)
- Host locks on first valid player press it receives

## Primary workflow: Arduino IDE (recommended)

1. Install Arduino IDE 2.x
2. Add board index URL in Preferences:
   - `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json`
3. Install **Adafruit nRF52 Boards** in Boards Manager
4. Open `arduino/lockout_mvp/lockout_mvp.ino`
5. Select board target (start with **Adafruit Feather nRF52840 Express**)
6. Select COM port
7. Click **Upload**

If upload fails, double-tap reset to enter bootloader, then upload again.

## Optional workflow: PlatformIO

Yes — you can use PlatformIO too.

- Config file: `platformio.ini`
- Default env: `feather_nrf52840`
- Source folder: `arduino/lockout_mvp`

Typical PlatformIO commands:
- Build: `pio run`
- Upload: `pio run -t upload`
- Serial monitor: `pio device monitor -b 115200`

## Pin defaults (change as needed)

- `PIN_LED = LED_BUILTIN`
- `PIN_BUTTON = 1` (board header D1, active-low to GND)
- `PIN_HOST_SELECT = 21` (board header D21, jumper to GND = host)

## Important MVP limitations

- This is intentionally minimal.
- Players currently transmit buzz packets to host.
- Host decides winner and indicates on host LED/serial.
- Full winner broadcast to all players is next step.
