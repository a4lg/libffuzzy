/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy_blocksize.h
	Block size utility for fuzzy hashes (internal)


	Copyright (C) 2014 Tsukasa OI <li@livegrid.org>


	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef FFUZZY_FFUZZY_BLOCKSIZE_H
#define FFUZZY_FFUZZY_BLOCKSIZE_H

/**
	\internal
	\file  ffuzzy_blocksize.h
	\brief Block size utility for fuzzy hashes
**/

#include "ffuzzy_config.h"

#include <assert.h>
#include <stdbool.h>
#include <limits.h>

#include "ffuzzy.h"

/**
	\internal
	\fn   bool ffuzzy_blocksize_is_natural_(unsigned long)
	\see  bool ffuzzy_blocksize_is_natural(unsigned long)
**/
static inline bool ffuzzy_blocksize_is_natural_(unsigned long block_size)
{
	if (block_size < FFUZZY_MIN_BLOCKSIZE)
		return false;
	while (block_size != FFUZZY_MIN_BLOCKSIZE && !(block_size & 1ul))
		block_size >>= 1;
	return block_size == FFUZZY_MIN_BLOCKSIZE;
}

/**
	\internal
	\fn   bool ffuzzy_blocksize_is_near_(unsigned long, unsigned long)
	\see  bool ffuzzy_blocksize_is_near(unsigned long, unsigned long)
**/
static inline bool ffuzzy_blocksize_is_near_(unsigned long block_size1, unsigned block_size2)
{
	return (
		block_size1 == block_size2 ||
		(block_size1 <= (ULONG_MAX / 2) && block_size1 * 2 == block_size2) ||
		(!(block_size1 & 1ul) && block_size1 / 2 == block_size2)
	);
}


/**
	\internal
	\fn     int ffuzzy_blocksizecmp(unsigned long, unsigned long)
	\brief  Compare two block size values
	\param  block_size1  Block size 1
	\param  block_size2  Block size 2
	\return
		Positive value if block_size1 < block_size2, negative value if block_size1 > block_size2
		and 0 if block_size1 is equal to block_size2.
**/
static inline int ffuzzy_blocksizecmp(unsigned long block_size1, unsigned long block_size2)
{
	if (block_size1 > block_size2)
		return +1;
	if (block_size1 < block_size2)
		return -1;
	return 0;
}

#endif
