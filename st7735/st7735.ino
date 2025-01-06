#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Library for ST7735

// Define pins
#define TFT_CS    17  // Chip select pin
#define TFT_RST   16  // Reset pin
#define TFT_DC    15  // Data/Command pin

// SPI connections (default for Pico)
#define TFT_MOSI  19  // SPI MOSI pin
#define TFT_SCLK  18  // SPI SCLK pin

// Create an instance of the display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);
  Serial.println("TFT Display Test");

  // Initialize the display
  tft.initR(INITR_144GREENTAB);  // Initialize with a specific tab
  tft.setRotation(1);           // Set rotation (0-3)

  // Fill the screen with a color
  tft.fillScreen(ST77XX_BLACK);

  // Display some text
  tft.setTextColor(ST77XX_WHITE);  // Set text color
  tft.setTextSize(1);              // Set text size
  tft.setCursor(10, 10);           // Set position (x, y)
  tft.println("Hello, World!");

  // Draw a rectangle
  tft.drawRect(20, 30, 50, 30, ST77XX_RED);

  // Draw a filled circle
  tft.fillCircle(60, 80, 20, ST77XX_BLUE);
}

void loop() {
  // Add any animations or updates here
}
