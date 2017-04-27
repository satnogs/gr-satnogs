/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016, Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDE_SATNOGS_CONFIG_H_
#define INCLUDE_SATNOGS_CONFIG_H_

/*!
 * Enable debug messages for the module
 */
#define ENABLE_DEBUG_MSG 1

/*!
 * Enable debug messages for the CW decoding mechanism
 */
#define CW_DEBUG 1

/*!
 * The maximum allowed frame length of the UPSAT satellite
 */
#define UPSAT_MAX_FRAME_LEN 255

#endif /* INCLUDE_SATNOGS_CONFIG_H_ */
