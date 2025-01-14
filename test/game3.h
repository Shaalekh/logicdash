#ifndef GAME3_H
#define GAME3_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

extern Adafruit_ST7735 tft;

void playGame3() {
  tft.fillScreen(ST7735_RED);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 20);
  tft.print("Playing Game 3");
  delay(2000);  // Simulate game logic
  tft.setCursor(10, 40);
  tft.print("Game Over!");
}

#endif
