Name: poppler-cpp
Description: cpp backend for Poppler PDF rendering library - uninstalled
Version: @POPPLER_VERSION@
Requires: poppler = @POPPLER_VERSION@

Libs: ${pc_top_builddir}/${pcfiledir}/cpp/libpoppler-cpp.la
Cflags: -I${pc_top_builddir}/${pcfiledir}/cpp
