/**
 * @file Display.cpp
 * @author Patrik Prochazka (xprochp00@stud.fit.vutbr.cz)
 * @brief 
 * @version 0.1
 * @date 2025-12-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdint.h>
#include "Display.h"

uint64_t lastBlinkTime = 0;
bool cursorVisible = false;
bool renderEnabled = true;
const uint32_t BLINK_INTERVAL = 1000;

extern uint64_t now;

Adafruit_SSD1306 Display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  SPI_MOSI,
  SPI_CLK,
  SPI_DC,
  SPI_RST,
  SPI_CS
);

void initDisplay() {
  Display.begin(SSD1306_SWITCHCAPVCC);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(SSD1306_WHITE);
  Display.setRotation(2);
  Display.setCursor(0, 0);
  Display.display();
}

void drawCursor(bool visible) {
  int16_t targetX = Display.getCursorX();
  int16_t targetY = Display.getCursorY();

  if (targetX + FONT_WIDTH > SCREEN_WIDTH) {
    targetX = 0;
    targetY += FONT_HEIGHT;
  }

  uint16_t color = visible ? SSD1306_WHITE : SSD1306_BLACK;

  Display.fillRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, color);

  cursorVisible = visible;
  Display.display();
}

void updateCursor() {
  if (now - lastBlinkTime >= BLINK_INTERVAL && renderEnabled) {
    cursorVisible = !cursorVisible;
    drawCursor(cursorVisible);
    lastBlinkTime = now;
  }
}

void enableCursor() {
  renderEnabled = true;
}

void disableCursor() {
  drawCursor(false);
  renderEnabled = false;
}

/**
 * @brief 
 * 
 */
void deleteChar() {
  int16_t x = Display.getCursorX();
  int16_t y = Display.getCursorY();

  drawCursor(false);
  cursorVisible = false;

  if (x >= FONT_WIDTH) {
    int16_t targetX = x - FONT_WIDTH;
    int16_t targetY = y;

    Display.fillRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, SSD1306_BLACK);

    Display.setCursor(targetX, targetY);
  }
  else if (y >= FONT_HEIGHT) {
    uint8_t charsPerLine = SCREEN_WIDTH / FONT_WIDTH;
    int16_t targetX = (charsPerLine - 1) * FONT_WIDTH;
    int16_t targetY = y - FONT_HEIGHT;

    Display.fillRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, SSD1306_BLACK);
    Display.setCursor(targetX, targetY);
    Display.display();
  }
  else {
    return;
  }

  drawCursor(true);
  cursorVisible = true;
  lastBlinkTime = now;

  Display.display();
}