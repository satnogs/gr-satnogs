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
#include <satnogs/morse.h>
#include "cw_to_symbol_impl.h"

namespace gr
{
  namespace satnogs
  {

    cw_to_symbol::sptr
    cw_to_symbol::make (double sampling_rate, float threshold, size_t wpm)
    {
      return gnuradio::get_initial_sptr (
	  new cw_to_symbol_impl (sampling_rate, threshold, wpm));
    }

    /*
     * The private constructor
     */
    cw_to_symbol_impl::cw_to_symbol_impl (double sampling_rate, float threshold,
					  size_t wpm) :
	gr::sync_block ("cw_to_symbol",
			gr::io_signature::make (1, 1, sizeof(float)),
			gr::io_signature::make (0, 0, 0)),
			d_sampling_rate(sampling_rate),
			d_act_thrshld(threshold),
			d_confidence_level(0.90),
			d_dot_samples((1.2/wpm) / (1.0 / sampling_rate)),
			d_dash_samples(3 * d_dot_samples),
			d_short_pause_samples(3 * d_dot_samples),
			d_long_pause_samples(7 * d_dot_samples),
			d_state(IDLE),
			d_state_cnt(0),
			d_pause_cnt(0)
    {
      message_port_register_out(pmt::mp("out"));
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
    }

    inline void
    cw_to_symbol_impl::set_long_on ()
    {
      d_state = LONG_ON_PERIOD;
      d_pause_cnt = 0;
    }

    inline void
    cw_to_symbol_impl::set_short_off (size_t borrow_cnt)
    {
      d_state = SHORT_OFF_PERIOD;
      d_state_cnt = 0;
      d_pause_cnt += borrow_cnt;

      /* Due to time-slots borrowing we may be already at the Long pause */
      if(d_pause_cnt > d_short_pause_samples) {
	d_state = LONG_OFF_PERIOD;
      }
    }

    inline void
    cw_to_symbol_impl::set_long_off ()
    {
      d_state = LONG_OFF_PERIOD;
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
	    // TODO Avoid unnecessary pause messages
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
	       *  Otherwise, it was a false alarm. So the time-slots counted
	       *  so far should be assigned as pause time-slots.
	       */
	      conf_lvl = ((float) d_state_cnt) / d_dot_samples;
	      if(conf_lvl > d_confidence_level){
		message_port_pub(pmt::mp("out"), pmt::from_long(MORSE_DOT));
		set_short_off(0);
	      }
	      else{
		set_short_off(d_state_cnt);
	      }
	    }
	    break;

	  case LONG_ON_PERIOD:
	    if( in[i] > d_act_thrshld ) {
	      d_state_cnt++;
	    }
	    else{
	      /*
	       * In this case the FSM should continue at the SHORT_OFF state.
	       * Before this depending the confidence for a long pulse, produce
	       * a short or long pulse message
	       */
	      conf_lvl = ((float) d_state_cnt) / d_dash_samples;
	      if(conf_lvl > d_confidence_level){
		message_port_pub(pmt::mp("out"), pmt::from_long(MORSE_DASH));
	      }
	      else{
		message_port_pub(pmt::mp("out"), pmt::from_long(MORSE_DOT));
	      }
	      set_short_off(0);
	    }
	    break;

	  case SHORT_OFF_PERIOD:
	    if(in[i] > d_act_thrshld) {
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
	      set_short_on();
	    }
	    else {
	      d_pause_cnt++;
	      if(d_pause_cnt > d_long_pause_samples) {
		message_port_pub(pmt::mp("out"), pmt::from_long(MORSE_L_SPACE));
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

  } /* namespace satnogs */
} /* namespace gr */

