#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: CW Decoder
# Author: Manolis Surligas (surligas@gmail.com)
# Description: A CW (Morse) Decoder
# Generated: Sun Mar 25 17:48:02 2018
##################################################

from gnuradio import analog
from gnuradio import blocks
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


class satnogs_cw_decoder(gr.top_block):

    def __init__(self, antenna=satnogs.not_set_antenna, bb_gain=satnogs.not_set_rx_bb_gain, decoded_data_file_path='/tmp/.satnogs/data/data', dev_args=satnogs.not_set_dev_args, doppler_correction_per_sec=1000, enable_iq_dump=0, file_path='test.txt', if_gain=satnogs.not_set_rx_if_gain, iq_file_path='/tmp/iq.dat', lo_offset=100e3, ppm=0, rf_gain=satnogs.not_set_rx_rf_gain, rigctl_port=4532, rx_freq=100e6, rx_sdr_device='usrpb200', udp_IP='127.0.0.1', udp_port=16887, waterfall_file_path='/tmp/waterfall.dat', wpm=20):
        gr.top_block.__init__(self, "CW Decoder")

        ##################################################
        # Parameters
        ##################################################
        self.antenna = antenna
        self.bb_gain = bb_gain
        self.decoded_data_file_path = decoded_data_file_path
        self.dev_args = dev_args
        self.doppler_correction_per_sec = doppler_correction_per_sec
        self.enable_iq_dump = enable_iq_dump
        self.file_path = file_path
        self.if_gain = if_gain
        self.iq_file_path = iq_file_path
        self.lo_offset = lo_offset
        self.ppm = ppm
        self.rf_gain = rf_gain
        self.rigctl_port = rigctl_port
        self.rx_freq = rx_freq
        self.rx_sdr_device = rx_sdr_device
        self.udp_IP = udp_IP
        self.udp_port = udp_port
        self.waterfall_file_path = waterfall_file_path
        self.wpm = wpm

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_rx = samp_rate_rx = satnogs.hw_rx_settings[rx_sdr_device]['samp_rate']
        self.xlating_decimation = xlating_decimation = 5
        self.xlate_filter_taps = xlate_filter_taps = firdes.low_pass(1, samp_rate_rx, 125000, 25000, firdes.WIN_HAMMING, 6.76)
        self.lpf_decimation = lpf_decimation = 5
        self.audio_samp_rate = audio_samp_rate = 48000

        ##################################################
        # Blocks
        ##################################################
        self.satnogs_waterfall_sink_0 = satnogs.waterfall_sink((samp_rate_rx/xlating_decimation), 0.0, 10, 1024, waterfall_file_path, 1)
        self.satnogs_udp_msg_sink_0_0 = satnogs.udp_msg_sink(udp_IP, udp_port, 1500)
        self.satnogs_tcp_rigctl_msg_source_0 = satnogs.tcp_rigctl_msg_source("127.0.0.1", rigctl_port, False, 1000, 1500)
        self.satnogs_ogg_encoder_0 = satnogs.ogg_encoder(file_path, audio_samp_rate, 1.0)
        self.satnogs_morse_decoder_0 = satnogs.morse_decoder(ord('#'), 3)
        self.satnogs_iq_sink_0 = satnogs.iq_sink(16768, iq_file_path, False, enable_iq_dump)
        self.satnogs_frame_file_sink_0_0 = satnogs.frame_file_sink(decoded_data_file_path, 0)
        self.satnogs_cw_to_symbol_0 = satnogs.cw_to_symbol(samp_rate_rx/xlating_decimation/lpf_decimation/4, 0.25, 0.75, wpm)
        self.satnogs_coarse_doppler_correction_cc_0 = satnogs.coarse_doppler_correction_cc(rx_freq, samp_rate_rx)
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=audio_samp_rate,
                decimation=int((samp_rate_rx/xlating_decimation)/lpf_decimation),
                taps=None,
                fractional_bw=None,
        )
        self.osmosdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + satnogs.handle_rx_dev_args(rx_sdr_device, dev_args) )
        self.osmosdr_source_0.set_sample_rate(samp_rate_rx)
        self.osmosdr_source_0.set_center_freq(rx_freq - lo_offset, 0)
        self.osmosdr_source_0.set_freq_corr(ppm, 0)
        self.osmosdr_source_0.set_dc_offset_mode(2, 0)
        self.osmosdr_source_0.set_iq_balance_mode(0, 0)
        self.osmosdr_source_0.set_gain_mode(False, 0)
        self.osmosdr_source_0.set_gain(satnogs.handle_rx_rf_gain(rx_sdr_device, rf_gain), 0)
        self.osmosdr_source_0.set_if_gain(satnogs.handle_rx_if_gain(rx_sdr_device, if_gain), 0)
        self.osmosdr_source_0.set_bb_gain(satnogs.handle_rx_bb_gain(rx_sdr_device, bb_gain), 0)
        self.osmosdr_source_0.set_antenna(satnogs.handle_rx_antenna(rx_sdr_device, antenna), 0)
        self.osmosdr_source_0.set_bandwidth(samp_rate_rx, 0)

        self.low_pass_filter_0_0 = filter.fir_filter_ccf(4, firdes.low_pass(
        	4, samp_rate_rx/xlating_decimation/lpf_decimation, 100, 100, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0 = filter.fir_filter_ccf(lpf_decimation, firdes.low_pass(
        	lpf_decimation, samp_rate_rx/xlating_decimation, 2e3, 1e3, firdes.WIN_HAMMING, 6.76))
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(xlating_decimation, (xlate_filter_taps), lo_offset, samp_rate_rx)
        self.blocks_complex_to_real_0 = blocks.complex_to_real(1)
        self.blocks_complex_to_mag_squared_0 = blocks.complex_to_mag_squared(1)
        self.analog_pll_carriertracking_cc_0 = analog.pll_carriertracking_cc(2*math.pi/100, 2*math.pi*2e3/(samp_rate_rx/xlating_decimation/lpf_decimation), -2*math.pi*2e3/(samp_rate_rx/xlating_decimation/lpf_decimation))

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.satnogs_cw_to_symbol_0, 'out'), (self.satnogs_morse_decoder_0, 'in'))
        self.msg_connect((self.satnogs_morse_decoder_0, 'out'), (self.satnogs_frame_file_sink_0_0, 'frame'))
        self.msg_connect((self.satnogs_morse_decoder_0, 'out'), (self.satnogs_udp_msg_sink_0_0, 'in'))
        self.msg_connect((self.satnogs_tcp_rigctl_msg_source_0, 'freq'), (self.satnogs_coarse_doppler_correction_cc_0, 'freq'))
        self.connect((self.analog_pll_carriertracking_cc_0, 0), (self.low_pass_filter_0_0, 0))
        self.connect((self.blocks_complex_to_mag_squared_0, 0), (self.satnogs_cw_to_symbol_0, 0))
        self.connect((self.blocks_complex_to_real_0, 0), (self.satnogs_ogg_encoder_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.low_pass_filter_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.satnogs_waterfall_sink_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.analog_pll_carriertracking_cc_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.low_pass_filter_0_0, 0), (self.blocks_complex_to_mag_squared_0, 0))
        self.connect((self.osmosdr_source_0, 0), (self.satnogs_coarse_doppler_correction_cc_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.blocks_complex_to_real_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.satnogs_iq_sink_0, 0))
        self.connect((self.satnogs_coarse_doppler_correction_cc_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))

    def get_antenna(self):
        return self.antenna

    def set_antenna(self, antenna):
        self.antenna = antenna
        self.osmosdr_source_0.set_antenna(satnogs.handle_rx_antenna(self.rx_sdr_device, self.antenna), 0)

    def get_bb_gain(self):
        return self.bb_gain

    def set_bb_gain(self, bb_gain):
        self.bb_gain = bb_gain
        self.osmosdr_source_0.set_bb_gain(satnogs.handle_rx_bb_gain(self.rx_sdr_device, self.bb_gain), 0)

    def get_decoded_data_file_path(self):
        return self.decoded_data_file_path

    def set_decoded_data_file_path(self, decoded_data_file_path):
        self.decoded_data_file_path = decoded_data_file_path

    def get_dev_args(self):
        return self.dev_args

    def set_dev_args(self, dev_args):
        self.dev_args = dev_args

    def get_doppler_correction_per_sec(self):
        return self.doppler_correction_per_sec

    def set_doppler_correction_per_sec(self, doppler_correction_per_sec):
        self.doppler_correction_per_sec = doppler_correction_per_sec

    def get_enable_iq_dump(self):
        return self.enable_iq_dump

    def set_enable_iq_dump(self, enable_iq_dump):
        self.enable_iq_dump = enable_iq_dump

    def get_file_path(self):
        return self.file_path

    def set_file_path(self, file_path):
        self.file_path = file_path

    def get_if_gain(self):
        return self.if_gain

    def set_if_gain(self, if_gain):
        self.if_gain = if_gain
        self.osmosdr_source_0.set_if_gain(satnogs.handle_rx_if_gain(self.rx_sdr_device, self.if_gain), 0)

    def get_iq_file_path(self):
        return self.iq_file_path

    def set_iq_file_path(self, iq_file_path):
        self.iq_file_path = iq_file_path

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

    def get_rf_gain(self):
        return self.rf_gain

    def set_rf_gain(self, rf_gain):
        self.rf_gain = rf_gain
        self.osmosdr_source_0.set_gain(satnogs.handle_rx_rf_gain(self.rx_sdr_device, self.rf_gain), 0)

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
        self.osmosdr_source_0.set_gain(satnogs.handle_rx_rf_gain(self.rx_sdr_device, self.rf_gain), 0)
        self.osmosdr_source_0.set_if_gain(satnogs.handle_rx_if_gain(self.rx_sdr_device, self.if_gain), 0)
        self.osmosdr_source_0.set_bb_gain(satnogs.handle_rx_bb_gain(self.rx_sdr_device, self.bb_gain), 0)
        self.osmosdr_source_0.set_antenna(satnogs.handle_rx_antenna(self.rx_sdr_device, self.antenna), 0)

    def get_udp_IP(self):
        return self.udp_IP

    def set_udp_IP(self, udp_IP):
        self.udp_IP = udp_IP

    def get_udp_port(self):
        return self.udp_port

    def set_udp_port(self, udp_port):
        self.udp_port = udp_port

    def get_waterfall_file_path(self):
        return self.waterfall_file_path

    def set_waterfall_file_path(self, waterfall_file_path):
        self.waterfall_file_path = waterfall_file_path

    def get_wpm(self):
        return self.wpm

    def set_wpm(self, wpm):
        self.wpm = wpm

    def get_samp_rate_rx(self):
        return self.samp_rate_rx

    def set_samp_rate_rx(self, samp_rate_rx):
        self.samp_rate_rx = samp_rate_rx
        self.set_xlate_filter_taps(firdes.low_pass(1, self.samp_rate_rx, 125000, 25000, firdes.WIN_HAMMING, 6.76))
        self.osmosdr_source_0.set_sample_rate(self.samp_rate_rx)
        self.osmosdr_source_0.set_bandwidth(self.samp_rate_rx, 0)
        self.low_pass_filter_0_0.set_taps(firdes.low_pass(4, self.samp_rate_rx/self.xlating_decimation/self.lpf_decimation, 100, 100, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0.set_taps(firdes.low_pass(self.lpf_decimation, self.samp_rate_rx/self.xlating_decimation, 2e3, 1e3, firdes.WIN_HAMMING, 6.76))
        self.analog_pll_carriertracking_cc_0.set_max_freq(2*math.pi*2e3/(self.samp_rate_rx/self.xlating_decimation/self.lpf_decimation))
        self.analog_pll_carriertracking_cc_0.set_min_freq(-2*math.pi*2e3/(self.samp_rate_rx/self.xlating_decimation/self.lpf_decimation))

    def get_xlating_decimation(self):
        return self.xlating_decimation

    def set_xlating_decimation(self, xlating_decimation):
        self.xlating_decimation = xlating_decimation
        self.low_pass_filter_0_0.set_taps(firdes.low_pass(4, self.samp_rate_rx/self.xlating_decimation/self.lpf_decimation, 100, 100, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0.set_taps(firdes.low_pass(self.lpf_decimation, self.samp_rate_rx/self.xlating_decimation, 2e3, 1e3, firdes.WIN_HAMMING, 6.76))
        self.analog_pll_carriertracking_cc_0.set_max_freq(2*math.pi*2e3/(self.samp_rate_rx/self.xlating_decimation/self.lpf_decimation))
        self.analog_pll_carriertracking_cc_0.set_min_freq(-2*math.pi*2e3/(self.samp_rate_rx/self.xlating_decimation/self.lpf_decimation))

    def get_xlate_filter_taps(self):
        return self.xlate_filter_taps

    def set_xlate_filter_taps(self, xlate_filter_taps):
        self.xlate_filter_taps = xlate_filter_taps
        self.freq_xlating_fir_filter_xxx_0.set_taps((self.xlate_filter_taps))

    def get_lpf_decimation(self):
        return self.lpf_decimation

    def set_lpf_decimation(self, lpf_decimation):
        self.lpf_decimation = lpf_decimation
        self.low_pass_filter_0_0.set_taps(firdes.low_pass(4, self.samp_rate_rx/self.xlating_decimation/self.lpf_decimation, 100, 100, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0.set_taps(firdes.low_pass(self.lpf_decimation, self.samp_rate_rx/self.xlating_decimation, 2e3, 1e3, firdes.WIN_HAMMING, 6.76))
        self.analog_pll_carriertracking_cc_0.set_max_freq(2*math.pi*2e3/(self.samp_rate_rx/self.xlating_decimation/self.lpf_decimation))
        self.analog_pll_carriertracking_cc_0.set_min_freq(-2*math.pi*2e3/(self.samp_rate_rx/self.xlating_decimation/self.lpf_decimation))

    def get_audio_samp_rate(self):
        return self.audio_samp_rate

    def set_audio_samp_rate(self, audio_samp_rate):
        self.audio_samp_rate = audio_samp_rate


def argument_parser():
    description = 'A CW (Morse) Decoder'
    parser = OptionParser(usage="%prog: [options]", option_class=eng_option, description=description)
    parser.add_option(
        "", "--antenna", dest="antenna", type="string", default=satnogs.not_set_antenna,
        help="Set antenna [default=%default]")
    parser.add_option(
        "", "--bb-gain", dest="bb_gain", type="eng_float", default=eng_notation.num_to_str(satnogs.not_set_rx_bb_gain),
        help="Set bb_gain [default=%default]")
    parser.add_option(
        "", "--decoded-data-file-path", dest="decoded_data_file_path", type="string", default='/tmp/.satnogs/data/data',
        help="Set decoded_data_file_path [default=%default]")
    parser.add_option(
        "", "--dev-args", dest="dev_args", type="string", default=satnogs.not_set_dev_args,
        help="Set dev_args [default=%default]")
    parser.add_option(
        "", "--doppler-correction-per-sec", dest="doppler_correction_per_sec", type="intx", default=1000,
        help="Set doppler_correction_per_sec [default=%default]")
    parser.add_option(
        "", "--enable-iq-dump", dest="enable_iq_dump", type="intx", default=0,
        help="Set enable_iq_dump [default=%default]")
    parser.add_option(
        "", "--file-path", dest="file_path", type="string", default='test.txt',
        help="Set file_path [default=%default]")
    parser.add_option(
        "", "--if-gain", dest="if_gain", type="eng_float", default=eng_notation.num_to_str(satnogs.not_set_rx_if_gain),
        help="Set if_gain [default=%default]")
    parser.add_option(
        "", "--iq-file-path", dest="iq_file_path", type="string", default='/tmp/iq.dat',
        help="Set iq_file_path [default=%default]")
    parser.add_option(
        "", "--lo-offset", dest="lo_offset", type="eng_float", default=eng_notation.num_to_str(100e3),
        help="Set lo_offset [default=%default]")
    parser.add_option(
        "", "--ppm", dest="ppm", type="intx", default=0,
        help="Set ppm [default=%default]")
    parser.add_option(
        "", "--rf-gain", dest="rf_gain", type="eng_float", default=eng_notation.num_to_str(satnogs.not_set_rx_rf_gain),
        help="Set rf_gain [default=%default]")
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
        "", "--udp-IP", dest="udp_IP", type="string", default='127.0.0.1',
        help="Set udp_IP [default=%default]")
    parser.add_option(
        "", "--udp-port", dest="udp_port", type="intx", default=16887,
        help="Set udp_port [default=%default]")
    parser.add_option(
        "", "--waterfall-file-path", dest="waterfall_file_path", type="string", default='/tmp/waterfall.dat',
        help="Set waterfall_file_path [default=%default]")
    parser.add_option(
        "", "--wpm", dest="wpm", type="intx", default=20,
        help="Set wpm [default=%default]")
    return parser


def main(top_block_cls=satnogs_cw_decoder, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(antenna=options.antenna, bb_gain=options.bb_gain, decoded_data_file_path=options.decoded_data_file_path, dev_args=options.dev_args, doppler_correction_per_sec=options.doppler_correction_per_sec, enable_iq_dump=options.enable_iq_dump, file_path=options.file_path, if_gain=options.if_gain, iq_file_path=options.iq_file_path, lo_offset=options.lo_offset, ppm=options.ppm, rf_gain=options.rf_gain, rigctl_port=options.rigctl_port, rx_freq=options.rx_freq, rx_sdr_device=options.rx_sdr_device, udp_IP=options.udp_IP, udp_port=options.udp_port, waterfall_file_path=options.waterfall_file_path, wpm=options.wpm)
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
