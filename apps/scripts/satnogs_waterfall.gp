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
#

reset
if (!exists("height")) height=800
if (!exists("width")) width=800
if (!exists("outfile")) outfile='/tmp/waterfall.png'

set terminal pngcairo size height,width enhanced font 'Verdana,14'
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

# palette
set palette defined (0 '#3288BD',\
                     1 '#66C2A5',\
                     2 '#ABDDA4',\
                     3 '#E6F598',\
                     4 '#FEE08B',\
                     5 '#FDAE61',\
                     6 '#F46D43',\
                     7 '#D53E4F')
set ylabel 'Time (seconds)'
set cbrange [-110:-20]
set cblabel 'Power (dB)'

# Get automatically the axis ranges from the file 
stats inputfile using 1 binary nooutput
set xrange [STATS_min*1e-3:STATS_max*1e-3 + 1]
stats inputfile using 2 binary nooutput
set yrange [0:STATS_max + 1]

# Plot and scale the frequency axis to kHz for readability 
plot inputfile using ($1*1e-3):2:3 binary matrix with image
