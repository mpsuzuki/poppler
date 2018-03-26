prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: poppler-qt5
Description: Qt5 bindings for poppler
Version: @POPPLER_VERSION@
Requires: @POPPLER_QT5_REQUIRES_EXTRA@
@POPPLER_QT5_REQUIRES_PRIVATE@

Cflags: -I${includedir}/poppler/qt5
Libs: -L${libdir} -lpoppler-qt5 @POPPLER_QT5_LIBS_EXTRA@
@POPPLER_QT5_LIBS_PRIVATE@
