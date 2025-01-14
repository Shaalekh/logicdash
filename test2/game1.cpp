#include "game1.h"

void generateQuestion(int &a, int &b, int &correct, int options[4], int selectedOperation) {
  a = random(1, 100);
  b = random(1, 100);
  if (selectedOperation == 3 && b == 0) b = 1; // Avoid division by zero

  switch (selectedOperation) {
    case 0: correct = a + b; break;
    case 1: correct = a - b; break;
    case 2: correct = a * b; break;
    case 3: correct = a / b; a = correct * b; break;
  }

  options[0] = correct;
  for (int i = 1; i < 4; i++) {
    options[i] = correct + random(-10, 11);
    if (options[i] == correct) options[i] += 1;
  }

  // Shuffle options
  for (int i = 3; i > 0; i--) {
    int j = random(0, i + 1);
    int temp = options[i];
    options[i] = options[j];
    options[j] = temp;
  }
}

void displayMenu(Adafruit_ST7735 &tft) {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  tft.setCursor(10, 20);
  tft.print("Select Operation:");

  const char *operations[] = {"Addition", "Subtraction", "Multiplication", "Division"};
  for (int i = 0; i < 4; i++) {
    tft.setTextColor(i == currentOption ? ST7735_RED : ST7735_WHITE);
    tft.setCursor(20, 60 + i * 20);
    tft.print(operations[i]);
  }
}

void displayQuestion(Adafruit_ST7735 &tft, int a, int b, int options[4]) {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  tft.setCursor(10, 10);
  tft.print("Q: ");
  tft.print(questionCount);
  tft.print("/25");

  tft.setCursor(10, 40);
  const char *operations[] = {"+", "-", "x", "/"};
  tft.print(a);
  tft.print(" ");
  tft.print(operations[selectedOperation]);
  tft.print(" ");
  tft.print(b);
  tft.print(" = ?");

  int optionX[2] = {20, 80};
  int optionY[2] = {80, 110};
  char labels[4] = {'A', 'B', 'C', 'D'};

  for (int i = 0; i < 4; i++) {
    tft.setTextColor(i == currentOption ? ST7735_RED : ST7735_WHITE);
    int col = i % 2;
    int row = i / 2;
    tft.setCursor(optionX[col], optionY[row]);
    tft.print(labels[i]);
    tft.print(") ");
    tft.print(options[i]);
  }
}

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

void restartGame(Adafruit_ST7735 &tft) {
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

void runGame1(Adafruit_ST7735 &tft, int exitButtonPin) {
  static int a, b, options[4];

  if (!gameActive && questionCount == 0) {
    displayMenu(tft);
    if (isButtonPressed(ROT_BTN, lastButtonState)) {
      selectedOperation = currentOption;
      gameActive = true;
    } else {
      int currentRotState = digitalRead(ROT_A);
      if (currentRotState != lastRotState) {
        currentOption = (digitalRead(ROT_B) != currentRotState) 
                        ? (currentOption + 1) % 4 
                        : (currentOption - 1 + 4) % 4;
        displayMenu(tft);
      }
      lastRotState = currentRotState;
    }
  } else if (gameActive) {
    if (newQuestion) {
      generateQuestion(a, b, correctAnswer, options, selectedOperation);
      displayQuestion(tft, a, b, options);
      questionCount++;
      newQuestion = false;
    }

    if (isButtonPressed(ROT_BTN, lastButtonState)) {
      if (options[currentOption] == correctAnswer) score++;
      newQuestion = questionCount < 25;
      if (!newQuestion) gameActive = false;
    }

    if (digitalRead(exitButtonPin) == LOW) return;

    int currentRotState = digitalRead(ROT_A);
    if (currentRotState != lastRotState) {
      currentOption = (digitalRead(ROT_B) != currentRotState) 
                      ? (currentOption + 1) % 4 
                      : (currentOption - 1 + 4) % 4;
      displayQuestion(tft, a, b, options);
    }
    lastRotState = currentRotState;
  }
}
