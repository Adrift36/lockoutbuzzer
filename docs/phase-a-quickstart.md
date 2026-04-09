# Phase A quickstart

This repo starts with a single goal: prove the firmware builds in GitHub Actions.

## Current build target

- Board: `nrf52840dk_nrf52840`
- App: `app/`
- Workflow: `.github/workflows/phase-a-build.yml`

## What to verify

1. Push to GitHub.
2. Open **Actions** and run **Build Phase A firmware**.
3. Confirm the artifact uploads successfully.
4. Flash the result to one board and check:
   - HOST jumper low => HOST blink pattern
   - jumper high => PLAYER blink pattern
   - button press logs and blinks three times

## Notes

- This keeps the first step simple and reproducible.
- After this passes, we can switch from build-only to radio bring-up.
