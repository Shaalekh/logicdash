#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "game1.h"  // Include Game 1
#include "game2.h"  // Include Game 2
#include "game3.h"  // Include Game 3

// Pin Definitions
#define TFT_CS   17
#define TFT_RST  16
#define TFT_DC   15
#define ROT_A    14
#define ROT_B    13
#define ROT_BTN  12
#define EXIT_BTN 11

// TFT Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Variables
int selectedGame = 0;      // Tracks selected game in the menu
bool gameActive = false;   // Flag to indicate whether a game is running
bool exitPressed = false;  // Tracks if the Exit button is pressed

void setup() {
  // Initialize display
  tft.initR(INITR_144GREENTAB);
  tft.setRotation(1);

  // Pin Modes
  pinMode(ROT_A, INPUT_PULLUP);
  pinMode(ROT_B, INPUT_PULLUP);
  pinMode(ROT_BTN, INPUT_PULLUP);
  pinMode(EXIT_BTN, INPUT_PULLUP);

  // Show Startup Screen
  displayStartupScreen();

  // Show Home Screen
  displayHomeScreen();
}

void loop() {
  if (!gameActive) {
    // Navigate Home Screen
    navigateHomeScreen();

    // Select Game
    if (digitalRead(ROT_BTN) == LOW) {
      delay(300);  // Debounce
      gameActive = true;
      switch (selectedGame) {
        case 0:
          playGame1();  // Call Game 1
          break;
        case 1:
          playGame2();  // Call Game 2
          break;
        case 2:
          playGame3();  // Call Game 3
          break;
      }
      waitForExit();  // Wait for Exit button to return to Home Screen
    }
  }
}

void displayStartupScreen() {
  tft.fillScreen(ST7735_BLACK);
  
  tft.fillTriangle(10, 50, 10, 70, 25, 60, ST7735_YELLOW);
  tft.fillCircle  (70, 64, 10,   ST7735_RED);
  delay(1000);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_CYAN);
  tft.setCursor(10, 50);
  tft.print("LogicDash");
  delay(2000);  // Show for 3 seconds
  tft.drawRect(20, 80, 80, 5, ST7735_WHITE);
  delay(100);
  for (int i=20; i <= 80; i++) {
    tft.fillRect(20,80, i, 5, ST7735_WHITE);
    delay(100);
  }
}

void displayHomeScreen() {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(20, 10);
  tft.print("Select a Game:");

  const char* games[] = {"Game 1", "Game 2", "Game 3"};
  for (int i = 0; i < 3; i++) {
    if (i == selectedGame) {
      tft.setTextColor(ST7735_RED);
    } else {
      tft.setTextColor(ST7735_WHITE);
    }
    tft.setCursor(20, 40 + i * 20);
    tft.print(games[i]);
  }
}

void navigateHomeScreen() {
  static int lastState = HIGH;
  int currentState = digitalRead(ROT_A);
  if (currentState != lastState) {
    if (digitalRead(ROT_B) != currentState) {
      selectedGame = (selectedGame + 1) % 3;  // Navigate down
    } else {
      selectedGame = (selectedGame - 1 + 3) % 3;  // Navigate up
    }
    displayHomeScreen();  // Update Home Screen
  }
  lastState = currentState;
}

void waitForExit() {
  while (true) {
    if (digitalRead(EXIT_BTN) == LOW) {
      delay(300);  // Debounce
      gameActive = false;
      displayHomeScreen();
      break;
    }
  }
}
