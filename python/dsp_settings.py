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
# This file provides a variaty of settings for the flowgraphs
# based on the hardware SDR device that they are using 
#===============================================================================

fm_demod_settings = {'usrpb200' : {'decimation_rx' : 5, 'audio_gain' : 0.9},
                  'usrp2' : {'decimation_rx' : 5, 'audio_gain' : 0.9},
                  'airspy' : {'decimation_rx' : 10, 'audio_gain' : 0.9},
                  'airspymini' : {'decimation_rx' : 5, 'audio_gain' : 0.9},
                  'airspyhf' : {'decimation_rx' : 5, 'audio_gain' : 0.9},
                  'hackrf' : {'decimation_rx' : 5, 'audio_gain' : 0.9},
                  'rtlsdr' : {'decimation_rx' : 5, 'audio_gain' : 0.9}
                   }