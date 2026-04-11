#include <bluefruit.h>

// ---------- Pin config (adjust for your board wiring) ----------
const uint8_t PIN_LED         = LED_BUILTIN;
const uint8_t PIN_BUTTON      = PIN_A0;    // Active-low button to GND
const uint8_t PIN_HOST_SELECT = PIN_A1;    // Jumper to GND => HOST

// ---------- Protocol ----------
// 6-byte manufacturer payload:
// [0] magic0 0xBA
// [1] magic1 0x7A
// [2] player_id
// [3] seq
// [4] flags bit0=pressed
// [5] crc (xor of first 5 bytes)

const uint8_t PKT_MAGIC0 = 0xBA;
const uint8_t PKT_MAGIC1 = 0x7A;
const uint8_t FLAG_PRESSED = 0x01;

struct __attribute__((packed)) AdvPacket {
  uint8_t magic0;
  uint8_t magic1;
  uint8_t playerId;
  uint8_t seq;
  uint8_t flags;
  uint8_t crc;
};

volatile bool buttonEdge = false;
bool isHost = false;
bool roundLocked = false;
uint8_t winnerId = 0;
uint8_t playerId = 0;
uint8_t seqNo = 0;
uint32_t hostLockMillis = 0;

uint8_t crc8_xor(const uint8_t* p, size_t n) {
  uint8_t c = 0;
  for (size_t i = 0; i < n; i++) c ^= p[i];
  return c;
}

void buttonISR() {
  buttonEdge = true;
}

uint8_t derivePlayerId() {
  // Use part of BLE MAC as a simple unique-ish ID for MVP.
  ble_gap_addr_t addr;
  sd_ble_gap_addr_get(&addr);
  return addr.addr[0] ^ addr.addr[3] ^ addr.addr[5];
}

void ledBlink(uint16_t onMs, uint16_t offMs, uint8_t count = 1) {
  for (uint8_t i = 0; i < count; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(onMs);
    digitalWrite(PIN_LED, LOW);
    delay(offMs);
  }
}

void advertisePressBurst() {
  AdvPacket pkt;
  pkt.magic0 = PKT_MAGIC0;
  pkt.magic1 = PKT_MAGIC1;
  pkt.playerId = playerId;
  pkt.seq = ++seqNo;
  pkt.flags = FLAG_PRESSED;
  pkt.crc = crc8_xor((uint8_t*)&pkt, 5);

  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addManufacturerData((uint8_t*)&pkt, sizeof(pkt));
  Bluefruit.Advertising.setInterval(32, 48); // ~20-30ms

  // Short burst to improve reliability in collisions
  Bluefruit.Advertising.start(1); // 1 second
  delay(120);
  Bluefruit.Advertising.stop();
}

void scanCallback(ble_gap_evt_adv_report_t* report) {
  uint8_t buf[31];
  int len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, buf, sizeof(buf));
  if (len < (int)sizeof(AdvPacket)) return;

  AdvPacket pkt;
  memcpy(&pkt, buf, sizeof(pkt));

  if (pkt.magic0 != PKT_MAGIC0 || pkt.magic1 != PKT_MAGIC1) return;
  if (pkt.crc != crc8_xor((uint8_t*)&pkt, 5)) return;
  if (!(pkt.flags & FLAG_PRESSED)) return;

  if (!roundLocked) {
    roundLocked = true;
    winnerId = pkt.playerId;
    hostLockMillis = millis();

    Serial.print("LOCKOUT winner: ");
    Serial.println(winnerId);

    // Distinct winner indication on host LED (3 long blinks)
    ledBlink(200, 120, 3);
  }
}

void setupHost() {
  Bluefruit.Scanner.setRxCallback(scanCallback);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.restartOnDisconnect(false);
  Bluefruit.Scanner.start(0); // continuous

  Serial.println("Role=HOST");
  ledBlink(120, 120, 2);
}

void setupPlayer() {
  Serial.print("Role=PLAYER id=");
  Serial.println(playerId);
  ledBlink(60, 80, 3);
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_HOST_SELECT, INPUT_PULLUP);

  Serial.begin(115200);
  delay(200);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setName("lockout-mvp");

  playerId = derivePlayerId();
  isHost = (digitalRead(PIN_HOST_SELECT) == LOW); // jumper to GND => host

  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonISR, FALLING);

  if (isHost) setupHost();
  else setupPlayer();
}

void loop() {
  static uint32_t lastRoleBlink = 0;

  if (buttonEdge) {
    buttonEdge = false;

    // Basic debounce
    delay(8);
    if (digitalRead(PIN_BUTTON) == LOW) {
      if (!isHost && !roundLocked) {
        Serial.println("PLAYER press -> adv burst");
        advertisePressBurst();
        ledBlink(40, 40, 2);
      }

      // Host local reset shortcut for MVP
      if (isHost) {
        roundLocked = false;
        winnerId = 0;
        Serial.println("HOST reset round");
        ledBlink(40, 40, 4);
      }

      while (digitalRead(PIN_BUTTON) == LOW) delay(1);
    }
  }

  // Role heartbeat LED patterns
  if (millis() - lastRoleBlink > 1000) {
    lastRoleBlink = millis();

    if (isHost) {
      if (!roundLocked) {
        ledBlink(80, 0, 1); // short heartbeat
      } else {
        digitalWrite(PIN_LED, HIGH); // locked => solid for short window
        delay(60);
        digitalWrite(PIN_LED, LOW);
      }
    } else {
      ledBlink(25, 0, 1); // tiny player heartbeat
    }
  }

  // Auto-unlock timeout for MVP demo rounds
  if (isHost && roundLocked && (millis() - hostLockMillis > 8000)) {
    roundLocked = false;
    winnerId = 0;
    Serial.println("HOST auto reset round");
  }
}
