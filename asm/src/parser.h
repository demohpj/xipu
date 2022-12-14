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

#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QList>
#include <QRegularExpression>
#include <QFile>
#include <QFileInfo>
#include <QSharedPointer>

#include "codeitem.h"
#include "codesection.h"
#include "ramitem.h"
#include "flashitem.h"
#include "data.h"
#include "datalist.h"

//! This class parses all input source files for later compilation process
class Parser
{
	public:
		Parser(DataList &dataList);

		bool parseFile(const QString &path);

	private:
		DataList &dataList; //!< List of data for each source file

		bool parseMap(int i, const QString &mapPath, const QStringList &lineItems);

		QSharedPointer<RamItem> parseRam(int i, QString &line);
		QSharedPointer<FlashItem> parseFlash(int i, QString &line);
		QSharedPointer<CodeItem> parseCode(int i, const QString &label, QString &line);
};

#endif
