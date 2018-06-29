#! /usr/bin/python
#
# gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
#
#  Copyright (C) 2016,2017
#  Libre Space Foundation <http://librespacefoundation.org/>
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
                                'antenna' : 'TX/RX', 'dev_arg': 'uhd'},
                  'usrp2' : {'rf_gain' : 50.0, 'if_gain' : 0.0,
                             'bb_gain' : 0.0, 'samp_rate' : 5e5,
                             'antenna' : 'TX/RX', 'dev_arg': 'uhd'},
                  'airspy' : {'rf_gain' : 30.0, 'if_gain' : 0.0,
                              'bb_gain' : 0.0, 'samp_rate' : 10e6,
                              'antenna' : '', 'dev_arg': 'airspy'},
                  'airspymini' : {'rf_gain' : 15.0, 'if_gain' : 0.0,
                              'bb_gain' : 0.0, 'samp_rate' : 3e6,
                              'antenna' : '', 'dev_arg': 'airspy'},
                  'airspyhf' : {'rf_gain' : 15.0, 'if_gain' : 0.0,
                              'bb_gain' : 0.0, 'samp_rate' : 768e3,
                              'antenna' : '', 'dev_arg': 'airspyhf'},
                  'hackrf' : {'rf_gain' : 14.0, 'if_gain' : 16.0,
                              'bb_gain' : 20.0, 'samp_rate' : 8e6,
                              'antenna' : '', 'dev_arg': 'hackrf'},
                  'rtlsdr' : {'rf_gain' : 32.8, 'if_gain' : 0.0,
                              'bb_gain' : 0.0, 'samp_rate' : 1e6,
                              'antenna' : '', 'dev_arg' : 'rtl,buffers=32,buflen=16384' }}

#===============================================================================
# Default values indicating that the user does not passed any custom value
#===============================================================================
not_set_rx_rf_gain = -1.0
not_set_rx_if_gain = -1.0
not_set_rx_bb_gain = -1.0
not_set_antenna = ''
not_set_dev_args = ''

def handle_rx_dev_args(device, dev_args):
    """
    Handles the RX device arguments.
    :param device: the device name 
    :type device: string
    :param dev_args: the device arguments. Emtpy string for no or default 
    device arguements
    :type dev_args: string
    """
    if(len(dev_args) == 0):
        return hw_rx_settings[device]['dev_arg']
    else:
        return dev_args

def handle_rx_rf_gain(device, gain):
    """
    Handles the RX RF gain. If this parameter is not set from the user
    the default settings are returned, otherwise the user setting itself
    :param device: the device name
    :type device: string
    :param gain: the user defined gain value
    :type gain: float
    """
    if(gain == not_set_rx_rf_gain):
        return hw_rx_settings[device]['rf_gain']
    else:
        return gain
    
def handle_rx_if_gain(device, gain):
    """
    Handles the IF RF gain. If this parameter is not set from the user
    the default settings are returned, otherwise the user setting itself
    :param device: the device name
    :type device: string
    :param gain: the user defined gain value
    :type gain: float
    """
    if(gain == not_set_rx_if_gain):
        return hw_rx_settings[device]['if_gain']
    else:
        return gain
    

def handle_rx_bb_gain(device, gain):
    """
    Handles the RX BB gain. If this parameter is not set from the user
    the default settings are returned, otherwise the user setting itself
    :param device: the device name
    :type device: string
    :param gain: the user defined gain value
    :type gain: float
    """
    if(gain == not_set_rx_bb_gain):
        return hw_rx_settings[device]['bb_gain']
    else:
        return gain
    
def handle_rx_antenna(device, antenna):
    """
    Handles the RX antenna. If this parameter is not set from the user
    the default settings are returned, otherwise the user setting itself
    :param device: the device name
    :type device: string
    :param gain: the user defined antenna
    :type gain: string
    """
    if(len(antenna) == 0):
        return hw_rx_settings[device]['antenna']
    else:
        return antenna
    