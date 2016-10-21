INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_NOVA Nova)

FIND_PATH(
    NOVA_INCLUDE_DIRS
    NAMES libnova/libnova.h
    HINTS $ENV{NOVA_DIR}/include
        ${PC_NOVA_INCLUDEDIR}
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    NOVA_LIBRARIES
    NAMES nova
    HINTS $ENV{NOVA_DIR}/lib
        ${PC_NOVA_LIBDIR}
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NOVA DEFAULT_MSG NOVA_LIBRARIES NOVA_INCLUDE_DIRS)
MARK_AS_ADVANCED(NOVA_LIBRARIES NOVA_INCLUDE_DIRS)