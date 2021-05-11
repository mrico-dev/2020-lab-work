#pragma once

#include <string>
#include <iostream>
#include <spot/parseaut/public.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twa/bddprint.hh>

void print_twa_as_dot(std::ostream& out, spot::twa_graph_ptr& aut);

