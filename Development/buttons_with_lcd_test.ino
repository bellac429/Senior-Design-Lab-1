#include <LiquidCrystal.h>

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(14, 12, 27, 26, 25, 33);

// Button pins
#define BUTTON1 18
#define BUTTON2 19

// Variables to track button states
bool lastButton1State = HIGH;
bool lastButton2State = HIGH;

void setup() {
  // LCD setup
  lcd.begin(16, 2);
  lcd.print("Hello ESP32");

  // Button setup
  pinMode(BUTTON1, INPUT_PULLUP);  
  pinMode(BUTTON2, INPUT_PULLUP);  
}

void loop() {
  // Read button states
  bool button1State = digitalRead(BUTTON1);
  bool button2State = digitalRead(BUTTON2);

  // Detect button 1 press + release
  if (lastButton1State == LOW && button1State == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button 1 pressed");
  }

  // Detect button 2 press + release
  if (lastButton2State == LOW && button2State == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button 2 pressed");
  }

  // Save states for next loop
  lastButton1State = button1State;
  lastButton2State = button2State;
}
