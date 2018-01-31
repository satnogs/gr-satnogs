#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Debug Afsk Transceiver Osmocom
# Generated: Mon Jun 13 20:30:12 2016
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import analog
from gnuradio import audio
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import math
import numpy
import satnogs
import sip
import sys


class debug_afsk_transceiver_osmocom(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Debug Afsk Transceiver Osmocom")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Debug Afsk Transceiver Osmocom")
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "debug_afsk_transceiver_osmocom")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.samples_per_symbol_tx = samples_per_symbol_tx = 4
        self.sq_wave = sq_wave = (1.0, ) * samples_per_symbol_tx
        self.gaussian_taps = gaussian_taps = filter.firdes.gaussian(1.0, samples_per_symbol_tx, 1.0, 4*samples_per_symbol_tx)
        self.deviation = deviation = 800
        self.baud_rate = baud_rate = 1200
        self.tx_frequency = tx_frequency = 145.835e6
        self.samp_rate_tx = samp_rate_tx = 48e3
        self.modulation_index = modulation_index = deviation / (baud_rate / 2.0)
        self.interp_taps = interp_taps = numpy.convolve(numpy.array(gaussian_taps), numpy.array(sq_wave))
        self.atten = atten = 0.1

        ##################################################
        # Blocks
        ##################################################
        self._atten_range = Range(0, 0.9, 0.01, 0.1, 200)
        self._atten_win = RangeWidget(self._atten_range, self.set_atten, "Attenuation", "counter_slider", float)
        self.top_layout.addWidget(self._atten_win)
        self.satnogs_upsat_fsk_frame_encoder_0 = satnogs.upsat_fsk_frame_encoder([0x33]*8, [0x7A, 0x0E], False, False, False, True, True, "ABCD", 0, "UPSAT", 0, 1024)
        self.satnogs_udp_msg_source_0 = satnogs.udp_msg_source("127.0.0.1", 16886, 1500)
        self.satnogs_debug_msg_source_0 = satnogs.debug_msg_source("HELLO"*4, 1, True)
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=10,
                decimation=1,
                taps=None,
                fractional_bw=None,
        )
        self.qtgui_time_sink_x_0_0_0 = qtgui.time_sink_c(
        	1024, #size
        	samp_rate_tx, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0_0_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0_0_0.set_y_axis(-1, 1)
        
        self.qtgui_time_sink_x_0_0_0.set_y_label("Amplitude", "")
        
        self.qtgui_time_sink_x_0_0_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0_0_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0_0_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0_0_0.enable_grid(False)
        self.qtgui_time_sink_x_0_0_0.enable_control_panel(True)
        
        if not True:
          self.qtgui_time_sink_x_0_0_0.disable_legend()
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [2, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        
        for i in xrange(2*1):
            if len(labels[i]) == 0:
                if(i % 2 == 0):
                    self.qtgui_time_sink_x_0_0_0.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_0_0_0.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_0_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0_0_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_time_sink_x_0_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0_0_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_0_0_win)
        self.qtgui_time_sink_x_0_0 = qtgui.time_sink_f(
        	1024, #size
        	samp_rate_tx, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0_0.set_y_axis(-1, 1)
        
        self.qtgui_time_sink_x_0_0.set_y_label("Amplitude", "")
        
        self.qtgui_time_sink_x_0_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0_0.enable_grid(False)
        self.qtgui_time_sink_x_0_0.enable_control_panel(True)
        
        if not True:
          self.qtgui_time_sink_x_0_0.disable_legend()
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [2, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_time_sink_x_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_0_win)
        self.interp_fir_filter_xxx_0 = filter.interp_fir_filter_fff(samples_per_symbol_tx, (interp_taps))
        self.interp_fir_filter_xxx_0.declare_sample_delay(0)
        self.blocks_vco_f_0 = blocks.vco_f(48e3, -48e3, 1.0)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff((atten, ))
        self.audio_sink_0_0 = audio.sink(48000, "", True)
        self.analog_quadrature_demod_cf_0 = analog.quadrature_demod_cf(48e3/(2*math.pi*deviation/8.0))
        self.analog_frequency_modulator_fc_0 = analog.frequency_modulator_fc((math.pi*modulation_index) / samples_per_symbol_tx)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.satnogs_debug_msg_source_0, 'msg'), (self.satnogs_upsat_fsk_frame_encoder_0, 'pdu'))    
        self.msg_connect((self.satnogs_udp_msg_source_0, 'msg'), (self.satnogs_upsat_fsk_frame_encoder_0, 'pdu'))    
        self.connect((self.analog_frequency_modulator_fc_0, 0), (self.qtgui_time_sink_x_0_0_0, 0))    
        self.connect((self.analog_frequency_modulator_fc_0, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.analog_quadrature_demod_cf_0, 0), (self.blocks_multiply_const_vxx_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.audio_sink_0_0, 0))    
        self.connect((self.blocks_vco_f_0, 0), (self.qtgui_time_sink_x_0_0, 0))    
        self.connect((self.interp_fir_filter_xxx_0, 0), (self.analog_frequency_modulator_fc_0, 0))    
        self.connect((self.interp_fir_filter_xxx_0, 0), (self.blocks_vco_f_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.analog_quadrature_demod_cf_0, 0))    
        self.connect((self.satnogs_upsat_fsk_frame_encoder_0, 0), (self.interp_fir_filter_xxx_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "debug_afsk_transceiver_osmocom")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_samples_per_symbol_tx(self):
        return self.samples_per_symbol_tx

    def set_samples_per_symbol_tx(self, samples_per_symbol_tx):
        self.samples_per_symbol_tx = samples_per_symbol_tx
        self.set_gaussian_taps(filter.firdes.gaussian(1.0, self.samples_per_symbol_tx, 1.0, 4*self.samples_per_symbol_tx))
        self.set_sq_wave((1.0, ) * self.samples_per_symbol_tx)
        self.analog_frequency_modulator_fc_0.set_sensitivity((math.pi*self.modulation_index) / self.samples_per_symbol_tx)

    def get_sq_wave(self):
        return self.sq_wave

    def set_sq_wave(self, sq_wave):
        self.sq_wave = sq_wave
        self.set_interp_taps(numpy.convolve(numpy.array(self.gaussian_taps), numpy.array(self.sq_wave)))

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
        self.analog_quadrature_demod_cf_0.set_gain(48e3/(2*math.pi*self.deviation/8.0))

    def get_baud_rate(self):
        return self.baud_rate

    def set_baud_rate(self, baud_rate):
        self.baud_rate = baud_rate
        self.set_modulation_index(self.deviation / (self.baud_rate / 2.0))

    def get_tx_frequency(self):
        return self.tx_frequency

    def set_tx_frequency(self, tx_frequency):
        self.tx_frequency = tx_frequency

    def get_samp_rate_tx(self):
        return self.samp_rate_tx

    def set_samp_rate_tx(self, samp_rate_tx):
        self.samp_rate_tx = samp_rate_tx
        self.qtgui_time_sink_x_0_0_0.set_samp_rate(self.samp_rate_tx)
        self.qtgui_time_sink_x_0_0.set_samp_rate(self.samp_rate_tx)

    def get_modulation_index(self):
        return self.modulation_index

    def set_modulation_index(self, modulation_index):
        self.modulation_index = modulation_index
        self.analog_frequency_modulator_fc_0.set_sensitivity((math.pi*self.modulation_index) / self.samples_per_symbol_tx)

    def get_interp_taps(self):
        return self.interp_taps

    def set_interp_taps(self, interp_taps):
        self.interp_taps = interp_taps
        self.interp_fir_filter_xxx_0.set_taps((self.interp_taps))

    def get_atten(self):
        return self.atten

    def set_atten(self, atten):
        self.atten = atten
        self.blocks_multiply_const_vxx_0.set_k((self.atten, ))


def main(top_block_cls=debug_afsk_transceiver_osmocom, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
