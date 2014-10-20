/*

	libffuzzy : Fast ssdeep comparison library

	str_elim_seq.h
	String Sequence Elimination


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
#ifndef FFUZZY_STR_ELIM_SEQ_H
#define FFUZZY_STR_ELIM_SEQ_H

#include <assert.h>
#include <string.h>


/*
	FUNCTION: eliminate_sequences

	This function eliminates sequences of longer than
	3 identical characters. It returns size of the output buffer.

	Examples:

	"1"    -> "1"
	"ABC"  -> "ABC"
	"LLL"  -> "LLL"
	"LLLL" -> "LLL"
	"longcatisLOOOOOOOOOOOOOONG" -> "longcatisLOOONG"
*/
static size_t eliminate_sequences(
	char *buf,
	const char *s, size_t slen
)
{
	if (slen < 3)
	{
		memcpy(buf, s, slen);
		return slen;
	}
	memcpy(buf, s, 3);
	const char *strend = s + (slen - 3);
	size_t i = 3;
	for (; s != strend; s++)
	{
		if (
			s[3] != s[0] ||
			s[3] != s[1] ||
			s[3] != s[2]
		)
		{
			buf[i++] = s[3];
		}
	}
	return i;
}

static size_t eliminate_sequences_n(
	char *buf, size_t buflen,
	const char *s, size_t slen
)
{
	assert(buflen != SIZE_MAX);
	if (slen < 3 || buflen < 3)
	{
		if (buflen >= slen)
		{
			memcpy(buf, s, slen);
			return slen;
		}
		else
		{
			memcpy(buf, s, buflen);
			return buflen + 1;
		}
	}
	memcpy(buf, s, 3);
	const char *strend = s + (slen - 3);
	size_t i = 3;
	for (; s != strend; s++)
	{
		if (
			s[3] != s[0] ||
			s[3] != s[1] ||
			s[3] != s[2]
		)
		{
			if (i == buflen)
				return buflen + 1;
			buf[i++] = s[3];
		}
	}
	return i;
}

#endif
