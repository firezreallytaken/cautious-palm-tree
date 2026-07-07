#include <WiFi.h>
#include <WebServer.h>

#define LED_PIN 8   // ESP32-C3 Super Mini often uses GPIO8 for onboard blue LED

WebServer server(80);

bool ledOn = false;

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32-C3 Server</title>
  <style>
    body { font-family: sans-serif; text-align: center; padding: 30px; }
    button { font-size: 24px; padding: 15px 25px; }
  </style>
</head>
<body>
  <h1>ESP32-C3 Works</h1>
  <p>This page is served from the ESP32-C3.</p>
  <p><a href="/toggle"><button>Toggle LED</button></a></p>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleToggle() {
  ledOn = !ledOn;

  // Some ESP32-C3 boards have active-low LEDs.
  digitalWrite(LED_PIN, ledOn ? LOW : HIGH);

  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Starting ESP32-C3 AP server...");

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32-C3-Test", "12345678");

  Serial.print("Connect to WiFi: ");
  Serial.println("ESP32-C3-Test");
  Serial.print("Password: ");
  Serial.println("12345678");
  Serial.print("Open: http://");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();

  Serial.println("Server started.");
}

void loop() {
  server.handleClient();
}
