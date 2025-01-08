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
#define BUS         3
// Initialize ST7735 Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Game Variables
int currentOption = 0;      // Tracks the selected option
int correctAnswer = 0;      // Correct answer for the current question
int level = 1;              // Current game level
int score = 0;              // Player's score
bool newQuestion = true;    // Flag to generate a new question
int difficulty = 0;         // Difficulty level: 1 = Easy, 2 = Medium, 3 = Hard

// Encoder State
int lastRotState = LOW;     // Previous state of the encoder
int lastButtonState = HIGH; // Previous state of the encoder button
int lastRestartState = HIGH; // Previous state of the restart button

// Function to Generate a New Pattern Question
void generatePatternQuestion(int &correct, int options[4]) {
  int sequence[5]; // Sequence of numbers

  // Generate the sequence based on difficulty level
  if (difficulty == 1) { // Easy: Arithmetic sequence
    int start = random(1, 10);
    int step = random(1, 5);
    for (int i = 0; i < 5; i++) {
      sequence[i] = start + i * step;
    }
  } else if (difficulty == 2) { // Medium: Geometric sequence
    int start = random(2, 10);
    int ratio = random(2, 5);
    for (int i = 0; i < 5; i++) {
      sequence[i] = start * pow(ratio, i);
    }
  } else if (difficulty == 3) { // Hard: Custom pattern
    int start = random(10, 100);
    int modifier = random(1, 5);
    for (int i = 0; i < 5; i++) {
      sequence[i] = start + (i * i + modifier * i);
    }
  }

  // Pick a random index to hide as the correct answer
  int missingIndex = random(0, 5);
  correct = sequence[missingIndex];
  sequence[missingIndex] = -1; // Mark as missing

  // Display the pattern with ? for the missing number
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  tft.print("Level: ");
  tft.print(level);
  tft.setCursor(10, 30);
  tft.print("Pattern:");

  for (int i = 0; i < 5; i++) {
    tft.setCursor(10 + i * 20, 50);
    if (sequence[i] == -1) {
      tft.print("?");
    } else {
      tft.print(sequence[i]);
    }
  }

  // Generate answer options
  options[0] = correct;
  for (int i = 1; i < 4; i++) {
    options[i] = correct + random(-5, 6); // Generate nearby random numbers
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
// Function to Display Options
void displayOptions(int options[4]) {
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
    tft.print(options[i]); // Print the option value
  }
}

// Function to Handle Answer Submission
void submitAnswer(int options[4]) {
  if (options[currentOption] == correctAnswer) {
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
  level++;
  if (level > 10) {
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.setCursor(20, 40);
    tft.print("Game Over!");
    tft.setCursor(10, 60);
    tft.print("Score: ");
    tft.print(score);
    while (true); // Stop game until restart
  }
  newQuestion = true; // Generate new question
}

// Function to Restart Game
void restartGame() {
  level = 1;
  score = 0;
  newQuestion = true;
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

  // Difficulty selection screen
  tft.setTextSize(1);
  tft.setCursor(10, 20);
  tft.print("Select Difficulty:");
  tft.setCursor(10, 50);
  tft.print("1) Easy");
  tft.setCursor(10, 70);
  tft.print("2) Medium");
  tft.setCursor(10, 90);
  tft.print("3) Hard");

  while (difficulty == 0) {
    if (isButtonPressed(ROT_BTN, lastButtonState)) {
      difficulty = currentOption + 1;
    }
    int currentRotState = digitalRead(ROT_A);
    if (currentRotState != lastRotState) {
      if (digitalRead(ROT_B) != currentRotState) {
        currentOption = (currentOption + 1) % 3; // Clockwise
      } else {
        currentOption = (currentOption - 1 + 3) % 3; // Counterclockwise
      }
      tft.fillScreen(ST7735_BLACK);
      tft.setCursor(10, 20);
      tft.print("Select Difficulty:");
      tft.setCursor(10, 50);
      tft.print("1) Easy");
      tft.setCursor(10, 70);
      tft.print("2) Medium");
      tft.setCursor(10, 90);
      tft.print("3) Hard");
      tft.setCursor(10 + currentOption * 40, 110);
      tft.print("^ Selected");
    }
    lastRotState = currentRotState;
  }
}

// Main Loop
void loop() {
  static int options[4];

  // Generate new question if flag is set
  if (newQuestion) {
    generatePatternQuestion(correctAnswer, options);
    displayOptions(options);
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
    displayOptions(options); // Update display
  }
  lastRotState = currentRotState;

  // Check answer button press
  if (isButtonPressed(ROT_BTN, lastButtonState)) {
    submitAnswer(options);
  }

  // Check restart button press
  if (isButtonPressed(RESTART_BTN, lastRestartState)) {
    restartGame();
  }
}
