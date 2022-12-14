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

#include "instruction.h"

/**
 * Constructor using for create an instruction with a defined opcode
 *
 * @param name Name of the instruction
 * @param opcode Opcode number
 * @param arg0 First argument type
 * @param arg1 Second argument type
 * @param arg2 Third argument type
 * @param opcodeSecond Second optional opcode number
 * @param arg0Second First argument type of the second optional opcode
 * @param arg1Second Second argument type of the second optional opcode
 * @param arg2Second Third argument type of the second optional opcode
 */
Instruction::Instruction(const QString &name, unsigned char opcode, Instruction::Arg arg0, Instruction::Arg arg1, Instruction::Arg arg2, unsigned char opcodeSecond, Arg arg0Second, Arg arg1Second, Arg arg2Second)
{
	this->name = name;
	this->opcode = opcode;

	this->argList.append(arg0);
	this->argList.append(arg1);
	this->argList.append(arg2);

	this->opcodeSecond = opcodeSecond;

	this->argSecondList.append(arg0Second);
	this->argSecondList.append(arg1Second);
	this->argSecondList.append(arg2Second);
}

/**
 * Get the name of the instruction
 *
 * @return Name of the instruction
 */
const QString &Instruction::getName() const
{
	return(this->name);
}

/**
 * Get the opcode number of the instruction
 *
 * @return Opcode of the instruction
 */
unsigned char Instruction::getOpcode() const
{
	return(this->opcode);
}

/**
 * Get the argument type list of the instruction
 *
 * @return Argument type list
 */
const QList<Instruction::Arg> &Instruction::getArgList() const
{
	return(this->argList);
}

/**
 * Get the second optional opcode number of the instruction
 *
 * @return Second optional opcode of the instruction
 */
unsigned char Instruction::getOpcodeSecond() const
{
	return(this->opcodeSecond);
}

/**
 * Get the second optional argument type list of the instruction
 *
 * @return Second optional argument type list
 */
const QList<Instruction::Arg> &Instruction::getArgSecondList() const
{
	return(this->argSecondList);
}

