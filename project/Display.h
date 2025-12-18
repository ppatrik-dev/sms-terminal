/**
 * @file Display.h
 * @author Patrik Prochazka (xprochp00@stud.fit.vutbr.cz)
 * @brief 
 * @version 0.1
 * @date 2025-12-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef DISPLAY_H
#define DISPLAY_H

// Screen size config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// SPI pins config
#define SPI_MOSI 23
#define SPI_CLK  18
#define SPI_DC   27
#define SPI_CS   5
#define SPI_RST  17

// Text font size config
#define FONT_WIDTH 12
#define FONT_HEIGHT 16

// Delay values for cursor
#define CURSOR_BLINK_DELAY 750
#define CURSOR_MOVE_DELAY 250

// Number of characters on one line and total lines
#define CHARS_PER_LINE (SCREEN_WIDTH / FONT_WIDTH)
#define TOTAL_LINES (SCREEN_HEIGHT / FONT_HEIGHT)

// Coord limit values for x
#define MIN_X_POS 0
#define MAX_X_POS ((CHARS_PER_LINE - 1) * FONT_WIDTH)

// Coord limit values for y
#define MIN_Y_POS 0
#define MAX_Y_POS ((TOTAL_LINES - 1) * FONT_HEIGHT)

typedef enum {
    UP, LEFT, RIGHT, DOWN
} Direction;

typedef struct {
  int16_t x;
  int16_t y;
} Coord;

/**
 * @brief 
 * 
 */
void initDisplay();

/**
 * @brief
 * 
 * @param direction 
 * @return Coord 
 */
Coord getTargetCursorPos(Direction direction);

/**
 * @brief 
 * 
 * @param ch 
 */
void drawChar(char ch, bool isCycle);

/**
 * @brief 
 * 
 * @param visible 
 */
void drawCursor(bool visible);

/**
 * @brief 
 * 
 */
void updateCursor();

/**
 * @brief 
 * 
 */
void enableCursor();

/**
 * @brief 
 * 
 */
void disableCursor();

/**
 * @brief 
 * 
 */
void deleteChar(uint64_t time);

/**
 * @brief 
 * 
 * @param time 
 */
void moveUp(uint64_t time);

/**
 * @brief 
 * 
 */
void moveLeft(uint64_t time);

/**
 * @brief 
 * 
 */
void moveRight(uint64_t time);

/**
 * @brief 
 * 
 */
void moveDown(uint64_t time);

#endif