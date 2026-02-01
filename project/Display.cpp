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

// Flag for cursor visibility
bool cursorVisible = false;

// Flag for cursor enabled
bool cursorEnabled = true;

// Time of last cursor blink
uint64_t lastBlinkTime = 0;

// Counter of scroll rows
uint16_t scrollRow = 0;

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

/**
 * @brief Start and clear the display, set size and color
 * of the text, set rotation of display and cursor coordinates.
 * 
 */
void initDisplay() {
  Display.begin(SSD1306_SWITCHCAPVCC);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(SSD1306_WHITE);
  Display.setRotation(2);
  Display.setCursor(MIN_X_POS, MIN_Y_POS);
  Display.display();
}

/**
 * @brief Draw the header with active case mode, current line,
 * remaining characters and current page.
 * 
 */
void drawHeader() {
  // Save message cursor position
  int16_t savedX = Display.getCursorX();
  int16_t savedY = Display.getCursorY();

  // Clear header
  Display.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, SSD1306_BLACK);

  // Case mode
  Display.setTextSize(1);
  Display.setTextColor(SSD1306_WHITE);
  Display.setCursor(2, 1);
  CaseMode mode = getCaseMode();
  switch (mode) {
    case MODE_LOWER: Display.print("abc"); break;
    case MODE_UPPER: Display.print("ABC"); break;
    case MODE_SMART: Display.print("Abc"); break;
    default:         Display.print("???"); break;
  }

  // Current line
  char line[15];
  int currentLine = (bufferIndex / CHARS_PER_LINE) + 1;
  sprintf(line, "Line %d", currentLine);
  int16_t textWidth = strlen(line) * HEADER_FONT_WIDTH;
  int16_t lineX = (SCREEN_WIDTH - textWidth) / 2;
  Display.setCursor(lineX, 1);
  Display.print(line);
  
  // Remaining characters and page
  char stats[15];
  int remaining = MESSAGE_SIZE - getBufferLen();
  int page = (scrollRow / VISIBLE_LINES) + 1;
  sprintf(stats, "%d/%d", remaining, page);
  int16_t statsX = SCREEN_WIDTH - (strlen(stats) * HEADER_FONT_WIDTH) - 2;
  Display.setCursor(statsX, 1);
  Display.print(stats);

  // Restore text settings and message cursor position
  Display.setTextSize(2);
  Display.setTextColor(SSD1306_WHITE);
  Display.setCursor(savedX, savedY);

  Display.display();
}

/**
 * @brief Draw the message from the buffer based on scrolled page.
 * 
 */
void drawMessage() {
  // Clear the text area
  Display.fillRect(MIN_X_POS, MIN_Y_POS, SCREEN_WIDTH, SCREEN_HEIGHT - MIN_Y_POS, SSD1306_BLACK);
  
  // Get start index and characters limit of the text area
  int startIdx = scrollRow * CHARS_PER_LINE;
  int maxVisibleChars = VISIBLE_LINES * CHARS_PER_LINE;

  Display.setCursor(MIN_X_POS, MIN_Y_POS);
  Display.setTextColor(SSD1306_WHITE);

  // Draw the message characters
  for (int i = 0; i < maxVisibleChars; i++) {
    int bufferPos = startIdx + i;

    if (bufferPos >= getBufferLen()) break;

    char c = getBufferCharByIndex(bufferPos);
    Display.print(c);
  }
}

/**
 * @brief Get the Target Cursor Pos object
 * 
 * @param move 
 * @return Coord 
 */
Coord getTargetCursorPos(Move move) {
  int16_t x = Display.getCursorX();
  int16_t y = Display.getCursorY();

  switch (move) {
    // Move cursor up
    case MOVE_UP:
      y -= FONT_HEIGHT;
      break;

    // Move cursor left
    case MOVE_LEFT:
      if (x - FONT_WIDTH < MIN_X_POS) {
        x = MAX_X_POS;
        y -= FONT_HEIGHT;
      } else {
        x -= FONT_WIDTH;
      }
      break;
    
    // Move cursor right
    case MOVE_RIGHT:
      if (x + FONT_WIDTH >= SCREEN_WIDTH) {
        x = MIN_X_POS;
        y += FONT_HEIGHT;
      } else {
        x += FONT_WIDTH;
      }
      break;

    // Move cursor donw
    case MOVE_DOWN:
      y += FONT_HEIGHT;
      break;
  }

  // Check for y position vertical overflow
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

/**
 * @brief Draw specified char on the current cursor position, 
 * if cycle moves cursor one position left, write the char to
 * message buffer and increase the bufferIndex, if screen 
 * overflows move to another page and redraw the message
 * 
 * @param ch 
 * @param isCycle 
 */
void drawChar(char ch, bool isCycle) {
  // If message length hits the message limit return
  if (getBufferLen() == MESSAGE_SIZE) return;

  // Move the cursor one position back in left direction
  if (isCycle) {
    Coord crs = getTargetCursorPos(MOVE_LEFT);
    Display.setCursor(crs.x, crs.y);
  }

  setBufferChar(ch);
  bufferIndex++;

  // Handle text area overflow
  int currentRow = bufferIndex / CHARS_PER_LINE;
  if (currentRow >= scrollRow + VISIBLE_LINES) {
    scrollRow += VISIBLE_LINES;
    drawMessage();
    
    Display.setCursor(MIN_X_POS, MIN_Y_POS);
  }
  else {
    Display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    Display.print(ch);
  }

  Display.display();
}

/**
 * @brief Draw the cursor as filled rectangle or if there 
 * is char under the cursor draw as outline rectangle. 
 * 
 * @param visible 
 */
void drawCursor(bool visible) {
  int16_t targetX = Display.getCursorX();
  int16_t targetY = Display.getCursorY();

  // Check for cursor position screen overflow
  if (targetX + FONT_WIDTH > SCREEN_WIDTH) {
    targetX = MIN_X_POS;
    targetY += FONT_HEIGHT;
    Display.setCursor(targetX, targetY);
  }

  // Choose color of the cursor render
  uint8_t color = visible ? SSD1306_WHITE : SSD1306_BLACK;
  char ch = getBufferChar();
  
  // Draw outline rectangle if character present on position
  if (ch != MESSAGE_END) {
    Display.drawRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, color);
  }
  else {
    Display.fillRect(targetX, targetY, FONT_WIDTH, FONT_HEIGHT, color);
  }
  
  Display.display();
  cursorVisible = visible;
}

/**
 * @brief Periodically redraw the cursor.
 * 
 */
void updateCursor() {
  // Draw the cursor if delay expired and cursor enabled
  if (now - lastBlinkTime >= CURSOR_BLINK_DELAY && cursorEnabled) {
    cursorVisible = !cursorVisible;
    drawCursor(cursorVisible);
    lastBlinkTime = now;
  }
}

/**
 * @brief Enable redrawing of the cursor.
 * 
 */
void enableCursor() {
  cursorEnabled = true;
}

/**
 * @brief Disable redrawing of the cursor.
 * 
 */
void disableCursor() {
  drawCursor(false);
  cursorEnabled = false;
}

/**
 * @brief Delete the char from the message either at the end
 * of the message or inside the message and shift the text.
 * 
 * @param time 
 */
void deleteChar(uint64_t time) {
  if (bufferIndex > 0) { 
    drawCursor(false);

    // Decrease the buffer index and overwrite by termination character
    // if deleting before cursor otherwise remove on any position and shift text
    if (bufferIndex == getBufferLen()) {
      bufferIndex--;
      setBufferChar(MESSAGE_END);
    }
    else {
      removeBufferChar();
    }
    
    // Handle screen overflow
    int targetRow = bufferIndex / CHARS_PER_LINE;
    if (targetRow < scrollRow) {
       if (scrollRow >= VISIBLE_LINES) scrollRow -= VISIBLE_LINES;
       else scrollRow = 0;
    }

    drawMessage(); 

    // Set row value
    int16_t relativeRow = targetRow - scrollRow;
    if (relativeRow < 0) relativeRow = 0; 
    
    // Set cursor new position
    int16_t newX = (bufferIndex % CHARS_PER_LINE) * FONT_WIDTH;
    int16_t newY = MIN_Y_POS + (relativeRow * FONT_HEIGHT);
    Display.setCursor(newX, newY);

    drawCursor(true);
    Display.display();
    
    lastBlinkTime = time;
  }
}

/**
 * @brief Move the cursor up in the message text.
 * 
 * @param time 
 */
void moveUp(uint64_t time) {
  if (bufferIndex >= CHARS_PER_LINE) {
    // Move up if cursor speed delay expired
    if (time - lastBlinkTime > CURSOR_MOVE_DELAY) {
      drawCursor(false);
      bufferIndex -= CHARS_PER_LINE;

      // Handle the screen overflow
      int targetRow = bufferIndex / CHARS_PER_LINE;
      if (targetRow < scrollRow) {
        int16_t savedX = Display.getCursorX();

        if (scrollRow >= VISIBLE_LINES) scrollRow -= VISIBLE_LINES;
        else scrollRow = 0;

        drawMessage();
        Display.setCursor(savedX, MAX_Y_POS);
      }
      else {
        Coord crs = getTargetCursorPos(MOVE_UP);
        Display.setCursor(crs.x, crs.y);
      }

      drawCursor(true);
      lastBlinkTime = time;
    }
  }
  else {
    drawCursor(true);
    lastBlinkTime = time;
  }
}

/**
 * @brief Move the cursor left in the message text.
 * 
 * @param time 
 */
void moveLeft(uint64_t time) {
  if (bufferIndex > 0) {
    // Move left if cursor speed delay expired
    if (time - lastBlinkTime > CURSOR_MOVE_DELAY) {
      drawCursor(false);
      bufferIndex--;

      // Handle the possible screen overflow
      int targetRow = bufferIndex / CHARS_PER_LINE;
      if (targetRow < scrollRow) {
        if (scrollRow >= VISIBLE_LINES) scrollRow -= VISIBLE_LINES;
        else scrollRow = 0;

        drawMessage();
        Display.setCursor(MAX_X_POS, MAX_Y_POS);
      } 
      else {
        Coord crs = getTargetCursorPos(MOVE_LEFT);
        Display.setCursor(crs.x, crs.y);
      }

      drawCursor(true);
      lastBlinkTime = time;
    }
  }
  else {
    drawCursor(true);
    lastBlinkTime = time;
  }
}

/**
 * @brief Move the cursor right in the message text.
 * 
 * @param time 
 */
void moveRight(uint64_t time) {
  if (bufferIndex < getBufferLen()) {
    // Move right if cursor speed delay expired
    if ((time - lastBlinkTime > CURSOR_MOVE_DELAY)) {
      drawCursor(false);
      bufferIndex++;
      
      // Handle the possible screen overflow
      int targetRow = bufferIndex / CHARS_PER_LINE;
      if (targetRow >= scrollRow + VISIBLE_LINES) {
        scrollRow += VISIBLE_LINES;
        drawMessage();
        Display.setCursor(MIN_X_POS, MIN_Y_POS);
      }
      else {
        Coord crs = getTargetCursorPos(MOVE_RIGHT);
        Display.setCursor(crs.x, crs.y);
      }

      drawCursor(true);
      lastBlinkTime = time;
    }
  }
  else {
    drawCursor(true);
    lastBlinkTime = time;
  }
}

/**
 * @brief Move the cursor down in the message text.
 * 
 * @param time 
 */
void moveDown(uint64_t time) {
  if (bufferIndex + CHARS_PER_LINE <= getBufferLen()) {
    // Move down if cursor speed delay expired
    if (time - lastBlinkTime > CURSOR_MOVE_DELAY) {
      drawCursor(false);
      bufferIndex += CHARS_PER_LINE;

      // Handle the possible screen overflow
      int targetRow = bufferIndex / CHARS_PER_LINE;
      if (targetRow >= scrollRow + VISIBLE_LINES) {
        int16_t savedX = Display.getCursorX();
        scrollRow += VISIBLE_LINES;
        drawMessage();
        Display.setCursor(savedX, MIN_Y_POS);
      }
      else {
        Coord crs = getTargetCursorPos(MOVE_DOWN);
        Display.setCursor(crs.x, crs.y);
      }

      drawCursor(true);
      lastBlinkTime = time;
    }
  }
  else {
    drawCursor(true);
    lastBlinkTime = time;
  }
}

/**
 * @brief Draw the help table with the terminal control informations.
 * 
 * @param time 
 */
void showHelp(uint64_t time) {
  drawCursor(false);
  Display.fillRect(0, MIN_Y_POS, SCREEN_WIDTH, SCREEN_HEIGHT - MIN_Y_POS, SSD1306_BLACK);

  Display.setTextSize(1);
  Display.setTextColor(SSD1306_WHITE);
  
  // Set the positions
  int16_t y = MIN_Y_POS + 4;
  int16_t lineStep = 10;
  int16_t col1_Key = 4;
  int16_t col1_Act = 16;
  int16_t col2_Key = 68;
  int16_t col2_Act = 80;

  // Display the content
  Display.setCursor(col1_Key, y); Display.print("0");
  Display.setCursor(col1_Act, y); Display.print(": Clear");
  Display.setCursor(col2_Key, y); Display.print("2");
  Display.setCursor(col2_Act, y); Display.print(": UP");
  y += lineStep;
  Display.setCursor(col1_Key, y); Display.print("5");
  Display.setCursor(col1_Act, y); Display.print(": Send");
  Display.setCursor(col2_Key, y); Display.print("4");
  Display.setCursor(col2_Act, y); Display.print(": LEFT");
  y += lineStep;
  Display.setCursor(col1_Key, y); Display.print("*");
  Display.setCursor(col1_Act, y); Display.print(": Mode");
  Display.setCursor(col2_Key, y); Display.print("6");
  Display.setCursor(col2_Act, y); Display.print(": RIGHT");
  y += lineStep;
  Display.setCursor(col1_Key, y); Display.print("#");
  Display.setCursor(col1_Act, y); Display.print(": Del");
  Display.setCursor(col2_Key, y); Display.print("8");
  Display.setCursor(col2_Act, y); Display.print(": DOWN");

  Display.drawFastVLine(62, MIN_Y_POS + 2, 40, SSD1306_WHITE);
  Display.display();
}

/**
 * @brief Hide the help table and redraw the message.
 * 
 * @param time 
 */
void hideHelp(uint64_t time) {
  drawMessage();

  // Set the original cursor position
  int currentAbsRow = bufferIndex / CHARS_PER_LINE;
  int relativeRow = currentAbsRow - scrollRow;
  int col = bufferIndex % CHARS_PER_LINE;
  int16_t x = MIN_X_POS + (col * FONT_WIDTH);
  int16_t y = MIN_Y_POS + (relativeRow * FONT_HEIGHT);

  Display.setCursor(x, y);
  drawCursor(true);
  lastBlinkTime = time;
}

/**
 * @brief Clear the text area.
 * 
 */
void clearMessage() {
  if (getBufferLen() > 0) {
    clearBuffer();
    scrollRow = 0;
    drawMessage();
  }
}

/**
 * @brief If message not empty send the message and clear the text area.
 * 
 */
void sendMessage() {
  // If message not empty send the message
  if (getBufferLen() > 0) {
    clearMessage();
    Display.setCursor(MIN_X_POS, MIN_Y_POS);
    Display.print("Sending...");
    Display.display();

    delay(2000);

    Display.setCursor(MIN_X_POS, MIN_Y_POS + FONT_HEIGHT);
    Display.print("SMS sent.");
    Display.display();

    delay(1000);
    drawMessage();
  }
}