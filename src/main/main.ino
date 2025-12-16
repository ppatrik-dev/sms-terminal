/**
 * @file main.ino
 * @author Patrik Prochazka (xprochp00@stud.fit.vutbr.cz)
 * @brief 
 * @version 0.1
 * @date 2025-12-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

// Convert character to integer value
#define CHAR_TO_INT(ch) ((ch) - '0')

// SPI screen config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// SPI pins config
#define OLED_MOSI 23
#define OLED_CLK  18
#define OLED_DC   27
#define OLED_CS   5
#define OLED_RST  17

// Adafruit display object
Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  OLED_MOSI,
  OLED_CLK,
  OLED_DC,
  OLED_RST,
  OLED_CS
);

// Display text font width
const uint8_t FONT_WIDTH = 12;
// Display text font height
const uint8_t FONT_HEIGHT = 16;

// Number of keypad cols
const uint8_t COLS = 3;
// Number of keypad rows
const uint8_t ROWS = 4;

// GPIO cols pins        C1, C2, C3
uint8_t colPins[COLS] = {25, 26, 13};

// GPIO rows pins        R1, R2, R3, R4
uint8_t rowPins[ROWS] = {14, 12, 19, 16};

// Keypad symbols
char keypad[ROWS][COLS] = {
  {'1','2','3'}, // R1
  {'4','5','6'}, // R2
  {'7','8','9'}, // R3
  {'*','0','#'}  // R4
//  C1, C2, C3
};

// Key characters
const char* keyChars[10] = {
  " 0",     // Key 0
  ".,?!1",  // Key 1
  "ABC2",   // Key 2
  "DEF3",   // Key 3
  "GHI4",   // Key 4
  "JKL5",   // Key 5
  "MNO6",   // Key 6
  "PQRS7",  // Key 7
  "TUV8",   // Key 8
  "WXYZ9"   // Key 9
};

// Upper case mode active flag 
bool upperCaseMode = false;

// Multitap index of key character
int multitapIndex = 0;

/**
 * @brief 
 * 
 * @return char 
 */
char scanKeypad() {
  for (int c = 0; c < 3; c++) {
    digitalWrite(colPins[c], LOW);
    for (int r = 0; r < 4; r++) {
      if (digitalRead(rowPins[r]) == LOW) {
        while(digitalRead(rowPins[r]) == LOW);
        digitalWrite(colPins[c], HIGH);
        return keypad[r][c];
      }
    }
    digitalWrite(colPins[c], HIGH);
  }

  return '\0';
}

/**
 * @brief
 * 
 * @param index 
 * @return char 
 */
char getKeyChar(int index) {
  const char *chars = keyChars[index];

  if (multitapIndex < strlen(chars)) {
    return chars[multitapIndex];
  }

  return '\0';
}

/**
 * @brief 
 * 
 * @param key 
 */
void displayKeyChar(const char key) {
  char letter = getKeyChar(CHAR_TO_INT(key));

  if (!upperCaseMode) {
    letter = (char)tolower(letter);
  }

  display.print(letter);
  display.display();
}

/**
 * @brief 
 * 
 */
inline void changeCaseMode() {
  upperCaseMode = !upperCaseMode;
}

/**
 * @brief 
 * 
 */
void deleteLastChar() {
  int x = display.getCursorX();
  int y = display.getCursorY();

  if (x >= FONT_WIDTH) {
    int targetX = x - FONT_WIDTH;
    int targetY = y;

    display.fillRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, SSD1306_BLACK);

    display.setCursor(targetX, targetY);
    display.display();
  }
  else if (y >= FONT_HEIGHT) {
    int charsPerLine = SCREEN_WIDTH / FONT_WIDTH;
    int targetX = (charsPerLine - 1) * FONT_WIDTH;
    int targetY = y - FONT_HEIGHT;

    display.fillRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, SSD1306_BLACK);
    display.setCursor(targetX, targetY);
    display.display();
  }
}

/**
 * @brief 
 * 
 */
void setup() {
  Serial.begin(921600);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setRotation(2);
  display.setCursor(0, 0);
  display.display();

  // Columns as OUTPUT, set HIGH initially
  for (int c = 0; c < 3; c++) {
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c], HIGH);
  }

  for (int r = 0; r < 4; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  } 
}

/**
 * @brief 
 * 
 */
void loop() {
  char key = scanKeypad();

  switch (key) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      displayKeyChar(key);
      break;

    case '*':
      changeCaseMode();
      break;

    case '#':
      deleteLastChar();
      break;

    default:
      break;
  }
}
