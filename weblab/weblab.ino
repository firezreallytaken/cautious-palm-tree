#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Preferences.h>
#include "web.h"

#define LED_PIN 8

WebServer server(80);
Preferences prefs;

bool ledOn = false;
unsigned long bootTime = 0;

String apSsid = "ESP32-C3-WebLab";
String apPass = "12345678";

File uploadFile;
String lastUploadName = "";

String jsonEscape(String s) {
  s.replace("\\", "\\\\");
  s.replace("\"", "\\\"");
  s.replace("\n", "\\n");
  s.replace("\r", "");
  return s;
}

String urlEncode(String s) {
  String out = "";
  const char *hex = "0123456789ABCDEF";

  for (int i = 0; i < s.length(); i++) {
    char c = s[i];

    if (
      (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9') ||
      c == '-' || c == '_' || c == '.'
    ) {
      out += c;
    } else {
      out += '%';
      out += hex[(c >> 4) & 0xF];
      out += hex[c & 0xF];
    }
  }

  return out;
}

String safeName(String name) {
  name.replace("\\", "/");

  int q = name.indexOf("?");
  if (q >= 0) name = name.substring(0, q);

  int slash = name.lastIndexOf("/");
  if (slash >= 0) name = name.substring(slash + 1);

  String out = "";
  for (int i = 0; i < name.length(); i++) {
    char c = name[i];

    bool ok =
      (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9') ||
      c == '.' || c == '-' || c == '_';

    if (ok) out += c;
  }

  if (out.length() == 0) out = "app.html";

  if (
    !out.endsWith(".html") &&
    !out.endsWith(".htm") &&
    !out.endsWith(".css") &&
    !out.endsWith(".js") &&
    !out.endsWith(".json") &&
    !out.endsWith(".txt")
  ) {
    out += ".html";
  }

  return out;
}

String appPath(String name) {
  return "/apps/" + safeName(name);
}

String appUrl(String name) {
  return "/app?name=" + urlEncode(safeName(name));
}

String contentType(String path) {
  if (path.endsWith(".html") || path.endsWith(".htm")) return "text/html";
  if (path.endsWith(".css")) return "text/css";
  if (path.endsWith(".js")) return "application/javascript";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".txt")) return "text/plain";
  if (path.endsWith(".svg")) return "image/svg+xml";
  return "application/octet-stream";
}

void setLed(bool on) {
  ledOn = on;

  // ESP32-C3 Super Mini onboard blue LED is usually active-low.
  digitalWrite(LED_PIN, ledOn ? LOW : HIGH);
}

void rawLed(bool on) {
  digitalWrite(LED_PIN, on ? LOW : HIGH);
}

String ledText() {
  return ledOn ? "ON" : "OFF";
}

void loadSettings() {
  prefs.begin("weblab", false);

  apSsid = prefs.getString("ssid", "ESP32-C3-WebLab");
  apPass = prefs.getString("pass", "12345678");

  if (apSsid.length() < 1 || apSsid.length() > 32) {
    apSsid = "ESP32-C3-WebLab";
  }

  if (apPass.length() > 0 && apPass.length() < 8) {
    apPass = "12345678";
  }
}

void writePreset(String name, const char *content) {
  String path = appPath(name);

  if (LittleFS.exists(path)) return;

  File f = LittleFS.open(path, "w");
  if (!f) return;

  f.print(content);
  f.close();
}

void seedPresetApps() {
  if (!LittleFS.exists("/apps")) {
    LittleFS.mkdir("/apps");
  }

  writePreset("morse.html", PRESET_MORSE_HTML);
  writePreset("clock.html", PRESET_CLOCK_HTML);
  writePreset("terminal.html", PRESET_TERMINAL_HTML);
}

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleStatus() {
  unsigned long uptime = (millis() - bootTime) / 1000;

  String json = "{";
  json += "\"board\":\"ESP32-C3\",";
  json += "\"ssid\":\"" + jsonEscape(apSsid) + "\",";
  json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"uptime\":" + String(uptime) + ",";
  json += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"chip_model\":\"" + String(ESP.getChipModel()) + "\",";
  json += "\"chip_revision\":" + String(ESP.getChipRevision()) + ",";
  json += "\"cpu_mhz\":" + String(ESP.getCpuFreqMHz()) + ",";
  json += "\"flash_size\":" + String(ESP.getFlashChipSize()) + ",";
  json += "\"fs_total\":" + String(LittleFS.totalBytes()) + ",";
  json += "\"fs_used\":" + String(LittleFS.usedBytes()) + ",";
  json += "\"led\":\"" + ledText() + "\"";
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
  server.send(200, "application/json", "{\"led\":\"" + ledText() + "\"}");
}

void handleBlink() {
  int times = server.hasArg("times") ? server.arg("times").toInt() : 3;
  if (times < 1) times = 1;
  if (times > 30) times = 30;

  for (int i = 0; i < times; i++) {
    rawLed(true);
    delay(100);
    rawLed(false);
    delay(100);
  }

  setLed(ledOn);
  server.send(200, "application/json", "{\"blink\":\"done\"}");
}

String morseChar(char c) {
  c = toupper(c);

  switch (c) {
    case 'A': return ".-";
    case 'B': return "-...";
    case 'C': return "-.-.";
    case 'D': return "-..";
    case 'E': return ".";
    case 'F': return "..-.";
    case 'G': return "--.";
    case 'H': return "....";
    case 'I': return "..";
    case 'J': return ".---";
    case 'K': return "-.-";
    case 'L': return ".-..";
    case 'M': return "--";
    case 'N': return "-.";
    case 'O': return "---";
    case 'P': return ".--.";
    case 'Q': return "--.-";
    case 'R': return ".-.";
    case 'S': return "...";
    case 'T': return "-";
    case 'U': return "..-";
    case 'V': return "...-";
    case 'W': return ".--";
    case 'X': return "-..-";
    case 'Y': return "-.--";
    case 'Z': return "--..";
    case '0': return "-----";
    case '1': return ".----";
    case '2': return "..---";
    case '3': return "...--";
    case '4': return "....-";
    case '5': return ".....";
    case '6': return "-....";
    case '7': return "--...";
    case '8': return "---..";
    case '9': return "----.";
    default: return "";
  }
}

String encodeMorse(String text) {
  String out = "";

  for (int i = 0; i < text.length(); i++) {
    char c = text[i];

    if (c == ' ') {
      out += "/ ";
      continue;
    }

    String m = morseChar(c);
    if (m.length() > 0) {
      out += m;
      out += " ";
    }
  }

  out.trim();
  return out;
}

void playMorse(String text, int unitMs) {
  if (unitMs < 40) unitMs = 40;
  if (unitMs > 1000) unitMs = 1000;

  if (text.length() > 80) {
    text = text.substring(0, 80);
  }

  for (int i = 0; i < text.length(); i++) {
    char c = text[i];

    if (c == ' ') {
      rawLed(false);
      delay(unitMs * 7);
      continue;
    }

    String code = morseChar(c);
    if (code.length() == 0) continue;

    for (int j = 0; j < code.length(); j++) {
      rawLed(true);

      if (code[j] == '.') {
        delay(unitMs);
      } else {
        delay(unitMs * 3);
      }

      rawLed(false);
      delay(unitMs);
    }

    delay(unitMs * 2);
  }

  setLed(ledOn);
}

void handleMorseEncode() {
  String text = server.hasArg("text") ? server.arg("text") : "SOS";
  String code = encodeMorse(text);

  String json = "{";
  json += "\"text\":\"" + jsonEscape(text) + "\",";
  json += "\"morse\":\"" + jsonEscape(code) + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleMorsePlay() {
  String text = server.hasArg("text") ? server.arg("text") : "SOS";
  int unitMs = server.hasArg("unit") ? server.arg("unit").toInt() : 120;

  String code = encodeMorse(text);

  playMorse(text, unitMs);

  String json = "{";
  json += "\"played\":true,";
  json += "\"text\":\"" + jsonEscape(text) + "\",";
  json += "\"morse\":\"" + jsonEscape(code) + "\",";
  json += "\"unit\":" + String(unitMs);
  json += "}";

  server.send(200, "application/json", json);
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

void handleApps() {
  File root = LittleFS.open("/apps");

  String json = "[";
  bool first = true;

  if (root && root.isDirectory()) {
    File file = root.openNextFile();

    while (file) {
      if (!file.isDirectory()) {
        String raw = String(file.name());
        String name = raw;

        int slash = name.lastIndexOf("/");
        if (slash >= 0) name = name.substring(slash + 1);

        if (name.length() > 0) {
          if (!first) json += ",";
          first = false;

          json += "{";
          json += "\"name\":\"" + jsonEscape(name) + "\",";
          json += "\"path\":\"/apps/" + jsonEscape(name) + "\",";
          json += "\"url\":\"" + jsonEscape(appUrl(name)) + "\",";
          json += "\"size\":" + String(file.size());
          json += "}";
        }
      }

      file = root.openNextFile();
    }
  }

  json += "]";
  server.send(200, "application/json", json);
}

void handleSave() {
  if (!server.hasArg("name") || !server.hasArg("content")) {
    server.send(400, "application/json", "{\"error\":\"missing name or content\"}");
    return;
  }

  String name = safeName(server.arg("name"));
  String path = appPath(name);

  File f = LittleFS.open(path, "w");
  if (!f) {
    server.send(500, "application/json", "{\"error\":\"cannot write file\"}");
    return;
  }

  f.print(server.arg("content"));
  f.close();

  String json = "{";
  json += "\"saved\":true,";
  json += "\"name\":\"" + jsonEscape(name) + "\",";
  json += "\"url\":\"" + jsonEscape(appUrl(name)) + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleDelete() {
  if (!server.hasArg("name")) {
    server.send(400, "application/json", "{\"error\":\"missing name\"}");
    return;
  }

  String name = safeName(server.arg("name"));
  String path = appPath(name);

  if (!LittleFS.exists(path)) {
    server.send(404, "application/json", "{\"error\":\"file not found\"}");
    return;
  }

  LittleFS.remove(path);
  server.send(200, "application/json", "{\"deleted\":true}");
}

void handleUploadFinish() {
  if (lastUploadName.length() == 0) {
    server.send(400, "application/json", "{\"error\":\"no file uploaded\"}");
    return;
  }

  String json = "{";
  json += "\"uploaded\":true,";
  json += "\"name\":\"" + jsonEscape(lastUploadName) + "\",";
  json += "\"url\":\"" + jsonEscape(appUrl(lastUploadName)) + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleUploadData() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    lastUploadName = safeName(upload.filename);
    String path = appPath(lastUploadName);

    if (LittleFS.exists(path)) {
      LittleFS.remove(path);
    }

    uploadFile = LittleFS.open(path, "w");
  }

  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
  }

  else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
  }
}

void handleAppOpen() {
  if (!server.hasArg("name")) {
    server.send(400, "text/plain", "Missing app name");
    return;
  }

  String name = safeName(server.arg("name"));
  String path = appPath(name);

  if (!LittleFS.exists(path)) {
    server.send(404, "text/plain", "Mini app not found: " + name);
    return;
  }

  File f = LittleFS.open(path, "r");
  server.streamFile(f, contentType(path));
  f.close();
}

void handleSettings() {
  if (!server.hasArg("ssid") || !server.hasArg("pass")) {
    server.send(400, "application/json", "{\"error\":\"missing ssid or pass\"}");
    return;
  }

  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  ssid.trim();
  pass.trim();

  if (ssid.length() < 1 || ssid.length() > 32) {
    server.send(400, "application/json", "{\"error\":\"ssid must be 1-32 characters\"}");
    return;
  }

  if (pass.length() > 0 && pass.length() < 8) {
    server.send(400, "application/json", "{\"error\":\"password must be empty or 8+ characters\"}");
    return;
  }

  if (pass.length() > 63) {
    server.send(400, "application/json", "{\"error\":\"password too long\"}");
    return;
  }

  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);

  server.send(200, "application/json", "{\"saved\":true,\"reboot_required\":true}");
}

void handleReboot() {
  server.send(200, "text/plain", "Rebooting...");
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

  loadSettings();

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS failed");
  }

  seedPresetApps();

  WiFi.mode(WIFI_AP);

  if (apPass.length() >= 8) {
    WiFi.softAP(apSsid.c_str(), apPass.c_str());
  } else {
    WiFi.softAP(apSsid.c_str());
  }

  server.on("/", HTTP_GET, handleRoot);

  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/scan", HTTP_GET, handleScan);

  server.on("/api/led/on", HTTP_GET, handleLedOn);
  server.on("/api/led/off", HTTP_GET, handleLedOff);
  server.on("/api/led/toggle", HTTP_GET, handleLedToggle);
  server.on("/api/blink", HTTP_GET, handleBlink);

  server.on("/api/morse/encode", HTTP_GET, handleMorseEncode);
  server.on("/api/morse/play", HTTP_GET, handleMorsePlay);

  server.on("/api/apps", HTTP_GET, handleApps);
  server.on("/api/save", HTTP_POST, handleSave);
  server.on("/api/delete", HTTP_POST, handleDelete);
  server.on("/api/upload", HTTP_POST, handleUploadFinish, handleUploadData);

  server.on("/app", HTTP_GET, handleAppOpen);

  server.on("/api/settings", HTTP_POST, handleSettings);
  server.on("/api/reboot", HTTP_GET, handleReboot);

  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println();
  Serial.println("ESP32-C3 Web Lab v3 started");
  Serial.print("WiFi: ");
  Serial.println(apSsid);
  Serial.print("Password: ");
  Serial.println(apPass);
  Serial.print("Open: http://");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
}
