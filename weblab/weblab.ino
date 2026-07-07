#include <WiFi.h>
#include <WebServer.h>
#include "web.h"

#define LED_PIN 8  // ESP32-C3 Super Mini blue LED is usually GPIO8

WebServer server(80);

bool ledOn = false;
unsigned long bootTime;

String ledStateText() {
  return ledOn ? "ON" : "OFF";
}

void setLed(bool on) {
  ledOn = on;

  // Most ESP32-C3 Super Mini blue LEDs are active-low:
  // LOW = on, HIGH = off
  digitalWrite(LED_PIN, ledOn ? LOW : HIGH);
}

String jsonEscape(String s) {
  s.replace("\\", "\\\\");
  s.replace("\"", "\\\"");
  s.replace("\n", "\\n");
  return s;
}

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleStatus() {
  unsigned long uptime = (millis() - bootTime) / 1000;

  String json = "{";
  json += "\"board\":\"ESP32-C3\",";
  json += "\"mode\":\"AP\",";
  json += "\"ssid\":\"ESP32-C3-WebLab\",";
  json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"uptime\":" + String(uptime) + ",";
  json += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"chip_model\":\"" + String(ESP.getChipModel()) + "\",";
  json += "\"chip_revision\":" + String(ESP.getChipRevision()) + ",";
  json += "\"cpu_mhz\":" + String(ESP.getCpuFreqMHz()) + ",";
  json += "\"flash_size\":" + String(ESP.getFlashChipSize()) + ",";
  json += "\"led\":\"" + ledStateText() + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleLedOn() {
  setLed(true);
  server.send(200, "application/json", "{\"led\":\"ON\"}");
}

void handleLedOff() {
  setLed(false);
  server.send(200, "application/json", "{\"led\":\"OFF\"}");
}

void handleLedToggle() {
  setLed(!ledOn);
  server.send(200, "application/json", "{\"led\":\"" + ledStateText() + "\"}");
}

void handleScan() {
  int n = WiFi.scanNetworks();

  String json = "[";
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";

    json += "{";
    json += "\"ssid\":\"" + jsonEscape(WiFi.SSID(i)) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"channel\":" + String(WiFi.channel(i)) + ",";
    json += "\"secure\":" + String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "false" : "true");
    json += "}";
  }
  json += "]";

  WiFi.scanDelete();
  server.send(200, "application/json", json);
}

void handleBlink() {
  int times = server.hasArg("times") ? server.arg("times").toInt() : 3;
  if (times < 1) times = 1;
  if (times > 20) times = 20;

  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(120);
    digitalWrite(LED_PIN, HIGH);
    delay(120);
  }

  setLed(ledOn);
  server.send(200, "application/json", "{\"blink\":\"done\"}");
}

void handleReboot() {
  server.send(200, "text/plain", "Rebooting ESP32-C3...");
  delay(500);
  ESP.restart();
}

void handleNotFound() {
  server.send(404, "text/plain", "404 Not Found");
}

void setup() {
  bootTime = millis();

  pinMode(LED_PIN, OUTPUT);
  setLed(false);

  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32-C3-WebLab", "12345678");

  server.on("/", handleRoot);
  server.on("/api/status", handleStatus);
  server.on("/api/led/on", handleLedOn);
  server.on("/api/led/off", handleLedOff);
  server.on("/api/led/toggle", handleLedToggle);
  server.on("/api/scan", handleScan);
  server.on("/api/blink", handleBlink);
  server.on("/api/reboot", handleReboot);
  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println();
  Serial.println("ESP32-C3 Web Lab started");
  Serial.println("WiFi: ESP32-C3-WebLab");
  Serial.println("Password: 12345678");
  Serial.print("Open: http://");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
}
