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
#define BUFFER_SIZE 255

// Message end symbol
#define MESSAGE_END '\0'

/**
 * @brief 
 * 
 * @return char 
 */
char getBufferChar();

/**
 * @brief
 * 
 * @param ch 
 */
void setBufferChar(char ch);

/**
 * @brief 
 * 
 */
size_t getBufferLen();

#endif