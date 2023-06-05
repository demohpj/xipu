/*
 * Author: Pawel Jablonski
 * E-mail: pj@xirx.net
 * WWW: xirx.net
 * GIT: git.xirx.net
 *
 * License: You can use this code however you like
 * but leave information about the original author.
 * Code is free for non-commercial and commercial use.
 */

#ifndef _LCD_H
#define _LCD_H

#include "type.h"

#define LCD_CHAR_WIDTH 8 //!< Width of the char in pixels
#define LCD_CHAR_HEIGHT 8 //!< Height of the char in pixels

#define LCD_WIDTH 40 //!< Chars per row
#define LCD_HEIGHT 30 //!< Chars per column

#define LCD_MATRIX_WIDTH (LCD_CHAR_WIDTH * LCD_WIDTH) //!< Pixel matrix width
#define LCD_MATRIX_HEIGHT (LCD_CHAR_HEIGHT * LCD_HEIGHT) //!< Pixel matrix height

void lcdInit(void);

void lcdRefresh(void);

void lcdClear(void);
void lcdSetChar(unsigned char c, unsigned int x, unsigned int y, unsigned char charColor, unsigned char backgroundColor);

void lcdScroll(void);

void lcdDrawLoop(void);

#endif
