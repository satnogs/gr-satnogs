/* -*- c++ -*- */

%include <typemaps.i>
%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "satnogs_swig_doc.i"

%{
#include "satnogs/morse_debug_source.h"
#include "satnogs/debug_msg_source.h"
#include "satnogs/debug_msg_source_raw.h"
%}

%include "satnogs/morse_debug_source.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, morse_debug_source);

%include "satnogs/debug_msg_source.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, debug_msg_source);

%include "satnogs/debug_msg_source_raw.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, debug_msg_source_raw);

