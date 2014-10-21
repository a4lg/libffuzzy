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

#include "ffuzzy_config.h"

#include <stdbool.h>
#include <limits.h>

#include "ffuzzy.h"

static inline bool ffuzzy_blocksize_is_valid_(unsigned long block_size)
{
	return block_size <= (ULONG_MAX / 2);
}

static inline bool ffuzzy_blocksize_is_natural_(unsigned long block_size)
{
	if (block_size < FFUZZY_MIN_BLOCKSIZE)
		return false;
	if (!ffuzzy_blocksize_is_valid(block_size))
		return false;
	while (block_size != FFUZZY_MIN_BLOCKSIZE && !(block_size & 1ul))
		block_size >>= 1;
	return block_size == FFUZZY_MIN_BLOCKSIZE;
}

#endif
