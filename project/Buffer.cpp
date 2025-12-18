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

char Buffer[MESSAGE_SIZE + 1] = {'\0'};
uint8_t bufferIndex = 0;

char getBufferChar() {
  return getCharByIndex(bufferIndex);
}

char getCharByIndex(uint8_t index) {
  if (index >= 0 && index < MESSAGE_SIZE) {
    return Buffer[index];
  }

  return MESSAGE_END;
}

void setBufferChar(char ch) {
  if (bufferIndex >= 0 && bufferIndex < MESSAGE_SIZE) {
    Buffer[bufferIndex] = ch;
  }
}

size_t getBufferLen() {
  return strlen(Buffer);
}