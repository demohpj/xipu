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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QString>
#include <QList>

//! This class contains a one description of the CPU instruction
class Instruction
{
	public:
		//! Definitions of the instruction argument types
		enum class Arg
		{
			None, //!< No argument
			ABXY, //!< Main and auxiliary registers
			AB, //!< Only main registers
			Val256, //!< 8 bit unsigned value
			Addr //!< 16 bit unsigned address
		};

		Instruction(const QString &name, unsigned char opcode, Arg arg0 = Arg::None, Arg arg1 = Arg::None, Arg arg2 = Arg::None, unsigned char opcodeSecond = 0, Arg arg0Second = Arg::None, Arg arg1Second = Arg::None, Arg arg2Second = Arg::None);

		const QString &getName() const;
		unsigned char getOpcode() const;
		const QList<Arg> &getArgList() const;

		unsigned char getOpcodeSecond() const;
		const QList<Arg> &getArgSecondList() const;

	private:
		QString name; //!< Instruction name
		unsigned char opcode; //!< Opcode of the defined instruction
		QList<Arg> argList; //!< Argument list of the defined instruction

		unsigned char opcodeSecond; //!< Second optional opcode of the defined instruction used for a two-step instruction type
		QList<Arg> argSecondList; //!< Second optional argument list of the defined instruction used for a two-step instruction type
};

#endif
