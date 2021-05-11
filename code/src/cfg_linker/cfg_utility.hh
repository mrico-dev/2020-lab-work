#pragma once

#include "graph_linker.hh"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

typedef GraphLinker::graph_t cfg_t;
typedef GraphLinker::vertex_t vertex_t;

void put_calls_on_edges(cfg_t& cfg);
