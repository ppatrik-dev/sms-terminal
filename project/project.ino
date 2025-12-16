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

#include "Display.h"
#include "Keypad.h"

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

// Current time
uint64_t now = 0;

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

  initKeypad();
}

/**
 * @brief 
 * 
 */
void loop() {
  now = millis();

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
        deleteLastChar();
        break;
    }
  }
}
