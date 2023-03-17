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

#include "lcd.h"

/**
 * Constructor for the LCD class
 *
 * @param parent Parent object
 */
LCD::LCD(QObject *parent) : QObject(parent)
{
	this->reset();
}

//! Reset the LCD buffers and clear the screen
void LCD::reset()
{
	this->clear();
	this->refresh();
}

//! Copy data from the canvas painting buffer to the ready to show buffer
void LCD::refresh()
{
	for(int i = 0; i < HEIGHT; i++)
	{
		for(int j = 0; j < WIDTH; j++)
		{
			this->buffer[1].charData[i][j] = this->buffer[0].charData[i][j];
			this->buffer[1].colorData[i][j] = this->buffer[0].colorData[i][j];
		}
	}
}

//! Clear the canvas painting buffer
void LCD::clear()
{
	for(int i = 0; i < HEIGHT; i++)
	{
		for(int j = 0; j < WIDTH; j++)
		{
			this->buffer[0].charData[i][j] = 0;
			this->buffer[0].colorData[i][j] = 0;
		}
	}
}

/**
 * Set char in the buffer
 *
 * @param c Char to set
 * @param x Horizontal position in range 0-WIDTH
 * @param y Vertical position in range 0-HEIGHT
 * @param charColor Foreground color code in range 0-15
 * @param backgroundColor Background color code in range 0-15
 */
void LCD::setChar(unsigned char c, int x, int y, unsigned char charColor, unsigned char backgroundColor)
{
	if((x >= 0) && (x < WIDTH) && (y >= 0) && (y < HEIGHT))
	{
		this->buffer[0].charData[y][x] = c;
		this->buffer[0].colorData[y][x] = static_cast<unsigned char>((charColor & COLOR_MASK) | ((backgroundColor & COLOR_MASK) << 4));
	}
}

//! Scroll the canvas painting buffer one line up
void LCD::scroll()
{
	for(int y = 0; y < (HEIGHT - 1); y++)
	{
		for(int x = 0; x < WIDTH; x++)
		{
			this->buffer[0].charData[y][x] = this->buffer[0].charData[y + 1][x];
			this->buffer[0].colorData[y][x] = this->buffer[0].colorData[y + 1][x];
		}
	}

	for(int x = 0; x < WIDTH; x++)
	{
		this->buffer[0].charData[HEIGHT - 1][x] = 0;
		this->buffer[0].colorData[HEIGHT - 1][x] = 0;
	}
}

//! Emit the update signal with ready to show buffer data
void LCD::update()
{
	emit updateBufferSignal(this->buffer[1]);
}
