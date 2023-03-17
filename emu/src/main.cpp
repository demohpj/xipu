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

#include <QApplication>

#include "emu.h"

/**
 * Main entry function of the application
 *
 * @param argc Arguments count
 * @param argv Arguments list stored in an array
 */
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QFont font = a.font();

	font.setFamily("DejaVu Sans Mono");
	font.setPixelSize(11);
	font.setBold(true);

	a.setFont(font, "QWidget");

	Emu emu;
	emu.show();

	return(a.exec());
}
