AM_LDFLAGS = @auto_import_flags@

noinst_PROGRAMS = poppler_qt5viewer

BUILT_SOURCES_MOC =				\
	abstractinfodock.moc			\
	embeddedfiles.moc			\
	fonts.moc				\
	info.moc				\
	metadata.moc				\
	navigationtoolbar.moc			\
	optcontent.moc				\
	pageview.moc				\
	permissions.moc				\
	thumbnails.moc				\
	toc.moc					\
	viewer.moc

BUILT_SOURCES_CC = $(BUILT_SOURCES_MOC:.moc=.cc)
BUILT_SOURCES = $(BUILT_SOURCES_MOC) $(BUILT_SOURCES_CC)
EXTRA_DIST = $(BUILT_SOURCES_MOC:.moc=.cpp)
nodist_poppler_qt5viewer_SOURCES = $(BUILT_SOURCES_CC)

.cpp.cc: $(@:.cc=.cpp) $(@:.cc=.moc)
	$(AM_V_GEN) cat $(srcdir)/$(@:.cc=.cpp) $(builddir)/$(@:.cc=.moc) > $@

poppler_qt5viewer_SOURCES =			\
	abstractinfodock.h			\
	documentobserver.cpp			\
	documentobserver.h			\
	embeddedfiles.h				\
	fonts.h					\
	info.h					\
	main_viewer.cpp				\
	metadata.h				\
	navigationtoolbar.h			\
	optcontent.h				\
	pageview.h				\
	permissions.h				\
	thumbnails.h				\
	toc.h					\
	viewer.h

poppler_qt5viewer_CPPFLAGS =			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/poppler			\
	-I$(top_srcdir)/qt5/src			\
	$(POPPLER_QT5_CFLAGS)

poppler_qt5viewer_LDADD =				\
	$(top_builddir)/poppler/libpoppler.la		\
	$(top_builddir)/qt5/src/libpoppler-qt5.la	\
	$(POPPLER_QT5_LIBS)

# This rule lets GNU make create any *.moc from the equivalent *.h
.h.moc:
	$(AM_V_GEN) $(MOCQT5) -i $< -o $@
SUFFIXES: .moc

clean-generic:
	rm -f *.moc
