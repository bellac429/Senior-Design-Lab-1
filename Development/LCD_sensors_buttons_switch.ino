#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ---------------- LCD Setup ----------------
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(14, 12, 27, 26, 25, 33);

// ---------------- Temperature Sensor Setup ----------------
#define ONE_WIRE_BUS 4  // DS18B20 data pin connected to GPIO 4
#define BUTTON1 18      // Button for Sensor 1
#define BUTTON2 19      // Button for Sensor 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int numberOfDevices;
DeviceAddress tempDeviceAddress;

// Button state tracking
bool sensor1State = true;  // true = ON, false = OFF
bool sensor2State = true;  // true = ON, false = OFF
bool lastButton1State = HIGH;
bool lastButton2State = HIGH;

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
}

void loop() {

  // Read button states (LOW = pressed, HIGH = released)
//   bool button1Pressed = (digitalRead(BUTTON1) == LOW);
//   bool button2Pressed = (digitalRead(BUTTON2) == LOW);

//   if (button1Pressed) {
//     Serial.println("Button 1 is PRESSED");
//   } else {
//     Serial.println("Button 1 is RELEASED");
//   }
///////////////////////////////// FIX THIS BUTTON LOGIC ///////////////////////////////////////
  // Read button states (LOW = pressed, HIGH = released)
  bool currentButton1State = digitalRead(BUTTON1);
  bool currentButton2State = digitalRead(BUTTON2);

  // Check for button 1 press (falling edge)
  if (currentButton1State == LOW && lastButton1State == HIGH) {
    sensor1State = !sensor1State;  // Toggle state
    //delay(50);  // Simple debounce
  }

  // Check for button 2 press (falling edge)
  if (currentButton2State == LOW && lastButton2State == HIGH) {
    sensor2State = !sensor2State;  // Toggle state
    //delay(50);  // Simple debounce
  }

  // Update last button states
  lastButton1State = currentButton1State;
  lastButton2State = currentButton2State;


///////////////////////////////////////////////////////////////////////////////////////////////

  // Update temperature display
  sensors.requestTemperatures();

  if (numberOfDevices >= 2) {
    //lcd.clear();
    
    // Sensor 1 display
    lcd.setCursor(0, 0);
    lcd.print("S1:");
    if (sensor1State) {
      float tempC1 = sensors.getTempCByIndex(0);
      lcd.print(tempC1, 1);
      lcd.print((char)223);
      lcd.print("C");
    } else {
      lcd.print("OFF    ");
    }

    // Sensor 2 display
    lcd.setCursor(0, 1);
    lcd.print("S2:");
    if (sensor2State) {
      float tempC2 = sensors.getTempCByIndex(1);
      lcd.print(tempC2, 1);
      lcd.print((char)223);
      lcd.print("C");
    } else {
      lcd.print("OFF    ");
    }

    // Serial output for debugging
    // Serial.print("Sensor 1: ");
    // Serial.print(sensor1State ? String(sensors.getTempCByIndex(0)) : "OFF");
    // Serial.print(" | Sensor 2: ");
    // Serial.println(sensor2State ? String(sensors.getTempCByIndex(1)) : "OFF");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Need 2 sensors!");
    //Serial.println("Less than 2 temperature sensors detected.");
  }

  delay(20); // Update every 20 milliseconds
}