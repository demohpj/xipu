#ifndef ASM_H
#define ASM_H

#include <QString>
#include <QTextStream>

#include "datalist.h"
#include "parser.h"
#include "compiler.h"
#include "linker.h"

//! This class contains assembler start functions
class Asm
{
	public:
		Asm();

		bool compileRom(const QString &inputFile, const QString &outputFile, const QString &outputMapFile);
		bool compileApp(const QString &inputFile, const QString &outputFile);

	private:
		bool compile(DataList::Mode mode, QString inputFile, QString outputFile,  QString outputMapFile = "");

		void initInstructions(Compiler &compiler);
};

#endif
