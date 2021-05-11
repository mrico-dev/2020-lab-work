#pragma once

#include <iostream>

template <class Graph>
class DotEdgeWriter
{
public:

    DotEdgeWriter(Graph graph) : graph_(graph)
    {}

    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const
    {
        out << "[label=\"" << graph_[v].label << "\"]";
    }


private:
     Graph graph_;
};
