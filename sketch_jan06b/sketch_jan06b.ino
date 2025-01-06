#include <Keypad.h>

// Define the number of rows and columns
const byte ROWS = 2;  // 2 Rows
const byte COLS = 2;  // 2 Columns

// Define the keys on the keypad
char keys[ROWS][COLS] = {
  {'1', '2'},  // Button layout
  {'3', '4'}
};

// Connect row and column pins to the Arduino
byte rowPins[ROWS] = {4, 5};  // R1 → Pin 4, R2 → Pin 5
byte colPins[COLS] = {2, 3};  // L1 → Pin 2, L2 → Pin 3

// Create the Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);  // Initialize Serial Monitor
}

void loop() {
  char key = keypad.getKey();  // Check if a key is pressed

  if (key) {
    Serial.println(key);  // Print the key to the Serial Monitor
  }
}
