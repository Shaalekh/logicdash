#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Library for ST7735

// Define TFT display pins
#define TFT_CS    17
#define TFT_RST   16
#define TFT_DC    15
#define TFT_MOSI  19
#define TFT_SCLK  18

// Create an instance of the display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Rotary encoder pins
#define ENCODER_CLK  14  // Rotary encoder DT pin
#define ENCODER_DT   13  // Rotary encoder CLK pin
#define ENCODER_SW   12  // Rotary encoder button pin

volatile int encoderValue = 0;  // Tracks rotary encoder value
int lastEncoderState = HIGH;    // Last known state of CLK
int lastButtonState = HIGH;     // Last known state of the button
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("TFT and Rotary Encoder Test");

  // Initialize the TFT display
  tft.initR(INITR_144GREENTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  // Draw static text
  tft.setCursor(10, 10);
  tft.println("Encoder Value:");

  // Rotary encoder pins
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
}

void loop() {
  // Read the encoder state
  int currentState = digitalRead(ENCODER_CLK);
  if (currentState != lastEncoderState) {
    // Check direction
    if (digitalRead(ENCODER_DT) != currentState) {
      encoderValue++;
    } else {
      encoderValue--;
    }

    // Update the TFT display
    tft.fillRect(10, 30, 80, 20, ST77XX_BLACK);  // Clear the value area
    tft.setCursor(10, 30);
    tft.print("Value: ");
    tft.println(encoderValue);

    lastEncoderState = currentState;
  }

  // Handle button press
  int buttonState = digitalRead(ENCODER_SW);
  if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    encoderValue = 0;  // Reset the encoder value on button press

    // Update the TFT display
    tft.fillRect(10, 30, 80, 20, ST77XX_BLACK);  // Clear the value area
    tft.setCursor(10, 30);
    tft.print("Value: ");
    tft.println(encoderValue);

    Serial.println("Encoder reset!");
  }

  lastButtonState = buttonState;
}
