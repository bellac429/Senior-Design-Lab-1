#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ---------------- LCD Setup ----------------
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(23, 32, 33, 25, 26, 27);

// ---------------- Temperature Sensor Setup ----------------
#define ONE_WIRE_BUS 21  // DS18B20 data pin connected to GPIO 21
#define BUTTON1 18      // Button for Sensor 1
#define BUTTON2 19      // Button for Sensor 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int numberOfDevices;
DeviceAddress tempDeviceAddress;

// Button state tracking
bool lastButton1State = HIGH;
bool lastButton2State = HIGH;
// Sensor State tracking
bool sensor1State = true;  // true = ON, false = OFF
bool sensor2State = true;  // true = ON, false = OFF
bool lastSensor1State = true; // when system boots, sensoor is ON
bool lastSensor2State = true;


// =======================  Setup =======================
void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Initializing...");

  // Initialize buttons with pull-up resistors
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

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
  delay(2000);
  lcd.clear();

  // Sensor 1 display
  lcd.setCursor(0, 0);
  lcd.print("S1:");
  // Sensor 2 display
  lcd.setCursor(0, 1);
  lcd.print("S2:");
}

// =======================  Main loop =======================
void loop() {
  // Read button states (LOW = pressed, HIGH = released)
  bool currentButton1State = digitalRead(BUTTON1);
  bool currentButton2State = digitalRead(BUTTON2);
  // Check for button 1 press (falling edge)
  if (currentButton1State == LOW && lastButton1State == HIGH) {
    lastSensor1State = sensor1State;
    sensor1State = !sensor1State;  // Toggle state
    //delay(50);  // Simple debounce
  }
  // Check for button 2 press (falling edge)
  if (currentButton2State == LOW && lastButton2State == HIGH) {
    lastSensor2State = sensor2State;
    sensor2State = !sensor2State;  // Toggle state
    //delay(50);  // Simple debounce
  }
  // Update last button states
  lastButton1State = currentButton1State;
  lastButton2State = currentButton2State;


  // Update temperature display
  sensors.requestTemperatures(); // MAYBE MOVE THIS TO INSIDE IF STATEMENTS?

  // Always update temperature if sensor 1 is ON
  if (sensor1State) {
    lcd.setCursor(4, 0); // Position for Sensor 1 reading
    float tempC1 = sensors.getTempCByIndex(0);
    if (tempC1 == DEVICE_DISCONNECTED_C) {
      lcd.print("Error     ");
    } else {
      lcd.print(tempC1, 1);
      lcd.print((char)223);
      lcd.print("C");
    }
  } else if (lastSensor1State != sensor1State && !sensor1State) { // Only update display if state changed to OFF
    lcd.setCursor(4, 0); // Position for Sensor 1 reading
    lcd.print("OFF    ");
  }

  // Always update temperature if sensor 2 is ON
  if (sensor2State) {
    lcd.setCursor(4, 1); // Position for Sensor 2 reading
    float tempC2 = sensors.getTempCByIndex(1);
    if (tempC2 == DEVICE_DISCONNECTED_C) {
      lcd.print("Error     ");
    } else {
      lcd.print(tempC2, 1);
      lcd.print((char)223);
      lcd.print("C");
    }
  } else if (lastSensor2State != sensor2State && !sensor2State) { // Only update display if state changed to OFF
    lcd.setCursor(4, 1); // Position for Sensor 2 reading
    lcd.print("OFF    ");
  }

  delay(50); // Update X milliseconds
}