// ================== ESP32 Temperature Monitor ==================
// This code connects an ESP32 to Wi-Fi, reads temperatures from
// two DS18B20 sensors, and provides a simple HTTP API that a
// webpage can use to display live data and control GPIO pins.
// ================================================================

#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ----------------- WiFi Setup -----------------
const char* ssid = "UI-DeviceNet";      // WiFi network name
const char* password = "UI-DeviceNet";  // WiFi password

WiFiServer server(80); // Start a web server on port 80

// ----------------- GPIO Setup -----------------
const int output18 = 18;  // Pin 18
const int output19 = 19;  // Pin 19

String output18State = "off"; // track ON/OFF state
String output19State = "off";
String temp18State = "F";     // track unit (not really needed)
String temp19State = "F";

// ----------------- Temperature Setup -----------------
const int oneWireBus = 4; // pin where DS18B20 sensors are connected
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

float tempsF[2];  // Fahrenheit
float tempsC[2];  // Celsius

// ----------------- Timers -----------------
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 1000; // 1 second timeout

// ----------------- Setup Function -----------------
void setup() {
  Serial.begin(115200);
  sensors.begin();

  pinMode(output18, OUTPUT);
  pinMode(output19, OUTPUT);
  digitalWrite(output18, LOW);
  digitalWrite(output19, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

// ----------------- Loop Function -----------------
void loop() {
  WiFiClient client = server.available();

  // update temperature readings
  sensors.requestTemperatures();
  for (int i = 0; i < 2; i++) {
    tempsC[i] = sensors.getTempCByIndex(i);
    tempsF[i] = sensors.getTempFByIndex(i);
  }

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    String header = "";

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {

          // ---- Always send response headers ----
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:application/json");
          client.println("Connection: close");
          client.println();

          // ---- Routes ----

          // Send temperature data
          if (header.indexOf("GET /data") >= 0) {
            client.print("{\"probe1\": {\"C\": ");
            client.print(tempsC[0]);
            client.print(", \"F\": ");
            client.print(tempsF[0]);
            client.print("}, \"probe2\": {\"C\": ");
            client.print(tempsC[1]);
            client.print(", \"F\": ");
            client.print(tempsF[1]);
            client.print("}}");
          }

          // GPIO 18 control
          else if (header.indexOf("GET /18/on") >= 0) {
            output18State = "on";
            digitalWrite(output18, HIGH);
            client.print("{\"status\":\"GPIO 18 ON\"}");
          } else if (header.indexOf("GET /18/off") >= 0) {
            output18State = "off";
            digitalWrite(output18, LOW);
            client.print("{\"status\":\"GPIO 18 OFF\"}");
          }

          // GPIO 19 control
          else if (header.indexOf("GET /19/on") >= 0) {
            output19State = "on";
            digitalWrite(output19, HIGH);
            client.print("{\"status\":\"GPIO 19 ON\"}");
          } else if (header.indexOf("GET /19/off") >= 0) {
            output19State = "off";
            digitalWrite(output19, LOW);
            client.print("{\"status\":\"GPIO 19 OFF\"}");
          }

          // Unit toggling (not strictly needed since /data sends both)
          else if (header.indexOf("GET /18/C") >= 0) {
            temp18State = "C";
            client.print("{\"status\":\"18 C\"}");
          } else if (header.indexOf("GET /18/F") >= 0) {
            temp18State = "F";
            client.print("{\"status\":\"18 F\"}");
          } else if (header.indexOf("GET /19/C") >= 0) {
            temp19State = "C";
            client.print("{\"status\":\"19 C\"}");
          } else if (header.indexOf("GET /19/F") >= 0) {
            temp19State = "F";
            client.print("{\"status\":\"19 F\"}");
          }

          break; // done processing
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
  }
}
