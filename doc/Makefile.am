if DOXYGEN_DOCS_ENABLED

EXTRA_DIST = Doxyfile.in

noinst_PROGRAMS = index.html

index_html_SOURCES = Doxyfile

index.html: Doxyfile
	doxygen Doxyfile

endif

MAINTAINERCLEANFILES = \
	Makefile.in
