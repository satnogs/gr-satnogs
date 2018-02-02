# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT satnogs_sources)
    MESSAGE(STATUS "No C++ sources... skipping swig/")
    return()
endif(NOT satnogs_sources)

########################################################################
# Include swig generation macros
########################################################################
find_package(SWIG)
find_package(PythonLibs 2)
if(NOT SWIG_FOUND OR NOT PYTHONLIBS_FOUND)
    return()
endif()
include(GrSwig)
include(GrPython)

########################################################################
# Setup swig generation
########################################################################
foreach(incdir ${GNURADIO_RUNTIME_INCLUDE_DIRS})
    list(APPEND GR_SWIG_INCLUDE_DIRS ${incdir}/gnuradio/swig)
endforeach(incdir)

if(${INCLUDE_DEBUG_BLOCKS})
    set(INCLUDE_DEBUG_BLOCKS_BOOL_VAL 1)
else()
    set(INCLUDE_DEBUG_BLOCKS_BOOL_VAL 0)
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/satnogs_swig.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/satnogs_swig.py
@ONLY)

if(${INCLUDE_DEBUG_BLOCKS})
    set(SATONGS_SWIG_FILES
        satnogs_swig0
        satnogs_debug_swig
    )
else()
    set(SATONGS_SWIG_FILES
        satnogs_swig0
    )
endif()

foreach(swigfile ${SATONGS_SWIG_FILES})
  set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
  set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include)
  
  set(GR_SWIG_LIBRARIES gnuradio-satnogs)
  GR_SWIG_MAKE(${swigfile} ${swigfile}.i)

  GR_SWIG_INSTALL(TARGETS ${swigfile} DESTINATION ${GR_PYTHON_DIR}/satnogs)

  list(APPEND SWIGFILES ${swigfile}.i)
  list(APPEND SWIGDOCFILES ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
endforeach(swigfile)

install(
    FILES
    ${SWIGFILES}
    ${SWIGDOCFILES}
    DESTINATION ${GR_INCLUDE_DIR}/satnogs/swig
)

# Install the Python file that pulls in the swig built files.
GR_PYTHON_INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/satnogs_swig.py
    DESTINATION ${GR_PYTHON_DIR}/satnogs
    COMPONENT "satnogs_python"
)
