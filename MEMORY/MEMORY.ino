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
int currentOption = 0;      // Tracks the selected option
int currentQuestion = 0;    // Tracks the current question number
int score = 0;              // Player's score
bool newQuestion = true;    // Flag to generate a new question
bool gameActive = true;     // Tracks whether the game is active

// Encoder and Restart Button States
int lastRotState = LOW;     // Previous state of the encoder
int lastButtonState = HIGH; // Previous state of the encoder button
int lastRestartState = HIGH; // Previous state of the restart button

// Structure for Questions
struct Question {
  String series;
  int correctAnswer; // Correct answer index (0 = A, 1 = B, etc.)
  int options[4];    // Four options
};

// Array of Questions
Question questions[25];

// Function to Generate a Question
void generateQuestion(Question &q) {
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

// Function to Display Question
void displayQuestion(Question &q) {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  // Display the Question Number and \n Series
  tft.setCursor(10, 10);
  tft.print("Q");
  tft.print(currentQuestion + 1);
  tft.print(": Complete the \n series");

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

// Function to Restart Game
void restartGame() {
  currentQuestion = 0;
  score = 0;
  newQuestion = true;
  gameActive = true;
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 60);
  tft.print("Game Restarted");
  delay(1000);
}

// Setup Function
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

  // Generate all questions
  for (int i = 0; i < 25; i++) {
    generateQuestion(questions[i]);
  }
}

// Main Loop
void loop() {
  // Check restart button press
  if (isButtonPressed(RESTART_BTN, lastRestartState)) {
    restartGame();
  }

  if (gameActive) {
    if (newQuestion) {
      displayQuestion(questions[currentQuestion]);
      newQuestion = false;
    }

    // Check rotary encoder rotation
    int currentRotState = digitalRead(ROT_A);
    if (currentRotState != lastRotState) {
      if (digitalRead(ROT_B) != currentRotState) {
        currentOption = (currentOption + 1) % 4; // Clockwise
      } else {
        currentOption = (currentOption - 1 + 4) % 4; // Counterclockwise
      }
      displayQuestion(questions[currentQuestion]); // Update display
    }
    lastRotState = currentRotState;

    // Check answer button press
    if (isButtonPressed(ROT_BTN, lastButtonState)) {
      if (currentOption == questions[currentQuestion].correctAnswer) {
        score++;
        tft.fillScreen(ST7735_GREEN);
        tft.setTextColor(ST7735_WHITE);
        tft.setTextSize(2);
        tft.setCursor(20, 50);
        tft.print("Correct!");
        digitalWrite(BUS, HIGH);
        delay(200);
        digitalWrite(BUS, LOW);
      } else {
        tft.fillScreen(ST7735_RED);
        tft.setTextColor(ST7735_WHITE);
        tft.setTextSize(2);
        tft.setCursor(25, 50);
        tft.print("Wrong!");
      }
      delay(1000); // Pause before next question

      currentQuestion++;
      if (currentQuestion >= 25) {
        tft.fillScreen(ST7735_BLACK);
        tft.setTextColor(ST7735_WHITE);
        tft.setTextSize(1);
        tft.setCursor(20, 40);
        tft.print("Game Over!");
        tft.setCursor(10, 60);
        tft.print("Score: ");
        tft.print(score);
        gameActive = false; // Deactivate game state
      } else {
        newQuestion = true;
      }
    }
  }
}
