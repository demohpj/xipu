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

#ifndef UROM_H
#define UROM_H

#include "step.h"
#include "instruction.h"
#include "uromdata.h"

//! This class contains an uROM data
class URom
{
	public:
		URom();

		bool saveFiles(const QString &urom0Path, const QString &urom1Path);

	private:
		URomData uRomData; //!< uROM data with an opcode table
};

#endif
