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

/**
	\internal
	\file  ffuzzy_compare.c
	\brief Fuzzy hash comparison implementation
**/

#include "ffuzzy_config.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "ffuzzy.h"
#include "ffuzzy_blocksize.h"
#include "ffuzzy_parse.h"

#include "str_common_substr.h"
#include "str_edit_dist.h"
#include "util.h"

#if FFUZZY_SPAMSUM_LENGTH > EDIT_DISTN_MAXLEN
#error EDIT_DISTN_MAXLEN must be large enough to contain FFUZZY_SPAMSUM_LENGTH string
#endif
#if FFUZZY_SPAMSUM_LENGTH > HAS_COMMON_SUBSTR_MAXLEN
#error HAS_COMMON_SUBSTR_MAXLEN must be large enough to contain FFUZZY_SPAMSUM_LENGTH string
#endif


/**
	\internal
	\fn   int ffuzzy_score_cap_1_(int, unsigned long)
	\see  int ffuzzy_score_cap_1(int, unsigned long)
**/
static inline int ffuzzy_score_cap_1_(int minslen, unsigned long block_size)
{
	assert(minslen > 0 && minslen < FFUZZY_SPAMSUM_LENGTH);
	if (block_size >= FFUZZY_MIN_BLOCKSIZE * 100)
		return 100;
	return (int)block_size / FFUZZY_MIN_BLOCKSIZE * minslen;
}


inline int ffuzzy_score_cap_1(int minslen, unsigned long block_size)
{
	if (minslen == 0)
		return 0;
	return ffuzzy_score_cap_1_(minslen, block_size);
}


int ffuzzy_score_cap(int s1len, int s2len, unsigned long block_size)
{
	return ffuzzy_score_cap_1(MIN(s1len, s2len), block_size);
}


/**
	\internal
	\fn     int ffuzzy_score_strings_unsafe(const char*, size_t, const char*, size_t, unsigned long)
	\brief  Compute partial similarity score for given two block strings and block size (unsafe version)
	\param  [in] s1          Digest block 1
	\param       s1len       Length of s1
	\param  [in] s2          Digest block 2
	\param       s2len       Length of s2
	\param       block_size  Block size for two digest blocks
	\return [0,100] values represent partial similarity score or negative values on failure.
	\see    fuzzy_score_strings(const char*, size_t, const char*, size_t, unsigned long)
**/
static inline int ffuzzy_score_strings_unsafe(
	const char *s1, size_t s1len,
	const char *s2, size_t s2len,
	unsigned long block_size
)
{
	// the two strings must have a common substring
	// of length FFUZZY_MIN_MATCH to be candidates
	if (!has_common_substring(s1, s1len, s2, s2len))
		return 0;
	// compute the score by scaling edit distance by
	// the lengths of the two strings, and then
	// scale it to [0,100] scale (0 is the worst match)
	int score = edit_distn_norm(s1, s1len, s2, s2len) * FFUZZY_SPAMSUM_LENGTH / ((int)s1len + (int)s2len);
	score = 100 - (100 * score) / FFUZZY_SPAMSUM_LENGTH;
	// when the blocksize is small we don't want to exaggerate the match size
	if (block_size >= FFUZZY_MIN_BLOCKSIZE * 100)
	{
		// don't cap first (to avoid arithmetic overflow)
		return score;
	}
	int score_cap = (int)block_size / FFUZZY_MIN_BLOCKSIZE * MIN((int)s1len, (int)s2len);
	return MIN(score, score_cap);
}


int ffuzzy_score_strings(
	const char *s1, size_t s1len,
	const char *s2, size_t s2len,
	unsigned long block_size
)
{
	// cannot score long signatures
	if (s1len > FFUZZY_SPAMSUM_LENGTH || s2len > FFUZZY_SPAMSUM_LENGTH)
		return 0;
	return ffuzzy_score_strings_unsafe(s1, s1len, s2, s2len, block_size);
}


inline int ffuzzy_compare_digest_near(const ffuzzy_digest *d1, const ffuzzy_digest *d2)
{
	assert(ffuzzy_blocksize_is_near_(d1->block_size, d2->block_size));
	assert(ffuzzy_digest_is_valid(d1));
	assert(ffuzzy_digest_is_valid(d2));
	// special case if two signatures are identical
	if (
		d1->block_size == d2->block_size &&
		d1->len1 == d2->len1 &&
		d1->len2 == d2->len2 &&
		!memcmp(d1->digest, d2->digest, d1->len1 + d1->len2)
	)
	{
		// cap scores (same as ffuzzy_score_strings)
		int score_cap;
		if (d1->len2 >= FFUZZY_MIN_MATCH)
		{
			if (d1->block_size > FFUZZY_MIN_BLOCKSIZE * 50)
				return 100;
			score_cap = ffuzzy_score_cap_1_((int)d1->len2, d1->block_size * 2);
			if (score_cap >= 100)
				return 100;
		}
		else
			score_cap = 0;
		if (d1->len1 >= FFUZZY_MIN_MATCH)
		{
			int tmp = ffuzzy_score_cap_1_((int)d1->len1, d1->block_size);
			score_cap = MAX(score_cap, tmp);
		}
		return MIN(100, score_cap);
	}
	// each signature has a string for two block sizes. We now
	// choose how to combine the two block sizes. We checked above
	// that they have at least one block size in common
	if (d1->block_size <= (ULONG_MAX / 2))
	{
		if (d1->block_size == d2->block_size)
		{
			int score1 = ffuzzy_score_strings_unsafe(d1->digest, d1->len1, d2->digest, d2->len1, d1->block_size);
			int score2 = ffuzzy_score_strings_unsafe(d1->digest+d1->len1, d1->len2, d2->digest+d2->len1, d2->len2, d1->block_size * 2);
			return MAX(score1, score2);
		}
		else if (d1->block_size * 2 == d2->block_size)
			return ffuzzy_score_strings_unsafe(d1->digest + d1->len1, d1->len2, d2->digest, d2->len1, d2->block_size);
		else
			return ffuzzy_score_strings_unsafe(d1->digest, d1->len1, d2->digest + d2->len1, d2->len2, d1->block_size);
	}
	else
	{
		if (d1->block_size == d2->block_size) // second digest block is empty or invalid
			return ffuzzy_score_strings_unsafe(d1->digest, d1->len1, d2->digest, d2->len1, d1->block_size);
		else if (!(d1->block_size & 1ul) && (d1->block_size / 2 == d2->block_size))
			return ffuzzy_score_strings_unsafe(d1->digest, d1->len1, d2->digest + d2->len1, d2->len2, d1->block_size);
		else
			return 0;
	}
}


int ffuzzy_compare_digest_near_eq(const ffuzzy_digest *d1, const ffuzzy_digest *d2)
{
	assert(ffuzzy_digest_is_valid(d1));
	assert(ffuzzy_digest_is_valid(d2));
	assert(d1->block_size == d2->block_size);
	// special case if two signatures are identical
	if (
		d1->len1 == d2->len1 &&
		d1->len2 == d2->len2 &&
		!memcmp(d1->digest, d2->digest, d1->len1 + d1->len2)
	)
	{
		// cap scores (same as ffuzzy_score_strings)
		int score_cap;
		if (d1->len2 >= FFUZZY_MIN_MATCH)
		{
			if (d1->block_size > FFUZZY_MIN_BLOCKSIZE * 50)
				return 100;
			score_cap = ffuzzy_score_cap_1_((int)d1->len2, d1->block_size * 2);
			if (score_cap >= 100)
				return 100;
		}
		else
			score_cap = 0;
		if (d1->len1 >= FFUZZY_MIN_MATCH)
		{
			int tmp = ffuzzy_score_cap_1_((int)d1->len1, d1->block_size);
			score_cap = MAX(score_cap, tmp);
		}
		return MIN(100, score_cap);
	}
	if (d1->block_size <= (ULONG_MAX / 2))
	{
		int score1 = ffuzzy_score_strings_unsafe(d1->digest, d1->len1, d2->digest, d2->len1, d1->block_size);
		int score2 = ffuzzy_score_strings_unsafe(d1->digest+d1->len1, d1->len2, d2->digest+d2->len1, d2->len2, d1->block_size * 2);
		return MAX(score1, score2);
	}
	else
	{
		// second digest block is empty or invalid
		return ffuzzy_score_strings_unsafe(d1->digest, d1->len1, d2->digest, d2->len1, d1->block_size);
	}
}


int ffuzzy_compare_digest_near_lt(const ffuzzy_digest *d1, const ffuzzy_digest *d2)
{
	assert(ffuzzy_digest_is_valid(d1));
	assert(ffuzzy_digest_is_valid(d2));
	assert(d1->block_size <= (ULONG_MAX / 2));
	assert(d1->block_size * 2 == d2->block_size);
	return ffuzzy_score_strings_unsafe(d1->digest + d1->len1, d1->len2, d2->digest, d2->len1, d2->block_size);
}


int ffuzzy_compare_digest(const ffuzzy_digest *d1, const ffuzzy_digest *d2)
{
	// don't compare if the blocksizes are not close.
	if (!ffuzzy_blocksize_is_near_(d1->block_size, d2->block_size))
		return 0;
	return ffuzzy_compare_digest_near(d1, d2);
}


int ffuzzy_compare(const char *str1, const char *str2)
{
	ffuzzy_digest d1, d2;
	char *p1, *p2;
	// read blocksize part first
	if (!ffuzzy_read_digests_blocksize(&(d1.block_size), &p1, str1) || !ffuzzy_read_digests_blocksize(&(d2.block_size), &p2, str2))
		return -1;
	// don't compare if the blocksizes are not close.
	if (!ffuzzy_blocksize_is_near_(d1.block_size, d2.block_size))
		return 0;
	// read remaining parts
	if (!ffuzzy_read_digest_after_blocksize(&d1, p1) || !ffuzzy_read_digest_after_blocksize(&d2, p2))
		return -1;
	// then compare without blocksize checks
	return ffuzzy_compare_digest_near(&d1, &d2);
}
