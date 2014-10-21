/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy_blocksize.c
	Block size utility for fuzzy hashes


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
#include "ffuzzy_config.h"

#include <stdbool.h>
#include <limits.h>

#include "ffuzzy.h"

bool ffuzzy_blocksize_is_valid(unsigned long block_size)
{
	return block_size <= (ULONG_MAX / 2);
}

bool ffuzzy_blocksize_is_natural(unsigned long block_size)
{
	if (block_size < MIN_BLOCKSIZE)
		return false;
	while (block_size != MIN_BLOCKSIZE && !(block_size & 1ul))
		block_size >>= 1;
	return block_size == MIN_BLOCKSIZE;
}

bool ffuzzy_blocksize_is_near(unsigned long block_size1, unsigned long block_size2)
{
	return
		block_size1     == block_size2 ||
		block_size1 * 2 == block_size2 ||
		block_size2 * 2 == block_size1;
}
