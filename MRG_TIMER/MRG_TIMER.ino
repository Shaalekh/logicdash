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
int level = 1;              // Current game level
int score = 0;              // Player's score
bool newQuestion = true;    // Flag to generate a new question
bool gameActive = true;     // Tracks whether the game is active
int questionCount = 0;      // Tracks number of questions answered
unsigned long startTime;    // Timer start time
int timeLimit = 180;        // 3-minute timer (180 seconds)

// Encoder State
int lastRotState = LOW;     // Previous state of the encoder
int lastButtonState = HIGH; // Previous state of the encoder button
int lastRestartState = HIGH; // Previous state of the restart button

// Function to Generate a New Question
void generateQuestion(int &a, int &b, int &correct, int options[4]) {
  int maxNumber = 9 * level; // Increase difficulty by increasing the numbers as level progresses
  if (level > 10) maxNumber = 99;  // 2-digit max after level 10
  if (level > 20) maxNumber = 999; // 3-digit max after level 20
  
  a = random(1, maxNumber); // Random number within the level difficulty
  b = random(1, maxNumber); // Random number within the level difficulty
  correct = a + b;          // Correct answer
  options[0] = correct;     // Add correct answer to options
  
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

// Function to Display Question
void displayQuestion(int a, int b, int options[4], int timeLeft) {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  // Display the Level and Question
  tft.setCursor(10, 10);
  tft.print("Level: ");
  tft.print(level);

  // Display the timer at the top-right corner
  tft.setCursor(100, 10);
  tft.print("Time: ");
  tft.print(timeLeft);

  tft.setCursor(10, 40);
  tft.print(a);
  tft.print(" + ");
  tft.print(b);
  tft.print(" = ?");

  // Display options in 2x2 grid
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
  level = 1;
  score = 0;
  questionCount = 0;
  newQuestion = true;
  gameActive = true; // Reactivate game state
  startTime = millis(); // Reset timer
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
  startTime = millis(); // Start timer
}

// Main Loop
void loop() {
  // Check restart button press
  if (isButtonPressed(RESTART_BTN, lastRestartState)) {
    restartGame();
  }

  if (gameActive) {
    static int a, b;
    static int options[4];

    // Calculate time left
    int timeElapsed = (millis() - startTime) / 1000;  // Time elapsed in seconds
    int timeLeft = timeLimit - timeElapsed;  // Time remaining

    if (timeLeft <= 0) {
      gameActive = false;
      tft.fillScreen(ST7735_BLACK);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(1);
      tft.setCursor(20, 40);
      tft.print("Game Over!");
      tft.setCursor(10, 60);
      tft.print("Score: ");
      tft.print(score);
      delay(3000); // Display the result for 3 seconds
    } else {
      // Generate new question if flag is set
      if (newQuestion) {
        generateQuestion(a, b, correctAnswer, options);
        displayQuestion(a, b, options, timeLeft);
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
        displayQuestion(a, b, options, timeLeft); // Update display
      }
      lastRotState = currentRotState;

      // Check answer button press
      if (isButtonPressed(ROT_BTN, lastButtonState)) {
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
        questionCount++;
        if (questionCount >= 25) {
          gameActive = false; // End the game after 25 questions
        } else {
          newQuestion = true; // Generate new question
        }
      }
    }
  }
}
