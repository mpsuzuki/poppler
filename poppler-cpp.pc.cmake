prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: poppler-cpp
Description: cpp backend for Poppler PDF rendering library
Version: @POPPLER_VERSION@
Requires: @POPPLER_CPP_REQUIRES_EXTRA@
@POPPLER_CPP_REQUIRES_PRIVATE@

Cflags: -I${includedir}/poppler/cpp
Libs: -L${libdir} -lpoppler-cpp @POPPLER_CPP_LIBS_EXTRA@
@POPPLER_CPP_LIBS_PRIVATE@
