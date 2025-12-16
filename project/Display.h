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

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

#endif