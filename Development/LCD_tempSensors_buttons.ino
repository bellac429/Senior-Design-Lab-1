#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ---------------- LCD Setup ----------------
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(14, 12, 27, 26, 25, 33);

// ---------------- Temperature Sensor Setup ----------------
#define ONE_WIRE_BUS 4  // DS18B20 data pin connected to GPIO 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int numberOfDevices;
DeviceAddress tempDeviceAddress;

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Initializing...");

  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("Starting temperature sensors...");

  // Start DallasTemperature library
  sensors.begin();

  // Count devices
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sensors found:");
  lcd.setCursor(0, 1);
  lcd.print(numberOfDevices);
  delay(2000); // Show device count for 2s
}

void loop() {
  sensors.requestTemperatures(); // Ask all sensors for temperature

  // Only display if at least 2 sensors found
  if (numberOfDevices >= 2) {
    // Read first sensor
    float tempC1 = sensors.getTempCByIndex(0);
    float tempC2 = sensors.getTempCByIndex(1);

    // Print to Serial Monitor
    Serial.print("Sensor 1: ");
    Serial.print(tempC1);
    Serial.print(" °C | Sensor 2: ");
    Serial.print(tempC2);
    Serial.println(" °C");

    // Print to LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("S1:");
    lcd.print(tempC1, 1); // 1 decimal
    lcd.print((char)223);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("S2:");
    lcd.print(tempC2, 1);
    lcd.print((char)223);
    lcd.print("C");
  } 
  else {
    // Not enough sensors
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Need 2 sensors!");
    Serial.println("Less than 2 temperature sensors detected.");
  }

  delay(500); // Update every 0.5 seconds
}
