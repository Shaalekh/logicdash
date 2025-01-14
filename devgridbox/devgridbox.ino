#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Define TFT Pins
#define TFT_CS    17
#define TFT_RST   16
#define TFT_DC    15

// Define Rotary Encoder Pins
#define ENCODER_CLK 14
#define ENCODER_DT  13
#define ENCODER_SW  12

// Initialize TFT
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Rotary Encoder Variables
volatile int encoderValue = 0;
int lastEncoderValue = 0;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
bool buttonPressed = false;

// Game Variables
int currentQuestion = 0;
int currentSelection = 0;
int score = 0;
const int totalQuestions = 2; // Adjust for the number of questions
bool selections[9] = {false, false, false, false, false, false, false, false, false}; // Track selected boxes

// Questions and Answers
struct Question {
  String prompt;
  int grid[9];
  int correctAnswers[9]; // 1 for correct, 0 for incorrect
  int correctCount;      // Number of correct answers
};

Question questions[totalQuestions] = {
  {"Select multiples of 5", {5, 12, 15, 8, 20, 7, 25, 2, 10}, {1, 0, 1, 0, 1, 0, 1, 0, 1}, 5},
  {"Select even numbers", {3, 8, 5, 4, 12, 9, 14, 11, 10}, {0, 1, 0, 1, 1, 0, 1, 0, 1}, 5}
};

// Interrupt Service Routine for Rotary Encoder
void handleEncoder() {
  static uint8_t lastState;
  uint8_t state = digitalRead(ENCODER_CLK);
  if (state != lastState) {
    if (digitalRead(ENCODER_DT) != state) {
      encoderValue++;
    } else {
      encoderValue--;
    }
  }
  lastState = state;
}

void handleButton() {
  int reading = digitalRead(ENCODER_SW);

  // Debounce the button press
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && lastButtonState == HIGH) {
      buttonPressed = true;
    }
    lastDebounceTime = millis();
  }

  lastButtonState = reading;
}

// Setup
void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  // Initialize TFT Display
  tft.initR(INITR_BLACKTAB); // For 1.44 TFT
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);

  // Initialize Rotary Encoder
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), handleButton, CHANGE);

  // Display the first question
  displayQuestion();
}

// Display Current Question and Grid
void displayQuestion() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.println(questions[currentQuestion].prompt);

  // Draw the grid
  for (int i = 0; i < 9; i++) {
    int x = (i % 3) * 31; // X position in the grid (1px spacing)
    int y = (i / 3) * 31 + 20; // Y position in the grid (1px spacing)
    uint16_t color = ST77XX_WHITE;

    if (selections[i]) {
      color = ST77XX_GREEN; // Highlight selected boxes
    } else if (i == currentSelection) {
      color = ST77XX_BLUE; // Highlight current selection
    }

    tft.fillRect(x, y, 30, 30, color); // Draw 30x30 grid box
    tft.setCursor(x + 10, y + 10);
    tft.setTextColor((color == ST77XX_WHITE) ? ST77XX_BLACK : ST77XX_WHITE);
    tft.print(questions[currentQuestion].grid[i]);
  }
}

// Check Answer
void checkAnswer() {
  int correctSelected = 0;
  int incorrectSelected = 0;

  // Evaluate the selections
  for (int i = 0; i < 9; i++) {
    if (selections[i]) {
      if (questions[currentQuestion].correctAnswers[i] == 1) {
        correctSelected++;
      } else {
        incorrectSelected++;
      }
    }
  }

  // Check if the number of correct answers matches
  if (correctSelected == questions[currentQuestion].correctCount && incorrectSelected == 0) {
    score++;
    tft.fillScreen(ST77XX_GREEN);
    tft.setCursor(20, 60);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Correct!");
  } else {
    tft.fillScreen(ST77XX_RED);
    tft.setCursor(20, 60);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Wrong!");
  }

  delay(2000); // Pause to show feedback
  currentQuestion++;

  // Reset for the next question or end the game
  if (currentQuestion >= totalQuestions) {
    displayScore();
  } else {
    memset(selections, false, sizeof(selections)); // Reset selections
    displayQuestion();
  }
}

// Display Final Score
void displayScore() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(20, 40);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("Game Over!");

  tft.setCursor(20, 80);
  tft.setTextSize(1);
  tft.print("Score: ");
  tft.print(score);
  tft.println(" / ");
  tft.print(totalQuestions);

  while (true) {
    delay(100);
  }
}

// Main Loop
void loop() {
  // Update grid selection based on encoder value
  if (encoderValue != lastEncoderValue) {
    currentSelection = (encoderValue + 9) % 9; // Ensure selection stays in range 0-8
    displayQuestion();
    lastEncoderValue = encoderValue;
  }

  // Handle button press for toggling selection
  if (buttonPressed) {
    buttonPressed = false;
    selections[currentSelection] = !selections[currentSelection]; // Toggle selection
    displayQuestion();
  }
}
