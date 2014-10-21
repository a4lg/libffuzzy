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
#include "ffuzzy_config.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ffuzzy.h"
#include "ffuzzy_blocksize.h"

inline bool ffuzzy_digest_is_valid_lengths(const ffuzzy_digest *digest)
{
	return
		digest->size1 <= FFUZZY_SPAMSUM_LENGTH &&
		digest->size2 <= FFUZZY_SPAMSUM_LENGTH &&
		(digest->size1 + digest->size2) <= FFUZZY_SPAMSUM_LENGTH * 2;
}

static inline bool is_base64(char c)
{
	switch (c)
	{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z':
		case '+': case '/':
			return true;
		default:
			return false;
	}
}

bool ffuzzy_digest_is_valid_buffer(const ffuzzy_digest *digest)
{
	const char *buf = digest->digest;
	for (size_t i = 3; i < digest->size1; i++, buf++)
		if (buf[0] == buf[1] && buf[0] == buf[2] && buf[0] == buf[3])
			return false;
	for (size_t i = 3; i < digest->size2; i++, buf++)
		if (buf[0] == buf[1] && buf[0] == buf[2] && buf[0] == buf[3])
			return false;
	return true;
}

bool ffuzzy_digest_is_natural_buffer(const ffuzzy_digest *digest)
{
	const char *buf = digest->digest;
	for (size_t i = 0; i < digest->size1 && i < 3; i++)
		if (!is_base64(buf[i]))
			return false;
	for (size_t i = 3; i < digest->len1; i++, buf++)
		if (!is_base64(buf[3]) || (buf[0] == buf[1] && buf[0] == buf[2] && buf[0] == buf[3]))
			return false;
	for (size_t i = 0; i < digest->size2 && i < 3; i++, buf++)
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
		ffuzzy_blocksize_is_valid_(digest->block_size) &&
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
	if (d1->size1 > d2->size1)
		return +1;
	if (d1->size1 < d2->size1)
		return -1;
	if (d1->size2 > d2->size2)
		return +1;
	if (d1->size2 < d2->size2)
		return -1;
	return memcmp(d1->digest, d2->digest, d1->size1 + d1->size2);
}

bool ffuzzy_pretty_digest(char *buf, size_t buflen, const ffuzzy_digest *digest)
{
	// pretty hash contains two colons and trailing '\0'
	if (buflen < 3)
		return false;
	// buf must be big enough to contain two colons and two buffers.
	if ((buflen - 3) < (digest->size1 + digest->size2))
		return false;
	// write block size if possible
	{
		size_t bslen = (buflen - 2) - (digest->size1 + digest->size2);
		int bsret = snprintf(buf, bslen, "%lu", digest->block_size);
		if (bsret < 0)
			return false;
		if ((size_t)bsret >= bslen)
			return false;
		buf += bslen;
	}
	// write blocks
	buf[0] = ':';
	memcpy(buf + 1, digest->digest, digest->size1);
	buf[digest->size1 + 1] = ':';
	memcpy(buf + digest->size1 + 1, digest->digest + digest->size1, digest->size2);
	buf[digest->size1 + digest->size2 + 2] = '\0';
	return true;
}
