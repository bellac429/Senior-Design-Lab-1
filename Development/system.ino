#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>

#include "FS.h"

// ---------------- LCD Setup ----------------
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(23, 32, 33, 25, 26, 27);

// ---------------- Temperature Sensor Setup ----------------
#define ONE_WIRE_BUS 21  // DS18B20 data pin connected to GPIO 21
#define BUTTON1 18      // Button for Sensor 1
#define BUTTON2 19      // Button for Sensor 2
#define ONOFFSWITCH 22 // On off switch

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int numberOfDevices;
DeviceAddress tempDeviceAddress;

// Button state tracking
bool lastButton1State = HIGH;
bool lastButton2State = HIGH;
// Sensor State tracking
bool sensor1State = false;  // true = ON, false = OFF
bool sensor2State = false;  // true = ON, false = OFF
bool lastSensor1State = true; // when system boots, sensoor is ON
bool lastSensor2State = true;

// ----------------- WiFi Setup -----------------
const char* ssid = "iPhone";      // WiFi network name
const char* password = "burrito2";  // WiFi password

WiFiServer server(80); // Start a web server on port 80

// ----------------- Timers -----------------
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 1000; // 1 second timeout

float tempsF[2] = {75.20, 80.00};  // Fahrenheit
float tempsC[2] = {24, 26.67};  // Celsius

// =======================  Setup =======================
void setup() {
    // Initialize LCD
    lcd.begin(16, 2);

    // Initialize buttons with pull-up resistors
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    // On/off switch (use INPUT_PULLUP or INPUT depending on wiring)
    pinMode(ONOFFSWITCH, INPUT);

    // Initialize Serial Monitor
    Serial.begin(115200);
    Serial.println("Starting temperature sensors...");

    // Start DallasTemperature library
    sensors.begin();

    // Connect to Wi-Fi
    // WiFi.begin(ssid, password);
    // Serial.print("Connecting to WiFi: ");
    // while (WiFi.status() != WL_CONNECTED) {
    //     delay(500);
    //     Serial.print(".");
    // }
    // Serial.println("");
    // Serial.println("WiFi connected.");
    // Serial.println("ESP32 IP address: ");
    // Serial.println(WiFi.localIP());

    // server.begin();
}

// =======================  Main loop =======================
void loop() {
  WiFiClient client = server.available();   // Check for incoming client

  // Check ON/OFF switch
  bool switchState = digitalRead(ONOFFSWITCH);
  if (switchState == HIGH) {
    // Show "No data available"
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No data");
    lcd.setCursor(0, 1);
    lcd.print("available");
    delay(200);
    lcd.clear();

    // Disable server response
    // if (client) {
    //   client.stop(); 
    //   Serial.println("Client disconnected.");
    // }
    return;     
  }

  // ----- Normal hardware logic (buttons + LCD) -----
  bool currentButton1State = digitalRead(BUTTON1);
  bool currentButton2State = digitalRead(BUTTON2);

  if (currentButton1State == LOW && lastButton1State == HIGH) {
    lastSensor1State = sensor1State;
    sensor1State = !sensor1State;
  }
  if (currentButton2State == LOW && lastButton2State == HIGH) {
    lastSensor2State = sensor2State;
    sensor2State = !sensor2State;
  }

  lastButton1State = currentButton1State;
  lastButton2State = currentButton2State;

  // Read sensors
  sensors.requestTemperatures();
  float tempC1 = sensors.getTempCByIndex(0);
  float tempC2 = sensors.getTempCByIndex(1);

  // Save temps for webpage response
  tempsC[0] = tempC1; tempsF[0] = DallasTemperature::toFahrenheit(tempC1);
  tempsC[1] = tempC2; tempsF[1] = DallasTemperature::toFahrenheit(tempC2);

  // LCD update logic ===============================
  // Always update temperature if sensor 1 is ON
  if (sensor1State) {
      // Sensor 1 display
      lcd.setCursor(0, 0);
      lcd.print("S1:");
      // Sensor 2 display
      lcd.setCursor(0, 1);
      lcd.print("S2:");
    lcd.setCursor(4, 0); // Position for Sensor 1 reading
    tempC1 = sensors.getTempCByIndex(0);
    if (tempC1 == DEVICE_DISCONNECTED_C) {
      lcd.print("Error     ");
    } else {
      lcd.print(tempC1, 1);
      lcd.print((char)223);
      lcd.print("C");
    }
  } 
  else if (lastSensor1State != sensor1State && !sensor1State) { // Only update display if state changed to OFF
    // Sensor 1 display
    lcd.setCursor(0, 0);
    lcd.print("S1:");
    // Sensor 2 display
    lcd.setCursor(0, 1);
    lcd.print("S2:");
    lcd.setCursor(4, 0); // Position for Sensor 1 reading
    lcd.print("OFF    ");
  }

  // Always update temperature if sensor 2 is ON
  if (sensor2State) {
    // Sensor 1 display
    lcd.setCursor(0, 0);
    lcd.print("S1:");
    // Sensor 2 display
    lcd.setCursor(0, 1);
    lcd.print("S2:");
    lcd.setCursor(4, 1); // Position for Sensor 2 reading
    tempC2 = sensors.getTempCByIndex(1);
    if (tempC2 == DEVICE_DISCONNECTED_C) {
      lcd.print("Error     ");
    } else {
      lcd.print(tempC2, 1);
      lcd.print((char)223);
      lcd.print("C");
    }
  } 
  else if (lastSensor2State != sensor2State && !sensor2State) { // Only update display if state changed to OFF
    // Sensor 1 display
    lcd.setCursor(0, 0);
    lcd.print("S1:");
    // Sensor 2 display
    lcd.setCursor(0, 1);
    lcd.print("S2:");
    lcd.setCursor(4, 1); // Position for Sensor 2 reading
    lcd.print("OFF    ");
  }


  // ----- Webpage backend logic -----
//   if (client) {
//     Serial.println("New Client.");
//     String header = "";
//     unsigned long currentTime = millis();
//     unsigned long previousTime = currentTime;
//     const long timeoutTime = 2000;

//     while (client.connected() && currentTime - previousTime <= timeoutTime) {
//       currentTime = millis();
//       if (client.available()) {
//         char c = client.read();
//         header += c;
//         if (c == '\n') {
//           // ---- Always send headers ----
//           client.println("HTTP/1.1 200 OK");
//           client.println("Content-type:application/json");
//           client.println("Access-Control-Allow-Origin: *");
//           client.println("Connection: close");
//           client.println();

//           // ---- Routes ----
//           if (header.indexOf("GET /data") >= 0) {
//             client.print("{\"probe1\": {\"C\": ");
//             client.print(tempsC[0]);
//             client.print(", \"F\": ");
//             client.print(tempsF[0]);
//             client.print("}, \"probe2\": {\"C\": ");
//             client.print(tempsC[1]);
//             client.print(", \"F\": ");
//             client.print(tempsF[1]);
//             client.print("}}");
//           }

//           else if (header.indexOf("GET /18/on") >= 0) {
//             output18State = "on";
//             digitalWrite(output18, HIGH);
//             client.print("{\"status\":\"GPIO 18 ON\"}");
//           } else if (header.indexOf("GET /18/off") >= 0) {
//             output18State = "off";
//             digitalWrite(output18, LOW);
//             client.print("{\"status\":\"GPIO 18 OFF\"}");
//           }

//           else if (header.indexOf("GET /19/on") >= 0) {
//             output19State = "on";
//             digitalWrite(output19, HIGH);
//             client.print("{\"status\":\"GPIO 19 ON\"}");
//           } else if (header.indexOf("GET /19/off") >= 0) {
//             output19State = "off";
//             digitalWrite(output19, LOW);
//             client.print("{\"status\":\"GPIO 19 OFF\"}");
//           }

//           break;
//         }
//       }
//     }
//     client.stop();
//     Serial.println("Client disconnected.");
//   }

  delay(50);
}
