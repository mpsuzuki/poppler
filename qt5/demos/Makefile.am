AM_LDFLAGS = @auto_import_flags@

noinst_PROGRAMS = poppler_qt5viewer

BUILT_SOURCES =					\
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

poppler_qt5viewer_SOURCES =			\
	abstractinfodock.cc			\
	abstractinfodock.h			\
	documentobserver.cpp			\
	documentobserver.h			\
	embeddedfiles.cc			\
	embeddedfiles.h				\
	fonts.cc				\
	fonts.h					\
	info.cc					\
	info.h					\
	main_viewer.cpp				\
	metadata.cc				\
	metadata.h				\
	navigationtoolbar.cc			\
	navigationtoolbar.h			\
	optcontent.cc				\
	optcontent.h				\
	pageview.cc				\
	pageview.h				\
	permissions.cc				\
	permissions.h				\
	thumbnails.cc				\
	thumbnails.h				\
	toc.cc					\
	toc.h					\
	viewer.cc				\
	viewer.h

BUILT_SOURCES += \
	abstractinfodock.cc \
	embeddedfiles.cc \
	fonts.cc \
	info.cc \
	metadata.cc \
	navigationtoolbar.cc \
	optcontent.cc \
	pageview.cc \
	permissions.cc \
	thumbnails.cc \
	toc.cc \
	viewer.cc

.cpp.cc: $(@:.cc=.cpp) $(@:.cc=.moc)
	cat $(srcdir)/$(@:.cc=.cpp) $(builddir)/$(@:.cc=.moc) > $@

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
