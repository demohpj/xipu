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

#ifndef _FLASH_H
#define _FLASH_H

#include "type.h"

#define FLASH_SECTOR_SIZE 512 //!< Sector size
#define FLASH_SECTOR_QUANTITY 4096 //!< Sector quantity

#define FLASH_SIZE (FLASH_SECTOR_SIZE * FLASH_SECTOR_QUANTITY) //!< Total flash size

void flashInit(void);

bool flashRead(unsigned int sector, unsigned char *buffer);
bool flashWrite(unsigned int sector, unsigned char *buffer);

#endif
