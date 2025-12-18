/**
 * @file project.ino
 * @author Patrik Prochazka (xprochp00@stud.fit.vutbr.cz)
 * @brief 
 * @version 0.1
 * @date 2025-12-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <Arduino.h>

#include "Display.h"
#include "Keypad.h"

// Current time
uint64_t now = 0;

/**
 * @brief 
 * 
 */
void setup() {
  Serial.begin(921600);
  initDisplay();
  initKeypad();
}

/**
 * @brief 
 * 
 */
void loop() {
  now = millis();

  drawHeader();

  Key key = scanKeypad();

  if (key != KEY_NONE) {
    switch (key) {
      case KEY_0: case KEY_1: 
      case KEY_2: case KEY_3: 
      case KEY_4: case KEY_5: 
      case KEY_6: case KEY_7: 
      case KEY_8: case KEY_9:
        handleKey(key);
        break;

      case KEY_S:
        changeCaseMode();
        break;

      case KEY_H:
        handleDelete(now);
        break;
    }
  }

  updateCursor();
}
