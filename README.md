ESP32-CAM MQTT Based IoT Monitoring System

Overview

This project is a beginner-level Embedded Systems and Industrial IoT (IIoT) project that demonstrates real-time sensor monitoring using the MQTT protocol.

The system uses:

- ESP32-CAM
- DHT22 Temperature & Humidity Sensor
- MQ2 Gas/Smoke Sensor
- Mosquitto MQTT Broker
- MQTT Explorer

Sensor telemetry is transmitted over WiFi using MQTT and visualized in real time.

---

Project Architecture

DHT22 + MQ2
↓
ESP32-CAM (Publisher)
↓
WiFi
↓
Mosquitto MQTT Broker
↓
MQTT Explorer (Subscriber)

---

MQTT Components

MQTT Component| Used Device
Publisher| ESP32-CAM
Broker| Mosquitto
Subscriber| MQTT Explorer

---

Features

- Real-time MQTT communication
- WiFi-based telemetry transmission
- JSON payload support
- Temperature & humidity monitoring
- Smoke/gas detection
- Live MQTT Explorer graphs
- Auto reconnect support
- Local MQTT broker setup

---

Components Used

- ESP32-CAM
- DHT22 Sensor
- MQ2 Gas Sensor
- Arduino UNO
- Breadboard
- Jumper Wires
- 10kΩ Resistor

---

Sensor Connections

DHT22

DHT22| ESP32-CAM
VCC| 3.3V
DATA| GPIO14
GND| GND

10kΩ resistor between:
DATA ↔ 3.3V

---

MQ2

MQ2| ESP32-CAM
VCC| 5V
GND| GND
D0| GPIO13

---

ESP32-CAM Upload Connections

Arduino UNO| ESP32-CAM
RESET| GND
5V| 5V
GND| GND
TX| U0R
RX| U0T

For upload mode:
GPIO0 → GND

Disconnect GPIO0 ↔ GND after upload.

---

Software Used

- Arduino IDE
- Mosquitto MQTT Broker
- MQTT Explorer

---

Required Libraries

- PubSubClient
- ArduinoJson
- DHT Sensor Library
- Adafruit Unified Sensor

---

MQTT Topics

Telemetry:
iiot/lakshya/station01/telemetry

Alert:
iiot/lakshya/station01/alert

Command:
iiot/lakshya/station01/command

---

Example JSON Payload

{
  "device":"station01",
  "temperature":33.2,
  "humidity":60,
  "smoke_alert":false
}

Mosquitto Test Commands

Subscriber:

mosquitto_sub -h localhost -t "iiot/#" -v

Publisher:

mosquitto_pub -h localhost -t "iiot/test" -m "hello"

Learning Outcomes

- MQTT fundamentals
- Publish/Subscribe architecture
- Embedded WiFi communication
- JSON telemetry handling
- Sensor interfacing
- Real-time IoT systems
- MQTT Broker setup

Future Improvements

- Cloud MQTT integration
- Node-RED dashboard
- OLED display
- Relay automation
- OTA updates
- Secure MQTT (TLS)

Conclusion

This project demonstrates a complete beginner-level MQTT-based IoT telemetry pipeline using ESP32-CAM and sensors. It provides practical understanding of Embedded Systems, MQTT communication, and real-time IIoT monitoring.
