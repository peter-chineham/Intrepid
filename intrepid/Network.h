#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <vector>

#include "arc.h"
#include "Edge.h"
#include "equipment.h"
#include "linetype.h"
#include "Node.h"
#include "Parm.h"
#include "Requirements.h"

using namespace std;

class Network
{
public:
	static string id;                  /*  Network ID               */
	static int    version_number;      /*  RC021994 We are going crazy otherwise */
	static char   coord_type;          /*  V = V&H ; L = LAT/LNG    */
	static vector<Node> nodes;         /*  Node list                */
	static vector<Edge> edges;		   /*  Edge list                */
	static vector<Arc> arcs;		   /*  Arc list                 */
	static vector<Req> reqs;		   /*  Requirements list        */
    static vector<vector<float> > Lreq; /*  Line Requirements 2D   */
	static vector<Parm> parms;         /*  Parameters               */
	static vector<Equipment> devtypes; /*  Device type struct.      */
	static vector<Linetype> linetypes; /*  Line type struct.        */
	static Qt::GlobalColor edge_colours[12]; // array of colours for line type
	static int default_link_type;      /*  Default line type        */

	Network();
	~Network();

	static void addNode(string name, string ntypei, string vcord, string hcord,
		string latitude, string longitude, string idd, string local_attach,
		string service, string parent);
	static int findNode(string node);
	static void makeWorld();
	static int addEdge(int node, int node2);
	static int addArc(string prefix, int edge, int node, int node2);
	static bool defineLreq(int nn);
	static bool addReq(string reqid, string source, string dest, string msg_bytes, string msg_bits,
		string msg_rate, string bandwidth);
    static int getReq(int n1, int n2);
	};
#endif // NETWORK_H
