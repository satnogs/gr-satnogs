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

#ifndef LOG_H_
#define LOG_H_


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <satnogs/config.h>
#include <sys/syscall.h>


#if CW_DEBUG
#define LOG_INFO(M, ...) 							\
		fprintf(stderr, "[INFO]: " M " \n", ##__VA_ARGS__)

#else
#define LOG_INFO(M, ...)
#endif

#define LOG_ERROR(M, ...) 							\
	fprintf(stderr, "[ERROR] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_WARN(M, ...) 								\
	fprintf(stderr, "[WARNING] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#if CW_DEBUG
#define LOG_DEBUG(M, ...) 							\
	fprintf(stderr, "[DEBUG]: " M "\n", ##__VA_ARGS__)
#else
#define LOG_DEBUG(M, ...)
#endif


#endif /* UTILS_LOG_H_ */
