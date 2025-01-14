#include "game1.h" // Include the header file for Game 1

// Pin Definitions
#define TFT_CS   17  // Chip select
#define TFT_RST  16  // Reset
#define TFT_DC   15  // Data/Command
#define EXIT_BTN  11 // Exit button pin

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST); // Initialize the display

void setup() {
  tft.initR(INITR_144GREENTAB);  // Initialize the display
  tft.setRotation(1);           // Set the display rotation
  pinMode(EXIT_BTN, INPUT_PULLUP); // Configure the exit button
}

void loop() {
  // Example of how to call Game 1
  runGame1(tft, EXIT_BTN);

  // Once the user presses the exit button, return to the home menu
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 20);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print("Returning to Home...");
  delay(2000); // Simulate returning to the home menu
}
