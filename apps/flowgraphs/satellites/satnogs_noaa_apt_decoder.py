#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: NOAA APT Decoder
# Author: Manolis Surligas, George Vardakis
# Description: A NOAA APT Decoder with automatic image synchronization
# Generated: Thu Feb  8 14:09:41 2018
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

    def __init__(self, antenna=satnogs.not_set_antenna, bb_gain=satnogs.not_set_rx_bb_gain, decoded_data_file_path='/tmp/.satnogs/data/noaa', dev_args=satnogs.not_set_dev_args, doppler_correction_per_sec=20, enable_iq_dump=0, file_path='/tmp/test.ogg', flip_images=0, if_gain=satnogs.not_set_rx_if_gain, iq_file_path='/tmp/iq.dat', lo_offset=100e3, ppm=0, rf_gain=satnogs.not_set_rx_rf_gain, rigctl_port=4532, rx_freq=90.4e6, rx_sdr_device='usrpb200', sync=1, waterfall_file_path='/tmp/waterfall.dat'):
        gr.top_block.__init__(self, "NOAA APT Decoder")

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
        self.flip_images = flip_images
        self.if_gain = if_gain
        self.iq_file_path = iq_file_path
        self.lo_offset = lo_offset
        self.ppm = ppm
        self.rf_gain = rf_gain
        self.rigctl_port = rigctl_port
        self.rx_freq = rx_freq
        self.rx_sdr_device = rx_sdr_device
        self.sync = sync
        self.waterfall_file_path = waterfall_file_path

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_rx = samp_rate_rx = satnogs.hw_rx_settings[rx_sdr_device]['samp_rate']
        self.first_stage_decimation = first_stage_decimation = 4

        self.noaa_filter_taps = noaa_filter_taps = firdes.low_pass(1.0, samp_rate_rx /first_stage_decimation, 16.5e3, 4e3, firdes.WIN_HAMMING, 6.76)

        self.initial_bandwidth = initial_bandwidth = 100e3

        self.first_stage_filter_taps = first_stage_filter_taps = firdes.low_pass(1.0, 1.0, 0.2, 0.1, firdes.WIN_HAMMING, 6.76)

        self.audio_decimation = audio_decimation = 2

        ##################################################
        # Blocks
        ##################################################
        self.satnogs_waterfall_sink_0 = satnogs.waterfall_sink(samp_rate_rx /first_stage_decimation, 0.0, 8, 1024, waterfall_file_path, 0)
        self.satnogs_tcp_rigctl_msg_source_0 = satnogs.tcp_rigctl_msg_source("127.0.0.1", rigctl_port, False, 1000/doppler_correction_per_sec, 1500)
        self.satnogs_ogg_encoder_0 = satnogs.ogg_encoder(file_path, 48000, 0.8)
        self.satnogs_noaa_apt_sink_0 = satnogs.noaa_apt_sink(decoded_data_file_path, 2080, 1800, False, bool(sync), bool(flip_images))
        self.satnogs_iq_sink_0 = satnogs.iq_sink(32767, iq_file_path, False, enable_iq_dump)
        self.satnogs_coarse_doppler_correction_cc_0 = satnogs.coarse_doppler_correction_cc(rx_freq, samp_rate_rx /first_stage_decimation)
        self.rational_resampler_xxx_2 = filter.rational_resampler_ccc(
                interpolation=48000,
                decimation=int(samp_rate_rx/ ( first_stage_decimation  * int(samp_rate_rx/ first_stage_decimation / initial_bandwidth)) / audio_decimation),
                taps=None,
                fractional_bw=None,
        )
        self.rational_resampler_xxx_1 = filter.rational_resampler_fff(
                interpolation=48000,
                decimation=int(samp_rate_rx/ ( first_stage_decimation  * int(samp_rate_rx/ first_stage_decimation / initial_bandwidth)) / audio_decimation),
                taps=None,
                fractional_bw=None,
        )
        self.rational_resampler_xxx_0_0 = filter.rational_resampler_fff(
                interpolation=1,
                decimation=4,
                taps=None,
                fractional_bw=None,
        )
        self.rational_resampler_xxx_0 = filter.rational_resampler_fff(
                interpolation=4*4160,
                decimation=int((samp_rate_rx/ ( first_stage_decimation  * int(samp_rate_rx/ first_stage_decimation / initial_bandwidth)) / audio_decimation)/2),
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

        self.hilbert_fc_0 = filter.hilbert_fc(65, firdes.WIN_HAMMING, 6.76)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(first_stage_decimation, (first_stage_filter_taps), lo_offset, samp_rate_rx)
        self.fir_filter_xxx_1 = filter.fir_filter_fff(2, ([0.5, 0.5]))
        self.fir_filter_xxx_1.declare_sample_delay(0)
        self.fft_filter_xxx_0 = filter.fft_filter_ccc(int(samp_rate_rx/ first_stage_decimation / initial_bandwidth), (noaa_filter_taps), 1)
        self.fft_filter_xxx_0.declare_sample_delay(0)
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
        self.connect((self.analog_wfm_rcv_0, 0), (self.rational_resampler_xxx_1, 0))
        self.connect((self.band_pass_filter_0, 0), (self.fir_filter_xxx_1, 0))
        self.connect((self.blocks_complex_to_mag_0, 0), (self.rational_resampler_xxx_0_0, 0))
        self.connect((self.fft_filter_xxx_0, 0), (self.analog_wfm_rcv_0, 0))
        self.connect((self.fft_filter_xxx_0, 0), (self.rational_resampler_xxx_2, 0))
        self.connect((self.fir_filter_xxx_1, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.satnogs_coarse_doppler_correction_cc_0, 0))
        self.connect((self.hilbert_fc_0, 0), (self.blocks_complex_to_mag_0, 0))
        self.connect((self.osmosdr_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.hilbert_fc_0, 0))
        self.connect((self.rational_resampler_xxx_0_0, 0), (self.satnogs_noaa_apt_sink_0, 0))
        self.connect((self.rational_resampler_xxx_1, 0), (self.satnogs_ogg_encoder_0, 0))
        self.connect((self.rational_resampler_xxx_2, 0), (self.satnogs_iq_sink_0, 0))
        self.connect((self.satnogs_coarse_doppler_correction_cc_0, 0), (self.fft_filter_xxx_0, 0))
        self.connect((self.satnogs_coarse_doppler_correction_cc_0, 0), (self.satnogs_waterfall_sink_0, 0))

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

    def get_flip_images(self):
        return self.flip_images

    def set_flip_images(self, flip_images):
        self.flip_images = flip_images

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

    def get_sync(self):
        return self.sync

    def set_sync(self, sync):
        self.sync = sync

    def get_waterfall_file_path(self):
        return self.waterfall_file_path

    def set_waterfall_file_path(self, waterfall_file_path):
        self.waterfall_file_path = waterfall_file_path

    def get_samp_rate_rx(self):
        return self.samp_rate_rx

    def set_samp_rate_rx(self, samp_rate_rx):
        self.samp_rate_rx = samp_rate_rx
        self.osmosdr_source_0.set_sample_rate(self.samp_rate_rx)
        self.osmosdr_source_0.set_bandwidth(self.samp_rate_rx, 0)
        self.band_pass_filter_0.set_taps(firdes.band_pass(6, self.samp_rate_rx/ ( self.first_stage_decimation  * int(self.samp_rate_rx/ self.first_stage_decimation / self.initial_bandwidth)) / self.audio_decimation, 500, 4.2e3, 200, firdes.WIN_HAMMING, 6.76))

    def get_first_stage_decimation(self):
        return self.first_stage_decimation

    def set_first_stage_decimation(self, first_stage_decimation):
        self.first_stage_decimation = first_stage_decimation
        self.band_pass_filter_0.set_taps(firdes.band_pass(6, self.samp_rate_rx/ ( self.first_stage_decimation  * int(self.samp_rate_rx/ self.first_stage_decimation / self.initial_bandwidth)) / self.audio_decimation, 500, 4.2e3, 200, firdes.WIN_HAMMING, 6.76))

    def get_noaa_filter_taps(self):
        return self.noaa_filter_taps

    def set_noaa_filter_taps(self, noaa_filter_taps):
        self.noaa_filter_taps = noaa_filter_taps
        self.fft_filter_xxx_0.set_taps((self.noaa_filter_taps))

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

    def get_audio_decimation(self):
        return self.audio_decimation

    def set_audio_decimation(self, audio_decimation):
        self.audio_decimation = audio_decimation
        self.band_pass_filter_0.set_taps(firdes.band_pass(6, self.samp_rate_rx/ ( self.first_stage_decimation  * int(self.samp_rate_rx/ self.first_stage_decimation / self.initial_bandwidth)) / self.audio_decimation, 500, 4.2e3, 200, firdes.WIN_HAMMING, 6.76))


def argument_parser():
    description = 'A NOAA APT Decoder with automatic image synchronization'
    parser = OptionParser(usage="%prog: [options]", option_class=eng_option, description=description)
    parser.add_option(
        "", "--antenna", dest="antenna", type="string", default=satnogs.not_set_antenna,
        help="Set antenna [default=%default]")
    parser.add_option(
        "", "--bb-gain", dest="bb_gain", type="eng_float", default=eng_notation.num_to_str(satnogs.not_set_rx_bb_gain),
        help="Set bb_gain [default=%default]")
    parser.add_option(
        "", "--decoded-data-file-path", dest="decoded_data_file_path", type="string", default='/tmp/.satnogs/data/noaa',
        help="Set decoded_data_file_path [default=%default]")
    parser.add_option(
        "", "--dev-args", dest="dev_args", type="string", default=satnogs.not_set_dev_args,
        help="Set dev_args [default=%default]")
    parser.add_option(
        "", "--doppler-correction-per-sec", dest="doppler_correction_per_sec", type="intx", default=20,
        help="Set doppler_correction_per_sec [default=%default]")
    parser.add_option(
        "", "--enable-iq-dump", dest="enable_iq_dump", type="intx", default=0,
        help="Set enable_iq_dump [default=%default]")
    parser.add_option(
        "", "--file-path", dest="file_path", type="string", default='/tmp/test.ogg',
        help="Set file_path [default=%default]")
    parser.add_option(
        "", "--flip-images", dest="flip_images", type="intx", default=0,
        help="Set flip_images [default=%default]")
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
        "", "--rx-freq", dest="rx_freq", type="eng_float", default=eng_notation.num_to_str(90.4e6),
        help="Set rx_freq [default=%default]")
    parser.add_option(
        "", "--rx-sdr-device", dest="rx_sdr_device", type="string", default='usrpb200',
        help="Set rx_sdr_device [default=%default]")
    parser.add_option(
        "", "--sync", dest="sync", type="intx", default=1,
        help="Set sync [default=%default]")
    parser.add_option(
        "", "--waterfall-file-path", dest="waterfall_file_path", type="string", default='/tmp/waterfall.dat',
        help="Set waterfall_file_path [default=%default]")
    return parser


def main(top_block_cls=satnogs_noaa_apt_decoder, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(antenna=options.antenna, bb_gain=options.bb_gain, decoded_data_file_path=options.decoded_data_file_path, dev_args=options.dev_args, doppler_correction_per_sec=options.doppler_correction_per_sec, enable_iq_dump=options.enable_iq_dump, file_path=options.file_path, flip_images=options.flip_images, if_gain=options.if_gain, iq_file_path=options.iq_file_path, lo_offset=options.lo_offset, ppm=options.ppm, rf_gain=options.rf_gain, rigctl_port=options.rigctl_port, rx_freq=options.rx_freq, rx_sdr_device=options.rx_sdr_device, sync=options.sync, waterfall_file_path=options.waterfall_file_path)
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
