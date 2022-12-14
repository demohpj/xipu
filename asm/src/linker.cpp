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

#include "linker.h"

/**
 * Constructor of the linker
 *
 * @param dataList Data list
 */
Linker::Linker(DataList &dataList) : dataList(dataList)
{
}

/**
 * Link all parts of the application together and fill addresses
 *
 * @return Status of linking the application
 */
bool Linker::link()
{
	QTextStream out(stdout);

	unsigned int ramSize = ((this->dataList.getMode() == DataList::Mode::Rom) ? ROM_RAM_SIZE : APP_RAM_SIZE);
	unsigned int flashSize = ((this->dataList.getMode() == DataList::Mode::Rom) ? ROM_FLASH_SIZE : APP_FLASH_SIZE);

	unsigned int ramOffset = ((this->dataList.getMode() == DataList::Mode::Rom) ? ROM_RAM_OFFSET : APP_RAM_OFFSET);
	unsigned int flashOffset = ((this->dataList.getMode() == DataList::Mode::Rom) ? ROM_FLASH_OFFSET : APP_FLASH_OFFSET);

	unsigned int ramUsage = 0;
	unsigned int flashUsage = 0;

	this->flashVector.clear();

	// Move code to flash
	for(const QSharedPointer<Data> &data : this->dataList)
	{
		// Copy code to flash list
		for(const QSharedPointer<CodeSection> &codeSection : data->getCodeSectionList())
		{
			codeSection->setAddress(flashUsage + flashOffset);

			for(const QSharedPointer<CodeItem> &codeItem : codeSection->getCodeItemList())
			{
				codeItem->setAddress(flashUsage + flashOffset);

				flashUsage += static_cast<unsigned int>(codeItem->getDataVector().length());

				this->flashVector.append(codeItem->getDataVector());
			}
		}
	}

	// Move data to ram and flash
	for(const QSharedPointer<Data> &data : this->dataList)
	{
		// Count ram
		for(const QSharedPointer<RamItem> &ramItem : data->getRamItemList())
		{
			ramItem->setAddress(ramUsage + ramOffset);

			ramUsage += ramItem->getDataSize();
		}

		// Copy flash to flash list
		for(const QSharedPointer<FlashItem> &flashItem : data->getFlashItemList())
		{
			flashItem->setAddress(flashUsage + flashOffset);

			flashUsage += static_cast<unsigned int>(flashItem->getDataSize());

			this->flashVector.append(flashItem->getDataVector());
		}
	}

	if(ramUsage > ramSize)
	{
		out << QString("ERROR: %1 of %2 (%3%) bytes of RAM are used\n").arg(ramUsage).arg(ramSize).arg(QString::asprintf("%.1f", ((static_cast<double>(ramUsage) / static_cast<double>(ramSize)) * 100.0)));
		return(false);
	}

	if(flashUsage > flashSize)
	{
		out << QString("ERROR: %1 of %2 (%3%) bytes of FLASH are used\n").arg(flashUsage).arg(flashSize).arg(QString::asprintf("%.1f", ((static_cast<double>(flashUsage) / static_cast<double>(flashSize)) * 100.0)));
		return(false);
	}

	// Update addresses
	for(const QSharedPointer<Data> &data : this->dataList)
	{
		for(const QSharedPointer<CodeSection> &codeSection : data->getCodeSectionList())
		{
			for(const QSharedPointer<CodeItem> &codeItem : codeSection->getCodeItemList())
			{
				QHashIterator<QString, unsigned int> it(codeItem->getToFillUpHash());

				while(it.hasNext())
				{
					it.next();

					const QString &label = it.key();
					unsigned int offset = it.value();

					unsigned int address;

					offset += (codeItem->getAddress() - flashOffset);

					bool isOk = false;

					if((label.left(1) == ".") || (label.left(1) == "*"))
					{
						if(this->findAddress(*data, label, address))
						{
							this->flashVector.replace(static_cast<int>(offset), static_cast<unsigned char>(address % 256));
							this->flashVector.replace(static_cast<int>(offset + 1), static_cast<unsigned char>(address / 256));

							isOk = true;
						}
					}
					else if(!label.leftRef(3).compare(QString("low"), Qt::CaseInsensitive))
					{
						QString tLabel = label.mid(4, (label.length() - 5));

						if(this->findAddress(*data, tLabel, address))
						{
							this->flashVector.replace(static_cast<int>(offset), static_cast<unsigned char>(address % 256));

							isOk = true;
						}
					}
					else if(!label.leftRef(4).compare(QString("high"), Qt::CaseInsensitive))
					{
						QString tLabel = label.mid(5, (label.length() - 6));

						if(this->findAddress(*data, tLabel, address))
						{
							this->flashVector.replace(static_cast<int>(offset), static_cast<unsigned char>(address / 256));

							isOk = true;
						}
					}

					if(!isOk)
					{
						out << QString("ERROR: Unknown label name \"%1\" at line %2 in file: \"%3\"\n").arg(label).arg(codeItem->getLine()).arg(data->getFileName());
						return(false);
					}
				}
			}
		}
	}

	out << QString("INFO: %1 of %2 (%3%) bytes of RAM are used\n").arg(ramUsage).arg(ramSize).arg(QString::asprintf("%.1f", ((static_cast<double>(ramUsage) / static_cast<double>(ramSize)) * 100.0)));
	out << QString("INFO: %1 of %2 (%3%) bytes of FLASH are used\n").arg(flashUsage).arg(flashSize).arg(QString::asprintf("%.1f", ((static_cast<double>(flashUsage) / static_cast<double>(flashSize)) * 100.0)));

	return(true);
}

/**
 * Save the binary to the file
 *
 * @param path Path to the binary file
 * @param pathMap Path to the map file
 *
 * @return Status of saving data to the file
 */
bool Linker::saveFile(const QString &path, const QString &pathMap)
{
	if(!this->flashVector.empty())
	{
		if(this->flashVector.size() > ROM_FLASH_SIZE)
		{
			return(false);
		}

		if(this->dataList.getMode() == DataList::Mode::Rom)
		{
			this->flashVector.append(QVector<unsigned char>((ROM_FLASH_SIZE - this->flashVector.size()), 0));
		}

		QFile file(path);

		if(file.open(QIODevice::WriteOnly))
		{
			file.write(reinterpret_cast<const char *>(this->flashVector.constData()), this->flashVector.size());
			file.close();
		}
		else
		{
			return(false);
		}
	}
	else
	{
		return(false);
	}

	if(this->dataList.getMode() == DataList::Mode::Rom)
	{
		QFile file(pathMap);

		if(file.open(QIODevice::WriteOnly))
		{
			for(const QSharedPointer<RamItem> &ramItem : this->dataList.getExternalRamItemList())
			{
				QString line = QString("external var %1 = 0x%2\n").arg(ramItem->getName()).arg(ramItem->getAddress(), 4, 16, QChar('0'));

				file.write(line.toUtf8());
			}

			for(const QSharedPointer<FlashItem> &flashItem : this->dataList.getExternalFlashItemList())
			{
				QString line = QString("external const %1 = 0x%2\n").arg(flashItem->getName()).arg(flashItem->getAddress(), 4, 16, QChar('0'));

				file.write(line.toUtf8());
			}

			for(const QSharedPointer<CodeSection> &codeSection : this->dataList.getExternalCodeSectionList())
			{
				QString line = QString("%1 = 0x%2\n").arg(codeSection->getName()).arg(codeSection->getAddress(), 4, 16, QChar('0'));

				file.write(line.toUtf8());
			}

			file.close();
		}
		else
		{
			return(false);
		}
	}

	return(true);
}

/**
 * Find the correct address in memory
 *
 * @param data Data of the compiled application
 * @param label Label name to find
 * @param address Found address
 *
 * @return Status of searching the address
 */
bool Linker::findAddress(Data &data, const QString &label, unsigned int &address)
{
	QString name;

	if(label.contains(QRegularExpression(R"(^(\.[0-9a-zA-Z_-]+)$)")))
	{
		name = label;

		return(this->findCodeAddress(data, name, address));
	}
	else if(label.contains(QRegularExpression(R"(^(\*[0-9a-zA-Z_-]+)$)")))
	{
		name = label.mid(1);

		return(this->findMemoryAddress(data, name, address));
	}
	else if(label.contains(QRegularExpression(R"(^(\*[0-9a-zA-Z_-]+\[[0-9]+\])$)")))
	{
		int cOffset = label.indexOf("[");
		name = label.mid(1, (cOffset - 1));
		unsigned int offset = label.midRef((cOffset + 1), (label.length() - cOffset - 2)).toUInt();

		if(this->findMemoryAddress(data, name, address))
		{
			address += offset;

			return(true);
		}
	}

	return(false);
}

/**
 * Find the correct address of code in memory
 *
 * @param data Data of the compiled application
 * @param name Name to find
 * @param address Found address
 *
 * @return Status of searching the address
 */
bool Linker::findCodeAddress(Data &data, const QString &name, unsigned int &address)
{
	for(const QSharedPointer<CodeSection> &codeSection : data.getCodeSectionList())
	{
		if(!codeSection->getName().compare(name, Qt::CaseInsensitive))
		{
			address = codeSection->getAddress();

			return(true);
		}
	}

	for(const QSharedPointer<CodeSection> &codeSection : this->dataList.getGlobalCodeSectionList())
	{
		if(!codeSection->getName().compare(name, Qt::CaseInsensitive))
		{
			address = codeSection->getAddress();

			return(true);
		}
	}

	return(false);
}

/**
 * Find the correct address of variable or const in memory
 *
 * @param data Data of the compiled application
 * @param name Name to find
 * @param address Found address
 *
 * @return Status of searching the address
 */
bool Linker::findMemoryAddress(Data &data, const QString &name, unsigned int &address)
{
	for(const QSharedPointer<RamItem> &ramItem : data.getRamItemList())
	{
		if(!ramItem->getName().compare(name, Qt::CaseInsensitive))
		{
			address = ramItem->getAddress();

			return(true);
		}
	}

	for(const QSharedPointer<FlashItem> &flashItem : data.getFlashItemList())
	{
		if(!flashItem->getName().compare(name, Qt::CaseInsensitive))
		{
			address = flashItem->getAddress();

			return(true);
		}
	}

	for(const QSharedPointer<RamItem> &ramItem : this->dataList.getGlobalRamItemList())
	{
		if(!ramItem->getName().compare(name, Qt::CaseInsensitive))
		{
			address = ramItem->getAddress();

			return(true);
		}
	}

	for(const QSharedPointer<FlashItem> &flashItem : this->dataList.getGlobalFlashItemList())
	{
		if(!flashItem->getName().compare(name, Qt::CaseInsensitive))
		{
			address = flashItem->getAddress();

			return(true);
		}
	}

	return(false);
}
