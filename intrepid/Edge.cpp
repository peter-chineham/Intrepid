
#include "Edge.h"

//#include "arc.h"
#include "Network.h"
#include "linetype.h"
#include "Network.h"
#include "Parm.h"
#include "Utils.h"

void Edge::sizeEdge(int n1, int n2, int &Ltype, int &mult, float load) {
    float best_cost = FLT_MAX;
    int   best_c    = -1;
    int   best_mult = -1;
    float t_cost;
    int   t_mult;
    int   line_cap;
    int   t_cap;
    string::size_type sz;     // alias of size_t

    for (int c = 0; c < Linetype::ncap; c++) {
        line_cap = Linetype::pLinetype[c].speedin * stof(Parm::getParm("UTILIZATION"), &sz);
        t_mult = 0;
        do {
            t_mult++;
            t_cap  = line_cap * t_mult;
        } while (t_cap < load);
        t_cost = Utils::line_cost(c, n1, n2) * t_mult;
        if (t_cost < best_cost) {
            best_cost = t_cost;
            best_c = c;
            best_mult = t_mult;
        }
    }
    Ltype = best_c;
    mult  = best_mult;
}

Edge::Edge()
{
//    cout << "Edge constructor called." << endl;
    id = "LINK";
    edgenum = -1;
    ends.resize(2);
    ends[0] = -1;
    ends[1] = -1;
    arcs.resize(2);
    arcs[0] = -1;
    arcs[1] = -1;

    dist   = -1;
    cost   = -1;
    type   = -1;
    mult   = -1;
    length = -1;
    hops   = -1;
    cap    = -1;
	this->Lcolor = Network::edge_colours[0];
	this->Ltree = false;
    load   = -1;
}

Edge::~Edge()
{
//    cout << "Edge destructor called." << endl;
}

void Edge::setEdge(int id, int node, int node2) {
    this->id += to_string(id); // C++11
    this->edgenum = id;
    this->ends[0] = node;
    this->ends[1] = node2;
    Network::nodes[node].adj_edges.push_back(this->edgenum);
	Network::nodes[node2].adj_edges.push_back(this->edgenum);
    char coord_type = Parm::getParm("LLVH")[0];
    this->dist = Utils::coordinates_to_distance(node, node2, coord_type);
}

void Edge::edge_delete()
{
    this->id = "LINK";
    this->edgenum = -1;
    this->ends[0] = -1;
    this->ends[1] = -1;
    this->arcs[0] = -1;
    this->arcs[1] = -1;

    this->dist = -1;
    this->cost   = -1;
    this->type   = -1;
    this->mult   = -1;
    this->length = -1;
    this->hops   = -1;
    this->cap    = -1;
}
