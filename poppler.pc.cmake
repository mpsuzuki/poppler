prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: poppler
Description: PDF rendering library
Version: @POPPLER_VERSION@
@PC_REQUIRES_POPPLER_PRIVATE@

Libs: -L${libdir} -lpoppler
Libs.private: @PC_LIBS_POPPLER_PRIVATE@
Cflags: -I${includedir}/poppler
