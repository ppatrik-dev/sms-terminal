/**
 * @file Keypad.h
 * @author Patrik Prochazka (xprochp00@stud.fit.vutbr.cz)
 * @brief 
 * @version 0.1
 * @date 2025-12-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#define KEYPAD_COLS 3
#define KEYPAD_ROWS 4

// Key multitap delay
#define MULTITAP_DELAY 500

// Delay for long press
#define LONG_PRESS_DELAY 500

#define MODE_SWITCH_DELAY 500

// Delete hold delay
#define DELETE_SPEED_DELAY 200

/**
 * @brief 
 * 
 */
typedef enum {
  KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, 
  KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, 
  KEY_S, KEY_H, KEY_NONE = -1
} Key;

typedef enum {
  MODE_LOWER, MODE_UPPER, MODE_SMART
} CaseMode;

/**
 * @brief 
 * 
 */
void initKeypad(); 

/**
 * @brief 
 * 
 * @return Key 
 */
Key scanKeypad();

/**
 * @brief
 * 
 * @param key 
 * @return const char* 
 */
const char* getSymbols(Key key);

/**
 * @brief Get the Key Char object
 * 
 * @param key 
 * @return char 
 */
char getKeyChar(Key key);

/**
 * @brief 
 * 
 * @param key 
 * @param isCycle 
 */
void displayKey(Key key, bool isCycle);

/**
 * @brief 
 * 
 * @param key 
 */
void handleKey(Key key);

/**
 * @brief Get the Mode object
 * 
 * @return Mode 
 */
CaseMode getCaseMode();

/**
 * @brief 
 * 
 */
void switchCaseMode();

/**
 * @brief 
 * 
 */
void handleDelete(uint64_t time);

/**
 * @brief 
 * 
 */
void handleLongPress(Key key, uint64_t time);

#endif