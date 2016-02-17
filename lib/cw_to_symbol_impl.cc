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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <satnogs/log.h>
#include "cw_to_symbol_impl.h"

namespace gr
{
  namespace satnogs
  {

    cw_to_symbol::sptr
    cw_to_symbol::make (double sampling_rate, float threshold,
			float conf_level, size_t wpm)
    {
      return gnuradio::get_initial_sptr (
	  new cw_to_symbol_impl (sampling_rate, threshold, conf_level, wpm));
    }

    /*
     * The private constructor
     */
    cw_to_symbol_impl::cw_to_symbol_impl (double sampling_rate, float threshold,
					  float conf_level, size_t wpm) :
	gr::sync_block ("cw_to_symbol",
			gr::io_signature::make (1, 1, sizeof(float)),
			gr::io_signature::make (0, 0, 0)),
			d_sampling_rate(sampling_rate),
			d_act_thrshld(threshold),
			d_confidence_level(conf_level),
			d_dot_samples((1.2/wpm) / (1.0 / sampling_rate)),
			d_dash_samples(3 * d_dot_samples),
			d_short_pause_samples(3 * d_dot_samples),
			d_long_pause_samples(7 * d_dot_samples),
			d_state(IDLE),
			d_state_cnt(0),
			d_pause_cnt(0),
			d_seq_started(false)
    {
      message_port_register_in(pmt::mp("act_threshold"));
      message_port_register_out(pmt::mp("out"));
      set_msg_handler(pmt::mp("act_threshold"),
      		      boost::bind(&cw_to_symbol_impl::set_act_threshold_msg_handler,
      				  this, _1));
    }

    inline void
    cw_to_symbol_impl::send_symbol_msg (morse_symbol_t s)
    {
      message_port_pub(pmt::mp("out"), pmt::from_long(s));
    }

    /*
     * Our virtual destructor.
     */
    cw_to_symbol_impl::~cw_to_symbol_impl ()
    {
    }

    inline void
    cw_to_symbol_impl::set_idle ()
    {
      d_state = IDLE;
      d_state_cnt = 0;
      d_pause_cnt = 0;
    }

    inline void
    cw_to_symbol_impl::set_short_on ()
    {
      d_state = SHORT_ON_PERIOD;
      d_state_cnt = 1;
      d_pause_cnt = 0;
    }

    inline void
    cw_to_symbol_impl::set_long_on ()
    {
      d_state = LONG_ON_PERIOD;
    }

    inline void
    cw_to_symbol_impl::set_short_off ()
    {
      d_state = SHORT_OFF_PERIOD;
      d_state_cnt = 0;
      d_pause_cnt = 1;
    }

    inline void
    cw_to_symbol_impl::set_long_off ()
    {
      d_state = LONG_OFF_PERIOD;
    }

    void
    cw_to_symbol_impl::set_act_threshold_msg_handler (pmt::pmt_t msg)
    {
      if(pmt::is_pair(msg)){
	set_act_threshold(pmt::to_float(pmt::cdr(msg)));
      }
    }

    int
    cw_to_symbol_impl::work (int noutput_items,
			     gr_vector_const_void_star &input_items,
			     gr_vector_void_star &output_items)
    {
      int i;
      float conf_lvl;
      const float *in = (const float *) input_items[0];

      for(i = 0; i < noutput_items; i++) {
	switch(d_state){
	  case IDLE:
	    if(in[i] > d_act_thrshld) {
	      set_short_on();
	    }
	    break;

	  case SHORT_ON_PERIOD:
	    if( in[i] > d_act_thrshld ) {
	      d_state_cnt++;

	      if(d_state_cnt > d_dot_samples){
		set_long_on();
	      }
	    }
	    else{
	      /*
	       * Before going to short pause, check the confidence level.
	       *  Perhaps a short symbol should be produced.
	       *
	       *  Otherwise, it was a false alarm.
	       */
	      conf_lvl = ((float) d_state_cnt) / d_dot_samples;
	      if(conf_lvl > d_confidence_level){
		LOG_DEBUG("DOT");
		send_symbol_msg(MORSE_DOT);
	      }

	      /* Go find a possible short pause symbol */
	      set_short_off();
	    }
	    break;

	  case LONG_ON_PERIOD:
	    if( in[i] > d_act_thrshld ) {
	      d_state_cnt++;
	    }
	    else {
	      conf_lvl = ((float) d_state_cnt) / d_dash_samples;
	      if(conf_lvl > d_confidence_level) {
		LOG_DEBUG("DASH");
		send_symbol_msg(MORSE_DASH);
		set_short_off();
		break;
	      }

	      /* Perhaps this was a short on symbol */
	      conf_lvl = ((float) d_state_cnt) / d_dot_samples;
	      if(conf_lvl > d_confidence_level) {
		LOG_DEBUG("DOT");
		send_symbol_msg(MORSE_DOT);
		set_short_off();
	      }

	    }
	    break;

	  case SHORT_OFF_PERIOD:
	    if(in[i] > d_act_thrshld) {
	      /*
	       * Before going to ON state, again check if a short pause symbol
	       * should be produced
	       */
	      conf_lvl = ((float) d_pause_cnt) / d_short_pause_samples;
	      if(conf_lvl > d_confidence_level) {
		LOG_DEBUG("Short space");
		send_symbol_msg(MORSE_S_SPACE);
	      }
	      set_short_on();
	    }
	    else {
	      d_pause_cnt++;
	      if(d_pause_cnt > d_short_pause_samples) {
		set_long_off();
	      }
	    }
	    break;

	  case LONG_OFF_PERIOD:
	    if(in[i] > d_act_thrshld) {
	      conf_lvl = ((float) d_pause_cnt) / d_long_pause_samples;
	      if (conf_lvl > d_confidence_level) {
		LOG_DEBUG("Long space");
		send_symbol_msg (MORSE_L_SPACE);
		set_idle();
		break;
	      }
	      else {
		LOG_DEBUG("Short space");
		send_symbol_msg (MORSE_S_SPACE);
	      }
	      set_short_on();
	    }
	    else {
	      d_pause_cnt++;
	      /*
	       * If the pause duration is greater than the long pause symbol,
	       * definitely a long pause symbol should be produced
	       */
	      if(d_pause_cnt > d_long_pause_samples) {
		LOG_DEBUG("Long space");
		send_symbol_msg(MORSE_L_SPACE);
		d_seq_started = false;
		set_idle();
	      }
	    }
	    break;
	  default:
	    LOG_ERROR("Invalid State.");
	    d_state = IDLE;
	}
      }
      return noutput_items;
    }

    void
    cw_to_symbol_impl::set_act_threshold (float thrhld)
    {
      d_act_thrshld = thrhld;
    }

  } /* namespace satnogs */
} /* namespace gr */

