# ---------------------------------------------------------------
# generate conventional LIBS flags from pathname of the libraries
#
function (MAKE_LDFLAGS_AND_LIBS_FROM_LIBRARY_PATHNAME pathname libname libdir)
  string(REGEX REPLACE "/[^/]+$" "/" _libdir "${pathname}")

  string(REGEX REPLACE "^.*/lib" "" _libname "${pathname}")
  if (${libname} MATCHES ".*\\${CMAKE_STATIC_LIBRARY_SUFFIX}$")
    string(REGEX REPLACE "\\${CMAKE_STATIC_LIBRARY_SUFFIX}$" "" _libname "${_libname}")
  else()
    string(REGEX REPLACE "\\${CMAKE_SHARED_LIBRARY_SUFFIX}$" "" _libname "${_libname}")
  endif()

  string(REGEX REPLACE "\/+$" "" _libdir "${_libdir}")

  set("${libname}" "${_libname}" PARENT_SCOPE)
  set("${libdir}" "${_libdir}" PARENT_SCOPE)
endfunction()

# ---------------------------------------------------------------
# if package is maganed by pkg-config, append it list_requires.
# if not, append LDFLAGS + LIBS to list_libs.
#
function (APPEND_PKG_REQUIRES_OR_LIBS pkgnames library list_requires list_libs)
  # message("")
  # message("check pkg-config + flags for ${library}")
  MAKE_LDFLAGS_AND_LIBS_FROM_LIBRARY_PATHNAME("${library}" pkg_libname pkg_libdir)

  foreach(a_pkg IN LISTS pkgnames)
    execute_process(COMMAND "${PKG_CONFIG_EXECUTABLE}" ${a_pkg} --exists
                    RESULT_VARIABLE _result_VARIABLE
                    OUTPUT_VARIABLE _pkgconfigDevNull )
    # message("check pkg-config --exists ${a_pkg}: ${_result_VARIABLE}")
    if (_result_VARIABLE STREQUAL "0")
      execute_process(COMMAND "${PKG_CONFIG_EXECUTABLE}" ${a_pkg} --variable=libdir
                      RESULT_VARIABLE _result_VARIABLE
                      OUTPUT_VARIABLE _output_VARIABLE
                      OUTPUT_STRIP_TRAILING_WHITESPACE)
      # message("check pkg-config --variable=libdir ${a_pkg}: ${_output_VARIABLE}")
      # message("${_output_VARIABLE} == ${pkg_libdir}")
      if ("${_output_VARIABLE}" STREQUAL "${pkg_libdir}")
        execute_process(COMMAND "${PKG_CONFIG_EXECUTABLE}" ${a_pkg} --libs
                        RESULT_VARIABLE _result_VARIABLE
                        OUTPUT_VARIABLE _output_VARIABLE
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
        # message("check pkg-config --libs ${a_pkg}: ${_output_VARIABLE}")
        string(REGEX REPLACE "\/+$" "" _output_VARIABLE "${_output_VARIABLE}")
        string(FIND " ${_output_VARIABLE} " " -l${pkg_libname} " _pos)
        if (_pos GREATER -1)
          set(${list_requires} "${${list_requires}} ${a_pkg}" PARENT_SCOPE)
          return()
        endif() 
      else()
        # message("*** different directories")
      endif()
    endif()
  endforeach(a_pkg)

  set(${list_libs} "${${list_libs}} -L${pkg_libdir} -l${pkg_libname}" PARENT_SCOPE)
endfunction()
