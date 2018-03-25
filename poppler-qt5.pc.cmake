prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: poppler-qt5
Description: Qt5 bindings for poppler
Version: @POPPLER_VERSION@
Requires: @PC_REQUIRES_POPPLER@
@PC_REQUIRES_POPPLER_PRIVATE@

Libs: -L${libdir} -lpoppler-qt5
Cflags: -I${includedir}/poppler/qt5
