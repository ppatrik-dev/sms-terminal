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
 */
void moveLeft(uint64_t time);

/**
 * @brief 
 * 
 */
void moveRight(uint64_t time);

#endif