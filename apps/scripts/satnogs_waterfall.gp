#!/usr/bin/gnuplot
#
#  gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
#
#  Copyright (C) 2017, Libre Space Foundation <http://librespacefoundation.org/>
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

# satnogs-waterfall.gp
# Plot a waterfall diagramm from the output of the satnogs_waterfall_sink block.
#
# Usage:
# gnuplot -e "inputfile='waterfall_sink.data'" -e "outfile='waterfall.png'" /usr/local/share/satnogs/scripts/satnogs_waterfall.gp

reset
if (!exists("height")) height=800
if (!exists("width")) width=800
if (!exists("outfile")) outfile='/tmp/waterfall.png'

################################################################################
# Get FFT size and maximum number of records so we can plot the waterfall
# even in case the file was not properly closed and contains data that are
# not an integer multiple of the FFT size
################################################################################

# Set terminal to unkown so no ploting is performed.
set terminal unknown

# Read the first float to get the FFT size 
# The file contains a float specifying the FFT size the x-axis scale 
# and then each each pixel line
# starts with the time in seconds from the beginning of the capture. 
# Then, read until the last float of a fully filled pixel line to get the number
# of floats that should be in the file.
# For strange reasons the STATS_records returns inconcistent size (always the half)
plot inputfile binary format='%float32' index 0 every ::0:0:0:0 using (fft_size = $1), \
     "" binary format='%float32' every fft_size + 1::fft_size using (float_num = $0 + 1)
cmd = sprintf("truncate -s %u %s", float_num * 4, inputfile)

# Truncate properly the file
system(cmd)

set view map
set terminal pngcairo size width,height enhanced font 'Verdana,14'
set output outfile

unset key
set style line 11 lc rgb '#808080' lt 1
set border 3 front ls 11
set style line 12 lc rgb '#888888' lt 0 lw 1
set grid front ls 12
set tics nomirror out scale 0.75

set xlabel 'Frequency (kHz)'
set ylabel 'Time'
set cbtics scale 0

# Spectravue palette and scale
set cbtics (-110, -105, -100, -95, -90, -85, -80, -75, -70, -65, -60, -55, -50, -55, -40)

# palette
set palette defined (0 '#000000', \
                     1 '#0000e7', \
                     2 '#0094ff', \
                     3 '#00ffb8', \
                     4 '#2eff00', \
                     5 '#ffff00', \
                     6 '#ff8800', \
                     7 '#ff0000', \
                     8 '#ff007c')
set ylabel 'Time (seconds)'
set cbrange [-100:-50]
set cblabel 'Power (dB)'


# Get automatically the axis ranges from the file
stats inputfile using 1 binary nooutput
set xrange [STATS_min*1e-3:STATS_max*1e-3 + 1]
stats inputfile using 2 binary nooutput
set yrange [0:STATS_max + 1]

# Plot and scale the frequency axis to kHz for readability
plot inputfile every 2 using ($1*1e-3):2:3 binary matrix with image
