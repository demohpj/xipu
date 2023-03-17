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

#include "fs.h"

/**
 * Constructor for the file system class
 *
 * @param parent Parent object
 */
FS::FS(QObject *parent) : QObject(parent)
{
	this->path = "";

	this->reset();
}

//! Reset the file system buffers and path
void FS::reset()
{
	this->buffer.clear();
}

/**
 * Set path to the emulated file system on the local disk
 *
 * @param path Path to emulated file system
 */
void FS::setPath(const QString &path)
{
	this->path = path;
}

/**
 * Open and load to buffer a file from the emulated file system
 *
 * @param dirName Name of the directory. "app" or "file".
 * @param fileName Name of the file to open
 * @param size Struct where size of the file will be written
 *
 * @return Status of the open operation
 */
bool FS::open(const QString &dirName, const FS::Name &fileName, FS::Size &size)
{
	size.low = 0;
	size.high = 0;

	if(!dirName.contains(QRegularExpression("^([0-9a-zA-Z_-]{1,8})$")))
	{
		return(false);
	}

	QString name(reinterpret_cast<const char *>(fileName.data.constData()));

	if(!name.contains(QRegularExpression("^([0-9a-zA-Z_-]{1,8})(([\\\\.])([0-9a-zA-Z_-]{1,3}))?$")))
	{
		return(false);
	}

	QString filePath = QString(this->path).append("/").append(dirName).append("/").append(name);
	QFile file(filePath);

	if(!file.open(QIODevice::ReadOnly))
	{
		return(false);
	}

	if(file.size() > FILE_MAX_SIZE)
	{
		file.close();

		return(false);
	}

	this->buffer = file.readAll();

	file.close();

	size.low = static_cast<unsigned char>(this->buffer.size() % 256);
	size.high = static_cast<unsigned char>((this->buffer.size() / 256) & 0xff);

	return(true);
}

/**
 * Create a list of files in the directory from the emulated file system
 *
 * @param dirName Name of the directory. "app" or "file".
 * @param size Struct where size of the buffer with description of the files will be written
 *
 * @return Status of the list operation
 */
bool FS::list(const QString &dirName, FS::Size &size)
{
	size.low = 0;
	size.high = 0;

	if(!dirName.contains(QRegularExpression("^([0-9a-zA-Z_-]{1,8})$")))
	{
		return(false);
	}

	QString dirPath = QString(this->path).append("/").append(dirName);
	QDir dir;

	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	dir.setSorting(QDir::Name);

	if(!dir.cd(dirPath))
	{
		return(false);
	}

	QFileInfoList fileInfoList = dir.entryInfoList();

	this->buffer.clear();

	for(const QFileInfo &fileInfo : fileInfoList)
	{
		QString fileName = fileInfo.fileName();
		qint64 fileSize = fileInfo.size();

		if(fileSize > FILE_MAX_SIZE)
		{
			continue;
		}

		if(!fileName.contains(QRegularExpression(R"(^([0-9a-zA-Z_-]{1,8})(([\\.])([0-9a-zA-Z_-]{1,3}))?$)")))
		{
			continue;
		}

		int j = 0;

		for(; ((j < NAME_MAX_LENGTH) && (j < fileName.length())); j++)
		{
			this->buffer.append(fileName.at(j).toLatin1());
		}

		for(; j < (DIR_ENTRY_SIZE - DIR_SIZE_FIELD_SIZE); j++)
		{
			this->buffer.append(static_cast<char>(0));
		}

		QVector<unsigned char> size(2);

		size[0] = static_cast<unsigned char>(fileSize % 256);
		size[1] = static_cast<unsigned char>((fileSize / 256) & 0xff);

		this->buffer.append(static_cast<char>(size[0]));
		this->buffer.append(static_cast<char>(size[1]));

		if(this->buffer.size() >= (DIR_MAX_QUANTITY * DIR_ENTRY_SIZE))
		{
			break;
		}
	}

	size.low = static_cast<unsigned char>(this->buffer.size() % 256);
	size.high = static_cast<unsigned char>((this->buffer.size() / 256) & 0xff);

	return(true);
}

/**
 * Read data from buffer to block based on offset
 *
 * @param block Struct where the data from the buffer will be written
 * @param number Number of the selected virtual block selected to read
 */
void FS::readBlock(FS::Block &block, int number)
{
	for(int i = 0; i < BLOCK_SIZE; i++)
	{
		int j = ((number * BLOCK_SIZE) + i);

		if(j < this->buffer.size())
		{
			block.data[i] = static_cast<unsigned char>(this->buffer.at(j));
		}
		else
		{
			block.data[i] = 0;
		}
	}
}
