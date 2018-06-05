#ifndef NODE_H_
#define NODE_H_

#include <iostream>
#include <vector>
#include <QPainter>

using namespace std;

class Node {
public:
    // immutable (or input) properties
    int    nodenum;			// Node number
    string name;			// Node name
    char   ntypei;			// Initial node type
    int    vcord;			// V Coordinate
    int    hcord;			// H Coordinate
    double latitude;		// Latitude
    double longitude;		// Longitude
    int    idd;				// Internation Direct Dial code
    int    local_attach;	// Dunno
    double service;			// Dunno
    string parent;			// Node to which homed

    // design properties
    char   ntyped;			// current node type
    int    pred_node;		// predecessor of each node in tree formed
    int    pred_arc;		// predecessor of each arc in tree formed
    QPoint screen;			// screen coordinates
    float  weight;			// traffic to and from this node in bps
    int    bkbn;			// backbone 1=always, 0=never, -1=maybe
    float  merit;			// used by threshold clustering to deterine backbone nodes
    int    dcent;			// distance to centre
    float  moment;
    double mps;				// messages per second
    vector<int> adj_edges;  // adjacent edges list
    vector<int> adj_arcs;   // adjacent arcs list
    vector<int> inadj_arcs; // dunno
    int    device;			// Equipment type
    int    nadapt_ls;		// Number of LS adapters
    int    nadapt_t1;		// Number of T1 adapters
    int    nadapt_t3;		// Number of T3 adapters
    int    cost;			// Cost of this node
    double utilization;		// node utilisation

    Node();  // constructor
    ~Node(); // destructor

	static float lonToX(string lon);
	static float latToY(string lat);
	void setNode(int nodenum, string name, string ntypei, string vcord, string hcord,
        string latitude, string longitude, string idd, string local_attach,
        string service, string parent);
};
#endif /* NODE_H_ */
