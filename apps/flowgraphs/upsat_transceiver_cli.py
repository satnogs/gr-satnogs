#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Upsat Transceiver Cli
# Author: Manolis Surligas (surligas@gmail.com)
# Description: SATNOGS transceiver for UPSAT satellite
# Generated: Sun Jul 24 21:44:09 2016
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from optparse import OptionParser
import math
import numpy
import osmosdr
import satnogs
import time


class upsat_transceiver_cli(gr.top_block):

    def __init__(self, bind_addr="0.0.0.0", dest_addr="127.0.0.1", lo_offset=100e3, recv_port=16886, rx_sdr_device="usrpb200", send_port=5022, tx_sdr_device="usrpb200"):
        gr.top_block.__init__(self, "Upsat Transceiver Cli")

        ##################################################
        # Parameters
        ##################################################
        self.bind_addr = bind_addr
        self.dest_addr = dest_addr
        self.lo_offset = lo_offset
        self.recv_port = recv_port
        self.rx_sdr_device = rx_sdr_device
        self.send_port = send_port
        self.tx_sdr_device = tx_sdr_device

        ##################################################
        # Variables
        ##################################################
        self.samples_per_symbol_tx = samples_per_symbol_tx = 4*8
        self.sq_wave = sq_wave = (1.0, ) * samples_per_symbol_tx
        self.samp_rate_rx = samp_rate_rx = satnogs.hw_rx_settings[rx_sdr_device]['samp_rate']
        self.gaussian_taps = gaussian_taps = filter.firdes.gaussian(1.0, samples_per_symbol_tx, 1.0, 4*samples_per_symbol_tx)
        self.deviation = deviation = 3.9973e3
        self.decimation_rx = decimation_rx = 20
        self.baud_rate = baud_rate = 9600
        self.tx_frequency = tx_frequency = 145.835e6
        
        self.taps = taps = firdes.low_pass(1.0, samp_rate_rx, 20000, 60000, firdes.WIN_HAMMING, 6.76)
          
        self.samp_rate_tx = samp_rate_tx = satnogs.hw_tx_settings[rx_sdr_device]['samp_rate']
        self.rx_frequency = rx_frequency = 435.765e6
        self.modulation_index = modulation_index = deviation / (baud_rate / 2.0)
        self.interp_taps = interp_taps = numpy.convolve(numpy.array(gaussian_taps), numpy.array(sq_wave))
        self.first_stage_samp_rate_rx = first_stage_samp_rate_rx = samp_rate_rx / decimation_rx

        ##################################################
        # Blocks
        ##################################################
        self.satnogs_upsat_fsk_frame_encoder_0 = satnogs.upsat_fsk_frame_encoder([0x33]*8 , [0x7A, 0x0E], False, False, False, True, True, "ABCD", 0, "ON02GR", 0, 64)
        self.satnogs_udp_msg_source_0 = satnogs.udp_msg_source(bind_addr, recv_port, 1500)
        self.satnogs_udp_msg_sink_0_0 = satnogs.udp_msg_sink(dest_addr, send_port, 1500)
        self.satnogs_multi_format_msg_sink_0_0 = satnogs.multi_format_msg_sink(1)
        self.satnogs_ax25_decoder_bm_0 = satnogs.ax25_decoder_bm('GND', 0, False, True, 256)
        self.pfb_arb_resampler_xxx_0 = pfb.arb_resampler_ccf(
        	  samp_rate_tx / (baud_rate * samples_per_symbol_tx),
                  taps=(firdes.low_pass_2(32, 32, 0.8, 0.1, 60)),
        	  flt_size=32)
        self.pfb_arb_resampler_xxx_0.declare_sample_delay(0)
        	
        self.osmosdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_source_0.set_sample_rate(samp_rate_rx)
        self.osmosdr_source_0.set_center_freq(rx_frequency - lo_offset, 0)
        self.osmosdr_source_0.set_freq_corr(0, 0)
        self.osmosdr_source_0.set_dc_offset_mode(0, 0)
        self.osmosdr_source_0.set_iq_balance_mode(0, 0)
        self.osmosdr_source_0.set_gain_mode(False, 0)
        self.osmosdr_source_0.set_gain(satnogs.hw_rx_settings[rx_sdr_device]['rf_gain'], 0)
        self.osmosdr_source_0.set_if_gain(satnogs.hw_rx_settings[rx_sdr_device]['if_gain'], 0)
        self.osmosdr_source_0.set_bb_gain(satnogs.hw_rx_settings[rx_sdr_device]['bb_gain'], 0)
        self.osmosdr_source_0.set_antenna(satnogs.hw_rx_settings[rx_sdr_device]['antenna'], 0)
        self.osmosdr_source_0.set_bandwidth(samp_rate_rx, 0)
          
        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_sink_0.set_sample_rate(samp_rate_tx)
        self.osmosdr_sink_0.set_center_freq(tx_frequency - lo_offset, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(satnogs.hw_tx_settings[tx_sdr_device]['rf_gain'], 0)
        self.osmosdr_sink_0.set_if_gain(satnogs.hw_tx_settings[tx_sdr_device]['if_gain'], 0)
        self.osmosdr_sink_0.set_bb_gain(satnogs.hw_tx_settings[tx_sdr_device]['bb_gain'], 0)
        self.osmosdr_sink_0.set_antenna(satnogs.hw_tx_settings[tx_sdr_device]['antenna'], 0)
        self.osmosdr_sink_0.set_bandwidth(samp_rate_tx, 0)
          
        self.interp_fir_filter_xxx_0 = filter.interp_fir_filter_fff(samples_per_symbol_tx, (interp_taps))
        self.interp_fir_filter_xxx_0.declare_sample_delay(0)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(decimation_rx, (taps), lo_offset, samp_rate_rx)
        self.digital_clock_recovery_mm_xx_0 = digital.clock_recovery_mm_ff(first_stage_samp_rate_rx/baud_rate, 0.25*0.175*0.175, 0.5, 0.175, 0.005)
        self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.analog_sig_source_x_0 = analog.sig_source_c(samp_rate_tx, analog.GR_COS_WAVE, lo_offset , 1, 0)
        self.analog_quadrature_demod_cf_0_0 = analog.quadrature_demod_cf(((first_stage_samp_rate_rx) / baud_rate)/(math.pi*modulation_index))
        self.analog_frequency_modulator_fc_0 = analog.frequency_modulator_fc((math.pi*modulation_index) / samples_per_symbol_tx)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.satnogs_ax25_decoder_bm_0, 'pdu'), (self.satnogs_multi_format_msg_sink_0_0, 'in'))    
        self.msg_connect((self.satnogs_ax25_decoder_bm_0, 'pdu'), (self.satnogs_udp_msg_sink_0_0, 'in'))    
        self.msg_connect((self.satnogs_udp_msg_source_0, 'msg'), (self.satnogs_upsat_fsk_frame_encoder_0, 'pdu'))    
        self.connect((self.analog_frequency_modulator_fc_0, 0), (self.pfb_arb_resampler_xxx_0, 0))    
        self.connect((self.analog_quadrature_demod_cf_0_0, 0), (self.digital_clock_recovery_mm_xx_0, 0))    
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0, 1))    
        self.connect((self.blocks_multiply_xx_0, 0), (self.osmosdr_sink_0, 0))    
        self.connect((self.digital_binary_slicer_fb_0, 0), (self.satnogs_ax25_decoder_bm_0, 0))    
        self.connect((self.digital_clock_recovery_mm_xx_0, 0), (self.digital_binary_slicer_fb_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.analog_quadrature_demod_cf_0_0, 0))    
        self.connect((self.interp_fir_filter_xxx_0, 0), (self.analog_frequency_modulator_fc_0, 0))    
        self.connect((self.osmosdr_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))    
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.blocks_multiply_xx_0, 0))    
        self.connect((self.satnogs_upsat_fsk_frame_encoder_0, 0), (self.interp_fir_filter_xxx_0, 0))    

    def get_bind_addr(self):
        return self.bind_addr

    def set_bind_addr(self, bind_addr):
        self.bind_addr = bind_addr

    def get_dest_addr(self):
        return self.dest_addr

    def set_dest_addr(self, dest_addr):
        self.dest_addr = dest_addr

    def get_lo_offset(self):
        return self.lo_offset

    def set_lo_offset(self, lo_offset):
        self.lo_offset = lo_offset
        self.analog_sig_source_x_0.set_frequency(self.lo_offset )
        self.osmosdr_sink_0.set_center_freq(self.tx_frequency - self.lo_offset, 0)
        self.osmosdr_source_0.set_center_freq(self.rx_frequency - self.lo_offset, 0)
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.lo_offset)

    def get_recv_port(self):
        return self.recv_port

    def set_recv_port(self, recv_port):
        self.recv_port = recv_port

    def get_rx_sdr_device(self):
        return self.rx_sdr_device

    def set_rx_sdr_device(self, rx_sdr_device):
        self.rx_sdr_device = rx_sdr_device
        self.set_samp_rate_rx(satnogs.hw_rx_settings[self.rx_sdr_device]['samp_rate'])
        self.set_samp_rate_tx(satnogs.hw_tx_settings[self.rx_sdr_device]['samp_rate'])
        self.osmosdr_source_0.set_gain(satnogs.hw_rx_settings[self.rx_sdr_device]['rf_gain'], 0)
        self.osmosdr_source_0.set_if_gain(satnogs.hw_rx_settings[self.rx_sdr_device]['if_gain'], 0)
        self.osmosdr_source_0.set_bb_gain(satnogs.hw_rx_settings[self.rx_sdr_device]['bb_gain'], 0)
        self.osmosdr_source_0.set_antenna(satnogs.hw_rx_settings[self.rx_sdr_device]['antenna'], 0)

    def get_send_port(self):
        return self.send_port

    def set_send_port(self, send_port):
        self.send_port = send_port

    def get_tx_sdr_device(self):
        return self.tx_sdr_device

    def set_tx_sdr_device(self, tx_sdr_device):
        self.tx_sdr_device = tx_sdr_device
        self.osmosdr_sink_0.set_gain(satnogs.hw_tx_settings[self.tx_sdr_device]['rf_gain'], 0)
        self.osmosdr_sink_0.set_if_gain(satnogs.hw_tx_settings[self.tx_sdr_device]['if_gain'], 0)
        self.osmosdr_sink_0.set_bb_gain(satnogs.hw_tx_settings[self.tx_sdr_device]['bb_gain'], 0)
        self.osmosdr_sink_0.set_antenna(satnogs.hw_tx_settings[self.tx_sdr_device]['antenna'], 0)

    def get_samples_per_symbol_tx(self):
        return self.samples_per_symbol_tx

    def set_samples_per_symbol_tx(self, samples_per_symbol_tx):
        self.samples_per_symbol_tx = samples_per_symbol_tx
        self.set_gaussian_taps(filter.firdes.gaussian(1.0, self.samples_per_symbol_tx, 1.0, 4*self.samples_per_symbol_tx))
        self.set_sq_wave((1.0, ) * self.samples_per_symbol_tx)
        self.analog_frequency_modulator_fc_0.set_sensitivity((math.pi*self.modulation_index) / self.samples_per_symbol_tx)
        self.pfb_arb_resampler_xxx_0.set_rate(self.samp_rate_tx / (self.baud_rate * self.samples_per_symbol_tx))

    def get_sq_wave(self):
        return self.sq_wave

    def set_sq_wave(self, sq_wave):
        self.sq_wave = sq_wave
        self.set_interp_taps(numpy.convolve(numpy.array(self.gaussian_taps), numpy.array(self.sq_wave)))

    def get_samp_rate_rx(self):
        return self.samp_rate_rx

    def set_samp_rate_rx(self, samp_rate_rx):
        self.samp_rate_rx = samp_rate_rx
        self.set_first_stage_samp_rate_rx(self.samp_rate_rx / self.decimation_rx)
        self.osmosdr_source_0.set_sample_rate(self.samp_rate_rx)
        self.osmosdr_source_0.set_bandwidth(self.samp_rate_rx, 0)

    def get_gaussian_taps(self):
        return self.gaussian_taps

    def set_gaussian_taps(self, gaussian_taps):
        self.gaussian_taps = gaussian_taps
        self.set_interp_taps(numpy.convolve(numpy.array(self.gaussian_taps), numpy.array(self.sq_wave)))

    def get_deviation(self):
        return self.deviation

    def set_deviation(self, deviation):
        self.deviation = deviation
        self.set_modulation_index(self.deviation / (self.baud_rate / 2.0))

    def get_decimation_rx(self):
        return self.decimation_rx

    def set_decimation_rx(self, decimation_rx):
        self.decimation_rx = decimation_rx
        self.set_first_stage_samp_rate_rx(self.samp_rate_rx / self.decimation_rx)

    def get_baud_rate(self):
        return self.baud_rate

    def set_baud_rate(self, baud_rate):
        self.baud_rate = baud_rate
        self.set_modulation_index(self.deviation / (self.baud_rate / 2.0))
        self.pfb_arb_resampler_xxx_0.set_rate(self.samp_rate_tx / (self.baud_rate * self.samples_per_symbol_tx))
        self.analog_quadrature_demod_cf_0_0.set_gain(((self.first_stage_samp_rate_rx) / self.baud_rate)/(math.pi*self.modulation_index))
        self.digital_clock_recovery_mm_xx_0.set_omega(self.first_stage_samp_rate_rx/self.baud_rate)

    def get_tx_frequency(self):
        return self.tx_frequency

    def set_tx_frequency(self, tx_frequency):
        self.tx_frequency = tx_frequency
        self.osmosdr_sink_0.set_center_freq(self.tx_frequency - self.lo_offset, 0)

    def get_taps(self):
        return self.taps

    def set_taps(self, taps):
        self.taps = taps
        self.freq_xlating_fir_filter_xxx_0.set_taps((self.taps))

    def get_samp_rate_tx(self):
        return self.samp_rate_tx

    def set_samp_rate_tx(self, samp_rate_tx):
        self.samp_rate_tx = samp_rate_tx
        self.analog_sig_source_x_0.set_sampling_freq(self.samp_rate_tx)
        self.pfb_arb_resampler_xxx_0.set_rate(self.samp_rate_tx / (self.baud_rate * self.samples_per_symbol_tx))
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate_tx)
        self.osmosdr_sink_0.set_bandwidth(self.samp_rate_tx, 0)

    def get_rx_frequency(self):
        return self.rx_frequency

    def set_rx_frequency(self, rx_frequency):
        self.rx_frequency = rx_frequency
        self.osmosdr_source_0.set_center_freq(self.rx_frequency - self.lo_offset, 0)

    def get_modulation_index(self):
        return self.modulation_index

    def set_modulation_index(self, modulation_index):
        self.modulation_index = modulation_index
        self.analog_frequency_modulator_fc_0.set_sensitivity((math.pi*self.modulation_index) / self.samples_per_symbol_tx)
        self.analog_quadrature_demod_cf_0_0.set_gain(((self.first_stage_samp_rate_rx) / self.baud_rate)/(math.pi*self.modulation_index))

    def get_interp_taps(self):
        return self.interp_taps

    def set_interp_taps(self, interp_taps):
        self.interp_taps = interp_taps
        self.interp_fir_filter_xxx_0.set_taps((self.interp_taps))

    def get_first_stage_samp_rate_rx(self):
        return self.first_stage_samp_rate_rx

    def set_first_stage_samp_rate_rx(self, first_stage_samp_rate_rx):
        self.first_stage_samp_rate_rx = first_stage_samp_rate_rx
        self.analog_quadrature_demod_cf_0_0.set_gain(((self.first_stage_samp_rate_rx) / self.baud_rate)/(math.pi*self.modulation_index))
        self.digital_clock_recovery_mm_xx_0.set_omega(self.first_stage_samp_rate_rx/self.baud_rate)


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--bind-addr", dest="bind_addr", type="string", default="0.0.0.0",
        help="Set bind_addr [default=%default]")
    parser.add_option(
        "", "--dest-addr", dest="dest_addr", type="string", default="127.0.0.1",
        help="Set dest_addr [default=%default]")
    parser.add_option(
        "", "--lo-offset", dest="lo_offset", type="eng_float", default=eng_notation.num_to_str(100e3),
        help="Set lo_offset [default=%default]")
    parser.add_option(
        "", "--recv-port", dest="recv_port", type="intx", default=16886,
        help="Set recv_port [default=%default]")
    parser.add_option(
        "", "--rx-sdr-device", dest="rx_sdr_device", type="string", default="usrpb200",
        help="Set rx_sdr_device [default=%default]")
    parser.add_option(
        "", "--send-port", dest="send_port", type="intx", default=5022,
        help="Set send_port [default=%default]")
    parser.add_option(
        "", "--tx-sdr-device", dest="tx_sdr_device", type="string", default="usrpb200",
        help="Set tx_sdr_device [default=%default]")
    return parser


def main(top_block_cls=upsat_transceiver_cli, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(bind_addr=options.bind_addr, dest_addr=options.dest_addr, lo_offset=options.lo_offset, recv_port=options.recv_port, rx_sdr_device=options.rx_sdr_device, send_port=options.send_port, tx_sdr_device=options.tx_sdr_device)
    tb.start()
    try:
        raw_input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
