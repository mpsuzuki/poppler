lib_LTLIBRARIES = libpoppler-qt5.la

EXTRA_DIST = Doxyfile Mainpage.dox

# This rule lets GNU make create any *.moc from the equivalent *.h
.h.moc:
	$(AM_V_GEN) $(MOCQT5) -i $< -o $@

SUFFIXES: .moc

clean-generic:
	rm -f *.moc

BUILT_SOURCES_MOC = poppler-optcontent.moc
BUILT_SOURCES_CXX = $(BUILT_SOURCES_MOC:.moc=.cxx)
BUILT_SOURCES = $(BUILT_SOURCES_MOC) $(BUILT_SOURCES_CXX)
nodist_libpoppler_qt5_la_SOURCES = $(BUILT_SOURCES_CXX)
EXTRA_DIST += $(BUILT_SOURCES_MOC:.moc=.cc)

.cc.cxx: $(@:.cxx=.cc) $(@:.cxx=.moc)
	$(AM_V_GEN) cat $(srcdir)/$(@:.cxx=.cc) $(builddir)/$(@:.cxx=.moc) > $@

poppler_includedir = $(includedir)/poppler/qt5
poppler_include_HEADERS =			\
	poppler-qt5.h				\
	poppler-link.h				\
	poppler-link-private.h			\
	poppler-annotation.h			\
	poppler-form.h				\
	poppler-optcontent.h			\
	poppler-export.h			\
	poppler-media.h				\
	poppler-page-transition.h

libpoppler_qt5_la_SOURCES =			\
	poppler-document.cc			\
	poppler-page.cc				\
	poppler-fontinfo.cc			\
	poppler-embeddedfile.cc			\
	poppler-textbox.cc			\
	poppler-link.cc				\
	poppler-annotation.cc			\
	poppler-link-extractor.cc		\
	poppler-optcontent.h			\
	poppler-page-transition.cc		\
	poppler-sound.cc			\
	poppler-form.cc				\
	poppler-base-converter.cc		\
	poppler-ps-converter.cc			\
	poppler-pdf-converter.cc		\
	poppler-qiodeviceoutstream.cc		\
	poppler-private.cc			\
	poppler-movie.cc			\
	poppler-media.cc			\
	poppler-annotation-helper.h		\
	poppler-page-private.h			\
	poppler-link-extractor-private.h	\
	poppler-annotation-private.h		\
	poppler-converter-private.h		\
	poppler-qiodeviceoutstream-private.h	\
	poppler-optcontent-private.h		\
	poppler-export.h			\
	poppler-private.h			\
	poppler-page-transition-private.h	\
	poppler-embeddedfile-private.h		\
	ArthurOutputDev.h			\
	ArthurOutputDev.cc

libpoppler_qt5_la_CPPFLAGS =			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/poppler			\
	$(POPPLER_QT5_CFLAGS)			\
	$(NSS3_CFLAGS)                          \
	$(LCMS_CFLAGS)				\
	$(FREETYPE_CFLAGS)

libpoppler_qt5_la_CXXFLAGS =			\
	-Dpoppler_qt5_EXPORTS

libpoppler_qt5_la_LIBADD =			\
	$(top_builddir)/poppler/libpoppler.la	\
	$(POPPLER_QT5_LIBS)

libpoppler_qt5_la_LDFLAGS =			\
	-version-info @POPPLER_QT5_SO_VERSION@	\
	@create_shared_lib@			\
	@auto_import_flags@

