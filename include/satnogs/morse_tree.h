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

#ifndef INCLUDED_SATNOGS_MORSE_TREE_H
#define INCLUDED_SATNOGS_MORSE_TREE_H

#include <satnogs/api.h>

namespace gr {
  namespace satnogs {

    /*!
     * \brief Binary tree node containing the corresponding character
     */
    class tree_node
    {
    private:
      const char d_char;
      tree_node *d_left;
      tree_node *d_right;

    public:
      tree_node (char c);

      void
      set_left_child (tree_node *child);

      void
      set_right_child (tree_node *child);

      tree_node*
      get_left_child();

      tree_node*
      get_right_child();
    };

    /*!
     * \brief A Binary tree representation of the Morse coding scheme.
     * Left transitions occur when a dot is received, whereas right transitions
     * are perfromed during the reception of a dash.
     *
     * The tree follows the ITU International Morse code representation
     * ITU-R M.1677-1
     */
    class SATNOGS_API morse_tree
    {
    public:
      morse_tree();
      ~morse_tree();

      void reset();
    private:
      tree_node *d_root;
      tree_node *d_current;

      void construct_tree();
      void delete_tree(tree_node *node);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_MORSE_TREE_H */

