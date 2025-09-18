#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#endif
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is connected to GPIO 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Variables to store temperature values for two sensors
String temperatureC1 = "";
String temperatureF1 = "";
String temperatureC2 = "";
String temperatureF2 = "";

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;

// Replace with your network credentials
const char* ssid = "UI-DeviceNet";
const char* password = "UI-DeviceNet";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Function to read sensor values by index
String readDSTemperatureC(int index) {
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(index);
  if(tempC == -127.00) {
    Serial.printf("Failed to read Celsius from DS18B20 sensor %d\n", index);
    return "--";
  } else {
    Serial.printf("Sensor %d Celsius: %.2f\n", index, tempC);
  }
  return String(tempC);
}

String readDSTemperatureF(int index) {
  sensors.requestTemperatures(); 
  float tempF = sensors.getTempFByIndex(index);
  if(int(tempF) == -196){
    Serial.printf("Failed to read Fahrenheit from DS18B20 sensor %d\n", index);
    return "--";
  } else {
    Serial.printf("Sensor %d Fahrenheit: %.2f\n", index, tempF);
  }
  return String(tempF);
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css">
  <style>
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center; }
    h2 { font-size: 2.5rem; }
    p { font-size: 2.0rem; }
    .units { font-size: 1.2rem; }
    .ds-labels { font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px; }
  </style>
</head>
<body>
  <h2>ESP32 Dual DS18B20 Server</h2>
  <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="ds-labels">Sensor 1 Celsius:</span> 
    <span id="temperaturec1">%TEMPERATUREC1%</span><sup class="units">&deg;C</sup>
  </p>
  <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="ds-labels">Sensor 1 Fahrenheit:</span>
    <span id="temperaturef1">%TEMPERATUREF1%</span><sup class="units">&deg;F</sup>
  </p>
  <hr>
  <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="ds-labels">Sensor 2 Celsius:</span> 
    <span id="temperaturec2">%TEMPERATUREC2%</span><sup class="units">&deg;C</sup>
  </p>
  <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="ds-labels">Sensor 2 Fahrenheit:</span>
    <span id="temperaturef2">%TEMPERATUREF2%</span><sup class="units">&deg;F</sup>
  </p>
</body>
<script>
function updateValue(id, url) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById(id).innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", url, true);
  xhttp.send();
}

setInterval(function() {
  updateValue("temperaturec1", "/temperaturec1");
  updateValue("temperaturef1", "/temperaturef1");
  updateValue("temperaturec2", "/temperaturec2");
  updateValue("temperaturef2", "/temperaturef2");
}, 10000);
</script>
</html>)rawliteral";

// Replaces placeholder with DS18B20 values
String processor(const String& var){
  if(var == "TEMPERATUREC1"){ return temperatureC1; }
  else if(var == "TEMPERATUREF1"){ return temperatureF1; }
  else if(var == "TEMPERATUREC2"){ return temperatureC2; }
  else if(var == "TEMPERATUREF2"){ return temperatureF2; }
  return String();
}

void setup(){
  Serial.begin(115200);
  Serial.println();
  
  sensors.begin();
  int deviceCount = sensors.getDeviceCount();
  Serial.printf("Found %d DS18B20 devices\n", deviceCount);

  // Initial readings
  temperatureC1 = readDSTemperatureC(0);
  temperatureF1 = readDSTemperatureF(0);
  temperatureC2 = readDSTemperatureC(1);
  temperatureF2 = readDSTemperatureF(1);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println();  
  Serial.println(WiFi.localIP());

  // Root page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Sensor 1 endpoints
  server.on("/temperaturec1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", temperatureC1.c_str());
  });
  server.on("/temperaturef1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", temperatureF1.c_str());
  });

  // Sensor 2 endpoints
  server.on("/temperaturec2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", temperatureC2.c_str());
  });
  server.on("/temperaturef2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", temperatureF2.c_str());
  });

  server.begin();
}
 
void loop(){
  if ((millis() - lastTime) > timerDelay) {
    temperatureC1 = readDSTemperatureC(0);
    temperatureF1 = readDSTemperatureF(0);
    temperatureC2 = readDSTemperatureC(1);
    temperatureF2 = readDSTemperatureF(1);
    lastTime = millis();
  }  
}
