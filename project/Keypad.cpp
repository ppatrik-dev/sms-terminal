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
#include "Buffer.h"

extern uint64_t now;
extern uint8_t bufferIndex;

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

// Key symbols
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
CaseMode activeCaseMode = MODE_SMART;

// Multitap index of key character
Key lastKey = KEY_NONE;

// Current key symbol index
uint8_t symbolIndex = 0;

// Last key press time
uint64_t lastPressTime = 0;

// Last delete time
uint64_t lastDeleteTime = 0;

/**
 * @brief Set pins for columns as OUTPUT and initialize them to HIGH
 * and set rows pins as INPUT_PULLUP resistors.
 * 
 */
void initKeypad() {
  for (int c = 0; c < KEYPAD_COLS; ++c) {
    pinMode(ColPins[c], OUTPUT);
    digitalWrite(ColPins[c], HIGH);
  }

  for (int r = 0; r < KEYPAD_ROWS; ++r) {
    pinMode(RowPins[r], INPUT_PULLUP);
  } 
}

/**
 * @brief Iterates over the columns pins, set the pin LOW, then
 * iterates over the rows pins and checks if any pin is set to 
 * LOW, so it was pressed, otherwise the row pin is HIGH
 * thanks to the used internal pull-up resistors. 
 * Finally set the column pin back to HIGH level and return the
 * pressed key enum value or KEY_NONE.
 * 
 * Also handles keys long press, if key long press detected call 
 * handler for long press and return KEY_NONE.
 * 
 * @return Key 
 */
Key scanKeypad() {
  uint64_t currentMillis = now;

  // After multitap delay expire restore the cursor 
  if (currentMillis - lastPressTime >= MULTITAP_DELAY) {
    enableCursor();
  }

  for (int c = 0; c < KEYPAD_COLS; ++c) {
    digitalWrite(ColPins[c], LOW);

    for (int r = 0; r < KEYPAD_ROWS; ++r) {
      if (digitalRead(RowPins[r]) == LOW) {
        bool longPressTriggered = false;
        uint64_t pressStartTime = millis();

        // Handle long press
        while(digitalRead(RowPins[r]) == LOW) {
          uint64_t loopTime = millis();

          // If key pressed longer than long press delay
          if (loopTime - pressStartTime > LONG_PRESS_DELAY) {
            handleLongPress(Keypad[r][c], loopTime);
            longPressTriggered = true;
          }

          delay(20);
        }

        uint64_t pressEndTime = millis();
        digitalWrite(ColPins[c], HIGH);

        // If hold press not detected return pressed key
        if (longPressTriggered) {
          if (Keypad[r][c] == KEY_S) {
            hideHelp(pressEndTime);
          }
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

/**
 * @brief Get the symbols string for the passed key value, also
 * checks if passed key value is in interval between key 0 and 9.  * 
 * @param key 
 * @return const char* 
 */
const char* getSymbols(Key key) {
  if (key < KEY_0 || key > KEY_9) {
    return NULL;
  }

  return KeySymbols[key];
}

/**
 * @brief Get the char value from the symbols for passed keypad
 * key and current position of symbolIndex.
 *  
 * @param key 
 * @return char 
 */
char getKeyChar(Key key) {
  const char *symbols = getSymbols(key);

  if (symbols == NULL) {
    return KEY_NONE;
  }

  return symbols[symbolIndex];
}

/**
 * @brief Get smart case formatted char value from the input
 * 
 * @param input 
 * @return char 
 */
char getSmartCase(char input) {
  if (isdigit(input)) {
    return input;
  }

  // Capitalize the first letter of message
  if (bufferIndex == 0) {
    return toupper(input);
  }

  // Check for sentence end
  if (bufferIndex >= 2) {
    const char prevChar = getBufferCharByIndex(bufferIndex - 1);
    const char prevPrevChar = getBufferCharByIndex(bufferIndex - 2);

    if (prevChar == ' ' && 
        (prevPrevChar == '.' || prevPrevChar == '!' || prevPrevChar == '?')) {
      return toupper(input);
    }
  }

  return tolower(input);
}

/**
 * @brief Get the key char for the passed key,
 * if it is key cycle decrement bufferIndex, case the correct
 * final char value based on active mode and call display draw char.
 * 
 * @param key 
 */
void displayKey(Key key, bool isCycle) {
  char input = getKeyChar(key);

  // Decrease buffer index if key cycle present
  if (isCycle && bufferIndex > 0) bufferIndex--;

  // Format smart case char value
  if (activeCaseMode == MODE_SMART) {
    input = getSmartCase(input);
  }

  // Convert to upper case in upper case mode
  if (activeCaseMode == MODE_UPPER && !isdigit(input)) {
    input = toupper(input);
  }

  drawChar(input, isCycle);
}

/**
 * @brief Handle key cycle, call display key and 
 * update last press time
 *  
 * @param key 
 */
void handleKey(Key key) {
  // Check for key cycle conditions
  if (key == lastKey && (now - lastPressTime < MULTITAP_DELAY)) {
    disableCursor();

    // Increase the key symbols index
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

/**
 * @brief Get the active mode.
 * 
 * @return CaseMode 
 */
CaseMode getCaseMode() {
  return activeCaseMode;
}

/**
 * @brief Switch the active case mode to next one.
 * 
 */
void switchCaseMode() {
  int next = (int)activeCaseMode + 1;
  if (next > 2) next = 0;
  activeCaseMode = (CaseMode)next;
}

/**
 * @brief Call display delete char, reset last key and
 * symbolIndex and update the last delete time.
 * 
 * @param time 
 */
void handleDelete(uint64_t time) {
  deleteChar(time);

  // Resey the last key and symbol index
  lastKey = KEY_NONE;
  symbolIndex = 0;

  lastDeleteTime = time;
}

/**
 * @brief Handle key long press, based on pressed key calls
 * the action function and redraw the header.
 * 
 * @param key 
 * @param currentLoopTime 
 */
void handleLongPress(Key key, uint64_t currentLoopTime) {
  switch (key) {
    // Clear message
    case KEY_0:
      clearMessage();
      break;

    // Move top
    case KEY_2:
      moveUp(currentLoopTime);
      break;

    // Move left
    case KEY_4:
      moveLeft(currentLoopTime);
      break;

    // Send message
    case KEY_5:
      sendMessage();
      break;

    // Move right
    case KEY_6:
      moveRight(currentLoopTime);
      break;

    // Move down
    case KEY_8:
      moveDown(currentLoopTime);
      break;

    // Show help
    case KEY_S:
      showHelp(currentLoopTime);
      break;

    // Delete
    case KEY_H:
      if (currentLoopTime - lastDeleteTime > DELETE_SPEED_DELAY) {
        handleDelete(currentLoopTime);
      }
      break;
    
    default:
      break;
  }

  drawHeader();
}