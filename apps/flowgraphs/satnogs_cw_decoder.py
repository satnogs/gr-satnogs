#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: CW Decoder
# Author: Manolis Surligas (surligas@gmail.com)
# Description: A CW (Morse) Decoder
# Generated: Tue Apr 25 21:28:22 2017
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
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import osmosdr
import satnogs
import sip
import sys
import time
from gnuradio import qtgui


class satnogs_cw_decoder(gr.top_block, Qt.QWidget):

    def __init__(self, cw_offset=700, doppler_correction_per_sec=1000, file_path='test.txt', lo_offset=100e3, ppm=0, rigctl_port=4532, rx_freq=100e6, rx_sdr_device='usrpb200', waterfall_file_path='/tmp/waterfall.dat', wpm=20):
        gr.top_block.__init__(self, "CW Decoder")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("CW Decoder")
        qtgui.util.check_set_qss()
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

        self.settings = Qt.QSettings("GNU Radio", "satnogs_cw_decoder")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Parameters
        ##################################################
        self.cw_offset = cw_offset
        self.doppler_correction_per_sec = doppler_correction_per_sec
        self.file_path = file_path
        self.lo_offset = lo_offset
        self.ppm = ppm
        self.rigctl_port = rigctl_port
        self.rx_freq = rx_freq
        self.rx_sdr_device = rx_sdr_device
        self.waterfall_file_path = waterfall_file_path
        self.wpm = wpm

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_rx = samp_rate_rx = satnogs.hw_rx_settings[rx_sdr_device]['samp_rate']
        self.xlating_decimation = xlating_decimation = 5
        self.xlate_filter_taps = xlate_filter_taps = firdes.low_pass(1, samp_rate_rx, 125000, 25000, firdes.WIN_HAMMING, 6.76)

        self.taps = taps = firdes.low_pass(12.0, samp_rate_rx, 100e3, 60000, firdes.WIN_HAMMING, 6.76)

        self.ndelay = ndelay = 250
        self.lpf_decimation = lpf_decimation = 5

        ##################################################
        # Blocks
        ##################################################
        self.satnogs_tcp_rigctl_msg_source_0 = satnogs.tcp_rigctl_msg_source("127.0.0.1", rigctl_port, False, 1000, 1500)
        self.satnogs_multi_format_msg_sink_0 = satnogs.multi_format_msg_sink(0, True, True, 'test.txt')
        self.satnogs_morse_decoder_0 = satnogs.morse_decoder(ord('#'))
        self.satnogs_cw_to_symbol_0 = satnogs.cw_to_symbol(int((samp_rate_rx/xlating_decimation)/lpf_decimation), 300000, 0.9, wpm, False)
        self.satnogs_coarse_doppler_correction_cc_0 = satnogs.coarse_doppler_correction_cc(rx_freq, samp_rate_rx)
        self.qtgui_time_sink_x_0 = qtgui.time_sink_f(
        	1024, #size
        	int((samp_rate_rx/xlating_decimation)/lpf_decimation), #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0.enable_grid(False)
        self.qtgui_time_sink_x_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0.enable_control_panel(False)

        if not True:
          self.qtgui_time_sink_x_0.disable_legend()

        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]

        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_win)
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	0, #fc
        	samp_rate_rx/xlating_decimation, #bw
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(False)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)

        if not True:
          self.qtgui_freq_sink_x_0.disable_legend()

        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_freq_sink_x_0.set_plot_pos_half(not True)

        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_freq_sink_x_0_win)
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

        self.low_pass_filter_0 = filter.fir_filter_ccf(lpf_decimation, firdes.low_pass(
        	1, samp_rate_rx/xlating_decimation, 2e3, 500, firdes.WIN_HAMMING, 6.76))
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(xlating_decimation, (xlate_filter_taps), lo_offset - cw_offset, samp_rate_rx)
        self.fir_filter_xxx_0 = filter.fir_filter_ccc(1, ([1,] * ndelay))
        self.fir_filter_xxx_0.declare_sample_delay(0)
        self.blocks_multiply_conjugate_cc_0 = blocks.multiply_conjugate_cc(1)
        self.blocks_moving_average_xx_0 = blocks.moving_average_ff(ndelay, 1, 4000)
        self.blocks_delay_0 = blocks.delay(gr.sizeof_gr_complex*1, ndelay)
        self.blocks_complex_to_mag_squared_0 = blocks.complex_to_mag_squared(1)
        self.analog_agc2_xx_0 = analog.agc2_cc(0.1, 0.8, 0.6, 0.0)
        self.analog_agc2_xx_0.set_max_gain(1e3)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.satnogs_cw_to_symbol_0, 'out'), (self.satnogs_morse_decoder_0, 'in'))
        self.msg_connect((self.satnogs_morse_decoder_0, 'out'), (self.satnogs_multi_format_msg_sink_0, 'in'))
        self.msg_connect((self.satnogs_tcp_rigctl_msg_source_0, 'freq'), (self.satnogs_coarse_doppler_correction_cc_0, 'freq'))
        self.connect((self.analog_agc2_xx_0, 0), (self.blocks_delay_0, 0))
        self.connect((self.analog_agc2_xx_0, 0), (self.blocks_multiply_conjugate_cc_0, 0))
        self.connect((self.blocks_complex_to_mag_squared_0, 0), (self.blocks_moving_average_xx_0, 0))
        self.connect((self.blocks_delay_0, 0), (self.blocks_multiply_conjugate_cc_0, 1))
        self.connect((self.blocks_moving_average_xx_0, 0), (self.qtgui_time_sink_x_0, 0))
        self.connect((self.blocks_moving_average_xx_0, 0), (self.satnogs_cw_to_symbol_0, 0))
        self.connect((self.blocks_multiply_conjugate_cc_0, 0), (self.fir_filter_xxx_0, 0))
        self.connect((self.fir_filter_xxx_0, 0), (self.blocks_complex_to_mag_squared_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.low_pass_filter_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.analog_agc2_xx_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.qtgui_freq_sink_x_0, 0))
        self.connect((self.osmosdr_source_0, 0), (self.satnogs_coarse_doppler_correction_cc_0, 0))
        self.connect((self.satnogs_coarse_doppler_correction_cc_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "satnogs_cw_decoder")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_cw_offset(self):
        return self.cw_offset

    def set_cw_offset(self, cw_offset):
        self.cw_offset = cw_offset
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.lo_offset - self.cw_offset)

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
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.lo_offset - self.cw_offset)

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
        self.qtgui_time_sink_x_0.set_samp_rate(int((self.samp_rate_rx/self.xlating_decimation)/self.lpf_decimation))
        self.qtgui_freq_sink_x_0.set_frequency_range(0, self.samp_rate_rx/self.xlating_decimation)
        self.osmosdr_source_0.set_sample_rate(self.samp_rate_rx)
        self.osmosdr_source_0.set_bandwidth(self.samp_rate_rx, 0)
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate_rx/self.xlating_decimation, 2e3, 500, firdes.WIN_HAMMING, 6.76))

    def get_xlating_decimation(self):
        return self.xlating_decimation

    def set_xlating_decimation(self, xlating_decimation):
        self.xlating_decimation = xlating_decimation
        self.qtgui_time_sink_x_0.set_samp_rate(int((self.samp_rate_rx/self.xlating_decimation)/self.lpf_decimation))
        self.qtgui_freq_sink_x_0.set_frequency_range(0, self.samp_rate_rx/self.xlating_decimation)
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate_rx/self.xlating_decimation, 2e3, 500, firdes.WIN_HAMMING, 6.76))

    def get_xlate_filter_taps(self):
        return self.xlate_filter_taps

    def set_xlate_filter_taps(self, xlate_filter_taps):
        self.xlate_filter_taps = xlate_filter_taps
        self.freq_xlating_fir_filter_xxx_0.set_taps((self.xlate_filter_taps))

    def get_taps(self):
        return self.taps

    def set_taps(self, taps):
        self.taps = taps

    def get_ndelay(self):
        return self.ndelay

    def set_ndelay(self, ndelay):
        self.ndelay = ndelay
        self.fir_filter_xxx_0.set_taps(([1,] * self.ndelay))
        self.blocks_moving_average_xx_0.set_length_and_scale(self.ndelay, 1)
        self.blocks_delay_0.set_dly(self.ndelay)

    def get_lpf_decimation(self):
        return self.lpf_decimation

    def set_lpf_decimation(self, lpf_decimation):
        self.lpf_decimation = lpf_decimation
        self.qtgui_time_sink_x_0.set_samp_rate(int((self.samp_rate_rx/self.xlating_decimation)/self.lpf_decimation))


def argument_parser():
    description = 'A CW (Morse) Decoder'
    parser = OptionParser(usage="%prog: [options]", option_class=eng_option, description=description)
    parser.add_option(
        "", "--cw-offset", dest="cw_offset", type="eng_float", default=eng_notation.num_to_str(700),
        help="Set cw_offset [default=%default]")
    parser.add_option(
        "", "--doppler-correction-per-sec", dest="doppler_correction_per_sec", type="intx", default=1000,
        help="Set doppler_correction_per_sec [default=%default]")
    parser.add_option(
        "", "--file-path", dest="file_path", type="string", default='test.txt',
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
    parser.add_option(
        "", "--wpm", dest="wpm", type="intx", default=20,
        help="Set wpm [default=%default]")
    return parser


def main(top_block_cls=satnogs_cw_decoder, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(cw_offset=options.cw_offset, doppler_correction_per_sec=options.doppler_correction_per_sec, file_path=options.file_path, lo_offset=options.lo_offset, ppm=options.ppm, rigctl_port=options.rigctl_port, rx_freq=options.rx_freq, rx_sdr_device=options.rx_sdr_device, waterfall_file_path=options.waterfall_file_path, wpm=options.wpm)
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
