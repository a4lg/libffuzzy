/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy_digest.c
	Digest utility for fuzzy hashes


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
	\file  ffuzzy_digest.c
	\brief Digest utility for fuzzy hashes
**/

#include "ffuzzy_config.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ffuzzy.h"
#include "ffuzzy_blocksize.h"
#include "str_base64.h"


inline bool ffuzzy_digest_is_valid_lengths(const ffuzzy_digest *digest)
{
	return
		digest->len1 <= FFUZZY_SPAMSUM_LENGTH &&
		digest->len2 <= FFUZZY_SPAMSUM_LENGTH &&
		(digest->len1 + digest->len2) <= FFUZZY_SPAMSUM_LENGTH * 2;
}


bool ffuzzy_digest_is_valid_buffer(const ffuzzy_digest *digest)
{
	assert(fuzzy_digest_is_valid_lengths(digest));
	const char *buf = digest->digest;
	for (size_t i = 3; i < digest->len1; i++, buf++)
		if (buf[0] == buf[1] && buf[0] == buf[2] && buf[0] == buf[3])
			return false;
	for (size_t i = 3; i < digest->len2; i++, buf++)
		if (buf[0] == buf[1] && buf[0] == buf[2] && buf[0] == buf[3])
			return false;
	return true;
}


bool ffuzzy_digest_is_natural_buffer(const ffuzzy_digest *digest)
{
	assert(fuzzy_digest_is_valid_lengths(digest));
	const char *buf = digest->digest;
	for (size_t i = 0; i < digest->len1 && i < 3; i++)
		if (!is_base64(buf[i]))
			return false;
	for (size_t i = 3; i < digest->len1; i++, buf++)
		if (!is_base64(buf[3]) || (buf[0] == buf[1] && buf[0] == buf[2] && buf[0] == buf[3]))
			return false;
	for (size_t i = 0; i < digest->len2 && i < 3; i++, buf++)
		if (!is_base64(buf[3]))
			return false;
	for (size_t i = 3; i < digest->len2; i++, buf++)
		if (!is_base64(buf[3]) || (buf[0] == buf[1] && buf[0] == buf[2] && buf[0] == buf[3]))
			return false;
	return true;
}


bool ffuzzy_digest_is_valid(const ffuzzy_digest *digest)
{
	return
		ffuzzy_digest_is_valid_lengths(digest) &&
		ffuzzy_digest_is_valid_buffer(digest);
}


bool ffuzzy_digest_is_natural(const ffuzzy_digest *digest)
{
	return
		ffuzzy_blocksize_is_natural_(digest->block_size) &&
		ffuzzy_digest_is_valid_lengths(digest) &&
		ffuzzy_digest_is_natural_buffer(digest);
}


int ffuzzy_digestcmp(const ffuzzy_digest *d1, const ffuzzy_digest *d2)
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


int ffuzzy_digestcmp_blocksize(const ffuzzy_digest *d1, const ffuzzy_digest *d2)
{
	return ffuzzy_blocksizecmp(d1->block_size, d2->block_size);
}


int ffuzzy_digestcmp_blocksize_n(const ffuzzy_digest *d1, const ffuzzy_digest *d2)
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


bool ffuzzy_pretty_digest(char *buf, size_t buflen, const ffuzzy_digest *digest)
{
	assert(fuzzy_digest_is_valid_lengths(digest));
	// pretty hash contains two colons and trailing '\0'
	if (buflen < 3)
		return false;
	// buf must be big enough to contain two colons and two buffers.
	if ((buflen - 3) < (digest->len1 + digest->len2))
		return false;
	// write block size if possible
	{
		size_t bslen = (buflen - 2) - (digest->len1 + digest->len2);
		int bsret = snprintf(buf, bslen, "%lu", digest->block_size);
		if (bsret < 0)
			return false;
		if ((size_t)bsret >= bslen)
			return false;
		buf += bsret;
	}
	// write blocks
	buf[0] = ':';
	memcpy(buf + 1, digest->digest, digest->len1);
	buf[digest->len1 + 1] = ':';
	memcpy(buf + digest->len1 + 2, digest->digest + digest->len1, digest->len2);
	buf[digest->len1 + digest->len2 + 2] = '\0';
	return true;
}
