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
String lastUploadPath = "";

String jsonEscape(String s) {
  s.replace("\\", "\\\\");
  s.replace("\"", "\\\"");
  s.replace("\n", "\\n");
  s.replace("\r", "");
  return s;
}

String safeName(String name) {
  name.replace("\\", "/");
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

String contentType(String path) {
  if (path.endsWith(".html") || path.endsWith(".htm")) return "text/html";
  if (path.endsWith(".css")) return "text/css";
  if (path.endsWith(".js")) return "application/javascript";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".txt")) return "text/plain";
  if (path.endsWith(".png")) return "image/png";
  if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
  if (path.endsWith(".svg")) return "image/svg+xml";
  return "application/octet-stream";
}

String appUrlFromPath(String path) {
  if (path.startsWith("/apps/")) {
    return "/app/" + path.substring(6);
  }
  return path;
}

void setLed(bool on) {
  ledOn = on;

  // ESP32-C3 Super Mini blue LED is usually active-low
  digitalWrite(LED_PIN, ledOn ? LOW : HIGH);
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
  if (times > 20) times = 20;

  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
  }

  setLed(ledOn);
  server.send(200, "application/json", "{\"blink\":\"done\"}");
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

void handleFiles() {
  File root = LittleFS.open("/apps");

  String json = "[";
  bool first = true;

  if (root && root.isDirectory()) {
    File file = root.openNextFile();

    while (file) {
      if (!file.isDirectory()) {
        String path = String(file.name());

        if (!first) json += ",";
        first = false;

        json += "{";
        json += "\"path\":\"" + jsonEscape(path) + "\",";
        json += "\"name\":\"" + jsonEscape(path.substring(6)) + "\",";
        json += "\"url\":\"" + jsonEscape(appUrlFromPath(path)) + "\",";
        json += "\"size\":" + String(file.size());
        json += "}";
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
  String path = "/apps/" + name;

  File f = LittleFS.open(path, "w");
  if (!f) {
    server.send(500, "application/json", "{\"error\":\"cannot write file\"}");
    return;
  }

  f.print(server.arg("content"));
  f.close();

  String json = "{";
  json += "\"saved\":true,";
  json += "\"path\":\"" + jsonEscape(path) + "\",";
  json += "\"url\":\"" + jsonEscape(appUrlFromPath(path)) + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleUploadFinish() {
  if (lastUploadPath.length() == 0) {
    server.send(400, "application/json", "{\"error\":\"no file uploaded\"}");
    return;
  }

  String json = "{";
  json += "\"uploaded\":true,";
  json += "\"path\":\"" + jsonEscape(lastUploadPath) + "\",";
  json += "\"url\":\"" + jsonEscape(appUrlFromPath(lastUploadPath)) + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleUploadData() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    String name = safeName(upload.filename);
    lastUploadPath = "/apps/" + name;

    if (LittleFS.exists(lastUploadPath)) {
      LittleFS.remove(lastUploadPath);
    }

    uploadFile = LittleFS.open(lastUploadPath, "w");
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

void handleDelete() {
  if (!server.hasArg("path")) {
    server.send(400, "application/json", "{\"error\":\"missing path\"}");
    return;
  }

  String path = server.arg("path");

  if (!path.startsWith("/apps/")) {
    server.send(403, "application/json", "{\"error\":\"only /apps files can be deleted\"}");
    return;
  }

  if (!LittleFS.exists(path)) {
    server.send(404, "application/json", "{\"error\":\"file not found\"}");
    return;
  }

  LittleFS.remove(path);
  server.send(200, "application/json", "{\"deleted\":true}");
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

void serveFsFile(String path) {
  if (!LittleFS.exists(path)) {
    server.send(404, "text/plain", "File not found");
    return;
  }

  File f = LittleFS.open(path, "r");
  server.streamFile(f, contentType(path));
  f.close();
}

void handleNotFound() {
  String uri = server.uri();

  if (uri.startsWith("/app/")) {
    String name = safeName(uri.substring(5));
    serveFsFile("/apps/" + name);
    return;
  }

  if (uri.startsWith("/fs/")) {
    String path = "/" + uri.substring(4);
    serveFsFile(path);
    return;
  }

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

  if (!LittleFS.exists("/apps")) {
    LittleFS.mkdir("/apps");
  }

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

  server.on("/api/files", HTTP_GET, handleFiles);
  server.on("/api/save", HTTP_POST, handleSave);
  server.on("/api/delete", HTTP_POST, handleDelete);
  server.on("/api/upload", HTTP_POST, handleUploadFinish, handleUploadData);

  server.on("/api/settings", HTTP_POST, handleSettings);
  server.on("/api/reboot", HTTP_GET, handleReboot);

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println();
  Serial.println("ESP32-C3 Web Lab v2 started");
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
