// Bai 1 - REST: GET /api/sensor tra JSON
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "TEN_WIFI";   // DOI
const char* WIFI_PASS = "MAT_KHAU";   // DOI
const char* MSSV      = "20xxxxxx";   // DOI: ma SV cua ban (truong tu them)

WebServer server(80);

// Chua gan DHT11 thi dung so gia cho chay duoc ngay
float docNhiet() { return 27.0 + 2.0 * sin(millis() / 10000.0); }
float docAm()    { return 55.0 + 5.0 * sin(millis() / 15000.0); }

void handleSensor() {
  JsonDocument doc;
  doc["id"]     = MSSV;                 // truong tu them
  doc["temp"]   = docNhiet();
  doc["hum"]    = docAm();
  doc["rssi"]   = WiFi.RSSI();          // truong tu them
  doc["uptime"] = millis() / 1000;
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Dang noi Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { delay(400); Serial.print("."); }
  Serial.println();
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());       // ghi dong nay vao README

  server.on("/api/sensor", HTTP_GET, handleSensor);
  server.begin();
}

void loop() {
  server.handleClient();
}
