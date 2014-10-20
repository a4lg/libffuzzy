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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ffuzzy.h"

#include "str_common_substr.h"
#include "str_edit_dist.h"
#include "str_elim_seq.h"
#include "util.h"

#if SPAMSUM_LENGTH > EDIT_DISTN_MAXLEN
#error EDIT_DISTN_MAXLEN must be large enough to contain SPAMSUM_LENGTH string
#endif
#if SPAMSUM_LENGTH > HAS_COMMON_SUBSTR_MAXLEN
#error HAS_COMMON_SUBSTR_MAXLEN must be large enough to contain SPAMSUM_LENGTH string
#endif


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
	int score = edit_distn(s1, s1len, s2, s2len) * SPAMSUM_LENGTH / ((int)s1len + (int)s2len);
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


int ffuzzy_compare(const char *str1, const char *str2)
{
	char s1a[SPAMSUM_LENGTH], s1b[SPAMSUM_LENGTH], s2a[SPAMSUM_LENGTH], s2b[SPAMSUM_LENGTH];
	size_t s1la, s1lb, s2la, s2lb;
	unsigned long block_size1, block_size2;

	if (!str1 || !str2)
		return -1;
	// scan blocksizes
	if (
		sscanf(str1, "%lu:", &block_size1) != 1 ||
		sscanf(str2, "%lu:", &block_size2) != 1
	)
	{
		return -1;
	}
	// don't compare if the blocksizes are not close.
	if (
		block_size1 != block_size2     &&
		block_size1 != block_size2 * 2 &&
		block_size2 != block_size1 * 2
	)
	{
		return 0;
	}
	// read chunks and format
	{
		char *p1a = strchr(str1, ':');
		char *p2a = strchr(str2, ':');
		if (!p1a || !p2a)
			return -1;
		char *p1b = strchr(++p1a, ':');
		char *p2b = strchr(++p2a, ':');
		if (!p1b || !p2b)
			return -1;
		char *p1c = strchr(++p1b, ',');
		char *p2c = strchr(++p2b, ',');
		if (!p1c) p1c = strchr(p1b, '\0');
		if (!p2c) p2c = strchr(p2b, '\0');
		// we can't compare such long hashes (original version of ssdeep may)
		// INCOMPATIBILITY:
		// this function will not accept some long hashes that ssdeep may.
		// note that such hashes will not be generated from ssdeep.
		if ((p1b - p1a - 1) > SPAMSUM_LENGTH || (p2b - p2a - 1) > SPAMSUM_LENGTH)
			return -1;
		if ((p1c - p1b) > SPAMSUM_LENGTH || (p2c - p2b) > SPAMSUM_LENGTH)
			return -1;
		// remove sequences of the same character (longer than 3)
		s1la = eliminate_sequences(s1a, p1a, p1b - p1a - 1);
		s2la = eliminate_sequences(s2a, p2a, p2b - p2a - 1);
		s1lb = eliminate_sequences(s1b, p1b, p1c - p1b);
		s2lb = eliminate_sequences(s2b, p2b, p2c - p2b);
	}
	// special case if two signatures are identical
	if (
		block_size1 == block_size2 &&
		s1la == s2la && s1lb == s2lb &&
		!memcmp(s1a, s2a, s1la) &&
		!memcmp(s1b, s2b, s1lb)
	)
	{
		// cap maximum score
		unsigned long block_scale2 = block_size1 * 2 / MIN_BLOCKSIZE;
		if (s1lb >= ROLLING_WINDOW && block_scale2 >= 100)
			return 100;
		unsigned long block_scale1 = block_size1 / MIN_BLOCKSIZE;
		if (s1la >= ROLLING_WINDOW && block_scale1 >= 100)
			return 100;
		int score_cap1 = (int)(block_scale1 * s1la);
		if (s1la < ROLLING_WINDOW)
			score_cap1 = 0;
		int score_cap2 = (int)(block_scale2 * s1lb);
		if (s1lb < ROLLING_WINDOW)
			score_cap2 = 0;
		int score_cap = MAX(score_cap1, score_cap2);
		return MIN(100, score_cap);
	}
	// each signature has a string for two block sizes. We now
	// choose how to combine the two block sizes. We checked above
	// that they have at least one block size in common
	if (block_size1 == block_size2)
	{
		int score1 = ffuzzy_score_strings(s1a, s1la, s2a, s2la, block_size1);
		int score2 = ffuzzy_score_strings(s1b, s1lb, s2b, s2lb, block_size1 * 2);
		return MAX(score1, score2);
	}
	else if (block_size1 == block_size2 * 2)
		return ffuzzy_score_strings(s1a, s1la, s2b, s2lb, block_size1);
	else
		return ffuzzy_score_strings(s1b, s1lb, s2a, s2la, block_size2);
}
