// loads the required wifi library
#include <WiFi.h>
// loads the required temp sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>

// IP Address: 172.23.17.235

// grabs the required ssid and password to connect the esp32 to wifi.
// the esp32 was connected to university wifi by registering it through
// deviceNet by getting the MAC address first
const char* ssid = "UI-DeviceNet";
const char* password = "UI-DeviceNet";

// Set web server port number to 80
// Port 80 is the designated port for the Hypertext Transfer Protocol (HTTP)
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output18State = "off";
String output19State = "off";

// Assign output variables to GPIO pins
// TODO: Change pins
const int output18 = 18;
const int output19 = 19;

// Current time
unsigned long currentTime = millis();
// Previous time

unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 1000;

// GPIO where the DS18B20 (temp sensor) is connected to
const int oneWireBus = 4;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Number of temperature devices
int numberOfDevices = 2;

// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress;

// An array for storing the farehnheir temps
float tempsF[2];

// An array for storing th celcius temps
float tempsC[2];

// this only runs once when the esp boots
void setup() {
  // start the serial monitor
  Serial.begin(115200);
  // Start the DS18B20 (temp) sensor
  sensors.begin();
  // Set the GPIOs as outputs
  pinMode(output18, OUTPUT);
  pinMode(output19, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output18, LOW);
  digitalWrite(output19, LOW);

  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

// establish what happens when a new client establish a connection with a web server
void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients, esp is always listening

  sensors.requestTemperatures(); // Send the command to get temperatures

  // set the temperature to their respective variables
  float temperatureC = sensors.getTempCByIndex(0); // get the temp in celcius
  float temperatureF = sensors.getTempFByIndex(0); // get the temp in farenheit

  // print to the serial monitor
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");

  for(int i=0; i < numberOfDevices; i++){
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      // Output the device ID
      //Serial.print("Temperature for device: ");
      //Serial.println(i,DEC);
      // Print the data
      float tempC = sensors.getTempC(tempDeviceAddress);
      tempsC[i] = tempC; // stores the celcius temperature into the array
      //Serial.print("Temp C: ");
      //Serial.print(tempC);
      float tempF = DallasTemperature::toFahrenheit(tempC);
      tempsF[i] = tempF; // stores the farenheit temperature into the array
      //Serial.print(" Temp F: ");
      //Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
    }
  }

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    // this while loop runs as long as the client stays connected
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            


            // checks which button is pressed in the webpage
            // turns the GPIOs on and off
            if (header.indexOf("GET /18/on") >= 0) {
              Serial.println("GPIO 18 on"); // prints message onto serial monitor
              output18State = "on"; // changes the output state to on for the html
              digitalWrite(output18, HIGH); // turns the button on
            } 
            else if (header.indexOf("GET /18/off") >= 0) {
              Serial.println("GPIO 18 off");
              output18State = "off";
              digitalWrite(output18, LOW);
            } 
            else if (header.indexOf("GET /19/on") >= 0) {
              Serial.println("GPIO 19 on");
              output19State = "on";
              digitalWrite(output19, HIGH);
            } 
            else if (header.indexOf("GET /19/off") >= 0) {
              Serial.println("GPIO 19 off");
              output19State = "off";
              digitalWrite(output19, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            // Probably dont need
            // client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 18
            // aka the thermometer 1  
            client.println("<p>Probe 1 " + String(tempsF[0], 2) + "</p>");
            // If the thermometer 1 is off, it displays the ON button       
            if (output18State=="off") {
              client.println("<p><a href=\"/18/on\"><button class=\"button\">ON</button></a></p>");
            }
            // if the thermometer 1 is on, it displays the OFF button
            else {
              client.println("<p><a href=\"/18/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 19  
            // aka the thermometer 2
            client.println("<p>Probe 2 " + String(tempsF[1], 2)/*tempsF[1]*/ + "</p>");
            // If the thermometer 2 is off, it displays the ON button       
            if (output19State=="off") {
              client.println("<p><a href=\"/19/on\"><button class=\"button\">ON</button></a></p>");
            } 
            // if the thermometer 2 is on, it displays the Off button
            else {
              client.println("<p><a href=\"/19/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
