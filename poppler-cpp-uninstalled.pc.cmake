Name: poppler-cpp
Description: cpp backend for Poppler PDF rendering library - uninstalled
Version: @POPPLER_VERSION@
Requires: poppler = @POPPLER_VERSION@

Libs: -L${pc_top_builddir}/${pcfiledir}/cpp -lpoppler-cpp -L${pc_top_builddir}/${pcfiledir}/ -lpoppler
Cflags: -I${pc_top_builddir}/${pcfiledir}/cpp -I@CMAKE_SOURCE_DIR@/cpp
