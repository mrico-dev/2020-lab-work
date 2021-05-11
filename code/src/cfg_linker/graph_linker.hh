#pragma once

#include "dot_reader.hh"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/copy.hpp>
#include <optional>

class GraphLinker {

public:
    typedef DotReader::graph_t graph_t;
    typedef std::vector<std::pair<std::shared_ptr<graph_t>, std::string>> graphs_t;
    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
    typedef std::vector<std::pair<std::string, std::pair<vertex_t, std::optional<vertex_t>>>> call_stack_t;
    typedef std::list<std::vector<std::string>> fcalls_t;

    GraphLinker(std::vector<std::string> filenames);
    ~GraphLinker() = default;

    std::shared_ptr<graph_t> link();
    std::shared_ptr<graph_t> link_sub(unsigned int max_depth);
    std::shared_ptr<graph_t> link_over();

private:
    void copy_graph_into(const graph_t& other);
    void reset_edge_label(vertex_t v);
    graph_t* find_graph(const std::string& func_name);
    std::optional<std::pair<vertex_t, std::optional<vertex_t>>> insert_func(const std::string& func_name, vertex_t v);
    std::optional<vertex_t> get_parent(vertex_t curr);
    std::pair<vertex_t, std::optional<vertex_t>> insert_graph_between(const graph_t& to_insert, vertex_t begin, vertex_t end, std::string func_name);
    void link_over_rec(vertex_t from, call_stack_t& call_stack);

private:
    graphs_t graphs;
    std::shared_ptr<graph_t> main;
    fcalls_t function_calls;
};

std::string get_func_name(std::string label);
