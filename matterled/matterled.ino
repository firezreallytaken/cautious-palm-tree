

#include <Arduino.h>
#include <Matter.h>

// Existing ESP32-C3 WebLab LED
#define LED_PIN 8

MatterOnOffLight light;

// Called when Apple Home changes the light state
bool setLight(bool on) {
  // Active-low LED
  digitalWrite(LED_PIN, on ? LOW : HIGH);

  Serial.println(on ? "LED ON" : "LED OFF");
  return true;
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED off

  // Create a Matter light accessory
  light.begin(false);
  light.onChange(setLight);

  // Start Matter over Wi-Fi
  Matter.begin();

  Serial.println("Matter started");
  Serial.print("Pairing code: ");
  Serial.println(Matter.getManualPairingCode());

  Serial.print("QR URL: ");
  Serial.println(Matter.getOnboardingQRCodeUrl());

  // Apply initial state
  light.updateAccessory();
}

void loop() {
  delay(100);
}
