/**
 * @file Buffer.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-12-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdint.h>
#include <string.h>
#include "Buffer.h"

// Message buffer
char Buffer[MESSAGE_SIZE + 1] = {'\0'};

// Buffer current position index 
uint8_t bufferIndex = 0;

/**
 * @brief Get the char in buffer on bufferIndex position
 * 
 * @return char 
 */
char getBufferChar() {
  return getBufferCharByIndex(bufferIndex);
}

/**
 * @brief Get the char value from the buffer on
 * specified index, checks if the index in buffer range. 
 * 
 * @param index 
 * @return char 
 */
char getBufferCharByIndex(uint8_t index) {
  if (index >= 0 && index < MESSAGE_SIZE) {
    return Buffer[index];
  }

  return MESSAGE_END;
}

/**
 * @brief Removes the char from buffer on bufferIndex.
 * 
 */
void removeBufferChar() {
  removeBufferCharOnIndex(bufferIndex);
}

/**
 * @brief Removes the char from buffer on passed index by
 * shifting the array from the index position.
 * 
 * @param index 
 */
void removeBufferCharOnIndex(uint8_t index) {
  size_t bufferLen = strlen(Buffer);

  if (bufferLen == 0 || index >= bufferLen) {
    return;
  }
  
  for (int idx = index; idx < bufferLen - 1; ++idx) {
    Buffer[idx] = Buffer[idx + 1];
  }
  
  Buffer[bufferLen - 1] = MESSAGE_END; 
}

/**
 * @brief Set the buffer char on the bufferIndex position.
 * 
 * @param ch 
 */
void setBufferChar(char ch) {
  if (bufferIndex >= 0 && bufferIndex < MESSAGE_SIZE) {
    Buffer[bufferIndex] = ch;
  }
}

/**
 * @brief Get the current length of buffer message.
 * 
 * @return size_t 
 */
size_t getBufferLen() {
  return strlen(Buffer);
}

/**
 * @brief Clear the buffer and reset the bufferIndex to 0.
 * 
 */
void clearBuffer() {
  size_t bufferLen = strlen(Buffer);

  for (int idx = 0; idx < bufferLen; ++idx) {
    Buffer[idx] = MESSAGE_END;
  }

  bufferIndex = 0;
}