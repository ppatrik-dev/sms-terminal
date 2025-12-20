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

// Text font size
#define FONT_WIDTH 12
#define FONT_HEIGHT 16

// Header size
#define HEADER_HEIGHT 10
#define HEADER_FONT_WIDTH 6
#define HEADER_FONT_HEIGHT 8

// Delay values for cursor
#define CURSOR_BLINK_DELAY 700
#define CURSOR_MOVE_DELAY 200

// Number of characters on one line and total lines
#define CHARS_PER_LINE (SCREEN_WIDTH / FONT_WIDTH)
#define VISIBLE_LINES ((SCREEN_HEIGHT / FONT_HEIGHT) - 1)

// Coord limit values for x
#define MIN_X_POS 0
#define MAX_X_POS (MIN_X_POS + ((CHARS_PER_LINE - 1) * FONT_WIDTH))

// Coord limit values for y
#define MIN_Y_POS FONT_HEIGHT
#define MAX_Y_POS (VISIBLE_LINES * FONT_HEIGHT)

/**
 * @brief Enum values for move direction
 * 
 */
typedef enum {
  MOVE_UP, MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN
} Move;

/**
 * @brief Structure for display coordinates.
 * 
 */
typedef struct {
  int16_t x;
  int16_t y;
} Coord;

/**
 * @brief Initialize the display.
 * 
 */
void initDisplay();

/**
 * @brief Draw the header.
 * 
 */
void drawHeader();

/**
 * @brief Draw the message.
 * 
 */
void drawMessage();

/**
 * @brief Get the the target coords based on move.
 * 
 * @param direction 
 * @return Coord 
 */
Coord getTargetCursorPos(Move move);

/**
 * @brief Draw the char.
 * 
 * @param ch 
 */
void drawChar(char ch, bool isCycle);

/**
 * @brief Draw the cursor.
 * 
 * @param visible 
 */
void drawCursor(bool visible);

/**
 * @brief Show or hide the cursor.
 * 
 */
void updateCursor();

/**
 * @brief Enable the cursor.
 * 
 */
void enableCursor();

/**
 * @brief Disable the cursor.
 * 
 */
void disableCursor();

/**
 * @brief Delete char from text.
 * 
 */
void deleteChar(uint64_t time);

/**
 * @brief Move up in text.
 * 
 * @param time 
 */
void moveUp(uint64_t time);

/**
 * @brief Move left in text.

 * 
 */
void moveLeft(uint64_t time);

/**
 * @brief Move right in text.
 * 
 */
void moveRight(uint64_t time);

/**
 * @brief Move down in text.
 * 
 */
void moveDown(uint64_t time);

/**
 * @brief Show the help table.
 * 
 */
void showHelp(uint64_t time);

/**
 * @brief Hide the help table.
 * 
 */
void hideHelp(uint64_t time);

/**
 * @brief Clear the message.
 * 
 */
void clearMessage();

/**
 * @brief Send the message.
 * 
 */
void sendMessage();

#endif