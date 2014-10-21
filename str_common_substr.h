/*

	libffuzzy : Fast ssdeep comparison library

	str_common_substr.h
	Common substring finder


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
#ifndef FFUZZY_STR_COMMON_SUBSTR_H
#define FFUZZY_STR_COMMON_SUBSTR_H

#include "ffuzzy_config.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "str_hash_rolling.h"

#define HAS_COMMON_SUBSTR_MAXLEN 64


/*
	FUNCTION: has_common_substring

	We only accept a match if we have at least one common substring
	in the signature of length ROLLING_WINDOW. It returns true if
	the two strings have a common substring, false otherwise.

	Examples (if ROLLING_WINDOW equals 3):

	"abcde" and "cdefg":
		true (with substring "cde")
	"common" and "different":
		false (no common substring of length 3)
	"abc" and "abc":
		true (with substring "abc")
	"ab" and "ab":
		false (they are identical but don't have common substring of length 3)
*/
static inline bool has_common_substring(
	const char *s1, size_t s1len,
	const char *s2, size_t s2len
)
{
#if HAS_COMMON_SUBSTR_MAXLEN >= ROLLING_WINDOW
	// if (at least) one of two strings is shorter than
	// ROLLING_WINDOW length, it will never find substring
	if (s1len < ROLLING_WINDOW)
		return false;
	if (s2len < ROLLING_WINDOW)
		return false;

	uint_least32_t hashes[HAS_COMMON_SUBSTR_MAXLEN - (ROLLING_WINDOW - 1)];
	roll_state state;

	// compute ROLLING_WINDOW-width rolling hashes for each index of s1
	memset(hashes, 0, sizeof(hashes));
	roll_init(&state);
	for (size_t i = 0; i < ROLLING_WINDOW - 1; i++)
		roll_hash(&state, (unsigned char)s1[i]);
	for (size_t i = ROLLING_WINDOW - 1; i < s1len; i++)
	{
		roll_hash(&state, (unsigned char)s1[i]);
		hashes[i - (ROLLING_WINDOW - 1)] = roll_sum(&state);
	}

	// compute ROLLING_WINDOW-width rolling hashes for each index of s2
	roll_init(&state);
	for (size_t j = 0; j < ROLLING_WINDOW - 1; j++)
		roll_hash(&state, (unsigned char)s2[j]);
	for (size_t j = 0; j < s2len - (ROLLING_WINDOW - 1); j++)
	{
		roll_hash(&state, (unsigned char)s2[j + (ROLLING_WINDOW - 1)]);
		uint_least32_t h = roll_sum(&state);
		for (size_t i = 0; i < s1len - (ROLLING_WINDOW - 1); i++)
		{
			// make sure we actually have common substring if hash matches
			if (hashes[i] == h && !memcmp(s1 + i, s2 + j, ROLLING_WINDOW))
			{
				return true;
			}
		}
	}
	return false;
#else
	return false;
#endif
}

#endif
