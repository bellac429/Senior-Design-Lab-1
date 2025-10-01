#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials for iPhone hotspot
const char* ssid = "ESPHotspot";
const char* password = "Alvarez2006";

// Flask server details (update with your computer's IP address when connected to the hotspot)
const char* serverAddress = "http://127.0.0.1:5000/receive-message";  // update this IP


void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting ESP32...");

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Wait for connection with timeout
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected Successfully!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi!");
    Serial.println("Check SSID and password");
  }
}

void loop() {
  // Create JSON document
  StaticJsonDocument<200> doc;
  doc["message"] = "Hello from ESP32!";
  
  // Serialize JSON to string
  String jsonString;
  serializeJson(doc, jsonString);

  // Create HTTP client
  HTTPClient http;

  // Send POST request to Flask server
  Serial.print("Sending data to server: ");
  Serial.println(serverAddress);
  
  http.begin(serverAddress);
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.print("Server response: ");
    Serial.println(response);
  } else {
    Serial.print("Error sending POST request. Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  
  delay(2000);  // Send message every 2 seconds
}