#include "cfg_linker/dot_reader.hh"
#include "cfg_linker/graph_linker.hh"
#include "cfg_linker/edge_writer.hh"
#include "cfg_linker/vertex_writer.hh"
#include "cfg_linker/cfg_utility.hh"

#include "automata_check/automata_print.hh"
#include "automata_check/formula_check.hh"
#include "automata_check/automata_builder.hh"

#include <spot/twaalgos/hoa.hh>

#define MAX_DEPTH 5

std::vector<std::string> get_filenames(int argc, char **argv)
{
    std::vector<std::string> res;
    for (auto i = 1; i < argc; ++i)
        res.push_back(argv[i]);

    return res;
}

typedef DotReader::graph_t graph_t;

void print_graph_info(graph_t& graph)
{
    std::cout << "Printing info about the graph:\n";
    // ACCESSING VERTEXES
    typedef boost::graph_traits<graph_t>::vertex_iterator vertex_iter;
    typedef boost::property_map<graph_t, boost::vertex_index_t>::type IndexMap;
    IndexMap index = boost::get(boost::vertex_index, graph);

    std::cout << "vertices(g) = ";
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(graph); vp.first != vp.second; ++vp.first)
    {
        boost::graph_traits<graph_t>::vertex_descriptor v = *vp.first;
        std::cout << index[v]
                  << ":"
                  << (graph)[v].name
                  << ":"
                  << (graph)[v].label
                  << ", ";
    }
    std::cout << std::endl;

    // ACCESSING EDGES
    std::cout << "edges(g) = ";
    boost::graph_traits<graph_t>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei)
        std::cout << "(" << index[source(*ei, graph)]
                  << "," << index[target(*ei, graph)] << ") ";
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: ./exe LTL_formula files.dot ...\n";
        return 1;
    }

    auto formula = std::string(argv[1]);

    // Getting graph
    std::cout << "Getting graph from files...\n";
    auto filenames = get_filenames(argc - 1, argv + 1);
    GraphLinker gl_sub(filenames);
    GraphLinker gl_over(filenames);

    // Linking graph (3 possible options)
    graph_t graph_sub;
    graph_t graph_over;
    try
    {
        std::cout << "Done.\n" << "Linking sub graphs...\n";
        graph_sub = *gl_sub.link_sub(MAX_DEPTH);
        std::cout << "Done.\n" << "Linking over graphs...\n";
        graph_over = *gl_over.link_over();
        std::cout << "Done.\n" << "Putting calls on edges graphs for sub...\n";
        put_calls_on_edges(graph_sub);
        std::cout << "Done.\n" << "Putting calls on edges graphs for over...\n";
        put_calls_on_edges(graph_over);
    }
    catch (std::exception* e)
    {
        std::cerr << e->what();
        return 1;
    }
    std :: cout << "Done.\n";

    print_graph_info(graph_sub);
    print_graph_info(graph_over);

    std::cout << "Exporting result graph as .dot file.\n";
    std::ofstream file_sub;
    std::ofstream file_over;
    file_sub.open("res_sub.dot");
    file_over.open("res_over.dot");

    auto ewriter_sub = DotEdgeWriter(graph_sub);
    auto vwriter_sub = DotVertexWriter(graph_sub);
    auto ewriter_over = DotEdgeWriter(graph_over);
    auto vwriter_over = DotVertexWriter(graph_over);
    boost::write_graphviz(file_sub, graph_sub, vwriter_sub, ewriter_sub); 
    boost::write_graphviz(file_over, graph_over, vwriter_over, ewriter_over); 

    file_sub.close();
    file_over.close();

    auto aut_sub = get_twa_from_cfg(graph_sub);
    auto aut_over = get_twa_from_cfg(graph_over);
    // spot::print_hoa(std::cout, aut_sub);
    // spot::print_hoa(std::cout, aut_over);
    std::cout << '\n';

    if (aut_sub->is_empty())
    {
        std::cout << "Warning! sub automata is empty!\n";
        return 1;
    }
    else if (aut_over->is_empty())
    {
        std::cout << "Warning! over automata is empty!\n";
        return 1;
    }

    // Getting a ,dot file of the spot automata
    std::ofstream aut_sub_file;
    std::ofstream aut_over_file;
    aut_sub_file.open("aut_sub.dot");
    aut_over_file.open("aut_over.dot");
    print_twa_as_dot(aut_sub_file, aut_sub);
    print_twa_as_dot(aut_over_file, aut_over);
    aut_sub_file.close();
    aut_over_file.close();

    try
    {
        int res = verifies_formula_on_models(aut_sub, aut_over, formula);
        std::cout << "RESULT: ";
        if (res == 2)
            std::cout << "Could not conclude due to model imprecision"
               << " (maybe increase the max_depth value?..)";
        else if (res == 1)
            std::cout << "True";
        else if (!res)
            std::cout << "False";
        else if (res < 0)
            std::cout << "Formula needs to be stutter invariant (No use of the X operator)\n";
        else
            std::cout << "Unknown return value '" << res << "'";
        std::cout << std::endl;
    }
    catch (std::logic_error& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
