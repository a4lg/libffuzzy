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

#include <stddef.h>
#include "util.h"

#define EDIT_DISTN_MAXLEN 64

/*
	FUNCTION: edit_distn

	This function computes edit distance between
	given two strings with no replacement.

	Examples:
	"123"  and "1234" -> 1 (insert 4 to tail)
	"2034" and "234"  -> 1 (remove '0' in the middle of the first string)
	"kiss" and "miss" -> 2 (remove 'k' and then insert 'm' to the same place)
	"kitten" and "sitting" -> 5
		(remove 'k', insert 's', remove 'e', insert 'i' and insert 'g' to the tail)
*/
static int edit_distn(const char *s1, size_t s1len, const char *s2, size_t s2len)
{
	int t[2][EDIT_DISTN_MAXLEN+1];
	int *t1 = t[0], *t2 = t[1], *t3;
	size_t i1, i2;
	for (i2 = 0; i2 <= s2len; i2++) t1[i2] = i2;
	for (i1 = 0; i1 < s1len; i1++)
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

#endif
