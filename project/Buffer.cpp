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

char Buffer[BUFFER_SIZE] = {'\0'};
uint8_t bufferIndex = 0;

char getBufferChar() {
  if (bufferIndex >= 0 && bufferIndex < BUFFER_SIZE) {
    return Buffer[bufferIndex];
  }

  return MESSAGE_END;
}

void setBufferChar(char ch) {
  if (bufferIndex >= 0 && bufferIndex < BUFFER_SIZE) {
    Buffer[bufferIndex] = ch;
  }
}

size_t getBufferLen() {
  return strlen(Buffer);
}