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

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Display.h"
#include "Buffer.h"
#include "Keypad.h"

extern uint64_t now;
extern uint8_t bufferIndex;
extern bool currentMode;

bool cursorVisible = false;
bool cursorEnabled = true;
uint64_t lastBlinkTime = 0;

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
  Display.setCursor(MIN_X_POS, MIN_Y_POS);
  Display.display();
}

void drawHeader() {
  int16_t savedX = Display.getCursorX();
  int16_t savedY = Display.getCursorY();

  Display.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, SSD1306_BLACK);

  Display.setTextSize(1);
  Display.setCursor(2, 0);
  Mode mode = getMode();
  switch (mode) {
    case MODE_LOWER:
      Display.print("abc");
      break;

    case MODE_UPPER:
      Display.print("ABC");
      break;
    
    case MODE_SMART:
      Display.print("Abc");
      break;
  }
  
  char stats[5];
  sprintf(stats, "%d", MESSAGE_SIZE - getBufferLen());
  int16_t smallFontWidth = 6; 
  int16_t statsX = SCREEN_WIDTH - (strlen(stats) * smallFontWidth) - 2;
  Display.setCursor(statsX, 0);
  Display.print(stats);

  Display.setTextSize(2);
  Display.setTextColor(SSD1306_WHITE);
  Display.setCursor(savedX, savedY);
}

Coord getTargetCursorPos(Direction direction) {
  int16_t x = Display.getCursorX();
  int16_t y = Display.getCursorY();

  switch (direction) {
    case UP:
      y -= FONT_HEIGHT;
      break;

    case LEFT:
      if (x - FONT_WIDTH < MIN_X_POS) {
        // Wrap to end of previous line
        x = MAX_X_POS;
        y -= FONT_HEIGHT;
      } else {
        x -= FONT_WIDTH;
      }
      break;

    case RIGHT:
      if (x + FONT_WIDTH >= SCREEN_WIDTH) {
        // Wrap to start of next line
        x = MIN_X_POS;
        y += FONT_HEIGHT;
      } else {
        x += FONT_WIDTH;
      }
      break;

    case DOWN:
      y += FONT_HEIGHT;
    
    default:
      break;
  }

  if (y < MIN_Y_POS) {
    x = MIN_X_POS;
    y = MIN_Y_POS;
  }
  if (y > MAX_Y_POS) {
    x = MAX_X_POS;
    y = MAX_Y_POS;
  }

  return {x, y};
}


void drawChar(char ch, bool isCycle) {
  if (isCycle) {
    Coord crs = getTargetCursorPos(LEFT);
    int16_t targetX = crs.x;
    int16_t targetY = crs.y;

    Display.setCursor(targetX, targetY);
  }

  setBufferChar(ch);
  bufferIndex++;
  
  Display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  Display.print(ch);
  Display.display();
}

void drawCursor(bool visible) {
  int16_t targetX = Display.getCursorX();
  int16_t targetY = Display.getCursorY();

  if (targetX + FONT_WIDTH > SCREEN_WIDTH) {
    targetX = MIN_X_POS;
    targetY += FONT_HEIGHT;
    Display.setCursor(targetX, targetY);
  }

  uint8_t color = visible ? SSD1306_WHITE : SSD1306_BLACK;

  char ch = getBufferChar();
  
  if (ch != MESSAGE_END) {
    Display.drawRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, color);
  }
  else {
    Display.fillRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, color);
  }
  
  Display.display();

  cursorVisible = visible;
}

void updateCursor() {
  if (now - lastBlinkTime >= CURSOR_BLINK_DELAY && cursorEnabled) {
    cursorVisible = !cursorVisible;
    drawCursor(cursorVisible);
    lastBlinkTime = now;
  }
}

void enableCursor() {
  cursorEnabled = true;
}

void disableCursor() {
  drawCursor(false);
  cursorEnabled = false;
}

/**
 * @brief 
 * 
 */
void deleteChar(uint64_t time) {
  if (bufferIndex == getBufferLen()) {
    Coord crs = getTargetCursorPos(LEFT);
    int16_t targetX = crs.x;
    int16_t targetY = crs.y;

    drawCursor(false);

    bufferIndex--;
    setBufferChar(MESSAGE_END);

    Display.setCursor(targetX, targetY);
    drawCursor(true);
    Display.display();
    
    lastBlinkTime = time;
  }
}

void moveUp(uint64_t time) {
  // Check if we are at least on the second line
  if (bufferIndex >= CHARS_PER_LINE) {
    if (time - lastBlinkTime > CURSOR_MOVE_DELAY) {
      drawCursor(false);
      
      // Move index back by one full row
      bufferIndex -= CHARS_PER_LINE;

      Coord crs = getTargetCursorPos(UP);
      Display.setCursor(crs.x, crs.y);

      drawCursor(true);
      lastBlinkTime = time;
    }
  }
  else {
    drawCursor(true);
    lastBlinkTime = time;
  }
}

void moveLeft(uint64_t time) {
  if (bufferIndex > 0) {
    if (time - lastBlinkTime > CURSOR_MOVE_DELAY) {
      drawCursor(false);
      bufferIndex--;

      Coord crs = getTargetCursorPos(LEFT);
      Display.setCursor(crs.x, crs.y);

      drawCursor(true);
      lastBlinkTime = time;
    }
  }
  else {
    drawCursor(true);
    lastBlinkTime = time;
  }
}

void moveRight(uint64_t time) {
  if (bufferIndex < getBufferLen()) {
    if ((time - lastBlinkTime > CURSOR_MOVE_DELAY)) {
      drawCursor(false);
      bufferIndex++;

      Coord crs = getTargetCursorPos(RIGHT);
      Display.setCursor(crs.x, crs.y);

      drawCursor(true);
      lastBlinkTime = time;
    }
  }
  else {
    drawCursor(true);
    lastBlinkTime = time;
  }
}

void moveDown(uint64_t time) {
  // Check if moving down keeps us within the buffer length
  if (bufferIndex + CHARS_PER_LINE <= getBufferLen()) {
    if (time - lastBlinkTime > CURSOR_MOVE_DELAY) {
      drawCursor(false);
      
      // Move index forward by one full row
      bufferIndex += CHARS_PER_LINE;

      Coord crs = getTargetCursorPos(DOWN);
      Display.setCursor(crs.x, crs.y);

      drawCursor(true);
      lastBlinkTime = time;
    }
  }
  else {
    drawCursor(true);
    lastBlinkTime = time;
  }
}