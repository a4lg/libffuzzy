/*

	libffuzzy : Fast ssdeep comparison library

	str_base64.h
	Base64 utility


	Copyright (C) 2014 Tsukasa OI <li@livegrid.org>


	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef FFUZZY_STR_BASE64_H
#define FFUZZY_STR_BASE64_H

/**
	\internal
	\file  str_base64.h
	\brief Base64 utility
**/

#include "ffuzzy_config.h"


/**
	\internal
	\fn     bool is_base64(char)
	\brief  Determine if given character is a base64 character
	\param  c  The character to determine.
	\return true if c is one of the base64 characters; false otherwise.
**/
static inline bool is_base64(char c)
{
	switch (c)
	{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z':
		case '+': case '/':
			return true;
		default:
			return false;
	}
}

#endif
