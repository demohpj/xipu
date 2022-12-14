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

#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QList>
#include <QSharedPointer>

#include "codesection.h"
#include "ramitem.h"
#include "flashitem.h"

//! This class contains all parsed sections of code, RAM and Flash for one source file
class Data
{
	public:
		static const int MEMORY_SIZE = 65536; //!< Maximum size of memory

		Data(const QString &fileName);

		const QString &getFileName() const;

		bool addCodeSection(const QSharedPointer<CodeSection> &codeSection);
		bool addRamItem(const QSharedPointer<RamItem> &ramItem);
		bool addFlashItem(const QSharedPointer<FlashItem> &flashItem);

		QList<QSharedPointer<CodeSection>> &getCodeSectionList();
		QList<QSharedPointer<RamItem>> &getRamItemList();
		QList<QSharedPointer<FlashItem>> &getFlashItemList();

	private:
		QString fileName; //!< Name of the source file

		QList<QSharedPointer<CodeSection>> codeSectionList; //!< List of code sections
		QList<QSharedPointer<RamItem>> ramItemList; //!< List of RAM items
		QList<QSharedPointer<FlashItem>> flashItemList; //!< List of Flash items
};

#endif
