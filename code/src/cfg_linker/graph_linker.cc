#include "graph_linker.hh"

#include <exception>

typedef DotReader::graph_t graph_t;
typedef std::vector<std::pair<std::unique_ptr<graph_t>, std::string>> graphs_t;
typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
typedef boost::graph_traits<graph_t>::vertex_iterator vertex_iter;
typedef std::vector<std::pair<std::string, std::pair<vertex_t, std::optional<vertex_t>>>> call_stack_t;

static void set_func_name_as_prpty(graph_t& graph, std::string& func_name)
{
    // Accessing vertexes
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(graph); vp.first != vp.second; ++vp.first)
        graph[*vp.first].func_name = func_name;
}

GraphLinker::GraphLinker(std::vector<std::string> filenames)
{
    // Creating vector of graph with their associated func name
    for (auto i = 0u; i < filenames.size(); ++i)
    {
        auto g = std::make_shared<graph_t>();
        DotReader::fill_graph(*g, filenames[i], function_calls);

        std::string func_name = filenames[i];
        // TODO Make this more reliable
        func_name = func_name.substr(7, func_name.size() - 11);
        std::cout << func_name << '\n';

        set_func_name_as_prpty(*g, func_name);

        auto p = std::pair<std::shared_ptr<graph_t>, std::string>(g, func_name);
        graphs.push_back(p);
    }

    // Looking for main function
    for (auto i = 0u; i < graphs.size(); ++i)
    {
        if (graphs[i].second == "main")
        {
            main = graphs[i].first;
            (*main)[0].is_program_entrance = true;
            return;
        }
    }

    throw new std::invalid_argument("Cannot find .main.dot");
}

// Deprecated
std::string get_func_name(std::string label)
{
    if (label.size() < 6)
        return "";

    auto begin_name = label.find("call ") + 5;
    if (begin_name == label.size() + 5)
        return "";

    std::string res = label.substr(begin_name);

    // Getting end of function call
    auto find_end = res.find(" ");
    if (find_end == std::string::npos)
        find_end = res.size();

    res = res.substr(0, find_end);

    return res;
}

graph_t *GraphLinker::find_graph(const std::string& func_name)
{
    for (auto p : graphs)
    {
        if (p.second == func_name)
            return p.first.get();
    }

    return nullptr;
}

std::optional<vertex_t> GraphLinker::get_parent(vertex_t curr)
{
    std::optional<vertex_t> res;
    auto index = boost::get(boost::vertex_index, *main);

    boost::graph_traits<graph_t>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(*main); ei != ei_end; ++ei)
    {
        if (index[target(*ei, *main)] == curr)
        {
            return index[source(*ei, *main)];
        }
    }

    return std::nullopt;
}

std::optional<vertex_t> get_bottom_vertex(const graph_t& g)
{
    auto index = boost::get(boost::vertex_index, g);

    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        boost::graph_traits<graph_t>::vertex_descriptor v = *vp.first;

        auto found = true;
        boost::graph_traits<graph_t>::edge_iterator ei, ei_end;
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

vertex_t get_top_vertex(const graph_t& g)
{
    auto index = boost::get(boost::vertex_index, g);

    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(g); vp.first != vp.second; ++vp.first)
    {
        boost::graph_traits<graph_t>::vertex_descriptor v = *vp.first;

        auto found = true;
        boost::graph_traits<graph_t>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end && found; ++ei)
        {
            if (index[target(*ei, g)] == v)
                found = false;
        }

        if (found)
            return v;
    }

    throw new std::invalid_argument("Graph has no top");
}

void GraphLinker::copy_graph_into(const graph_t& other)
{
    auto offset = boost::num_vertices(*main);

    // Copying vertexes
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(other); vp.first != vp.second; ++vp.first)
    {
        DotVertex vertex = DotVertex(other[*vp.first]);
        auto new_v = boost::add_vertex(*main);
        (*main)[new_v] = vertex;
        function_calls.emplace_back(*vertex.function_calls);
        (*main)[new_v].function_calls = &(*(--function_calls.end()));
    }

    // Copying edges
    boost::graph_traits<graph_t>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(other); ei != ei_end; ++ei)
    {
        DotEdge edge = other[*ei];
        boost::add_edge(source(*ei, other) + offset, target(*ei, other) + offset, edge, *main);
    }
}

std::pair<vertex_t, std::optional<vertex_t>>
GraphLinker::insert_graph_between(const graph_t& to_insert, vertex_t begin, vertex_t end, std::string func_name)
{
    std::cout << "Inserting " << func_name << " ...\n"; 
    // Getting begin and end of the graph to be inserted
    auto sub_begin = get_top_vertex(to_insert);
    auto sub_end = get_bottom_vertex(to_insert);
    auto old_len = boost::num_vertices(*main);

    copy_graph_into(to_insert);

    // Connecting the two graphs
    DotEdge edge_in;
    edge_in.label = func_name;
    DotEdge edge_out;
    edge_out.label = "end " + func_name;

    boost::add_edge(begin, old_len + sub_begin, edge_in, *main);
    if (sub_end) // If sub_end does not exist it means sub function has infinite loop but that is ok
        boost::add_edge(old_len + *sub_end, end, edge_out, *main);

    std::cout << func_name << " got successfully inserted.\n";

    if (sub_end)
        return std::pair<vertex_t, std::optional<vertex_t>>(old_len + sub_begin, old_len + *sub_end);
    else
        return std::pair<vertex_t, std::optional<vertex_t>>(old_len + sub_begin, std::nullopt);
}

static vertex_t create_new_parent(graph_t& graph, vertex_t v)
{
    auto new_v = boost::add_vertex(graph);
    boost::add_edge(new_v, v, graph);

    if (graph[v].is_program_entrance)
    {
        graph[v].is_program_entrance = false;
        graph[new_v].is_program_entrance = true;
    }

    graph[new_v].func_name = graph[v].func_name;
    return new_v;
}

std::optional<std::pair<vertex_t, std::optional<vertex_t>>>
GraphLinker::insert_func(const std::string& func_name, vertex_t v)
{
    // Getting function graph
    auto* func_graph = find_graph(func_name);
    if (func_graph == nullptr)
    {
        std::cerr << "Warning: no graph found for function '"
            << func_name 
            << "', ignoring insertion.\n";

        return std::nullopt;
    }

    // Getting parent of calling vertex
    auto parent = get_parent(v);
    if (!parent)
        parent = create_new_parent(*main, v);

    // Inserting func graph between parent and current
    auto res = insert_graph_between(*func_graph, parent.value(), v, func_name);
    // Deleting former edge
    boost::remove_edge(parent.value(), v, *main);

    return res;
}

// DEPRECATED
std::shared_ptr<graph_t> GraphLinker::link()
{
    // ACCESSING VERTEXES
    std::pair<vertex_iter, vertex_iter> vp;
    auto has_inserted = true;
    while (has_inserted)
    {
        has_inserted = false;
        for (vp = boost::vertices(*main); vp.first != vp.second; ++vp.first)
        {
            // Getting vertex label
            auto& v = (*main)[*vp.first];

            // Inserting called functions
            for (auto i = 0u; i < v.function_calls->size(); ++i)
            {
                auto& func = (*v.function_calls)[i];

                if (insert_func(func, *vp.first))
                {
                    // Resetting call
                    v.function_calls->erase(v.function_calls->begin() + i);
                    has_inserted = true;
                }
            }
        }
    }

    return main;
}

std::shared_ptr<graph_t> GraphLinker::link_sub(unsigned int max_depth)
{
    // ACCESSING VERTEXES
    std::pair<vertex_iter, vertex_iter> vp;
    auto has_inserted = true;
    for (auto curr_depth = 0u; curr_depth < max_depth && has_inserted; ++curr_depth)
    {
        has_inserted = false;
        // TODO Set vp.first value to the old value of vp.second for optimisation
        for (vp = boost::vertices(*main); vp.first != vp.second; ++vp.first)
        {
            auto& v = (*main)[*vp.first];
            auto& calls = *v.function_calls;

            if (!v.function_calls)
                continue;

            // Inserting called functions
            for (auto i = 0u; i < calls.size(); ++i)
            {
                auto& func = calls[i];
                if (insert_func(func, *vp.first))
                {
                    // Resetting call
                    calls.erase(calls.begin() + i);
                    has_inserted = true;
                }
            }
        }
    }

    return main;
}

std::optional<std::pair<vertex_t, std::optional<vertex_t>>>
get_func_from_callstack(const std::string& func_name, const call_stack_t& call_stack)
{
    for (auto obj : call_stack)
    {
        if (obj.first == func_name)
            return obj.second;
    }

    return std::nullopt;
}

std::shared_ptr<graph_t> GraphLinker::link_over()
{
    call_stack_t call_stack;

    if (!boost::num_vertices(*main))
        return main;
    
    link_over_rec(0, call_stack);
    return main;
}

void GraphLinker::link_over_rec(vertex_t from, call_stack_t& call_stack)
{
    // ACCESSING VERTEXES
    std::pair<vertex_iter, vertex_iter> vp;
    (void) from;

    for (vp = boost::vertices(*main); vp.first != vp.second; ++vp.first) {
        // Getting vertex label
        vertex_t vt = *vp.first;
        if (vt < from)
            continue;

        auto v = (*main)[vt];

        // Inserting each functions
        for (auto i = 0u; i < v.function_calls->size(); ++i)
        {
            // Debug print
            /*
            std::cout << "Call stack: ";
            for (auto& e : call_stack)
               std::cout << '(' << e.first << '(' << e.second.first << ',' << e.second.second << ")) ";
            std::cout << '\n';
            */
            auto& func = (*v.function_calls)[i];

            // Looking for function in parents
            auto index = get_func_from_callstack(func, call_stack);
            if (index)
            {
                DotEdge edge_top;
                DotEdge edge_bot;
                edge_top.label = func;
                edge_bot.label = "end " + func;

                boost::add_edge(vt, index.value().first, edge_top, *main);
                if (index.value().second) // Case where subfunc is is not infinite loop
                    boost::add_edge(*index.value().second, vt, edge_bot, *main);
                
                v.function_calls->erase(v.function_calls->begin() + i);
                continue;
            }

            // if not found then inserting called function
            auto insert = insert_func(func, vt);
            if (insert)
            {
                v = (*main)[vt];
                
                // Recursive call
                call_stack.push_back(std::pair(func, insert.value()));
                link_over_rec(insert.value().first, call_stack);
                call_stack.pop_back();
                
                // Removing call
                v.function_calls->erase(v.function_calls->begin() + i);
            }
        }
    }
}
