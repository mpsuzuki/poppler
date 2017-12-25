Name: poppler-qt5
Description: Qt5 bindings for poppler - uninstalled
Version: @POPPLER_VERSION@
Requires: poppler = @POPPLER_VERSION@ 

Libs: -L${pc_top_builddir}/${pcfiledir}/qt5/src -lpoppler-qt5 -L${pc_top_builddir}/${pcfiledir}/ -lpoppler
Cflags: -I${pc_top_builddir}/${pcfiledir}/qt5/src
