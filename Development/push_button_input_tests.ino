#define BUTTON1 18
#define BUTTON2 19

void setup() {
  pinMode(BUTTON1, INPUT_PULLUP);  // use internal pull-up resistors
  pinMode(BUTTON2, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Button test ready!");
}

void loop() {
  // Read button states (LOW = pressed, HIGH = released)
  bool button1Pressed = (digitalRead(BUTTON1) == LOW);
  bool button2Pressed = (digitalRead(BUTTON2) == LOW);

  if (button1Pressed) {
    Serial.println("Button 1 is PRESSED");
  } else {
    Serial.println("Button 1 is RELEASED");
  }

  if (button2Pressed) {
    Serial.println("Button 2 is PRESSED");
  } else {
    Serial.println("Button 2 is RELEASED");
  }

  delay(300); // slow down printing for readability
}
