Name: poppler-cairo
Description: Cairo backend for Poppler PDF rendering library - uninstalled
Version: @POPPLER_VERSION@
Requires: poppler = @POPPLER_VERSION@ cairo >= @CAIRO_VERSION@

Libs: -L${pc_top_builddir}/${pcfiledir}/poppler -lpoppler-cairo
