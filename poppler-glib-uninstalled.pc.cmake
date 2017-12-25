Name: poppler-glib
Description: GLib wrapper for poppler - uninstalled
Version: @POPPLER_VERSION@
Requires: glib-2.0 >= @GLIB_REQUIRED@ gobject-2.0 >= @GLIB_REQUIRED@ gio-2.0 >= @GLIB_REQUIRED@ cairo >= @CAIRO_VERSION@

Libs: -L${pc_top_builddir}/${pcfiledir}/glib -lpoppler-glib -L${pc_top_builddir}/${pcfiledir}/ -lpoppler
Cflags: -I${pc_top_builddir}/${pcfiledir}/glib
