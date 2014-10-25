/*

	libffuzzy : Fast ssdeep comparison library

	ffuzzy.h
	Public API for libffuzzy


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

/**

	\mainpage

	This is the documentation for the libffuzzy,
	a fast ssdeep comparison library.

	\author  Tsukasa OI, li@livegrid.org and original ssdeep authors
	\version 2.1

**/

/**
	\file  ffuzzy.h
	\brief Public API for libffuzzy
**/

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>

/** \brief Maximum length for the digest block **/
#define FFUZZY_SPAMSUM_LENGTH 64

/** \brief Minimum block size to start in ssdeep implementation **/
#define FFUZZY_MIN_BLOCKSIZE 3

/** \brief
	The minimal match (length of common substring) required
	for (at least) one of the block digests
**/
#define FFUZZY_MIN_MATCH 7


#ifdef __cplusplus
extern "C" {
#endif

/**

	\struct ffuzzy_digest
	\brief  The type to store ssdeep digest after parsing.
	\details
		This structure contains all information in the ssdeep digest
		in machine-friendly format. You can accelerate comparing
		fuzzy hashes by storing this type first.
		\see ffuzzy_read_digest(ffuzzy_digest*, const char*)
		\see ffuzzy_compare_digest(const ffuzzy_digest*, const ffuzzy_digest*)

	\var   ffuzzy_digest::len1
	\brief Digest length for first block of the digest.

	\var   ffuzzy_digest::len2
	\brief Digest length for second block of the digest.

	\var   ffuzzy_digest::block_size
	\brief Block size of the ssdeep digest.
	\details
		Technically, this is the block size of first block of the digest.
		Block size of the second block is twice as this.

	\var   ffuzzy_digest::digest
	\brief Digest buffer for both blocks of the digest.
	\details
		This buffer stores the digest in a compressed form.
		From the beginning, the buffer is formed like this:

		-	@link len1 len1@endlink-sized characters of the first block
		-	@link len2 len2@endlink-sized characters of the second block

		Valid blocks in the buffer do not contain sequences of
		four or more identical characters.

**/
typedef struct
{
	size_t len1, len2;
	unsigned long block_size;
	char digest[FFUZZY_SPAMSUM_LENGTH * 2];
} ffuzzy_digest;



/**
	\name Comparison and Parsing
	\{
**/

/**
	\fn     bool ffuzzy_read_digest(ffuzzy_digest*, const char*)
	\brief  Read ssdeep digest from the string
	\details
		This function always sets valid digest if succeeds.
	\param  [out] digest  The pointer to the buffer to store valid digest after parsing.
	\param  [in]  s       The string which contains a ssdeep digest.
	\return true if succeeds; false otherwise.
**/
bool ffuzzy_read_digest(ffuzzy_digest *digest, const char *s);

/**
	\fn     int ffuzzy_compare_digest(const ffuzzy_digest*, const ffuzzy_digest*)
	\brief  Compare two fuzzy hashes and compute similarity score
	\param  [in] d1  Valid digest 1
	\param  [in] d2  Valid digest 2
	\return [0,100] values represent similarity score or negative values on failure.
**/
int ffuzzy_compare_digest(const ffuzzy_digest *d1, const ffuzzy_digest *d2);

/**
	\fn     int ffuzzy_compare(const char*, const char*)
	\brief  Compute similarity score for given ssdeep hash strings
	\param  [in] str1  ssdeep hash 1
	\param  [in] str2  ssdeep hash 2
	\return [0,100] values represent similarity score or negative values on failure.
**/
int ffuzzy_compare(const char *str1, const char *str2);

/** \} **/



/**
	\name Optimized / Specialized Comparison
	\{
**/

/**
	\fn     int ffuzzy_compare_digest_near(const ffuzzy_digest*, const ffuzzy_digest*)
	\brief  Compare two fuzzy hashes assuming two block sizes of given hashes are "near"
	\details
		In this context, "near" means two block sizes are equal or
		one of the block size is twice as other.

		This function assumes two block sizes are "near"
		(ffuzzy_blocksize_is_near on two block sizes returns true) and
		make the computation slightly faster.
	\param  [in] d1  Valid digest 1
	\param  [in] d2  Valid digest 2
	\return [0,100] values represent similarity score or negative values on failure.
**/
int ffuzzy_compare_digest_near(const ffuzzy_digest *d1, const ffuzzy_digest *d2);

/**
	\fn     int ffuzzy_compare_digest_near_eq(const ffuzzy_digest*, const ffuzzy_digest*)
	\brief  Compare two fuzzy hashes assuming two block sizes are same
	\details
		This function assumes two block sizes are same.
	\param  [in] d1  Valid digest 1 (with same block size as d2)
	\param  [in] d2  Valid digest 2 (with same block size as d1)
	\return [0,100] values represent similarity score or negative values on failure.
	\see    int ffuzzy_compare_digest_near(const ffuzzy_digest*, const ffuzzy_digest*)
**/
int ffuzzy_compare_digest_near_eq(const ffuzzy_digest *d1, const ffuzzy_digest *d2);

/**
	\fn     int ffuzzy_compare_digest_near_lt(const ffuzzy_digest*, const ffuzzy_digest*)
	\brief  Compare two fuzzy hashes assuming second block size is double as first one
	\details
		This function assumes second block size is double as first one.
	\param  [in] d1  Valid digest 1
	\param  [in] d2  Valid digest 2 (with double block size as d1)
	\return [0,100] values represent similarity score or negative values on failure.
	\see    int ffuzzy_compare_digest_near(const ffuzzy_digest*, const ffuzzy_digest*)
**/
int ffuzzy_compare_digest_near_lt(const ffuzzy_digest *d1, const ffuzzy_digest *d2);

/** \} **/



/**
	\name Block Size Utilities
	\{
**/

/**
	\fn     bool ffuzzy_blocksize_is_valid(unsigned long)
	\brief  Determines whether given block size is valid to use in libffuzzy
	\details
		To prevent arithmetic overflow, not all unsigned long values are valid in libffuzzy.
		This function determines whether the given block size is valid and
		safe to use in libffuzzy.

		You will not need to use this function if you use ffuzzy_read_digest function
		because it always returns valid digest on success.

		Note that this is not the restriction of ssdeep digest,
		but restriction of the implementation.
	\param  block_size  Block size (which may not be valid)
	\return true if the given block size is valid; false otherwise.
**/
bool ffuzzy_blocksize_is_valid(unsigned long block_size);

/**
	\fn     bool ffuzzy_blocksize_is_natural(unsigned long)
	\brief  Determines whether given block size is "natural"
	\details
		In this context, "natural" means given parameter of fuzzy hash
		may be generated by ssdeep or its backend, libfuzzy.
		Depending on the job, handling only "natural" digests
		may make your program efficient.

		This function doesn't only check whether the block size is valid,
		but it checks the given size is a product of FFUZZY_MIN_BLOCKSIZE
		and a power of two.
	\param  block_size  Block size (which may not be valid or "natural")
	\return true if the given block size is valid and "natural"; false otherwise.
**/
bool ffuzzy_blocksize_is_natural(unsigned long block_size);

/**
	\fn     bool ffuzzy_blocksize_is_near(unsigned long, unsigned long)
	\brief  Determines whether given block sizes are "near"
	\details
		In this context, "near" means two block sizes are equal or
		one of the block size is twice as other.

		This function determines whether given block sizes are "near".
		If this function returns true, it is safe to use ffuzzy_compare_digest_near
		function for two digests which have given block sizes.
	\param  block_size1  Valid block size 1
	\param  block_size2  Valid block size 2
	\return true if the given block sizes are "near"; false otherwise.
**/
bool ffuzzy_blocksize_is_near(unsigned long block_size1, unsigned long block_size2);

/**
	\fn     bool ffuzzy_blocksize_is_far_le(unsigned long, unsigned long)
	\brief  Determines whether given ordered block sizes "far" enough
	\details
		In this context, "far" means
		the second block size is greater than double of the first block size.

		For block size-sorted digests, "far" means there are no
		subsequent entries which will match.

		This function determines whether given block sizes are "far".

		You may want to inline or reimplement this because
		this function is very easy. There's nothing preventing you to do that.
	\param  block_size1  Valid block size 1
	\param  block_size2  Valid block size 2 (must be equal or greater than block_size1)
	\return true if the given block sizes are "far"; false otherwise.
**/
bool ffuzzy_blocksize_is_far_le(unsigned long block_size1, unsigned long block_size2);

/** \} **/



/**
	\name Digest Utilities
	\{
**/

/**
	\fn     bool ffuzzy_digest_is_valid_lengths(const ffuzzy_digest*)
	\brief  Determines whether block lengths of given digest are valid
	\param  [in] digest  Digest (which may not be valid)
	\return true if values of ffuzzy_digest::len1 and ffuzzy_digest::len2 are valid.
**/
bool ffuzzy_digest_is_valid_lengths(const ffuzzy_digest *digest);

/**
	\fn     bool ffuzzy_digest_is_valid_buffer(const ffuzzy_digest*)
	\brief  Determines whether digest blocks are valid
	\details
		This function determines whether there are no sequences
		which consist of four or more identical characters.

		This function needs valid digest block lengths.
		If digest block lengths are not guaranteed to be valid,
		use ffuzzy_digest_is_valid_lengths first.

		You will not need to use this function if you use ffuzzy_read_digest function
		because it always returns valid digests on success.
	\param  [in] digest  Digest (which may not be valid but block lengths are valid)
	\return true if the digest blocks are valid; false otherwise.
**/
bool ffuzzy_digest_is_valid_buffer(const ffuzzy_digest *digest);

/**
	\fn     bool ffuzzy_digest_is_natural_buffer(const ffuzzy_digest*)
	\brief  Determines whether digest blocks are valid and "natural"
	\details
		This function determines whether valid range of ffuzzy_digest::digest
		values consist of base64 characters (in other words, "natural").

		This function needs valid digest block lengths.
		If digest block lengths are not guaranteed to be valid,
		use ffuzzy_digest_is_valid_lengths first.

		You may need to use this function even after success call to
		ffuzzy_read_digest because this function is not guaranteed to set
		digests with "natural" digest blocks.

		However, if you are just comparing, this check is not necessary because
		fuzzy hash comparison will not decode base64 characters (it just "compares").

		You will need this function ONLY if you need to verify
		whether given digest is truly "natural".
	\param  [in] digest  Digest (which may not be valid or natural but block lengths are valid)
	\return true if the digest blocks are valid and "natural"; false otherwise.
**/
bool ffuzzy_digest_is_natural_buffer(const ffuzzy_digest *digest);

/**
	\fn     bool ffuzzy_digest_is_valid(const ffuzzy_digest*)
	\brief  Determines whether given digest is valid
	\param  [in] digest  Digest (which may not be valid)
	\return true if the digest is valid; false otherwise.
**/
bool ffuzzy_digest_is_valid(const ffuzzy_digest *digest);

/**
	\fn     bool ffuzzy_digest_is_natural(const ffuzzy_digest*)
	\brief  Determines whether given digest is valid and "natural"
	\param  [in] digest  Digest (which may not be valid or natural)
	\return true if the digest is valid and natural; false otherwise.
**/
bool ffuzzy_digest_is_natural(const ffuzzy_digest *digest);

/**
	\fn     int ffuzzy_digestcmp(const ffuzzy_digest*, const ffuzzy_digest*)
	\brief  Compare two ffuzzy_digest values
	\details
		This comparison has priorities.

		1. Compare block sizes.
		2. Compare block lengths of the first block.
		3. Compare block lengths of the second block.
		4. Compare block buffer contents (first and second).

	\param  [in] d1  Valid digest 1
	\param  [in] d2  Valid digest 2
	\return
		Positive value if d1 < d2, negativa value if d2 > d1
		and 0 if d1 is equal to d2.
**/
int ffuzzy_digestcmp(const ffuzzy_digest *d1, const ffuzzy_digest *d2);

/**
	\fn     int ffuzzy_digestcmp_blocksize(const ffuzzy_digest*, const ffuzzy_digest*)
	\brief  Compare two ffuzzy_digest values by block sizes
	\param  [in] d1  Valid digest 1
	\param  [in] d2  Valid digest 2
	\return
		Positive value if d1 < d2, negativa value if d2 > d1
		and 0 if block size of d1 is equal to d2.
	\see    int ffuzzy_digestcmp(const ffuzzy_digest*, const ffuzzy_digest*)
**/
int ffuzzy_digestcmp_blocksize(const ffuzzy_digest *d1, const ffuzzy_digest *d2);

/**
	\fn     int ffuzzy_digestcmp_blocksize_n(const ffuzzy_digest*, const ffuzzy_digest*)
	\brief  Compare two ffuzzy_digest values by whether block sizes are "natural" and block size values
	\details
		This comparison has priorities.

		1. Compare whether block sizes are "natural" (for ffuzzy_blocksize_is_natural return value, true comes first)
		2. Compare block sizes.

	\param  [in] d1  Valid digest 1
	\param  [in] d2  Valid digest 2
	\return
		Positive value if d1 < d2, negativa value if d2 > d1
		and 0 if block size of d1 is equal to d2.
	\see    bool ffuzzy_blocksize_is_natural(unsigned long)
	\see    int ffuzzy_digestcmp(const ffuzzy_digest*, const ffuzzy_digest*)
**/
int ffuzzy_digestcmp_blocksize_n(const ffuzzy_digest *d1, const ffuzzy_digest *d2);

/**
	\fn     bool ffuzzy_pretty_digest(char*, size_t, const ffuzzy_digest*)
	\brief  Convert ffuzzy_digest to the string
	\param  [out] buf     Buffer to store string
	\param        buflen  Size of buf
	\param  [in]  digest  A valid digest to convert
	\return true if succeeds; false otherwise.
**/
bool ffuzzy_pretty_digest(char *buf, size_t buflen, const ffuzzy_digest *digest);

/** \} **/



/**
	\name Unnormalized Digests
	\{
**/

/**

	\struct ffuzzy_udigest
	\brief  The type to store unnormalized ssdeep digest after parsing.
	\details
		Unlike ffuzzy_digest type, this type (actually, identical to
		ffuzzy_digest) may include sequences of four or more identical
		characters. fuzzy_digest function (by default) does not eliminate
		such sequences and this type allows preserving such sequences.

		This type is easily convertible to ffuzzy_digest.

	\var   ffuzzy_udigest::len1
	\brief Digest length for first block of the digest.
	\see   ffuzzy_digest::len1

	\var   ffuzzy_udigest::len2
	\brief Digest length for second block of the digest.
	\see   ffuzzy_digest::len2

	\var   ffuzzy_udigest::block_size
	\brief Block size of the ssdeep digest.
	\see   ffuzzy_digest::block_size

	\var   ffuzzy_udigest::digest
	\brief Digest buffer for both blocks of the unnormalized digest.
	\details
		This buffer is very similar to ffuzzy_digest::digest but
		allows long sequences of identical characters.
	\see   ffuzzy_digest::digest

**/
typedef struct
{
	size_t len1, len2;
	unsigned long block_size;
	char digest[FFUZZY_SPAMSUM_LENGTH * 2];
} ffuzzy_udigest;


/**
	\fn     bool ffuzzy_read_udigest(ffuzzy_udigest*, const char*)
	\brief  Read unnormalized ssdeep digest from the string
	\details
		This function always sets valid and unnormalized digest if succeeds.
	\param  [out] udigest  The pointer to the buffer to store valid unnormalized digest after parsing.
	\param  [in]  s        The string which contains a ssdeep digest.
	\return true if succeeds; false otherwise.
	\see    ffuzzy_udigest
**/
bool ffuzzy_read_udigest(ffuzzy_udigest *udigest, const char *s);

/**
	\fn     bool ffuzzy_udigest_is_valid_lengths(const ffuzzy_udigest*)
	\brief  Determines whether block lengths of given digest are valid
	\param  [in] udigest  Unnormalized digest (which may not be valid)
	\return true if values of ffuzzy_udigest::len1 and ffuzzy_udigest::len2 are valid.
**/
bool ffuzzy_udigest_is_valid_lengths(const ffuzzy_udigest *udigest);

/**
	\fn     bool ffuzzy_udigest_is_natural_buffer(const ffuzzy_udigest*)
	\brief  Determines whether digest blocks are "natural"
	\details
		This function determines whether valid range of ffuzzy_udigest::digest
		values consist of base64 characters (in other words, "natural").

		This function needs valid digest block lengths.
		If digest block lengths are not guaranteed to be valid,
		use ffuzzy_udigest_is_valid_lengths first.

		You will need this function ONLY if you need to verify
		whether given digest is truly "natural".
	\param  [in] udigest  Unnormalized digest (which may not be natural but block lengths are valid)
	\return true if the digest blocks are "natural"; false otherwise.
**/
bool ffuzzy_udigest_is_natural_buffer(const ffuzzy_udigest *udigest);

/**
	\fn     bool ffuzzy_udigest_is_valid(const ffuzzy_udigest*)
	\brief  Determines whether given digest is valid
	\param  [in] udigest  Unnormalized digest (which may not be valid)
	\return true if the digest is valid; false otherwise.
**/
bool ffuzzy_udigest_is_valid(const ffuzzy_udigest *udigest);

/**
	\fn     bool ffuzzy_udigest_is_natural(const ffuzzy_udigest*)
	\brief  Determines whether given digest is valid and "natural"
	\param  [in] udigest  Unnormalized digest (which may not be valid or natural)
	\return true if the digest is valid and "natural"; false otherwise.
**/
bool ffuzzy_udigest_is_natural(const ffuzzy_udigest *udigest);

/**
	\fn     int ffuzzy_udigestcmp(const ffuzzy_udigest*, const ffuzzy_udigest*)
	\brief  Compare two ffuzzy_udigest values
	\details
		This comparison has priorities.

		1. Compare block sizes.
		2. Compare block lengths of the first block.
		3. Compare block lengths of the second block.
		4. Compare block buffer contents (first and second).

	\param  [in] d1  Valid digest 1
	\param  [in] d2  Valid digest 2
	\return
		Positive value if d1 < d2, negativa value if d2 > d1
		and 0 if d1 is equal to d2.
**/
int ffuzzy_udigestcmp(const ffuzzy_udigest *d1, const ffuzzy_udigest *d2);

/**
	\fn     int ffuzzy_udigestcmp_blocksize(const ffuzzy_udigest*, const ffuzzy_udigest*)
	\brief  Compare two ffuzzy_udigest values by block sizes
	\param  [in] d1  Valid digest 1
	\param  [in] d2  Valid digest 2
	\return
		Positive value if d1 < d2, negativa value if d2 > d1
		and 0 if block size of d1 is equal to d2.
	\see    int ffuzzy_udigestcmp(const ffuzzy_udigest*, const ffuzzy_udigest*)
**/
int ffuzzy_udigestcmp_blocksize(const ffuzzy_udigest *d1, const ffuzzy_udigest *d2);

/**
	\fn     int ffuzzy_udigestcmp_blocksize_n(const ffuzzy_udigest*, const ffuzzy_udigest*)
	\brief  Compare two ffuzzy_udigest values by whether block sizes are "natural" and block size values
	\details
		This comparison has priorities.

		1. Compare whether block sizes are "natural" (for ffuzzy_blocksize_is_natural return value, true comes first)
		2. Compare block sizes.

	\param  [in] d1  Valid digest 1
	\param  [in] d2  Valid digest 2
	\return
		Positive value if d1 < d2, negativa value if d2 > d1
		and 0 if block size of d1 is equal to d2.
	\see    bool ffuzzy_blocksize_is_natural(unsigned long)
	\see    int ffuzzy_udigestcmp(const ffuzzy_udigest*, const ffuzzy_udigest*)
**/
int ffuzzy_udigestcmp_blocksize_n(const ffuzzy_udigest *d1, const ffuzzy_udigest *d2);

/**
	\fn     bool ffuzzy_pretty_udigest(char*, size_t, const ffuzzy_udigest*)
	\brief  Convert ffuzzy_udigest to the string
	\param  [out] buf      Buffer to store string
	\param        buflen   Size of buf
	\param  [in]  udigest  A valid digest to convert
	\return true if succeeds; false otherwise.
**/
bool ffuzzy_pretty_udigest(char *buf, size_t buflen, const ffuzzy_udigest *udigest);


/**
	\fn     void ffuzzy_convert_digest_to_udigest(ffuzzy_udigest*, const ffuzzy_digest*)
	\brief  Convert ffuzzy_digest to ffuzzy_udigest
	\param  [out] udigest  The pointer to buffer to the unnormalized digest
	\param  [in]  digest   The pointer to the valid digest
**/
void ffuzzy_convert_digest_to_udigest(ffuzzy_udigest *udigest, const ffuzzy_digest *digest);

/**
	\fn     void ffuzzy_convert_udigest_to_digest(ffuzzy_digest*, const ffuzzy_udigest*)
	\brief  Convert ffuzzy_udigest to ffuzzy_digest
	\param  [out] digest   The pointer to buffer to the normalized digest
	\param  [in]  udigest  The pointer to the valid and unnormalized digest
**/
void ffuzzy_convert_udigest_to_digest(ffuzzy_digest *digest, const ffuzzy_udigest *udigest);

/** \} **/



/**
	\name Internal Comparison Utilities
	\{
**/

/**
	\fn     int ffuzzy_score_cap(int, int, unsigned long)
	\brief  Retrieve score cap for given block lengths and the block size
	\details
		The (partial) similarity score is capped when the block is short
		and the block size is small to prevent exaggerate match.
		This function returns this score cap for given block lengths and the block size.
	\param  s1len       Length of block 1
	\param  s2len       Length of block 2
	\param  block_size  Block size
	\return
		Maximum (partial) similarity score value.
		If the return value is greater than 100, the score cap is 100.

		If s1len or s2len is out of range [0,FFUZZY_SPAMSUM_LENGTH], the value is undefined.
**/
int ffuzzy_score_cap(int s1len, int s2len, unsigned long block_size);

/**
	\fn     int ffuzzy_score_cap_1(int, unsigned long)
	\brief  Retrieve score cap for given block length and size
	\details
		ffuzzy_score_cap function computes the score cap by
		the block size and "minimum" length of the given blocks.
		This function exposes internal interface of ffuzzy_score_cap.
	\param  minslen     Minimum length of the blocks
	\param  block_size  Block size
	\return
		Maximum (partial) similarity score value.
		If the return value is greater than 100, the score cap is 100.

		If minslen is out of range [0,FFUZZY_SPAMSUM_LENGTH], the value is undefined.
**/
int ffuzzy_score_cap_1(int minslen, unsigned long block_size);

/**
	\fn     int ffuzzy_score_strings(const char*, size_t, const char*, size_t, unsigned long)
	\brief  Compute partial similarity score for given two block strings and block size
	\details
		In the fuzzy computation, the digest block of the
		same block sizes are selected to compare.
		This is the internal interface for ffuzzy_compare and ffuzzy_compare_digest.
	\param  [in] s1          Digest block 1
	\param       s1len       Length of s1
	\param  [in] s2          Digest block 2
	\param       s2len       Length of s2
	\param       block_size  Block size for two digest blocks
	\return [0,100] values represent partial similarity score or negative values on failure.
**/
int ffuzzy_score_strings(
	const char *s1, size_t s1len,
	const char *s2, size_t s2len,
	unsigned long block_size
);

/** \} **/



#ifdef __cplusplus
}
#endif

#endif
