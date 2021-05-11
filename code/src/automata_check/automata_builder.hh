#pragma once

#include <spot/twaalgos/hoa.hh>
#include <spot/twa/twagraph.hh>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "../cfg_linker/graph_linker.hh"

typedef GraphLinker::graph_t cfg_t;
typedef GraphLinker::vertex_t cfg_vertex_t;
typedef spot::twa_graph_ptr twa_ptr;

twa_ptr get_twa_from_cfg(const cfg_t& cfg);

