#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/phoenix.hpp>
#include <memory>

#define SIZE 1024

using boost::phoenix::arg_names::arg1;

struct DotVertex
{
    DotVertex() = default;

    std::vector<std::string>* function_calls = nullptr;

    std::string name;
    std::string label;
    std::string func_name;

    bool is_program_entrance = false;
    int Attribute[SIZE];
};

struct DotEdge
{
    DotEdge() = default;
    DotEdge(std::string l) : label(l) {}
    std::string label;
};
    
// We use a list so the address won't change because of re-allocation
typedef std::list<std::vector<std::string>> fcalls_t;

class DotReader
{
public:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, DotVertex, DotEdge> graph_t;

	DotReader() = default;

	static void fill_graph(graph_t& empty, const std::string& filename, fcalls_t& func_calls);
};
