/*

	libffuzzy : Fast ssdeep comparison library

	str_edit_dist.h
	Edit distance (Levenshtein distance with no "replacement")

	Copyright (C) 2014 kikairoya <kikairoya@gmail.com>
	Copyright (C) 2014 Tsukasa OI <li@livegrid.org>


	This program can be used, redistributed or modified under any of
	Boost Software License 1.0, GPL v2 or GPL v3

*/
#ifndef FFUZZY_STR_EDIT_DIST_H
#define FFUZZY_STR_EDIT_DIST_H

/**
	\internal
	\file  str_edit_dist.h
	\brief Edit distance (Levenshtein distance with no "replacement")
**/

#include "ffuzzy_config.h"

#include <stddef.h>
#include "util.h"

/** \internal \brief Maximum length for edit_distn function **/
#define EDIT_DISTN_MAXLEN 64


/**
	\internal
	\fn     int edit_distn(const char*, size_t, const char*, size_t)
	\brief  Compute edit distance between two strings with no replacement
	\details
		This function computes Levenshtein distance with no "replacement".
		It means, single cost 1 operations allowed are "insertion" and "removal".
	\param  s1     String 1 (non-empty)
	\param  s1len  Length of s1
	\param  s2     String 2
	\param  s2len  Length of s2
	\return The edit distance.
	\example examples/internal/edit_distn.c
**/
static inline int edit_distn(const char *s1, size_t s1len, const char *s2, size_t s2len)
{
	int t[2][EDIT_DISTN_MAXLEN+1];
	int *t1 = t[0], *t2 = t[1], *t3;
	size_t i1, i2;
	// assume that s1len (length of s1) is greater than zero.
	t1[0] = 1;
	for (i2 = 0; i2 < s2len; i2++)
	{
		int cost_d = t1[i2] + 1;
		int cost_r = i2 + (s1[0] == s2[i2] ? 0 : 2);
		t1[i2+1] = MIN(cost_d, cost_r);
	}
	for (i1 = 1; i1 < s1len; i1++)
	{
		t2[0] = i1 + 1;
		for (i2 = 0; i2 < s2len; i2++)
		{
			int cost_a = t1[i2+1] + 1;
			int cost_d = t2[i2]   + 1;
			int cost_r = t1[i2]   + (s1[i1] == s2[i2] ? 0 : 2);
			t2[i2+1] = MIN(MIN(cost_a, cost_d), cost_r);
		}
		t3 = t1; t1 = t2; t2 = t3;
	}
	return t1[s2len];
}


/**
	\internal
	\fn     int edit_distn_norm(const char*, size_t, const char*, size_t)
	\brief  Compute edit distance between two strings with no replacement
	\details
		This function computes Levenshtein distance with no "replacement".
		It means, single cost 1 operations allowed are "insertion" and "removal".

		This function swaps the string to make sure that
		s1len to pass through edit_distn is always equal or less than s2len.
		This may prevent stalls on modern processors.
	\param  s1     String 1 (non-empty)
	\param  s1len  Length of s1
	\param  s2     String 2 (non-empty)
	\param  s2len  Length of s2
	\return The edit distance.
**/
static inline int edit_distn_norm(const char *s1, size_t s1len, const char *s2, size_t s2len)
{
	if (s1len <= s2len)
		return edit_distn(s1, s1len, s2, s2len);
	else
		return edit_distn(s2, s2len, s1, s1len);
}

#endif
