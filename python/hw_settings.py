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
# *  along with this program.  If not, see <http://www.gnu.org/licenses/>
#

#===============================================================================
# Bellow we provide some good and working settings for a large variety of
# different SDR hardware. Feel free, to provide additional hardware
# configurations
#===============================================================================
hw_tx_settings = {'usrpb200' : {'rf_gain' : 60.0, 'samp_rate' : 2e6},
                  'usrp2' : {'rf_gain' : 20.0, 'samp_rate' : 2e6},
                  'hackrf' : {'rf_gain' : 20.0, 'if_gain' : 8.0,
                              'bb_gain' : 5.0, 'samp_rate' : 2e6} }

hw_rx_settings = {'usrpb200' : {'rf_gain' : 20.0, 'if_gain' : 0.0,
                                'bb_gain' : 0.0, 'samp_rate' : 2e6},
                  'usrp2' : {'rf_gain' : 20.0, 'if_gain' : 0.0,
                             'bb_gain' : 0.0, 'samp_rate' : 2e6},
                  'airspy' : {'rf_gain' : 20.0, 'if_gain' : 0.0,
                              'bb_gain' : 0.0, 'samp_rate' : 2.5e6},
                  'hackrf' : {'rf_gain' : 20.0, 'if_gain' : 8.0,
                              'bb_gain' : 5.0, 'samp_rate' : 2e6} }
