/**
 * @file Display.cpp
 * @author Patrik Prochazka (xprochp00@stud.fit.vutbr.cz)
 * @brief 
 * @version 0.1
 * @date 2025-12-16
 * * @copyright Copyright (c) 2025
 * */

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

bool cursorVisible = false;
bool cursorEnabled = true;
uint64_t lastBlinkTime = 0;
uint16_t scrollRow = 0;

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

  // Clear header area (Black bar)
  Display.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, SSD1306_BLACK);

  Display.setTextSize(1);
  Display.setTextColor(SSD1306_WHITE); // Ensure text is white on black bar
  
  // 1. LEFT: Mode Indicator (Start at X=2)
  // Y=1 centers the 8px font inside the 10px bar
  Display.setCursor(2, 1); 
  
  CaseMode mode = getCaseMode();
  switch (mode) {
    case MODE_LOWER: Display.print("abc"); break;
    case MODE_UPPER: Display.print("ABC"); break;
    case MODE_SMART: Display.print("Abc"); break;
    default:         Display.print("???"); break;
  }

  // 2. CENTER: Line Info
  char line[15];
  // Calculate current line (1-based)
  int currentLine = (bufferIndex / CHARS_PER_LINE) + 1;
  sprintf(line, "Line %d", currentLine);
  
  // Calculate width: Length * 6px (standard width for size 1)
  int16_t textWidth = strlen(line) * 6;
  
  // Center math: (Screen - Text) / 2
  int16_t lineX = (SCREEN_WIDTH - textWidth) / 2;
  
  Display.setCursor(lineX, 1);
  Display.print(line);
  
  // 3. RIGHT: Stats (Remaining / Page)
  char stats[15];
  int remaining = MESSAGE_SIZE - getBufferLen();
  int page = (scrollRow / VISIBLE_LINES) + 1;
  
  sprintf(stats, "%d/%d", remaining, page);
  
  // Align right: ScreenWidth - TextWidth - Margin(2)
  int16_t statsX = SCREEN_WIDTH - (strlen(stats) * 6) - 2;
  
  Display.setCursor(statsX, 1);
  Display.print(stats);

  // Restore State
  Display.setTextSize(2);
  Display.setTextColor(SSD1306_WHITE);
  Display.setCursor(savedX, savedY);
}

void drawMessage() {
  // Clear only the text area
  Display.fillRect(MIN_X_POS, MIN_Y_POS, SCREEN_WIDTH, SCREEN_HEIGHT - MIN_Y_POS, SSD1306_BLACK);
  
  int startIdx = scrollRow * CHARS_PER_LINE;
  int maxVisibleChars = VISIBLE_LINES * CHARS_PER_LINE;

  Display.setCursor(MIN_X_POS, MIN_Y_POS);
  Display.setTextColor(SSD1306_WHITE);

  for (int i = 0; i < maxVisibleChars; i++) {
    int bufferPos = startIdx + i;

    if (bufferPos >= getBufferLen()) break;

    char c = getBufferCharByIndex(bufferPos);
    Display.print(c);
  }
}

Coord getTargetCursorPos(Move move) {
  int16_t x = Display.getCursorX();
  int16_t y = Display.getCursorY();

  switch (move) {
    case MOVE_UP:
      y -= FONT_HEIGHT;
      break;

    case MOVE_LEFT:
      if (x - FONT_WIDTH < MIN_X_POS) {
        // Wrap to end of previous line
        x = MAX_X_POS;
        y -= FONT_HEIGHT;
      } else {
        x -= FONT_WIDTH;
      }
      break;

    case MOVE_RIGHT:
      if (x + FONT_WIDTH >= SCREEN_WIDTH) {
        // Wrap to start of next line
        x = MIN_X_POS;
        y += FONT_HEIGHT;
      } else {
        x += FONT_WIDTH;
      }
      break;

    case MOVE_DOWN:
      y += FONT_HEIGHT;
      break; // Added missing break
    
    default:
      break;
  }

  // Clamp values to valid screen area
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
  if (getBufferLen() == MESSAGE_SIZE) return;

  if (isCycle) {
    // Visually step back to overwrite
    Coord crs = getTargetCursorPos(MOVE_LEFT);
    Display.setCursor(crs.x, crs.y);
  }

  setBufferChar(ch);
  bufferIndex++;

  int currentRow = bufferIndex / CHARS_PER_LINE;

  // --- PAGE FLIP LOGIC (Typing) ---
  // If we typed onto a line that is NOT visible on current page
  if (currentRow >= scrollRow + VISIBLE_LINES) {
    scrollRow += VISIBLE_LINES; // Jump to next page
    drawMessage();
    
    // Cursor goes to TOP LEFT of new page
    Display.setCursor(MIN_X_POS, MIN_Y_POS);
  }
  else {
    Display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    Display.print(ch);
  }
  
  drawHeader(); // Update stats
  Display.display();
}

void drawCursor(bool visible) {
  int16_t targetX = Display.getCursorX();
  int16_t targetY = Display.getCursorY();

  // Handle visual wrapping for cursor rectangle
  if (targetX + FONT_WIDTH > SCREEN_WIDTH) {
    targetX = MIN_X_POS;
    targetY += FONT_HEIGHT;
  }

  uint8_t color = visible ? SSD1306_WHITE : SSD1306_BLACK;
  char ch = getBufferChar(); // Char at bufferIndex
  
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

void deleteChar(uint64_t time) {
  if (bufferIndex > 0) { 
    drawCursor(false);

    if (bufferIndex == getBufferLen()) {
      bufferIndex--;
      setBufferChar(MESSAGE_END);
    }
    else {
      removeBufferChar();
    }
    
    int targetRow = bufferIndex / CHARS_PER_LINE;
    
    // Check if we backspaced onto a previous page
    if (targetRow < scrollRow) {
       // Reverse Page Flip Logic
       if (scrollRow >= VISIBLE_LINES) scrollRow -= VISIBLE_LINES;
       else scrollRow = 0;
    }

    // Redraw the screen to show the shifted text
    drawMessage(); 

    // 4. Place Cursor correctly
    // Calculate new cursor position based on the new bufferIndex
    int16_t relativeRow = targetRow - scrollRow;
    
    // Safety clamp if scrolling math gets weird
    if (relativeRow < 0) relativeRow = 0; 
    
    int16_t newX = (bufferIndex % CHARS_PER_LINE) * FONT_WIDTH;
    int16_t newY = MIN_Y_POS + (relativeRow * FONT_HEIGHT);

    Display.setCursor(newX, newY);

    drawHeader(); // Update character count
    drawCursor(true);
    Display.display();
    
    lastBlinkTime = time;
  }
}

void moveUp(uint64_t time) {
  if (bufferIndex >= CHARS_PER_LINE) {
    if (time - lastBlinkTime > CURSOR_MOVE_DELAY) {
      drawCursor(false);
      bufferIndex -= CHARS_PER_LINE;

      int targetRow = bufferIndex / CHARS_PER_LINE;

      // --- PAGE FLIP UP ---
      if (targetRow < scrollRow) {
        int16_t savedX = Display.getCursorX();

        if (scrollRow >= VISIBLE_LINES) scrollRow -= VISIBLE_LINES;
        else scrollRow = 0;

        drawMessage();
        // Cursor jumps to BOTTOM of previous page
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

void moveLeft(uint64_t time) {
  if (bufferIndex > 0) {
    if (time - lastBlinkTime > CURSOR_MOVE_DELAY) {
      drawCursor(false);
      bufferIndex--;

      int targetRow = bufferIndex / CHARS_PER_LINE;
      
      // --- PAGE FLIP BACK CHECK ---
      if (targetRow < scrollRow) {
        if (scrollRow >= VISIBLE_LINES) scrollRow -= VISIBLE_LINES;
        else scrollRow = 0;

        drawMessage();
        Display.setCursor(MAX_X_POS, MAX_Y_POS); // Bottom Right of prev page
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

void moveRight(uint64_t time) {
  if (bufferIndex < getBufferLen()) {
    if ((time - lastBlinkTime > CURSOR_MOVE_DELAY)) {
      drawCursor(false);
      bufferIndex++;
      
      int targetRow = bufferIndex / CHARS_PER_LINE;

      // --- PAGE FLIP FORWARD CHECK ---
      if (targetRow >= scrollRow + VISIBLE_LINES) {
        scrollRow += VISIBLE_LINES;
        drawMessage();
        Display.setCursor(MIN_X_POS, MIN_Y_POS); // Top Left of new page
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

void moveDown(uint64_t time) {
  if (bufferIndex + CHARS_PER_LINE <= getBufferLen()) {
    if (time - lastBlinkTime > CURSOR_MOVE_DELAY) {
      drawCursor(false);
      bufferIndex += CHARS_PER_LINE;

      int targetRow = bufferIndex / CHARS_PER_LINE;

      // --- PAGE FLIP DOWN ---
      if (targetRow >= scrollRow + VISIBLE_LINES) {
        int16_t savedX = Display.getCursorX();
        
        scrollRow += VISIBLE_LINES;
        drawMessage();
        // Cursor jumps to TOP of next page
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