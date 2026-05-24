#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

//
// ======================================================
// WiFi Credentials
// ======================================================
//

const char* ssid     = "WIFI ID";
const char* password = "WIFI PASSWORD";

//
// ======================================================
// MQTT Broker Configuration
// ======================================================
//

// Your PC IP Address
// Example: 192.168.1.5

const char* mqtt_server = "YOUR DESKTOP IP ADRESS Example: 182.160.1.5";

const int mqtt_port = 1883;

//
// ======================================================
// MQTT Topics
// ======================================================
//

const char* topicTelemetry =
"iiot/lakshya/station01/telemetry";

const char* topicAlert =
"iiot/lakshya/station01/alert";

//
// ======================================================
// DHT22 Configuration
// ======================================================
//

#define DHTPIN   14
#define DHTTYPE  DHT22

DHT dht(DHTPIN, DHTTYPE);

//
// ======================================================
// MQ2 Digital Pin
// ======================================================
//

// ONLY digital output used

#define MQ2_DIGITAL 13

//
// ======================================================
// WiFi + MQTT Objects
// ======================================================
//

WiFiClient espClient;

PubSubClient client(espClient);

//
// ======================================================
// Publish Timing
// ======================================================
//

unsigned long lastPublish = 0;

const long publishInterval = 10000;

//
// ======================================================
// Connect WiFi
// ======================================================
//

void connectWiFi()
{
  Serial.println();
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi Connected!");

  Serial.print("ESP32 IP Address: ");

  Serial.println(WiFi.localIP());
}

//
// ======================================================
// MQTT Callback
// ======================================================
//

void mqttCallback(char* topic,
                  byte* payload,
                  unsigned int length)
{
  Serial.print("Message received on topic: ");

  Serial.println(topic);

  String message = "";

  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.print("Payload: ");

  Serial.println(message);
}

//
// ======================================================
// Connect MQTT Broker
// ======================================================
//

void connectMQTT()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT Broker...");

    String clientID = "ESP32CAM-";

    clientID += String(random(0xffff), HEX);

    if (client.connect(clientID.c_str()))
    {
      Serial.println("Connected!");

      client.subscribe(
        "iiot/lakshya/station01/command"
      );
    }
    else
    {
      Serial.print("Failed. State = ");

      Serial.print(client.state());

      Serial.println(" -> Retrying in 3 seconds");

      delay(3000);
    }
  }
}

//
// ======================================================
// Read Sensors + Publish MQTT
// ======================================================
//

void publishSensorData()
{
  //
  // Read DHT22
  //

  float temperature =
    dht.readTemperature();

  float humidity =
    dht.readHumidity();

  //
  // Validate DHT22
  //

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("DHT22 Read Failed!");

    return;
  }

  //
  // Read MQ2 Digital Output
  //

  int mq2State =
    digitalRead(MQ2_DIGITAL);

  //
  // Smoke Detection Logic
  //

  bool smokeDetected = false;

  // Most MQ2 modules:
  // LOW = Smoke Detected

  if (mq2State == LOW)
  {
    smokeDetected = true;
  }

  //
  // Create JSON Payload
  //

  StaticJsonDocument<256> doc;

  doc["device"]      = "station01";

  doc["temperature"] = temperature;

  doc["humidity"]    = humidity;

  doc["mq2_digital"] = mq2State;

  doc["smoke_alert"] = smokeDetected;

  doc["wifi_rssi"]   = WiFi.RSSI();

  //
  // Convert JSON to String
  //

  char payload[256];

  serializeJson(doc, payload);

  //
  // Publish Telemetry
  //

  bool success =
    client.publish(topicTelemetry,
                   payload);

  if (success)
  {
    Serial.println();

    Serial.println("Telemetry Published:");

    Serial.println(payload);
  }
  else
  {
    Serial.println("MQTT Publish Failed!");
  }

  //
  // Publish Alert Topic
  //

  if (smokeDetected)
  {
    StaticJsonDocument<128> alertDoc;

    alertDoc["alert"] =
      "SMOKE_DETECTED";

    char alertPayload[128];

    serializeJson(alertDoc,
                  alertPayload);

    client.publish(topicAlert,
                   alertPayload);

    Serial.println(
      "Smoke Alert Published!"
    );
  }
}

//
// ======================================================
// Setup
// ======================================================
//

void setup()
{
  Serial.begin(115200);

  Serial.println();

  Serial.println(
    "ESP32-CAM MQTT Project Starting"
  );

  //
  // Initialize DHT22
  //

  dht.begin();

  //
  // Initialize MQ2 Digital Pin
  //

  pinMode(MQ2_DIGITAL, INPUT);

  //
  // MQ2 Warmup
  //

  Serial.println("MQ2 Warming Up...");

  delay(30000);

  //
  // Connect WiFi
  //

  connectWiFi();

  //
  // Configure MQTT
  //

  client.setServer(
    mqtt_server,
    mqtt_port
  );

  client.setCallback(mqttCallback);

  //
  // Connect MQTT
  //

  connectMQTT();
}

//
// ======================================================
// Main Loop
// ======================================================
//

void loop()
{
  //
  // Reconnect WiFi
  //

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi Lost!");

    connectWiFi();
  }

  //
  // Reconnect MQTT
  //

  if (!client.connected())
  {
    Serial.println("MQTT Disconnected!");

    connectMQTT();
  }

  //
  // MQTT Background Tasks
  //

  client.loop();

  //
  // Publish Every 10 Seconds
  //

  unsigned long currentMillis = millis();

  if (currentMillis - lastPublish
      >= publishInterval)
  {
    lastPublish = currentMillis;

    publishSensorData();
  }
}
