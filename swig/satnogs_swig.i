/* -*- c++ -*- */

#define SATNOGS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "satnogs_swig_doc.i"

%{
#include "satnogs/cw_matched_filter_ff.h"
#include "satnogs/morse_tree.h"
#include "satnogs/morse_decoder.h"
#include "satnogs/morse_debug_source.h"
#include "satnogs/clear_text_msg_sink.h"
#include "satnogs/cw_to_symbol.h"
#include "satnogs/afsk_decoder.h"
#include "satnogs/sine_matched_filter_ff.h"
%}

%include "satnogs/cw_matched_filter_ff.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, cw_matched_filter_ff);
%include "satnogs/morse_tree.h"
%include "satnogs/morse_decoder.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, morse_decoder);
%include "satnogs/morse_debug_source.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, morse_debug_source);
%include "satnogs/clear_text_msg_sink.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, clear_text_msg_sink);
%include "satnogs/cw_to_symbol.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, cw_to_symbol);
%include "satnogs/afsk_decoder.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, afsk_decoder);
%include "satnogs/sine_matched_filter_ff.h"
GR_SWIG_BLOCK_MAGIC2(satnogs, sine_matched_filter_ff);
