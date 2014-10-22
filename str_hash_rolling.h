/*

	libffuzzy : Fast ssdeep comparison library

	str_hash_rolling.h
	Rolling hash implementation


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
#ifndef FFUZZY_STR_HASH_ROLLING_H
#define FFUZZY_STR_HASH_ROLLING_H

/**
	\internal
	\file  str_hash_rolling.h
	\brief Rolling hash implementation
**/

#include "ffuzzy_config.h"

#include <stdint.h>
#include <string.h>

/** \internal \brief The window size for rolling hash **/
#define ROLLING_WINDOW 7


/**
	\internal
	\struct roll_state
	\brief  State for rolling hash

	\internal
	\var   roll_state::h1
	\brief The sum of characters in the window.
	\internal
	\var   roll_state::h2
	\brief The sum of characters in the window (weighted by its state).
	\internal
	\var   roll_state::h3
	\brief Shift and XOR-based hash.
	\internal
	\var   roll_state::n
	\brief Next index to insert.
	\internal
	\var   roll_state::window
	\brief Inserted haracters.
**/
typedef struct
{
	uint_least32_t h1, h2, h3;
	uint_least32_t n;
	unsigned char window[ROLLING_WINDOW];
} roll_state;


/**
	\internal
	\fn     void roll_init(roll_state*)
	\brief  Initialize rolling hash state
	\param  [out] self  The pointer to the rolling hash state to initialize.
**/
static inline void roll_init(roll_state *self)
{
	memset(self, 0, sizeof(roll_state));
}


/**
	\internal
	\fn     void roll_hash(roll_state*, unsigned char)
	\brief  Insert a character to the rolling hash
	\param  [in,out] self  The pointer to the rolling hash state.
	\param           c     The character to insert.
**/
static inline void roll_hash(roll_state *self, unsigned char c)
{
	self->h2 = self->h2 - self->h1 + ROLLING_WINDOW * (uint_least32_t)c;
	self->h1 = self->h1 + (uint_least32_t)c - (uint_least32_t)self->window[self->n];
	self->h3 = ((self->h3 << 5) ^ (uint_least32_t)c) & UINT32_C(0xffffffff);
	self->window[self->n] = c;
	// instead of modulo, branch will be faster on modern architectures
	self->n++;
	if (self->n == ROLLING_WINDOW)
		self->n = 0;
}


/**
	\internal
	\fn     uint_least32_t roll_sum(const roll_state*)
	\brief  Extract rolling hash from current state
	\param  [in] self  The pointer to the rolling hash state.
	\return A 32-bit rolling hash value.
**/
static inline uint_least32_t roll_sum(const roll_state *self)
{
	return (self->h1 + self->h2 + self->h3) & UINT32_C(0xffffffff);
}

#endif
