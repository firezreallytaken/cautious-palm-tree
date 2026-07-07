#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED_PIN 8  // ESP32-C3 Super Mini blue LED is often GPIO8

BLECharacteristic *statusCharacteristic;
bool ledOn = false;

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define COMMAND_UUID        "12345678-1234-1234-1234-1234567890ac"
#define STATUS_UUID         "12345678-1234-1234-1234-1234567890ad"

void updateLED() {
  // Most ESP32-C3 blue LEDs are active-low
  digitalWrite(LED_PIN, ledOn ? LOW : HIGH);

  if (statusCharacteristic) {
    statusCharacteristic->setValue(ledOn ? "LED ON" : "LED OFF");
    statusCharacteristic->notify();
  }
}

class CommandCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override {
    String cmd = characteristic->getValue().c_str();
    cmd.toUpperCase();

    if (cmd == "ON") {
      ledOn = true;
    } else if (cmd == "OFF") {
      ledOn = false;
    } else if (cmd == "TOGGLE") {
      ledOn = !ledOn;
    }

    updateLED();
  }
};

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting BLE test...");

  BLEDevice::init("ESP32-C3 BLE");

  BLEServer *server = BLEDevice::createServer();
  BLEService *service = server->createService(SERVICE_UUID);

  BLECharacteristic *commandCharacteristic = service->createCharacteristic(
    COMMAND_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );

  statusCharacteristic = service->createCharacteristic(
    STATUS_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  statusCharacteristic->addDescriptor(new BLE2902());
  commandCharacteristic->setCallbacks(new CommandCallback());

  statusCharacteristic->setValue("LED OFF");

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->start();

  Serial.println("BLE ready. Search for ESP32-C3 BLE.");
}

void loop() {
  delay(1000);
}
