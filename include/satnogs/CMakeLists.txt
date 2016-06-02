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
# Install public header files
########################################################################
install(FILES
    api.h
    ax25.h
    config.h
    cw_matched_filter_ff.h
    log.h
    morse_tree.h
    morse.h
    morse_decoder.h
    morse_debug_source.h
    multi_format_msg_sink.h
    cw_to_symbol.h
    afsk_decoder.h
    sine_matched_filter_ff.h
    utils.h
    ax25_encoder_bf.h
    ax25_decoder_b.h
    udp_msg_source.h
    debug_msg_source.h
    json_to_ecss_src.h
    tcp_rigctl_msg_source.h
    frame_encoder.h
    doppler_correction_cc.h
    doppler_fit.h
    freq_drift.h
    upsat_fsk_frame_acquisition.h
    upsat_fsk_frame_encoder.h
    whitening.h
    udp_msg_sink.h
    coarse_doppler_correction_cc.h
    debug_msg_source_raw.h DESTINATION include/satnogs
)
