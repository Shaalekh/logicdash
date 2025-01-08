// Pin connected to the push button
const int buttonPin = 9;

// Variable to store the button state
int buttonState = 0;

void setup() {
  // Initialize the button pin as input
  pinMode(buttonPin, INPUT_PULLUP);

  // Start serial communication at 9600 baud rate
  Serial.begin(9600);

  // Optional: Add a startup message
  Serial.println("Push Button Monitoring Initialized");
}

void loop() {
  // Read the state of the button
  buttonState = digitalRead(buttonPin);

  // Check if the button is pressed (LOW when using INPUT_PULLUP)
  if (buttonState == LOW) {
    Serial.println("Button Pressed");
  } else {
    Serial.println("Button Released");
  }

  // Small delay to reduce serial output flooding
  delay(100);
}
