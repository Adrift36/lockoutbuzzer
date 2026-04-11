# Implementation steps (small and testable)

## Step 0 — Repo split ✅
- Lockout firmware moved to dedicated repo.

## Step 1 — Arduino local MVP ✅ (current)

Deliverables:
- Arduino sketch with same-firmware role select by jumper
- Host receives player buzz packets (BLE advertising)
- Local flashing via Arduino IDE
- Optional local build/upload via PlatformIO

Verification:
- Flash two boards with same sketch:
  - one host (jumper low)
  - one player (jumper high)
- Player button press prints lockout winner on host serial
- Repeat over multiple rounds

Exit criteria:
- Repeatable lockout detection on bench

## Step 2 — Winner broadcast to players
- Host publishes winner packet
- Players show local LED state for winner/non-winner

## Step 3 — Robustness
- Duplicate suppression
- Retry/backoff tuning
- Multi-player contention tests

## Step 4 — Scale target
- Test toward 20 players
- Measure miss/collision rate

## Step 5 — Power
- Sleep and battery behavior
