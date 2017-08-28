/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016,2017
 *  Libre Space Foundation <http://librespacefoundation.org/>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDE_SATNOGS_MORSE_H_
#define INCLUDE_SATNOGS_MORSE_H_

#define MIN_WPM 5
#define MAX_WPM 30
/**
 * The different Morse symbols
 */
typedef enum {
  MORSE_DOT = 0,        //!< Morse dot (.) symbol
  MORSE_DASH,           //!< Morse dash (-) symbol
  MORSE_INTRA_SPACE,    //!< Space between dot and dash symbols
  MORSE_S_SPACE,        //!< Morse short space between characters
  MORSE_L_SPACE         //!<Morse long space between words
} morse_symbol_t;



#endif /* INCLUDE_SATNOGS_MORSE_H_ */
