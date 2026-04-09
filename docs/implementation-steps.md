# Implementation steps (small and testable)

## Step 0 — Repo and scope freeze ✅

Deliverables:
- Repository created
- This checklist committed
- Architecture assumptions written

Verification:
- `main` has README and this doc

---

## Step 1 — Phase A: board bring-up (no radio)

Deliverables:
- Single Zephyr app
- Same firmware image for all boards
- Role detect from `HOST_SELECT` jumper at boot
- LED pattern shows role
- Button press event detected and logged

Verification:
- Flash 2 boards with same image
- Board A jumper = HOST pattern
- Board B no jumper = PLAYER pattern
- Button press on each board logs and blinks burst

Exit criteria:
- 100% repeatable across reboots

---

## Step 2 — Phase B: one-way radio smoke test

Deliverables:
- Player sends packet on button press
- Host receives and logs sender ID

Verification:
- 2 boards, 20+ presses, zero missed on short range

Exit criteria:
- Basic wireless link stable

---

## Step 3 — Phase C: lockout core

Deliverables:
- Host round state machine
- First valid press wins
- Host broadcasts lockout winner
- Players show winner state on LED

Verification:
- 3+ boards, simultaneous presses, deterministic first-wins behavior

Exit criteria:
- Correct winner selection in repeated contention tests

---

## Step 4 — Robustness and scaling

Deliverables:
- Retry strategy
- Duplicate suppression (seq numbers)
- Up to 20 players tested

Verification:
- Stress tests with many rapid rounds

Exit criteria:
- No lockups, no stale round states

---

## Step 5 — Power and enclosure readiness

Deliverables:
- Sleep behavior
- Battery measurement hook (optional)
- Final pin map docs

Verification:
- Idle current spot-check and wake reliability

Exit criteria:
- Ready for hardware packaging

---

## Notes

- Keep each step in its own commit(s).
- Do not begin next step until verification passes.
