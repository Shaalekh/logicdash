#ifndef GAME1_H
#define GAME1_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Pin Definitions
#define ROT_A    14   // Rotary Encoder Pin A
#define ROT_B    13   // Rotary Encoder Pin B
#define ROT_BTN  12   // Rotary Encoder Button
#define RESTART_BTN 9 // Restart button pin
#define BUS      3    // Tunes on correct answer

// Game Variables
int currentOption = 0;      // Tracks the selected option
int correctAnswer = 0;      // Correct answer for the current question
int questionCount = 0;      // Tracks total questions asked
int score = 0;              // Player's score
bool newQuestion = true;    // Flag to generate a new question
bool gameActive = false;    // Tracks whether the game is active
int selectedOperation = 0;  // Selected operation (0: Addition, 1: Subtraction, 2: Multiplication, 3: Division)

int lastRotState = LOW;     // Previous state of the encoder
int lastButtonState = HIGH; // Previous state of the encoder button
int lastRestartState = HIGH; // Previous state of the restart button

// Function to generate a new question
void generateQuestion(int &a, int &b, int &correct, int options[4], int selectedOperation);

// Function to display menu
void displayMenu(Adafruit_ST7735 &tft);

// Function to display question
void displayQuestion(Adafruit_ST7735 &tft, int a, int b, int options[4]);

// Function to check button press
bool isButtonPressed(int pin, int &lastState);

// Function to restart game
void restartGame(Adafruit_ST7735 &tft);

// Main Game Function
void runGame1(Adafruit_ST7735 &tft, int exitButtonPin);

#endif
