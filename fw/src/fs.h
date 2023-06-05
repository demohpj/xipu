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

#ifndef _FS_H
#define _FS_H

#include "type.h"

#define FS_NAME_MAX_LENGTH 12 //!< Maximum name length of the file
#define FS_PATH_MAX_LENGTH ((FS_NAME_MAX_LENGTH * 2) + 1) //!< Maximum path length to the file

#define FS_BLOCK_SIZE 256 //!<  Virtual block size available to address by communication bus between motherboard and CPU

#define FS_DIR_ENTRY_SIZE 16 //!< Size of the header with information about file
#define FS_DIR_SIZE_FIELD_SIZE 2 //!< Size of the size field in the header
#define FS_DIR_MAX_QUANTITY 64 //!< Maximum quantity of files in directory

//! This struct contains a name of the file
struct FSName
{
	unsigned char data[FS_NAME_MAX_LENGTH + 1]; //!< Name of the file
};

//! This struct contains a size of the file
struct FSSize
{
	unsigned char low; //!< Lower byte of the size
	unsigned char high; //!< Higher byte of the size
};

//! This struct contains virtual block data
struct FSBlock
{
	unsigned char data[FS_BLOCK_SIZE]; //!< Virtual block data
};

void fsInit(void);

bool fsOpen(const char *dirName, const struct FSName *fileName, struct FSSize *size);
bool fsList(const char *dirName, struct FSSize *size);

void fsReadBlock(struct FSBlock *block, int number);

#endif
