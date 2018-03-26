prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: poppler-glib
Description: GLib wrapper for poppler
Version: @POPPLER_VERSION@
Requires: glib-2.0 >= @GLIB_REQUIRED@ gobject-2.0 >= @GLIB_REQUIRED@ cairo >= @CAIRO_VERSION@ @POPPLER_GLIB_REQUIRES_EXTRA@
@POPPLER_GLIB_REQUIRES_PRIVATE@

Cflags: -I${includedir}/poppler/glib
Libs: -L${libdir} -lpoppler-glib @POPPLER_GLIB_LIBS_EXTRA@
@POPPLER_GLIB_LIBS_PRIVATE@
