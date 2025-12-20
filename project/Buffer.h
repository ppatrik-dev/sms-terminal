/**
 * @file Buffer.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-12-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef BUFFER_H
#define BUFFER_H

// Message buffer length
#define MESSAGE_SIZE 160

// Message end symbol
#define MESSAGE_END '\0'

/**
 * @brief Get char from buffer on specified index
 * 
 * @param index 
 * @return char 
 */
char getBufferCharByIndex(uint8_t index);

/**
 * @brief Get char on the current position of bufferIndex
 * 
 * @return char 
 */
char getBufferChar();

/**
 * @brief Set char on the current position of bufferIndex
 * 
 * @param ch 
 */
void setBufferChar(char ch);


/**
 * @brief Remove char from buffer on specified position
 * and shift the array if necessary
 * 
 * @param index 
 */
void removeBufferCharOnIndex(uint8_t index);

/**
 * @brief Remove char on the current position of bufferIndex
 * 
 */
void removeBufferChar();

/**
 * @brief Get current length of buffer
 * 
 * @return size_t 
 */
size_t getBufferLen();

/**
 * @brief Clear whole buffer and reset bufferIndex
 * 
 */
void clearBuffer();

#endif