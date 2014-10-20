/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy.h
	Fuzzy hash comparison library definitions


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
#ifndef FFUZZY_FFUZZY_H
#define FFUZZY_FFUZZY_H

#include <stdbool.h>
#include <stddef.h>

#define SPAMSUM_LENGTH 64
#define MIN_BLOCKSIZE 3

typedef struct ffuzzy_digest_
{
	size_t size1, size2;
	unsigned long block_size;
	char digest[SPAMSUM_LENGTH * 2];
} ffuzzy_digest;

int ffuzzy_score_cap(int s1len, int s2len, unsigned long block_size);
int ffuzzy_score_cap_1(int minslen, unsigned long block_size);

bool ffuzzy_read_digest(ffuzzy_digest *digest, const char *s);
int ffuzzy_compare_digest(const ffuzzy_digest *d1, const ffuzzy_digest *d2);
int ffuzzy_compare_digest_near(const ffuzzy_digest *d1, const ffuzzy_digest *d2);

int ffuzzy_score_strings(
	const char *s1, size_t s1len,
	const char *s2, size_t s2len,
	unsigned long block_size
);
int ffuzzy_compare(const char *str1, const char *str2);

#endif
