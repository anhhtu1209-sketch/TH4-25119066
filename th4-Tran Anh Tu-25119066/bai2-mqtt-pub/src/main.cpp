// Bai 2 - MQTT: publish telemetry moi 2 giay vao aet2022/<SV_ID>/telemetry
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "TEN_WIFI";          // DOI
const char* WIFI_PASS = "MAT_KHAU";          // DOI
const char* BROKER    = "broker.hivemq.com"; // DOI neu lop dung broker khac
const int   PORT      = 1883;
const char* SV_ID     = "sv-20xxxxxx";       // DOI: dung ma cua ban, tranh trung nhom khac

WiFiClient wifi;
PubSubClient mqtt(wifi);
String TOPIC_TELE;
String CLIENT_ID;
unsigned long lan = 0;

float docNhiet() { return 27.0 + 2.0 * sin(millis() / 10000.0); }
float docAm()    { return 55.0 + 5.0 * sin(millis() / 15000.0); }

void noiBroker() {
  while (!mqtt.connected()) {
    Serial.print("Noi broker... ");
    if (mqtt.connect(CLIENT_ID.c_str())) {
      Serial.println("OK");
    } else {
      Serial.print("loi rc="); Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(400); Serial.print("."); }
  Serial.println(WiFi.localIP());

  TOPIC_TELE = String("aet2022/") + SV_ID + "/telemetry";
  CLIENT_ID  = String("esp32-") + SV_ID + "-" + WiFi.macAddress();  // duy nhat moi kit
  mqtt.setServer(BROKER, PORT);
  Serial.println("Topic: " + TOPIC_TELE);
}

void loop() {
  if (!mqtt.connected()) noiBroker();
  mqtt.loop();                       // goi thuong xuyen

  if (millis() - lan >= 2000) {
    lan = millis();
    JsonDocument doc;
    doc["temp"]   = docNhiet();
    doc["hum"]    = docAm();
    doc["uptime"] = millis() / 1000;
    doc["rssi"]   = WiFi.RSSI();     // truong tu them
    char b[192];                     // du cho them truong
    serializeJson(doc, b);
    mqtt.publish(TOPIC_TELE.c_str(), b);
    Serial.println(b);
  }
}
