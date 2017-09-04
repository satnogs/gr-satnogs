/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2016,2017
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <satnogs/morse_tree.h>
#include <satnogs/log.h>
#include <string.h>

namespace gr
{
  namespace satnogs
  {

    /*!
     * Constructs a Morse code tree for Morse code decoding
     */
    morse_tree::morse_tree () :
            d_unrecognized_symbol ('#'),
            d_root (new tree_node (0)),
            d_current (d_root),
            d_buff_len (4096),
            d_word_len (0),
            d_word_buffer (new char[d_buff_len])
    {
      construct_tree ();
    }

    /*!
     * Constructs a Morse code tree for Morse code decoding
     * @param unrecognized the character that will be placed
     * in the place of unrecognized symbols
     */
    morse_tree::morse_tree (char unrecognized) :
            d_unrecognized_symbol (unrecognized),
            d_root (new tree_node (0)),
            d_current (d_root),
            d_buff_len (4096),
            d_word_len (0),
            d_word_buffer (new char[d_buff_len])
    {
      construct_tree ();
    }

    morse_tree::~morse_tree ()
    {
      delete_tree(d_root);
    }

    /*!
     * Resets the pointer to the initial root node
     */
    void
    morse_tree::reset ()
    {
      d_current = d_root;
      d_word_len = 0;
      memset(d_word_buffer.get(), 0, d_buff_len);
    }

    /*!
     * Creates and initializes the Morse code decoder tree
     */
    void
    morse_tree::construct_tree ()
    {
      tree_node *e = new tree_node ('E');
      tree_node *t = new tree_node ('T');
      tree_node *i = new tree_node ('I');
      tree_node *a = new tree_node ('A');
      tree_node *n = new tree_node ('N');
      tree_node *m = new tree_node ('M');
      tree_node *s = new tree_node ('S');
      tree_node *u = new tree_node ('U');
      tree_node *r = new tree_node ('R');
      tree_node *w = new tree_node ('W');
      tree_node *d = new tree_node ('D');
      tree_node *k = new tree_node ('K');
      tree_node *g = new tree_node ('G');
      tree_node *o = new tree_node ('O');
      tree_node *h = new tree_node ('H');
      tree_node *v = new tree_node ('V');
      tree_node *f = new tree_node ('F');
      tree_node *u_u = new tree_node ('U');
      tree_node *l = new tree_node ('L');
      tree_node *u_a = new tree_node ('A');
      tree_node *p = new tree_node ('P');
      tree_node *j = new tree_node ('J');
      tree_node *b = new tree_node ('B');
      tree_node *x = new tree_node ('X');
      tree_node *c = new tree_node ('C');
      tree_node *y = new tree_node ('Y');
      tree_node *z = new tree_node ('Z');
      tree_node *q = new tree_node ('Q');
      tree_node *u_o = new tree_node ('O');
      tree_node *null0 = new tree_node (0);
      tree_node *n5 = new tree_node ('5');
      tree_node *n4 = new tree_node ('4');
      tree_node *a_s = new tree_node ('S');
      tree_node *n3 = new tree_node ('3');
      tree_node *a_e = new tree_node ('E');
      tree_node *d_d = new tree_node ('D');
      tree_node *n2 = new tree_node ('2');
      tree_node *d_e = new tree_node ('E');
      tree_node *plus = new tree_node ('+');
      tree_node *d_a = new tree_node ('A');
      tree_node *d_j = new tree_node ('J');
      tree_node *n1 = new tree_node ('1');
      tree_node *n6 = new tree_node ('6');
      tree_node *eq = new tree_node ('=');
      tree_node *slash = new tree_node ('/');
      tree_node *null1 = new tree_node (0);
      tree_node *n7 = new tree_node ('7');
      tree_node *null2 = new tree_node (0);
      tree_node *n8 = new tree_node ('8');
      tree_node *n9 = new tree_node ('9');
      tree_node *n0 = new tree_node ('0');

      d_root->set_left_child (e);
      d_root->set_right_child (t);

      e->set_left_child (i);
      e->set_right_child (a);
      t->set_left_child (n);
      t->set_right_child (m);

      i->set_left_child (s);
      i->set_right_child (u);
      a->set_left_child (r);
      a->set_right_child (w);
      n->set_left_child (d);
      n->set_right_child (k);
      m->set_left_child (g);
      m->set_right_child (o);

      s->set_left_child (h);
      s->set_right_child (v);
      u->set_left_child (f);
      u->set_right_child (u_u);
      r->set_left_child (l);
      r->set_right_child (u_a);
      w->set_left_child (p);
      w->set_right_child (j);
      d->set_left_child (b);
      d->set_right_child (x);
      k->set_left_child (c);
      k->set_right_child (y);
      g->set_left_child (z);
      g->set_right_child (q);
      o->set_left_child (u_o);
      o->set_right_child (null0);

      h->set_left_child (n5);
      h->set_right_child (n4);
      v->set_left_child (a_s);
      v->set_right_child (n3);
      f->set_left_child (a_e);
      u_u->set_left_child (d_d);
      u_u->set_right_child (n2);
      l->set_right_child (d_e);
      u_a->set_left_child (plus);
      p->set_right_child (d_a);
      j->set_left_child (d_j);
      j->set_right_child (n1);
      b->set_left_child (n6);
      b->set_right_child (eq);
      x->set_left_child (slash);
      c->set_right_child (null1);
      z->set_left_child (n7);
      z->set_right_child (null2);
      u_o->set_left_child (n8);
      null0->set_left_child (n9);
      null0->set_right_child (n0);
    }

    bool
    morse_tree::received_symbol (morse_symbol_t s)
    {
      char c = 0;
      bool ret = false;
      /* Check for overflow */
      if (d_word_len == d_buff_len) {
        return false;
      }
      switch (s)
        {
        case MORSE_DOT:
          if (d_current->get_left_child ()) {
            d_current = d_current->get_left_child ();
            ret = true;
          }
          break;
        case MORSE_DASH:
          if (d_current->get_right_child ()) {
            d_current = d_current->get_right_child ();
            ret = true;
          }
          break;
        case MORSE_S_SPACE:
          /*
           * A short space received, but the decoder is still at the root.
           * This is not in general an error so we return true
           */
          if (d_current == d_root) {
            return true;
          }
          c = d_current->get_char ();
          d_current = d_root;
          /*
           * Some nodes are null transitions and do not correspond to
           * a specific character
           */
          if (c != 0) {
            d_word_buffer[d_word_len] = c;
            d_word_len++;
            ret = true;
          }
          break;
        default:
          LOG_ERROR("Unsupported Morse symbol");
          return false;
        }
      return ret;
    }

    std::string
    morse_tree::get_word ()
    {
      return std::string(d_word_buffer.get(), d_word_len);
    }

    size_t
    morse_tree::get_max_word_len () const
    {
      return d_buff_len;
    }

    size_t
    morse_tree::get_word_len ()
    {
      return d_word_len;
    }

    void
    morse_tree::delete_tree (tree_node *node)
    {
      if (!node) {
        return;
      }
      delete_tree (node->get_left_child ());
      delete_tree (node->get_right_child ());
      delete node;
    }

    tree_node::tree_node (char c) :
            d_char (c),
            d_left (NULL),
            d_right (NULL)
    {
    }

    void
    tree_node::set_left_child (tree_node* child)
    {
      d_left = child;
    }

    void
    tree_node::set_right_child (tree_node* child)
    {
      d_right = child;
    }

    tree_node*
    tree_node::get_left_child ()
    {
      return d_left;
    }

    tree_node*
    tree_node::get_right_child ()
    {
      return d_right;
    }

    char
    tree_node::get_char ()
    {
      return d_char;
    }

  } /* namespace satnogs */
} /* namespace gr */
