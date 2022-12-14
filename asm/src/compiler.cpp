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

#include "compiler.h"

/**
 * Constructor of the compiler
 *
 * @param dataList Data list
 */
Compiler::Compiler(DataList &dataList) : dataList(dataList)
{
}

/**
 * Add an instruction to the uROM opcode table
 *
 * @param instruction Instruction to add
 */
void Compiler::addInstruction(const Instruction &instruction)
{
	this->instructionList.append(instruction);
}

/**
 * Compile the application
 *
 * @return Status of compiling the application
 */
bool Compiler::compile()
{
	QTextStream out(stdout);

	for(const QSharedPointer<Data> &data : this->dataList)
	{
		if(!(this->compileRam(*data)))
		{
			return(false);
		}

		if(!(this->compileFlash(*data)))
		{
			return(false);
		}

		if(!(this->compileCode(*data)))
		{
			return(false);
		}
	}

	return(true);
}

/**
 * Check the type of the param
 *
 * @param param String to check
 *
 * @return Param type
 */
QList<Instruction::Arg> Compiler::checkParamType(const QString &param)
{
	QList<Instruction::Arg> argList;

	if(param.contains(QRegularExpression("^(a|b|x|y|A|B|X|Y)$")))
	{
		argList.append(Instruction::Arg::ABXY);

		if(param.contains(QRegularExpression("^(a|b|A|B)$")))
		{
			argList.append(Instruction::Arg::AB);
		}
	}
	else if(param.contains(QRegularExpression("^([0-9]+|0b[01]+|0x[0-9abcdefABCDEF]+|'.')$")))
	{
		argList.append(Instruction::Arg::Val256);

		if(param.contains(QRegularExpression("^(0x[0-9abcdefABCDEF]+)$")))
		{
			argList.append(Instruction::Arg::Addr);
		}
	}
	else if(param.contains(QRegularExpression(R"(^(\.[0-9a-zA-Z_-]+)$)")))
	{
		argList.append(Instruction::Arg::Addr);
	}
	else if(param.contains(QRegularExpression(R"(^(\*[0-9a-zA-Z_-]+)$)")))
	{
		argList.append(Instruction::Arg::Addr);
	}
	else if(param.contains(QRegularExpression(R"(^(\*[0-9a-zA-Z_-]+\[[0-9]+\])$)")))
	{
		argList.append(Instruction::Arg::Addr);
	}
	else if(param.contains(QRegularExpression(R"(^((low|high)\(\.[0-9a-zA-Z_-]+\))$)")))
	{
		argList.append(Instruction::Arg::Val256);
	}
	else if(param.contains(QRegularExpression(R"(^((low|high)\(\*[0-9a-zA-Z_-]+\))$)")))
	{
		argList.append(Instruction::Arg::Val256);
	}
	else if(param.contains(QRegularExpression(R"(^((low|high)\(\*[0-9a-zA-Z_-]+\[[0-9]+\]\))$)")))
	{
		argList.append(Instruction::Arg::Val256);
	}

	return(argList);
}

/**
 * Compile the RAM items
 *
 * @param data Data of the parsed application
 *
 * @return Status of compiling the RAM items
 */
bool Compiler::compileRam(Data &data)
{
	QTextStream out(stdout);

	for(const QSharedPointer<RamItem> &ramItem : data.getRamItemList())
	{
		bool compiled = false;
		QString arraySize = ramItem->getArraySize();

		if(arraySize.length() > 0)
		{
			if(arraySize.contains(QRegularExpression("^([0-9]+)$")))
			{
				bool isOk;
				unsigned int size = arraySize.toUInt(&isOk, 10);

				if(isOk)
				{
					ramItem->setDataSize(size);
					compiled = true;
				}
			}
		}
		else
		{
			ramItem->setDataSize(1);
			compiled = true;
		}

		if(!compiled)
		{
			out << QString("ERROR: Syntax error at line %1 in file: \"%2\"\n").arg(ramItem->getLine()).arg(data.getFileName());
			return(false);
		}
	}

	return(true);
}

/**
 * Compile the Flash items
 *
 * @param data Data of the parsed application
 *
 * @return Status of compiling the Flash items
 */
bool Compiler::compileFlash(Data &data)
{
	QTextStream out(stdout);

	for(const QSharedPointer<FlashItem> &flashItem : data.getFlashItemList())
	{
		bool compiled = true;
		bool isString = false;

		if(flashItem->getValueList().size() == 1)
		{
			QString string = flashItem->getValueList().at(0);

			if(string.contains(QRegularExpression("^(\".+\")$")))
			{
				isString = true;

				for(int j = 1; j < (string.length() - 1); j++)
				{
					char cChar = string.at(j).toLatin1();

					if(cChar > 0)
					{
						flashItem->getDataVector().append(static_cast<unsigned char>(cChar));
					}
					else
					{
						compiled = false;
						break;
					}
				}

				flashItem->getDataVector().append(0x00);
			}
		}

		if(!isString)
		{
			if(!flashItem->getValueList().empty())
			{
				for(int j = 0; j < flashItem->getValueList().size(); j++)
				{
					unsigned char value;

					if(this->parseParamVal256(flashItem->getValueList().at(j), value))
					{
						flashItem->getDataVector().append(value);
					}
					else
					{
						compiled = false;
						break;
					}
				}
			}
			else
			{
				compiled = false;
			}
		}

		if(!compiled)
		{
			out << QString("ERROR: Syntax error at line %1 in file: \"%2\"\n").arg(flashItem->getLine()).arg(data.getFileName());
			return(false);
		}
	}

	return(true);
}

/**
 * Compile the code items
 *
 * @param data Data of the parsed application
 *
 * @return Status of compiling the code items
 */
bool Compiler::compileCode(Data &data)
{
	QTextStream out(stdout);

	for(const QSharedPointer<CodeSection> &codeSection : data.getCodeSectionList())
	{
		for(const QSharedPointer<CodeItem> &codeItem : codeSection->getCodeItemList())
		{
			bool compiled = false;

			QVector<QList<Instruction::Arg>> arg(3);

			if(codeItem->getParamList().length() == 3)
			{
				arg[0].append(this->checkParamType(codeItem->getParamList().at(0)));
				arg[1].append(this->checkParamType(codeItem->getParamList().at(1)));
				arg[2].append(this->checkParamType(codeItem->getParamList().at(2)));

				for(const Instruction &instruction : this->instructionList)
				{
					if(!instruction.getName().compare(codeItem->getName(), Qt::CaseInsensitive))
					{
						if(arg[0].contains(instruction.getArgList().at(0)) && arg[1].contains(instruction.getArgList().at(1)) && arg[2].contains(instruction.getArgList().at(2)))
						{
							QVector<unsigned char> dataList;
							QVector<unsigned char> tmp(2);
							unsigned char opCode = instruction.getOpcode();
							bool isOk = true;

							if(instruction.getArgList().at(0) == Instruction::Arg::AB)
							{
								isOk &= this->parseParamAB(codeItem->getParamList().at(0), tmp[0]);
								opCode |= (tmp[0] << 2);

								if(instruction.getArgList().at(1) == Instruction::Arg::ABXY)
								{
									isOk &= this->parseParamABXY(codeItem->getParamList().at(1), tmp[0]);
									opCode |= tmp[0];
								}
								else
								{
									isOk = false;
								}
							}
							else if(instruction.getArgList().at(1) == Instruction::Arg::ABXY)
							{
								isOk &= this->parseParamABXY(codeItem->getParamList().at(1), tmp[0]);
								opCode |= tmp[0];
							}

							dataList.append(opCode);

							if(instruction.getArgList().at(0) == Instruction::Arg::Val256)
							{
								isOk &= this->parseParamVal256(codeItem->getParamList().at(0), tmp[0]);

								dataList.append(tmp[0]);
							}

							if(instruction.getArgList().at(2) == Instruction::Arg::Addr)
							{
								if(isOk && this->isAddrLabel(codeItem->getParamList().at(2)))
								{
									codeItem->getToFillUpHash().insert(codeItem->getParamList().at(2), static_cast<unsigned int>(dataList.size()));
								}
								else
								{
									isOk &= this->parseParamAddr(codeItem->getParamList().at(2), tmp[0], tmp[1]);
								}

								dataList.append(tmp[0]);
								dataList.append(tmp[1]);
							}

							if(isOk)
							{
								codeItem->getDataVector().append(dataList);
								compiled = true;
							}

							break;
						}
					}
				}
			}
			else if(codeItem->getParamList().length() == 2)
			{
				arg[0].append(this->checkParamType(codeItem->getParamList().at(0)));
				arg[1].append(this->checkParamType(codeItem->getParamList().at(1)));

				for(const Instruction &instruction : this->instructionList)
				{
					if(!instruction.getName().compare(codeItem->getName(), Qt::CaseInsensitive))
					{
						if(arg[0].contains(instruction.getArgList().at(0)) && arg[1].contains(instruction.getArgList().at(1)) && (instruction.getArgList().at(2) == Instruction::Arg::None))
						{
							QVector<unsigned char> dataList;
							QVector<unsigned char> tmp(2);
							unsigned char opCode = instruction.getOpcode();
							bool isOk = true;

							if(instruction.getArgList().at(0) == Instruction::Arg::AB)
							{
								isOk &= this->parseParamAB(codeItem->getParamList().at(0), tmp[0]);
								opCode |= (tmp[0] << 2);

								if(instruction.getArgList().at(1) == Instruction::Arg::ABXY)
								{
									isOk &= this->parseParamABXY(codeItem->getParamList().at(1), tmp[0]);
									opCode |= tmp[0];
								}
								else
								{
									isOk = false;
								}
							}
							else if(instruction.getArgList().at(0) == Instruction::Arg::ABXY)
							{
								isOk &= this->parseParamABXY(codeItem->getParamList().at(0), tmp[0]);

								if(instruction.getArgList().at(1) == Instruction::Arg::ABXY)
								{
									isOk &= this->parseParamABXY(codeItem->getParamList().at(1), tmp[1]);
									opCode |= ((tmp[0] << 2) | tmp[1]);
								}
								else
								{
									opCode |= tmp[0];
								}
							}
							else if(instruction.getArgList().at(1) == Instruction::Arg::ABXY)
							{
								isOk &= this->parseParamABXY(codeItem->getParamList().at(1), tmp[0]);
								opCode |= tmp[0];
							}

							dataList.append(opCode);

							if(instruction.getArgList().at(0) == Instruction::Arg::Val256)
							{
								if(isOk && this->isAddrPartLabel(codeItem->getParamList().at(0)))
								{
									codeItem->getToFillUpHash().insert(codeItem->getParamList().at(0), static_cast<unsigned int>(dataList.size()));
								}
								else
								{
									isOk &= this->parseParamVal256(codeItem->getParamList().at(0), tmp[0]);
								}

								dataList.append(tmp[0]);

							}
							else if(instruction.getArgList().at(0) == Instruction::Arg::Addr)
							{
								if(isOk && this->isAddrLabel(codeItem->getParamList().at(0)))
								{
									codeItem->getToFillUpHash().insert(codeItem->getParamList().at(0), static_cast<unsigned int>(dataList.size()));
								}
								else
								{
									isOk &= this->parseParamAddr(codeItem->getParamList().at(0), tmp[0], tmp[1]);
								}

								dataList.append(tmp[0]);
								dataList.append(tmp[1]);
							}

							if(instruction.getArgList().at(1) == Instruction::Arg::Addr)
							{
								if(isOk && this->isAddrLabel(codeItem->getParamList().at(1)))
								{
									codeItem->getToFillUpHash().insert(codeItem->getParamList().at(1), static_cast<unsigned int>(dataList.size()));
								}
								else
								{
									isOk &= this->parseParamAddr(codeItem->getParamList().at(1), tmp[0], tmp[1]);
								}

								dataList.append(tmp[0]);
								dataList.append(tmp[1]);
							}
							if(isOk)
							{
								if(instruction.getOpcodeSecond() != 0)
								{
									opCode = instruction.getOpcodeSecond();

									if(instruction.getArgSecondList().at(0) == Instruction::Arg::ABXY)
									{
										isOk &= this->parseParamABXY(codeItem->getParamList().at(0), tmp[0]);
										opCode |= tmp[0];
									}

									dataList.append(opCode);
								}

								if(isOk)
								{
									codeItem->getDataVector().append(dataList);
									compiled = true;
								}
							}

							break;
						}
					}
				}
			}
			else if(codeItem->getParamList().length() == 1)
			{
				arg[0].append(this->checkParamType(codeItem->getParamList().at(0)));

				for(const Instruction &instruction : this->instructionList)
				{
					if(!instruction.getName().compare(codeItem->getName(), Qt::CaseInsensitive))
					{
						if(arg[0].contains(instruction.getArgList().at(0)) && (instruction.getArgList().at(1) == Instruction::Arg::None) && (instruction.getArgList().at(2) == Instruction::Arg::None))
						{
							QVector<unsigned char> dataList;
							QVector<unsigned char> tmp(2);
							unsigned char opCode = instruction.getOpcode();
							bool isOk = true;

							if(instruction.getArgList().at(0) == Instruction::Arg::ABXY)
							{
								isOk &= this->parseParamABXY(codeItem->getParamList().at(0), tmp[0]);
								opCode |= tmp[0];
							}

							dataList.append(opCode);

							if(instruction.getArgList().at(0) == Instruction::Arg::Val256)
							{
								if(isOk && this->isAddrPartLabel(codeItem->getParamList().at(0)))
								{
									codeItem->getToFillUpHash().insert(codeItem->getParamList().at(0), static_cast<unsigned int>(dataList.size()));
								}
								else
								{
									isOk &= this->parseParamVal256(codeItem->getParamList().at(0), tmp[0]);
								}

								dataList.append(tmp[0]);
							}
							else if(instruction.getArgList().at(0) == Instruction::Arg::Addr)
							{
								if(isOk && this->isAddrLabel(codeItem->getParamList().at(0)))
								{
									codeItem->getToFillUpHash().insert(codeItem->getParamList().at(0), static_cast<unsigned int>(dataList.size()));
								}
								else
								{
									isOk &= this->parseParamAddr(codeItem->getParamList().at(0), tmp[0], tmp[1]);
								}

								dataList.append(tmp[0]);
								dataList.append(tmp[1]);
							}

							if(isOk)
							{
								codeItem->getDataVector().append(dataList);
								compiled = true;
							}

							break;
						}
					}
				}
			}
			else
			{
				for(const Instruction &instruction : this->instructionList)
				{
					if(!instruction.getName().compare(codeItem->getName(), Qt::CaseInsensitive))
					{
						if((instruction.getArgList().at(0) == Instruction::Arg::None) && (instruction.getArgList().at(1) == Instruction::Arg::None) && (instruction.getArgList().at(2) == Instruction::Arg::None))
						{
							codeItem->getDataVector().append(instruction.getOpcode());
							compiled = true;

							break;
						}
					}
				}
			}

			if(!compiled)
			{
				out << QString("ERROR: Syntax error at line %1 in file: \"%2\"\n").arg(codeItem->getLine()).arg(data.getFileName());
				return(false);
			}
		}
	}

	return(true);
}

/**
 * Parse the string and return AB
 *
 * @param param String to parsing
 * @param value Parsed value
 *
 * @return Status of parsing the string
 */
bool Compiler::parseParamAB(const QString &param, unsigned char &value)
{
	if(!param.compare("a", Qt::CaseInsensitive))
	{
		value = 0;
	}
	else if(!param.compare("b", Qt::CaseInsensitive))
	{
		value = 1;
	}
	else
	{
		return(false);
	}

	return(true);
}

/**
 * Parse the string and return ABXY
 *
 * @param param String to parsing
 * @param value Parsed value
 *
 * @return Status of parsing the string
 */
bool Compiler::parseParamABXY(const QString &param, unsigned char &value)
{
	if(!param.compare("a", Qt::CaseInsensitive))
	{
		value = 0;
	}
	else if(!param.compare("b", Qt::CaseInsensitive))
	{
		value = 1;
	}
	else if(!param.compare("x", Qt::CaseInsensitive))
	{
		value = 2;
	}
	else if(!param.compare("y", Qt::CaseInsensitive))
	{
		value = 3;
	}
	else
	{
		return(false);
	}

	return(true);
}

/**
 * Parse the string and return Val256
 *
 * @param param String to parsing
 * @param value Parsed value
 *
 * @return Status of parsing the string
 */
bool Compiler::parseParamVal256(const QString &param, unsigned char &value)
{
	bool isOk = false;
	unsigned int number = 0;

	if(param.contains(QRegularExpression("^([0-9]+)$")))
	{
		number = param.toUInt(&isOk, 10);
	}
	else if(param.contains(QRegularExpression("^(0b[01]+)$")))
	{
		number = param.midRef(2).toUInt(&isOk, 2);
	}
	else if(param.contains(QRegularExpression("^(0x[0-9abcdefABCDEF]+)$")))
	{
		number = param.midRef(2).toUInt(&isOk, 16);
	}
	else if(param.contains(QRegularExpression("^('.')$")))
	{
		char cChar = param.at(1).toLatin1();

		if(cChar > 0)
		{
			number = static_cast<unsigned int>(cChar);
			isOk = true;
		}
	}

	if(isOk)
	{
		if(number < 256)
		{
			value = static_cast<unsigned char>(number);
		}
		else
		{
			isOk = false;
		}
	}

	return(isOk);
}

/**
 * Parse the string and return address
 *
 * @param param String to parsing
 * @param valueLow Parsed lower byte of the value
 * @param valueHigh Parsed higher byte of the value
 *
 * @return Status of parsing the string
 */
bool Compiler::parseParamAddr(const QString &param, unsigned char &valueLow, unsigned char &valueHigh)
{
	bool isOk = false;
	unsigned int number = 0;

	if(param.contains(QRegularExpression("^(0x[0-9abcdefABCDEF]+)$")))
	{
		number = param.midRef(2).toUInt(&isOk, 16);
	}

	if(isOk)
	{
		if(number < Data::MEMORY_SIZE)
		{
			valueLow = static_cast<unsigned char>(number % 256);
			valueHigh = static_cast<unsigned char>(number / 256);
		}
		else
		{
			isOk = false;
		}
	}

	return(isOk);
}

/**
 * Check if the param is an address label
 *
 * @param param String to check
 *
 * @return Status of the check
 */
bool Compiler::isAddrLabel(const QString &param) const
{
	if(param.contains(QRegularExpression(R"(^(\.[0-9a-zA-Z_-]+)$)")))
	{
		return(true);
	}
	else if(param.contains(QRegularExpression(R"(^(\*[0-9a-zA-Z_-]+)$)")))
	{
		return(true);
	}
	else if(param.contains(QRegularExpression(R"(^(\*[0-9a-zA-Z_-]+\[[0-9]+\])$)")))
	{
		return(true);
	}

	return(false);
}

/**
 * Check if the param is a low or high address
 *
 * @param param String to check
 *
 * @return Status of the check
 */
bool Compiler::isAddrPartLabel(const QString &param) const
{
	if(param.contains(QRegularExpression(R"(^((low|high)\(\.[0-9a-zA-Z_-]+\))$)")))
	{
		return(true);
	}
	else if(param.contains(QRegularExpression(R"(^((low|high)\(\*[0-9a-zA-Z_-]+\))$)")))
	{
		return(true);
	}
	else if(param.contains(QRegularExpression(R"(^((low|high)\(\*[0-9a-zA-Z_-]+\[[0-9]+\]\))$)")))
	{
		return(true);
	}

	return(false);
}
