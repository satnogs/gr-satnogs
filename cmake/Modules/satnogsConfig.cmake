INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_SATNOGS satnogs)

FIND_PATH(
    SATNOGS_INCLUDE_DIRS
    NAMES satnogs/api.h
    HINTS $ENV{SATNOGS_DIR}/include
        ${PC_SATNOGS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SATNOGS_LIBRARIES
    NAMES gnuradio-satnogs
    HINTS $ENV{SATNOGS_DIR}/lib
        ${PC_SATNOGS_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SATNOGS DEFAULT_MSG SATNOGS_LIBRARIES SATNOGS_INCLUDE_DIRS)
MARK_AS_ADVANCED(SATNOGS_LIBRARIES SATNOGS_INCLUDE_DIRS)

