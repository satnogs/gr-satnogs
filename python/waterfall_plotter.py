#!/usr/bin/env python
#
# gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
#
#  Copyright (C) 2016, Libre Space Foundation <http://librespacefoundation.org/>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#

###############################################################################
# Block derived from gnuradio/gr-fft/python/logpwrfft.py
###############################################################################
from gnuradio import gr
from gnuradio import blocks
import sys, math

try:
    from gnuradio import fft
except ImportError:
    sys.stderr.write('satnogs waterfall plotter required gr-fft.\n')
    sys.exit(1)
try:
    from gnuradio import filter
except ImportError:
    sys.stderr.write('satnogs waterfall plotter required gr-filter.\n')
    sys.exit(1)

class waterfall_plotter(gr.hier_block2):
    """
    A variation of the Log Power FFT block that ships with GNU Radio, 
    supporting FFT Shift
    """
    
    def __init__(self, sample_rate, fft_size, fft_shift, ref_scale,
                frame_rate, average, avg_alpha, win=None):
        """
        Create an log10(abs(fft)) stream chain.
        Provide access to the setting the filter and sample rate.

        Args:
            sample_rate: Incoming stream sample rate
            fft_size: Number of FFT bins
            fft_shift: If set to true FFT shift is performed
            ref_scale: Sets 0 dB value input amplitude
            frame_rate: Output frame rate
            avg_alpha: FFT averaging (over time) constant [0.0-1.0]
            avg: Whether to average [True, False]
            win: the window taps generation function
        """
        gr.hier_block2.__init__(self, self._name,
                                gr.io_signature(1, 1, self._item_size),          # Input signature
                                gr.io_signature(1, 1, gr.sizeof_float*fft_size)) # Output signature

        self._sd = blocks.stream_to_vector_decimator(item_size=self._item_size,
                                                     sample_rate=sample_rate,
                                                     vec_rate=frame_rate,
                                                     vec_len=fft_size)

        if win is None: win = fft.window_blackman_harris
        fft_window = win(fft_size)
        fft_block = self._fft_block[0](fft_size, True, fft_window, fft_shift)
        window_power = sum(map(lambda x: x*x, fft_window))

        c2magsq = blocks.complex_to_mag_squared(fft_size)
        self._avg = filter.single_pole_iir_filter_ff(1.0, fft_size)
        self._log = blocks.nlog10_ff(10, fft_size, 
                                     -20*math.log10(fft_size)              # Adjust for number of bins
                                     -10*math.log10(window_power/fft_size) # Adjust for windowing loss
                                     -20*math.log10(ref_scale/2))      # Adjust for reference scale
        self.connect(self, self._sd, fft_block, c2magsq, self._avg, self._log, self)

        self._average = average
        self._avg_alpha = avg_alpha
        self.set_avg_alpha(avg_alpha)
        self.set_average(average)

    def set_decimation(self, decim):
        """
        Set the decimation on stream decimator.

        Args:
            decim: the new decimation
        """
        self._sd.set_decimation(decim)

    def set_vec_rate(self, vec_rate):
        """
        Set the vector rate on stream decimator.

        Args:
            vec_rate: the new vector rate
        """
        self._sd.set_vec_rate(vec_rate)

    def set_sample_rate(self, sample_rate):
        """
        Set the new sampling rate

        Args:
            sample_rate: the new rate
        """
        self._sd.set_sample_rate(sample_rate)

    def set_average(self, average):
        """
        Set the averaging filter on/off.

        Args:
            average: true to set averaging on
        """
        self._average = average
        if self._average:
            self._avg.set_taps(self._avg_alpha)
        else:
            self._avg.set_taps(1.0)

    def set_avg_alpha(self, avg_alpha):
        """
        Set the average alpha and set the taps if average was on.

        Args:
            avg_alpha: the new iir filter tap
        """
        self._avg_alpha = avg_alpha
        self.set_average(self._average)

    def sample_rate(self):
        """
        Return the current sample rate.
        """
        return self._sd.sample_rate()

    def decimation(self):
        """
        Return the current decimation.
        """
        return self._sd.decimation()

    def frame_rate(self):
        """
        Return the current frame rate.
        """
        return self._sd.frame_rate()

    def average(self):
        """
        Return whether or not averaging is being performed.
        """
        return self._average

    def avg_alpha(self):
        """
        Return averaging filter constant.
        """
        return self._avg_alpha

class waterfall_plotter_f(waterfall_plotter):
        """
        Create an fft block chain, with real input.
        """
        _name = "waterfall_plotter_f"
        _item_size = gr.sizeof_float
        _fft_block = (fft.fft_vfc, )

class waterfall_plotter_c(waterfall_plotter):
        """
        Create an fft block chain, with complex input.
        """
        _name = "waterfall_plotter_c"
        _item_size = gr.sizeof_gr_complex
        _fft_block = (fft.fft_vcc, )
