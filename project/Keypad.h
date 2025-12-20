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

// Keypad cols and rows number
#define KEYPAD_COLS 3
#define KEYPAD_ROWS 4

// Key multitap delay
#define MULTITAP_DELAY 500

// Delay for long press
#define LONG_PRESS_DELAY 500

// Delay for delete long press
#define DELETE_SPEED_DELAY 200

/**
 * @brief Enum values for keypad keys.
 * 
 */
typedef enum {
  KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, 
  KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, 
  KEY_S, KEY_H, KEY_NONE = -1
} Key;

/**
 * @brief Enum values for typing mode.
 * 
 */
typedef enum {
  MODE_LOWER, MODE_UPPER, MODE_SMART
} CaseMode;

/**
 * @brief Initialize the keypad.
 */
void initKeypad(); 

/**
 * @brief Scan the keypad manually.
 * 
 * @return Key 
 */
Key scanKeypad();

/**
 * @brief Get the symbols for the keypad key.
 * 
 * @param key 
 * @return const char* 
 */
const char* getSymbols(Key key);

/**
 * @brief Get the char to be displayed.
 * 
 * @param key 
 * @return char 
 */
char getKeyChar(Key key);

/**
 * @brief Get char in smart case 
 * 
 * @param input 
 * @return char 
 */
char getSmartCase(char input);

/**
 * @brief Display the char value.
 * 
 * @param key 
 * @param isCycle 
 */
void displayKey(Key key, bool isCycle);

/**
 * @brief Handle the pressed key.
 * 
 * @param key 
 */
void handleKey(Key key);

/**
 * @brief Get the active case mode.
 * 
 * @return CaseMode 
 */
CaseMode getCaseMode();

/**
 * @brief Switch the active case mode.
 * 
 */
void switchCaseMode();

/**
 * @brief Handle delete key pressed.
 * 
 */
void handleDelete(uint64_t time);

/**
 * @brief 
 * 
 */
void handleLongPress(Key key, uint64_t time);

#endif