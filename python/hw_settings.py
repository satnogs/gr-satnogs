#! /usr/bin/python
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

#===============================================================================
# Bellow we provide some good and working settings for a large variety of
# different SDR hardware. Feel free, to provide additional hardware
# configurations
#===============================================================================
hw_tx_settings = {'usrpb200' : {'rf_gain' : 60.0, 'if_gain' : 0.0,
                                'bb_gain' : 0.0, 'samp_rate' : 2e6,
                                'antenna' : 'TX/RX', 'dev_arg': 'uhd'},
                  'usrp2' : {'rf_gain' : 20.0, 'samp_rate' : 2e6,
                             'bb_gain' : 0.0, 'samp_rate' : 2e6,
                             'antenna' : 'TX/RX', 'dev_arg': 'uhd'},
                  'hackrf' : {'rf_gain' : 0.0, 'if_gain' : 16.0,
                              'bb_gain' : 20.0, 'samp_rate' : 2e6,
                              'antenna' : '', 'dev_arg': 'hackrf'} }

hw_rx_settings = {'usrpb200' : {'rf_gain' : 50.0, 'if_gain' : 0.0,
                                'bb_gain' : 0.0, 'samp_rate' : 5e5,
                                'antenna' : 'RX2', 'dev_arg': 'uhd'},
                  'usrp2' : {'rf_gain' : 50.0, 'if_gain' : 0.0,
                             'bb_gain' : 0.0, 'samp_rate' : 5e5,
                             'antenna' : 'RX2', 'dev_arg': 'uhd'},
                  'airspy' : {'rf_gain' : 30.0, 'if_gain' : 0.0,
                              'bb_gain' : 0.0, 'samp_rate' : 10e6,
                              'antenna' : '', 'dev_arg': 'airspy'},
                  'hackrf' : {'rf_gain' : 14.0, 'if_gain' : 16.0,
                              'bb_gain' : 20.0, 'samp_rate' : 8e6,
                              'antenna' : '', 'dev_arg': 'hackrf'},
                  'rtlsdr' : {'rf_gain' : 32.8, 'if_gain' : 0.0,
                              'bb_gain' : 0.0, 'samp_rate' : 1e6,
                              'antenna' : '', 'dev_arg' : 'rtl'} }
