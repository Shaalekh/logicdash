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
#define RESTART_BTN 9 // Restart button
#define BUS      3    // Buzzer for feedback

// Initialize ST7735 Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Game Variables
enum GameState { MODE_SELECTION, QUESTION_ASKED, GAME_OVER };
GameState gameState = MODE_SELECTION;

enum Mode { NUMBERS, ALPHABETS };
Mode gameMode = NUMBERS;

int currentOption = 0;      // Tracks the selected option
int currentQuestion = 0;    // Tracks the current question number
int score = 0;              // Player's score
bool newQuestion = true;    // Flag to generate a new question

// Encoder and Button States
int lastRotState = LOW;     // Previous state of the encoder
int lastButtonState = HIGH; // Previous state of the encoder button
int lastRestartState = HIGH; // Previous state of the restart button

// Structure for Questions
struct Question {
  String series;
  int correctAnswer; // Correct answer index (0 = A, 1 = B, etc.)
  int options[4];    // Four options
};

// Current Question
Question currentQ;

// Function to Generate Number-Based Question
void generateNumberQuestion(Question &q) {
  int start = random(1, 5) * 2;   // Random start for series
  int step = random(1, 4);        // Random step size
  int length = 4;                 // Series length
  int missingIndex = random(0, length); // Index of the missing element

  // Generate the series
  q.series = "";
  for (int i = 0; i < length; i++) {
    if (i == missingIndex) {
      q.series += "_";
    } else {
      q.series += String(start + i * step);
    }
    if (i < length - 1) q.series += ", ";
  }

  // Correct answer
  q.correctAnswer = random(0, 4);
  q.options[q.correctAnswer] = start + missingIndex * step;

  // Generate random incorrect options
  for (int i = 0; i < 4; i++) {
    if (i == q.correctAnswer) continue;
    q.options[i] = q.options[q.correctAnswer] + random(-5, 6);
    while (q.options[i] == q.options[q.correctAnswer]) {
      q.options[i] = q.options[q.correctAnswer] + random(-5, 6);
    }
  }
}

// Function to Generate Alphabet-Based Question
void generateAlphabetQuestion(Question &q) {
  char start = 'A' + random(0, 10);   // Random start for series
  int step = random(1, 3);           // Random step size
  int length = 4;                    // Series length
  int missingIndex = random(0, length); // Index of the missing element

  // Generate the series
  q.series = "";
  for (int i = 0; i < length; i++) {
    if (i == missingIndex) {
      q.series += "_";
    } else {
      q.series += char(start + i * step);
    }
    if (i < length - 1) q.series += ", ";
  }

  // Correct answer
  q.correctAnswer = random(0, 4);
  q.options[q.correctAnswer] = start + missingIndex * step;

  // Generate random incorrect options
  for (int i = 0; i < 4; i++) {
    if (i == q.correctAnswer) continue;
    q.options[i] = q.options[q.correctAnswer] + random(-3, 4);
    while (q.options[i] == q.options[q.correctAnswer]) {
      q.options[i] = q.options[q.correctAnswer] + random(-3, 4);
    }
  }
}

// Function to Display Question
void displayQuestion(Question &q) {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  // Display the Question Number and Series
  tft.setCursor(10, 10);
  tft.print("Q");
  tft.print(currentQuestion + 1);
  tft.print(": Complete the \nseries");

  tft.setCursor(10, 40);
  tft.print(q.series);

  // Display Options in 2x2 Matrix
  int optionX[2] = {20, 80}; // X-coordinates for the two columns
  int optionY[2] = {80, 110}; // Y-coordinates for the two rows
  char labels[4] = {'A', 'B', 'C', 'D'}; // Option labels

  for (int i = 0; i < 4; i++) {
    int col = i % 2; // Column index (0 or 1)
    int row = i / 2; // Row index (0 or 1)

    if (i == currentOption) {
      tft.setTextColor(ST7735_RED); // Highlight selected option
    } else {
      tft.setTextColor(ST7735_WHITE);
    }

    tft.setCursor(optionX[col], optionY[row]);
    tft.print(labels[i]); // Print the label (A, B, C, D)
    tft.print(") ");
    tft.print(q.options[i]); // Print the option value
  }
}

// Function to Display Mode Selection
void displayModeSelection() {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);

  tft.setCursor(10, 20);
  tft.print("Select Mode:");

  tft.setTextSize(1);
  if (currentOption == 0) {
    tft.setTextColor(ST7735_RED);
  } else {
    tft.setTextColor(ST7735_WHITE);
  }
  tft.setCursor(30, 60);
  tft.print("1) Numbers");

  if (currentOption == 1) {
    tft.setTextColor(ST7735_RED);
  } else {
    tft.setTextColor(ST7735_WHITE);
  }
  tft.setCursor(30, 80);
  tft.print("2) Alphabets");
}

// Main Setup Function
void setup() {
  tft.initR(INITR_144GREENTAB); // Initialize display
  tft.setRotation(1);

  pinMode(ROT_A, INPUT_PULLUP);
  pinMode(ROT_B, INPUT_PULLUP);
  pinMode(ROT_BTN, INPUT_PULLUP);
  pinMode(RESTART_BTN, INPUT_PULLUP);
  pinMode(BUS, OUTPUT);

  randomSeed(analogRead(0)); // Seed random number generator
  tft.fillScreen(ST7735_BLACK);
}

// Main Loop
void loop() {
  if (gameState == MODE_SELECTION) {
    displayModeSelection();

    int currentRotState = digitalRead(ROT_A);
    if (currentRotState != lastRotState) {
      if (digitalRead(ROT_B) != currentRotState) {
        currentOption = (currentOption + 1) % 2;
      } else {
        currentOption = (currentOption - 1 + 2) % 2;
      }
      displayModeSelection();
    }
    lastRotState = currentRotState;

    if (digitalRead(ROT_BTN) == LOW) {
      gameMode = (currentOption == 0) ? NUMBERS : ALPHABETS;
      gameState = QUESTION_ASKED;
      newQuestion = true;
    }
  } else if (gameState == QUESTION_ASKED && newQuestion) {
    if (gameMode == NUMBERS) {
      generateNumberQuestion(currentQ);
    } else {
      generateAlphabetQuestion(currentQ);
    }
    displayQuestion(currentQ);
    newQuestion = false;
  }
}
