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

#ifndef FS_H
#define FS_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QFile>
#include <QDir>
#include <QFileInfoList>
#include <QRegularExpression>

//! This class contains file system functions
class FS : public QObject
{
	Q_OBJECT

	public:
		static const int NAME_MAX_LENGTH = 12; //!< Maximum name length of the file

		static const int BLOCK_SIZE = 256; //!<  Virtual block size available to address by communication bus between motherboard and CPU

		static const int DIR_ENTRY_SIZE = 16; //!< Size of the header with information about file
		static const int DIR_SIZE_FIELD_SIZE = 2; //!< Size of the size field in the header
		static const int DIR_MAX_QUANTITY = 64; //!< Maximum quantity of files in directory

		static const int FILE_MAX_SIZE = 32768; //!< Maximum size of the file

		//! This struct contains a name of the file
		struct Name
		{
			QVector<unsigned char> data = QVector<unsigned char>(NAME_MAX_LENGTH + 1); //!< Name of the file
		};

		//! This struct contains a size of the file
		struct Size
		{
			unsigned char low; //!< Lower byte of the size
			unsigned char high; //!< Higher byte of the size
		};

		//! This struct contains virtual block data
		struct Block
		{
			QVector<unsigned char> data = QVector<unsigned char>(BLOCK_SIZE); //!< Virtual block data
		};

		FS(QObject *parent = nullptr);

		void reset();

		void setPath(const QString &path);

		bool open(const QString &dirName, const FS::Name &fileName, FS::Size &size);
		bool list(const QString &dirName, FS::Size &size);

		void readBlock(FS::Block &block, int number);

	private:
		QString path; //!< Path to the emulated file system on the local disk
		QByteArray buffer; //!< Buffer used to loading whole file or a list with description of the files
};

#endif
