/**
 * @file Keypad.cpp
 * @author Patrik Prochazka (xprochp00@stud.fit.vutbr.cz)
 * @brief 
 * @version 0.1
 * @date 2025-12-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <Arduino.h>

#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "Keypad.h"
#include "Display.h"

extern uint64_t now;

// GPIO columns pins                  C1, C2, C3
const uint8_t ColPins[KEYPAD_COLS] = {25, 26, 13};

// GPIO rows pins                     R1, R2, R3, R4
const uint8_t RowPins[KEYPAD_ROWS] = {14, 12, 19, 16};

// Keypad keys
const Key Keypad[KEYPAD_ROWS][KEYPAD_COLS] = {
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

// Multitap index of key character
Key lastKey = KEY_NONE;

// Current key symbol index
uint8_t symbolIndex = 0;

// Last key press time
uint64_t lastPressTime = 0;

// Last delete time
uint64_t lastDeleteTime = 0;

void initKeypad() {
  // Columns as OUTPUT, set HIGH initially
  for (int c = 0; c < 3; ++c) {
    pinMode(ColPins[c], OUTPUT);
    digitalWrite(ColPins[c], HIGH);
  }

  // Rows as INPUT_PULLUP resistors
  for (int r = 0; r < 4; ++r) {
    pinMode(RowPins[r], INPUT_PULLUP);
  } 
}

Key scanKeypad() {
  uint64_t currentMillis = now;

  if (currentMillis - lastPressTime >= MULTITAP_DELAY) {
    enableCursor();
  }

  for (int c = 0; c < KEYPAD_COLS; ++c) {
    digitalWrite(ColPins[c], LOW);

    for (int r = 0; r < KEYPAD_ROWS; ++r) {
      if (digitalRead(RowPins[r]) == LOW) {
        bool longPressTriggered = false;
        uint64_t pressStartTime = millis();

        while(digitalRead(RowPins[r]) == LOW) {
          uint64_t loopTime = millis();

          if (loopTime - pressStartTime > LONG_PRESS_DELAY) {
            handleLongPress(Keypad[r][c], loopTime);
            longPressTriggered = true;
          }

          delay(10);
        }

        digitalWrite(ColPins[c], HIGH);

        if (longPressTriggered) {
          return KEY_NONE;
        }
        else {
          return Keypad[r][c];
        }
      }
    }
    digitalWrite(ColPins[c], HIGH);
  }
  
  return KEY_NONE;
}

char getKeyChar(Key key) {
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

  drawChar(ch, isCycle);
}

void handleKey(Key key) {
  if (key == lastKey && (now - lastPressTime < MULTITAP_DELAY)) {
    disableCursor();

    size_t symbolsLen = strlen(KeySymbols[key]);
    symbolIndex++;

    if (symbolIndex >= symbolsLen) {
      symbolIndex = 0;
    }

    displayKey(key, true);
  }
  else {
    symbolIndex = 0;
    displayKey(key, false);
    lastKey = key;
  }

  lastPressTime = now;
}

void changeCaseMode() {
  upperCaseMode = !upperCaseMode;
}

void handleDelete(uint64_t time) {
  deleteChar(time);

  lastKey = KEY_NONE;
  symbolIndex = 0;

  lastDeleteTime = time;
}

void handleLongPress(Key key, uint64_t currentLoopTime) {
  switch (key) {
    // Move top
    case KEY_2:
      moveUp(currentLoopTime);
      break;

    // Move left
    case KEY_4:
      moveLeft(currentLoopTime);
      break;

    // Move right
    case KEY_6:
      moveRight(currentLoopTime);
      break;

    // Move down
    case KEY_8:
      moveDown(currentLoopTime);
      break;

    case KEY_H:
      if (currentLoopTime - lastDeleteTime > DELETE_SPEED_DELAY) {
        handleDelete(currentLoopTime);
      }
      break;
    
    default:
      break;
    }
}