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

#include <stm32f0xx.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_rcc.h>
#include <system_stm32f0xx.h>

#include "type.h"
#include "font.h"

#include "lcd.h"

//! LCD buffer type list
enum LCDBufferID
{
	LCD_BUFFER_WORK = 0, //!< Work buffer
	LCD_BUFFER_READY, //!< Ready to display buffer
	LCD_BUFFER_DISPLAY, //!< Display buffer used to generate an image on the LCD
	LCD_BUFFER_QUANTITY //!< Quantity of the buffers
};

static volatile bool refreshRequest = FALSE; //!< Status of refresh request

static unsigned char charBuffer[LCD_BUFFER_QUANTITY][LCD_HEIGHT][LCD_WIDTH]; //!< Char buffers
static unsigned char colorBuffer[LCD_BUFFER_QUANTITY][LCD_HEIGHT][LCD_WIDTH]; //!< Background and foreground color buffers

static void lcdCopyWorkBuffer(void);
static void lcdCopyReadyBuffer(void);

static void lcdDraw(void);
static void lcdDrawLine(int lineNumber);

//! Init LCD and set up buffers and output pins
void lcdInit(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitTypeDef gpioInit = {0};
	
	gpioInit.Pin = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8);
	gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioInit.Pull = GPIO_NOPULL;
	gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOC, &gpioInit);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_Delay(10);
	
	lcdClear();
	lcdRefresh();
}

//! Copy data from the canvas painting buffer to the ready to show buffer
void lcdRefresh(void)
{
	lcdCopyWorkBuffer();
	
	__disable_irq();
	refreshRequest = TRUE;
	__enable_irq();
}

//! Clear the canvas painting buffer
void lcdClear(void)
{
	for(int i = 0; i < LCD_HEIGHT; i++)
	{
		for(int j = 0; j < LCD_WIDTH; j++)
		{
			charBuffer[LCD_BUFFER_WORK][i][j] = 0;
			colorBuffer[LCD_BUFFER_WORK][i][j] = 0;
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
void lcdSetChar(unsigned char c, unsigned int x, unsigned int y, unsigned char charColor, unsigned char backgroundColor)
{
	if((x < LCD_WIDTH) && (y < LCD_HEIGHT))
	{
		charBuffer[LCD_BUFFER_WORK][y][x] = c;
		colorBuffer[LCD_BUFFER_WORK][y][x] = ((charColor & 0x0f) | (backgroundColor << 4));
	}
}

//! Scroll the canvas painting buffer one line up
void lcdScroll(void)
{
	for(int y = 0; y < (LCD_HEIGHT - 1); y++)
	{
		for(int x = 0; x < LCD_WIDTH; x++)
		{
			charBuffer[LCD_BUFFER_WORK][y][x] = charBuffer[LCD_BUFFER_WORK][y + 1][x];
			colorBuffer[LCD_BUFFER_WORK][y][x] = colorBuffer[LCD_BUFFER_WORK][y + 1][x];
		}
	}
	
	for(int x = 0; x < LCD_WIDTH; x++)
	{
		charBuffer[LCD_BUFFER_WORK][LCD_HEIGHT - 1][x] = 0;
		colorBuffer[LCD_BUFFER_WORK][LCD_HEIGHT - 1][x] = 0;
	}
}

//! Infinity draw loop on the LCD. It works at background, and it refreshes the LCD when no other operation is in progress.
void lcdDrawLoop(void)
{
	while(1)
	{
		if(refreshRequest)
		{
			__disable_irq();
			{
				lcdCopyReadyBuffer();
				
				refreshRequest = FALSE;
			}
			__enable_irq();
			
			lcdDraw();
		}
	}
}

//! Copy data from the work buffer to the ready buffer
static void lcdCopyWorkBuffer(void)
{
	for(int i = 0; i < LCD_HEIGHT; i++)
	{
		for(int j = 0; j < LCD_WIDTH; j++)
		{
			charBuffer[LCD_BUFFER_READY][i][j] = charBuffer[LCD_BUFFER_WORK][i][j];
			colorBuffer[LCD_BUFFER_READY][i][j] = colorBuffer[LCD_BUFFER_WORK][i][j];
		}
	}
}

//! Copy data from the ready buffer to the display buffer
static void lcdCopyReadyBuffer(void)
{
	for(int i = 0; i < LCD_HEIGHT; i++)
	{
		for(int j = 0; j < LCD_WIDTH; j++)
		{
			charBuffer[LCD_BUFFER_DISPLAY][i][j] = charBuffer[LCD_BUFFER_READY][i][j];
			colorBuffer[LCD_BUFFER_DISPLAY][i][j] = colorBuffer[LCD_BUFFER_READY][i][j];
		}
	}
}

//! Draw a whole buffer on the LCD
static void lcdDraw(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
	
	for(int i = 0; i < 4; i++)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	}

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

	for(int i = 0; i < LCD_HEIGHT; i++)
	{
		lcdDrawLine(i);
	}
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
}

/**
 * Draw a one line on the LCD
 *
 * @param lineNumber Row number to draw on the LCD
 */
static void lcdDrawLine(int lineNumber)
{
	for(int i = 0; i < (LCD_CHAR_HEIGHT * 2); i++)
	{
		int row = lineNumber;
		int fontRow = (i / 2);
		
		for(int column = 0; column < LCD_WIDTH; column++)
		{
			unsigned char c = charBuffer[LCD_BUFFER_DISPLAY][row][column];
			unsigned char b = font[c][fontRow];
			
			unsigned char colorChar = (colorBuffer[LCD_BUFFER_DISPLAY][row][column] & 0x0f);
			unsigned char colorBackground = ((colorBuffer[LCD_BUFFER_DISPLAY][row][column] >> 4) & 0x0f);
			
			for(int k = 0; k < LCD_CHAR_WIDTH; k++)
			{
				if(b & 0x01)
				{
					GPIOC->ODR = ((GPIOC->ODR & 0xff00) | 0xa0 | colorChar);
					GPIOC->BSRR = 0x40;
					GPIOC->BRR = 0x40;
					GPIOC->BSRR = 0x40;
				}
				else
				{
					GPIOC->ODR = ((GPIOC->ODR & 0xff00) | 0xa0 | colorBackground);
					GPIOC->BSRR = 0x40;
					GPIOC->BRR = 0x40;
					GPIOC->BSRR = 0x40;
				}
				
				b >>= 1;
			}
		}
	}
}
