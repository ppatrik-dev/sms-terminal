# ESP32 Matrix Keypad Text Terminal

## Overview
This project implements a text terminal on the **ESP32 platform** (WeMos D1 R32) that simulates the experience of writing SMS messages on vintage mobile phones. It features a 3x4 matrix keypad for Multi-tap input and a 0.96-inch OLED display for visualization.

The core feature of this project is the **manual keypad scanning algorithm** implemented via direct GPIO manipulation, removing the need for external keypad libraries.

[![Watch the demo video](https://img.youtube.com/vi/0r5asS0tQAE/maxresdefault.jpg)](https://youtube.com/shorts/0r5asS0tQAE)

## Hardware Requirements
* **Microcontroller:** ESP32 (WeMos D1 R32)
* **Input:** 3x4 Matrix Keypad
* **Output:** 0.96" OLED Display (SPI)
* **Connection:** Breadboard and jumper wires

## Pin Configuration

### 1. Matrix Keypad (GPIO)
The keypad uses an active column scanning method.

| Keypad Pin | Connection | ESP32 GPIO |
| :--- | :--- | :--- |
| **Pin 1** | Column 2 | `GPIO 26` |
| **Pin 2** | Row 1 | `GPIO 14` |
| **Pin 3** | Column 1 | `GPIO 25` |
| **Pin 4** | Row 4 | `GPIO 16` |
| **Pin 5** | Column 3 | `GPIO 13` |
| **Pin 6** | Row 3 | `GPIO 19` |
| **Pin 7** | Row 2 | `GPIO 12` |

### 2. OLED Display (SPI)
The display utilizes the standard SPI interface.

| Display Pin | Function | ESP32 GPIO |
| :--- | :--- | :--- |
| **CS** | Chip Select | `GPIO 5` |
| **DC** | Data/Command | `GPIO 27` |
| **RES** | Reset | `GPIO 17` |
| **D0** | Clock (SCLK) | `GPIO 18` |
| **D1** | Data (MOSI) | `GPIO 23` |

## Software Dependencies
The project is built using the **Arduino IDE**.
* **Required Libraries:**
    * `Adafruit_GFX`
    * `Adafruit_SSD1306`
* **Custom Logic:** Keypad handling is custom-written (no library required).

## User Manual and Controls

### Navigation and Typing
The terminal distinguishes between **Short Presses** (typing/mode) and **Long Presses** (commands).

| Key | Short Press Action | Long Press Action |
| :--- | :--- | :--- |
| **0** | Space | **Clear Message** |
| **2** | Type `a b c 2` | **Cursor UP** |
| **4** | Type `g h i 4` | **Cursor LEFT** |
| **5** | Type `j k l 5` | **SEND Message** |
| **6** | Type `m n o 6` | **Cursor RIGHT** |
| **8** | Type `t u v 8` | **Cursor DOWN** |
| **#** | Backspace / Delete | — |
| **\*** | Switch Mode (`abc/ABC/Abc`) | **Show Help** (Hold) |

### Status Bar
The display includes a status bar showing:
* **Input Mode:** `abc` (lowercase), `ABC` (caps), `Abc` (smart case).
* **Line:** Current cursor line.
* **Stats:** Characters remaining (Limit: 160) and current page.

### Features
* **Multi-tap Input:** Cycle through characters by pressing a key multiple times rapidly.
* **Smart Case:** Automatically capitalizes the first letter of a new sentence.
* **Paging:** Supports messages longer than one screen.

## License and Copyright
© 2025 Patrik Procházka.
All rights reserved. No part of this code may be copied, modified, or redistributed without explicit permission from the author.