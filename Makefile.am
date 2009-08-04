ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -I$(top_srcdir)
SUBDIRS = osx sqlite3x src doc
# include aminclude.am
EXTRA_DIST = platform.xcodeproj doc/html $(DX_CONFIG)
# ln -s include platform
nobase_include_HEADERS = platform/sqlite3x.hpp platform/database.h platform/tags.h platform/search.h
# rm platform

bin_PROGRAMS = test
test_SOURCES = src/test.cpp
test_CXXFLAGS = -I.
test_LDADD = $(top_builddir)/src/libplatform.la
