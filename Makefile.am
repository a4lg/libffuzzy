ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libffuzzy.la
libffuzzy_la_LDFLAGS = -no-undefined -version-info 4:2:1
libffuzzy_la_SOURCES = \
	ffuzzy_compare.c \
	ffuzzy_blocksize.c \
	ffuzzy_digest.c \
	ffuzzy_digest_unnorm.c \
	ffuzzy_digest_conv.c \
	ffuzzy_parse.c \
	ffuzzy_parse_unnorm.c
include_HEADERS = ffuzzy.h
EXTRA_DIST = \
	README NEWS \
	COPYING COPYING.GPLv2 COPYING.Boost \
	ffuzzy_blocksize.h \
	ffuzzy_parse.h \
	str_base64.h \
	str_common_substr.h \
	str_edit_dist.h \
	str_hash_rolling.h \
	util.h \
	.gitignore .gitattributes ext/.gitignore m4/.gitignore \
	Doxyfile.example \
	examples/internal/has_common_substring.c \
	examples/internal/edit_distn.c
