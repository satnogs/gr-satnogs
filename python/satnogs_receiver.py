
from gnuradio import gr
from gnuradio import analog
from gnuradio import digital
from gnuradio import filter
from gnuradio.filter import firdes
import math
import satnogs
from gtk.keysyms import mu
from gnuradio.gr.runtime_swig import sizeof_gr_complex

class satnogs_receiver(gr.hier_block2):
    """
    docstring for block satnogs_receiver
    """
    def __init__(self,
                 decimation_rx,
                 taps,
                 lo_offset,
                 samp_rate_rx,
                 baud_rate,
                 deviation,
                 frame_preamble,
                 sync_word,
                 use_whitening,
                 use_manchester_coding,
                 use_crc,
                 use_ax25):
        gr.hier_block2.__init__(self,
            "satnogs_receiver",
            gr.io_signature(1 , 1 , sizeof_gr_complex),
            gr.io_signature(0, 0, 0))
        
        self.decimation_rx = decimation_rx
        self.taps = taps
        self.lo_offset = lo_offset
        self.samp_rate_rx = samp_rate_rx
        self.samp_rate =  self.samp_rate_rx/self.decimation_rx
        self.deviation = deviation
        self.baud_rate = baud_rate
        self.omega = self.samp_rate/self.baud_rate
        self.gain_omega = 0.25*0.175*0.175
        self.mu = 0.5
        self.gain_mu = 0.175
        self.omega_relative_lim = 0.005
        self.modulation_index = self.deviation / (self.baud_rate / 2.0)
        self.frame_preamble = frame_preamble
        self.sync_word = sync_word
        self.use_whitening = use_whitening
        self.use_manchester_coding = use_manchester_coding
        self.use_crc = use_crc
        self.use_ax25 = use_ax25
        
        self.message_port_register_hier_out("hier_out")
        
        ##
        # Block definitions
        ##
        self.freq_xlating_fir_filter = filter.freq_xlating_fir_filter_ccc(self.decimation_rx,
                                                                          (self.taps),
                                                                           self.lo_offset, 
                                                                           self.samp_rate_rx)
        
        self.analog_quadrature_demod_cf = analog.quadrature_demod_cf(self.omega/(math.pi*self.modulation_index))
        
        self.digital_clock_recovery_mm_xx = digital.clock_recovery_mm_ff(self.omega, 
                                                                         self.gain_omega, 
                                                                         self.mu,
                                                                         self.gain_mu,
                                                                         self.omega_relative_lim)
        
        self.satnogs_upsat_fsk_frame_acquisition = satnogs.upsat_fsk_frame_acquisition(self.frame_preamble , 
                                                                                       self.sync_word, 
                                                                                       self.use_whitening, 
                                                                                       self.use_manchester_coding, 
                                                                                       self.use_crc, 
                                                                                       self.use_ax25)
        
        ##
        # Block connections
        ## 
        self.connect((self, 0), (self.freq_xlating_fir_filter, 0))
        self.connect((self.freq_xlating_fir_filter, 0), (self.analog_quadrature_demod_cf, 0)) 
        self.connect((self.analog_quadrature_demod_cf, 0), (self.digital_clock_recovery_mm_xx, 0)) 
        self.connect((self.digital_clock_recovery_mm_xx, 0), (self.satnogs_upsat_fsk_frame_acquisition, 0))
        self.msg_connect((self.satnogs_upsat_fsk_frame_acquisition, "pdu"), (self, "hier_out"))
        
        