/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy_digest_unnorm.c
	Digest utility for fuzzy hashes (unnormalized form)


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
	\file  ffuzzy_digest_unnorm.c
	\brief Digest utility for fuzzy hashes (unnormalized form)
**/

#include "ffuzzy_config.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ffuzzy.h"
#include "ffuzzy_blocksize.h"
#include "str_base64.h"


inline bool ffuzzy_udigest_is_valid_lengths(const ffuzzy_udigest *udigest)
{
	return
		udigest->len1 <= FFUZZY_SPAMSUM_LENGTH &&
		udigest->len2 <= FFUZZY_SPAMSUM_LENGTH &&
		(udigest->len1 + udigest->len2) <= FFUZZY_SPAMSUM_LENGTH * 2;
}


bool ffuzzy_udigest_is_natural_buffer(const ffuzzy_udigest *udigest)
{
	assert(fuzzy_udigest_is_valid_lengths(udigest));
	for (size_t i = 0; i < udigest->len1 + udigest->len2; i++)
		if (!is_base64(udigest->digest[i]))
			return false;
	return true;
}


bool ffuzzy_udigest_is_valid(const ffuzzy_udigest *udigest)
{
	return ffuzzy_udigest_is_valid_lengths(udigest);
}


bool ffuzzy_udigest_is_natural(const ffuzzy_udigest *udigest)
{
	return
		ffuzzy_blocksize_is_natural_(udigest->block_size) &&
		ffuzzy_udigest_is_valid_lengths(udigest) &&
		ffuzzy_udigest_is_natural_buffer(udigest);
}


int ffuzzy_udigestcmp(const ffuzzy_udigest *d1, const ffuzzy_udigest *d2)
{
	if (d1->block_size > d2->block_size)
		return +1;
	if (d1->block_size < d2->block_size)
		return -1;
	if (d1->len1 > d2->len1)
		return +1;
	if (d1->len1 < d2->len1)
		return -1;
	if (d1->len2 > d2->len2)
		return +1;
	if (d1->len2 < d2->len2)
		return -1;
	return memcmp(d1->digest, d2->digest, d1->len1 + d1->len2);
}


int ffuzzy_udigestcmp_blocksize(const ffuzzy_udigest *d1, const ffuzzy_udigest *d2)
{
	return ffuzzy_blocksizecmp(d1->block_size, d2->block_size);
}


int ffuzzy_udigestcmp_blocksize_n(const ffuzzy_udigest *d1, const ffuzzy_udigest *d2)
{
	bool nat1 = ffuzzy_blocksize_is_natural_(d1->block_size);
	bool nat2 = ffuzzy_blocksize_is_natural_(d2->block_size);
	if (nat1 == nat2)
		return ffuzzy_blocksizecmp(d1->block_size, d2->block_size);
	else
	{
		if (!nat1 && nat2)
			return +1;
		else
			return -1;
	}
}


bool ffuzzy_pretty_udigest(char *buf, size_t buflen, const ffuzzy_udigest *udigest)
{
	assert(fuzzy_udigest_is_valid_lengths(udigest));
	// pretty hash contains two colons and trailing '\0'
	if (buflen < 3)
		return false;
	// buf must be big enough to contain two colons and two buffers.
	if ((buflen - 3) < (udigest->len1 + udigest->len2))
		return false;
	// write block size if possible
	{
		size_t bslen = (buflen - 2) - (udigest->len1 + udigest->len2);
		int bsret = snprintf(buf, bslen, "%lu", udigest->block_size);
		if (bsret < 0)
			return false;
		if ((size_t)bsret >= bslen)
			return false;
		buf += bsret;
	}
	// write blocks
	buf[0] = ':';
	memcpy(buf + 1, udigest->digest, udigest->len1);
	buf[udigest->len1 + 1] = ':';
	memcpy(buf + udigest->len1 + 2, udigest->digest + udigest->len1, udigest->len2);
	buf[udigest->len1 + udigest->len2 + 2] = '\0';
	return true;
}
