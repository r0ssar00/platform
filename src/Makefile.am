INCLUDES = -I../ -I ../osx
#include_HEADERS = $(top_srcdir)/database.h $(top_srcdir)/search.h $(top_srcdir)/tags.h
lib_LTLIBRARIES = libplatform.la
libplatform_la_SOURCES = database.cpp
noinst_LTLIBRARIES = libobjcpplibs.la
libobjcpplibs_la_SOURCES = search.cpp tags.cpp
libobjcpplibs_la_CPPFLAGS = -x objective-c++
libplatform_la_LIBADD = $(top_srcdir)/osx/libosx.la $(top_srcdir)/sqlite3x/libsqlite3x.la libobjcpplibs.la
