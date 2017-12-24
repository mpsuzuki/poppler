Name: poppler-qt5
Description: Qt5 bindings for poppler - uninstalled
Version: @POPPLER_VERSION@
Requires: poppler = @POPPLER_VERSION@ 

Libs: ${pc_top_builddir}/${pcfiledir}/qt5/src/libpoppler-qt5.la
Cflags: -I${pc_top_builddir}/${pcfiledir}/qt5/src
