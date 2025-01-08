#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

// Pin Definitions
#define TFT_CS   17  // Chip select
#define TFT_RST  16  // Reset
#define TFT_DC   15  // Data/Command

#define ROT_A    14   // Rotary Encoder Pin A
#define ROT_B    13   // Rotary Encoder Pin B
#define ROT_BTN  12   // Rotary Encoder Button
#define RESTART_BTN 9 // Restart button pin
#define BUS      3    // Tunes on correct answer

// Initialize ST7735 Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Game Variables
int currentOption = 0;      // Tracks the selected option
int correctAnswer = 0;      // Correct answer for the current question
int questionCount = 0;      // Tracks total questions asked
int score = 0;              // Player's score
bool newQuestion = true;    // Flag to generate a new question
bool gameActive = false;    // Tracks whether the game is active
int difficulty = 0;         // Difficulty level (0: Easy, 1: Medium, 2: Hard)

// Encoder State
int lastRotState = LOW;     // Previous state of the encoder
int lastButtonState = HIGH; // Previous state of the encoder button
int lastRestartState = HIGH; // Previous state of the restart button

// Function to Generate a New Question Based on Difficulty
void generateQuestion(int &a, int &b, int &correct, int options[4], int difficulty) {
  int rangeStart, rangeEnd;
  if (difficulty == 0) { // Easy
    rangeStart = 1;
    rangeEnd = 9;
  } else if (difficulty == 1) { // Medium
    rangeStart = 10;
    rangeEnd = 99;
  } else { // Hard
    rangeStart = 100;
    rangeEnd = 999;
  }

  a = random(rangeStart, rangeEnd + 1);
  b = random(rangeStart, rangeEnd + 1);
  correct = a + b;

  options[0] = correct; // Add correct answer to options
  for (int i = 1; i < 4; i++) {
    options[i] = correct + random(-10, 11); // Generate nearby random numbers
    if (options[i] == correct) options[i] += 1; // Ensure no duplicate correct answer
  }

  // Shuffle options
  for (int i = 3; i > 0; i--) {
    int j = random(0, i + 1);
    int temp = options[i];
    options[i] = options[j];
    options[j] = temp;
  }
}

// Function to Display the Main Menu
void displayMenu() {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  tft.setCursor(10, 20);
  tft.print("Select Difficulty:");

  const char *options[] = {"Easy", "Medium", "Difficult"};
  for (int i = 0; i < 3; i++) {
    if (i == currentOption) {
      tft.setTextColor(ST7735_RED);
    } else {
      tft.setTextColor(ST7735_WHITE);
    }
    tft.setCursor(20, 60 + i * 20);
    tft.print(options[i]);
  }
}

// Function to Display a Question
void displayQuestion(int a, int b, int options[4]) {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  tft.setCursor(10, 10);
  tft.print("Q: ");
  tft.print(questionCount);
  tft.print("/25");

  tft.setCursor(10, 40);
  tft.print(a);
  tft.print(" + ");
  tft.print(b);
  tft.print(" = ?");

  int optionX[2] = {20, 80};
  int optionY[2] = {80, 110};
  char labels[4] = {'A', 'B', 'C', 'D'};

  for (int i = 0; i < 4; i++) {
    int col = i % 2;
    int row = i / 2;

    if (i == currentOption) {
      tft.setTextColor(ST7735_RED);
    } else {
      tft.setTextColor(ST7735_WHITE);
    }

    tft.setCursor(optionX[col], optionY[row]);
    tft.print(labels[i]);
    tft.print(") ");
    tft.print(options[i]);
  }
}

// Function to Check Button Press
bool isButtonPressed(int pin, int &lastState) {
  int buttonState = digitalRead(pin);
  if (buttonState == LOW && lastState == HIGH) {
    delay(50); // Debounce
    lastState = buttonState;
    return true;
  }
  lastState = buttonState;
  return false;
}

// Function to Restart the Game
void restartGame() {
  gameActive = false;
  questionCount = 0;
  score = 0;
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 60);
  tft.print("Game Restarted");
  delay(1000);
}

void setup() {
  tft.initR(INITR_144GREENTAB);   // Initialize the display
  delay(100);                     // Small delay to ensure display is properly initialized

  tft.setRotation(1);             // Set the display rotation

  // Pin Modes
  pinMode(ROT_A, INPUT_PULLUP);
  pinMode(ROT_B, INPUT_PULLUP);
  pinMode(ROT_BTN, INPUT_PULLUP);
  pinMode(RESTART_BTN, INPUT_PULLUP);
  pinMode(BUS, OUTPUT);

  randomSeed(analogRead(0));      // Initialize random seed

  tft.fillScreen(ST7735_BLACK);   // Clear the screen
}


// Main Loop
void loop() {
  if (!gameActive && questionCount == 0) {
    displayMenu();
    if (isButtonPressed(ROT_BTN, lastButtonState)) {
      difficulty = currentOption;
      gameActive = true;
    } else {
      int currentRotState = digitalRead(ROT_A);
      if (currentRotState != lastRotState) {
        if (digitalRead(ROT_B) != currentRotState) {
          currentOption = (currentOption + 1) % 3;
        } else {
          currentOption = (currentOption - 1 + 3) % 3;
        }
        displayMenu();
      }
      lastRotState = currentRotState;
    }
  } else if (gameActive) {
    static int a, b;
    static int options[4];

    if (newQuestion) {
      generateQuestion(a, b, correctAnswer, options, difficulty);
      displayQuestion(a, b, options);
      questionCount++;
      newQuestion = false;
    }

    if (isButtonPressed(ROT_BTN, lastButtonState)) {
      if (options[currentOption] == correctAnswer) {
        score++;
        tft.fillScreen(ST7735_GREEN);
        tft.setCursor(23, 50);
        tft.setTextColor(ST7735_WHITE);
        tft.setTextSize(2);
        tft.print("Correct!");
        digitalWrite(BUS, HIGH);
        delay(200);
        digitalWrite(BUS, LOW);
      } else {
        tft.fillScreen(ST7735_RED);
        tft.setTextColor(ST7735_WHITE);
        tft.setCursor(30, 50);
        tft.setTextSize(2);
        tft.print("Wrong!");
      }
      delay(1000);
      newQuestion = questionCount < 25;
      if (!newQuestion) {
        gameActive = false;
        tft.fillScreen(ST7735_BLACK);
        tft.setCursor(35, 40);
        tft.setTextSize(1);
        tft.print("Game Over!");
        tft.setCursor(20, 60);
        tft.print("Score: ");
        tft.print(score);
      }
    }

    int currentRotState = digitalRead(ROT_A);
    if (currentRotState != lastRotState) {
      if (digitalRead(ROT_B) != currentRotState) {
        currentOption = (currentOption + 1) % 4;
      } else {
        currentOption = (currentOption - 1 + 4) % 4;
      }
      displayQuestion(a, b, options);
    }
    lastRotState = currentRotState;
  }

  if (isButtonPressed(RESTART_BTN, lastRestartState)) {
    restartGame();
  }
}
