ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libffuzzy.la
libffuzzy_la_LDFLAGS = -no-undefined -version-info 2:1:1
libffuzzy_la_SOURCES = \
	ffuzzy_compare.c \
	ffuzzy_blocksize.c
include_HEADERS = ffuzzy.h
EXTRA_DIST = \
	README NEWS \
	COPYING COPYING.GPLv2 COPYING.Boost \
	str_common_substr.h \
	str_edit_dist.h \
	str_hash_rolling.h \
	util.h \
	.gitignore .gitattributes ext/.gitignore m4/.gitignore
