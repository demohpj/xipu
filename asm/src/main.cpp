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

#include <QCoreApplication>
#include <QTextStream>

#include "asm.h"

/**
 * Print an exit message and the usage prompt if needed
 *
 * @param message Message to print
 * @param showUsage Show or not the usage prompt
 */
static void printExitMessage(const QString &message, const bool showUsage = false)
{
	QTextStream out(stdout);

	out << message << "\n\n";

	if(showUsage)
	{
		out << "Run as ROM build:\n";
		out << "  asm rom [input_file] [output_bin] [output_map]\n\n";
		out << "Run as APP build:\n";
		out << "  asm app [input_file] [output_bin]\n\n";
	}
}

/**
 * Main entry function of the application
 *
 * @param argc Arguments count
 * @param argv Arguments list stored in an array
 */
int main(int argc, char *argv[])
{
	QString inputFile;
	QString outputFile;
	QString outputMapFile;

	// Prepare the assembler
	Asm assembler = Asm();

	if(argc >= 2)
	{
		if(!QString(argv[1]).compare("rom", Qt::CaseInsensitive))
		{
			if(argc == 5)
			{
				inputFile = QString(argv[2]);
				outputFile = QString(argv[3]);
				outputMapFile = QString(argv[4]);

				// Compile the kernel
				return(assembler.compileRom(inputFile, outputFile, outputMapFile));
			}
		}
		else if(!QString(argv[1]).compare("app", Qt::CaseInsensitive))
		{
			if(argc == 4)
			{
				inputFile = QString(argv[2]);
				outputFile = QString(argv[3]);

				// Compile the application
				return(assembler.compileApp(inputFile, outputFile));
			}
		}
		else
		{
			printExitMessage(QString("ERROR: Unknow compilation mode"), true);
			return(-1);
		}
	}

	printExitMessage(QString("ERROR: Bad number of arguments"), true);
	return(-1);
}
