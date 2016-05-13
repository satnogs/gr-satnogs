#! /usr/bin/python

import satnogs
import satnogs_swig
import math
from gnuradio import blocks
from gnuradio import filter
from gnuradio import analog
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.gr.runtime_swig import sizeof_gr_complex


class satnogs_upsat_transmitter(gr.hier_block2):
    
    def __init__(self,
                 frame_preamble,
                 sync_word,
                 append_crc,
                 whitening,
                 manchester,
                 msb_first,
                 ax25_format,
                 dest_addr,
                 dest_ssid,
                 src_addr,
                 src_ssid,
                 settling_samples,
                 samps_per_symbol,
                 interpolation_taps,
                 samp_rate,
                 lo_offset,
                 deviation,
                 baud_rate):
        gr.hier_block2.__init__(self, "satnogs_upsat_transmitter",
                                gr.io_signature(0 , 0 , 0),
                                # Output 0: The complex TX signal for the SDR device
                                # Output 1: The constellation output for the vector analyzer
                                gr.io_signature(1, 1, sizeof_gr_complex)) 
        self.frame_preamble = frame_preamble
        self.sync_word = sync_word
        self.append_crc = append_crc
        self.whitening = whitening
        self.manchester = manchester
        self.msb_first = msb_first
        self.ax25_format = ax25_format
        self.dest_addr = dest_addr
        self.dest_ssid = dest_ssid
        self.src_addr = src_addr
        self.src_ssid = src_ssid
        self.settling_samples = settling_samples
        self.samps_per_symbol = samps_per_symbol
        self.interpolation_taps = interpolation_taps
        self.samp_rate = samp_rate
        self.lo_offset = lo_offset   
        self.deviation=deviation
        self.baud_rate=baud_rate
        self.message_port_register_hier_out("in")
        
        
        self.modulation_index = self.deviation/(self.baud_rate / 2.0)
        self.sensitivity = (math.pi*self.modulation_index) / self.samps_per_symbol
        self.resampling_rate = self.samp_rate / (self.baud_rate*self.samps_per_symbol )
        self.par_taps = filter.firdes.low_pass_2(32, 32, 0.8, 0.1, 60)
        self.num_filters = 32
 
#=================================================================
# TX Related blocks
#=================================================================

        self.fsk_frame_encoder = satnogs_swig.upsat_fsk_frame_encoder(self.frame_preamble,
                                                                  self.sync_word,
                                                                  self.append_crc,
                                                                  self.whitening,
                                                                  self.manchester,
                                                                  self.msb_first,
                                                                  self.ax25_format,
                                                                  self.dest_addr,
                                                                  self.dest_ssid,
                                                                  self.src_addr,
                                                                  self.src_ssid,
                                                                  self.settling_samples
                                                                  )
    
        self.interp_fir_filter = filter.interp_fir_filter_fff(self.samps_per_symbol,
                                                                 self.interpolation_taps
                                                                 )
    
        self.frequency_modulator = analog.frequency_modulator_fc(self.sensitivity)
        self.signal_source = analog.sig_source_c(self.samp_rate,
                                             102,
                                             self.lo_offset,
                                             1,
                                             0)
        self.polyphase_arbitrary_resampler = filter.pfb_arb_resampler_ccf(self.resampling_rate,
                                                                      self.par_taps,
                                                                      self.num_filters)
        self.multiply = blocks.multiply_cc(1)
    
#=================================================================
# Connections
#=================================================================  
        self.msg_connect((self, "in"), (self.fsk_frame_encoder, "pdu"))
        self.connect((self.fsk_frame_encoder, 0), (self.interp_fir_filter, 0))
        self.connect((self.interp_fir_filter, 0), (self.frequency_modulator, 0))
        self.connect((self.frequency_modulator, 0), (self.polyphase_arbitrary_resampler, 0))
        self.connect((self.signal_source, 0) , (self.multiply, 0))
        self.connect((self.polyphase_arbitrary_resampler, 0) , (self.multiply, 1))
        self.connect((self.multiply, 0), self)
    
  
