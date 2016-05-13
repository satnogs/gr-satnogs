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
install(FILES
    satnogs_cw_matched_filter_ff.xml
    satnogs_morse_decoder.xml
    satnogs_morse_debug_source.xml
    satnogs_clear_text_msg_sink.xml
    satnogs_cw_to_symbol.xml
    satnogs_afsk_decoder.xml
    satnogs_sine_matched_filter_ff.xml
    satnogs_ax25_encoder_bf.xml
    satnogs_ax25_decoder_b.xml
    satnogs_udp_msg_source.xml
    satnogs_debug_msg_source.xml
    satnogs_json_to_ecss_src.xml
    satnogs_tcp_rigctl_msg_source.xml
    satnogs_frame_encoder.xml
    satnogs_doppler_correction_cc.xml
    satnogs_upsat_fsk_frame_acquisition.xml
    satnogs_upsat_fsk_frame_encoder.xml
    satnogs_whitening.xml
    satnogs_udp_msg_sink.xml
    satnogs_receiver.xml DESTINATION share/gnuradio/grc/blocks
)
