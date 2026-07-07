#include <WiFi.h>
#include <WebServer.h>
#include "web.h"

#define LED_PIN 8

WebServer server(80);
bool ledOn = false;

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleToggle() {
  ledOn = !ledOn;
  digitalWrite(LED_PIN, ledOn ? LOW : HIGH);
  server.send(200, "text/plain", ledOn ? "ON" : "OFF");
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("North Norea Wi-Fi Surveillance System", "12345678");

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();
}

void loop() {
  server.handleClient();
}
