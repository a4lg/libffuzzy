/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy_compare.c
	Fuzzy hash comparison implementation


	CREDITS OF ORIGINAL VERSION OF SSDEEP

	Copyright (C) 2002 Andrew Tridgell <tridge@samba.org>
	Copyright (C) 2006 ManTech International Corporation
	Copyright (C) 2013 Helmut Grohne <helmut@subdivi.de>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


	CREDIT OF MODIFIED PORTIONS

	Copyright (C) 2014 Tsukasa OI <li@livegrid.org>

*/
#include "ffuzzy_config.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "ffuzzy.h"

#include "str_common_substr.h"
#include "str_edit_dist.h"
#include "util.h"

#if SPAMSUM_LENGTH > EDIT_DISTN_MAXLEN
#error EDIT_DISTN_MAXLEN must be large enough to contain SPAMSUM_LENGTH string
#endif
#if SPAMSUM_LENGTH > HAS_COMMON_SUBSTR_MAXLEN
#error HAS_COMMON_SUBSTR_MAXLEN must be large enough to contain SPAMSUM_LENGTH string
#endif


inline int ffuzzy_score_cap_1(int minslen, unsigned long block_size)
{
	unsigned long block_scale = block_size / MIN_BLOCKSIZE;
	if (block_scale >= 100)
		return 100;
	return (int)block_scale * minslen;
}


inline int ffuzzy_score_cap(int s1len, int s2len, unsigned long block_size)
{
	return ffuzzy_score_cap_1(MIN(s1len, s2len), block_size);
}


int ffuzzy_score_strings(
	const char *s1, size_t s1len,
	const char *s2, size_t s2len,
	unsigned long block_size
)
{
	// cannot score long signatures
	if (s1len > SPAMSUM_LENGTH || s2len > SPAMSUM_LENGTH)
		return 0;
	// the two strings must have a common substring
	// of length ROLLING_WINDOW to be candidates
	if (!has_common_substring(s1, s1len, s2, s2len))
		return 0;
	// compute the score by scaling edit distance by
	// the lengths of the two strings, and then
	// scale it to [0,100] scale (0 is the worst match)
	int score = edit_distn_norm(s1, s1len, s2, s2len) * SPAMSUM_LENGTH / ((int)s1len + (int)s2len);
	score = 100 - (100 * score) / SPAMSUM_LENGTH;
	// when the blocksize is small we don't want to exaggerate the match size
	unsigned long block_scale = block_size / MIN_BLOCKSIZE;
	if (block_scale >= 100)
	{
		// don't cap first (to avoid arithmetic overflow)
		return score;
	}
	int score_cap = (int)block_scale * MIN((int)s1len, (int)s2len);
	return MIN(score, score_cap);
}


static inline bool ffuzzy_read_digest_blocksize(ffuzzy_digest *digest, char** srem, const char *s)
{
	errno = 0;
	digest->block_size = strtoul(s, srem, 10);
	// arithmetic overflow occurred
	if (digest->block_size == ULONG_MAX && errno == ERANGE)
		return false;
	// the string does not start with numbers
	if (*srem == s)
		return false;
	return true;
}


static inline bool ffuzzy_read_digest_after_blocksize(ffuzzy_digest *digest, const char *s)
{
	// ':' must follow after the number (which is block_size)
	if (*s != ':')
		return false;
	// read first block of ssdeep hash
	// (eliminating sequences of 4 or more identical characters)
	digest->size2 = 0;
	char *o = digest->digest;
	while (true)
	{
		char c = *++s;
		if (!c)
			return false;
		if (c == ':')
			break;
		if (digest->size2 < 3 || c != s[-1] || c != s[-2] || c != s[-3])
		{
			if (digest->size2 == SPAMSUM_LENGTH)
				return false;
			digest->size2++;
			*o++ = c;
		}
	}
	// read second block of ssdeep hash
	// (eliminating sequences of 4 or more identical characters)
	digest->size1 = digest->size2;
	while (true)
	{
		char c = *++s;
		if (!c || c == ',')
			break;
		if (digest->size2 < 3 || c != s[-1] || c != s[-2] || c != s[-3])
		{
			if (digest->size2 == digest->size1 + SPAMSUM_LENGTH)
				return false;
			digest->size2++;
			*o++ = c;
		}
	}
	digest->size2 -= digest->size1;
	return true;
}


inline bool ffuzzy_read_digest(ffuzzy_digest *digest, const char *s)
{
	char *p;
	if (!ffuzzy_read_digest_blocksize(digest, &p, s))
		return false;
	return ffuzzy_read_digest_after_blocksize(digest, p);
}


// skip block size checks and compare two digest
inline int ffuzzy_compare_digest_near(const ffuzzy_digest *d1, const ffuzzy_digest *d2)
{
	// special case if two signatures are identical
	if (
		d1->block_size == d2->block_size &&
		d1->size1 == d2->size1 &&
		d1->size2 == d2->size2 &&
		!memcmp(d1->digest, d2->digest, d1->size1 + d1->size2)
	)
	{
		// cap scores (same as ffuzzy_score_strings)
		int score_cap;
		if (d1->size2 >= ROLLING_WINDOW)
		{
			score_cap = ffuzzy_score_cap_1((int)d1->size2, d1->block_size * 2);
			if (score_cap >= 100)
				return 100;
		}
		else
			score_cap = 0;
		if (d1->size1 >= ROLLING_WINDOW)
		{
			int tmp = ffuzzy_score_cap_1((int)d1->size1, d1->block_size);
			score_cap = MAX(score_cap, tmp);
		}
		return MIN(100, score_cap);
	}
	// each signature has a string for two block sizes. We now
	// choose how to combine the two block sizes. We checked above
	// that they have at least one block size in common
	if (d1->block_size == d2->block_size)
	{
		int score1 = ffuzzy_score_strings(d1->digest, d1->size1, d2->digest, d2->size1, d1->block_size);
		int score2 = ffuzzy_score_strings(d1->digest+d1->size1, d1->size2, d2->digest+d2->size1, d2->size2, d1->block_size * 2);
		return MAX(score1, score2);
	}
	else if (d1->block_size == d2->block_size * 2)
		return ffuzzy_score_strings(d1->digest, d1->size1, d2->digest + d2->size1, d2->size2, d1->block_size);
	else
		return ffuzzy_score_strings(d1->digest + d1->size1, d1->size2, d2->digest, d2->size1, d2->block_size);
}


inline int ffuzzy_compare_digest(const ffuzzy_digest *d1, const ffuzzy_digest *d2)
{
	// don't compare if the blocksizes are not close.
	if (
		d1->block_size != d2->block_size     &&
		d2->block_size != d1->block_size * 2 &&
		d1->block_size != d2->block_size * 2
	)
	{
		return 0;
	}
	return ffuzzy_compare_digest_near(d1, d2);
}


int ffuzzy_compare(const char *str1, const char *str2)
{
	ffuzzy_digest d1, d2;
	char *p1, *p2;
	// read blocksize part first
	if (!ffuzzy_read_digest_blocksize(&d1, &p1, str1) || !ffuzzy_read_digest_blocksize(&d2, &p2, str2))
		return -1;
	// don't compare if the blocksizes are not close.
	if (
		d1.block_size != d2.block_size     &&
		d2.block_size != d1.block_size * 2 &&
		d1.block_size != d2.block_size * 2
	)
	{
		return 0;
	}
	// read remaining parts
	if (!ffuzzy_read_digest_after_blocksize(&d1, p1) || !ffuzzy_read_digest_after_blocksize(&d2, p2))
		return -1;
	// then compare without blocksize checks
	return ffuzzy_compare_digest_near(&d1, &d2);
}
