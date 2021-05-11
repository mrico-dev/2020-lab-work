#include "automata_builder.hh"

#include <iostream>
#include <spot/twaalgos/hoa.hh>
#include <spot/twa/twagraph.hh>

static cfg_vertex_t get_cfg_init_state(const cfg_t& cfg)
{
    typedef boost::graph_traits<cfg_t>::vertex_iterator vertex_iter; 

    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(cfg); vp.first != vp.second; ++vp.first)
        if (cfg[*vp.first].is_program_entrance)
            return *vp.first;

    throw new std::runtime_error("Cannot find any entrance node in CFG");
}

static void set_edges_from_cfg(spot::twa_graph_ptr aut, const cfg_t& cfg)
{
    boost::graph_traits<cfg_t>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(cfg); ei != ei_end; ++ei)
    {
        auto from = static_cast<unsigned>(source(*ei, cfg));
        auto to = static_cast<unsigned>(target(*ei, cfg));

        bdd p = bdd_ithvar(aut->register_ap(cfg[*ei].label));
        aut->new_edge(from, to, p, {0});
    }
}

static void simplify(spot::twa_graph_ptr aut)
{
    aut->merge_edges();
    aut->merge_states();
    aut->purge_dead_states();
}

spot::twa_graph_ptr get_twa_from_cfg(const cfg_t& cfg)
{
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    spot::twa_graph_ptr aut = make_twa_graph(dict);
    
    auto cfg_len = boost::num_vertices(cfg); 
    aut->set_buchi();
    aut->prop_state_acc(false);

    aut->new_states(cfg_len);
    aut->set_init_state(static_cast<unsigned>(get_cfg_init_state(cfg)));

    set_edges_from_cfg(aut, cfg);

    simplify(aut);

    return aut;
}
