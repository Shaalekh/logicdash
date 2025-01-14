#ifndef GAME1_H
#define GAME1_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

extern Adafruit_ST7735 tft;

void playGame1() {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 20);
  tft.print("Playing Game 1");
  delay(2000);  // Simulate game logic
  tft.setCursor(10, 40);
  tft.print("Game Over!");
}

#endif
