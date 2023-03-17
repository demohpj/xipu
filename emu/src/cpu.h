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

#ifndef CPU_H
#define CPU_H

#include <QObject>
#include <QVector>
#include <QTimer>

#include "io.h"

//! This class contains CPU contex and functions
class CPU : public QObject
{
	Q_OBJECT

	public:
		static const int FREQUENCY = 1000000; //!< Base frequency of CPU

		static const int INTERVAL = 125; //!< Time in milliseconds between every step of the emulation
		static const int TICKS_PER_INTERVAL = ((FREQUENCY * INTERVAL) / 1000); //!< How many clock ticks of CPU will be processed in a single step of the emulation

		static const int CLOCK_INTERVAL = 125; //!< How many milliseconds is needed to toggle the clock pin status on the IO BUS. Default frequency is 8 Hz
		static const int CLOCK_TICKS_PER_INTERVAL = ((FREQUENCY * CLOCK_INTERVAL) / 1000); //!< How many clock ticks of CPU is needed to toggle the clock pin status on the IO BUS.

		static const int MEMORY_PAGE_SIZE = 256; //!< Memory page size
		static const int MEMORY_PAGE_QUANTITY = 128; //!< Quantity of pages in one memory block
		static const int MEMORY_SIZE = (MEMORY_PAGE_SIZE * MEMORY_PAGE_QUANTITY); //!< Size of the memory block

		static const int ROM_OS_ADDRESS = 0x0000; //!< Start of ROM OS address space
		static const int ROM_APP_ADDRESS = 0x8000; //!< Start of ROM Application address space

		static const int RAM_OS_ADDRESS = 0x0000; //!< Start of RAM OS address space
		static const int RAM_SP_ADDRESS = 0x0f00; //!< Start of RAM Stack Pointer address space
		static const int RAM_APP_ADDRESS = 0x1000; //!< Start of RAM Application address space

		static const int STACK_PAGE = 0x0f; //!< Page number of the stack

		static const int UROM_ADDRESS_CYCLE_MASK = 0x0f; //!< Micro-step mask of the CPU step for uROM

		static const int UROM_ADDRESS_INSTRUCTION_POSITION = 0; //!< Offset of instruction for uROM
		static const int UROM_ADDRESS_CYCLE_POSITION = 8; //!< Offset of micro-step for uROM
		static const int UROM_ADDRESS_FLAG_C_POSITION = 12; //!< Offset of C Flag for uROM
		static const int UROM_ADDRESS_FLAG_Z_POSITION = 13; //!< Offset of Z Flag for uROM

		static const int UROM_0_BUS_AR_MASK = 0b00000111; //!< BUS A Read mask for first uROM bank
		static const int UROM_0_BUS_AW_MASK = 0b01111000; //!< BUS A Write mask for first uROM bank
		static const int UROM_0_BUS_B_MASK = 0b10000000; //!< BUS B mask for first uROM bank

		static const int UROM_0_BUS_AR_POSITION = 0; //!< BUS A Read offset for first uROM bank
		static const int UROM_0_BUS_AW_POSITION = 3; //!< BUS A Write offset for first uROM bank
		static const int UROM_0_BUS_B_POSITION = 7; //!< BUS B offset for first uROM bank

		static const int UROM_1_BUS_C_MASK = 0b00000011; //!< BUS C mask for second uROM bank
		static const int UROM_1_ALU_S_MASK = 0b00111100; //!< ALU S Flags mask for second uROM bank
		static const int UROM_1_ALU_M_MASK = 0b01000000; //!< ALU M Flag mask for second uROM bank
		static const int UROM_1_ALU_C_MASK = 0b10000000; //!< ALU C Flag mask for second uROM bank

		static const int UROM_1_BUS_C_POSITION = 0; //!< BUS C offset for second uROM bank
		static const int UROM_1_ALU_S_POSITION = 2; //!< ALU S Flags offset for second uROM bank

		static const int INSTRUCTION_REG_ABXY_FIRST_MASK = 0b00000011; //!< First argument ABXY Register mask for opcode
		static const int INSTRUCTION_REG_ABXY_SECOND_MASK = 0b00001100; //!< Second argument ABXY Register mask for opcode
		static const int INSTRUCTION_REG_AB_MASK = 0b00000100; //!< Second argument AB Register mask for opcode

		static const int INSTRUCTION_REG_ABXY_FIRST_OFFSET = 0; //!< First argument ABXY Register offset for opcode
		static const int INSTRUCTION_REG_ABXY_SECOND_OFFSET = 2; //!< Second argument ABXY Register offset for opcode
		static const int INSTRUCTION_REG_AB_OFFSET = 2; //!< Second argument AB Register offset for opcode

		static const int ALU_4BIT_MASK = 0x0f; //!< ALU Input/Output mask for lower and higher part of byte
		static const int ALU_4BIT_OFFSET = 4; //!< ALU Input/Output offset for lower and higher part of byte

		//! Memory buffer
		struct Memory
		{
			QVector<unsigned char> data = QVector<unsigned char>(MEMORY_SIZE); //!< Memory data
		};

		//! Register buffer
		struct Reg
		{
			unsigned char i; //!< Instruction register
			bool c; //!< Carry Flag register
			bool z; //!< Zero Flag register
			unsigned char pch; //!< Program Counter High register
			unsigned char pcl; //!< Program Counter Low register
			unsigned char sph; //!< Stack Pointer High register
			unsigned char spl; //!< Stack Pointer Low register
			unsigned char maxSpl; //!< Maximum reached value of Stack Pointer
			unsigned char a; //!< A register
			unsigned char b; //!< B register
			unsigned char x; //!< X register
			unsigned char y; //!< Y rRegister
			unsigned char in; //!< Input register
			unsigned char out; //!< Output register
			unsigned char d; //!< Hidden Data register
			unsigned char t; //!< ALU Output register
			unsigned char mah; //!< Memory Address High register
			unsigned char mal; //!<Memory Address Low register
		};

		CPU(QObject *parent = nullptr);

		void setUrom0(const CPU::Memory &urom);
		void setUrom1(const CPU::Memory &urom);
		void setRom(const CPU::Memory &rom);

		void run();
		void step();
		void pause();
		void stop();

		const CPU::Reg &getReg() const;
		unsigned long long getTicks() const;

		const CPU::Memory &getRom() const;
		const CPU::Memory &getRamLow() const;
		const CPU::Memory &getRamHigh() const;

	private:
		//! Line select defines for the main reading data bus
		enum class BusAR
		{
			ABXY = 0, //!< Main and auxiliary registers
			D, //!< Hidden data register
			IN, //!< Input register
			T, //!< ALU temp register
			Ram, //!< RAM access
			Flash, //!< FLASH access
			PCL, //!< Program Counter Low register
			PCH //!< Program Counter High register
		};

		//! Line select defines for the main writing data bus
		enum class BusAW
		{
			None = 0, //!< Not selected
			ABXY, //!< Main and auxiliary registers
			D, //!< Hidden data register
			OUT, //!< Output register
			ALU_T, //!< ALU operation
			RPC, //!< Reset Program Counter
			I, //!< Instruction register
			Ram, //!< RAM access
			PCL, //!< Program Counter Low register
			PCH, //!< Program Counter High register
			MAL, //!< Memory Address Low register
			MAH, //!< Memory Address High register
			PC_PLUS, //!< Increse Program Counter
			SP_PLUS, //!< Increse Stack Pointer
			SP_MINUS, //!< Decrese Stack Pointer
			RPC_PLUS //!< Reset the step counter and increse Program Counter
		};

		//! Line select defines for the secondary reading data bus
		enum class BusB
		{
			AB = 0, //!< Main registers
			D //!< Hidden data register
		};

		//! Line select defines for the addressing data bus
		enum class BusC
		{
			PC = 0, //!< Full 16 bit Program Counter register
			MA, //!< Full 16 bit Memory Address register
			SP, //!< Full 16 bit Stack Pointer register
			XY //!< Full 16 bit auxiliary register
		};

		void reset();

		unsigned char alu(unsigned char a, unsigned char b, unsigned char s, bool m, bool &c, bool &z);

		bool stepMode; //!< Step mode enabler for emulation
		unsigned long long ticks; //!< Counter of clock ticks of CPU
		QTimer timer; //!< Timer for executing emulation steps

		Memory urom0; //!< First uROM memory buffer
		Memory urom1; //!< Second uROM memory buffer
		Memory rom; //!< ROM memory buffer

		Memory ramLow; //!< Lower 32 kB of RAM buffer
		Memory ramHigh; //!< Higher 32 kB of RAM buffer

		Reg reg; //!< Register buffer

	signals:
		void outSignal(unsigned char out);
		void updateSignal();

	public slots:
		void inSlot(unsigned char in);

	private slots:
		void emulation();
};

#endif
