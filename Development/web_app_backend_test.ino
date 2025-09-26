#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WebServer.h>

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WebServer server(80);

const char* ssid = "ESP32-TempNet";
const char* password = "12345678";

const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>ESP32 Temp</title></head>
<body>
  <h1>Temperature Monitor</h1>
  <p>Temperature: <span id="temp">--</span> °C</p>
  <script>
    async function update() {
      let res = await fetch('/temperature');
      let data = await res.json();
      document.getElementById('temp').innerText = data.temperature;
    }
    setInterval(update, 2000);
    update();
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleTemperature() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  String json = "{ \"temperature\": " + String(tempC) + " }";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000); // give time for serial to open

  sensors.begin();

  Serial.println("Starting Access Point...");
  bool apResult = WiFi.softAP(ssid, password);

  if (apResult) {
    Serial.println("Access Point started successfully!");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Failed to start Access Point!");
  }

  server.on("/", handleRoot);
  server.on("/temperature", handleTemperature);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();
}
