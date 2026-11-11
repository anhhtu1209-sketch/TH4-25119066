# TH4 - REST / MQTT / Node-RED (AET2022)

- Sinh vien: <HO TEN> - <MSSV>
- SV_ID dung trong topic: `sv-<MSSV>`
- Broker: `broker.hivemq.com:1883`
- IP ESP32 (Bai 1): `192.168.x.x`   <-- ghi IP that

## So do noi chan
| Linh kien | Chan ESP32 |
|---|---|
| DHT11 DATA | GPIO4 |
| LED (+220 ohm ve GND) | GPIO14 |
| Nut nhan (ve GND) | GPIO27 |
| OLED SSD1306 SDA/SCL | GPIO21 / GPIO22 |

## Bai 1 - REST
`GET /api/sensor` tra `{id, temp, hum, rssi, uptime}`. Truong tu them: `id`, `rssi`.
Anh: `media/bai1-json.png`

## Bai 2 - MQTT publish
Topic `aet2022/<SV_ID>/telemetry`, 2 giay/lan. Truong tu them: `rssi`.
Client id ghep MAC de khong trung giua cac kit.
Anh: `media/bai2-explorer.png`

## Bai 3 - Node-RED
Flow: `nodered/flow.json`. Chieu di: `mqtt in` -> `function` -> gauge/chart.
Chieu ve: `ui_switch` -> `mqtt out` (`cmd/led`, gia tri `1`/`0`) -> ESP32 bat/tat LED, OLED doi.
Phan tu them: gauge uptime. Anh/video: `media/bai3-dashboard.png`, `media/bai3-led.mp4`

## Bai 4 - MQTT Explorer
Subscribe `aet2022/<SV_ID>/#`: thay `telemetry`, `button`, `cmd/led`.
Anh: `media/bai4-explorer-tree.png`

## Tra loi nhanh
- Can `mqtt.loop()` de client xu ly goi tin den va giu keep-alive; neu khong se mat ket noi va khong nhan duoc lenh.
- Client id trung thi broker ngat ket noi cai cu, hai kit dá nhau lien tuc.
