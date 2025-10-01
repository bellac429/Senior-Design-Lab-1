// ================== ESP32 Temperature + GPIO Control ==================
// ESP32 reads DS18B20 sensors and POSTs data to a Python Flask server.
// It also receives GPIO control commands from the server.
// =====================================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

// ----------------- WiFi Setup -----------------
const char* ssid = "Adrian's IPhone";      // WiFi network name
const char* password = "Alvarez2006";      // WiFi password

// ----------------- Server Setup -----------------
const char* serverName = "http://192.168.1.100:5000/api/data"; 
// 👆 replace with your Python server's IP and port

// ----------------- GPIO Setup -----------------
const int output18 = 18;
const int output19 = 19;

// ----------------- Temperature Setup -----------------
const int oneWireBus = 4;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

float tempsF[2];
float tempsC[2];

// ----------------- Timers -----------------
unsigned long lastSendTime = 0;
const long sendInterval = 5000; // send every 5s

// ----------------- Setup -----------------
void setup() {
  Serial.begin(115200);
  sensors.begin();

  pinMode(output18, OUTPUT);
  pinMode(output19, OUTPUT);
  digitalWrite(output18, LOW);
  digitalWrite(output19, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

// ----------------- Loop -----------------
void loop() {
  unsigned long now = millis();

  if (now - lastSendTime >= sendInterval) {
    lastSendTime = now;

    // update temperature readings
    sensors.requestTemperatures();
    for (int i = 0; i < 2; i++) {
      tempsC[i] = sensors.getTempCByIndex(i);
      tempsF[i] = sensors.getTempFByIndex(i);
    }

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      // Build JSON payload
      String jsonPayload = "{";
      jsonPayload += "\"probe1\":{\"C\":" + String(tempsC[0]) + ",\"F\":" + String(tempsF[0]) + "},";
      jsonPayload += "\"probe2\":{\"C\":" + String(tempsC[1]) + ",\"F\":" + String(tempsF[1]) + "}";
      jsonPayload += "}";

      int httpResponseCode = http.POST(jsonPayload);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("Server response: ");
        Serial.println(response);

        // Parse response for GPIO commands
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, response);
        if (!error) {
          const char* gpio18 = doc["GPIO18"];
          const char* gpio19 = doc["GPIO19"];

          if (gpio18 && strcmp(gpio18, "ON") == 0) {
            digitalWrite(output18, HIGH);
            Serial.println("GPIO 18 -> ON");
          } else if (gpio18 && strcmp(gpio18, "OFF") == 0) {
            digitalWrite(output18, LOW);
            Serial.println("GPIO 18 -> OFF");
          }

          if (gpio19 && strcmp(gpio19, "ON") == 0) {
            digitalWrite(output19, HIGH);
            Serial.println("GPIO 19 -> ON");
          } else if (gpio19 && strcmp(gpio19, "OFF") == 0) {
            digitalWrite(output19, LOW);
            Serial.println("GPIO 19 -> OFF");
          }
        }
      } else {
        Serial.print("Error sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    } else {
      Serial.println("WiFi not connected, retrying...");
      WiFi.begin(ssid, password);
    }
  }
}
