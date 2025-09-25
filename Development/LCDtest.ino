#include <LiquidCrystal.h>

// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(14, 12, 27, 26, 25, 33);

void setup() {
  lcd.begin(16, 2);       // initialize 16x2 LCD
  lcd.print("Hello ESP32");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}
