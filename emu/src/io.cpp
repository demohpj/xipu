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

#include "io.h"

/**
 * Constructor for the IO class. It connects all communication classes for IO operations.
 *
 * @param parent Parent object
 */
IO::IO(QObject *parent) : QObject(parent)
{
	this->reset();

	QObject::connect(&this->led, SIGNAL(updateRunSignal(bool)), this, SIGNAL(updateLEDRunSignal(bool)));
	QObject::connect(&this->led, SIGNAL(updateErrorSignal(bool)), this, SIGNAL(updateLEDErrorSignal(bool)));

	QObject::connect(&this->lcd, SIGNAL(updateBufferSignal(LCD::Buffer)), this, SIGNAL(updateLCDBufferSignal(LCD::Buffer)));

	QObject::connect(&this->rs232, SIGNAL(updateTxSignal(unsigned char)), this, SIGNAL(updateRS232TxSignal(unsigned char)));
	QObject::connect(&this->rs232, SIGNAL(updateRxSignal(unsigned char)), this, SIGNAL(updateRS232RxSignal(unsigned char)));

	QObject::connect(&this->rtc, SIGNAL(updateDateTimeSignal(QDateTime)), this, SIGNAL(updateRTCDateTimeSignal(QDateTime)));

	QObject::connect(&this->speaker, SIGNAL(updateStatusSignal(int)), this, SIGNAL(updateSpeakerStatusSignal(int)));

	QObject::connect(&this->keyboard, SIGNAL(dataReadySignal(bool)), this, SLOT(keyboardDataReadySlot(bool)));
	QObject::connect(&this->rs232, SIGNAL(dataReadySignal(bool)), this, SLOT(rs232DataReadySlot(bool)));
}

//! Destructor for the IO class
IO::~IO()
{
	QObject::disconnect(this);
}

//! Reset the IO buffers and trigger reset for all connected communication classes
void IO::reset()
{
	this->regSelected = 0;
	this->reg = {};
	this->in = 0;

	this->keyboard.reset();
	this->led.reset();
	this->lcd.reset();
	this->rs232.reset();
	this->rtc.reset();
	this->speaker.reset();
	this->fs.reset();
}

/**
 * Process a key press event
 *
 * @param key Pressed key code
 * @param modifiers Optional modifiers to pressed key
 */
void IO::keyboardKeyPress(int key, Qt::KeyboardModifiers modifiers)
{
	this->keyboard.keyPress(key, modifiers);
}

/**
 * Set enable status of the run LED
 *
 * @param enable Enable status
 */
void IO::ledSetRun(bool enable)
{
	this->led.setRun(enable);
}

/**
 * Set enable status of the error LED
 *
 * @param enable Enable status
 */
void IO::ledSetError(bool enable)
{
	this->led.setError(enable);
}

/**
 * Process a received text event
 *
 * @param text Received text
 */
void IO::rs232Receive(const QString &text)
{
	this->rs232.rx(text);
}

/**
 * Set date and time
 *
 * @param dateTime Date and time to set in own RTC format
 */
void IO::rtcSetDateTime(const QDateTime &dateTime)
{
	this->rtc.setDateTime(dateTime);
}

/**
 * Set volume
 *
 * @param volume Volume level in range 0-100
 */
void IO::speakerSetVolume(unsigned int volume)
{
	this->speaker.setVolume(volume);
}

/**
 * Set path to the emulated file system on the local disk
 *
 * @param path Path to emulated file system
 */
void IO::fsSetPath(const QString &path)
{
	this->fs.setPath(path);
}

/**
 * Process "keyboard data is ready to get" event
 *
 * @param ready Status of ready data to get
 */
void IO::keyboardDataReadySlot(bool ready)
{
	if(ready)
	{
		this->in |= IN_KEYBOARD_READY_BIT;
	}
	else
	{
		this->in &= ~IN_KEYBOARD_READY_BIT;
	}

	emit inSignal(this->in);
}

/**
 * Process "RS232 receive data is ready to get" event
 *
 * @param ready Status of ready data to get
 */
void IO::rs232DataReadySlot(bool ready)
{
	if(ready)
	{
		this->in |= IN_RS232_READY_BIT;
	}
	else
	{
		this->in &= ~IN_RS232_READY_BIT;
	}

	emit inSignal(this->in);
}

/**
 * Process "update the output register value" event.
 * Fill up the output register data half with corresponding data from selected virtual register.
 *
 * @param out Output register value with set up data of the flags half
 */
void IO::outSlot(unsigned char out)
{
	if((out & OUT_DATA_READY_BIT) != 0)
	{
		unsigned char value = out;

		bool half = ((value & OUT_HALF_BIT) != 0);
		bool mode = ((value & OUT_MODE_BIT) != 0);
		bool rw = ((value & OUT_RW_BIT) != 0);

		if(!mode)
		{
			// Select register to work
			regSelected = (value & REG_SELECT_MASK);
		}
		else
		{
			if(!rw)
			{
				unsigned char dataOut = 0x00;

				// Read register value
				switch(regSelected)
				{
					case REG_KEYBOARD :
						{
							if(!half)
							{
								this->reg.keyboard = this->keyboard.getKey();
							}

							dataOut = this->reg.keyboard;
						}
						break;

					case REG_RS232_RX :
						{
							if(!half)
							{
								this->reg.rs232Rx = this->rs232.receive();
							}

							dataOut = this->reg.rs232Rx;
						}
						break;

					case REG_RTC_FIELD :
						switch(this->reg.address.low)
						{
							case RTC_YEAR :
								dataOut = this->reg.rtcField.year;
								break;

							case RTC_MONTH :
								dataOut = this->reg.rtcField.month;
								break;

							case RTC_DAY :
								dataOut = this->reg.rtcField.day;
								break;

							case RTC_HOUR :
								dataOut = this->reg.rtcField.hour;
								break;

							case RTC_MINUTE :
								dataOut = this->reg.rtcField.minute;
								break;

							case RTC_SECOND :
								dataOut = this->reg.rtcField.second;
								break;

							default :
								dataOut = 0;
								break;
						}
						break;

					case REG_SPEAKER_FIELD :
						{
							if(!half)
							{
								this->reg.speakerBufferFree = this->speaker.getBufferFree();
							}

							dataOut = this->reg.speakerBufferFree;
						}
						break;

					case REG_FS_DATA :
						{
							dataOut = this->reg.fsBlock.data[this->reg.address.low];

							if(half)
							{
								this->reg.address.low++;
							}
						}
						break;

					case REG_FS_COMMAND :
						switch(this->reg.address.low)
						{
							case FS_OPEN_APP :
								{
									this->reg.fsName.data[FS::NAME_MAX_LENGTH] = 0;

									if(this->fs.open("app", this->reg.fsName, this->reg.fsSize))
									{
										dataOut = 1;
									}
									else
									{
										dataOut = 0;
									}
								}
								break;

							case FS_OPEN_FILE :
								{
									this->reg.fsName.data[FS::NAME_MAX_LENGTH] = 0;

									if(this->fs.open("file", this->reg.fsName, this->reg.fsSize))
									{
										dataOut = 1;
									}
									else
									{
										dataOut = 0;
									}
								}
								break;

							case FS_LIST_APP :
								{
									if(this->fs.list("app", this->reg.fsSize))
									{
										dataOut = 1;
									}
									else
									{
										dataOut = 0;
									}
								}
								break;

							case FS_LIST_FILE :
								{
									if(this->fs.list("file", this->reg.fsSize))
									{
										dataOut = 1;
									}
									else
									{
										dataOut = 0;
									}
								}
								break;

							case FS_SIZE :
								switch(this->reg.address.high)
								{
									case 0 :
										dataOut = this->reg.fsSize.low;
										break;

									case 1 :
										dataOut = this->reg.fsSize.high;
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
				this->in = ((this->in & IN_CONTROL_MASK) | (dataOut & IN_DATA_MASK));
			}
			else
			{
				unsigned char dataInRaw = (value & IN_DATA_MASK);
				unsigned char dataIn = dataInRaw;
				unsigned char mask = HALF_UP_DATA_MASK;

				if(half)
				{
					dataIn <<= HALF_DATA_OFFSET;
					mask >>= HALF_DATA_OFFSET;
				}

				// Write register value
				switch(this->regSelected)
				{
					case REG_LED_COMMAND :
						switch(dataInRaw)
						{
							case LED_RUN_OFF :
								this->led.setRun(false);
								break;
							case LED_RUN_ON :
								this->led.setRun(true);
								break;
							case LED_ERROR_OFF :
								this->led.setError(false);
								break;
							case LED_ERROR_ON :
								this->led.setError(true);
								break;
						}
						break;

					case REG_ADDRESS_LOW :
						this->reg.address.low = ((this->reg.address.low & mask) | dataIn);
						break;

					case REG_ADDRESS_HIGH :
						this->reg.address.high = ((this->reg.address.high & mask) | dataIn);
						break;

					case REG_LCD_COLOR :
						if(!half)
						{
							this->reg.lcdForegroundColor = dataInRaw;
						}
						else
						{
							this->reg.lcdBackgroundColor = dataInRaw;
						}
						break;

					case REG_LCD_CHAR :
						this->reg.lcdChar = ((this->reg.lcdChar & mask) | dataIn);
						if(half)
						{
							this->lcd.setChar(this->reg.lcdChar, this->reg.address.low, this->reg.address.high, this->reg.lcdForegroundColor, this->reg.lcdBackgroundColor);
						}
						break;

					case REG_LCD_COMMAND :
						switch(dataIn)
						{
							case LCD_CLEAR :
								this->lcd.clear();
								this->lcd.refresh();
								this->lcd.update();
								break;
							case LCD_REFRESH :
								this->lcd.refresh();
								this->lcd.update();
								break;
							case LCD_SCROLL :
								this->lcd.scroll();
								break;
						}
						break;

					case REG_RS232_TX :
						this->reg.rs232Tx = ((this->reg.rs232Tx & mask) | dataIn);
						if(half)
						{
							this->rs232.send(this->reg.rs232Tx);
						}
						break;

					case REG_RTC_FIELD :
						switch(this->reg.address.low)
						{
							case RTC_YEAR :
								this->reg.rtcField.year = ((this->reg.rtcField.year & mask) | dataIn);
								break;

							case RTC_MONTH :
								this->reg.rtcField.month = ((this->reg.rtcField.month & mask) | dataIn);
								break;

							case RTC_DAY :
								this->reg.rtcField.day = ((this->reg.rtcField.day & mask) | dataIn);
								break;

							case RTC_HOUR :
								this->reg.rtcField.hour = ((this->reg.rtcField.hour & mask) | dataIn);
								break;

							case RTC_MINUTE :
								this->reg.rtcField.minute = ((this->reg.rtcField.minute & mask) | dataIn);
								break;

							case RTC_SECOND :
								this->reg.rtcField.second = ((this->reg.rtcField.second & mask) | dataIn);
								break;
						}
						break;

					case REG_RTC_COMMAND :
						switch(dataInRaw)
						{
							case RTC_READ :
								this->reg.rtcField = this->rtc.getDateTime();
								break;

							case RTC_WRITE :
								this->rtc.setDateTime(this->reg.rtcField);
								break;
						}
						break;

					case REG_SPEAKER_FIELD :
						switch(this->reg.address.low)
						{
							case SPEAKER_NOTE :
								this->reg.speakerField.note = ((this->reg.speakerField.note & mask) | dataIn);
								break;

							case SPEAKER_TIME :
								this->reg.speakerField.time = ((this->reg.speakerField.time & mask) | dataIn);
								break;

							case SPEAKER_FILL :
								this->reg.speakerField.fill = dataInRaw;
								break;

							case SPEAKER_VOLUME :
								this->reg.speakerField.volume = dataInRaw;
								break;
						}
						break;

					case REG_SPEAKER_COMMAND :
						switch(dataInRaw)
						{
							case SPEAKER_CLEAR :
								this->speaker.clear();
								break;

							case SPEAKER_ADD_NOTE :
								this->speaker.addNote(this->reg.speakerField);
								break;
						}
						break;

					case REG_FS_NAME :
						if(this->reg.address.low < FS::NAME_MAX_LENGTH)
						{
							this->reg.fsName.data[this->reg.address.low] = ((this->reg.fsName.data[this->reg.address.low] & mask) | dataIn);
						}
						break;

					case REG_FS_COMMAND :
						switch(dataInRaw)
						{
							case FS_READ_BLOCK :
								this->fs.readBlock(this->reg.fsBlock, this->reg.address.low);
								break;
						}
						break;
				}
			}
		}

		// Operation is completed
		this->in ^= IN_OPERATION_COMPLETE_BIT;

		emit inSignal(this->in);
	}
}
