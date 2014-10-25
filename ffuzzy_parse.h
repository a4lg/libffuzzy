/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy_parse.h
	Fuzzy hash parser


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
#ifndef FFUZZY_FFUZZY_PARSE_H
#define FFUZZY_FFUZZY_PARSE_H

/**
	\internal
	\file  ffuzzy_parse.h
	\brief Fuzzy hash parser
**/

#include "ffuzzy_config.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "ffuzzy_blocksize.h"


/**
	\internal
	\fn     bool ffuzzy_read_digests_blocksize(unsigned long*, char**, const char*)
	\brief  Read block size from the string
	\param  [out] block_size  The pointer to the block size
	\param  [out] srem        The value pointed by this parameter is set to the first non-numerical character.
	\param  [in]  s           The string which contains a ssdeep digest.
	\return true if succeeds; false otherwise.
**/
static inline bool ffuzzy_read_digests_blocksize(unsigned long *block_size, char** srem, const char *s)
{
	errno = 0;
	*block_size = strtoul(s, srem, 10);
	// arithmetic overflow occurred
	if (*block_size == ULONG_MAX && errno == ERANGE)
		return false;
	// the string does not start with numbers
	if (*srem == s)
		return false;
	// we can't handle invalid block sizes
	if (!ffuzzy_blocksize_is_valid_(*block_size))
		return false;
	return true;
}


/**
	\internal
	\fn     bool ffuzzy_read_digest_after_blocksize(ffuzzy_digest*, const char*)
	\brief  Read remaining digest parts (except block size) from the string
	\param  [out] digest  The pointer to the buffer to store valid digest after parsing.
	\param  [in]  s       The pointer to the first non-numerical part of a ssdeep digest.
	\return true if succeeds; false otherwise.
	\see    bool ffuzzy_read_digests_blocksize(unsigned long*, char**, const char*)
**/
static inline bool ffuzzy_read_digest_after_blocksize(ffuzzy_digest *digest, const char *s)
{
	// ':' must follow after the number (which is block_size)
	if (*s != ':')
		return false;
	// read first block of ssdeep hash
	// (eliminating sequences of 4 or more identical characters)
	digest->len2 = 0;
	char *o = digest->digest;
	while (true)
	{
		char c = *++s;
		if (!c)
			return false;
		if (c == ':')
			break;
		if (digest->len2 < 3 || c != s[-1] || c != s[-2] || c != s[-3])
		{
			if (digest->len2 == FFUZZY_SPAMSUM_LENGTH)
				return false;
			digest->len2++;
			*o++ = c;
		}
	}
	// read second block of ssdeep hash
	// (eliminating sequences of 4 or more identical characters)
	digest->len1 = digest->len2;
	while (true)
	{
		char c = *++s;
		if (!c || c == ',')
			break;
		if (digest->len2 < 3 || c != s[-1] || c != s[-2] || c != s[-3])
		{
			if (digest->len2 == digest->len1 + FFUZZY_SPAMSUM_LENGTH)
				return false;
			digest->len2++;
			*o++ = c;
		}
	}
	digest->len2 -= digest->len1;
	return true;
}

#endif
