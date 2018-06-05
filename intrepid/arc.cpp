
#include "arc.h"

//#include "Edge.h"
#include "Network.h"

Arc::Arc()
{
//    cout << "Arc constructor called." << endl;
    id = "LINK";
    arcnum = -1;
    edge   = -1;
    ends.resize(2);
    ends[0] = -1;
    ends[1] = -1;
}

Arc::~Arc()
{
//    cout << "Arc destructor called." << endl;
}

void Arc::setArc(int arc, string prefix, int edge, int node, int node2) {
    this->id += to_string(edge) + prefix; // C++11
    this->arcnum = arc;
    this->edge = edge;
    this->ends[0] = node;
    this->ends[1] = node2;

    if (Network::edges[edge].arcs[0] == -1) {
		Network::edges[edge].arcs[0] = this->arcnum;
    }
    else if (Network::edges[edge].arcs[1] == -1) {
		Network::edges[edge].arcs[1] = this->arcnum;
    }
    else {
        cout << "Arc_add() failed. The edge already had two arc pointers." << endl;
        return;
    } /* endif */
	Network::nodes[node].adj_arcs.push_back(this->arcnum);
	Network::nodes[node2].inadj_arcs.push_back(this->arcnum);
}

void Arc::arc_delete()
{
    this->id      = "LINK";
    this->arcnum  = -1;
    this->edge    = -1;
    this->ends[0] = -1;
    this->ends[1] = -1;
}
