/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy_parse_unnorm.c
	Fuzzy hash parser (unnormalized form)


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
	\file  ffuzzy_parse_unnorm.c
	\brief Fuzzy hash parser (unnormalized form)
**/

#include "ffuzzy_config.h"

#include "ffuzzy_parse.h"

bool ffuzzy_read_udigest(ffuzzy_udigest *udigest, const char *s)
{
	char *p;
	if (!ffuzzy_read_digests_blocksize(&(udigest->block_size), &p, s))
		return false;
	return ffuzzy_read_udigest_after_blocksize(udigest, p);
}
