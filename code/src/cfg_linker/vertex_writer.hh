#pragma once

#include <iostream>

template <class Graph>
class DotVertexWriter
{
public:

    DotVertexWriter(Graph graph) : graph_(graph)
    {}

    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const
    {
        out << "[label=\"" 
            << (graph_[v].is_program_entrance ? " Program entrance\\l" : "");

        if (graph_[v].function_calls)
        {
            for (auto& func : *graph_[v].function_calls)
                out << func << "\\l";
        }

        out << "\\rfunction: " 
            << graph_[v].func_name
            << "\"]";
    }


private:
     Graph graph_;
};
