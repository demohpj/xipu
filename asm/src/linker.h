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

#ifndef LINKER_H
#define LINKER_H

#include <QString>
#include <QTextStream>
#include <QVector>
#include <QHash>
#include <QRegularExpression>
#include <QFile>
#include <QIODevice>

#include "codeitem.h"
#include "codesection.h"
#include "ramitem.h"
#include "flashitem.h"
#include "data.h"
#include "datalist.h"

//! This class glues all binary parts from the compilation part and set correct memory addresses
class Linker
{
	public:
		static const int ROM_RAM_SIZE = 3840; //!< Size of RAM reserved for the kernel
		static const int ROM_FLASH_SIZE = 32768; //!< Size of Flash reserved for the kernel
		static const int ROM_RAM_OFFSET = 0; //!< Start of RAM reserved for the kernel
		static const int ROM_FLASH_OFFSET = 0; //!< Start of Flash reserved for the kernel

		static const int APP_RAM_SIZE = 28672; //!<  Size of RAM reserved for a user application
		static const int APP_FLASH_SIZE = 32768; //!< Size of Flash reserved for a user application
		static const int APP_RAM_OFFSET = 4096; //!< Start of RAM reserved for a user application
		static const int APP_FLASH_OFFSET = 32768; //!< Start of Flash reserved for a user application

		Linker(DataList &dataList);

		bool link();
		bool saveFile(const QString &path, const QString &pathMap);

	private:
		DataList &dataList; //!< List of data for each source file
		QVector<unsigned char> flashVector; //!< Buffer with a compiled, parsed and linked binary code

		bool findAddress(Data &data, const QString &label, unsigned int &address);
		bool findCodeAddress(Data &data, const QString &name, unsigned int &address);
		bool findMemoryAddress(Data &data, const QString &name, unsigned int &address);
};

#endif
