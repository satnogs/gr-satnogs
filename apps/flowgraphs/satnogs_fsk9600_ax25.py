#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: FSK9600 AX.25 decoder with G3RUH support
# Author: Manolis Surligas (surligas@gmail.com)
# Description: FSK9600 AX.25 decoder with G3RUH support
# Generated: Sun Jul 30 00:21:32 2017
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import math
import osmosdr
import satnogs
import time


class satnogs_fsk9600_ax25(gr.top_block):

    def __init__(self, doppler_correction_per_sec=1000, file_path='test.wav', lo_offset=100e3, ppm=0, rigctl_port=4532, rx_freq=100e6, rx_sdr_device='usrpb200', waterfall_file_path='/tmp/waterfall.dat'):
        gr.top_block.__init__(self, "FSK9600 AX.25 decoder with G3RUH support")

        ##################################################
        # Parameters
        ##################################################
        self.doppler_correction_per_sec = doppler_correction_per_sec
        self.file_path = file_path
        self.lo_offset = lo_offset
        self.ppm = ppm
        self.rigctl_port = rigctl_port
        self.rx_freq = rx_freq
        self.rx_sdr_device = rx_sdr_device
        self.waterfall_file_path = waterfall_file_path

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_rx = samp_rate_rx = satnogs.hw_rx_settings[rx_sdr_device]['samp_rate']
        self.deviation = deviation = 5000
        self.baud_rate = baud_rate = 9600
        self.xlate_filter_taps = xlate_filter_taps = firdes.low_pass(1, samp_rate_rx, 125000, 25000, firdes.WIN_HAMMING, 6.76)

        self.taps = taps = firdes.low_pass(12.0, samp_rate_rx, 100e3, 60000, firdes.WIN_HAMMING, 6.76)

        self.modulation_index = modulation_index = deviation / (baud_rate / 2.0)
        self.filter_rate = filter_rate = 250000
        self.audio_samp_rate = audio_samp_rate = 48000
        self.audio_gain = audio_gain = satnogs.fm_demod_settings[rx_sdr_device]['audio_gain']

        ##################################################
        # Blocks
        ##################################################
        self.satnogs_waterfall_sink_0 = satnogs.waterfall_sink(audio_samp_rate, 0.0, 8, 1024, waterfall_file_path, 1)
        self.satnogs_tcp_rigctl_msg_source_0 = satnogs.tcp_rigctl_msg_source("127.0.0.1", rigctl_port, False, 1000, 1500)
        self.satnogs_ogg_encoder_0 = satnogs.ogg_encoder(file_path, audio_samp_rate, 1.0)
        self.satnogs_multi_format_msg_sink_0_0 = satnogs.multi_format_msg_sink(1)
        self.satnogs_multi_format_msg_sink_0 = satnogs.multi_format_msg_sink(1)
        self.satnogs_coarse_doppler_correction_cc_0 = satnogs.coarse_doppler_correction_cc(rx_freq, samp_rate_rx)
        self.satnogs_ax25_decoder_bm_0 = satnogs.ax25_decoder_bm('GND', 0, True, True, 1024, 3)
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

        self.low_pass_filter_0 = filter.fir_filter_fff(1, firdes.low_pass(
        	1, audio_samp_rate, 7850, 0.15, firdes.WIN_HAMMING, 6.76))
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(int(samp_rate_rx/filter_rate), (xlate_filter_taps), lo_offset, samp_rate_rx)
        self.digital_clock_recovery_mm_xx_0 = digital.clock_recovery_mm_ff(48e3/9600, 0.25*0.175*0.175, 0.5, 0.175, 0.005)
        self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
        self.dc_blocker_xx_0 = filter.dc_blocker_ff(1024, True)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_gr_complex*1, '/tmp/iq.dat', False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self.blks2_rational_resampler_xxx_1 = filter.rational_resampler_ccc(
                interpolation=24,
                decimation=125,
                taps=None,
                fractional_bw=None,
        )
        self.analog_quadrature_demod_cf_0_0 = analog.quadrature_demod_cf(((audio_samp_rate) / baud_rate)/(math.pi*modulation_index))

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.satnogs_ax25_decoder_bm_0, 'failed_pdu'), (self.satnogs_multi_format_msg_sink_0, 'in'))
        self.msg_connect((self.satnogs_ax25_decoder_bm_0, 'pdu'), (self.satnogs_multi_format_msg_sink_0_0, 'in'))
        self.msg_connect((self.satnogs_tcp_rigctl_msg_source_0, 'freq'), (self.satnogs_coarse_doppler_correction_cc_0, 'freq'))
        self.connect((self.analog_quadrature_demod_cf_0_0, 0), (self.dc_blocker_xx_0, 0))
        self.connect((self.blks2_rational_resampler_xxx_1, 0), (self.analog_quadrature_demod_cf_0_0, 0))
        self.connect((self.blks2_rational_resampler_xxx_1, 0), (self.blocks_file_sink_0, 0))
        self.connect((self.blks2_rational_resampler_xxx_1, 0), (self.satnogs_waterfall_sink_0, 0))
        self.connect((self.dc_blocker_xx_0, 0), (self.low_pass_filter_0, 0))
        self.connect((self.digital_binary_slicer_fb_0, 0), (self.satnogs_ax25_decoder_bm_0, 0))
        self.connect((self.digital_clock_recovery_mm_xx_0, 0), (self.digital_binary_slicer_fb_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.blks2_rational_resampler_xxx_1, 0))
        self.connect((self.low_pass_filter_0, 0), (self.digital_clock_recovery_mm_xx_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.satnogs_ogg_encoder_0, 0))
        self.connect((self.osmosdr_source_0, 0), (self.satnogs_coarse_doppler_correction_cc_0, 0))
        self.connect((self.satnogs_coarse_doppler_correction_cc_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))

    def get_doppler_correction_per_sec(self):
        return self.doppler_correction_per_sec

    def set_doppler_correction_per_sec(self, doppler_correction_per_sec):
        self.doppler_correction_per_sec = doppler_correction_per_sec

    def get_file_path(self):
        return self.file_path

    def set_file_path(self, file_path):
        self.file_path = file_path

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
        self.set_audio_gain(satnogs.fm_demod_settings[self.rx_sdr_device]['audio_gain'])

    def get_waterfall_file_path(self):
        return self.waterfall_file_path

    def set_waterfall_file_path(self, waterfall_file_path):
        self.waterfall_file_path = waterfall_file_path

    def get_samp_rate_rx(self):
        return self.samp_rate_rx

    def set_samp_rate_rx(self, samp_rate_rx):
        self.samp_rate_rx = samp_rate_rx
        self.set_xlate_filter_taps(firdes.low_pass(1, self.samp_rate_rx, 125000, 25000, firdes.WIN_HAMMING, 6.76))
        self.osmosdr_source_0.set_sample_rate(self.samp_rate_rx)
        self.osmosdr_source_0.set_bandwidth(self.samp_rate_rx, 0)

    def get_deviation(self):
        return self.deviation

    def set_deviation(self, deviation):
        self.deviation = deviation
        self.set_modulation_index(self.deviation / (self.baud_rate / 2.0))

    def get_baud_rate(self):
        return self.baud_rate

    def set_baud_rate(self, baud_rate):
        self.baud_rate = baud_rate
        self.set_modulation_index(self.deviation / (self.baud_rate / 2.0))
        self.analog_quadrature_demod_cf_0_0.set_gain(((self.audio_samp_rate) / self.baud_rate)/(math.pi*self.modulation_index))

    def get_xlate_filter_taps(self):
        return self.xlate_filter_taps

    def set_xlate_filter_taps(self, xlate_filter_taps):
        self.xlate_filter_taps = xlate_filter_taps
        self.freq_xlating_fir_filter_xxx_0.set_taps((self.xlate_filter_taps))

    def get_taps(self):
        return self.taps

    def set_taps(self, taps):
        self.taps = taps

    def get_modulation_index(self):
        return self.modulation_index

    def set_modulation_index(self, modulation_index):
        self.modulation_index = modulation_index
        self.analog_quadrature_demod_cf_0_0.set_gain(((self.audio_samp_rate) / self.baud_rate)/(math.pi*self.modulation_index))

    def get_filter_rate(self):
        return self.filter_rate

    def set_filter_rate(self, filter_rate):
        self.filter_rate = filter_rate

    def get_audio_samp_rate(self):
        return self.audio_samp_rate

    def set_audio_samp_rate(self, audio_samp_rate):
        self.audio_samp_rate = audio_samp_rate
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.audio_samp_rate, 7850, 0.15, firdes.WIN_HAMMING, 6.76))
        self.analog_quadrature_demod_cf_0_0.set_gain(((self.audio_samp_rate) / self.baud_rate)/(math.pi*self.modulation_index))

    def get_audio_gain(self):
        return self.audio_gain

    def set_audio_gain(self, audio_gain):
        self.audio_gain = audio_gain


def argument_parser():
    description = 'FSK9600 AX.25 decoder with G3RUH support'
    parser = OptionParser(usage="%prog: [options]", option_class=eng_option, description=description)
    parser.add_option(
        "", "--doppler-correction-per-sec", dest="doppler_correction_per_sec", type="intx", default=1000,
        help="Set doppler_correction_per_sec [default=%default]")
    parser.add_option(
        "", "--file-path", dest="file_path", type="string", default='test.wav',
        help="Set file_path [default=%default]")
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
        "", "--rx-freq", dest="rx_freq", type="eng_float", default=eng_notation.num_to_str(100e6),
        help="Set rx_freq [default=%default]")
    parser.add_option(
        "", "--rx-sdr-device", dest="rx_sdr_device", type="string", default='usrpb200',
        help="Set rx_sdr_device [default=%default]")
    parser.add_option(
        "", "--waterfall-file-path", dest="waterfall_file_path", type="string", default='/tmp/waterfall.dat',
        help="Set waterfall_file_path [default=%default]")
    return parser


def main(top_block_cls=satnogs_fsk9600_ax25, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(doppler_correction_per_sec=options.doppler_correction_per_sec, file_path=options.file_path, lo_offset=options.lo_offset, ppm=options.ppm, rigctl_port=options.rigctl_port, rx_freq=options.rx_freq, rx_sdr_device=options.rx_sdr_device, waterfall_file_path=options.waterfall_file_path)
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
