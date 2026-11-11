// Bai 3 - MQTT hai chieu: publish telemetry + nut, subscribe lenh LED, hien thi OLED
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* WIFI_SSID = "TEN_WIFI";          // DOI
const char* WIFI_PASS = "MAT_KHAU";          // DOI
const char* BROKER    = "broker.hivemq.com"; // DOI
const int   PORT      = 1883;
const char* SV_ID     = "sv-20xxxxxx";       // DOI

#define PIN_DHT 4
#define PIN_LED 14
#define PIN_BTN 27            // nut keo xuong GND, dung INPUT_PULLUP

DHT dht(PIN_DHT, DHT11);
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
WiFiClient wifi;
PubSubClient mqtt(wifi);

String TOPIC_TELE, TOPIC_BTN, TOPIC_CMD, CLIENT_ID;
unsigned long lan = 0;
bool ledOn = false;
bool oledOk = false;
float nhiet = 0, am = 0;

void veOled() {
  if (!oledOk) return;
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);  oled.println(SV_ID);
  oled.setCursor(0, 16); oled.printf("Nhiet: %.1f C", nhiet);
  oled.setCursor(0, 28); oled.printf("Am:    %.1f %%", am);
  oled.setCursor(0, 44); oled.printf("LED: %s", ledOn ? "BAT" : "TAT");
  oled.display();
}

void datLed(bool on) {
  ledOn = on;
  digitalWrite(PIN_LED, on ? HIGH : LOW);
  veOled();
}

// Chieu ve: nhan lenh tu Node-RED
void onMessage(char* topic, byte* payload, unsigned int len) {
  if (len == 0) return;
  char c = (char)payload[0];
  Serial.printf("[cmd] %s = %c\n", topic, c);
  if (c == '1') datLed(true);
  else if (c == '0') datLed(false);
}

void noiBroker() {
  while (!mqtt.connected()) {
    Serial.print("Noi broker... ");
    if (mqtt.connect(CLIENT_ID.c_str())) {
      Serial.println("OK");
      mqtt.subscribe(TOPIC_CMD.c_str());   // subscribe lai sau moi lan noi
    } else {
      Serial.print("loi rc="); Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void docCamBien() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {            // khong co DHT -> so gia
    t = 27.0 + 2.0 * sin(millis() / 10000.0);
    h = 55.0 + 5.0 * sin(millis() / 15000.0);
  }
  nhiet = t; am = h;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);
  dht.begin();
  oledOk = oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(400); Serial.print("."); }
  Serial.println(WiFi.localIP());

  String base = String("aet2022/") + SV_ID;
  TOPIC_TELE = base + "/telemetry";
  TOPIC_BTN  = base + "/button";
  TOPIC_CMD  = base + "/cmd/led";
  CLIENT_ID  = String("esp32-") + SV_ID + "-" + WiFi.macAddress();

  mqtt.setServer(BROKER, PORT);
  mqtt.setCallback(onMessage);
  veOled();
}

void loop() {
  if (!mqtt.connected()) noiBroker();
  mqtt.loop();

  // nut nhan: publish khi doi trang thai (day theo su kien)
  static int btnTruoc = HIGH;
  static unsigned long tBtn = 0;
  int btn = digitalRead(PIN_BTN);
  if (btn != btnTruoc && millis() - tBtn > 50) {
    tBtn = millis();
    btnTruoc = btn;
    mqtt.publish(TOPIC_BTN.c_str(), btn == LOW ? "1" : "0");
  }

  if (millis() - lan >= 2000) {
    lan = millis();
    docCamBien();
    JsonDocument doc;
    doc["temp"]   = nhiet;
    doc["hum"]    = am;
    doc["uptime"] = millis() / 1000;
    doc["rssi"]   = WiFi.RSSI();
    doc["led"]    = ledOn ? 1 : 0;
    char b[192];
    serializeJson(doc, b);
    mqtt.publish(TOPIC_TELE.c_str(), b);
    veOled();
  }
}
