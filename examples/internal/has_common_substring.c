// Assume that FFUZZY_MIN_MATCH == 7.
static void examples()
{
	// Example 1 ("abcdefghijklmn" and "hijklmnopqrstu"):
	//   expect true because they have common substring "hijklmn".
	assert(has_common_substring("abcdefghijklmn", 14, "hijklmnopqrstu", 14) == true);
	// Example 2 ("commonstring" and "differentstring"):
	//   expect false because they don't have common substrings of length FFUZZY_MIN_MATCH.
	assert(has_common_substring("commonstring", 12, "differentstring", 15) == false);
	// Example 3 ("abcdefg" and "abcdefg"):
	//   expect true because they have common substring "abcdefg" (which is the whole string)
	assert(has_common_substring("abcdefg", 7, "abcdefg", 7) == true);
	// Example 3 ("abc" and "abc"):
	//   expect false because they don't have common substrings of length FFUZZY_MIN_MATCH
	//   (even if they are identical).
	assert(has_common_substring("abc", 3, "abc", 3) == false);
}
