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

#include "urom.h"

static QString const UROM0_PATH = "urom0.bin"; //!< Path to the first microcode output file
static QString const UROM1_PATH = "urom1.bin"; //!< Path to the second microcode output file

//! Main entry function of the application
int main(int, char **)
{
	QTextStream out(stdout);

	// Create and generate the microcode
	URom uRom = URom();

	// Try to save the data to the output files
	if(!uRom.saveFiles(UROM0_PATH, UROM1_PATH))
	{
		// If failed show the error message
		out << "ERROR: Failed to save uROM data\n";

		// Return the error code
		return(-1);
	}

	// The data files were saved. Show the success message
	out << "OK: uROM data has been saved successfully\n";

	// Return the success code
	return(0);
}
