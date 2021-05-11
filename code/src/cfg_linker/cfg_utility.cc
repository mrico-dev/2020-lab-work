#include "cfg_utility.hh"

typedef boost::graph_traits<cfg_t>::vertex_iterator vertex_iter;

// Return the vertex with no children
std::optional<vertex_t> get_end(const cfg_t& g)
{
    auto index = boost::get(boost::vertex_index, g);

    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        boost::graph_traits<cfg_t>::vertex_descriptor v = *vp.first;

        auto found = true;
        boost::graph_traits<cfg_t>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end && found; ++ei)
        {
            if (index[source(*ei, g)] == v)
                found = false;
        }

        if (found)
            return v;
    }

    return std::nullopt;
}

static void loop_on_end(cfg_t& cfg)
{
    auto v_end = get_end(cfg);

    if (v_end)
    {
        DotEdge edge{"_loop_end"};
        boost::add_edge(*v_end, *v_end, edge, cfg);
    }
}

static void set_call_on_edge(cfg_t& cfg, vertex_t v, std::string func_call)
{
    auto new_v = boost::add_vertex(cfg);
    cfg[new_v].func_name = cfg[v].func_name;

    // Getting all nodes that are to be removed inside a vector, so we can
    // remove only after we're finished accessing everything
    std::vector<std::pair<vertex_t, vertex_t>> to_be_removed;

    // Accessing out-edges
    typedef boost::graph_traits<cfg_t> graph_traits_t;
    graph_traits_t::out_edge_iterator out_in, out_end;
    for (boost::tie(out_in, out_end) = boost::out_edges(v, cfg); out_in != out_end; ++out_in)
    {
        auto e = *out_in;
        // Moving all out-edges from v to new_v
        auto& e_label = cfg[e].label;
        DotEdge edge(e_label);
        boost::add_edge(new_v, boost::target(e, cfg), edge, cfg);

        to_be_removed.push_back({v, boost::target(e, cfg)});
    }

    for (auto& v_pair : to_be_removed)
        boost::remove_edge(v_pair.first, v_pair.second, cfg);
    
    DotEdge edge;
    edge.label = func_call;
    boost::add_edge(v, new_v, edge, cfg);
}

void put_calls_on_edges(cfg_t& cfg)
{
    // Accessing vertexes
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(cfg); vp.first != vp.second; ++vp.first)
    {
        auto index = *vp.first;
        auto& vertex = cfg[index];
        if (!vertex.function_calls)
            continue;

        for (int i = vertex.function_calls->size() - 1; i >= 0; --i)
        {
            // Safety: Reaccessing vertex in case of reallocation
            auto& vertex_re = cfg[index];
            auto& func = (*vertex_re.function_calls)[i];

            set_call_on_edge(cfg, index, func);

            // Safety: Reaccessing vertex in case of reallocation
            auto& vertex_rere = cfg[*vp.first];
            auto& calls = vertex_rere.function_calls;
            calls->erase(calls->begin() + i);
        }
    }

    loop_on_end(cfg);
}
