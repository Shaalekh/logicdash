#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Keypad.h>

// TFT Pins
#define TFT_CS 17
#define TFT_RST 16
#define TFT_DC 15
#define TFT_MOSI  19  // SPI MOSI pin
#define TFT_SCLK  18  // SPI SCLK pin

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Keypad setup
const byte ROWS = 2;  // 2 rows
const byte COLS = 2;  // 2 columns
char keys[ROWS][COLS] = {
  {'1', '2'},
  {'3', '4'}
};
byte rowPins[ROWS] = {2, 3};  // GPIO pins for rows
byte colPins[COLS] = {4, 5};  // GPIO pins for columns

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Game variables
int score = 0;
int level = 1;

void setup() {
  Serial.begin(9600);

  // Initialize TFT display
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);

  // Enable internal pull-up resistors
  for (int i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < COLS; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }

  // Game start message
  tft.setCursor(10, 20);
  tft.println("LogicDash Ready!");
  Serial.println("Game Initialized. Press a key to start!");
}

void loop() {
  char key = keypad.getKey();

  // Only proceed if a valid key is pressed
  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    // Display key on TFT
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 40);
    tft.print("Key: ");
    tft.println(key);

    delay(500);  // Brief delay for debounce
  }
}
