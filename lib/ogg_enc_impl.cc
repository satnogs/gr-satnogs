/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2017, Libre Space Foundation <http://librespacefoundation.org/>
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
#include "ogg_encoder_impl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


namespace gr {
  namespace satnogs {

    ogg_encoder::sptr
    ogg_encoder::make(char* filename, double samp_rate, float quality)
    {
      return gnuradio::get_initial_sptr
        (new ogg_encoder_impl(filename, samp_rate, quality));
    }

    /*
     * The private constructor
     */
    ogg_encoder_impl::ogg_encoder_impl(char* filename, double samp_rate, float quality)
      : gr::sync_block("ogg_encoder",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(0, 0, 0))
    {
    	d_quality = quality;
        d_out=fopen(filename,"wb");
        d_samp_rate = samp_rate;
        vorbis_info_init(&d_vi);
        int ret = vorbis_encode_init_vbr(&d_vi,1,d_samp_rate,d_quality);
        if(ret)exit(1);

        vorbis_comment_init(&d_vc);
        vorbis_comment_add_tag(&d_vc, "ENCODER", "satnogs ogg encoder");

        vorbis_analysis_init(&d_vd, &d_vi);
        vorbis_block_init(&d_vd, &d_vb);

        srand(time(NULL));
        ogg_stream_init(&d_os, rand());

        ogg_packet header;
        ogg_packet header_comm;
        ogg_packet header_code;

        vorbis_analysis_headerout(&d_vd, &d_vc, &header, &header_comm, &header_code);
        ogg_stream_packetin(&d_os, &header);
        ogg_stream_packetin(&d_os, &header_comm);
        ogg_stream_packetin(&d_os, &header_code);
        int result = 1;
        while(result){
          result=ogg_stream_flush(&d_os,&d_og);
          if(result==0)break;
          fwrite(d_og.header,1,d_og.header_len,d_out);
          fwrite(d_og.body,1,d_og.body_len,d_out);
        }
    }

    ogg_encoder_impl::~ogg_encoder_impl()
    {
    	  vorbis_analysis_wrote(&d_vd,0);
    	  ogg_stream_clear(&d_os);
    	  vorbis_block_clear(&d_vb);
    	  vorbis_dsp_clear(&d_vd);
    	  vorbis_comment_clear(&d_vc);
    	  vorbis_info_clear(&d_vi);
    	  fclose(d_out);
    }

    int
    ogg_encoder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      size_t chunks = (noutput_items*sizeof(float))/2;
      const signed char *in = (const signed char *) input_items[0];
    	  int i;
    	  long bytes = 1024;
    	  float **buffer=vorbis_analysis_buffer(&d_vd,chunks);
    	  for(i=0;i<chunks;i++){
    	          buffer[0][i]=((in[i*2+1]<<8)|
    	                        (0x00ff&(int)in[i*2]))/32768.f;
    	  }
    	  vorbis_analysis_wrote(&d_vd,i);

      while(vorbis_analysis_blockout(&d_vd,&d_vb)==1){
        vorbis_analysis(&d_vb,NULL);
        vorbis_bitrate_addblock(&d_vb);

        while(vorbis_bitrate_flushpacket(&d_vd,&d_op)){

          ogg_stream_packetin(&d_os,&d_op);
          int result = 1;
          while(result){
            int result=ogg_stream_pageout(&d_os,&d_og);
            if(result==0)break;
            fwrite(d_og.header,1,d_og.header_len,d_out);
            fwrite(d_og.body,1,d_og.body_len,d_out);
            if(ogg_page_eos(&d_og))result=1;
          }
        }
      }
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */
