/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016, 2017
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

#ifndef INCLUDED_SATNOGS_MORSE_DEBUG_SOURCE_IMPL_H
#define INCLUDED_SATNOGS_MORSE_DEBUG_SOURCE_IMPL_H

#include <satnogs/morse_debug_source.h>
#include <thread>
#include <algorithm>
#include <vector>

namespace gr
{
  namespace satnogs
  {

    class morse_debug_source_impl : public morse_debug_source
    {
    private:
      const size_t d_wpm;
      const bool d_inject_errors;
      const float d_p;
      const size_t d_seq_pause_ms;
      bool d_run;
      const char d_chars[36];
      const std::vector<std::string> d_symbols;
      std::thread d_thread;

      void
      send_debug_msg (std::string sentence);

    public:
      morse_debug_source_impl (const size_t wpm, std::string debug_seq,
                               bool inject_errors,
                               float error_prob, size_t seq_pause_ms);
      ~morse_debug_source_impl ();
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_MORSE_DEBUG_SOURCE_IMPL_H */

