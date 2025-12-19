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
  return getBufferCharByIndex(bufferIndex);
}

char getBufferCharByIndex(uint8_t index) {
  if (index >= 0 && index < MESSAGE_SIZE) {
    return Buffer[index];
  }

  return MESSAGE_END;
}

void removeBufferChar() {
  removeBufferCharOnIndex(bufferIndex);
}

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

void setBufferChar(char ch) {
  if (bufferIndex >= 0 && bufferIndex < MESSAGE_SIZE) {
    Buffer[bufferIndex] = ch;
  }
}

size_t getBufferLen() {
  return strlen(Buffer);
}

void clearBuffer() {
  size_t bufferLen = strlen(Buffer);

  for (int idx = 0; idx < bufferLen; ++idx) {
    Buffer[idx] = MESSAGE_END;
  }

  bufferIndex = 0;
}