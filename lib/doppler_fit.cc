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
#include <satnogs/doppler_fit.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace gr
{
  namespace satnogs
  {

    /**
     * Creates a polynomial fitting routine.
     * @param degree the degree of the polynomial
     */
    doppler_fit::doppler_fit (size_t degree) :
	d_degree(degree),
	d_last_x(0.0)
    {
    }

    /**
     * This method calculates the coefficients of the polynomial that will
     * be used by the predict_freqs() method to produce simulated frequency
     * differences
     * @param drifts the queue containing the frequency differences and the
     * corresponding timings that these frequencies diffrences occured. Time is
     * measured in samples since the start of the flowgraph execution.
     */
    void
    doppler_fit::fit (std::deque<freq_drift> drifts)
    {
      size_t i;
      size_t j;
      size_t s;
      size_t singular;
      double val;
      s = drifts.size();

      boost::numeric::ublas::matrix<double> x_matrix(s, d_degree+1);
      boost::numeric::ublas::matrix<double> y_matrix(s, 1);

      for(i = 0; i < s; i++){
	y_matrix(i, 0) = drifts[i].get_freq_drift();
      }

      for(i = 0; i < s; i++){
	val = 1.0;
	for(j = 0; j < d_degree + 1; j++) {
	  x_matrix(i, j) = val;
	  val *= drifts[i].get_x();
	}
      }

      /* Transpose the matrix with the x values */
      boost::numeric::ublas::matrix<double> tx_matrix (
	  boost::numeric::ublas::trans (x_matrix));

      boost::numeric::ublas::matrix<double> txx_matrix (
	  boost::numeric::ublas::prec_prod(tx_matrix, x_matrix));

      boost::numeric::ublas::matrix<double> txy_matrix (
	  boost::numeric::ublas::prec_prod(tx_matrix, y_matrix));

      boost::numeric::ublas::permutation_matrix<int> perm(txx_matrix.size1());
      singular = boost::numeric::ublas::lu_factorize(txx_matrix, perm);
      BOOST_ASSERT( singular == 0 );

      boost::numeric::ublas::lu_substitute(txx_matrix, perm, txy_matrix);

      /*
       * Lock the mutex to make sure that no one uses at the same time the
       * coefficients
       */
      boost::mutex::scoped_lock lock(d_mutex);
      d_coeff = std::vector<double> (txy_matrix.data().begin(),
				     txy_matrix.data().end());
      d_last_x = drifts[s - 1].get_x();
    }

    /**
     * Creates a number of frequency differences predictions using polynomial
     * curve fitting.
     * @param freqs buffer that will hold the predicted frequency differences.
     * It is responsibility of the caller to provide enough memory for at most
     * \p ncorrections double numbers.
     * @param ncorrections the number predicted frequencies that the method
     * will produce.
     * @param samples_per_correction the number of samples elapsed between each
     * correction.
     */
    void
    doppler_fit::predict_freqs (double *freqs, size_t ncorrections,
		     size_t samples_per_correction)
    {
      size_t i;
      size_t j;
      double predicted_freq_diff;
      double x;
      double xT;
      boost::mutex::scoped_lock lock(d_mutex);
      for(i = 0; i < ncorrections; i++){
	predicted_freq_diff = 0.0;
	xT = 1.0;
	x = d_last_x + i * samples_per_correction;
	for(j = 0; j < d_degree + 1; j++){
	  predicted_freq_diff += d_coeff[j] * xT;
	  xT *= x;
	}
	freqs[i] = predicted_freq_diff;
      }

      /*
       * The predict method can be called multiple times without update the
       * fitness of the polynomial. For this reason we alter the last x
       */
      d_last_x = d_last_x + (ncorrections + 1) * samples_per_correction;
    }

  } /* namespace satnogs */
} /* namespace gr */

