# Copyright 2011,2012 Free Software Foundation, Inc.
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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND satnogs_sources
    cw_matched_filter_ff_impl.cc
    morse_tree.cc
    morse_decoder_impl.cc
    morse_debug_source_impl.cc
    clear_text_msg_sink_impl.cc
    cw_to_symbol_impl.cc
    afsk_decoder_impl.cc
    sine_matched_filter_ff_impl.cc
    ax25_encoder_bf_impl.cc
    ax25_decoder_b_impl.cc
    udp_msg_source_impl.cc
    debug_msg_source_impl.cc
    tcp_rigctl_msg_source_impl.cc
    json_to_ecss_src_impl.cc 
    doppler_correction_cc_impl.cc
    frame_encoder_impl.cc
    doppler_fit.cc
    freq_drift.cc
    upsat_fsk_frame_acquisition_impl.cc )

set(satnogs_sources "${satnogs_sources}" PARENT_SCOPE)
if(NOT satnogs_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT satnogs_sources)

add_library(gnuradio-satnogs SHARED ${satnogs_sources})
target_link_libraries(gnuradio-satnogs 
    ${Boost_LIBRARIES} 
    ${GNURADIO_ALL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT})
    
set_target_properties(gnuradio-satnogs PROPERTIES DEFINE_SYMBOL "gnuradio_satnogs_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-satnogs PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-satnogs
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

#include_directories(${CPPUNIT_INCLUDE_DIRS})

#list(APPEND test_satnogs_sources
#    ${CMAKE_CURRENT_SOURCE_DIR}/test_satnogs.cc
#    ${CMAKE_CURRENT_SOURCE_DIR}/qa_satnogs.cc
#)

#add_executable(test-satnogs ${test_satnogs_sources})

#target_link_libraries(
#  test-satnogs
#  ${GNURADIO_RUNTIME_LIBRARIES}
#  ${Boost_LIBRARIES}
#  ${CPPUNIT_LIBRARIES}
#  gnuradio-satnogs
#)

#GR_ADD_TEST(test_satnogs test-satnogs)
