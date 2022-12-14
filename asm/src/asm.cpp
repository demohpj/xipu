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

#include "asm.h"

//! Constructor for assembler start functions
Asm::Asm()
{
}

/**
 * Compile the kernel
 *
 * @param inputFile Name of the input source file
 * @param outputFile Name of the output binary file
 * @param outputMapFile Name of the output map file
 *
 * @return Status of compilation
 */
bool Asm::compileRom(const QString &inputFile, const QString &outputFile, const QString &outputMapFile)
{
	return(this->compile(DataList::Mode::Rom, inputFile, outputFile, outputMapFile));
}

/**
 * Compile the application
 *
 * @param inputFile Name of the input source file
 * @param outputFile Name of the output binary file
 *
 * @return Status of compilation
 */
bool Asm::compileApp(const QString &inputFile, const QString &outputFile)
{
	return(this->compile(DataList::Mode::App, inputFile, outputFile));
}

/**
 * Compile the source
 *
 * @param mode Mode of compilation
 * @param inputFile Name of the input source file
 * @param outputFile Name of the output binary file
 * @param outputMapFile Name of the output map file
 *
 * @return Status of compilation
 */
bool Asm::compile(DataList::Mode mode, QString inputFile,  QString outputFile,  QString outputMapFile)
{
	QTextStream out(stdout);

	DataList dataList(mode);
	Parser parser(dataList);

	if(!parser.parseFile(inputFile))
	{
		out << "ERROR: Parsing error\n\n";
		return(-1);
	}

	Compiler compiler(dataList);
	this->initInstructions(compiler);

	if(!compiler.compile())
	{
		out << "ERROR: Compilation error\n\n";
		return(-1);
	}

	Linker linker(dataList);

	if(!linker.link())
	{
		out << "ERROR: Linking error\n\n";
		return(-1);
	}

	if(!linker.saveFile(outputFile, outputMapFile))
	{
		out << "ERROR: Failed to save file\n\n";
		return(-1);
	}

	out << "OK: The file was saved successfully\n\n";
	return(0);
}

/**
 * Init the instruction list for the compiler
 *
 * @param compiler Compiler to which the instructions will be added
 */
void Asm::initInstructions(Compiler &compiler)
{
	// LOAD_I + RPC+1
	compiler.addInstruction(Instruction("nop", 0b00000000));

	// LOAD_I + RPC
	compiler.addInstruction(Instruction("halt", 0b11111111));

	// LOAD_I + STORE + RPC+1
	compiler.addInstruction(Instruction("ldf", 0b00000100, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("ldr", 0b00001000, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("str", 0b00001100, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("mov", 0b10000000, Instruction::Arg::ABXY, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("in", 0b00010000, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("out", 0b00010100, Instruction::Arg::ABXY));

	// LOAD_I + ALU + RPC+1
	compiler.addInstruction(Instruction("cmp", 0b11100000, Instruction::Arg::AB, Instruction::Arg::ABXY));

	// LOAD_I + ALU + STORE + RPC+1
	compiler.addInstruction(Instruction("inc", 0b00011000, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("incc", 0b01110000, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("dec", 0b00011100, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("decc", 0b01110100, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("not", 0b00100000, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("add", 0b10010000, Instruction::Arg::AB, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("addc", 0b10011000, Instruction::Arg::AB, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("sub", 0b10100000, Instruction::Arg::AB, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("subc", 0b10101000, Instruction::Arg::AB, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("and", 0b10110000, Instruction::Arg::AB, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("or", 0b10111000, Instruction::Arg::AB, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("xor", 0b11000000, Instruction::Arg::AB, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("shl", 0b01111000, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("clr", 0b11001000, Instruction::Arg::ABXY));

	// LOAD_I + 8x ALU + STORE + RPC+1
	compiler.addInstruction(Instruction("shr", 0b01111100, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("swap", 0b01101100, Instruction::Arg::ABXY));

	// LOAD_I + PC+1 + STORE + RPC+1
	compiler.addInstruction(Instruction("mov", 0b00100100, Instruction::Arg::Val256, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("out", 0b00000010, Instruction::Arg::Val256));

	// LOAD_I + PC+1 + LOAD_D + STORE + RPC+1
	compiler.addInstruction(Instruction("str", 0b00000001, Instruction::Arg::Val256));

	// LOAD_I + PC+1 + LOAD_D + ALU + RPC+1
	compiler.addInstruction(Instruction("cmp", 0b00101000, Instruction::Arg::Val256, Instruction::Arg::ABXY));

	// LOAD_I + PC+1 + LOAD_D + ALU + STORE + RPC+1
	compiler.addInstruction(Instruction("add", 0b00101100, Instruction::Arg::Val256, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("addc", 0b00110000, Instruction::Arg::Val256, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("sub", 0b00110100, Instruction::Arg::Val256, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("subc", 0b00111000, Instruction::Arg::Val256, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("and", 0b00111100, Instruction::Arg::Val256, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("or", 0b01000000, Instruction::Arg::Val256, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("xor", 0b01000100, Instruction::Arg::Val256, Instruction::Arg::ABXY));

	// LOAD_I + PC+1 + LOAD_MAL + PC+1 + LOAD_MAH + STORE + RPC+1
	compiler.addInstruction(Instruction("ldf", 0b01011000, Instruction::Arg::Addr, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("ldr", 0b01011100, Instruction::Arg::Addr, Instruction::Arg::ABXY));
	compiler.addInstruction(Instruction("str", 0b01100000, Instruction::Arg::ABXY, Instruction::Arg::Addr));

	// LOAD_I + PC+1 + LOAD_D +  PC+1 + LOAD_MAL + PC+1 + LOAD_MAH + STORE + RPC+1
	compiler.addInstruction(Instruction("str", 0b00000011, Instruction::Arg::Val256, Instruction::Arg::Addr));

	// LOAD_I + LOAD + LOAD + RPC+1 + LOAD_I + LOAD + RPC+1
	compiler.addInstruction(Instruction("xchg", 0b11010000, Instruction::Arg::ABXY, Instruction::Arg::ABXY, Instruction::Arg::None, 0b11001100, Instruction::Arg::ABXY));

	// LOAD_I + PC+1 + LOAD_MAL + PC+1 + LOAD_MAH + 3x STORE + RPC+1
	compiler.addInstruction(Instruction("xchg", 0b01001000, Instruction::Arg::Addr, Instruction::Arg::ABXY));

	// LOAD_I + PC+1 + LOAD_PCL + PC+1 + LOAD_PCH + STORE + RESET
	compiler.addInstruction(Instruction("jmp", 0b11110000, Instruction::Arg::Addr));

	// LOAD_I + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + RESET
	//		  + PC+1 + PC+1 + RPC+1
	compiler.addInstruction(Instruction("je", 0b11111000, Instruction::Arg::Addr));
	compiler.addInstruction(Instruction("jne", 0b11111001, Instruction::Arg::Addr));
	compiler.addInstruction(Instruction("jg", 0b11111010, Instruction::Arg::Addr));
	compiler.addInstruction(Instruction("jge", 0b11111011, Instruction::Arg::Addr));
	compiler.addInstruction(Instruction("jl", 0b11111100, Instruction::Arg::Addr));
	compiler.addInstruction(Instruction("jle", 0b11111101, Instruction::Arg::Addr));

	// LOAD_I + STORE + SP-1 + RPC+1
	compiler.addInstruction(Instruction("push", 0b01100100, Instruction::Arg::ABXY));

	// LOAD_I + SP+1 + STORE + RPC+1
	compiler.addInstruction(Instruction("pop", 0b01101000, Instruction::Arg::ABXY));

	// LOAD_I + STORE + SP-1 + STORE + SP-1 + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + RESET
	compiler.addInstruction(Instruction("call", 0b11110001, Instruction::Arg::Addr));

	// LOAD_I + SP+1 + STORE + SP+1 + STORE + RESET
	compiler.addInstruction(Instruction("ret", 0b11110010));

	// LOAD_I + ALU + PC+1 + PC+1 + RPC+1
	//              + ALU + STORE + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + RESET
	compiler.addInstruction(Instruction("loope", 0b11101000, Instruction::Arg::AB, Instruction::Arg::ABXY, Instruction::Arg::Addr));

	// LOAD_I + ALU + STORE + PC+1 + PC+1 + PC+1 + RPC+1
	//                             + ALU + STORE + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + RESET
	compiler.addInstruction(Instruction("loope", 0b01001100, Instruction::Arg::Val256, Instruction::Arg::ABXY, Instruction::Arg::Addr));

	// LOAD_I + ALU STORE + PC+1 + PC+1 + RPC+1
	//                    + ALU + STORE + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + RESET
	compiler.addInstruction(Instruction("loopz", 0b11110100, Instruction::Arg::ABXY, Instruction::Arg::Addr));
}
