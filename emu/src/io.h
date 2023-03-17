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

#ifndef IO_H
#define IO_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QDateTime>
#include <QThread>
#include <QCoreApplication>

#include "keyboard.h"
#include "led.h"
#include "lcd.h"
#include "rs232.h"
#include "rtc.h"
#include "speaker.h"
#include "fs.h"

//! This class contains IO functions. It is a motherboard simulation part for the CPU.
class IO : public QObject
{
	Q_OBJECT

	public:
		static const unsigned char IN_DATA_MASK = 0x0f; //!< Input data mask for the data part of the BUS
		static const unsigned char IN_CONTROL_MASK = 0xf0; //!< Input data mask for the control bits part of the BUS

		static const unsigned char IN_CLOCK_BIT = (1 << 4); //!< Input clock control bit
		static const unsigned char IN_KEYBOARD_READY_BIT = (1 << 5); //!< Input "keyboard has data to read" control bit
		static const unsigned char IN_RS232_READY_BIT = (1 << 6); //!< Input "RS232 has data to read" control bit
		static const unsigned char IN_OPERATION_COMPLETE_BIT = (1 << 7); //!< Input operation completed control bit

		static const unsigned char OUT_DATA_MASK = 0x0f; //!< Output data mask for the data part of the BUS
		static const unsigned char OUT_CONTROL_MASK = 0xf0; //!< Output data mask for the control bits part of the BUS

		static const unsigned char OUT_HALF_BIT = (1 << 4); //!< Output "half selector of the byte" control bit
		static const unsigned char OUT_MODE_BIT = (1 << 5); //!< Output "register selection or data transision mode" control bit
		static const unsigned char OUT_RW_BIT = (1 << 6); //!< Output read/write control bit
		static const unsigned char OUT_DATA_READY_BIT = (1 << 7); //!< Input "data is ready to read from output" control bit

		static const unsigned char REG_SELECT_MASK = 0x0f; //!< Register selection mask

		static const unsigned char HALF_UP_DATA_MASK = 0xf0; //!< Upper half of the byte mask
		static const int HALF_DATA_OFFSET = 4; //!< Upper half of the byte offset

		IO(QObject *parent = nullptr);
		~IO() override;

		IO(const IO &) = delete;
		IO &operator=(const IO &) = delete;
		IO(IO &&) = delete;
		IO &operator=(IO &&) = delete;

		void reset();

		void keyboardKeyPress(int key, Qt::KeyboardModifiers modifiers);
		void ledSetRun(bool enable);
		void ledSetError(bool enable);
		void rs232Receive(const QString &text);
		void rtcSetDateTime(const QDateTime &dateTime);
		void speakerSetVolume(unsigned int volume);
		void fsSetPath(const QString &path);

	private:
		//!< List of motherboard's virtual registers
		enum RegAddress
		{
			REG_KEYBOARD = 0, //!< Keyboard register
			REG_LED_COMMAND = 1, //!< LED register
			REG_ADDRESS_LOW = 2, //!< Lower address register
			REG_ADDRESS_HIGH = 3, //!< Upper address register
			REG_LCD_COLOR = 4, //!< LCD color register
			REG_LCD_CHAR = 5, //!< LCD char register
			REG_LCD_COMMAND = 6, //!< LCD command register
			REG_RS232_RX = 7, //!< RS232 receive register
			REG_RS232_TX = 8, //!< RS232 transmit register
			REG_RTC_FIELD = 9, //!< Real Time Clock data register
			REG_RTC_COMMAND = 10, //!< Real Time Clock command register
			REG_SPEAKER_FIELD = 11, //!< Speaker data register
			REG_SPEAKER_COMMAND = 12, //!< Speaker command register
			REG_FS_NAME = 13, //!< File System name register
			REG_FS_DATA = 14, //!< File System data register
			REG_FS_COMMAND = 15 //!< File System command register
		};

		//!< List of LED commands
		enum LedCommand
		{
			LED_RUN_OFF = 0, //!< Turn off run LED
			LED_RUN_ON, //!< Turn on run LED
			LED_ERROR_OFF, //!< Turn off error LED
			LED_ERROR_ON //!< Turn on error LED
		};

		//!< List of LCD commands
		enum LcdCommand
		{
			LCD_CLEAR = 0, //!< Clear the screen
			LCD_REFRESH, //!< Refresh the screen using data from the framebuffer
			LCD_SCROLL //!< Scroll up one line up the screen
		};

		//!< List of Real Time Clock fields
		enum RtcField
		{
			RTC_YEAR = 0, //!< Year
			RTC_MONTH, //!< Month
			RTC_DAY, //!< Day
			RTC_HOUR, //!< Hour
			RTC_MINUTE, //!< Minute
			RTC_SECOND //!< Second
		};

		//!< List of Real Time Clock commands
		enum RtcCommand
		{
			RTC_READ = 0, //!< Read current date and time
			RTC_WRITE //!< Set current date and time
		};

		//!< List of Speaker fields
		enum SpeakerField
		{
			SPEAKER_NOTE = 0, //!< Note number
			SPEAKER_TIME, //!< Time to play
			SPEAKER_FILL, //!< Percentage fill of the square wave
			SPEAKER_VOLUME //!< Volume
		};

		//!< List of Speaker commands
		enum SpeakerCommand
		{
			SPEAKER_CLEAR = 0, //!< Stop playing and clear the buffer
			SPEAKER_ADD_NOTE, //!< Add note to the buffer and play it
		};

		//!< List of File System commands
		enum FSCommand
		{
			FS_OPEN_APP = 0, //!< Open an application
			FS_OPEN_FILE, //!< Open a file
			FS_LIST_APP, //!< List all applications
			FS_LIST_FILE, //!< List all files
			FS_SIZE, //!< Get size of the file
			FS_READ_BLOCK //!< Read data from the block
		};

		//!< This struct cointains a memory adddress
		struct Address
		{
			unsigned char low; //!< Lower byte of the address
			unsigned char high; //!< Upper byte of the address
		};

		unsigned char regSelected; //!< Selected virtual register number

		//!< This struct contains motherboard's virtual registers buffer
		struct
		{
			unsigned char keyboard; //!< Keyboard buffer
			Address address; //!< Address buffer
			unsigned char lcdForegroundColor; //!< LCD foreground color buffer
			unsigned char lcdBackgroundColor; //!< LCD background color buffer
			unsigned char lcdChar; //!< LCD char buffer
			unsigned char rs232Rx; //!< RS232 receive buffer
			unsigned char rs232Tx; //!< RS232 transmit bufffer
			unsigned char rs232TxBufferFree; //!< RS232 transmit free space in buffer counter
			RTC::DateTime rtcField; //!< Real Time Clock buffer
			Speaker::Note speakerField; //!< Speaker buffer
			unsigned char speakerBufferFree; //!< Speaker free space in buffer counter
			FS::Name fsName; //!< File System name buffer
			FS::Size fsSize; //!< File System size buffer
			FS::Block fsBlock; //!< File System data block buffer
		} reg;

		volatile unsigned char in; //!< Input register buffer

		Keyboard keyboard; //!< Keyboard class instance used for emulation motherboard's IO part
		LED led; //!< LED class instance used for emulation motherboard's IO part
		LCD lcd; //!< LCD class instance used for emulation motherboard's IO part
		RS232 rs232; //!< RS232 class instance used for emulation motherboard's IO part
		RTC rtc; //!< Real Time Clock class instance used for emulation motherboard's IO part
		Speaker speaker; //!< Speaker class instance used for emulation motherboard's IO part
		FS fs; //!< File System class instance used for emulation motherboard's IO part

	signals:
		void updateLEDRunSignal(bool enable);
		void updateLEDErrorSignal(bool enable);

		void updateLCDBufferSignal(LCD::Buffer buffer);

		void updateRS232TxSignal(unsigned char c);
		void updateRS232RxSignal(unsigned char c);

		void updateRTCDateTimeSignal(QDateTime dateTime);

		void updateSpeakerStatusSignal(int bufferUsed);

		void inSignal(unsigned char in);

	public slots:
		void keyboardDataReadySlot(bool ready);
		void rs232DataReadySlot(bool ready);

		void outSlot(unsigned char out);
};

#endif
