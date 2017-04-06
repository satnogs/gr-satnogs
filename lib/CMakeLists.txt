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

include_directories(
    ${Boost_INCLUDE_DIR}
    ${VOLK_INCLUDE_DIRS}
    ${VORBIS_INCLUDE_DIR}
    ${OGG_INCLUDE_DIR}
    ${PNG_INCLUDE_DIR}
)

link_directories(${Boost_LIBRARY_DIRS})

list(APPEND satnogs_debug_sources
	morse_debug_source_impl.cc
	debug_msg_source_impl.cc
	debug_msg_source_raw_impl.cc
	leo_channel_impl.cc 
)
list(APPEND satnogs_sources
    morse_tree.cc
    morse_decoder_impl.cc
    multi_format_msg_sink_impl.cc
    ogg_encoder_impl.cc 
    cw_to_symbol_impl.cc
    sine_matched_filter_ff_impl.cc
    udp_msg_source_impl.cc
    tcp_rigctl_msg_source_impl.cc
    doppler_correction_cc_impl.cc
    frame_encoder_impl.cc
    doppler_fit.cc
    freq_drift.cc
    upsat_fsk_frame_acquisition_impl.cc
    upsat_fsk_frame_encoder_impl.cc
    whitening.cc
    udp_msg_sink_impl.cc
    coarse_doppler_correction_cc_impl.cc
    ax25_encoder_mb_impl.cc
    ax25_decoder_bm_impl.cc
    qb50_deframer_impl.cc
    waterfall_sink_impl.cc
    ogg_source_impl.cc
    noaa_apt_sink_impl.cc
    frame_file_sink_impl.cc
    iq_sink_impl.cc)

if(${INCLUDE_DEBUG_BLOCKS})
	list(APPEND satnogs_sources ${satnogs_debug_sources})
endif()

set(satnogs_sources "${satnogs_sources}" PARENT_SCOPE)
if(NOT satnogs_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT satnogs_sources)

add_library(gnuradio-satnogs SHARED ${satnogs_sources})
target_link_libraries(gnuradio-satnogs 
    ${Boost_LIBRARIES} 
    ${GNURADIO_ALL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${NOVA_LIBRARIES}
    ${VOLK_LIBRARIES}
    ${OGGVORBIS_LIBRARIES}
    ${PNG_LIBRARIES}
)
    
set_target_properties(gnuradio-satnogs PROPERTIES DEFINE_SYMBOL "gnuradio_satnogs_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-satnogs PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-satnogs RUNTIME_COMPONENT "satnogs_runtime" DEVEL_COMPONENT "satnogs_devel")

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")
