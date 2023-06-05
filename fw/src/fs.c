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

#include "fatfs/ff_gen_drv.h"

#include "type.h"
#include "flash.h"

#include "fs.h"

//! File system block type list
enum FSReadBlockType
{
	FS_READ_BLOCK_UNKNOWN = 0, //!< Unknown type
	FS_READ_BLOCK_FILE, //!< File type
	FS_READ_BLOCK_DIR, //!< Directory type
};

static void fsMakePath(char *path, const char *dirName, const char *fileName);

static DSTATUS fsFatInit(BYTE pdrv);
static DSTATUS fsFatGetStatus(BYTE pdrv);
static DRESULT fsFatRead(BYTE pdrv, BYTE *buf, DWORD sector, UINT count);

//! File system handlers to functions
static Diskio_drvTypeDef fatDriver =
{
	fsFatInit,
	fsFatGetStatus,
	fsFatRead
};

static char fatDiskPath[4]; //!< Path to the FAT file system
static FATFS fatFS; //!< Descriptor of the FAT file system

static FRESULT fatMountStatus = FR_NOT_READY; //!< Mount status of the physical drive
static FRESULT fatOpenStatus = FR_NOT_READY; //!< Open status of the file

static FIL fatFile; //!< Descriptor of the file

static int readBlockType = FS_READ_BLOCK_UNKNOWN; //!< Type of reading block

static unsigned char dirBuffer[FS_DIR_ENTRY_SIZE * FS_DIR_MAX_QUANTITY]; //!< Directory entrys buffer

//! Init file system and set up the FAT
void fsInit(void)
{
	if(!FATFS_LinkDriver(&fatDriver, fatDiskPath))
	{
		fatMountStatus = f_mount(&fatFS, fatDiskPath, 1);
	}
}

/**
 * Open a file from the file system
 *
 * @param dirName Name of the directory. "app" or "file".
 * @param fileName Name of the file to open
 * @param size Struct where size of the file will be written
 *
 * @return Status of the open operation
 */
bool fsOpen(const char *dirName, const struct FSName *fileName, struct FSSize *size)
{
	size->low = 0;
	size->high = 0;
	
	readBlockType = FS_READ_BLOCK_FILE;
	
	if(!fatMountStatus)
	{
		char path[FS_PATH_MAX_LENGTH + 1];
		
		fsMakePath(path, dirName, (const char *)(fileName->data));
		
		fatOpenStatus = f_open(&fatFile, path, FA_READ);
		
		if(!fatOpenStatus)
		{
			DWORD fileSize = f_size(&fatFile);
			
			size->low = (fileSize % 256);
			size->high = ((fileSize / 256) & 0xff);
			
			return(TRUE);
		}
	}

	return(FALSE);
}

/**
 * Create a list of files in the directory from the file system
 *
 * @param dirName Name of the directory. "app" or "file".
 * @param size Struct where size of the buffer with description of the files will be written
 *
 * @return Status of the list operation
 */
bool fsList(const char *dirName, struct FSSize *size)
{
	size->low = 0;
	size->high = 0;
	
	readBlockType = FS_READ_BLOCK_DIR;
	
	if(!fatMountStatus)
	{
		char path[FS_PATH_MAX_LENGTH + 1];
		DIR dir;
		FILINFO info;
		
		fsMakePath(path, dirName, NULL);
		
		if(!f_opendir(&dir, path))
		{
			int dirSize = 0;
			
			while(1)
			{
				if(f_readdir(&dir, &info) != FR_OK)
				{
					break;
				}
				
				if(info.fname[0] == 0)
				{
					break;
				}
				
				for(int i = 0; i < FS_NAME_MAX_LENGTH; i++)
				{
					dirBuffer[dirSize + i] = info.fname[i];
				}
				
				dirBuffer[dirSize + FS_NAME_MAX_LENGTH] = 0;
				
				dirBuffer[dirSize + FS_DIR_ENTRY_SIZE - 2] = (info.fsize % 256);
				dirBuffer[dirSize + FS_DIR_ENTRY_SIZE - 1] = ((info.fsize / 256) & 0xff);
				
				dirSize += FS_DIR_ENTRY_SIZE;
				
				if(dirSize >= (FS_DIR_ENTRY_SIZE * FS_DIR_MAX_QUANTITY))
				{
					break;
				}
			}
			
			size->low = ((dirSize) % 256);
			size->high = (((dirSize) / 256) & 0xff);
			
			return(TRUE);
		}
	}
	
	return(FALSE);
}

/**
 * Read data from buffer to block based on offset
 *
 * @param block Struct where the data from the buffer will be written
 * @param number Number of the block selected to read
 */
void fsReadBlock(struct FSBlock *block, int number)
{
	if((!fatMountStatus) && (!fatOpenStatus))
	{
		if(readBlockType == FS_READ_BLOCK_FILE)
		{
			if(!f_lseek(&fatFile, (number * FS_BLOCK_SIZE)))
			{
				UINT read;
				
				f_read(&fatFile, block->data, 256, &read);
			}
		}
		else if(readBlockType == FS_READ_BLOCK_DIR)
		{
			for(int i = 0; i < FS_BLOCK_SIZE; i++)
			{
				block->data[i] = 0;
			}
			
			if(((number * FS_BLOCK_SIZE) / FS_DIR_ENTRY_SIZE) < (FS_DIR_MAX_QUANTITY))
			{
				int offset = (number * FS_BLOCK_SIZE);
				
				for(int i = 0; i < FS_BLOCK_SIZE; i++)
				{
					block->data[i] = dirBuffer[offset + i];
				}
			}	
		}
	}
}

/**
 * Generate a full path to file
 *
 * @param path Buffer to write
 * @param dirName 
 * @param fileName
 */
static void fsMakePath(char *path, const char *dirName, const char *fileName)
{
	path[0] = '/';
	
	int i = 1;
	
	for(int j = 0; dirName[j]; j++, i++)
	{
		path[i] = dirName[j];
	}

	if(fileName)
	{
		path[i] = '/';
		i++;
		
		for(int j = 0; fileName[j]; j++, i++)
		{
			path[i] = fileName[j];
		}
	}

	path[i] = 0;
}

/**
 * Get status of init
 *
 * @param pdrv Physical drive number
 *
 * @return Status of init
 */
static DSTATUS fsFatInit(BYTE pdrv)
{
	return(0);
}

/**
 * Get status of the physical drive
 *
 * @param pdrv Physical drive number
 *
 * @return Status of the physical drive
 */
static DSTATUS fsFatGetStatus(BYTE pdrv)
{
	return(0);
}

/**
 * Read sector of the physical drive
 *
 * @param pdrv Physical drive number
 * @param buf Buffer to write
 * @param sector Sector number to read
 * @param count Count of sectors to read
 *
 * @return Status of the reading process
 */
static DRESULT fsFatRead(BYTE pdrv, BYTE *buf, DWORD sector, UINT count)
{
	for(uint32_t i = 0; i < count; i++)
	{
		if(!flashRead((sector + i), (buf + (FLASH_SECTOR_SIZE * i))))
		{
			return(RES_ERROR);
		}
	}
	
	return(RES_OK);
}
