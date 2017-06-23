#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: NOAA APT Decoder
# Author: Manolis Surligas, George Vardakis
# Description: A NOAA APT Decoder with automatic image synchronization
# Generated: Fri Jun 23 15:41:33 2017
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import osmosdr
import satnogs
import time


class satnogs_noaa_apt_decoder(gr.top_block):

    def __init__(self, doppler_correction_per_sec=1000, image_file_path='/tmp/noaa.png', lo_offset=100e3, ppm=0, rigctl_port=4532, rx_freq=90.4e6, rx_sdr_device='usrpb200'):
        gr.top_block.__init__(self, "NOAA APT Decoder")

        ##################################################
        # Parameters
        ##################################################
        self.doppler_correction_per_sec = doppler_correction_per_sec
        self.image_file_path = image_file_path
        self.lo_offset = lo_offset
        self.ppm = ppm
        self.rigctl_port = rigctl_port
        self.rx_freq = rx_freq
        self.rx_sdr_device = rx_sdr_device

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_rx = samp_rate_rx = satnogs.hw_rx_settings[rx_sdr_device]['samp_rate']

        self.noaa_filter_taps = noaa_filter_taps = firdes.low_pass(1.0, 1.0, 0.17, 0.02, firdes.WIN_HAMMING, 6.76)

        self.initial_bandwidth = initial_bandwidth = 100e3

        self.first_stage_filter_taps = first_stage_filter_taps = firdes.low_pass(1.0, 1.0, 0.2, 0.1, firdes.WIN_HAMMING, 6.76)

        self.first_stage_decimation = first_stage_decimation = 2
        self.audio_decimation = audio_decimation = 2

        ##################################################
        # Blocks
        ##################################################
        self.satnogs_tcp_rigctl_msg_source_0 = satnogs.tcp_rigctl_msg_source("127.0.0.1", rigctl_port, False, 1000, 1500)
        self.satnogs_noaa_apt_sink_0 = satnogs.noaa_apt_sink(image_file_path, 2080, 1500, True, False, False)
        self.satnogs_coarse_doppler_correction_cc_0 = satnogs.coarse_doppler_correction_cc(rx_freq, samp_rate_rx /first_stage_decimation)
        self.rational_resampler_xxx_0_0 = filter.rational_resampler_fff(
                interpolation=4160,
                decimation=9600,
                taps=None,
                fractional_bw=None,
        )
        self.rational_resampler_xxx_0 = filter.rational_resampler_fff(
                interpolation=9600,
                decimation=int((samp_rate_rx/ ( first_stage_decimation  * int(samp_rate_rx/ first_stage_decimation / initial_bandwidth)) / audio_decimation)/2),
                taps=None,
                fractional_bw=None,
        )
        self.osmosdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + satnogs.hw_rx_settings[rx_sdr_device]['dev_arg'] )
        self.osmosdr_source_0.set_sample_rate(samp_rate_rx)
        self.osmosdr_source_0.set_center_freq(rx_freq - lo_offset, 0)
        self.osmosdr_source_0.set_freq_corr(ppm, 0)
        self.osmosdr_source_0.set_dc_offset_mode(2, 0)
        self.osmosdr_source_0.set_iq_balance_mode(0, 0)
        self.osmosdr_source_0.set_gain_mode(False, 0)
        self.osmosdr_source_0.set_gain(satnogs.hw_rx_settings[rx_sdr_device]['rf_gain'], 0)
        self.osmosdr_source_0.set_if_gain(satnogs.hw_rx_settings[rx_sdr_device]['if_gain'], 0)
        self.osmosdr_source_0.set_bb_gain(satnogs.hw_rx_settings[rx_sdr_device]['bb_gain'], 0)
        self.osmosdr_source_0.set_antenna(satnogs.hw_rx_settings[rx_sdr_device]['antenna'], 0)
        self.osmosdr_source_0.set_bandwidth(samp_rate_rx, 0)

        self.hilbert_fc_0 = filter.hilbert_fc(65, firdes.WIN_HAMMING, 6.76)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(first_stage_decimation, (first_stage_filter_taps), lo_offset, samp_rate_rx)
        self.fir_filter_xxx_1 = filter.fir_filter_fff(2, ([0.5, 0.5]))
        self.fir_filter_xxx_1.declare_sample_delay(0)
        self.fir_filter_xxx_0 = filter.fir_filter_ccc(int(samp_rate_rx/ first_stage_decimation / initial_bandwidth), (noaa_filter_taps))
        self.fir_filter_xxx_0.declare_sample_delay(0)
        self.blocks_complex_to_mag_0 = blocks.complex_to_mag(1)
        self.band_pass_filter_0 = filter.fir_filter_fff(1, firdes.band_pass(
        	6, samp_rate_rx/ ( first_stage_decimation  * int(samp_rate_rx/ first_stage_decimation / initial_bandwidth)) / audio_decimation, 500, 4.2e3, 200, firdes.WIN_HAMMING, 6.76))
        self.analog_wfm_rcv_0 = analog.wfm_rcv(
        	quad_rate=samp_rate_rx/ ( first_stage_decimation  * int(samp_rate_rx/ first_stage_decimation / initial_bandwidth)),
        	audio_decimation=audio_decimation,
        )

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.satnogs_tcp_rigctl_msg_source_0, 'freq'), (self.satnogs_coarse_doppler_correction_cc_0, 'freq'))
        self.connect((self.analog_wfm_rcv_0, 0), (self.band_pass_filter_0, 0))
        self.connect((self.band_pass_filter_0, 0), (self.fir_filter_xxx_1, 0))
        self.connect((self.blocks_complex_to_mag_0, 0), (self.rational_resampler_xxx_0_0, 0))
        self.connect((self.fir_filter_xxx_0, 0), (self.analog_wfm_rcv_0, 0))
        self.connect((self.fir_filter_xxx_1, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.satnogs_coarse_doppler_correction_cc_0, 0))
        self.connect((self.hilbert_fc_0, 0), (self.blocks_complex_to_mag_0, 0))
        self.connect((self.osmosdr_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.hilbert_fc_0, 0))
        self.connect((self.rational_resampler_xxx_0_0, 0), (self.satnogs_noaa_apt_sink_0, 0))
        self.connect((self.satnogs_coarse_doppler_correction_cc_0, 0), (self.fir_filter_xxx_0, 0))

    def get_doppler_correction_per_sec(self):
        return self.doppler_correction_per_sec

    def set_doppler_correction_per_sec(self, doppler_correction_per_sec):
        self.doppler_correction_per_sec = doppler_correction_per_sec

    def get_image_file_path(self):
        return self.image_file_path

    def set_image_file_path(self, image_file_path):
        self.image_file_path = image_file_path

    def get_lo_offset(self):
        return self.lo_offset

    def set_lo_offset(self, lo_offset):
        self.lo_offset = lo_offset
        self.osmosdr_source_0.set_center_freq(self.rx_freq - self.lo_offset, 0)
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.lo_offset)

    def get_ppm(self):
        return self.ppm

    def set_ppm(self, ppm):
        self.ppm = ppm
        self.osmosdr_source_0.set_freq_corr(self.ppm, 0)

    def get_rigctl_port(self):
        return self.rigctl_port

    def set_rigctl_port(self, rigctl_port):
        self.rigctl_port = rigctl_port

    def get_rx_freq(self):
        return self.rx_freq

    def set_rx_freq(self, rx_freq):
        self.rx_freq = rx_freq
        self.satnogs_coarse_doppler_correction_cc_0.set_new_freq_locked(self.rx_freq)
        self.osmosdr_source_0.set_center_freq(self.rx_freq - self.lo_offset, 0)

    def get_rx_sdr_device(self):
        return self.rx_sdr_device

    def set_rx_sdr_device(self, rx_sdr_device):
        self.rx_sdr_device = rx_sdr_device
        self.set_samp_rate_rx(satnogs.hw_rx_settings[self.rx_sdr_device]['samp_rate'])
        self.osmosdr_source_0.set_gain(satnogs.hw_rx_settings[self.rx_sdr_device]['rf_gain'], 0)
        self.osmosdr_source_0.set_if_gain(satnogs.hw_rx_settings[self.rx_sdr_device]['if_gain'], 0)
        self.osmosdr_source_0.set_bb_gain(satnogs.hw_rx_settings[self.rx_sdr_device]['bb_gain'], 0)
        self.osmosdr_source_0.set_antenna(satnogs.hw_rx_settings[self.rx_sdr_device]['antenna'], 0)

    def get_samp_rate_rx(self):
        return self.samp_rate_rx

    def set_samp_rate_rx(self, samp_rate_rx):
        self.samp_rate_rx = samp_rate_rx
        self.osmosdr_source_0.set_sample_rate(self.samp_rate_rx)
        self.osmosdr_source_0.set_bandwidth(self.samp_rate_rx, 0)
        self.band_pass_filter_0.set_taps(firdes.band_pass(6, self.samp_rate_rx/ ( self.first_stage_decimation  * int(self.samp_rate_rx/ self.first_stage_decimation / self.initial_bandwidth)) / self.audio_decimation, 500, 4.2e3, 200, firdes.WIN_HAMMING, 6.76))

    def get_noaa_filter_taps(self):
        return self.noaa_filter_taps

    def set_noaa_filter_taps(self, noaa_filter_taps):
        self.noaa_filter_taps = noaa_filter_taps
        self.fir_filter_xxx_0.set_taps((self.noaa_filter_taps))

    def get_initial_bandwidth(self):
        return self.initial_bandwidth

    def set_initial_bandwidth(self, initial_bandwidth):
        self.initial_bandwidth = initial_bandwidth
        self.band_pass_filter_0.set_taps(firdes.band_pass(6, self.samp_rate_rx/ ( self.first_stage_decimation  * int(self.samp_rate_rx/ self.first_stage_decimation / self.initial_bandwidth)) / self.audio_decimation, 500, 4.2e3, 200, firdes.WIN_HAMMING, 6.76))

    def get_first_stage_filter_taps(self):
        return self.first_stage_filter_taps

    def set_first_stage_filter_taps(self, first_stage_filter_taps):
        self.first_stage_filter_taps = first_stage_filter_taps
        self.freq_xlating_fir_filter_xxx_0.set_taps((self.first_stage_filter_taps))

    def get_first_stage_decimation(self):
        return self.first_stage_decimation

    def set_first_stage_decimation(self, first_stage_decimation):
        self.first_stage_decimation = first_stage_decimation
        self.band_pass_filter_0.set_taps(firdes.band_pass(6, self.samp_rate_rx/ ( self.first_stage_decimation  * int(self.samp_rate_rx/ self.first_stage_decimation / self.initial_bandwidth)) / self.audio_decimation, 500, 4.2e3, 200, firdes.WIN_HAMMING, 6.76))

    def get_audio_decimation(self):
        return self.audio_decimation

    def set_audio_decimation(self, audio_decimation):
        self.audio_decimation = audio_decimation
        self.band_pass_filter_0.set_taps(firdes.band_pass(6, self.samp_rate_rx/ ( self.first_stage_decimation  * int(self.samp_rate_rx/ self.first_stage_decimation / self.initial_bandwidth)) / self.audio_decimation, 500, 4.2e3, 200, firdes.WIN_HAMMING, 6.76))


def argument_parser():
    description = 'A NOAA APT Decoder with automatic image synchronization'
    parser = OptionParser(usage="%prog: [options]", option_class=eng_option, description=description)
    parser.add_option(
        "", "--doppler-correction-per-sec", dest="doppler_correction_per_sec", type="intx", default=1000,
        help="Set doppler_correction_per_sec [default=%default]")
    parser.add_option(
        "", "--image-file-path", dest="image_file_path", type="string", default='/tmp/noaa.png',
        help="Set image_file_path [default=%default]")
    parser.add_option(
        "", "--lo-offset", dest="lo_offset", type="eng_float", default=eng_notation.num_to_str(100e3),
        help="Set lo_offset [default=%default]")
    parser.add_option(
        "", "--ppm", dest="ppm", type="intx", default=0,
        help="Set ppm [default=%default]")
    parser.add_option(
        "", "--rigctl-port", dest="rigctl_port", type="intx", default=4532,
        help="Set rigctl_port [default=%default]")
    parser.add_option(
        "", "--rx-freq", dest="rx_freq", type="eng_float", default=eng_notation.num_to_str(90.4e6),
        help="Set rx_freq [default=%default]")
    parser.add_option(
        "", "--rx-sdr-device", dest="rx_sdr_device", type="string", default='usrpb200',
        help="Set rx_sdr_device [default=%default]")
    return parser


def main(top_block_cls=satnogs_noaa_apt_decoder, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(doppler_correction_per_sec=options.doppler_correction_per_sec, image_file_path=options.image_file_path, lo_offset=options.lo_offset, ppm=options.ppm, rigctl_port=options.rigctl_port, rx_freq=options.rx_freq, rx_sdr_device=options.rx_sdr_device)
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
