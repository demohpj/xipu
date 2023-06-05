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
#include "keyboard.h"
#include "led.h"
#include "lcd.h"
#include "rs232.h"
#include "rtc.h"
#include "speaker.h"
#include "fs.h"

#include "io.h"

//!< List of motherboard's virtual registers
enum IORegAddress
{
	IO_REG_KEYBOARD = 0, //!< Keyboard register
	IO_REG_LED_COMMAND = 1, //!< LED register
	IO_REG_ADDRESS_LOW = 2, //!< Lower address register
	IO_REG_ADDRESS_HIGH = 3, //!< Upper address register
	IO_REG_LCD_COLOR = 4, //!< LCD color register
	IO_REG_LCD_CHAR = 5, //!< LCD char register
	IO_REG_LCD_COMMAND = 6, //!< LCD command register
	IO_REG_RS232_RX = 7, //!< RS232 receive register
	IO_REG_RS232_TX = 8, //!< RS232 transmit register
	IO_REG_RTC_FIELD = 9, //!< Real Time Clock data register
	IO_REG_RTC_COMMAND = 10, //!< Real Time Clock command register
	IO_REG_SPEAKER_FIELD = 11, //!< Speaker data register
	IO_REG_SPEAKER_COMMAND = 12, //!< Speaker command register
	IO_REG_FS_NAME = 13, //!< File System name register
	IO_REG_FS_DATA = 14, //!< File System data register
	IO_REG_FS_COMMAND = 15 //!< File System command register
};

//!< List of LED commands
enum IOLedCommand
{
	IO_LED_RUN_OFF = 0, //!< Turn off run LED
	IO_LED_RUN_ON, //!< Turn on run LED
	IO_LED_ERROR_OFF, //!< Turn off error LED
	IO_LED_ERROR_ON //!< Turn on error LED
};

//!< List of LCD commands
enum IOLcdCommand
{
	IO_LCD_CLEAR = 0, //!< Clear the screen
	IO_LCD_REFRESH, //!< Refresh the screen using data from the framebuffer
	IO_LCD_SCROLL //!< Scroll up one line up the screen
};

//!< List of Real Time Clock fields
enum IORtcField
{
	IO_RTC_YEAR = 0, //!< Year
	IO_RTC_MONTH, //!< Month
	IO_RTC_DAY, //!< Day
	IO_RTC_HOUR, //!< Hour
	IO_RTC_MINUTE, //!< Minute
	IO_RTC_SECOND //!< Second
};

//!< List of Real Time Clock commands
enum IORtcCommand
{
	IO_RTC_READ = 0, //!< Read current date and time
	IO_RTC_WRITE //!< Set current date and time
};

//!< List of Speaker fields
enum IOSpeakerField
{
	IO_SPEAKER_NOTE = 0, //!< Note number
	IO_SPEAKER_TIME, //!< Time to play
	IO_SPEAKER_FILL, //!< Percentage fill of the square wave
	IO_SPEAKER_VOLUME //!< Volume
};

//!< List of Speaker commands
enum IOSpeakerCommand
{
	IO_SPEAKER_CLEAR = 0, //!< Stop playing and clear the buffer
	IO_SPEAKER_ADD_NOTE, //!< Add note to the buffer and play it
};

//!< List of File System commands
enum IOFSCommand
{
	IO_FS_OPEN_APP = 0, //!< Open an application
	IO_FS_OPEN_FILE, //!< Open a file
	IO_FS_LIST_APP, //!< List all applications
	IO_FS_LIST_FILE, //!< List all files
	IO_FS_SIZE, //!< Get size of the file
	IO_FS_READ_BLOCK //!< Read data from the block
};

//!< This struct cointains a memory adddress
struct IOAddress
{
	unsigned char low; //!< Lower byte of the address
	unsigned char high; //!< Upper byte of the address
};

static unsigned char regSelected = 0x00; //!< Selected virtual register number

//!< This struct contains motherboard's virtual registers buffer
struct
{
	unsigned char keyboard; //!< Keyboard buffer
	struct IOAddress address; //!< Address buffer
	unsigned char lcdForegroundColor; //!< LCD foreground color buffer
	unsigned char lcdBackgroundColor; //!< LCD background color buffer
	unsigned char lcdChar; //!< LCD char buffer
	unsigned char rs232Rx; //!< RS232 receive buffer
	unsigned char rs232Tx; //!< RS232 transmit bufffer
	unsigned char rs232TxBufferFree; //!< RS232 transmit free space in buffer counter
	struct RTCDateTime rtcField; //!< Real Time Clock buffer
	struct SpeakerNote speakerField; //!< Speaker buffer
	unsigned char speakerBufferFree; //!< Speaker free space in buffer counter
	struct FSName fsName; //!< File System name buffer
	struct FSSize fsSize; //!< File System size buffer
	struct FSBlock fsBlock; //!< File System data block buffer
} static reg;


//! First stage of init. It set up all IO pins. It must be executed before all devices init.
void ioPreInit(void)
{
	GPIO_InitTypeDef gpioInit = {0};
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	gpioInit.Pin = (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
	gpioInit.Mode = GPIO_MODE_INPUT;
	gpioInit.Pull = GPIO_PULLDOWN;
	gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOB, &gpioInit);
	
	gpioInit.Pin = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioInit.Pull = GPIO_NOPULL;
	gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOB, &gpioInit);
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
}

//! Second stage of init. It set up the IRQ used in IO communication. I can only be executed when all devices are inited.
void ioInit(void)
{
	GPIO_InitTypeDef gpioInit = {0};
	
	gpioInit.Pin = GPIO_PIN_15;
	gpioInit.Mode = GPIO_MODE_IT_RISING;
	gpioInit.Pull = GPIO_PULLDOWN;
	gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOB, &gpioInit);

	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 4, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

//! Process the IO clock fire event. Toggle the clock pin.
void ioFireClock(void)
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
}

//! Process the IO peripherial fire event. Check if the keyboard or the rs232 have a new data in the buffers.
void ioFirePeripherial(void)
{
	if(keyboardIsData())
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	}
	
	if(rs232RxIsData())
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	}
}

//! Process the IRQ event for the IO. It processes a new incoming request from the XiPU.
void EXTI4_15_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET) 
	{
		unsigned char value = ((GPIOB->IDR >> 8) & 0xff);
		
		unsigned char half = (value & (1 << 4));
		unsigned char mode = (value & (1 << 5));
		unsigned char rw = (value & (1 << 6));

		if(!mode)
		{
			// Select register to work
			regSelected = (value & IO_REG_SELECT_MASK);
		}
		else
		{
			if(!rw)
			{
				unsigned char dataOut = 0x00;

				// Read register value
				switch(regSelected)
				{
					case IO_REG_KEYBOARD :
						{
							if(!half)
							{
								reg.keyboard = keyboardGetKey();
								
								if(!keyboardIsData())
								{
									HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
								}
							}

							dataOut = reg.keyboard;
						}
						break;

					case IO_REG_RS232_RX :
						{
							if(!half)
							{
								reg.rs232Rx = rs232Receive();
								
								if(!rs232RxIsData())
								{
									HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
								}
							}

							dataOut = reg.rs232Rx;
						}
						break;

					case IO_REG_RTC_FIELD :
						switch(reg.address.low)
						{
							case IO_RTC_YEAR :
								dataOut = reg.rtcField.year;
								break;

							case IO_RTC_MONTH :
								dataOut = reg.rtcField.month;
								break;

							case IO_RTC_DAY :
								dataOut = reg.rtcField.day;
								break;

							case IO_RTC_HOUR :
								dataOut = reg.rtcField.hour;
								break;

							case IO_RTC_MINUTE :
								dataOut = reg.rtcField.minute;
								break;

							case IO_RTC_SECOND :
								dataOut = reg.rtcField.second;
								break;

							default :
								dataOut = 0;
								break;
						}
						break;

					case IO_REG_SPEAKER_FIELD :
						{
							if(!half)
							{
								reg.speakerBufferFree = speakerGetBufferFree();
							}

							dataOut = reg.speakerBufferFree;
						}
						break;

					case IO_REG_FS_DATA :
						{
							dataOut = reg.fsBlock.data[reg.address.low];

							if(half)
							{
								reg.address.low++;
							}
						}
						break;

					case IO_REG_FS_COMMAND :
						switch(reg.address.low)
						{
							case IO_FS_OPEN_APP :
								{
									reg.fsName.data[FS_NAME_MAX_LENGTH] = 0;

									if(fsOpen("app", &(reg.fsName), &(reg.fsSize)))
									{
										dataOut = 1;
									}
									else
									{
										dataOut = 0;
									}
								}
								break;

							case IO_FS_OPEN_FILE :
								{
									reg.fsName.data[FS_NAME_MAX_LENGTH] = 0;

									if(fsOpen("file", &(reg.fsName), &(reg.fsSize)))
									{
										dataOut = 1;
									}
									else
									{
										dataOut = 0;
									}
								}
								break;

							case IO_FS_LIST_APP :
								{
									if(fsList("app", &(reg.fsSize)))
									{
										dataOut = 1;
									}
									else
									{
										dataOut = 0;
									}
								}
								break;

							case IO_FS_LIST_FILE :
								{
									if(fsList("file", &(reg.fsSize)))
									{
										dataOut = 1;
									}
									else
									{
										dataOut = 0;
									}
								}
								break;

							case IO_FS_SIZE :
								switch(reg.address.high)
								{
									case 0 :
										dataOut = reg.fsSize.low;
										break;

									case 1 :
										dataOut = reg.fsSize.high;
										break;

									default:
										dataOut = 0;
										break;
								}
								break;

							default :
								dataOut = 0;
								break;
						}
						break;

					default :
						dataOut = 0;
						break;
				}

				if(half)
				{
					dataOut >>= 4;
				}

				// Register value to out
				GPIOB->ODR = ((GPIOB->ODR & IO_IN_CONTROL_MASK) | (dataOut & IO_IN_DATA_MASK));
			}
			else
			{
				unsigned char dataInRaw = (value & IO_IN_DATA_MASK);
				unsigned char dataIn = dataInRaw;
				unsigned char mask = IO_HALF_UP_DATA_MASK;

				if(half)
				{
					dataIn <<= IO_HALF_DATA_OFFSET;
					mask >>= IO_HALF_DATA_OFFSET;
				}

				// Write register value
				switch(regSelected)
				{
					case IO_REG_LED_COMMAND :
						switch(dataInRaw)
						{
							case IO_LED_RUN_OFF :
								ledSetRun(FALSE);
								break;
							case IO_LED_RUN_ON :
								ledSetRun(TRUE);
								break;
							case IO_LED_ERROR_OFF :
								ledSetError(FALSE);
								break;
							case IO_LED_ERROR_ON :
								ledSetError(TRUE);
								break;
						}
						break;

					case IO_REG_ADDRESS_LOW :
						reg.address.low = ((reg.address.low & mask) | dataIn);
						break;

					case IO_REG_ADDRESS_HIGH :
						reg.address.high = ((reg.address.high & mask) | dataIn);
						break;

					case IO_REG_LCD_COLOR :
						if(!half)
						{
							reg.lcdForegroundColor = dataInRaw;
						}
						else
						{
							reg.lcdBackgroundColor = dataInRaw;
						}
						break;

					case IO_REG_LCD_CHAR :
						reg.lcdChar = ((reg.lcdChar & mask) | dataIn);
						if(half)
						{
							lcdSetChar(reg.lcdChar, reg.address.low, reg.address.high, reg.lcdForegroundColor, reg.lcdBackgroundColor);
						}
						break;

					case IO_REG_LCD_COMMAND :
						switch(dataIn)
						{
							case IO_LCD_CLEAR :
								lcdClear();
								lcdRefresh();
								break;
							case IO_LCD_REFRESH :
								lcdRefresh();
								break;
							case IO_LCD_SCROLL :
								lcdScroll();
								break;
						}
						break;

					case IO_REG_RS232_TX :
						reg.rs232Tx = ((reg.rs232Tx & mask) | dataIn);
						if(half)
						{
							rs232Send(reg.rs232Tx);
						}
						break;

					case IO_REG_RTC_FIELD :
						switch(reg.address.low)
						{
							case IO_RTC_YEAR :
								reg.rtcField.year = ((reg.rtcField.year & mask) | dataIn);
								break;

							case IO_RTC_MONTH :
								reg.rtcField.month = ((reg.rtcField.month & mask) | dataIn);
								break;

							case IO_RTC_DAY :
								reg.rtcField.day = ((reg.rtcField.day & mask) | dataIn);
								break;

							case IO_RTC_HOUR :
								reg.rtcField.hour = ((reg.rtcField.hour & mask) | dataIn);
								break;

							case IO_RTC_MINUTE :
								reg.rtcField.minute = ((reg.rtcField.minute & mask) | dataIn);
								break;

							case IO_RTC_SECOND :
								reg.rtcField.second = ((reg.rtcField.second & mask) | dataIn);
								break;
						}
						break;

					case IO_REG_RTC_COMMAND :
						switch(dataInRaw)
						{
							case IO_RTC_READ :
								reg.rtcField = rtcGetDateTime();
								break;

							case IO_RTC_WRITE :
								rtcSetDateTime(reg.rtcField);
								break;
						}
						break;

					case IO_REG_SPEAKER_FIELD :
						switch(reg.address.low)
						{
							case IO_SPEAKER_NOTE :
								reg.speakerField.note = ((reg.speakerField.note & mask) | dataIn);
								break;

							case IO_SPEAKER_TIME :
								reg.speakerField.time = ((reg.speakerField.time & mask) | dataIn);
								break;

							case IO_SPEAKER_FILL :
								reg.speakerField.fill = dataInRaw;
								break;

							case IO_SPEAKER_VOLUME :
								reg.speakerField.volume = dataInRaw;
								break;
						}
						break;

					case IO_REG_SPEAKER_COMMAND :
						switch(dataInRaw)
						{
							case IO_SPEAKER_CLEAR :
								speakerClear();
								break;

							case IO_SPEAKER_ADD_NOTE :
								speakerAddNote(reg.speakerField);
								break;
						}
						break;

					case IO_REG_FS_NAME :
						if(reg.address.low < FS_NAME_MAX_LENGTH)
						{
							reg.fsName.data[reg.address.low] = ((reg.fsName.data[reg.address.low] & mask) | dataIn);
						}
						break;

					case IO_REG_FS_COMMAND :
						switch(dataInRaw)
						{
							case IO_FS_READ_BLOCK :
								fsReadBlock(&(reg.fsBlock), reg.address.low);
								break;
						}
						break;
				}
			}
		}
		
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
		
		// Operation is completed
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
	}
}
