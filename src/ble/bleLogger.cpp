#include "bleLogger.h"

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Nordic UART Service (NUS)
static const char* UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* UART_TX_UUID      = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"; // Notify
static const char* UART_RX_UUID      = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"; // Write

static BLECharacteristic* g_txChar = nullptr;

// ---- tiny single-message queue (simple + effective) ----
// If logs come faster than poll(), newest wins. Good enough for debug logs.
static char g_pending[256];
static volatile bool g_hasPending = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onDisconnect(BLEServer* s) override {
    // Keep advertising so phone can reconnect
    s->getAdvertising()->start();
  }
};

bool bleLoggerInit(const char* deviceName)
{
  if (!deviceName) return false;

  BLEDevice::init(deviceName);

  BLEServer* server = BLEDevice::createServer();
  if (!server) return false;
  server->setCallbacks(new ServerCallbacks());

  BLEService* service = server->createService(UART_SERVICE_UUID);
  if (!service) return false;

  // TX (notify)
  g_txChar = service->createCharacteristic(
    UART_TX_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  if (!g_txChar) return false;

  g_txChar->addDescriptor(new BLE2902());

  // RX (write) - many terminal apps expect it
  service->createCharacteristic(
    UART_RX_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );

  service->start();
  BLEDevice::getAdvertising()->start();

  // Queue a message; it will be sent once loop() starts polling and notifications are enabled
  bleLoggerWrite("BLE logger init done");

  return true;
}

// Queue-only: safe to call from timers/callbacks/anywhere.
void bleLoggerWrite(const char* msg)
{
  if (!msg) return;

  // Copy into pending buffer (atomic-ish pattern)
  // NOTE: not perfect under heavy contention, but totally fine for logging.
  size_t n = strlcpy(g_pending, msg, sizeof(g_pending));
  (void)n;
  g_hasPending = true;
}

// Must be called from loop() context.
void bleLoggerPoll(void)
{
  if (!g_txChar) return;
  if (!g_hasPending) return;

  // Take a snapshot and clear pending
  char msg[256];
  noInterrupts();
  strlcpy(msg, g_pending, sizeof(msg));
  g_hasPending = false;
  interrupts();

  // Send in 20-byte chunks (reliable)
  const char* p = msg;
  while (*p) {
    uint8_t chunk[20];
    int len = 0;
    while (len < 20 && p[len] != '\0') {
      chunk[len] = (uint8_t)p[len];
      len++;
    }
    g_txChar->setValue(chunk, len);
    g_txChar->notify();
    p += len;

    // tiny spacing helps some phones/apps
    delay(3);
  }

  // newline
  uint8_t nl = '\n';
  g_txChar->setValue(&nl, 1);
  g_txChar->notify();
}
