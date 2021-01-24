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

#include "urom.h"

//! Default constructor for creating an uROM. Generating a full instrution set.
URom::URom()
{
	Instruction instruction;

	// LOAD_I + RPC+1 // ======================================================

	// NOP
	// Fill whole instruction table as default
	for(unsigned int i = 0; i < URomData::INSTRUCTION_QUANTITY; i++)
	{
		instruction = Instruction(static_cast<unsigned char>(i));
		instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
		instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
		this->uRomData.addInstruction(instruction);
	}

	// LOAD_I + RPC // ========================================================

	// HALT
	instruction = Instruction(0b11111111);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + STORE + RPC+1 // ==============================================

	// LDF reg
	instruction = Instruction(0b00000100, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::ABXY, Step::BusB::Default, Step::BusC::XY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LDR reg
	instruction = Instruction(0b00001000, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Ram, Step::BusAW::ABXY, Step::BusB::Default, Step::BusC::XY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// STR reg
	instruction = Instruction(0b00001100, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::Ram, Step::BusB::Default, Step::BusC::XY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// MOV reg, reg
	instruction = Instruction(0b10000000, Instruction::Arg::ABXY, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// IN reg
	instruction = Instruction(0b00010000, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::IN, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// OUT reg
	instruction = Instruction(0b00010100, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::OUT));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + ALU + RPC+1 // ================================================

	// CMP reg, reg
	instruction = Instruction(0b11100000, Instruction::Arg::AB, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b0110, false, true));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + ALU + STORE + RPC+1 // ========================================

	// INC reg
	instruction = Instruction(0b00011000, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// INCC reg
	instruction = Instruction(0b01110000, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Disable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	instruction = Instruction(0b01110000, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Enable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// DEC reg
	instruction = Instruction(0b00011100, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1111, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// DECC reg
	instruction = Instruction(0b01110100, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Disable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1111, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	instruction = Instruction(0b01110100, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Enable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1111, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// NOT reg
	instruction = Instruction(0b00100000, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, true, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// ADD reg, reg
	instruction = Instruction(0b10010000, Instruction::Arg::AB, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b1001, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// ADDC reg, reg
	instruction = Instruction(0b10011000, Instruction::Arg::AB, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Flag::Disable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b1001, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	instruction = Instruction(0b10011000, Instruction::Arg::AB, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Flag::Enable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b1001, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// SUB reg, reg
	instruction = Instruction(0b10100000, Instruction::Arg::AB, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b0110, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// SUBC reg, reg
	instruction = Instruction(0b10101000, Instruction::Arg::AB, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Flag::Disable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b0110, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	instruction = Instruction(0b10101000, Instruction::Arg::AB, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Flag::Enable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b0110, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// AND reg, reg
	instruction = Instruction(0b10110000, Instruction::Arg::AB, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b1011, true, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// OR reg, reg
	instruction = Instruction(0b10111000, Instruction::Arg::AB, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b1110, true, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// XOR reg, reg
	instruction = Instruction(0b11000000, Instruction::Arg::AB, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b0110, true, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// SHL reg
	instruction = Instruction(0b01111000, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// CLR reg
	instruction = Instruction(0b11001000, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0011, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + 8x ALU + STORE + RPC+1 // =====================================

	// SHR reg
	instruction = Instruction(0b01111100, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Disable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	instruction = Instruction(0b01111100, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Enable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// SWAP reg
	instruction = Instruction(0b01101100, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Disable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	instruction = Instruction(0b01101100, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Enable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1100, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + STORE + RPC+1 // =======================================

	// MOV val, reg
	instruction = Instruction(0b00100100, Instruction::Arg::Val256, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// OUT val
	instruction = Instruction(0b00000010, Instruction::Arg::Val256);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::OUT));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + LOAD_D + STORE + RPC+1 // ===============================

	// STR val
	instruction = Instruction(0b00000001, Instruction::Arg::Val256);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::D, Step::BusAW::Ram, Step::BusB::Default, Step::BusC::XY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + LOAD_D + ALU + RPC+1 // ================================

	// CMP val, reg
	instruction = Instruction(0b00101000, Instruction::Arg::Val256, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b0110, false, true));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + LOAD_D + ALU + STORE + RPC+1 // ========================

	// ADD val, reg
	instruction = Instruction(0b00101100, Instruction::Arg::Val256, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b1001, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// ADDC val, reg
	instruction = Instruction(0b00110000, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Disable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b1001, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	instruction = Instruction(0b00110000, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Enable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b1001, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// SUB val, reg
	instruction = Instruction(0b00110100, Instruction::Arg::Val256, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b0110, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// SUBC val, reg
	instruction = Instruction(0b00111000, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Disable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b0110, false, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	instruction = Instruction(0b00111000, Instruction::Arg::ABXY, Instruction::Arg::None, Instruction::Arg::None, Instruction::Flag::Enable);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b0110, false, true));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// AND val, reg
	instruction = Instruction(0b00111100, Instruction::Arg::Val256, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b1011, true, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// OR val, reg
	instruction = Instruction(0b01000000, Instruction::Arg::Val256, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b1110, true, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// XOR val, reg
	instruction = Instruction(0b01000100, Instruction::Arg::Val256, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b0110, true, false));
	instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + LOAD_MAL + PC+1 + LOAD_MAH + STORE + RPC+1 // ==========

	// LDF addr, reg
	instruction = Instruction(0b01011000, Instruction::Arg::Addr, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAL));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAH));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::ABXY, Step::BusB::Default, Step::BusC::MA));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LDR addr, reg
	instruction = Instruction(0b01011100, Instruction::Arg::Addr, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAL));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAH));
	instruction.addStep(Step(Step::BusAR::Ram, Step::BusAW::ABXY, Step::BusB::Default, Step::BusC::MA));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// STR reg, addr
	instruction = Instruction(0b01100000, Instruction::Arg::ABXY, Instruction::Arg::Addr);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAL));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAH));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::Ram, Step::BusB::Default, Step::BusC::MA));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + LOAD_D +  PC+1 + LOAD_MAL + PC+1 + LOAD_MAH + STORE + RPC+1 //

	// STR val, addr
	instruction = Instruction(0b00000011, Instruction::Arg::Val256, Instruction::Arg::Addr);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAL));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAH));
	instruction.addStep(Step(Step::BusAR::D, Step::BusAW::Ram, Step::BusB::Default, Step::BusC::MA));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + LOAD + LOAD + RPC+1 + LOAD_I + LOAD + RPC+1 // ================

	// XCHG reg, reg
	instruction = Instruction(0b11010000, Instruction::Arg::ABXY, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	instruction = Instruction(0b11001100, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::D, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + LOAD_MAL + PC+1 + LOAD_MAH + 3x STORE + RPC+1 // =======

	// XCHG addr, reg
	instruction = Instruction(0b01001000, Instruction::Arg::Addr, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAL));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::MAH));
	instruction.addStep(Step(Step::BusAR::Ram, Step::BusAW::D, Step::BusB::Default, Step::BusC::MA));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::Ram, Step::BusB::Default, Step::BusC::MA));
	instruction.addStep(Step(Step::BusAR::D, Step::BusAW::ABXY));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + LOAD_PCL + PC+1 + LOAD_PCH + STORE + RESET // ============

	// JMP addr
	instruction = Instruction(0b11110000, Instruction::Arg::Addr);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
	instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + RESET // ============
	//		  + PC+1 + PC+1 + RPC+1 // ========================================

	// JE addr
	for(int c = 0; c < 2; c++)
	{
		for(int z = 0; z < 2; z++)
		{
			Instruction::Flag cFlag = (c ? Instruction::Flag::Enable : Instruction::Flag::Disable);
			Instruction::Flag zFlag = (z ? Instruction::Flag::Enable : Instruction::Flag::Disable);

			instruction = Instruction(0b11111000, Instruction::Arg::Addr, Instruction::Arg::None, Instruction::Arg::None, cFlag, zFlag);

			if(c & z)
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
				instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
			}
			else
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
			}

			this->uRomData.addInstruction(instruction);
		}
	}

	// JNE addr
	for(int c = 0; c < 2; c++)
	{
		for(int z = 0; z < 2; z++)
		{
			Instruction::Flag cFlag = (c ? Instruction::Flag::Enable : Instruction::Flag::Disable);
			Instruction::Flag zFlag = (z ? Instruction::Flag::Enable : Instruction::Flag::Disable);

			instruction = Instruction(0b11111001, Instruction::Arg::Addr, Instruction::Arg::None, Instruction::Arg::None, cFlag, zFlag);

			if(!(c & z))
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
				instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
			}
			else
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
			}

			this->uRomData.addInstruction(instruction);
		}
	}

	// JG addr
	for(int c = 0; c < 2; c++)
	{
		for(int z = 0; z < 2; z++)
		{
			Instruction::Flag cFlag = (c ? Instruction::Flag::Enable : Instruction::Flag::Disable);
			Instruction::Flag zFlag = (z ? Instruction::Flag::Enable : Instruction::Flag::Disable);

			instruction = Instruction(0b11111010, Instruction::Arg::Addr, Instruction::Arg::None, Instruction::Arg::None, cFlag, zFlag);

			if(c & !z)
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
				instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
			}
			else
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
			}

			this->uRomData.addInstruction(instruction);
		}
	}

	// JGE addr
	for(int c = 0; c < 2; c++)
	{
		for(int z = 0; z < 2; z++)
		{
			Instruction::Flag cFlag = (c ? Instruction::Flag::Enable : Instruction::Flag::Disable);
			Instruction::Flag zFlag = (z ? Instruction::Flag::Enable : Instruction::Flag::Disable);

			instruction = Instruction(0b11111011, Instruction::Arg::Addr, Instruction::Arg::None, Instruction::Arg::None, cFlag, zFlag);

			if(c)
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
				instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
			}
			else
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
			}

			this->uRomData.addInstruction(instruction);
		}
	}

	// JL addr
	for(int c = 0; c < 2; c++)
	{
		for(int z = 0; z < 2; z++)
		{
			Instruction::Flag cFlag = (c ? Instruction::Flag::Enable : Instruction::Flag::Disable);
			Instruction::Flag zFlag = (z ? Instruction::Flag::Enable : Instruction::Flag::Disable);

			instruction = Instruction(0b11111100, Instruction::Arg::Addr, Instruction::Arg::None, Instruction::Arg::None, cFlag, zFlag);

			if(!c & !z)
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
				instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
			}
			else
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
			}

			this->uRomData.addInstruction(instruction);
		}
	}

	// JLE addr
	for(int c = 0; c < 2; c++)
	{
		for(int z = 0; z < 2; z++)
		{
			Instruction::Flag cFlag = (c ? Instruction::Flag::Enable : Instruction::Flag::Disable);
			Instruction::Flag zFlag = (z ? Instruction::Flag::Enable : Instruction::Flag::Disable);

			instruction = Instruction(0b11111101, Instruction::Arg::Addr, Instruction::Arg::None, Instruction::Arg::None, cFlag, zFlag);

			if((!c & !z) || (c & z))
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
				instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
			}
			else
			{
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
			}

			this->uRomData.addInstruction(instruction);
		}
	}

	// LOAD_I + STORE + SP+1 + RPC+1 // =======================================

	// PUSH reg
	instruction = Instruction(0b01100100, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::Ram, Step::BusB::Default, Step::BusC::SP));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::SP_PLUS));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + SP-1 + STORE + RPC+1 // =======================================

	// POP reg
	instruction = Instruction(0b01101000, Instruction::Arg::ABXY);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::SP_MINUS));
	instruction.addStep(Step(Step::BusAR::Ram, Step::BusAW::ABXY, Step::BusB::Default, Step::BusC::SP));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + SP+1 + STORE + SP+1 + RESET //

	// CALL addr
	instruction = Instruction(0b11110001, Instruction::Arg::Addr);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
	instruction.addStep(Step(Step::BusAR::PCL, Step::BusAW::Ram, Step::BusB::Default, Step::BusC::SP));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::SP_PLUS));
	instruction.addStep(Step(Step::BusAR::PCH, Step::BusAW::Ram, Step::BusB::Default, Step::BusC::SP));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::SP_PLUS));
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
	instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + SP-1 + STORE + SP-1 + STORE + RPC+1 // ========================

	// RET
	instruction = Instruction(0b11110010);
	instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::SP_MINUS));
	instruction.addStep(Step(Step::BusAR::Ram, Step::BusAW::PCH, Step::BusB::Default, Step::BusC::SP));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::SP_MINUS));
	instruction.addStep(Step(Step::BusAR::Ram, Step::BusAW::PCL, Step::BusB::Default, Step::BusC::SP));
	instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
	this->uRomData.addInstruction(instruction);

	// LOAD_I + ALU + PC+1 + PC+1 + RPC+1 // ==================================
	//              + ALU + STORE + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + RESET //

	// LOOPE reg, reg, addr
	for(int c = 0; c < 2; c++)
	{
		for(int z = 0; z < 2; z++)
		{
			Instruction::Flag cFlag = (c ? Instruction::Flag::Enable : Instruction::Flag::Disable);
			Instruction::Flag zFlag = (z ? Instruction::Flag::Enable : Instruction::Flag::Disable);

			instruction = Instruction(0b11101000, Instruction::Arg::AB, Instruction::Arg::ABXY, Instruction::Arg::Addr, cFlag, zFlag);
			instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
			instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::AB, Step::BusC::Default, 0b0110, false, true));

			if(c & z)
			{
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
			}
			else
			{
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
			}

			instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
			instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
			instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, false));
			instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
			instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
			this->uRomData.addInstruction(instruction);
		}
	}

	// LOAD_I + ALU + STORE + PC+1 + PC+1 + PC+1 + RPC+1 // ===================
	//                             + ALU + STORE + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + RESET //

	// LOOPE val, reg, addr
	for(int c = 0; c < 2; c++)
	{
		for(int z = 0; z < 2; z++)
		{
			Instruction::Flag cFlag = (c ? Instruction::Flag::Enable : Instruction::Flag::Disable);
			Instruction::Flag zFlag = (z ? Instruction::Flag::Enable : Instruction::Flag::Disable);

			instruction = Instruction(0b01001100, Instruction::Arg::Val256, Instruction::Arg::ABXY, Instruction::Arg::Addr, cFlag, zFlag);
			instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
			instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
			instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
			instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b0110, false, true));

			if(c & z)
			{
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
			}
			else
			{

				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
			}

			instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
			instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
			instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0000, false, false));
			instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
			instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));
			this->uRomData.addInstruction(instruction);
		}
	}

	// LOAD_I + ALU STORE + PC+1 + PC+1 + RPC+1 // ============================
	//                    + ALU + STORE + PC+1 + LOAD_D + PC+1 + LOAD_PCH + STORE + RESET //

	// LOOPZ reg, addr
	for(int c = 0; c < 2; c++)
	{
		for(int z = 0; z < 2; z++)
		{
			Instruction::Flag cFlag = (c ? Instruction::Flag::Enable : Instruction::Flag::Disable);
			Instruction::Flag zFlag = (z ? Instruction::Flag::Enable : Instruction::Flag::Disable);

			instruction = Instruction(0b11110100, Instruction::Arg::ABXY, Instruction::Arg::Addr, Instruction::Arg::None, cFlag, zFlag);
			instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::I));
			instruction.addStep(Step(Step::BusAR::D, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b0011, false, false));
			instruction.addStep(Step(Step::BusAR::T, Step::BusAW::D));
			instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::D, Step::BusC::Default, 0b0110, false, true));

			if(c & z)
			{
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC_PLUS));
			}
			else
			{

				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
				instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::D));
				instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::PC_PLUS));
			}

			instruction.addStep(Step(Step::BusAR::Flash, Step::BusAW::PCH));
			instruction.addStep(Step(Step::BusAR::D, Step::BusAW::PCL));
			instruction.addStep(Step(Step::BusAR::ABXY, Step::BusAW::ALU_T, Step::BusB::Default, Step::BusC::Default, 0b1111, false, true));
			instruction.addStep(Step(Step::BusAR::T, Step::BusAW::ABXY));
			instruction.addStep(Step(Step::BusAR::Default, Step::BusAW::RPC));

			this->uRomData.addInstruction(instruction);
		}
	}
}

/**
 * Save a binary microcode to files
 *
 * @param urom0Path Path to the first output file
 * @param urom1Path Path to the second output file
 *
 * @return Status of the save operation
 */
bool URom::saveFiles(const QString &urom0Path, const QString &urom1Path)
{
	// Try to save the data to the output files
	return(this->uRomData.saveFiles(urom0Path, urom1Path));
}
