#include <iostream>
#include <fstream>
#include <string>

#include "dot_reader.hh"
    
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, DotVertex, DotEdge> graph_t;
typedef boost::graph_traits<graph_t>::vertex_iterator vertex_iter;

static void remove_curly_braces(graph_t& graph)
{
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(graph); vp.first != vp.second; ++vp.first)
    {
        auto& label = graph[*vp.first].label;

        if (label.empty())
            continue;

        if (label[0] == '{' && label[label.size() - 1] == '}')
            label = label.substr(1, label.size() - 2);
    }
}

// That is where we finish parsing the parenthesis part that sed cannot handle
static void fill_function_calls(DotVertex& vertex, fcalls_t& func_calls)
{
    auto i = 0u;
    const auto& label = vertex.label;

    func_calls.emplace_back(std::vector<std::string>());
    auto& calls = *(--func_calls.end());
    vertex.function_calls = &(*(--func_calls.end()));

    if (label.size() < 4)
        return;

    // FIXME Add recursive call to manage calls between parenthesis
    while (i < label.size() - 3)
    {
        if (label.substr(i, 4) != "call")
        {
            ++i;
            continue;
        } 

        i += 4;
        while (label[i] == ' ')
            ++i;

        auto begin_func_name = i;
        while (label[i] != '(')
            ++i;

        std::string func_name = label.substr(begin_func_name, i - begin_func_name);
        calls.emplace_back(func_name);

        if (i >= label.size() - 1)
            return;

        // Skipping parenthesis
        auto count = 1u;
        ++i;
        while (count)
        {
            auto c = label[i];
            if (c == '(')
                ++count;
            else if (c == ')')
                --count;
            ++i;
        }
    }
}

static void fill_all_function_calls(graph_t& graph, fcalls_t& func_calls)
{
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(graph); vp.first != vp.second; ++vp.first)
    {
        fill_function_calls(graph[*vp.first], func_calls);
        // We don't need the label anymore
        graph[*vp.first].label = "";
    }
}

void DotReader::fill_graph(graph_t& graph, const std::string& filename, fcalls_t& func_calls)
{
    boost::dynamic_properties dp(boost::ignore_other_properties);

    dp.property("node_id", boost::get(&DotVertex::name,  graph));
    dp.property("label",   boost::get(&DotVertex::label, graph));

    auto attr_map = boost::get(&DotVertex::Attribute, graph);

    for (int i = 0; i < SIZE; ++i)
        dp.property("Attribute" + std::to_string(i),
                    boost::make_transform_value_property_map(arg1[i], attr_map));

    // .DOT FILE IMPORTATION
    try
    {
        std::ifstream dot(filename);
        boost::read_graphviz(dot, graph, dp);
        dot.close();
    }
    catch (std::logic_error& e)
    {
        std::cerr << "Error while creating cfg from file: " << e.what() << '\n';
        exit(1);
    }

    remove_curly_braces(graph);
    fill_all_function_calls(graph, func_calls);
}
