ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -I$(top_srcdir)
SUBDIRS = osx sqlite3x src tests demos doc
# include aminclude.am
EXTRA_DIST = platform.xcodeproj doc/html $(DX_CONFIG)
# ln -s include platform
nobase_include_HEADERS = platform/sqlite3x.hpp platform/database.h platform/tags.h platform/search.h
# rm platform

