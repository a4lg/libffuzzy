void examples()
{
	// Example 1 ("123" to "1234"):
	//   expect 1 (insert '4' to tail)
	assert(edit_distn("123", 3, "1234", 4) == 1);
	// Example 2 ("2034" to "234"):
	//   expect 1 (remove '0' in the middle of the first string)
	assert(edit_distn("2034", 4, "234", 3) == 1);
	// Example 3 ("kiss" to "miss"):
	//   expect 2 (remove 'k' and then insert 'm' to the same place)
	assert(edit_distn("kiss", 4, "miss", 4) == 2);
	// Example 4 ("kitten" to "sitting"):
	//   expect 5 (remove 'k', insert 's', remove 'e', insert 'i' and insert 'g' to the tail)
	assert(edit_distn("kitten", 6, "sitting", 7) == 5);
}
