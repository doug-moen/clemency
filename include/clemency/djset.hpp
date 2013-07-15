// clemency - a OpenCL marching cubes mesh generator
//
// copyright (C) 2013, Tobias Sargeant <tobias.sargeant@gmail.com>
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#pragma once

#include <vector>



class djset {

protected:
  struct elem {
    size_t parent, rank;
    elem(size_t p, size_t r) : parent(p), rank(r) {}
    elem() {}
  };

  std::vector<elem> set;
  size_t n_sets;

public:
  djset() : set(), n_sets(0) {
  }

  djset(size_t N) {
    n_sets = N;
    set.reserve(N);
    for (size_t i = 0; i < N; ++i) {
      set.push_back(elem(i,0));
    }
  }

  void init(size_t N) {
    if (N == set.size()) {
      for (size_t i = 0; i < N; ++i) {
        set[i] = elem(i,0);
      }
      n_sets = N;
    } else {
      djset temp(N);
      std::swap(set, temp.set);
      std::swap(n_sets, temp.n_sets);
    }
  }

  size_t count() const {
    return n_sets;
  }

  size_t find_set_head(size_t a) {
    if (a == set[a].parent) return a;
    
    size_t a_head = a;
    while (set[a_head].parent != a_head) a_head = set[a_head].parent;
    set[a].parent = a_head;
    return a_head;
  }

  bool same_set(size_t a, size_t b) {
    return find_set_head(a) == find_set_head(b);
  }

  void merge_sets(size_t a, size_t b) {
    a = find_set_head(a);
    b = find_set_head(b);
    if (a != b) {
      n_sets--;
      if (set[a].rank < set[b].rank) {
        set[a].parent = b;
      } else if (set[b].rank < set[a].rank) {
        set[b].parent = a;
      } else {
        set[a].rank++;
        set[b].parent = a;
      }
    }
  }

  void get_index_to_set(std::vector<size_t> &index_set) {
    index_set.clear();
    index_set.resize(set.size(), n_sets);

    size_t c = 0;
    for (size_t i = 0; i < set.size(); ++i) {
      size_t s = find_set_head(i);
      if (index_set[s] == n_sets) index_set[s] = c++;
      index_set[i] = index_set[s];
    }
  }

  void get_index_to_set(std::vector<size_t> &index_set, std::vector<size_t> &set_size) {
    index_set.clear();
    index_set.resize(set.size(), n_sets);
    set_size.clear();
    set_size.resize(n_sets, 0);

    size_t c = 0;
    for (size_t i = 0; i < set.size(); ++i) {
      size_t s = find_set_head(i);
      if (index_set[s] == n_sets) index_set[s] = c++;
      index_set[i] = index_set[s];
      set_size[index_set[s]]++;
    }
  }

  template<typename in_iter_t, typename out_collection_t>
  void collate(in_iter_t in, out_collection_t &out) {
    std::tr1::unordered_map<size_t, size_t> set_id;
    out.clear();
    out.resize(n_sets);
    size_t c = 0;
    for (size_t i = 0; i < set.size(); ++i) {
      size_t s = find_set_head(i);
      std::tr1::unordered_map<size_t, size_t>::iterator id = set_id.find(s);
      if (id == set_id.end()) {
        s = set_id[s] = c++;
      } else {
        s = (*id).second;
      }

      std::insert_iterator<typename out_collection_t::value_type> j(out[s], out[s].end());
      *j = *in++;
    }
  }

  template<typename out_collection_t>
  void collate(out_collection_t &out) {
    std::tr1::unordered_map<size_t, size_t> set_id;
    out.clear();
    out.resize(n_sets);
    size_t c = 0;
    for (size_t i = 0; i < set.size(); ++i) {
      size_t s = find_set_head(i);
      std::tr1::unordered_map<size_t, size_t>::iterator id = set_id.find(s);
      if (id == set_id.end()) {
        s = set_id[s] = c++;
      } else {
        s = (*id).second;
      }
      std::insert_iterator<typename out_collection_t::value_type> j(out[s], out[s].end());
      *j = i;
    }
  }
};
