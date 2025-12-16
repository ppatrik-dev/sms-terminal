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

// Screen size config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// SPI pins config
#define SPI_MOSI 23
#define SPI_CLK  18
#define SPI_DC   27
#define SPI_CS   5
#define SPI_RST  17

// Adafruit display object
Adafruit_SSD1306 Display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  SPI_MOSI,
  SPI_CLK,
  SPI_DC,
  SPI_RST,
  SPI_CS
);

// Display text font width
const uint8_t FONT_WIDTH = 12;
// Display text font height
const uint8_t FONT_HEIGHT = 16;

// Number of keypad cols
const uint8_t COLS = 3;
// Number of keypad rows
const uint8_t ROWS = 4;

// GPIO cols pins              C1, C2, C3
const uint8_t ColPins[COLS] = {25, 26, 13};

// GPIO rows pins              R1, R2, R3, R4
const uint8_t RowPins[ROWS] = {14, 12, 19, 16};

typedef enum {
  KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, 
  KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, 
  KEY_S, KEY_H, KEY_NONE = -1
} Key;

// Keypad keys
const Key Keypad[ROWS][COLS] = {
  {KEY_1, KEY_2, KEY_3}, // ROW1
  {KEY_4, KEY_5, KEY_6}, // ROW2
  {KEY_7, KEY_8, KEY_9}, // ROW3
  {KEY_S, KEY_0, KEY_H}  // ROW4
//  COL1   COL2   COL3
};

// Key characters
const char* KeySymbols[] = {
  " 0",     // Key 0
  ".,?!1",  // Key 1
  "abc2",   // Key 2
  "def3",   // Key 3
  "ghi4",   // Key 4
  "jkl5",   // Key 5
  "mno6",   // Key 6
  "pqrs7",  // Key 7
  "tuv8",   // Key 8
  "wxyz9"   // Key 9
};

// Upper case mode active flag 
bool upperCaseMode = false;

// Pressed key value
Key pressKey = KEY_NONE;

// Multitap index of key character
Key lastKey = KEY_NONE;

// Current key symbol index
uint8_t symbolIndex = 0;

// Key multitap delay
uint16_t multitapDelay = 1000;

// Current time
uint64_t now = 0;

// Last key press time
uint64_t lastPressTime = 0;

/**
 * @brief 
 * 
 * @return Key 
 */
Key scanKeypad() {
  for (int c = 0; c < COLS; ++c) {
    digitalWrite(ColPins[c], LOW);

    for (int r = 0; r < ROWS; ++r) {
      if (digitalRead(RowPins[r]) == LOW) {
        while(digitalRead(RowPins[r]) == LOW);

        digitalWrite(ColPins[c], HIGH);
        return Keypad[r][c];
      }
    }
    digitalWrite(ColPins[c], HIGH);
  }

  return KEY_NONE;
}

/**
 * @brief
 * 
 * @param index 
 * @return char 
 */
inline char getKeyChar(Key key) {
  if (key < KEY_0 || key > KEY_9) {
    return KEY_NONE;
  }

  return KeySymbols[key][symbolIndex];
}

/**
 * @brief 
 * 
 * @param key 
 */
void displayKey(Key key, bool isCycle) {
  char ch = getKeyChar(key);

  if (upperCaseMode && !isdigit(ch)) {
    ch = (char)toupper(ch);
  }

  if (isCycle) {
    int16_t x = Display.getCursorX();

    if (x >= FONT_WIDTH) {
      Display.setCursor(x - FONT_WIDTH, Display.getCursorY());
    }
  }

  Display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);

  Display.print(ch);
  Display.display();
}

void handleKey(Key key) {
  if (key == lastKey && (now - lastPressTime < multitapDelay)) {
    size_t symbolsLen = strlen(KeySymbols[key]);
    symbolIndex++;

    if (symbolIndex >= symbolsLen) {
      symbolIndex = 0;
    }

    displayKey(key, true);
  }
  else {
    symbolIndex = 0;
    lastKey = key;

    displayKey(key, false);
  }
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
  int16_t x = Display.getCursorX();
  int16_t y = Display.getCursorY();

  if (x >= FONT_WIDTH) {
    int16_t targetX = x - FONT_WIDTH;
    int16_t targetY = y;

    Display.fillRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, SSD1306_BLACK);

    Display.setCursor(targetX, targetY);
    Display.display();
  }
  else if (y >= FONT_HEIGHT) {
    uint8_t charsPerLine = SCREEN_WIDTH / FONT_WIDTH;
    int16_t targetX = (charsPerLine - 1) * FONT_WIDTH;
    int16_t targetY = y - FONT_HEIGHT;

    Display.fillRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, SSD1306_BLACK);
    Display.setCursor(targetX, targetY);
    Display.display();
  }
}

/**
 * @brief 
 * 
 */
void setup() {
  Serial.begin(921600);

  Display.begin(SSD1306_SWITCHCAPVCC);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(SSD1306_WHITE);
  Display.setRotation(2);
  Display.setCursor(0, 0);
  Display.display();

  // Columns as OUTPUT, set HIGH initially
  for (int c = 0; c < 3; ++c) {
    pinMode(ColPins[c], OUTPUT);
    digitalWrite(ColPins[c], HIGH);
  }

  for (int r = 0; r < 4; ++r) {
    pinMode(RowPins[r], INPUT_PULLUP);
  } 
}

/**
 * @brief 
 * 
 */
void loop() {
  now = millis();

  pressKey = scanKeypad();

  if (pressKey != KEY_NONE) {
    switch (pressKey) {
      case KEY_0: case KEY_1: 
      case KEY_2: case KEY_3: 
      case KEY_4: case KEY_5: 
      case KEY_6: case KEY_7: 
      case KEY_8: case KEY_9:
        handleKey(pressKey);
        break;

      case KEY_S:
        changeCaseMode();
        break;

      case KEY_H:
        deleteLastChar();
        break;
    }

    lastPressTime = now;
  }
}
