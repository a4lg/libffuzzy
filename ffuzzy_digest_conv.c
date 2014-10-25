/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy_digest_conv.c
	Digest form converter (from/to unnormalized form)


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

/**
	\internal
	\file  ffuzzy_digest_conv.c
	\brief Digest form converter (from/to unnormalized form)
**/

#include "ffuzzy_config.h"

#include <assert.h>
#include <string.h>

#include "ffuzzy.h"


void ffuzzy_convert_digest_to_udigest(ffuzzy_udigest *udigest, const ffuzzy_digest *digest)
{
	assert(ffuzzy_digest_is_valid(digest));
	udigest->len1 = digest->len1;
	udigest->len2 = digest->len2;
	udigest->block_size = digest->block_size;
	memcpy(udigest->digest, digest->digest, digest->len1 + digest->len2);
}

void ffuzzy_convert_udigest_to_digest(ffuzzy_digest *digest, const ffuzzy_udigest *udigest)
{
	assert(ffuzzy_udigest_is_valid(udigest));
	digest->block_size = udigest->block_size;
	char *o = digest->digest;
	// eliminate sequences for digest block 1
	if (udigest->len1 <= 3)
	{
		memcpy(o, udigest->digest, udigest->len1);
		digest->len1 = udigest->len1;
		o += digest->len1;
	}
	else
	{
		memcpy(o, udigest->digest, 3);
		digest->len1 = 3;
		o += 3;
		for (size_t i = 3; i < udigest->len1; i++)
		{
			if (
				udigest->digest[i] != o[-3] ||
				udigest->digest[i] != o[-2] ||
				udigest->digest[i] != o[-1]
			)
			{
				*o++ = udigest->digest[i];
				digest->len1++;
			}
		}
	}
	// eliminate sequences for digest block 2
	if (udigest->len2 <= 3)
	{
		memcpy(o, udigest->digest + udigest->len1, udigest->len2);
		digest->len2 = udigest->len2;
	}
	else
	{
		memcpy(o, udigest->digest + udigest->len1, 3);
		digest->len2 = 3;
		o += 3;
		for (size_t i = 3; i < udigest->len2; i++)
		{
			if (
				udigest->digest[udigest->len1 + i] != o[-3] ||
				udigest->digest[udigest->len1 + i] != o[-2] ||
				udigest->digest[udigest->len1 + i] != o[-1]
			)
			{
				*o++ = udigest->digest[udigest->len1 + i];
				digest->len2++;
			}
		}
	}
}
